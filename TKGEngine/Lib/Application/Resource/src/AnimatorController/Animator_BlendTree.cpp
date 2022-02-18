
#include "Animator_BlendTree.h"

#include "Animator_StateMachine.h"
#include "Application/Resource/inc/Shader_Defined.h"
#include "Systems/inc/LogSystem.h"
#include "Utility/inc/myfunc_file.h"

#include <Windows.h>

namespace TKGEngine::Animations
{
	BlendTree::BlendTree()
		: AnimatorBlockBase(BlockType::BlendTree)
	{
		// �{�[�������̗v�f�m��
		for (auto& calc_keyframe : m_calc_child_keyframes)
		{
			calc_keyframe.resize(MAX_BONES);
		}
	}

	BlendTree::BlendTree(const char* name)
		: AnimatorBlockBase(BlockType::BlendTree, name)
	{
		// �{�[�������̗v�f�m��
		for (auto& calc_keyframe : m_calc_child_keyframes)
		{
			calc_keyframe.resize(MAX_BONES);
		}
	}

#ifdef USE_IMGUI
	int BlendTree::OnGUI(
		const GameObjectID goid,
		const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
		std::unordered_map<std::string, std::shared_ptr<Animations::AnimatorControllerParameter>>& parameters
	)
	{
		ImGui::IDWrapped id(this);

		// BlockBase��OnGUI(�u���b�N���ύX�ƕ\��)
		AnimatorBlockBase::OnGUI(goid, blocks, parameters);

		// Blend Type
		ImGui::Text("Blend Type");
		ImGui::AlignedSameLine(0.5f);
		ImGui::ComboEnum<BlendTreeType, BlendTreeType::Max_BlendTreeType>("##BlendType", &m_blend_tree_type);
		// Parameters
		{
			ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
			ImGui::Text("Parameter");
			switch (m_blend_tree_type)
			{
				case BlendTreeType::Simple1D:
				{
					// Parameters.first�𗅗�
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() * 0.4f);
					if (ImGui::BeginCombo("##Select Parameter", m_parameter.c_str()))
					{
						const auto itr_end = parameters.end();
						for (auto itr = parameters.begin(); itr != itr_end; ++itr)
						{
							// �p�����[�^��Float�̂��̂����\��
							if (itr->second->type != AnimatorControllerParameter::Type::Float)
							{
								continue;
							}
							if (ImGui::Selectable(itr->first.c_str(), itr->first == m_parameter ? true : false))
							{
								m_parameter = itr->first;
							}
						}
						ImGui::EndCombo();
					}
				}
				break;
				case BlendTreeType::Simple2D:
				case BlendTreeType::FreeformCartesian2D:
				{
					// X
					ImGui::Text("X");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() * 0.4f);
					if (ImGui::BeginCombo("##Select Parameter X", m_parameter.c_str()))
					{
						const auto itr_end = parameters.end();
						for (auto itr = parameters.begin(); itr != itr_end; ++itr)
						{
							// �p�����[�^��Float�̂��̂����\��
							if (itr->second->type != AnimatorControllerParameter::Type::Float)
							{
								continue;
							}
							if (ImGui::Selectable(itr->first.c_str(), itr->first == m_parameter ? true : false))
							{
								m_parameter = itr->first;
							}
						}
						ImGui::EndCombo();
					}
					// Y
					ImGui::SameLine();
					ImGui::Text("Y");
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() * 0.4f);
					ImGui::SameLine();
					if (ImGui::BeginCombo("##Select Parameter Y", m_parameter_y.c_str()))
					{
						const auto itr_end = parameters.end();
						for (auto itr = parameters.begin(); itr != itr_end; ++itr)
						{
							// �p�����[�^��Float�̂��̂����\��
							if (itr->second->type != AnimatorControllerParameter::Type::Float)
							{
								continue;
							}
							if (ImGui::Selectable(itr->first.c_str(), itr->first == m_parameter_y ? true : false))
							{
								m_parameter_y = itr->first;
							}
						}
						ImGui::EndCombo();
					}
				}
				break;
				case BlendTreeType::FreeformCartesian3D:
				{
					// X
					ImGui::Text("X");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() * 0.25f);
					if (ImGui::BeginCombo("##Select Parameter X", m_parameter.c_str()))
					{
						const auto itr_end = parameters.end();
						for (auto itr = parameters.begin(); itr != itr_end; ++itr)
						{
							// �p�����[�^��Float�̂��̂����\��
							if (itr->second->type != AnimatorControllerParameter::Type::Float)
							{
								continue;
							}
							if (ImGui::Selectable(itr->first.c_str(), itr->first == m_parameter ? true : false))
							{
								m_parameter = itr->first;
							}
						}
						ImGui::EndCombo();
					}
					// Y
					ImGui::SameLine();
					ImGui::Text("Y");
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() * 0.25f);
					ImGui::SameLine();
					if (ImGui::BeginCombo("##Select Parameter Y", m_parameter_y.c_str()))
					{
						const auto itr_end = parameters.end();
						for (auto itr = parameters.begin(); itr != itr_end; ++itr)
						{
							// �p�����[�^��Float�̂��̂����\��
							if (itr->second->type != AnimatorControllerParameter::Type::Float)
							{
								continue;
							}
							if (ImGui::Selectable(itr->first.c_str(), itr->first == m_parameter_y ? true : false))
							{
								m_parameter_y = itr->first;
							}
						}
						ImGui::EndCombo();
					}
					// Z
					ImGui::SameLine();
					ImGui::Text("Z");
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() * 0.25f);
					ImGui::SameLine();
					if (ImGui::BeginCombo("##Select Parameter Z", m_parameter_z.c_str()))
					{
						const auto itr_end = parameters.end();
						for (auto itr = parameters.begin(); itr != itr_end; ++itr)
						{
							// �p�����[�^��Float�̂��̂����\��
							if (itr->second->type != AnimatorControllerParameter::Type::Float)
							{
								continue;
							}
							if (ImGui::Selectable(itr->first.c_str(), itr->first == m_parameter_z ? true : false))
							{
								m_parameter_z = itr->first;
							}
						}
						ImGui::EndCombo();
					}
				}
				break;
				default:
				{
					assert(0 && "Invalid Enumration parameter.");
				}
				break;
			}
		}
		// Speed
		{
			ImGui::Text("Speed");
			ImGui::AlignedSameLine(0.5f);
			ImGui::DragFloat("##Speed", &m_speed, 0.002f, 0.0f, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);
		}
		// Speed Parameter
		{
			ImGui::Text("Speed Parameter");
			ImGui::AlignedSameLine(0.5f);
			ImGui::Checkbox("##Speed Parameter", &m_use_speed_parameter);
			if (m_use_speed_parameter)
			{
				// ���ݑI�𒆂̃p�����[�^���̑��݃`�F�b�N
				const auto itr_find = parameters.find(m_speed_parameter);
				const auto itr_end = parameters.end();
				bool is_registered = true;
				if (itr_find == itr_end)
				{
					is_registered = false;
					m_speed_parameter.clear();
				}
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
				if (ImGui::BeginCombo("##Select Speed Parameter", is_registered ? m_speed_parameter.c_str() : ""))
				{
					for (auto itr = parameters.begin(); itr != itr_end; ++itr)
					{
						// Float�ȊO�͕\�����Ȃ�
						if (itr->second->type != AnimatorControllerParameter::Type::Float)
						{
							continue;
						}
						// �I�����X�g�ɕ\��
						ImGui::IDWrapped speed_param_id(itr->second.get());
						if (ImGui::Selectable(itr->first.c_str()))
						{
							m_speed_parameter = itr->first;
						}
					}
					ImGui::EndCombo();
				}
			}
		}
		// Wrap mode
		{
			ImGui::Text("Wrap Mode");
			ImGui::AlignedSameLine(0.5f);
			int current_wrap_mode = static_cast<int>(m_wrap_mode) - static_cast<int>(WrapMode::Loop);
			constexpr const char* wrap_mode_names[] =
			{
				"Loop",
				"Clamp"
			};
			constexpr int name_size = std::size(wrap_mode_names);
			if (ImGui::Combo("##WrapMode", &current_wrap_mode, wrap_mode_names, name_size))
			{
				m_wrap_mode = static_cast<WrapMode>(current_wrap_mode + static_cast<int>(WrapMode::Loop));
			}
		}
		// Child Clips
		{
			ImGui::Text("Child Clips");
			ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
			{
				// ChildClip::GUI���Ă�
				for (size_t i = 0; i < m_children.size(); ++i)
				{
					ImGui::IDWrapped child_id(static_cast<int>(i));
					// �폜�{�^��
					if (ImGui::ButtonColorSettable("X##Remove ChildClip Button", ImVec4(0.75f, 0.2f, 0.2f, 1.0f), ImVec4(0.55f, 0.2f, 0.2f, 1.0f), ImVec4(0.4f, 0.05f, 0.05f, 1.0f)))
					{
						RemoveChild(i);
						break;
					}
					// ChildClip::GUI
					ImGui::SameLine();
					m_children.at(i).OnGUI(m_blend_tree_type, m_blend_data, i);
				}
				// Add�{�^��
				if (ImGui::Button("Add##ChildClip"))
				{
					m_children.emplace_back();
					const auto child_num = m_children.size();
					m_blend_data.resize(child_num);
					m_influences.resize(child_num);
				}
			}
		}
		// Transitions
		OnGUITransition(blocks, parameters);

		// Event
		constexpr ImGuiTreeNodeFlags tree_flags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
		if (ImGui::TreeNodeEx("Event##Animator BlendTree", tree_flags))
		{
			ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
			{
				// Event::GUI���Ă�
				for (size_t i = 0; i < m_events.size(); ++i)
				{
					ImGui::IDWrapped event_id(static_cast<int>(i));
					// �폜�{�^��
					if (ImGui::ButtonColorSettable("X##Erase Button", ImVec4(0.75f, 0.2f, 0.2f, 1.0f), ImVec4(0.55f, 0.2f, 0.2f, 1.0f), ImVec4(0.4f, 0.05f, 0.05f, 1.0f)))
					{
						RemoveEvent(i);
						break;
					}
					// Event::GUI
					ImGui::SameLine();
					m_events.at(i).OnGUI(goid);
				}
				// Add�{�^��
				if (ImGui::Button("Add##Event"))
				{
					m_events.emplace_back();
				}
			}
			ImGui::TreePop();
		}

		return INVALID_BLOCK_INDEX;
	}
#endif// #ifdef USE_IMGUI

	void BlendTree::SetParameter(const std::string& parameter)
	{
		m_parameter = parameter;
	}

	void BlendTree::SetParameterY(const std::string& parameter_y)
	{
		m_parameter_y = parameter_y;
	}

	void BlendTree::SetParameterZ(const std::string& parameter_z)
	{
		m_parameter_z = parameter_z;
	}

	void BlendTree::AddChild(const std::string& motion_filepath)
	{
		AddChild(motion_filepath, VECTOR2::Zero);
	}
	void BlendTree::AddChild(const std::string& motion_filepath, const float threshold)
	{
		AddChild(motion_filepath, VECTOR3(threshold, 0.0f, 0.0f));
	}
	void BlendTree::AddChild(const std::string& motion_filepath, const VECTOR2& position)
	{
		AddChild(motion_filepath, VECTOR3(position, 0.0f));
	}
	void BlendTree::AddChild(const std::string& motion_filepath, const VECTOR3& position)
	{
		auto& child = m_children.emplace_back();
		child.threshold = position;
		child.clip.AddMotion(motion_filepath);

		const auto child_num = m_children.size();
		// �u�����h�Ɏg�p����f�[�^���̕ύX
		m_blend_data.resize(child_num);
		// �e���x�o�b�t�@�̃T�C�Y�ύX
		m_influences.resize(child_num);
	}

	void BlendTree::RemoveChild(const int index)
	{
		if (index < 0 || index >= static_cast<int>(m_children.size()))
			return;
		m_children.erase(m_children.begin() + index);

		// �u�����h�Ɏg�p����f�[�^���̕ύX
		m_blend_data.pop_back();
		// �e���x�o�b�t�@�̃T�C�Y�ύX
		m_influences.pop_back();
	}

	void BlendTree::SetBlendTreeType(const BlendTreeType type)
	{
		m_blend_tree_type = type;
	}

	void BlendTree::Speed(const float speed)
	{
		m_speed = MyMath::Max(0.0f, speed);
	}

	float BlendTree::Speed() const
	{
		return m_speed;
	}

	void BlendTree::SetWrapMode(WrapMode mode)
	{
		if (mode != WrapMode::Clamp && mode != WrapMode::Loop)
		{
			LOG_ASSERT("BlendTree's WrapMode only Loop or Clamp.");
			return;
		}
		m_wrap_mode = mode;
	}

	void BlendTree::AddEvent(const std::string& class_type_name, const std::string& func_name, const float normalize_time)
	{
		m_events.emplace_back(class_type_name, func_name, normalize_time);
	}
	void BlendTree::AddEvent(const std::string& class_type_name, const std::string& func_name, const float normalize_time, int parameter)
	{
		m_events.emplace_back(class_type_name, func_name, normalize_time, parameter);
	}
	void BlendTree::AddEvent(const std::string& class_type_name, const std::string& func_name, const float normalize_time, float parameter)
	{
		m_events.emplace_back(class_type_name, func_name, normalize_time, parameter);
	}

	void BlendTree::RemoveEvent(const int index)
	{
		if (index < 0 || index >= static_cast<int>(m_events.size()))
			return;
		m_events.erase(m_events.begin() + index);
	}

	// Clip�̎擾��X�V�͂Ȃ��̂Ŗ߂�l��false
	bool BlendTree::UpdateBlock(
		const GameObjectID goid,
		const bool is_root_layer,
		const bool is_additive,
		const bool check_transition,
		const bool is_current,
		const bool state_transitioning,
		const float elapsed_time,
		const float normalize_time,
		const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
		const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters,
		const std::unordered_map<std::string, int>* node_index
	)
	{
		// Transition�ɂ��J�ڃ`�F�b�N
		if (check_transition)
		{
			float duration = 0.0f;
			float offset = 0.0f;
			const int next = CheckTransition(is_current, state_transitioning, normalize_time, m_owner_statemachine_index, parameters, blocks, duration, offset);
			// �J�ڏ����𖞂����Ă��鎞
			if (next != INVALID_BLOCK_INDEX)
			{
				// �J�ڐ悪��������X�e�[�g�}�V���̑J�ڃf�[�^���Z�b�g
				const int next_block_owner_index = blocks.at(next)->GetOwnerStateMachineIndex();
				std::static_pointer_cast<AnimatorStateMachine>(blocks.at(next_block_owner_index))->SetNextData(goid, next, duration, offset, blocks);
			}
		}
		// �C�x���g�̔��΃`�F�b�N
		{
			const size_t event_num = m_events.size();
			if (event_num == 0)
				return false;

			// 0 ~ 1.0�ɂ܂�߂����K������
			const float wrapped_time = normalize_time - static_cast<int>(normalize_time);
			const float prev_wrapped_time = m_prev_normalize_time - static_cast<int>(m_prev_normalize_time);

			// 1�����Ă���Ȃ�S�ẴC�x���g�����s����
			const bool is_next_loop = (wrapped_time < prev_wrapped_time) ? true : false;
			// �C�x���g���ƂɃ`�F�b�N���Ȃ�����s����
			for (size_t i = 0; i < event_num; ++i)
			{
				auto& event = m_events.at(i);
				// ���s�ς݃t���O�̊m�F
				if (event.IsExecutedEvent())
				{
					continue;
				}

				// 1�����Ă���Ȃ�S�ẴC�x���g�����s����
				if (is_next_loop)
				{
					// ���s
					event.ExecuteEvent(goid);
				}
				// wrap����Ă��Ȃ��Ȃ�o�ߎ��Ԃ̊m�F������
				else
				{
					if (event.CheckTime(wrapped_time))
					{
						// ���s
						event.ExecuteEvent(goid);
					}
				}
			}

			// 1��������C�x���g�t���O�����Z�b�g����
			if (is_next_loop)
			{
				for (auto& event : m_events)
				{
					event.ResetExecutedFlag();
				}
			}

			// wrap����1.0�����Ȃ������������邽�߂ɒl��ێ�����
			m_prev_normalize_time = normalize_time;
		}

		return false;
	}

	bool BlendTree::UpdateKeyFrame(
		const bool is_root_layer,
		const bool is_additive,
		const float normalize_time,
		const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
		const std::unordered_map<std::string, int>* node_index,
		std::vector<KeyData>& dst_keydata
	)
	{
		// �Q�ƃC���f�b�N�X�̃`�F�b�N
		if (m_blend_data_num == 0 || m_blend_data.at(0).child_index == INVALID_CHILD_INDEX)
		{
			// keydata�̎擾�Ɏ��s
			return false;
		}
		// WrapMode���l������0 ~ 1��wrap����
		float wrapped_normalize_time = 0.0f;
		bool is_clamped = false;
		if (m_wrap_mode == WrapMode::Loop)
		{
			wrapped_normalize_time = normalize_time - static_cast<int>(normalize_time);
		}
		else if (m_wrap_mode == WrapMode::Clamp)
		{
			if (normalize_time >= 1.0f)
			{
				wrapped_normalize_time = 1.0f;
				is_clamped = true;
			}
			else
			{
				wrapped_normalize_time = normalize_time;
			}
		}

		// 1�����Q�Ƃ��Ă��Ȃ��Ƃ�
		if (m_blend_data_num == 1 || m_blend_data.at(1).child_index == INVALID_CHILD_INDEX)
		{
			const auto& clip = m_children.at(m_blend_data.at(0).child_index).clip;
			const float length = clip.GetLength();
			const float rate = clip.GetSampleRate();
			// �A�j���[�V�������Ԃ̖��[��
			if (is_clamped)
			{
				const int frame = static_cast<int>(length * rate);
				auto clip_keyframe = clip.GetKeyFrame(frame);
				AlignKeyFrame(*clip_keyframe, clip.GetKeyIndex(), node_index, dst_keydata);
			}
			else
			{
				// ���t���[���ڂ�key��
				const float key = wrapped_normalize_time * length * rate;
				GetFrameBlendKey(clip, key, node_index, dst_keydata);
			}

			// ���Z���C���[�͍��������߂�
			if (!is_root_layer && is_additive)
			{
				GetKeyDifference(dst_keydata, *clip.GetKeyFrame(0), clip.GetKeyIndex(), node_index, dst_keydata);
			}
		}
		// 2�ȏ�̃u�����h�̂Ƃ�
		else
		{
			float total_weight = 0.0f;

			// �n�߂̃f�[�^���擾����
			{
				const auto& first_blend_data = m_blend_data.at(0);
				const auto& clip = m_children.at(first_blend_data.child_index).clip;
				const float length = clip.GetLength();
				const float rate = clip.GetSampleRate();
				// �A�j���[�V�������Ԃ̖��[��
				if (is_clamped)
				{
					const int frame = static_cast<int>(length * rate);
					auto clip_keyframe = clip.GetKeyFrame(frame);
					AlignKeyFrame(*clip_keyframe, clip.GetKeyIndex(), node_index, m_calc_child_keyframes[0]);
				}
				else
				{
					const float key = wrapped_normalize_time * clip.GetLength() * clip.GetSampleRate();
					GetFrameBlendKey(clip, key, node_index, m_calc_child_keyframes[0]);
				}

				// ���Z���C���[�͍��������߂�
				if (!is_root_layer && is_additive)
				{
					GetKeyDifference(m_calc_child_keyframes[0], *clip.GetKeyFrame(0), clip.GetKeyIndex(), node_index, m_calc_child_keyframes[0]);
				}

				// Weight�����Z
				total_weight += first_blend_data.weight;
			}

			// BlendData.child_index == INVALID_CHILD_INDEX�ɂȂ�܂Ńu�����h����
			int data_index = 1;
			while (true)
			{
				// �C���f�b�N�X�͈̔͂ƗL���l�`�F�b�N
				if (m_blend_data_num <= data_index || m_blend_data.at(data_index).child_index == INVALID_CHILD_INDEX)
				{
					break;
				}
				const auto& blend_data = m_blend_data.at(data_index);

				// data_index�Ɉʒu��������擾
				const auto& clip = m_children.at(blend_data.child_index).clip;
				const float length = clip.GetLength();
				const float rate = clip.GetSampleRate();
				// �A�j���[�V�������Ԃ̖��[��
				if (is_clamped)
				{
					const int frame = static_cast<int>(length * rate);
					auto clip_keyframe = clip.GetKeyFrame(frame);
					AlignKeyFrame(*clip_keyframe, clip.GetKeyIndex(), node_index, m_calc_child_keyframes[1]);
				}
				else
				{
					const float key = wrapped_normalize_time * clip.GetLength() * clip.GetSampleRate();
					GetFrameBlendKey(clip, key, node_index, m_calc_child_keyframes[1]);
				}

				// ���Z���C���[�͍��������߂�
				if (!is_root_layer && is_additive)
				{
					GetKeyDifference(m_calc_child_keyframes[1], *clip.GetKeyFrame(0), clip.GetKeyIndex(), node_index, m_calc_child_keyframes[1]);
				}

				// Weight�����Z
				const float prev_total_weight = total_weight;
				total_weight += blend_data.weight;

				// �d�݂Ńu�����h����
				BlendKeyFrame(m_calc_child_keyframes[1], m_calc_child_keyframes[0], prev_total_weight / total_weight, m_calc_child_keyframes[0]);

				// �Q�Ƃ���C���f�b�N�X��1��ɐi�߂�
				++data_index;
			}
			std::copy(m_calc_child_keyframes[0].begin(), m_calc_child_keyframes[0].end(), dst_keydata.begin());
		}

		// keydata�̎擾���ł���
		return true;
	}

	void BlendTree::OnReady(const float offset)
	{
		// ���O�̍Đ����Ԃ�����������
		m_prev_normalize_time = offset;
		// �C�x���g�t���O�����ׂă��Z�b�g����
		for (auto& event : m_events)
		{
			event.ResetExecutedFlag();
		}
	}

	void BlendTree::GetNormalizeTime(
		const float elapsed_time,
		const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
		const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters,
		float& weighted_normalize_time,
		float& normalize_time
	)
	{
		// �O�t���[���̏��̃N���A
		m_blend_data_num = 0;
		for (auto& data : m_blend_data)
		{
			data.Clear();
		}
		if (m_blend_tree_type == BlendTreeType::FreeformCartesian2D ||
			m_blend_tree_type == BlendTreeType::FreeformCartesian3D)
		{
			for (auto& influence : m_influences)
			{
				influence = 0.0f;
			}
		}

		// Blend ratio�̌v�Z
		switch (m_blend_tree_type)
		{
			case BlendTreeType::Simple1D:
				CalculateBlendRatioSimple1D(parameters);
				break;
			case BlendTreeType::Simple2D:
				CalculateBlendRatioSimple2D(parameters);
				break;
			case BlendTreeType::FreeformCartesian2D:
				CalculateBlendRatioFreeformCartesian2D(parameters);
				break;
			case BlendTreeType::FreeformCartesian3D:
				CalculateBlendRatioFreeformCartesian3D(parameters);
				break;
		}
		// ���K���o�ߎ��Ԃ̌v�Z
		{
			// �u�����h����N���b�v�����݂��Ȃ�
			if (m_blend_data_num == 0 || m_blend_data.at(0).child_index == INVALID_CHILD_INDEX)
			{
				// children�����݂��Ȃ�
				weighted_normalize_time = 0.0f;
				normalize_time = 0.0f;
				return;
			}

			// �p�����[�^�ŃA�j���[�V�����̑����ɏ�Z������̂����邩���ׂ�
			float speed_param = 1.0f;
			if (m_use_speed_parameter)
			{
				// �p�����[�^�����猟��
				const auto itr_find = parameters.find(m_speed_parameter);
				if (itr_find != parameters.end())
				{
					// Float�ł��邱�Ƃ��m�F
					if (itr_find->second->type == AnimatorControllerParameter::Type::Float)
					{
						speed_param = MyMath::Max(itr_find->second->default_float, 0.0f);
					}
				}
			}
			// Speed����Z����
			speed_param *= m_speed;

			// 1�����g�p������
			if (m_blend_data_num == 1 || m_blend_data.at(1).child_index == INVALID_CHILD_INDEX)
			{
				// children��1�����Q�Ƃ��Ȃ�
				const float rcp_length = m_children.at(m_blend_data.at(0).child_index).clip.GetRCPLength();
				weighted_normalize_time = rcp_length * elapsed_time * speed_param;
				normalize_time = rcp_length * elapsed_time;
				return;
			}
			// �u�����h�����l�����Ď��Ԃ��v�Z

			// �����ԂɊ|����W�������߂�
			float total_multiply_value = 0.0f;
			{
				float total_weight = 0.0f;
				// �n�߂̃f�[�^�����o���Ă���
				total_multiply_value += m_children.at(m_blend_data.at(0).child_index).clip.GetRCPLength();
				total_weight += m_blend_data.at(0).weight;
				// BlendData.child_index == INVALID_CHILD_INDEX�ɂȂ�܂Ńu�����h����
				int data_index = 1;
				while (true)
				{
					// �C���f�b�N�X�͈̔͂ƗL���l�`�F�b�N
					if (m_blend_data_num <= data_index || m_blend_data.at(data_index).child_index == INVALID_CHILD_INDEX)
					{
						break;
					}
					const auto& blend_data = m_blend_data.at(data_index);

					// �W�����u�����h����
					const float prev_weight = total_weight;
					total_weight += blend_data.weight;
					const float ratio = prev_weight / total_weight;
					// Calculate : elapsed / length_1 * ratio + elapsed / length_2 * (1 - ratio)
					total_multiply_value = total_multiply_value * ratio + m_children.at(blend_data.child_index).clip.GetRCPLength() * (1.0f - ratio);

					// �Q�Ƃ���C���f�b�N�X��1��ɐi�߂�
					++data_index;
				}
			}

			weighted_normalize_time = elapsed_time * speed_param * total_multiply_value;
			normalize_time = elapsed_time * total_multiply_value;
		}
	}

	void BlendTree::OnRemoveBlock(int removed_index, std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks)
	{
		// ���g�̃C���f�b�N�X���Ǘ�����X�e�[�g�}�V������폜
		std::static_pointer_cast<AnimatorStateMachine>(blocks.at(m_owner_statemachine_index))->RemoveChildStateAndTree(removed_index);
		// �ێ����Ă���C���f�b�N�X��Transition�̐�̃C���f�b�N�X���C���Aremoved_index�̍폜������
		OnRemoveBlockAllFixAndErase(removed_index, blocks);
	}

	void BlendTree::CalculateBlendRatioSimple1D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters)
	{
		// Children��1�ȉ��̏ꍇ�͑������^�[��
		const size_t child_num = m_children.size();
		if (child_num == 0)
		{
			return;
		}
		else if (child_num == 1)
		{
			if (m_children.at(0).clip.HasMotion())
			{
				m_blend_data.at(0).child_index = 0;
				m_blend_data.at(0).weight = 1.0f;
				m_blend_data_num = 1;

				return;
			}
		}

		// Parameter�̎擾
		const float param = GetParameter1D(parameters);

		// ���݂̒l����傫�����̂Ə��������̂��炻�ꂼ��A�ł��߂�Clip���擾
		int less_index = INVALID_CHILD_INDEX;
		int greater_index = INVALID_CHILD_INDEX;
		{
			float less_dist = FLT_MAX;
			float greater_dist = FLT_MAX;
			// param�ɋ߂����̂�T��
			for (size_t i = 0; i < child_num; ++i)
			{
				const auto& child = m_children.at(i);

				// Clip�̑��݊m�F
				if (!child.clip.HasMotion())
				{
					continue;
				}

				// 2�_�Ԃ̋���
				const float dist = MyMath::Abs(param - child.threshold.x);
				// param��菬����
				if (child.threshold.x <= param)
				{
					// ���ݕۑ����Ă��鋗���Ɣ�r
					if (dist < less_dist)
					{
						less_dist = dist;
						less_index = static_cast<int>(i);
					}
				}
				// param���傫��
				else
				{
					// ���ݕۑ����Ă��鋗���Ɣ�r
					if (dist < greater_dist)
					{
						greater_dist = dist;
						greater_index = static_cast<int>(i);
					}
				}
			}
		}
		// �u�����h�����v�Z
		{
			// less,greater�̗�����Invalid�Ȃ瑁�����^�[��
			if (less_index == INVALID_CHILD_INDEX && greater_index == INVALID_CHILD_INDEX)
			{
				return;
			}

			// less,greater�̈���������݂��Ȃ��Ƃ��͑��݂�����̂�100%�K�p����
			if (less_index == INVALID_CHILD_INDEX)
			{
				m_blend_data.at(0).child_index = greater_index;
				m_blend_data.at(0).weight = 1.0f;
				m_blend_data_num = 1;
				return;
			}
			else if (greater_index == INVALID_CHILD_INDEX)
			{
				m_blend_data.at(0).child_index = less_index;
				m_blend_data.at(0).weight = 1.0f;
				m_blend_data_num = 1;
				return;
			}
			// child��param�̋�������u�����h�����v�Z����
			//    | --------------length-------------------|
			// [less] ---------- (param) -----dist----- [greater]
			//    | --- (ratio) --- | ---- (1 - ratio) ----|
			const float less_param = m_children.at(less_index).threshold.x;
			const float greater_param = m_children.at(greater_index).threshold.x;
			const float length = greater_param - less_param;
			// length��0�̑Ή�
			if (MyMath::Approximately(length, 0.0f))
			{
				m_blend_data.at(0).child_index = less_index;
				m_blend_data.at(0).weight = 1.0f;
				m_blend_data_num = 1;
			}
			else
			{
				// less�̃f�[�^
				auto& less_data = m_blend_data.at(0);
				less_data.child_index = less_index;
				less_data.weight = (greater_param - param) / length;
				// greater�̃f�[�^
				auto& greater_data = m_blend_data.at(1);
				greater_data.child_index = greater_index;
				greater_data.weight = (param - less_param) / length;
				// �u�����h����f�[�^��
				m_blend_data_num = 2;
			}
		}
	}

	void BlendTree::CalculateBlendRatioSimple2D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters)
	{
		// Children��1�ȉ��̏ꍇ�͑������^�[��
		const size_t child_num = m_children.size();
		if (child_num == 0)
		{
			return;
		}
		if (child_num == 1)
		{
			if (m_children.at(0).clip.HasMotion())
			{
				m_blend_data.at(0).child_index = 0;
				m_blend_data.at(0).weight = 1.0f;
				m_blend_data_num = 1;

				return;
			}
		}

		// Parameter�̎擾
		const VECTOR2 param = GetParameter2D(parameters);

		// ���݂̒l�̍ŋߖT�l��2�擾
		// [0]:1�ԋ߂� [1]:2�Ԗڂɋ߂�
		int near_indices[2] = { INVALID_CHILD_INDEX, INVALID_CHILD_INDEX };
		{
			float near_square_distances[2] = { FLT_MAX, FLT_MAX };
			for (size_t i = 0; i < child_num; ++i)
			{
				const auto& child = m_children.at(i);

				// Clip�̑��݊m�F
				if (!child.clip.HasMotion())
				{
					continue;
				}
				// ����2��̌v�Z
				const float sq_dist = VECTOR2::DistanceSq(param, VECTOR2(child.threshold.x, child.threshold.y));
				// ���ݕێ����Ă��鋗��2��Ɣ�r
				if (near_square_distances[1] > sq_dist)
				{
					// 1�ԋ߂�
					if (sq_dist < near_square_distances[0])
					{
						near_indices[1] = near_indices[0];
						near_square_distances[1] = near_square_distances[0];
						near_indices[0] = static_cast<int>(i);
						near_square_distances[0] = sq_dist;
					}
					// 2�Ԗڂɋ߂�
					else if (sq_dist < near_square_distances[1])
					{
						near_indices[1] = static_cast<int>(i);
						near_square_distances[1] = sq_dist;
					}
				}
			}

			// �L����Child��1�A��������0�����擾�ł��Ȃ������ꍇ
			if (near_indices[1] == INVALID_CHILD_INDEX)
			{
				// 0�̏ꍇ
				if (near_indices[0] == INVALID_CHILD_INDEX)
				{
					return;
				}
				// 1�̏ꍇ
				else
				{
					m_blend_data.at(0).child_index = near_indices[0];
					m_blend_data.at(0).weight = 1.0f;
					m_blend_data_num = 1;

					return;
				}
			}
		}

		// �u�����h�����v�Z
		{
			// [0] -----> [1]
			// 
			//        P
			// vec(0 to 1)��vec(0 to P)���ˉe����length(0 to 1)�Ƃ̔䗦�����߂�
			const VECTOR3 threshold_0 = m_children.at(near_indices[0]).threshold;
			const VECTOR3 threshold_1 = m_children.at(near_indices[1]).threshold;
			const VECTOR2 vec_0_to_param = param - VECTOR2(threshold_0.x, threshold_0.y);
			const VECTOR2 vec_0_to_1 = VECTOR2(threshold_1.x, threshold_1.y) - VECTOR2(threshold_0.x, threshold_0.y);
			const VECTOR2 norm_0_to_1 = vec_0_to_1.Normalized();
			const float length_0_to_1 = vec_0_to_1.Length();
			const float dot_0P_to_01 = vec_0_to_param.Dot(norm_0_to_1);
			// ���ς����̎���[0]��100%
			if (dot_0P_to_01 <= 0.0f)
			{
				m_blend_data.at(0).child_index = near_indices[0];
				m_blend_data.at(0).weight = 1.0f;
				m_blend_data_num = 1;

				return;
			}

			float ratio;
			if (MyMath::Approximately(length_0_to_1, 0.0f))
			{
				ratio = 0.0f;
			}
			else
			{
				ratio = dot_0P_to_01 / length_0_to_1;
			}
			// �l�̕ۑ�
			auto& first_data = m_blend_data.at(0);
			first_data.child_index = near_indices[0];
			first_data.weight = 1.0f - ratio;
			auto& second_data = m_blend_data.at(1);
			second_data.child_index = near_indices[1];
			second_data.weight = ratio;
			// �u�����h����f�[�^��
			m_blend_data_num = 2;
		}
	}

	void BlendTree::CalculateBlendRatioFreeformCartesian2D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters)
	{
		// Children��1�ȉ��̏ꍇ�͑������^�[��
		const int child_num = static_cast<int>(m_children.size());
		if (child_num == 0)
		{
			return;
		}
		if (child_num == 1)
		{
			if (m_children.at(0).clip.HasMotion())
			{
				m_blend_data.at(0).child_index = 0;
				m_blend_data.at(0).weight = 1.0f;
				m_blend_data_num = 1;

				return;
			}
		}

		// Children���Ƃ̉e���x���v�Z����Weight���v�Z����

		// Parameter�̎擾
		const VECTOR2 param = GetParameter2D(parameters);

		// �e�N���b�v�̉e�������߂�
		for (int i = 0; i < child_num; ++i)
		{
			// �N���b�v�̑��݊m�F
			if (!m_children.at(i).clip.HasMotion())
				continue;

			// �e���x���v�Z����
			m_influences.at(i) = CalculateClipInfluence(param, i);
		}
		// ���a�����߂�
		float sum_influence = 0.0f;
		{
			for (const float influence : m_influences)
			{
				sum_influence += influence;
			}
		}

		// ���ꂼ���child�ɂ��d�݂����߂�
		for (int i = 0; i < child_num; ++i)
		{
			// 0�ȉ��̉e���x�̂��͉̂e�����󂯂Ȃ�
			if (MyMath::Approximately(m_influences.at(i), 0.0f))
			{
				continue;
			}

			// �u�����h����v�f�ɃZ�b�g����
			m_blend_data.at(m_blend_data_num).child_index = i;
			// �d�݂𐳋K������
			m_blend_data.at(m_blend_data_num).weight = m_influences.at(i) / sum_influence;

			// �Z�b�g����ꏊ���ړ�����
			++m_blend_data_num;
		}
	}

	void BlendTree::CalculateBlendRatioFreeformCartesian3D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters)
	{
		// Children��1�ȉ��̏ꍇ�͑������^�[��
		const int child_num = static_cast<int>(m_children.size());
		if (child_num == 0)
		{
			return;
		}
		if (child_num == 1)
		{
			if (m_children.at(0).clip.HasMotion())
			{
				m_blend_data.at(0).child_index = 0;
				m_blend_data.at(0).weight = 1.0f;
				m_blend_data_num = 1;

				return;
			}
		}

		// Children���Ƃ̉e���x���v�Z����Weight���v�Z����

		// Parameter�̎擾
		const VECTOR3 param = GetParameter3D(parameters);

		// �e�N���b�v�̉e�������߂�
		for (int i = 0; i < child_num; ++i)
		{
			// �N���b�v�̑��݊m�F
			if (!m_children.at(i).clip.HasMotion())
				continue;

			// �e���x���v�Z����
			m_influences.at(i) = CalculateClipInfluence(param, i);
		}
		// ���a�����߂�
		float sum_influence = 0.0f;
		{
			for (const float influence : m_influences)
			{
				sum_influence += influence;
			}
		}

		// ���ꂼ���child�ɂ��d�݂����߂�
		for (int i = 0; i < child_num; ++i)
		{
			// 0�ȉ��̉e���x�̂��͉̂e�����󂯂Ȃ�
			if (MyMath::Approximately(m_influences.at(i), 0.0f))
			{
				continue;
			}

			// �u�����h����v�f�ɃZ�b�g����
			m_blend_data.at(m_blend_data_num).child_index = i;
			// �d�݂𐳋K������
			m_blend_data.at(m_blend_data_num).weight = m_influences.at(i) / sum_influence;

			// �Z�b�g����ꏊ���ړ�����
			++m_blend_data_num;
		}
	}

	float BlendTree::CalculateClipInfluence(const VECTOR2& param, const int child_idx)
	{
		const int child_num = static_cast<int>(m_children.size());
		const VECTOR2 child_threshold = VECTOR2(m_children.at(child_idx).threshold.x, m_children.at(child_idx).threshold.y);

		// ���݂̍ŏ��e���x
		float min_influence = FLT_MAX;
		// �l�̍X�V�����ꂽ��
		bool set_min_value = false;

		for (int i = 0; i < child_num; ++i)
		{
			// ���g�Ɠ����Ƃ��͌v�Z���Ȃ�
			if (i == child_idx)
				continue;

			// �N���b�v�̑��݊m�F
			if (!m_children.at(i).clip.HasMotion())
				continue;

			// 1 - (dot(PiP,PiPj)/dot(PiPj,PiPj))
			const VECTOR2 threshold = VECTOR2(m_children.at(i).threshold.x, m_children.at(i).threshold.y);
			const VECTOR2 pi_to_pj = threshold - child_threshold;
			const float dot_pi_p_vs_pi_pj = VECTOR2::Dot(param - child_threshold, pi_to_pj);
			const float dot_pi_pj_vs_pi_pj = VECTOR2::Dot(pi_to_pj, pi_to_pj);
			const float influence = 1.0f - (dot_pi_p_vs_pi_pj * MyMath::InvertDivisionValue(dot_pi_pj_vs_pi_pj));

			// �ŏ��̒l���`�F�b�N
			if (min_influence > influence)
			{
				min_influence = influence;
				set_min_value = true;
			}
		}

		// �����ȊO�ɃN���b�v�����݂��Ȃ��Ƃ���1.0��Ԃ�
		if (!set_min_value)
		{
			return 1.0f;
		}

		return MyMath::Max(0.0f, min_influence);
	}

	float BlendTree::CalculateClipInfluence(const VECTOR3& param, const int child_idx)
	{
		const int child_num = static_cast<int>(m_children.size());
		const VECTOR3 child_threshold = m_children.at(child_idx).threshold;

		// ���݂̍ŏ��e���x
		float min_influence = FLT_MAX;
		// �l�̍X�V�����ꂽ��
		bool set_min_value = false;

		for (int i = 0; i < child_num; ++i)
		{
			// ���g�Ɠ����Ƃ��͌v�Z���Ȃ�
			if (i == child_idx)
				continue;

			// �N���b�v�̑��݊m�F
			if (!m_children.at(i).clip.HasMotion())
				continue;

			// 1 - (dot(PiP,PiPj)/dot(PiPj,PiPj))
			const VECTOR3 threshold = m_children.at(i).threshold;
			const VECTOR3 pi_to_pj = threshold - child_threshold;
			const float dot_pi_p_vs_pi_pj = VECTOR3::Dot(param - child_threshold, pi_to_pj);
			const float dot_pi_pj_vs_pi_pj = VECTOR3::Dot(pi_to_pj, pi_to_pj);
			const float influence = 1.0f - (dot_pi_p_vs_pi_pj * MyMath::InvertDivisionValue(dot_pi_pj_vs_pi_pj));

			// �ŏ��̒l���`�F�b�N
			if (min_influence > influence)
			{
				min_influence = influence;
				set_min_value = true;
			}
		}

		// �����ȊO�ɃN���b�v�����݂��Ȃ��Ƃ���1.0��Ԃ�
		if (!set_min_value)
		{
			return 1.0f;
		}

		return MyMath::Max(0.0f, min_influence);
	}

	float BlendTree::GetParameter1D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters) const
	{
		float param = 0.0f;
		// map�T��
		const auto itr_find = parameters.find(m_parameter);
		if (itr_find == parameters.end())
		{
			param = 0.0f;
		}
		else
		{
			// Parameter��Type��Float�łȂ��Ƃ���param = 0.0
			if (itr_find->second->type != AnimatorControllerParameter::Type::Float)
			{
				param = 0.0f;
			}
			else
			{
				// Parameter�̒l���擾
				param = itr_find->second->default_float;
			}
		}

		return param;
	}

	VECTOR2 BlendTree::GetParameter2D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters) const
	{
		VECTOR2 param = VECTOR2::Zero;
		// map�T��
		const auto itr_find_x = parameters.find(m_parameter);
		const auto itr_find_y = parameters.find(m_parameter_y);
		// X
		if (itr_find_x == parameters.end())
		{
			param.x = 0.0f;
		}
		else
		{
			// Parameter��Type��Float�łȂ��Ƃ���param = 0.0
			if (itr_find_x->second->type != AnimatorControllerParameter::Type::Float)
			{
				param.x = 0.0f;
			}
			else
			{
				// Parameter�̒l���擾
				param.x = itr_find_x->second->default_float;
			}
		}
		// Y
		if (itr_find_y == parameters.end())
		{
			param.y = 0.0f;
		}
		else
		{
			// Parameter��Type��Float�łȂ��Ƃ���param = 0.0
			if (itr_find_y->second->type != AnimatorControllerParameter::Type::Float)
			{
				param.y = 0.0f;
			}
			else
			{
				// Parameter�̒l���擾
				param.y = itr_find_y->second->default_float;
			}
		}

		return param;
	}

	VECTOR3 BlendTree::GetParameter3D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters) const
	{
		VECTOR3 param = VECTOR3::Zero;
		// map�T��
		const auto itr_find_x = parameters.find(m_parameter);
		const auto itr_find_y = parameters.find(m_parameter_y);
		const auto itr_find_z = parameters.find(m_parameter_z);
		// X
		if (itr_find_x == parameters.end())
		{
			param.x = 0.0f;
		}
		else
		{
			// Parameter��Type��Float�łȂ��Ƃ���param = 0.0
			if (itr_find_x->second->type != AnimatorControllerParameter::Type::Float)
			{
				param.x = 0.0f;
			}
			else
			{
				// Parameter�̒l���擾
				param.x = itr_find_x->second->default_float;
			}
		}
		// Y
		if (itr_find_y == parameters.end())
		{
			param.y = 0.0f;
		}
		else
		{
			// Parameter��Type��Float�łȂ��Ƃ���param = 0.0
			if (itr_find_y->second->type != AnimatorControllerParameter::Type::Float)
			{
				param.y = 0.0f;
			}
			else
			{
				// Parameter�̒l���擾
				param.y = itr_find_y->second->default_float;
			}
		}
		// Z
		if (itr_find_z == parameters.end())
		{
			param.z = 0.0f;
		}
		else
		{
			// Parameter��Type��Float�łȂ��Ƃ���param = 0.0
			if (itr_find_z->second->type != AnimatorControllerParameter::Type::Float)
			{
				param.z = 0.0f;
			}
			else
			{
				// Parameter�̒l���擾
				param.z = itr_find_z->second->default_float;
			}
		}

		return param;
	}

#ifdef USE_IMGUI
	void BlendTree::BlendChildClip::OnGUI(const BlendTreeType blend_type, const std::vector<BlendData>& blend_data, const int self_index)
	{
		// Threshold
		switch (blend_type)
		{
			case BlendTreeType::Simple1D:
			{
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.4f);
				ImGui::DragFloat("##child threshold", &threshold.x, 0.002f);
			}
			break;
			case BlendTreeType::Simple2D:
			case BlendTreeType::FreeformCartesian2D:
			{
				// X
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.2f);
				ImGui::DragFloat("##child threshold x", &threshold.x, 0.002f);
				// Y
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.2f);
				ImGui::DragFloat("##child threshold y", &threshold.y, 0.002f);
			}
			break;
			case BlendTreeType::FreeformCartesian3D:
			{
				// X
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.133f);
				ImGui::DragFloat("##child threshold x", &threshold.x, 0.002f);
				// Y
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.133f);
				ImGui::DragFloat("##child threshold y", &threshold.y, 0.002f);
				// Z
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.133f);
				ImGui::DragFloat("##child threshold z", &threshold.z, 0.002f);
			}
			break;
			default:
			{
				assert(0 && "Invalid Enumration parameter.");
			}
			break;
		}
		// Clip
		{
			// Blend ratio, Clip name
			{
				// Find self weight
				float weight = 0.0f;
				{
					for (const auto& data : blend_data)
					{
						// �I�[�܂ŗ����甭���ł��Ȃ�����
						if (data.child_index == INVALID_CHILD_INDEX)
							break;
						// ���g�̃C���f�b�N�X�ƈ�v������d�݂�Ԃ�
						if (data.child_index == self_index)
						{
							weight = data.weight;
							break;
						}
					}
				}
				// �v���O���X�o�[�ŏd�˂ĕ\��
				ImGui::SameLine();
				const float cursor_pos = ImGui::GetCursorPosX();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() * 0.8f);
				ImGui::ProgressBar(weight, ImVec2(-1.0f, 0.0f), "");

				// Motion Name
				ImGui::SameLine();
				ImGui::SetCursorPosX(cursor_pos);
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() * 0.85f);
				if (clip.HasMotion())
				{
					ImGui::Text(clip.GetMotionName());
				}
				else
				{
					ImGui::TextColored(ImGui::ERROR_FONT_COLOR, "No Clip!");
				}
			}

			// Set motion
			ImGui::SameLine();
			ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvailWidth() - 24.0f, 0.f));
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
			if (ImGui::SmallButton("=##Clip set motion"))
			{
				std::string filepath;
				if (MyFunc::FetchOpenFileName(filepath, TEXT("Motion(*.motion)\0*.motion\0"), TEXT("Load Motion"), TEXT(".\\Asset")))
				{
					clip.AddMotion(filepath);
				}
			}
		}

	}
#endif// #ifdef USE_IMGUI

}// namespace TKGEngine::Animations
