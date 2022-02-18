#include "../inc/CCollider.h"

#include "../inc/CTransform.h"

#include "Systems/inc/PhysicsSystem.h"
#include "Managers/MonoBehaviourManager.h"

#include <unordered_map>
#include <utility>



namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	Collider::Collider()
		: Behaviour(COMPONENT_TYPE::COMPONENT_COLLIDER)
	{
		/* nothing */
	}

	Collider::~Collider()
	{
		/* nothing */
	}

#ifdef USE_IMGUI
	void Collider::OnGUI()
	{
		// �h�����PushID���Ďg�p

		// Behaviour::OnGUI
		Behaviour::OnGUI();

		// Call Function
		ImGui::Checkbox("Call collision function", &m_do_call_function);
		ImGui::SameLine();
		ImGui::HelpMarker("If it is true, call OnCollision and OnTrigger function.");
	}

	void Collider::ShapeGUI()
	{
		constexpr ImGuiTreeNodeFlags tree_flags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
		if (ImGui::TreeNodeEx("Shapes", tree_flags))
		{
			constexpr ImGuiTabBarFlags tab_bar_flags =
				ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_FittingPolicyResizeDown;
			if (ImGui::BeginTabBar("ShapeTagBar", tab_bar_flags))
			{
				// �ǉ��p�̃{�^��
				{
					if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
					{
						ImGui::OpenPopup("Add Shape");
					}
					if (ImGui::BeginPopup("Add Shape"))
					{
						// Box
						if (ImGui::Selectable("Box"))
						{
							AddShape(ShapeType::Box);
						}
						// Sphere
						if (ImGui::Selectable("Sphere"))
						{
							AddShape(ShapeType::Sphere);
						}
						// Capsule
						if (ImGui::Selectable("Capsule"))
						{
							AddShape(ShapeType::Capsule);
						}
						// Cylinder
						if (ImGui::Selectable("Cylinder"))
						{
							AddShape(ShapeType::Cylinder);
						}
						// Cone
						if (ImGui::Selectable("Cone"))
						{
							AddShape(ShapeType::Cone);
						}
						ImGui::EndPopup();
					}
				}
				// �^�u�\��
				constexpr ImGuiTabItemFlags tab_item_flags = ImGuiTabItemFlags_None;
				for (size_t i = 0; i < m_shapes.size(); ++i)
				{
					ImGui::PushID(i);
					bool is_open = true;
					if (ImGui::BeginTabItem(m_shapes.at(i)->GetShapeTypeName(), &is_open, tab_item_flags))
					{
						if (m_shapes.at(i)->OnGUI())
						{
							// �q�`���Transform�̍X�V���������ꍇ
							m_compound_shape->updateChildTransform(i, m_shapes.at(i)->GetbtTransform(), true);
							RecalculateBody();
						}
						ImGui::EndTabItem();
					}
					// �^�u������Ƃ��́AShape���폜����
					if (!is_open)
					{
						RemoveShape(m_shapes.at(i)->GetShapeID());
					}
					ImGui::PopID();
				}
				ImGui::EndTabBar();
			}
			ImGui::TreePop();
		}
	}
#endif // USE_IMGUI

	bool Collider::IsStatic() const
	{
		return m_is_static;
	}

	bool Collider::IsKinematic() const
	{
		return m_is_kinematic;
	}

	void Collider::OnCreate(const std::shared_ptr<Object>& p_self)
	{
		// Compound Shape�̍쐬
		m_compound_shape = std::make_unique<btCompoundShape>();

		// �h���悲�Ƃ̐�������
		OnCreated(p_self);
	}

	void Collider::OnDestroy()
	{
		// �h���悲�Ƃ̔j������
		OnDestroyed();

		// Compound Shape�̔j��
		m_compound_shape.reset();
	}

	void Collider::UpdateChildShape(ShapeID id)
	{
		// �����`�󂩂�ID�T��
		const int child_index = FindShapeFromCompound(id);
		if (child_index < 0)
			return;

		// Shape Data����ID�T��
		const int data_index = FindShapeDataIndex(id);
		if (data_index < 0)
			return;

		// Shape�̍X�V
		m_compound_shape->updateChildTransform(child_index, m_shapes.at(data_index)->GetbtTransform(), true);
	}

	void Collider::AddTriggerHitID(int other)
	{
		m_current_trigger_list.emplace(other);
	}

	void Collider::AddCollisionHitID(int other)
	{
		m_current_collision_list.emplace(other);
	}

	void Collider::UpdateHitState()
	{
		// Trigger
		UpdateTriggerList();
		// ���g��Trigger�Ȃ�Collision�͌Ă΂�Ȃ�
		if (IsTrigger())
		{
			return;
		}

		// Collision
		UpdateCollisionList();
	}

	void Collider::UpdateTriggerList()
	{
		// Collider�}�b�v���擾���Ă���
		const auto* collider_map = PhysicsSystem::GetInstance()->GetColliderMap();

		// prev���X�g�̐擪����T������
		{
			auto itr = m_prev_trigger_list.begin();
			const auto itr_prev_end = m_prev_trigger_list.end();
			const auto itr_current_end = m_current_trigger_list.end();
			for (; itr != itr_prev_end;)
			{
				// prev�ɑ��݂������̂�current�ɑ��݂��邩�`�F�b�N
				const auto itr_find = m_current_trigger_list.find(*itr);
				// ColliderID����Collider�R���|�[�l���g���擾����
				auto other = std::static_pointer_cast<Collider>(collider_map->find(*itr)->second);
				// prev���X�g�ɑ��݂��Acurrent���X�g�ɑ��݂���Ȃ�
				if (itr_find != itr_current_end)
				{
					// OnTriggerStay���Ă�
					MonoBehaviourManager::OnTriggerStay(GetOwnerID(), other);

					// current���X�g�����������
					m_current_trigger_list.erase(itr_find);

					++itr;
				}
				// prev�ɑ��݂������̂�current�ɑ��݂��Ȃ��Ȃ�
				else
				{
					// OnTriggerExit���Ă�
					MonoBehaviourManager::OnTriggerExit(GetOwnerID(), other);

					// prev���X�g�����������
					itr = m_prev_trigger_list.erase(itr);
				}
			}
		}

		// current���X�g�Ɏc���Ă���ID��OnTriggerEnter���Ă�
		{
			for (const auto& id : m_current_trigger_list)
			{
				// ColliderID����Collider�R���|�[�l���g���擾����
				auto other = std::static_pointer_cast<Collider>(collider_map->find(id)->second);

				// OnTriggerEnter���Ă�
				MonoBehaviourManager::OnTriggerEnter(GetOwnerID(), other);

				// current���X�g��ID��prev���X�g�ɓ����
				m_prev_trigger_list.emplace(id);
			}

			// current���X�g���N���A����
			m_current_trigger_list.clear();
		}
	}

	void Collider::UpdateCollisionList()
	{
		// Collider�}�b�v���擾���Ă���
		const auto* collider_map = PhysicsSystem::GetInstance()->GetColliderMap();

		// prev���X�g�̐擪����T������
		{
			auto itr = m_prev_collision_list.begin();
			const auto itr_prev_end = m_prev_collision_list.end();
			const auto itr_current_end = m_current_collision_list.end();
			for (; itr != itr_prev_end;)
			{
				// prev�ɑ��݂������̂�current�ɑ��݂��邩�`�F�b�N
				const auto itr_find = m_current_collision_list.find(*itr);
				// ColliderID����Collider�R���|�[�l���g���擾����
				auto other = std::static_pointer_cast<Collider>(collider_map->find(*itr)->second);
				// prev���X�g�ɑ��݂��Acurrent���X�g�ɑ��݂���Ȃ�
				if (itr_find != itr_current_end)
				{
					// OnCollisionStay���Ă�
					MonoBehaviourManager::OnCollisionStay(GetOwnerID(), other);

					// current���X�g�����������
					m_current_collision_list.erase(itr_find);

					++itr;
				}
				// prev�ɑ��݂������̂�current�ɑ��݂��Ȃ��Ȃ�
				else
				{
					// OnCollisionExit���Ă�
					MonoBehaviourManager::OnCollisionExit(GetOwnerID(), other);

					// prev���X�g�����������
					itr = m_prev_collision_list.erase(itr);
				}
			}
		}

		// current���X�g�Ɏc���Ă���ID��OnCollisionEnter���Ă�
		{
			for (const auto& id : m_current_collision_list)
			{
				// ColliderID����Collider�R���|�[�l���g���擾����
				auto other = std::static_pointer_cast<Collider>(collider_map->find(id)->second);

				// OnCollisionEnter���Ă�
				MonoBehaviourManager::OnCollisionEnter(GetOwnerID(), other);

				// current���X�g��ID��prev���X�g�ɓ����
				m_prev_collision_list.emplace(id);
			}

			// current���X�g���N���A����
			m_current_collision_list.clear();
		}
	}

	int Collider::GetGameObjectID() const
	{
		return GetOwnerID();
	}

	int Collider::GetColliderID() const
	{
		return GetInstanceID();
	}

	int Collider::FindShapeDataIndex(const ShapeID id) const
	{
		// ID�̑��݃`�F�b�N
		int idx = -1;
		const int data_size = m_shapes.size();
		for (int i = 0; i < data_size; ++i)
		{
			// ID����v������break
			if (m_shapes.at(i)->GetShapeID() == id)
			{
				idx = i;
				break;
			}
		}

		return idx;
	}

	int Collider::FindShapeFromCompound(const ShapeID id) const
	{
		// ID�̑��݃`�F�b�N
		int idx = -1;
		const int data_size = m_compound_shape->getNumChildShapes();
		for (int i = 0; i < data_size; ++i)
		{
			// ID����v������break
			if (m_compound_shape->getChildShape(i)->getUserIndex() == id)
			{
				idx = i;
				break;
			}
		}

		return idx;
	}

	void Collider::RecalculateShapeAABB()
	{
		// �q�`��̂��Ȃ���
		{
			// ���O��
			const int child_cnt = m_compound_shape->getNumChildShapes();
			for (int i = child_cnt - 1; i >= 0; --i)
			{
				m_compound_shape->removeChildShapeByIndex(i);
			}

			// ����
			const size_t shape_cnt = m_shapes.size();
			for (const auto& shape : m_shapes)
			{
				m_compound_shape->addChildShape(shape->GetbtTransform(), shape->GetPtr());
			}
		}

		// �����`���AABB���Čv�Z
		m_compound_shape->recalculateLocalAabb();
	}


	ShapeID Collider::AddShape(ShapeType shape_type, const VECTOR3& translate, const Quaternion& quat)
	{
		// Shape���쐬
		std::unique_ptr<ShapeBase> new_shape;
		switch (shape_type)
		{
			case ShapeType::Box:
				new_shape = std::make_unique<BoxShape>();
				break;
			case ShapeType::Sphere:
				new_shape = std::make_unique<SphereShape>();
				break;
			case ShapeType::Capsule:
				new_shape = std::make_unique<CapsuleShape>();
				break;
			case ShapeType::Cylinder:
				new_shape = std::make_unique<CylinderShape>();
				break;
			case ShapeType::Cone:
				new_shape = std::make_unique<ConeShape>();
				break;
				// TODO : ���b�V���R���C�_�[�ǉ��\��
				//case ShapeType::StaticMesh:
				//	break;
				//case ShapeType::ConvexPolytope:
				//	break;
		}
		const ShapeID ret_id = new_shape->CreateShape(translate, quat);

		// CompoundShape�ɓo�^
		{
			const btTransform transform = new_shape->GetbtTransform();
			m_compound_shape->addChildShape(transform, new_shape->GetPtr());
		}

		// �`��f�[�^�z��ɒǉ�
		m_shapes.emplace_back(std::move(new_shape));

		// CompoundShape��AABB���X�V
		RecalculateShapeAABB();

		return ret_id;
	}

	void Collider::RemoveShape(ShapeID id)
	{
		// ID�̃`�F�b�N
		const int idx = FindShapeDataIndex(id);
		// ID�����݂��Ȃ������瑁�����^�[��
		if (idx < 0)
		{
			return;
		}

		// CompoundShape����폜
		{
			// �폜���ɓ�����reculculateLocallAabb()���Ă΂�Ă���
			m_compound_shape->removeChildShape(m_shapes.at(idx)->GetPtr());
		}

		// �����ɃX���b�v���Ă���폜����
		{
			// idx�������̗v�f�łȂ����
			const int shape_max_idx = static_cast<int>(m_shapes.size()) - 1;
			if (idx < shape_max_idx)
			{
				// �폜����v�f�Ɩ����̗v�f�����ւ���
				std::swap(m_shapes.at(idx), m_shapes.at(shape_max_idx));
			}

			// �����̍폜
			m_shapes.pop_back();
		}
	}

	void Collider::SetShapeTranslation(ShapeID id, const VECTOR3& translate)
	{
		// ID�̃`�F�b�N
		const int idx = FindShapeDataIndex(id);
		// ID�����݂��Ȃ������瑁�����^�[��
		if (idx < 0)
		{
			return;
		}

		m_shapes.at(idx)->SetTranslation(translate);

		// �q�`��̍X�V
		UpdateChildShape(id);
	}

	void Collider::SetShapeRotation(ShapeID id, const Quaternion& quat)
	{
		// ID�̃`�F�b�N
		const int idx = FindShapeDataIndex(id);
		// ID�����݂��Ȃ������瑁�����^�[��
		if (idx < 0)
		{
			return;
		}

		m_shapes.at(idx)->SetRotation(quat);

		// �q�`��̍X�V
		UpdateChildShape(id);
	}

	void Collider::SetShapeRotation(ShapeID id, const VECTOR3& euler)
	{
		// ID�̃`�F�b�N
		const int idx = FindShapeDataIndex(id);
		// ID�����݂��Ȃ������瑁�����^�[��
		if (idx < 0)
		{
			return;
		}

		m_shapes.at(idx)->SetRotation(euler.ToQuaternion());

		// �q�`��̍X�V
		UpdateChildShape(id);
	}

	void Collider::SetShapeRadius(ShapeID id, float radius)
	{
		// ID�̃`�F�b�N
		const int idx = FindShapeDataIndex(id);
		// ID�����݂��Ȃ������瑁�����^�[��
		if (idx < 0)
		{
			return;
		}

		m_shapes.at(idx)->SetRadius(radius);
	}

	void Collider::SetShapeHeight(ShapeID id, float height)
	{
		// ID�̃`�F�b�N
		const int idx = FindShapeDataIndex(id);
		// ID�����݂��Ȃ������瑁�����^�[��
		if (idx < 0)
		{
			return;
		}

		m_shapes.at(idx)->SetHeight(height);
	}

	void Collider::SetShapeExtents(ShapeID id, const VECTOR3& extents)
	{
		// ID�̃`�F�b�N
		const int idx = FindShapeDataIndex(id);
		// ID�����݂��Ȃ������瑁�����^�[��
		if (idx < 0)
		{
			return;
		}

		m_shapes.at(idx)->SetExtents(extents);
	}


}// namespace TKGEngine