

#include "../../inc/PhysicsSystem.h"
#include "IBulletDebugDraw.h"

#include "Systems/inc/IGUI.h"

#include "Application/Objects/Components/inc/CCollider.h"
#include "Application/Objects/Components/interface/ICollider.h"

#include <cassert>

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Static variable declaration
	////////////////////////////////////////////////////////
	PhysicsSystem* PhysicsSystem::m_instance = nullptr;


	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	bool PhysicsSystem::Create()
	{
		if (m_instance == nullptr)
		{
			m_instance = new PhysicsSystem;
			return m_instance->Initialize();
		}

		assert(0 && "PhysicsSystem::Create() has called twice.");
		return false;
	}

	void PhysicsSystem::Destroy()
	{
		if (m_instance)
		{
			m_instance->Finalize();
			delete m_instance;
			m_instance = nullptr;
		}
	}

	void PhysicsSystem::FrameUpdate(const float elapsed_time)
	{
		if (m_instance)
		{
			m_instance->Update(elapsed_time);
		}
	}

	PhysicsSystem* PhysicsSystem::GetInstance()
	{
		return m_instance;
	}

#ifdef USE_IMGUI
	void PhysicsSystem::DebugDraw()
	{
		if (m_instance)
		{
			m_instance->OnDebugDraw();
		}
	}

	void PhysicsSystem::OpenLayer()
	{
		m_is_gui_opened = true;
	}

	void PhysicsSystem::OnGUILayer()
	{
		// �E�B���h�E���J����Ă��Ȃ�
		if (!m_is_gui_opened)
			return;

		// ���C���[�ݒ�E�B���h�E�\��
		ImGui::IDWrapped id("PhysicsSystem");
		const ImVec2 window_size(300.0f, 450.0f);
		ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
		constexpr ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize ^ ImGuiWindowFlags_NoCollapse;
		if (ImGui::Begin("Collision Layer Mask", &m_is_gui_opened, flags))
		{
			constexpr int layer_num = static_cast<int>(Layer::MAX_USER_LAYER_NUM);
			static int selection_layer = 0;

			// �ݒ肷�郌�C���[�̑I��
			if(ImGui::BeginCombo("Layer", NAMEOF_ENUM(static_cast<Layer>(selection_layer)).data()))
			{
				for (int i = 0; i < static_cast<int>(Layer::MAX_USER_LAYER_NUM); ++i)
				{
					const bool is_selected = (selection_layer == i);
					if (ImGui::Selectable(NAMEOF_ENUM(static_cast<Layer>(i)).data(),is_selected))
					{
						selection_layer = i;
					}
				}
				ImGui::EndCombo();
			}
			ImGui::Separator();

			constexpr ImGuiTreeNodeFlags tree_flags =
				ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
			constexpr ImGuiWindowFlags child_flags = ImGuiWindowFlags_HorizontalScrollbar;
			ImGui::Indent(window_size.x * 0.05f);
			// �q�E�B���h�E�T�C�Y
			ImVec2 child_window_size;
			child_window_size.x = window_size.x * 0.85f;
			child_window_size.y =
				window_size.y - (ImGui::GetCursorPosY() + ImGui::DEFAULT_SPACE_SIZE * 2.0f + ImGui::ENABLE_SIZE_FROM_BOTTOM);
			// �q�E�B���h�E
			ImGui::BeginChild("Child Window", child_window_size, true, child_flags);
			{
				for (int i = 0; i < layer_num; ++i)
				{
					// �I������[i][j]��[j][i]�ɂ��������
					ImGui::IDWrapped select_id(i);
					if (ImGui::Checkbox("##is collision", &m_layer_mask[selection_layer][i]))
					{
						m_layer_mask[i][selection_layer] = m_layer_mask[selection_layer][i];
					}
					ImGui::SameLine();
					ImGui::TreeNodeEx(NAMEOF_ENUM(static_cast<Layer>(i)).data(), tree_flags);
					{
						if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
						{
							m_layer_mask[selection_layer][i] = !m_layer_mask[selection_layer][i];
							m_layer_mask[i][selection_layer] = m_layer_mask[selection_layer][i];
						}
					}
				}
			}
			ImGui::EndChild();
			ImGui::Unindent(window_size.x * 0.05f);

			// ���C���[�}�X�N�Čv�Z�̃{�^��
			ImVec2 button_size;
			{
				const float width = ImGui::GetWindowWidth();
				constexpr float button_space = 0.2f;
				button_size.x = width * (1.0f - button_space * 2.0f);
				button_size.y = ImGui::DEFAULT_SPACE_SIZE * 1.5f;
				// �{�^���𒆉��ɒu��
				ImGui::SetCursorPosX(width * button_space);
			}
			if (ImGui::Button("Create Layer Mask", button_size))
			{
				CreateLayerMaskParam();
			}
		}
		ImGui::End();
	}
#endif // USE_IMGUI

	void PhysicsSystem::RegisterRigidBody(std::shared_ptr<Collider>& collider, btRigidBody* rigid_body)
	{
		const int collider_id = collider->GetInstanceID();
		// �R���W�����I�u�W�F�N�g�ɌŗL��ID������U��
		rigid_body->setUserIndex(collider_id);

		// �Ǘ��p�z��ɓo�^
		// Collider�̓o�^
		m_collider_map.emplace(collider_id, collider);
		// ColliderID�̓o�^
		m_goid_collision_map[collider->GetOwnerID()].emplace(collider_id);
	}

	void PhysicsSystem::UnregisterRigidBody(btRigidBody* rigid_body)
	{
		// World�������
		m_dynamics_world->removeRigidBody(rigid_body);

		// �Ǘ��p�z�񂩂����
		const auto itr_find = m_collider_map.find(rigid_body->getUserIndex());
		// Collider������������
		if (itr_find != m_collider_map.end())
		{
			const auto goid_itr_find = m_goid_collision_map.find(itr_find->second->GetGameObjectID());
			// OwnerID������������
			if (goid_itr_find != m_goid_collision_map.end())
			{
				// ���X�g����ColliderID���폜
				const int collider_id = itr_find->second->GetColliderID();
				const auto col_itr = goid_itr_find->second.find(collider_id);
				if (col_itr != goid_itr_find->second.end())
				{
					goid_itr_find->second.erase(col_itr);
				}

				// �v�f���Ȃ��Ȃ������͍̂폜����
				if (goid_itr_find->second.empty() == true)
				{
					m_goid_collision_map.erase(goid_itr_find);
				}
			}
			// Collider�̍폜
			m_collider_map.erase(itr_find);
		}
	}

	void PhysicsSystem::RegisterGhostObject(std::shared_ptr<Collider>& collider, btGhostObject* ghost_obj)
	{
		const int collider_id = collider->GetInstanceID();
		// �R���W�����I�u�W�F�N�g�ɌŗL��ID������U��
		ghost_obj->setUserIndex(collider->GetInstanceID());

		// �Ǘ��p�z��ɓo�^
		// Collider�̓o�^
		m_collider_map.emplace(collider_id, collider);
		// ColliderID�̓o�^
		m_goid_collision_map[collider->GetOwnerID()].emplace(collider_id);
	}

	void PhysicsSystem::UnregisterGhostObject(btGhostObject* ghost_obj)
	{
		// World�������
		m_dynamics_world->removeCollisionObject(ghost_obj);

		// �Ǘ��p�z�񂩂����
		const auto itr_find = m_collider_map.find(ghost_obj->getUserIndex());
		// Collider������������
		if (itr_find != m_collider_map.end())
		{
			const auto goid_itr_find = m_goid_collision_map.find(itr_find->second->GetGameObjectID());
			// OwnerID������������
			if (goid_itr_find != m_goid_collision_map.end())
			{
				// ���X�g����ColliderID���폜
				const int collider_id = itr_find->second->GetColliderID();
				const auto col_itr = goid_itr_find->second.find(collider_id);
				if (col_itr != goid_itr_find->second.end())
				{
					goid_itr_find->second.erase(col_itr);
				}

				// �v�f���Ȃ��Ȃ������͍̂폜����
				if (goid_itr_find->second.empty())
				{
					m_goid_collision_map.erase(goid_itr_find);
				}
			}
			// Collider�̍폜
			m_collider_map.erase(itr_find);
		}
	}

	void PhysicsSystem::AddAction(btActionInterface* action) const
	{
		m_dynamics_world->addAction(action);
	}

	void PhysicsSystem::RemoveAction(btActionInterface* action) const
	{
		m_dynamics_world->removeAction(action);
	}

	void PhysicsSystem::SetCollisionActive(const GameObjectID goid, const bool is_active)
	{
		// GameObjectID�ŒT��
		const auto itr_find = m_goid_collision_map.find(goid);
		// �������^�[��
		if (itr_find == m_goid_collision_map.end())
		{
			return;
		}

		// ColliderID����Collider�̊֐����Ă�
		const auto col_itr_end = m_collider_map.end();
		for (const auto& id : itr_find->second)
		{
			const auto col_itr = m_collider_map.find(id);
			// �I�[�m�F
			if (col_itr == col_itr_end)
				continue;
			// �A�N�e�B�u��True�ɂ��鎞�͑S�Ă̏�����True�łȂ��Ƃ����Ȃ�
			if (is_active)
			{
				// �A�N�e�B�u�`�F�b�N
				if (!col_itr->second->IsActiveCollider())
					continue;
			}
			// Collider�̗L���A�����ύX���̊֐����Ă�
			col_itr->second->OnSetCollisionActive(is_active);
		}
	}

	void PhysicsSystem::SetWorldGravity(const VECTOR3& gravity)
	{
		m_gravity = gravity;
		m_dynamics_world->setGravity(ConvertVectorTobtVector(gravity));
	}

	void PhysicsSystem::CreateLayerMaskParam()
	{
		constexpr int MAX_USER_LAYER_NUM = static_cast<int>(Layer::MAX_USER_LAYER_NUM);
		constexpr int initialize_layer_mask_param = static_cast<int>(Layer::AllFilter);
		// ���C���[���ƂɌv�Z����
		for (int i = 0; i < MAX_USER_LAYER_NUM; ++i)
		{
			// �����͂��ׂďՓ�(0~31�r�b�g�𗧂Ă�)
			m_layer_mask_param[i] = initialize_layer_mask_param;

			// 32bit�̃r�b�g�V�t�g���g�p���ă��C���[�}�X�N�̒l���v�Z����
			for (int j = 0; j < MAX_USER_LAYER_NUM; ++j)
			{
				// ������s��Ȃ��g�ݍ��킹�̃t���O�͂��낷
				if (!m_layer_mask[i][j])
				{
					m_layer_mask_param[i] &= ~(1 << j);
				}
			}
		}
	}

	int PhysicsSystem::GetLayerMask(Layer layer)
	{
		return m_layer_mask_param[static_cast<int>(layer)];
	}

	std::shared_ptr<Collider> PhysicsSystem::GetColliderForID(const int col_id)
	{
		const auto itr_find = m_collider_map.find(col_id);
		if (itr_find == m_collider_map.end())
		{
			return std::shared_ptr<Collider>();
		}
		else
		{
			return std::static_pointer_cast<Collider>(itr_find->second);
		}
	}

	void PhysicsSystem::DrawLine(const VECTOR3& from, const VECTOR3& to, const VECTOR3& from_color, const VECTOR3& to_color) const
	{
#ifdef USE_IMGUI
		if (!m_bullet_debug_draw)
			return;
		m_bullet_debug_draw->DrawLine(from, to, from_color, to_color);
#endif // USE_IMGUI
	}

	void PhysicsSystem::DrawSphere(const VECTOR3& pos, const float radius, const VECTOR3& color) const
	{
#ifdef USE_IMGUI
		if (!m_bullet_debug_draw)
			return;
		m_bullet_debug_draw->DrawSphere(pos, radius, color);
#endif // USE_IMGUI
	}

	void PhysicsSystem::DrawBox(const VECTOR3& pos, const Quaternion& rot, const VECTOR3& half_extents, const VECTOR3& color) const
	{
#ifdef USE_IMGUI
		if (!m_bullet_debug_draw)
			return;
		m_bullet_debug_draw->DrawBox(pos, rot, half_extents, color);
#endif // USE_IMGUI
	}

	void PhysicsSystem::DrawCapsule(const VECTOR3& pos, const Quaternion& rot, const float radius, const float height, const VECTOR3& color) const
	{
#ifdef USE_IMGUI
		if (!m_bullet_debug_draw)
			return;
		m_bullet_debug_draw->DrawCapsule(pos, rot, radius, height, color);
#endif // USE_IMGUI
	}

	void PhysicsSystem::DrawCone(const VECTOR3& pos, const Quaternion& rot, const float radius, const float height, const VECTOR3& color) const
	{
#ifdef USE_IMGUI
		if (!m_bullet_debug_draw)
			return;
		m_bullet_debug_draw->DrawCone(pos, rot, radius, height, color);
#endif // USE_IMGUI
	}

	void PhysicsSystem::DrawContactPoint(const VECTOR3& pos, const VECTOR3& normal, const float distance, const VECTOR3& color) const
	{
#ifdef USE_IMGUI
		if (!m_bullet_debug_draw)
			return;
		m_bullet_debug_draw->DrawContactPoint(pos, normal, distance, color);
#endif // USE_IMGUI
	}

	PhysicsSystem::PhysicsSystem()
	{
		constexpr int MAX_USER_LAYER_NUM = static_cast<int>(Layer::MAX_USER_LAYER_NUM);
		// �r�b�g�}�X�N�̏�����
		for (int i = 0; i < MAX_USER_LAYER_NUM; ++i)
		{
			for (int j = 0; j < MAX_USER_LAYER_NUM; ++j)
			{
				m_layer_mask[i][j] = true;
			}
		}
	}

	bool PhysicsSystem::Initialize()
	{
		// �Փˌ��o���@�̑I��
		{
			// �R���W�����v�Z�̐ݒ�
			btDefaultCollisionConstructionInfo cci;
			m_collision_configuration = std::make_unique<btDefaultCollisionConfiguration>(cci);

			// Collision dispatcher
			m_collision_dispatcher =
				std::make_unique<btCollisionDispatcher>(m_collision_configuration.get());
		}
		// �u���[�h�t�F�[�Y�@�̐ݒ�
		m_broadphase = std::make_unique<btDbvtBroadphase>();

		// �S��(���̊ԃ����N)�̃\���o�ݒ�
		m_solver = std::make_unique<btSequentialImpulseConstraintSolver>();

		// Bullet�̃��[���h�쐬
		m_dynamics_world = std::make_unique<btDiscreteDynamicsWorld>
			(
				m_collision_dispatcher.get(),
				m_broadphase.get(),
				m_solver.get(),
				m_collision_configuration.get()
				);
		
		m_dynamics_world->setGravity(ConvertVectorTobtVector(m_gravity));

		//�S�[�X�g�y�A�R�[���o�b�N
		m_ghost_pair_call = std::make_unique<btGhostPairCallback>();
		m_dynamics_world->getPairCache()->setInternalGhostPairCallback(m_ghost_pair_call.get());

		// �u���[�h�t�F�[�Y�t�B���^�[�R�[���o�b�N
		m_broadphase_filter_callback = std::make_unique<BroadphaseFilterCallback>();
		m_dynamics_world->getPairCache()->setOverlapFilterCallback(m_broadphase_filter_callback.get());

#ifdef USE_IMGUI
		// Bullet�f�o�b�O�\��
		m_bullet_debug_draw = std::make_unique<IBulletDebugDraw>();
		m_bullet_debug_draw->Initialize();
		m_bullet_debug_draw->SetDebugMode
		(
			btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawContactPoints
		);
		m_dynamics_world->setDebugDrawer(m_bullet_debug_draw.get());
#endif // USE_IMGUI

		// ���C���[�}�X�N�̒l���v�Z����
		CreateLayerMaskParam();

		return true;
	}

	void PhysicsSystem::Finalize()
	{
		//Bullet�̃I�u�W�F�N�g�͏I�[��������Ă�����
		//�C�e���[�^(index)�����̂܂ܗ��p�ł���
		// Bullet�͔z��ŊǗ�
		//  Remove�̏����͍Ō�̗v�f��swap���ăT�C�Y��1���炷���߁A�I�[����폜����ق�������
		{
			const int constraint_num = m_dynamics_world->getNumConstraints();
			for (int i = constraint_num - 1; i >= 0; --i)
			{
				auto* constraint = m_dynamics_world->getConstraint(i);
				m_dynamics_world->removeConstraint(constraint);
				delete constraint;
			}

			const int col_num = m_dynamics_world->getNumCollisionObjects();
			for (int i = col_num - 1; i >= 0; --i)
			{
				btCollisionObject* obj = m_dynamics_world->getCollisionObjectArray()[i];
				m_dynamics_world->removeCollisionObject(obj);
				btRigidBody* rigid = btRigidBody::upcast(obj);
				if (rigid != nullptr)
				{
					delete rigid->getMotionState();
				}
				delete obj;
			}
		}

		// btBroadphaseInterface�Ȃǃ��[���h�쐬���Ɏw�肵�����̂�
		// btDiscreteDynamicsWorld�N���X�̃f�X�g���N�^�Ŏg�p���邽�ߌ�ō폜
		m_dynamics_world.reset();

#ifdef USE_IMGUI
		m_bullet_debug_draw.reset();
#endif // USE_IMGUI
		m_broadphase_filter_callback.reset();
		m_ghost_pair_call.reset();
		m_solver.reset();
		m_broadphase.reset();
		m_collision_dispatcher.reset();
		m_collision_configuration.reset();
	}

	void PhysicsSystem::Update(const float elapsed_time)
	{
		// Physics World�̍X�V
		m_dynamics_world->stepSimulation(elapsed_time, 1, elapsed_time);

#ifdef USE_IMGUI
		// ���s���ȊO�͌Ă΂�Ȃ�
		if (!IGUI::Get().IsPlaying())
		{
			return;
		}
#endif // USE_IMGUI

		// �Փˏ�Ԃ��擾���čX�V����
		UpdateCollisionState();

		// �Փˏ�Ԃɉ������֐����Ă�
		CallCollisionFunction();
	}

#ifdef USE_IMGUI
	void PhysicsSystem::OnDebugDraw() const
	{
		// TODO : ������ĂԑO�ɃJ��������CBuffer�ɑ���
		m_dynamics_world->debugDrawWorld();
	}
#endif // USE_IMGUI

	void PhysicsSystem::UpdateCollisionState()
	{
		auto* dispatcher = m_dynamics_world->getDispatcher();
		const int num_manifolds = dispatcher->getNumManifolds();
		for (int i = 0; i < num_manifolds; ++i)
		{
			const btPersistentManifold* contact_manifold = dispatcher->getManifoldByIndexInternal(i);
			// �Փ˓_���Ȃ���Α������^�[��
			const int contact_count = contact_manifold->getNumContacts();
			if (contact_count == 0)
			{
				continue;
			}

			const btCollisionObject* obj_a = contact_manifold->getBody0();
			const btCollisionObject* obj_b = contact_manifold->getBody1();

			// �L�����N�^�[�R���g���[���ƏՓ˂����ꍇ�͊֐��͌Ă΂Ȃ�
			if ((obj_a->getCollisionFlags() & btCollisionObject::CollisionFlags::CF_CHARACTER_OBJECT) != 0 ||
				(obj_b->getCollisionFlags() & btCollisionObject::CollisionFlags::CF_CHARACTER_OBJECT) != 0)
			{
				continue;
			}

			// CollisionID����Collider�R���|�[�l���g���擾
			const auto& col_a = m_collider_map.at(obj_a->getUserIndex());
			const auto& col_b = m_collider_map.at(obj_b->getUserIndex());

			// �֐����ĂԂ��ǂ���
			const bool do_call_func_a = col_a->IsCallOnFunction();
			const bool do_call_func_b = col_b->IsCallOnFunction();
			// �Փˑ����Trigger���܂܂�Ă��邩�A�L�l�}�e�B�b�N���m�Ȃ�Trigger���X�g�ɒǉ�
			if ((col_a->IsKinematic() && col_b->IsKinematic()) || col_a->IsTrigger() || col_b->IsTrigger())
			{
				if (do_call_func_a)
					col_a->AddTriggerHitID(obj_b->getUserIndex());

				if (do_call_func_b)
					col_b->AddTriggerHitID(obj_a->getUserIndex());
			}
			// �ǂ����Collision�Ȃ�Collision���X�g�ɒǉ�
			else
			{
				if (do_call_func_a)
					col_a->AddCollisionHitID(obj_b->getUserIndex());

				if (do_call_func_b)
					col_b->AddCollisionHitID(obj_a->getUserIndex());
			}
		}
	}

	void PhysicsSystem::CallCollisionFunction()
	{
		// Collider���X�g����Փˑ���̍X�V������
		for (const auto& col : m_collider_map)
		{
			// �I�u�W�F�N�g�̃A�N�e�B�u�`�F�b�N
			if (!col.second->IsActiveCollider())
				continue;

			// �֐����Ă΂Ȃ��Ȃ瑁�����^�[��
			if (!col.second->IsCallOnFunction())
				continue;

			// ���ꂼ��̍X�V������
			col.second->UpdateHitState();
		}
	}
}