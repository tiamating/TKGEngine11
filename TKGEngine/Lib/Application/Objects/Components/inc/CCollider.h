#pragma once

#include "Components/inc/CBehaviour.h"
#include "Components/interface/ICollider.h"

#include "Systems/inc/Physics_Shape.h"
#include "Systems/inc/TKGEngine_Defined.h"
#include "Systems/inc/Physics_Defined.h"

#include "Utility/inc/myfunc_vector.h"



namespace TKGEngine
{
	/// <summary>
	/// Management Collider parameter.
	/// </summary>
	class Collider
		: public Behaviour
		, public ICollider
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Collider(const Collider&) = delete;
		Collider& operator=(const Collider&) = delete;

		Collider();
		~Collider() override;

		// 位置を設定
		virtual void SetPosition(const VECTOR3& pos) = 0;
		// 回転を設定
		virtual void SetRotation(const Quaternion& rot) = 0;

		// 速度(RigidBody用)
		[[nodiscard]] virtual VECTOR3 GetLinearVelocity() const = 0;


		/// フラグ、パラメータの変更時に、作り直す必要があるため、その関数
		virtual void RecreateCollisionObject() = 0;

		// スタティック
		virtual void SetStatic(const bool is_static) = 0;
		bool IsStatic() const;
		
		// キネマティック
		virtual void SetKinematic(const bool is_kinematic) = 0;
		bool IsKinematic() const override;

		// 衝突検知時に関数を呼ぶかどうか
		inline void SetCallOnFunction(bool do_call);

		// RigidBodyかどうか
		virtual bool IsRigidBody() const = 0;


		// Shape関連
		
		/// <summary>
		/// 形状データを追加する
		/// </summary>
		/// <returns>ShapeごとのID</returns>
		virtual ShapeID AddShape(
			ShapeType shape_type,
			const VECTOR3& translate = VECTOR3::Zero,
			const Quaternion& quat = Quaternion::Identity
		);
		// 形状データの削除
		virtual void RemoveShape(ShapeID id);

		// 位置
		void SetShapeTranslation(ShapeID id, const VECTOR3& translate);
		// 回転
		void SetShapeRotation(ShapeID id, const Quaternion& quat);
		void SetShapeRotation(ShapeID id, const VECTOR3& euler);
		// 半径
		void SetShapeRadius(ShapeID id, float radius);
		// 高さ
		void SetShapeHeight(ShapeID id, float height);
		// 範囲
		void SetShapeExtents(ShapeID id, const VECTOR3& extents);


		// ~Shape関連


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	protected:
		// ==============================================
		// protected methods
		// ==============================================
#ifdef USE_IMGUI
		void OnGUI() override;
		void ShapeGUI();
#endif // USE_IMGUI

	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					cereal::base_class<Behaviour>(this),
					CEREAL_NVP(m_do_call_function),
					CEREAL_NVP(m_shapes),
					CEREAL_NVP(m_is_static),
					CEREAL_NVP(m_is_kinematic)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
		void OnCreate(const std::shared_ptr<Object>& p_self) final;
		void OnDestroy() final;

		// 子形状の形状を更新する
		void UpdateChildShape(ShapeID id);

		// ICollider
		/// <summary>
		/// 衝突状態のチェック時に呼ばれる
		/// </summary>
		/// <param name="other">衝突相手のID</param>
		void AddTriggerHitID(int other) final;
		void AddCollisionHitID(int other) final;

		// 衝突状態の更新
		void UpdateHitState() final;
		void UpdateTriggerList();
		void UpdateCollisionList();

		// 衝突時の関数を呼ぶかどうか
		bool IsCallOnFunction() const final;

		// GameObjectIDの取得
		int GetGameObjectID() const final;

		// ComponentIDの取得
		int GetColliderID() const final;
		// ~ICollider

		// ==============================================
		// private variables
		// ==============================================

		// OnCollision, OnTriggerを呼ぶか
		bool m_do_call_function = true;

		// 衝突状態を保存するIDリスト
		std::unordered_set<int> m_prev_trigger_list;
		std::unordered_set<int> m_current_trigger_list;

		std::unordered_set<int> m_prev_collision_list;
		std::unordered_set<int> m_current_collision_list;


	protected:
		// ==============================================
		// protected methods
		// ==============================================
		virtual void OnCreated(const std::shared_ptr<Object>& p_self) = 0;
		virtual void OnDestroyed() = 0;

		// ShapeIDからshape配列のインデックスを探索する
		int FindShapeDataIndex(const ShapeID id) const;
		// ShapeIDからCompoundShape内のインデックスを探索する
		int FindShapeFromCompound(const ShapeID id) const;

		// Compound ShapeのAABBの再計算
		void RecalculateShapeAABB();

		// レイヤーグループとレイヤーマスクの設定を作成する
		virtual void CreateLayerGroupAndMask(int& group, int& mask) = 0;

		// ICollider
		bool IsTrigger() const override = 0;
		// Collisionのアクティブを変化させるときに呼ばれる
		void OnSetCollisionActive(bool is_active) override = 0;
		// Colliderが有効かどうか
		bool IsActiveCollider() final;
		// ~ICollider

		// Compound Shapeの慣性、質量、AABBの再計算
		virtual void RecalculateBody() = 0;

		// ==============================================
		// protected variables
		// ==============================================

		// 複合形状
		std::unique_ptr<btCompoundShape> m_compound_shape = nullptr;
		// 形状データ
		std::vector<std::unique_ptr<ShapeBase>> m_shapes;
		//// 形状スケール
		//VECTOR3 m_scale = VECTOR3::One;

		// スタティック
		bool m_is_static = false;

		// キネマティック
		bool m_is_kinematic = false;

	};

	// --------------------------------------------------------------
	// inline
	// --------------------------------------------------------------
	inline void Collider::SetCallOnFunction(bool do_call)
	{
		m_do_call_function = do_call;
	}

	inline bool Collider::IsCallOnFunction() const
	{
		return m_do_call_function;
	}
	inline bool Collider::IsActiveCollider()
	{
		return IsActiveBehaviour();
	}

}// namespace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::Collider, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::Collider, "TKGEngine::Collider")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Behaviour, TKGEngine::Collider)