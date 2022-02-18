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


		// �ʒu��ݒ�
		virtual void SetPosition(const VECTOR3& pos) override;
		// ��]��ݒ�
		virtual void SetRotation(const Quaternion& rot) override;

		/// �t���O�A�p�����[�^�̕ύX���ɁA��蒼���K�v�����邽�߁A���̊֐�
		virtual void RecreateCollisionObject() override;

		// �d��
		void UseGravity(const bool use_gravity);
		bool UseGravity() const;

		// �X�^�e�B�b�N
		virtual void SetStatic(const bool is_static) override;

		// �L�l�}�e�B�b�N
		virtual void SetKinematic(const bool is_kinematic) override;

		// RigidBody���ǂ���
		bool IsRigidBody() const override
		{
			return true;
		}

		// �X�V�̗L����
		void Activate();

		// �����̍X�V��Ԃ�ݒ�
		void ForceDisableDeactivation(bool force);


		// Shape�֘A

		/// <summary>
		/// �`��f�[�^��ǉ�����
		/// </summary>
		/// <returns>Shape���Ƃ�ID</returns>
		ShapeID AddShape(
			ShapeType shape_type,
			const VECTOR3& translate = VECTOR3::Zero,
			const Quaternion& quat = Quaternion::Identity
		) override;
		// �`��f�[�^�̍폜
		void RemoveShape(ShapeID id) override;

		///// <summary>
		///// �`��f�[�^�̕ύX
		///// </summary>
		///// <returns>�V����ID</returns>
		//virtual ShapeID ChangeShape(ShapeID id, ShapeType shape_type);

		// ����
		void SetMass(ShapeID id, float mass);

		// ~Shape�֘A

		// �͂�������
		void AddForce(const VECTOR3& force);
		// ���[�J�����W�ɗ͂�������
		void AddForce(const VECTOR3& force, const VECTOR3& position);
		// �Ռ���������
		void AddImpulse(const VECTOR3& impulse);
		// ���[�J�����W�ɏՌ���������
		void AddImpulse(const VECTOR3& impulse, const VECTOR3& position);
		// �Ռ���������
		void AddPushImpulse(const VECTOR3& impulse);
		// ���[�J�����W�ɏՌ���������
		void AddPushImpulse(const VECTOR3& impulse, const VECTOR3& position);
		// �g���N��������
		void AddTorque(const VECTOR3& torque);
		// �g���N�̏Ռ���������
		void AddTorqueImpulse(const VECTOR3& torque_imp);
		// �g���N�̏Ռ���������
		void AddTorqueTurnImpulse(const VECTOR3& torque_imp);
		// �͂����Z�b�g
		void ResetForces();


		// �d��
		inline void ApplyWorldGravity(bool apply);
		void SetGravity(const VECTOR3& gravity);
		inline VECTOR3 GetGravity() const;

		// ���x
		void SetLinearVelocity(const VECTOR3& velocity);
		virtual VECTOR3 GetLinearVelocity() const override;
		// �p���x
		void SetAngularVelocity(const VECTOR3& velocity);
		VECTOR3 GetAngularVelocity() const;
		// �^�[�����x
		void SetTurnVelocity(const VECTOR3& velocity);
		VECTOR3 GetTurnVelocity() const;

		// ���x�����x(0 : �F�����)
		void SetLinearDamping(float damp);
		inline float GetLinearDamping() const;
		// �p���x�����x(0 : �F�����)
		void SetAngularDamping(float damp);
		inline float GetAngularDamping() const;

		// �����W��
		void SetRestitution(float restitution);
		inline float GetRestitution() const;

		// ���C
		void SetLinearFriction(float friction);
		inline float GetLinearFriction() const;
		void SetRollingFriction(float friction);
		inline float GetRollingFriction() const;
		void SetSpinningFriction(float friction);
		inline float GetSpinningFriction() const;

		// �ړ��A��]�̍S��
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
		/// ���[�V�����X�e�[�g
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
		// Trigger���ǂ���
		virtual bool IsTrigger() const override
		{
			return false;
		}

		// Collision�̃A�N�e�B�u��ω�������Ƃ��ɌĂ΂��
		virtual void OnSetCollisionActive(bool is_active) override;
		// ~ICollider

		// RigidBody�����
		void CreateRigidBody();

		// Compound Shape�̊����A���ʁAAABB�̍Čv�Z
		void RecalculateBody() override;

		// Gravity�̐ݒ������
		void ApplyGravitySetting();

		// �����ʂ��擾
		float GetTotalMass() const;

		// �����̍X�V��Ԃ�ݒ�
		void SetActivation();

		// ���C���[�O���[�v�ƃ��C���[�}�X�N�̐ݒ���쐬����
		virtual void CreateLayerGroupAndMask(int& group, int& mask) override;

		// ==============================================
		// private variables
		// ==============================================
		// ���W�b�h�{�f�B
		std::unique_ptr<btRigidBody> m_rigidbody = nullptr;
		// ���[�V�����X�e�[�g
		std::unique_ptr<ColliderMotionState> m_motion_state = nullptr;

		// �X�V�̖��������~����
		bool m_disable_deactivation = false;

		// ��~���Ă���Ɣ��f���鑬�x
		float m_linear_sleeping_threshold = 0.6f;
		// ��~���Ă���Ɣ��f����p���x
		float m_angular_sleeping_threshold = 0.3f;

		// �d�͂�K�p���邩
		bool m_use_gravity = true;
		// ���[���h�̏d�͂�K�p���邩
		bool m_applied_world_gravity = true;
		// �d��
		VECTOR3 m_gravity = VECTOR3(0.f, -9.8f, 0.f);

		// ������
		float m_total_mass = 0.0f;

		// ����
		VECTOR3 m_inertia = VECTOR3::Zero;
		// �d�S
		VECTOR3 m_principal = VECTOR3::Zero;

		// ���x�����x(0 : �F�����)
		float m_linear_damp = 0.0f;
		// �p���x�����x(0 : �F�����)
		float m_angular_damp = 0.0f;
		// �����W��
		float m_restitution = 0.0f;
		// ���C
		float m_friction = 0.5f;
		// ��]���C
		float m_rolling_friction = 0.0f;
		// ����]���C
		float m_spinning_friction = 0.0f;

		// �ړ��A��]�̍S��
		bool m_freeze_position_x = false;
		bool m_freeze_position_y = false;
		bool m_freeze_position_z = false;
		bool m_freeze_rotation_x = false;
		bool m_freeze_rotation_y = false;
		bool m_freeze_rotation_z = false;

#ifdef USE_IMGUI
		// �M�Y���ɂ���đ��삳�ꂽ��̏��
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