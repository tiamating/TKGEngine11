
#include "../inc/CTransform.h"

#include "Managers/GameObjectManager.h"
#include "Managers/SceneManager.h"
#include "Utility/inc/myfunc_math.h"

#ifdef USE_IMGUI
#include "Systems/inc/IGUI.h"
#endif // USE_IMGUI

#include <iterator>
#include <cassert>

REGISTERCOMPONENT(TKGEngine::Transform);

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	Transform::Transform()
		: Component(COMPONENT_TYPE::COMPONENT_GENERAL)
	{
		/* nothing */
	}

	Transform::~Transform()
	{
		/* nothing */
	}

	std::shared_ptr<Transform> Transform::Find(const std::string& obj_name) const
	{
		return OnFindRecurse(obj_name, GetTransform());
	}

#ifdef USE_IMGUI
	void Transform::OnGUI()
	{
		// �ύX���ꂽ���Ƃ����m����t���O
		bool has_changed = false;

		// �ʒu
		ImGui::Text("Pos");
		ImGui::AlignedSameLine(0.6f);
		ImGui::DragFloat3("##Pos", &(m_local_position.x), 0.02f, 0.f, 0.f, "%.3f", ImGuiSliderFlags_NoRoundToFormat);
		if (ImGui::IsItemActive())
		{
			has_changed = true;
		}
		// �I�C���[�p
		ImGui::Text("Rot");
		ImGui::AlignedSameLine(0.6f);
		const auto prev_euler = m_local_euler_angle;
		ImGui::DragFloat3("##Rot", &(m_local_euler_angle.x), 0.2f, 0.f, 0.f, "%.2f", ImGuiSliderFlags_NoRoundToFormat);
		if (ImGui::IsItemActive())
		{
			// NaN���
			if(std::isnan(m_local_euler_angle.x) || std::isnan(m_local_euler_angle.y) || std::isnan(m_local_euler_angle.z))
			{
				m_local_euler_angle = prev_euler;
			}
			else
			{
				m_local_rotation = Quaternion::EulerAngles(m_local_euler_angle);
				has_changed = true;
			}
		}
		// �X�P�[��
		ImGui::Text("Sca");
		ImGui::AlignedSameLine(0.6f);
		ImGui::DragFloat3("##Sca", &(m_local_scale.x), 0.02f, 0.0f, 0.0f, "%.3f", ImGuiSliderFlags_NoRoundToFormat);
		if (ImGui::IsItemActive())
		{
			has_changed = true;
		}

		// ���Z�b�g�{�^��
		const float indent = ImGui::GetContentRegionAvail().x * 0.1f;
		ImGui::Indent(indent);
		if (ImGui::Button("Reset", ImVec2(ImGui::GetContentRegionAvail().x * 0.8f, ImGui::GetFontSize() * 1.2f)))
		{
			m_local_position = VECTOR3::Zero;
			m_local_rotation = Quaternion::Identity;
			m_local_euler_angle = m_local_rotation.ToEulerAngles();
			m_local_scale = VECTOR3::One;
			has_changed = true;
		}
		ImGui::Unindent(indent);

		// �ύX���������ꍇ�ATransform���g��Gizmo�ɒm�点��
		if (has_changed)
		{
			m_is_changed = true;
			IGUI::Get().EnterControlTransform();
		}
		else
		{
			IGUI::Get().ExitControlTransform();
		}
	}
#endif // USE_IMGUI

	void Transform::OnCreate(const std::shared_ptr<Object>& p_self)
	{
		const auto p_comp = std::static_pointer_cast<Component>(p_self);
		m_gameObject = GameObjectManager::GetGameObject(p_comp->GetOwnerID());
		const auto p_transform = std::static_pointer_cast<Transform>(p_comp);
		p_transform->SetTransform(p_transform);
	}

	void Transform::OnDestroy()
	{
		if (!m_is_destroying)
		{
			m_is_root_deleter = true;
			// �e�����݂����炻��children���X�g�������
			if (m_parent)
			{
				m_parent->EraseFromParent(GetInstanceID());
			}
			// ���[�g�I�u�W�F�N�g�Ȃ�V�[������폜����
			else
			{
				const int goid = GetGameObject()->GetGameObjectID();
				SceneManager::EraseGameObjectFromScene(goid);
			}

			// ���g�Ƃ��̎qTransform��GameObject���폜����
			RecurseDestroy();
		}
	}

	void Transform::RecurseOnRotation(const std::shared_ptr<Transform>& parent, Quaternion& rot) const
	{
		// ���[�J���̉�]���|����
		rot *= parent->LocalRotation();
		// �e������΍ċA�I�ɌĂяo��
		if (parent->GetParent())
		{
			RecurseOnRotation(parent->GetParent(), rot);
		}
	}

	void Transform::RecurseOnEulerAngles(const std::shared_ptr<Transform>& parent, VECTOR3& angles) const
	{
		// ���[�J���̃I�C���[�p�����Z
		angles += parent->LocalEulerAngles();
		// �e������΍ċA�I�ɌĂяo��
		if (parent->GetParent())
		{
			RecurseOnEulerAngles(parent->GetParent(), angles);
		}
	}

	void Transform::RecurseOnScale(const std::shared_ptr<Transform>& parent, VECTOR3& scale) const
	{
		// ���[�J���̃X�P�[�����|����
		scale *= parent->LocalScale();
		// �e������΍ċA�I�ɌĂяo��
		if (parent->GetParent())
		{
			RecurseOnScale(parent->GetParent(), scale);
		}
	}

	void Transform::RecurseOnAxis(const std::shared_ptr<Transform>& parent, Quaternion& rot) const
	{
		// ���[�J���̉�]���|����
		rot *= parent->LocalRotation();
		// �e������΍ċA�I�ɌĂяo��
		if (parent->GetParent())
		{
			RecurseOnAxis(parent->GetParent(), rot);
		}
	}

	void Transform::RecurseOnSetScene(const std::shared_ptr<Transform>& child, const int scene_id) const
	{
		const int child_cnt = child->GetChildCount();
		// �S�Ă̎q�I�u�W�F�N�g���ċA�I�ɌĂяo��
		for (int i = 0; i < child_cnt; ++i)
		{
			auto s_ptr = child->GetChild(i);
			if (s_ptr == nullptr)
				continue;
			RecurseOnSetScene(s_ptr, scene_id);
		}
		// �����V�[����ύX����
		child->GetGameObject()->SetScene(scene_id);
	}

	void Transform::RecurseOnCalculateWorldMatrix(const std::shared_ptr<Transform>& parent, MATRIX& matrix) const
	{
		matrix *= parent->GetAffineTransform();
		// �e������΍ċA�I�Ɍv�Z����
		if (parent->GetParent())
		{
			RecurseOnCalculateWorldMatrix(parent->GetParent(), matrix);
		}
	}

	void Transform::RecurseDestroy()
	{
		m_is_destroying = true;
		m_parent.reset();

		if (m_child_count > 0)
		{
			for (auto&& child : m_children)
			{
				auto s_ptr = child.lock();
				if (s_ptr == nullptr)
					continue;
				s_ptr->RecurseDestroy();
			}
		}

		if (!m_is_root_deleter)
		{
			// ���g���A�^�b�`���ꂽGO���폜����
			const auto s_ptr = m_gameObject.lock();
			if (s_ptr == nullptr)
				return;
			ObjectManager::Destroy(s_ptr->GetInstanceID(), -1.0f);
		}
	}

	void Transform::EraseFromParent(InstanceID id)
	{
		const auto itr_end = m_children.end();
		for (auto itr = m_children.begin(); itr != itr_end;)
		{
			const auto s_ptr = itr->lock();
			if (!s_ptr)
			{
				itr = m_children.erase(itr);
				--m_child_count;
				continue;
			}

			if (s_ptr->GetInstanceID() == id)
			{
				m_children.erase(itr);
				--m_child_count;
				break;
			}

			++itr;
		}
	}

	void Transform::OnDeserialize()
	{
		// GameObject�̓o�^���T�|�[�g
		const auto gameobject = m_gameObject.lock();
		GameObjectManager::RegisterGameObjectOnDeserialize(gameobject);
		// ���g��Manager�ɓo�^
		const auto transform = gameobject->GetTransform();
		auto s_component = std::static_pointer_cast<Component>(transform);
		ComponentManager::AddComponentOnSerialize(gameobject->GetGameObjectID(), s_component);
		// Children�����݂���Ȃ�Ύq��Parent�ɓo�^����
		if (transform->m_child_count <= 0)
			return;
		for (auto& child : m_children)
		{
			const auto child_transform = child.lock();
			child_transform->m_parent = transform;
		}
	}

	void Transform::Translate(const VECTOR3& translation, bool self_space)
	{
		if (self_space/* Local */)
		{
			m_local_position += translation;
		}
		else/* World */
		{
			//Position(Position() + translation);
			//VECTOR3 pos = Position();
			OnTranslateSetPosition(translation);
		}

		m_is_changed = true;
	}

	void Transform::Translate(float x, float y, float z, bool self_space)
	{
		Translate(VECTOR3(x, y, z), self_space);
	}

	void Transform::Rotate(const Quaternion& quat, bool self_space)
	{
		if (self_space/* Local */)
		{
			m_local_rotation *= quat;// *m_local_rotation;
		}
		else/* World */
		{
			Rotation(Rotation() * quat);
		}
		m_local_rotation = m_local_rotation.Normalize();
		m_local_euler_angle = m_local_rotation.ToEulerAngles();

		m_is_changed = true;
	}

	void Transform::Rotate(const VECTOR3& eulers, bool self_space)
	{
		Rotate(Quaternion::EulerAngles(eulers), self_space);
	}

	void Transform::Rotate(float x_angle, float y_angle, float z_angle, bool self_space)
	{
		Rotate(Quaternion::EulerAngles(x_angle, y_angle, z_angle), self_space);
	}

	void Transform::Rotate(const VECTOR3& axis, float angle, bool self_space)
	{
		Rotate(Quaternion::AxisAngle(axis, angle), self_space);
	}

	void Transform::LookAt(const std::shared_ptr<Transform>& target, const VECTOR3& world_up)
	{
		LookAt(target->Position(), world_up);
	}

	void Transform::LookAt(const VECTOR3& point, const VECTOR3& world_up)
	{
		const VECTOR3 forward = point - Position();
		Rotation(Quaternion::LookRotation(forward, world_up));
	}

	VECTOR3 Transform::Position() const
	{
		// �e���Ȃ���΂��̂܂ܕԂ�
		if (!m_parent)
			return m_local_position;
		// �e������ΐe�̃��[���h�s����g���ċ��߂�
		return VECTOR3::TransformMatrix(m_local_position, m_parent->GetLocalToWorldMatrix());
	}

	void Transform::Position(const VECTOR3& pos)
	{
		// �e���Ȃ���΂��̂܂ܑ��
		if (!m_parent)
		{
			m_local_position = pos;
		}
		else
		{
			// Scale == 0.0f�Ȃ�0����ŃG���[���
			const VECTOR3 lossy_scale = m_parent->LossyScale();
			const VECTOR3 world_pos = Position();
			VECTOR3 translate = pos - world_pos;
			if (MyMath::Approximately(lossy_scale.x, 0.0f))
			{
				translate.x = 0.0f;
			}
			else
			{
				translate.x = translate.x / lossy_scale.x;
			}

			if (MyMath::Approximately(lossy_scale.y, 0.0f))
			{
				translate.y = 0.0f;
			}
			else
			{
				translate.y = translate.y / lossy_scale.y;
			}

			if (MyMath::Approximately(lossy_scale.z, 0.0f))
			{
				translate.z = 0.0f;
			}
			else
			{
				translate.z = translate.z / lossy_scale.z;
			}

			m_local_position += VECTOR3::TransformRotate(translate, m_parent->Rotation().Inverse());
		}

		m_is_changed = true;
	}

	std::shared_ptr<Transform> Transform::OnFindRecurse(const std::string& obj_name, const std::shared_ptr<Transform>& transform)
	{
		const size_t child_num = transform->m_children.size();
		std::list<std::shared_ptr<Transform>> child_have_list;

		// ���g�̎q�I�u�W�F�N�g���猟������
		for (size_t i = 0; i < child_num; ++i)
		{
			auto child = transform->GetChild(i);
			// null�`�F�b�N
			if (child == nullptr)
				continue;

			// ���O��r
			if (child->GetGameObject()->GetName() == obj_name)
				return child;

			// �����ɂ����炸�q�����Ȃ�T���\��̃��X�g�ɓ����
			if (child->GetChildCount() > 0)
				child_have_list.emplace_back(child);
		}
		// �Ō�܂Ō�����Ȃ������烊�X�g���̎q�I�u�W�F�N�g���ċA��������
		std::shared_ptr<Transform> find_obj = nullptr;
		for (const auto& child : child_have_list)
		{
			find_obj = OnFindRecurse(obj_name, child);
			if (find_obj != nullptr)
				break;
		}
		return find_obj;
	}

	void Transform::OnTranslateSetPosition(const VECTOR3& translate)
	{
		//const VECTOR3 world_pos = Position();
		VECTOR3 translation = translate;

		// Scale == 0.0f�Ȃ�0����ŃG���[���
		if (m_parent)
		{
			const VECTOR3 lossy_scale = m_parent->LossyScale();

			if (MyMath::Approximately(lossy_scale.x, 0.0f))
			{
				translation.x = 0.0f;
			}
			else
			{
				translation.x = translation.x / lossy_scale.x;
			}

			if (MyMath::Approximately(lossy_scale.y, 0.0f))
			{
				translation.y = 0.0f;
			}
			else
			{
				translation.y = translation.y / lossy_scale.y;
			}

			if (MyMath::Approximately(lossy_scale.z, 0.0f))
			{
				translation.z = 0.0f;
			}
			else
			{
				translation.z = translation.z / lossy_scale.z;
			}
		}

		if (m_parent)
		{
			m_local_position += VECTOR3::TransformRotate(translation, m_parent->Rotation().Inverse());
		}
		else
		{
			m_local_position += translation;
		}

		m_is_changed = true;
	}

	Quaternion Transform::Rotation() const
	{
		if (m_parent == nullptr)
			return m_local_rotation;

		// ���g����̊K�w�̉�]���ċA�I�Ɋ|����
		Quaternion rot = m_local_rotation;
		RecurseOnRotation(m_parent, rot);

		return rot;
	}

	void Transform::Rotation(const Quaternion& quat)
	{
		// �e�����I�u�W�F�N�g
		if (m_parent)
		{
			m_local_rotation = (quat / m_parent->Rotation());
		}
		// �e�������Ȃ��I�u�W�F�N�g
		else
		{
			m_local_rotation = quat;
		}
		m_local_rotation = m_local_rotation.Normalize();
		m_local_euler_angle = m_local_rotation.ToEulerAngles();

		m_is_changed = true;
	}

	VECTOR3 Transform::EulerAngles() const
	{
		if (m_parent == nullptr)
			return m_local_euler_angle;

		// ���g����̊K�w�̃I�C���[�p���ċA�I�ɉ��Z
		VECTOR3 angles = m_local_euler_angle;
		RecurseOnEulerAngles(m_parent, angles);

		angles.x = MyMath::Mod(angles.x, 360.0f);
		angles.y = MyMath::Mod(angles.y, 360.0f);
		angles.z = MyMath::Mod(angles.z, 360.0f);
		return angles;
	}

	void Transform::EulerAngles(const VECTOR3& angles)
	{
		m_local_euler_angle += (angles - EulerAngles());
		m_local_euler_angle.x = MyMath::Mod(m_local_euler_angle.x, 360.0f);
		m_local_euler_angle.y = MyMath::Mod(m_local_euler_angle.y, 360.0f);
		m_local_euler_angle.z = MyMath::Mod(m_local_euler_angle.z, 360.0f);
		m_local_rotation = Quaternion::EulerAngles(m_local_euler_angle);

		m_is_changed = true;
	}

	VECTOR3 Transform::LossyScale() const
	{
		if (m_parent == nullptr)
			return m_local_scale;

		// ���g����̊K�w�̃X�P�[�����ċA�I�Ɋ|����
		VECTOR3 scale = m_local_scale;
		RecurseOnScale(m_parent, scale);

		return scale;
	}

	VECTOR3 Transform::Right() const
	{
		if (m_parent == nullptr)
			return m_local_rotation.ToRotationMatrix().Right();

		// ���g����̊K�w�̉�]���ċA�I�Ɋ|����
		Quaternion rot = m_local_rotation;
		RecurseOnAxis(m_parent, rot);

		return rot.ToRotationMatrix().Right();
	}

	void Transform::Right(const VECTOR3& right)
	{
		Rotation(Quaternion::FromToRotation(VECTOR3::Right, right));

		//const Quaternion q_from_to = Quaternion::FromToRotation(Right(), right);
		//m_local_rotation *= q_from_to;
		//m_local_euler_angle = m_local_rotation.ToEulerAngles();

		//m_is_changed = true;
	}

	VECTOR3 Transform::Up() const
	{
		if (m_parent == nullptr)
			return m_local_rotation.ToRotationMatrix().Up();

		// ���g����̊K�w�̉�]���ċA�I�Ɋ|����
		Quaternion rot = m_local_rotation;
		RecurseOnAxis(m_parent, rot);

		return rot.ToRotationMatrix().Up();
	}

	void Transform::Up(const VECTOR3& up)
	{
		Rotation(Quaternion::FromToRotation(VECTOR3::Up, up));

		//const Quaternion q_from_to = Quaternion::FromToRotation(Up(), up);
		//m_local_rotation *= q_from_to;
		//m_local_euler_angle = m_local_rotation.ToEulerAngles();

		//m_is_changed = true;
	}

	VECTOR3 Transform::Forward() const
	{
		if (m_parent == nullptr)
			return m_local_rotation.ToRotationMatrix().Forward();

		// ���g����̊K�w�̉�]���ċA�I�Ɋ|����
		Quaternion rot = m_local_rotation;
		RecurseOnAxis(m_parent, rot);

		return rot.ToRotationMatrix().Forward();
	}

	void Transform::Forward(const VECTOR3& forward)
	{
		Rotation(Quaternion::FromToRotation(VECTOR3::Forward, forward));

		//const Quaternion q_from_to = Quaternion::FromToRotation(Forward(), forward);
		//m_local_rotation *= q_from_to;
		//m_local_euler_angle = m_local_rotation.ToEulerAngles();

		//m_is_changed = true;
	}

	void Transform::SetParent(bool keep_world_pos)
	{
		// �e�������Ă��Ȃ���Α������^�[��
		if (m_parent == nullptr)
			return;

		// ���[���h���W���ێ�����Ȃ�
		if (keep_world_pos == true)
		{
			// �V�������Εϊ��s����Z�b�g����
			SetRelativeMatrix(GetLocalToWorldMatrix());
		}

		// �e�̎q���X�g��T�����Ď��g��ID���������烊�X�g�������
		const int id = GetInstanceID();
		auto& children = m_parent->m_children;
		const auto itr_end = children.end();
		for (auto itr = children.begin(); itr != itr_end; ++itr)
		{
			auto s_ptr = itr->lock();
			if (s_ptr == nullptr)
				continue;

			// ID����v������
			if (s_ptr->GetInstanceID() == id)
			{
				// �q���X�g�������
				children.erase(itr);

				// �q�̃J�E���g���f�N�������g
				--(m_parent->m_child_count);
				break;
			}
		}
		m_parent.reset();

		// �V�[���̃��[�g�ɃZ�b�g����
		{
			const std::shared_ptr<IGameObject> s_go = GetGameObject();
			const auto scene_id = s_go->GetScene();
			SceneManager::MoveGameObjectToScene(s_go->GetGameObjectID(), scene_id);
			s_go->ApplyActiveInHierarchy(true, SceneManager::GetActive(scene_id));
		}
	}

	void Transform::SetParent(const std::shared_ptr<Transform>& parent, bool keep_world_pos)
	{
		// parent���Ȃ����Root�ɂȂ�悤�ɂ���
		if (parent == nullptr)
		{
			SetParent(keep_world_pos);
			return;
		}

		// ���ɐe�q�ɂȂ��Ă����瑁�����^�[��
		if (m_parent == parent)
		{
			return;
		}

		const std::shared_ptr<Transform>& p_transform = GetTransform();
		const std::shared_ptr<IGameObject> s_go = GetGameObject();

		// �V�����V�[�����Z�b�g����B�����Aprev�����[�g�Ȃ�V�[�����[�g����폜�Aprev���q�ŏ����V�[�����قȂ�Ύ��g�̎q���܂߂ĕύX����
		{
			if (p_transform->GetParent() == nullptr)
			{
				// prev�����[�g�Ȃ̂Ń��[�g����폜
				SceneManager::EraseGameObjectFromScene(s_go->GetGameObjectID());
			}
			const int parent_scene_id = parent->GetGameObject()->GetScene();
			const int prev_self_scene_id = s_go->GetScene();
			// �e�̃V�[���Ǝ��g�̃V�[�����قȂ�Ƃ��͎����ȉ��̃I�u�W�F�N�g�̏����V�[����ύX����
			if (parent_scene_id != prev_self_scene_id)
			{
				// �ċA�����Ŏ��g�ȉ��̑S�Ă̏����V�[����ύX����
				RecurseOnSetScene(p_transform, parent_scene_id);
			}
		}

		// ���[���h���W���ێ�����Ȃ�
		if (keep_world_pos)
		{
			// �e��InvWorld�s����擾
			const MATRIX parent_inv_world_matrix = parent->GetWorldToLocalMatrix();

			// ���g�̌��݂�World�s����擾
			const MATRIX self_world_matrix = GetLocalToWorldMatrix();

			// �V�����e���猩������World�s������߂�
			const MATRIX new_local_matrix = self_world_matrix * parent_inv_world_matrix;

			// �V�������Εϊ��s����Z�b�g����
			SetRelativeMatrix(new_local_matrix);
		}
		// ���݂̐e������Ȃ�
		if (m_parent)
		{
			// ���݂̎��g�̐e���玩�g���폜
			const int id = GetInstanceID();
			auto& children = m_parent->m_children;
			const auto itr_end = children.end();
			for (auto itr = children.begin(); itr != itr_end; ++itr)
			{
				auto s_ptr = itr->lock();
				if (s_ptr == nullptr)
					continue;

				// ID����v������
				if (s_ptr->GetInstanceID() == id)
				{
					// �q���X�g�������
					children.erase(itr);

					// �q�̃J�E���g���f�N�������g
					--(m_parent->m_child_count);
					break;
				}
			}
		}
		// �e�̎q�Ɏ��g���Z�b�g
		parent->m_children.emplace_back(p_transform);
		++parent->m_child_count;
		// ���g�̐e��parent���Z�b�g
		p_transform->m_parent = parent;

		// active��Ԃ̍X�V
		s_go->ApplyActiveInHierarchy(parent->GetGameObject()->GetActiveHierarchy(), SceneManager::GetActive(s_go->GetScene()));
	}

	std::weak_ptr<Transform> Transform::FindFromChild(const std::string& name)
	{
		if (m_child_count <= 0)
		{
			return std::weak_ptr<Transform>();
		}

		const auto itr_end = m_children.end();
		for (auto itr = m_children.begin(); itr != itr_end; ++itr)
		{
			const auto ref = itr->lock();
			if (ref == nullptr) continue;

			if (ref->GetName() == name)
			{
				return *itr;
			}
		}
		// Not find.
		LOG_ASSERT("error. Transform::FindFromChild() can not find child.");
		return std::weak_ptr<Transform>();
	}

	std::shared_ptr<Transform> Transform::GetChild(const int index)
	{
		if (m_child_count < index)
		{
			// Out of range
			return nullptr;
		}

		auto itr = m_children.begin();
		std::advance(itr, index);

		auto s_ptr = itr->lock();
		return s_ptr;
	}

	int Transform::GetChildCount() const
	{
		return m_child_count;
	}

	const MATRIX& Transform::GetAffineTransform()
	{
		// �p�����[�^�ɕύX�����������̂ݍs����쐬����
		if (!m_is_changed)
		{
			return m_affine_transform;
		}

		m_affine_transform = MATRIX::CreateWorld(m_local_position, m_local_rotation, m_local_scale);

		// �s��̍X�V����
		m_is_changed = false;
		return m_affine_transform;
	}

	MATRIX Transform::GetLocalToWorldMatrix()
	{
		MATRIX matrix = this->GetAffineTransform();
		// �e���Ȃ���΂��̂܂ܕԂ�
		if (!m_parent)
			return matrix;
		// �ċA�I�Ɍv�Z����
		RecurseOnCalculateWorldMatrix(m_parent, matrix);
		return matrix;
	}

	MATRIX Transform::GetWorldToLocalMatrix()
	{
		return GetLocalToWorldMatrix().Invert();
	}

	void Transform::SetRelativeMatrix(const MATRIX& m)
	{
		m.Decompose(m_local_scale, m_local_rotation, m_local_position);
		m_local_euler_angle = m_local_rotation.ToEulerAngles();

		m_is_changed = true;
	}

	void Transform::SetWorldMatrix(const MATRIX& m)
	{
		// �e�I�u�W�F�N�g���Ȃ��Ȃ�Α��΍s����Z�b�g���鏈���Ɠ���
		if (m_parent == nullptr)
		{
			SetRelativeMatrix(m);
			return;
		}

		// �e�I�u�W�F�N�g�̋t���[���h�s����擾
		const MATRIX parent_inv_world_matrix = m_parent->GetWorldToLocalMatrix();

		// ���΍s����쐬
		const MATRIX relative_matrix = m * parent_inv_world_matrix;

		// ���΍s����Z�b�g����
		SetRelativeMatrix(relative_matrix);
	}

}// namespace TKGEngine