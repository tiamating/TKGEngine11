#pragma once

#include "../Component.h"

#include "Application/Objects/inc/IGameObject.h"
#include "Utility/inc/myfunc_vector.h"

namespace TKGEngine
{
	/// <summary>
	/// Management object parameter, pos, rot, scale.
	/// </summary>
	class Transform
		: public Component
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Transform(const Transform&) = delete;
		Transform& operator=(const Transform&) = delete;

		Transform();
		virtual ~Transform() override;

		std::shared_ptr<IGameObject> GetGameObject() override;

		/// <summary>
		/// 自身のヒエラルキーからオブジェクトを探索する
		/// </summary>
		std::shared_ptr<Transform> Find(const std::string& obj_name) const;

#ifdef USE_IMGUI
		void OnGUI() override;
#endif // USE_IMGUI

		void Translate(const VECTOR3& translation, bool self_space = true);
		void Translate(float x, float y, float z, bool self_space = true);

		void Rotate(const Quaternion& quat, bool self_space = true);
		void Rotate(const VECTOR3& eulers, bool self_space = true);
		void Rotate(float x_angle, float y_angle, float z_angle, bool self_space = true);
		void Rotate(const VECTOR3& axis, float angle, bool self_space = true);

		void LookAt(const std::shared_ptr<Transform>& target, const VECTOR3& world_up = VECTOR3::Up);
		void LookAt(const VECTOR3& point, const VECTOR3& world_up = VECTOR3::Up);


		// Position
		[[nodiscard]] VECTOR3 Position() const;
		void Position(const VECTOR3& pos);
		inline void Position(float x, float y, float z);
		[[nodiscard]] inline const VECTOR3& LocalPosition() const;
		inline void LocalPosition(const VECTOR3& pos);
		inline void LocalPosition(float x, float y, float z);
		// ~Position

		// Rotation
		[[nodiscard]] Quaternion Rotation() const;
		void Rotation(const Quaternion& quat);
		[[nodiscard]] inline const Quaternion& LocalRotation() const;
		inline void LocalRotation(const Quaternion& quat);
		[[nodiscard]] VECTOR3 EulerAngles() const;
		void EulerAngles(const VECTOR3& angles);
		inline void EulerAngles(float x, float y, float z);
		[[nodiscard]] inline const VECTOR3& LocalEulerAngles() const;
		inline void LocalEulerAngles(const VECTOR3& angles);
		inline void LocalEulerAngles(float x, float y, float z);
		// ~Rotation

		// Scale
		[[nodiscard]] VECTOR3 LossyScale() const;
		[[nodiscard]] inline const VECTOR3& LocalScale() const;
		inline void LocalScale(const VECTOR3& scale);
		inline void LocalScale(float x, float y, float z);
		// ~Scale

		// Axis
		// In world space
		[[nodiscard]] VECTOR3 Right() const;
		void Right(const VECTOR3& right);
		[[nodiscard]] VECTOR3 Up() const;
		void Up(const VECTOR3& up);
		[[nodiscard]] VECTOR3 Forward() const;
		void Forward(const VECTOR3& forward);
		// ~Axis

		inline std::shared_ptr<Transform> GetParent();

		// ヒエラルキーのルートオブジェクトにする
		void SetParent(bool keep_world_pos = true);
		// 親子関係をつける
		void SetParent(const std::shared_ptr<Transform>& parent, bool keep_world_pos = true);	// Set parent.

		/// <summary>
		/// Find from self children. When same name, can not tell the difference.
		/// </summary>
		/// <returns>Found object or null.</returns>
		std::weak_ptr<Transform> FindFromChild(const std::string& name);
		std::shared_ptr<Transform> GetChild(int index);
		[[nodiscard]] int GetChildCount() const;

		const MATRIX& GetAffineTransform();

		MATRIX GetLocalToWorldMatrix();
		MATRIX GetWorldToLocalMatrix();

		// 親からの相対変換行列をセットする
		void SetRelativeMatrix(const MATRIX& m);

		// ワールド行列でパラメータをセットする
		void SetWorldMatrix(const MATRIX& m);

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		friend class cereal::access;
		template <class Archive>
		void load(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				// Transformのメンバ読み込み
				archive(
					cereal::base_class<Component>(this),
					CEREAL_NVP(m_local_position),
					CEREAL_NVP(m_local_rotation),
					CEREAL_NVP(m_local_euler_angle),
					CEREAL_NVP(m_local_scale),
					CEREAL_NVP(m_child_count),
					CEREAL_NVP(m_children),
					CEREAL_NVP(m_gameObject)
				);
				// m_gameObjectの登録をサポートする
				// 自身をManagerに登録する
				OnDeserialize();

				// Component数
				int component_cnt;
				const GameObjectID owner_id = GetOwnerID();
				archive(cereal::make_nvp("Component Count", component_cnt));
				// Componentの読み込み
				for (int i = 0; i < component_cnt; ++i)
				{
					std::shared_ptr<Component> component;
					archive(component);
					ComponentManager::AddComponentOnSerialize(owner_id, component);
				}
			}
		}
		template <class Archive>
		void save(Archive& archive, const std::uint32_t version) const
		{
			//if (version > 0)
			{
				// Transformのメンバ書き出し
				archive(
					cereal::base_class<Component>(this),
					CEREAL_NVP(m_local_position),
					CEREAL_NVP(m_local_rotation),
					CEREAL_NVP(m_local_euler_angle),
					CEREAL_NVP(m_local_scale),
					CEREAL_NVP(m_child_count),
					CEREAL_NVP(m_children),
					CEREAL_NVP(m_gameObject)
				);
			}

			// Component書き出し
			const GameObjectID owner_id = GetOwnerID();
			const auto components = ComponentManager::GetComponents(owner_id);
			// Transform以外のコンポーネントの数を保存しておく
			static const std::string transform_type_name("class TKGEngine::Transform");
			archive(cereal::make_nvp("Component Count", static_cast<int>(components.size() - 1)));
			for (auto& component : components)
			{
				auto s_ptr = component.lock();
				if (s_ptr)
				{
					// Transformは書き出さない
					const char* type_name = typeid(*s_ptr.get()).name();
					if (type_name == transform_type_name)
						continue;
					archive(cereal::make_nvp(type_name, s_ptr));
				}
			}
		}

		// ==============================================
		// private methods
		// ==============================================
		void OnCreate(const std::shared_ptr<Object>& p_self) override;
		void OnDestroy() override;

		// Find内で使用するオブジェクト探索の再帰処理
		static std::shared_ptr<Transform> OnFindRecurse(const std::string& obj_name, const std::shared_ptr<Transform>& transform);

		// 平行移動時の処理
		void OnTranslateSetPosition(const VECTOR3& translate);
		// 親情報取得用再帰処理用関数
		void RecurseOnRotation(const std::shared_ptr<Transform>& parent, Quaternion& rot) const;
		void RecurseOnEulerAngles(const std::shared_ptr<Transform>& parent, VECTOR3& angles) const;
		void RecurseOnScale(const std::shared_ptr<Transform>& parent, VECTOR3& scale) const;
		void RecurseOnAxis(const std::shared_ptr<Transform>& parent, Quaternion& rot) const;
		// SetParent時の所属シーン変更用再帰処理
		void RecurseOnSetScene(const std::shared_ptr<Transform>& child, const int scene_id) const;
		// ワールド行列を再帰的に計算する
		void RecurseOnCalculateWorldMatrix(const std::shared_ptr<Transform>& parent, MATRIX& matrix) const;
		// 子オブジェクトの再帰削除
		void RecurseDestroy();
		// 親オブジェクトの子オブジェクトリストからID検索して削除
		void EraseFromParent(InstanceID id);

		// デシリアライズ時にGameObjectの登録をサポートする
		void OnDeserialize();

		// ==============================================
		// private variables
		// ==============================================
		// 値が変更されたらワールド行列を作り直すためのフラグ
		bool m_is_changed = true;
		// 親が破棄されたら、親が子GameObjectの破棄を再帰的にするために使用するフラグ
		bool m_is_destroying = false;
		// 再帰的に破棄が呼ばれた時にルートを判別するフラグ
		bool m_is_root_deleter = false;

		MATRIX m_affine_transform = MATRIX::Identity;

		// Position
		VECTOR3 m_local_position = VECTOR3::Zero;
		// ~Position

		// Rotation
		Quaternion m_local_rotation = Quaternion::Identity;
		VECTOR3 m_local_euler_angle = VECTOR3::Zero;
		// ~Rotation

		// Scale
		VECTOR3 m_local_scale = VECTOR3::One;
		// ~Scale

		// Hierarchy
		int m_child_count = 0;
		std::shared_ptr<Transform> m_parent;
		std::list<std::weak_ptr<Transform>> m_children;
		// ~Hierarchy

		std::weak_ptr<IGameObject> m_gameObject;
	};

	// --------------------------------------------------------------
	// inline
	// --------------------------------------------------------------
	inline std::shared_ptr<IGameObject> Transform::GetGameObject()
	{
		auto s_ptr = m_gameObject.lock();
		return s_ptr == nullptr ? std::shared_ptr<IGameObject>() : s_ptr;
	}

	inline void Transform::Position(const float x, const float y, const float z)
	{
		Position(VECTOR3(x, y, z));
	}

	inline const VECTOR3& Transform::LocalPosition() const
	{
		return m_local_position;
	}

	inline void Transform::LocalPosition(const VECTOR3& pos)
	{
		m_local_position = pos;
		m_is_changed = true;
	}

	inline void Transform::LocalPosition(const float x, const float y, const float z)
	{
		LocalPosition(VECTOR3(x, y, z));
	}

	inline const Quaternion& Transform::LocalRotation() const
	{
		return m_local_rotation;
	}

	inline void Transform::LocalRotation(const Quaternion& quat)
	{
		m_local_rotation = quat;
		m_local_euler_angle = quat.ToEulerAngles();
		m_is_changed = true;
	}

	inline void Transform::EulerAngles(const float x, const float y, const float z)
	{
		EulerAngles(VECTOR3(x, y, z));
	}

	inline const VECTOR3& Transform::LocalEulerAngles() const
	{
		return m_local_euler_angle;
	}

	inline void Transform::LocalEulerAngles(const VECTOR3& angles)
	{
		m_local_euler_angle = angles;
		m_local_rotation = Quaternion::EulerToQuaternion(angles);
		m_is_changed = true;
	}

	inline void Transform::LocalEulerAngles(const float x, const float y, const float z)
	{
		LocalEulerAngles(VECTOR3(x, y, z));
	}

	inline const VECTOR3& Transform::LocalScale() const
	{
		return m_local_scale;
	}

	inline void Transform::LocalScale(const VECTOR3& scale)
	{
		m_local_scale = scale;
		m_is_changed = true;
	}

	inline void Transform::LocalScale(const float x, const float y, const float z)
	{
		LocalScale(VECTOR3(x, y, z));
	}

	inline std::shared_ptr<Transform> Transform::GetParent()
	{
		return m_parent;
	}

}// namespace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::Transform, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::Transform, "TKGEngine::Transform")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Component, TKGEngine::Transform)
// archive時にload saveを使用する曖昧さ回避
CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(TKGEngine::Transform, cereal::specialization::member_load_save)