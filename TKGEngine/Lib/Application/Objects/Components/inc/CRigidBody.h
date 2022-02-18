#pragma once

#include "CCollider.h"

namespace TKGEngine
{
	/// <summary>
	/// Rigid Body module.
	/// </summary>
	class RigidBody
		: public Collider
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		RigidBody(const RigidBody&) = delete;
		RigidBody& operator=(const RigidBody&) = delete;

		RigidBody();
		~RigidBody() override;


		// 位置を設定
		virtual void SetPosition(const VECTOR3& pos) override;
		// 回転を設定
		virtual void SetRotation(const Quaternion& rot) override;

		/// フラグ、パラメータの変更時に、作り直す必要があるため、その関数
		virtual void RecreateCollisionObject() override;

		// 重力
		void UseGravity(const bool use_gravity);
		bool UseGravity() const;

		// スタティック
		virtual void SetStatic(const bool is_static) override;

		// キネマティック
		virtual void SetKinematic(const bool is_kinematic) override;

		// RigidBodyかどうか
		bool IsRigidBody() const override
		{
			return true;
		}

		// 更新の有効化
		void Activate();

		// 物理の更新状態を設定
		void ForceDisableDeactivation(bool force);


		// Shape関連

		/// <summary>
		/// 形状データを追加する
		/// </summary>
		/// <returns>ShapeごとのID</returns>
		ShapeID AddShape(
			ShapeType shape_type,
			const VECTOR3& translate = VECTOR3::Zero,
			const Quaternion& quat = Quaternion::Identity
		) override;
		// 形状データの削除
		void RemoveShape(ShapeID id) override;

		///// <summary>
		///// 形状データの変更
		///// </summary>
		///// <returns>新しいID</returns>
		//virtual ShapeID ChangeShape(ShapeID id, ShapeType shape_type);

		// 質量
		void SetMass(ShapeID id, float mass);

		// ~Shape関連

		// 力を加える
		void AddForce(const VECTOR3& force);
		// ローカル座標に力を加える
		void AddForce(const VECTOR3& force, const VECTOR3& position);
		// 衝撃を加える
		void AddImpulse(const VECTOR3& impulse);
		// ローカル座標に衝撃を加える
		void AddImpulse(const VECTOR3& impulse, const VECTOR3& position);
		// 衝撃を加える
		void AddPushImpulse(const VECTOR3& impulse);
		// ローカル座標に衝撃を加える
		void AddPushImpulse(const VECTOR3& impulse, const VECTOR3& position);
		// トルクを加える
		void AddTorque(const VECTOR3& torque);
		// トルクの衝撃を加える
		void AddTorqueImpulse(const VECTOR3& torque_imp);
		// トルクの衝撃を加える
		void AddTorqueTurnImpulse(const VECTOR3& torque_imp);
		// 力をリセット
		void ResetForces();


		// 重力
		inline void ApplyWorldGravity(bool apply);
		void SetGravity(const VECTOR3& gravity);
		inline VECTOR3 GetGravity() const;

		// 速度
		void SetLinearVelocity(const VECTOR3& velocity);
		virtual VECTOR3 GetLinearVelocity() const override;
		// 角速度
		void SetAngularVelocity(const VECTOR3& velocity);
		VECTOR3 GetAngularVelocity() const;
		// ターン速度
		void SetTurnVelocity(const VECTOR3& velocity);
		VECTOR3 GetTurnVelocity() const;

		// 速度減衰度(0 : 宇宙空間)
		void SetLinearDamping(float damp);
		inline float GetLinearDamping() const;
		// 角速度減衰度(0 : 宇宙空間)
		void SetAngularDamping(float damp);
		inline float GetAngularDamping() const;

		// 反発係数
		void SetRestitution(float restitution);
		inline float GetRestitution() const;

		// 摩擦
		void SetLinearFriction(float friction);
		inline float GetLinearFriction() const;
		void SetRollingFriction(float friction);
		inline float GetRollingFriction() const;
		void SetSpinningFriction(float friction);
		inline float GetSpinningFriction() const;

		// 移動、回転の拘束
		void SetConstraintPosition(bool x, bool y, bool z);
		void SetConstraintRotation(bool x, bool y, bool z);


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			if (version == 2)
			{
				archive(
					cereal::base_class<Collider>(this),
					CEREAL_NVP(m_disable_deactivation),
					CEREAL_NVP(m_linear_sleeping_threshold),
					CEREAL_NVP(m_angular_sleeping_threshold),
					CEREAL_NVP(m_use_gravity),
					CEREAL_NVP(m_applied_world_gravity),
					CEREAL_NVP(m_gravity),
					CEREAL_NVP(m_linear_damp),
					CEREAL_NVP(m_angular_damp),
					CEREAL_NVP(m_restitution),
					CEREAL_NVP(m_friction),
					CEREAL_NVP(m_rolling_friction),
					CEREAL_NVP(m_spinning_friction),
					CEREAL_NVP(m_freeze_position_x),
					CEREAL_NVP(m_freeze_position_y),
					CEREAL_NVP(m_freeze_position_z),
					CEREAL_NVP(m_freeze_rotation_x),
					CEREAL_NVP(m_freeze_rotation_y),
					CEREAL_NVP(m_freeze_rotation_z)
				);
			}
			else if (version == 1)
			{
				archive(
					cereal::base_class<Collider>(this),
					CEREAL_NVP(m_disable_deactivation),
					CEREAL_NVP(m_linear_sleeping_threshold),
					CEREAL_NVP(m_angular_sleeping_threshold),
					CEREAL_NVP(m_use_gravity),
					CEREAL_NVP(m_applied_world_gravity),
					CEREAL_NVP(m_gravity),
					CEREAL_NVP(m_linear_damp),
					CEREAL_NVP(m_angular_damp),
					CEREAL_NVP(m_restitution),
					CEREAL_NVP(m_friction),
					CEREAL_NVP(m_rolling_friction),
					CEREAL_NVP(m_spinning_friction)
				);
			}
		}

		// ==============================================
		// private class
		// ==============================================
#ifdef USE_IMGUI
		void OnGUI() override;
		void OnGizmoEnter() override;
		void OnGizmoExit() override;
#endif // USE_IMGUI

		/// <summary>
		/// モーションステート
		/// </summary>
		class ColliderMotionState
			: public btMotionState
		{
		public:
			ColliderMotionState(
				const std::shared_ptr<TKGEngine::Transform>& transform,
				RigidBody* rigidbody
				);
			virtual ~ColliderMotionState();

			ColliderMotionState(const ColliderMotionState&) = delete;
			ColliderMotionState(ColliderMotionState&&) = default;

			virtual void getWorldTransform(btTransform& world_trans) const override;
			virtual void setWorldTransform(const btTransform& world_trans) override;

		private:
			std::shared_ptr<TKGEngine::Transform> m_transform = nullptr;
			RigidBody* m_rigidbody = nullptr;
		};


		// ==============================================
		// private methods
		// ==============================================
		// Collider
		void OnCreated(const std::shared_ptr<Object>& p_self) override;
		void OnDestroyed() override;
		// ~Collider
		// 
		// Behaviour
		void OnEnabled() override;
		void OnDisabled() override;
		// ~Behaviour

		// ICollider
		// Triggerかどうか
		virtual bool IsTrigger() const override
		{
			return false;
		}

		// Collisionのアクティブを変化させるときに呼ばれる
		virtual void OnSetCollisionActive(bool is_active) override;
		// ~ICollider

		// RigidBodyを作る
		void CreateRigidBody();

		// Compound Shapeの慣性、質量、AABBの再計算
		void RecalculateBody() override;

		// Gravityの設定をする
		void ApplyGravitySetting();

		// 総質量を取得
		float GetTotalMass() const;

		// 物理の更新状態を設定
		void SetActivation();

		// レイヤーグループとレイヤーマスクの設定を作成する
		virtual void CreateLayerGroupAndMask(int& group, int& mask) override;

		// ==============================================
		// private variables
		// ==============================================
		// リジッドボディ
		std::unique_ptr<btRigidBody> m_rigidbody = nullptr;
		// モーションステート
		std::unique_ptr<ColliderMotionState> m_motion_state = nullptr;

		// 更新の無効化を停止する
		bool m_disable_deactivation = false;

		// 停止していると判断する速度
		float m_linear_sleeping_threshold = 0.6f;
		// 停止していると判断する角速度
		float m_angular_sleeping_threshold = 0.3f;

		// 重力を適用するか
		bool m_use_gravity = true;
		// ワールドの重力を適用するか
		bool m_applied_world_gravity = true;
		// 重力
		VECTOR3 m_gravity = VECTOR3(0.f, -9.8f, 0.f);

		// 総質量
		float m_total_mass = 0.0f;

		// 慣性
		VECTOR3 m_inertia = VECTOR3::Zero;
		// 重心
		VECTOR3 m_principal = VECTOR3::Zero;

		// 速度減衰度(0 : 宇宙空間)
		float m_linear_damp = 0.0f;
		// 角速度減衰度(0 : 宇宙空間)
		float m_angular_damp = 0.0f;
		// 反発係数
		float m_restitution = 0.0f;
		// 摩擦
		float m_friction = 0.5f;
		// 回転摩擦
		float m_rolling_friction = 0.0f;
		// 軸回転摩擦
		float m_spinning_friction = 0.0f;

		// 移動、回転の拘束
		bool m_freeze_position_x = false;
		bool m_freeze_position_y = false;
		bool m_freeze_position_z = false;
		bool m_freeze_rotation_x = false;
		bool m_freeze_rotation_y = false;
		bool m_freeze_rotation_z = false;

#ifdef USE_IMGUI
		// ギズモによって操作された後の状態
		bool m_has_controlled_gizmo = false;
#endif // USE_IMGUI
	};


	// --------------------------------------------------------------
	// inline
	// --------------------------------------------------------------
	inline void RigidBody::ApplyWorldGravity(bool apply)
	{
		m_applied_world_gravity = apply;
	}

	inline VECTOR3 RigidBody::GetGravity() const
	{
		return m_gravity;
	}

	inline float RigidBody::GetLinearDamping() const
	{
		return m_linear_damp;
	}

	inline float RigidBody::GetAngularDamping() const
	{
		return m_angular_damp;
	}

	inline float RigidBody::GetRestitution() const
	{
		return m_restitution;
	}

	inline float RigidBody::GetLinearFriction() const
	{
		return m_friction;
	}

	inline float RigidBody::GetRollingFriction() const
	{
		return m_rolling_friction;
	}

	inline float RigidBody::GetSpinningFriction() const
	{
		return m_spinning_friction;
	}

}// ~namespaace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::RigidBody, 2)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::RigidBody, "TKGEngine::RigidBody")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Collider, TKGEngine::RigidBody)