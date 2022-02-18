#include "../inc/CTriggerBody.h"

#include "../inc/CTransform.h"
#include "../../inc/IGameObject.h"

#include "Systems/inc/PhysicsSystem.h"


REGISTERCOMPONENT(TKGEngine::TriggerBody)

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// TriggerBody class Methods
	////////////////////////////////////////////////////////
#pragma region TriggerBody
	TriggerBody::TriggerBody()
	{
		/* nothing */
	}

	TriggerBody::~TriggerBody()
	{
		/* nothing */
	}

#ifdef USE_IMGUI
	void TriggerBody::OnGUI()
	{
		ImGui::PushID(this);

		// Collider::OnGUI
		Collider::OnGUI();
		// Is Static
		if (ImGui::Checkbox("Is Static", &m_is_static))
		{
			SetStatic(m_is_static);
		}
		// Shapes
		ImGui::Separator();
		Collider::ShapeGUI();

		// Trigger����蒼��
		ImVec2 button_size;
		{
			const float width = ImGui::GetWindowWidth();
			constexpr float button_space = 0.2f;
			button_size.x = width * (1.0f - button_space * 2.0f);
			button_size.y = ImGui::DEFAULT_SPACE_SIZE * 1.5f;
			// �{�^���𒆉��ɒu��
			ImGui::SetCursorPosX(width * button_space);
		}
		if (ImGui::Button("Recreate Trigger", button_size))
		{
			RecreateCollisionObject();
		}

		ImGui::PopID();
	}

	void TriggerBody::OnGizmoEnter()
	{

	}

	void TriggerBody::OnGizmoStay()
	{
		// Trigger�̈ʒu�����݂�Transform�ʒu�Ɉړ�����
		const auto transform = GetTransform();
		const btTransform bt_transform(
			ConvertQuaternionTobtQuaternion(transform->Rotation()),
			ConvertVectorTobtVector(transform->Position())
		);
		m_ghost->setWorldTransform(bt_transform);
	}

	void TriggerBody::OnGizmoExit()
	{
		// Trigger�̈ʒu�����݂�Transform�ʒu�Ɉړ�����
		const auto transform = GetTransform();
		const btTransform bt_transform(
			ConvertQuaternionTobtQuaternion(transform->Rotation()),
			ConvertVectorTobtVector(transform->Position())
		);
		m_ghost->setWorldTransform(bt_transform);
	}
#endif // USE_IMGUI

	void TriggerBody::SetPosition(const VECTOR3& pos)
	{
		// �������^�[��
		if (m_ghost == nullptr || IsStatic())
			return;

		// Trigger�̈ʒu�����݂�Transform�ʒu�Ɉړ�����
		m_ghost->getWorldTransform().setOrigin(ConvertVectorTobtVector(pos));
	}

	void TriggerBody::SetRotation(const Quaternion& rot)
	{
		// �������^�[��
		if (!m_ghost || IsStatic())
			return;

		// Trigger�̉�]�����݂�Transform�ʒu�ɕύX����
		m_ghost->getWorldTransform().setRotation(ConvertQuaternionTobtQuaternion(rot));
	}

	VECTOR3 TriggerBody::GetLinearVelocity() const
	{
		LOG_ASSERT("Call GetLinearVelocity() on TriggerBody component. This is RigidBody only.");
		return VECTOR3::Zero;
	}

	void TriggerBody::RecreateCollisionObject()
	{
		// GameObject�ABehaviour����A�N�e�B�u�Ȃ瑁�����^�[��
		if (!IsActiveCollider())
		{
			return;
		}

		// Trigger��World����O��
		PhysicsSystem::GetInstance()->GetWorld()->removeCollisionObject(m_ghost.get());

		// �q�`��̍Đݒ�A����AABB�̍Čv�Z
		RecalculateShapeAABB();

		// ���C���[��ݒ肵�ă��[���h�ɓo�^
		{
			// ���C���[�O���[�v�ƃ}�X�N���C���[�̎擾
			int group = 0;
			int mask = 0;
			CreateLayerGroupAndMask(group, mask);

			// Ghost�̓��C���[�������o�^��Ԃł͕ύX�ł��Ȃ��̂ōēo�^����
			PhysicsSystem::GetInstance()->GetWorld()->addCollisionObject(m_ghost.get(), group, mask);
		}

		// Trigger�̈ʒu�����݂�Transform�ʒu�Ɉړ�����
		{
			const auto transform = GetTransform();
			m_ghost->getWorldTransform().setOrigin(ConvertVectorTobtVector(transform->Position()));
			m_ghost->getWorldTransform().setRotation(ConvertQuaternionTobtQuaternion(transform->Rotation()));
		}
	}

	void TriggerBody::SetStatic(const bool is_static)
	{
		// ��Ԃ̑��
		m_is_static = is_static;

		// �������^�[��
		if (!m_ghost)
		{
			return;
		}

		// Collision�̃t���O�̐ݒ�
		if (m_is_static)
		{
			m_ghost->setCollisionFlags(m_ghost->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
		}
		else
		{
			m_ghost->setCollisionFlags(m_ghost->getCollisionFlags() & ~btCollisionObject::CF_STATIC_OBJECT);
		}

		// Static�łȂ��Ȃ�A�N�V�����C���^�[�t�F�[�X��o�^����
		if (!IsStatic())
		{
			// �A�N�V�����C���^�[�t�F�[�X�̓o�^
			if (!m_set_action)
			{
				PhysicsSystem::GetInstance()->AddAction(m_action.get());
				m_set_action = true;
			}
		}
		else
		{
			// �A�N�V�����C���^�[�t�F�[�X�̉���
			if (m_set_action)
			{
				PhysicsSystem::GetInstance()->RemoveAction(m_action.get());
				m_set_action = false;
			}
		}
	}

	void TriggerBody::SetKinematic(const bool is_kinematic)
	{
		/* nothing */
		// Trigger�͏Փ˂��Ȃ��̂ŃL�l�}�e�B�b�N�ɂ͂Ȃ�Ȃ�
	}

	ShapeID TriggerBody::AddShape(ShapeType shape_type, const VECTOR3& translate, const Quaternion& quat)
	{
		// Children Shape�̍쐬
		const ShapeID ret_id = Collider::AddShape(shape_type, translate, quat);

		return ret_id;
	}

	void TriggerBody::RemoveShape(ShapeID id)
	{
		// Children Shape�̍폜
		Collider::RemoveShape(id);
	}

	void TriggerBody::OnCreated(const std::shared_ptr<Object>& p_self)
	{
		// �S�[�X�g�I�u�W�F�N�g�̍쐬
		m_ghost = std::make_unique<btGhostObject>();
		m_ghost->setCollisionFlags(m_ghost->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

		// ���[�h����Shape������΍쐬
		if (!m_shapes.empty())
		{
			for (const auto& shape : m_shapes)
			{
				// ���̍쐬
				shape->CreateShape();
				// CompoundShape�ɓo�^
				m_compound_shape->addChildShape(shape->GetbtTransform(), shape->GetPtr());
			}
			// AABB�A�����A���ʂ̍Čv�Z
			RecalculateBody();
		}
		// �S�[�X�g�ɃV�F�C�v����t
		m_ghost->setCollisionShape(m_compound_shape.get());

		// �}�l�[�W���[�ɓo�^
		{
			// Object��Collider�ɃL���X�g
			std::shared_ptr<Collider> collider = std::static_pointer_cast<Collider>(p_self);

			// Manager�ɓo�^
			PhysicsSystem::GetInstance()->RegisterGhostObject(collider, m_ghost.get());
		}

		// �A�N�V�����C���^�[�t�F�[�X�̍쐬
		m_action = std::make_unique<TriggerAction>(GetTransform(), m_ghost.get());

		// Static�łȂ��Ȃ�A�N�V�����C���^�[�t�F�[�X��o�^����
		if (!IsStatic())
		{
			// �A�N�V�����C���^�[�t�F�[�X�̓o�^
			PhysicsSystem::GetInstance()->AddAction(m_action.get());
			m_set_action = true;
		}
	}

	void TriggerBody::OnDestroyed()
	{
		// �A�N�V�����C���^�[�t�F�[�X���폜����
		PhysicsSystem::GetInstance()->RemoveAction(m_action.get());
		m_set_action = false;
		// Manager����폜
		PhysicsSystem::GetInstance()->UnregisterGhostObject(m_ghost.get());

		// �A�N�V�����C���^�[�t�F�[�X�̔j��
		m_action.reset();
		// �S�[�X�g�I�u�W�F�N�g�̔j��
		m_ghost.reset();
	}

	void TriggerBody::OnEnabled()
	{
		// ���݂�Transform�ɍX�V����
		{
			m_ghost->getWorldTransform().setOrigin(ConvertVectorTobtVector(GetTransform()->Position()));
			m_ghost->getWorldTransform().setRotation(ConvertQuaternionTobtQuaternion(GetTransform()->Rotation()));
		}
		// TriggerBody��World�ɃZ�b�g
		{
			// ���C���[�O���[�v�ƃ}�X�N���C���[�̎擾
			int group = 0;
			int mask = 0;
			CreateLayerGroupAndMask(group, mask);

			PhysicsSystem::GetInstance()->GetWorld()->addCollisionObject(m_ghost.get(), group, mask);
		}
		// �A�N�V�����C���^�[�t�F�[�X���Z�b�g
		if (!IsStatic())
		{
			if (!m_set_action)
			{
				PhysicsSystem::GetInstance()->GetWorld()->addAction(m_action.get());
				m_set_action = true;
			}
		}
	}

	void TriggerBody::OnDisabled()
	{
		// TriggerBody��World����O��
		PhysicsSystem::GetInstance()->GetWorld()->removeCollisionObject(m_ghost.get());

		// �A�N�V�����C���^�[�t�F�[�X���O��
		if (!IsStatic())
		{
			if (m_set_action)
			{
				PhysicsSystem::GetInstance()->GetWorld()->removeAction(m_action.get());
				m_set_action = false;
			}
		}
	}

	void TriggerBody::OnSetCollisionActive(bool is_active)
	{
		if (is_active)
		{
			OnEnabled();
		}
		else
		{
			OnDisabled();
		}
	}

	void TriggerBody::RecalculateBody()
	{
		// �q�`��̍Đݒ�A����AABB�̍Čv�Z
		RecalculateShapeAABB();
	}

	void TriggerBody::CreateLayerGroupAndMask(int& group, int& mask)
	{
		Layer layer = GetGameObject()->GetLayer();
		int current_layer = (1 << static_cast<int>(layer)) | (1 << static_cast<int>(Layer::Trigger));
		int current_layer_mask = PhysicsSystem::GetInstance()->GetLayerMask(layer);

		// �Փ˂��Ȃ����C���[�̐ݒ�
		int ignore_layer = 0;
		{
			// Static���m�͏Փ˂��Ȃ�
			if (IsStatic())
			{
				constexpr int static_layer = static_cast<int>(Layer::Static);
				current_layer |= (1 << static_layer);
				ignore_layer |= (1 << static_layer);
			}
		}

		group = current_layer;
		mask = current_layer_mask & ~ignore_layer;
	}
#pragma endregion
	// ~TriggerBody class


	////////////////////////////////////////////////////////
	// TriggerBody::TriggerAction class Methods
	////////////////////////////////////////////////////////
	TriggerBody::TriggerAction::TriggerAction(const std::shared_ptr<Transform>& transform_, btGhostObject* ghost_)
		: transform(transform_), ghost(ghost_)
	{
		/* nothing */
	}

	TriggerBody::TriggerAction::~TriggerAction()
	{
		transform.reset();
		ghost = nullptr;
	}

	void TriggerBody::TriggerAction::updateAction(btCollisionWorld* collisionWorld, btScalar deltaTimeStep)
	{
		// Trigger�̈ʒu�����݂�Transform�ʒu�Ɉړ�����
		const btTransform bt_transform(
			ConvertQuaternionTobtQuaternion(transform->Rotation()),
			ConvertVectorTobtVector(transform->Position())
		);
		ghost->setWorldTransform(bt_transform);
	}


}// ~namespace TKGEngine