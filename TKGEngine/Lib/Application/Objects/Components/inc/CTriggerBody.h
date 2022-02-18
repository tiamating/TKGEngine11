#pragma once

#include "CCollider.h"

#include <btBulletDynamicsCommon.h>


class btGhostObject;

namespace TKGEngine
{
	/// <summary>
	/// Trigger Object module.
	/// </summary>
	class TriggerBody
		: public Collider
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		TriggerBody(const TriggerBody&) = delete;
		TriggerBody& operator=(const TriggerBody&) = delete;

		TriggerBody();
		virtual ~TriggerBody();


		// �ʒu��ݒ�
		virtual void SetPosition(const VECTOR3& pos) override;
		// ��]��ݒ�
		virtual void SetRotation(const Quaternion& rot) override;

		// ���x(RigidBody�p)
		virtual VECTOR3 GetLinearVelocity() const override;

		/// �t���O�A�p�����[�^�̕ύX���ɁA��蒼���K�v�����邽�߁A���̊֐�
		virtual void RecreateCollisionObject() override;

		// �X�^�e�B�b�N
		virtual void SetStatic(const bool is_static) override;

		// �L�l�}�e�B�b�N
		virtual void SetKinematic(const bool is_kinematic) override;

		// RigidBody���ǂ���
		virtual bool IsRigidBody() const override
		{
			return false;
		}

		// Shape�֘A

		/// <summary>
		/// �`��f�[�^��ǉ�����
		/// </summary>
		/// <returns>ShapeData�z���ID</returns>
		virtual ShapeID AddShape(
			ShapeType shape_type,
			const VECTOR3& translate = VECTOR3::Zero,
			const Quaternion& quat = Quaternion::Identity
		);
		// �`��f�[�^�̍폜
		virtual void RemoveShape(ShapeID id);

		// ~Shape�֘A



		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					cereal::base_class<Collider>(this)
				);
			}
		}

		// ==============================================
		// private class
		// ==============================================
#ifdef USE_IMGUI
		void OnGUI() override;
		void OnGizmoEnter() override;
		void OnGizmoStay() override;
		void OnGizmoExit() override;
#endif // USE_IMGUI


		/// <summary>
		/// �g���K�[�I�u�W�F�N�g�p�A�N�V�����C���^�[�t�F�[�X
		/// </summary>
		class TriggerAction
			: public btActionInterface
		{
		public:
			TriggerAction(const std::shared_ptr<TKGEngine::Transform>& transform_, btGhostObject* ghost_);
			TriggerAction(const TriggerAction&) = delete;
			TriggerAction(TriggerAction&&) = default;
			virtual ~TriggerAction();

			virtual void updateAction(btCollisionWorld* collisionWorld, btScalar deltaTimeStep);

			virtual void debugDraw(btIDebugDraw* debugDrawer) {}

		private:
			std::shared_ptr<TKGEngine::Transform> transform;
			btGhostObject* ghost;
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
		virtual bool IsTrigger() const
		{
			return true;
		}

		// Collision�̃A�N�e�B�u��ω�������Ƃ��ɌĂ΂��
		virtual void OnSetCollisionActive(bool is_active) override;
		// ~ICollider

		// Compound Shape��AABB�̍Čv�Z
		void RecalculateBody() override;

		// ���C���[�O���[�v�ƃ��C���[�}�X�N�̐ݒ���쐬����
		virtual void CreateLayerGroupAndMask(int& group, int& mask) override;


		// ==============================================
		// private variables
		// ==============================================

		// �S�[�X�g�I�u�W�F�N�g
		std::unique_ptr<btGhostObject> m_ghost = nullptr;

		// �A�N�V�����C���^�[�t�F�[�X
		std::unique_ptr<TriggerAction> m_action = nullptr;
		bool m_set_action = false;
	};

}// ~namespaace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::TriggerBody, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::TriggerBody, "TKGEngine::TriggerBody")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Collider, TKGEngine::TriggerBody)