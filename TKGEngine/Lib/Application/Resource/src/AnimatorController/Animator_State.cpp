
#include "Animator_State.h"

#include "Animator_StateMachine.h"
#include "Application/Resource/inc/Shader_Defined.h"
#include "Utility/inc/myfunc_math.h"
#include "Utility/inc/myfunc_file.h"

#include <Windows.h>

namespace TKGEngine::Animations
{
	AnimatorState::AnimatorState()
		: AnimatorBlockBase(BlockType::State)
	{
		/* nothing */
	}

	AnimatorState::AnimatorState(const char* name)
		: AnimatorBlockBase(BlockType::State, name)
	{
		/* nothing */
	}

#ifdef USE_IMGUI
	int AnimatorState::OnGUI(
		const GameObjectID goid,
		const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
		std::unordered_map<std::string, std::shared_ptr<Animations::AnimatorControllerParameter>>& parameters
	)
	{
		ImGui::IDWrapped id(this);

		// BlockBase��OnGUI(�u���b�N���ύX�ƕ\��)
		AnimatorBlockBase::OnGUI(goid, blocks, parameters);

		// Clip
		{
			ImGui::Text("Clip");
			ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
			{
				// ���[�V�����f�[�^�����Ɏ����Ă���Ȃ����\��
				if (m_clip.HasMotion())
				{
					if (ImGui::BeginChild("Motion Data", ImVec2(0.0f, ImGui::GetFontSize() * 6), true))
					{
						m_clip.OnGUI();
					}
					ImGui::EndChild();
				}
				// �Ȃ���Ζ�����������
				else
				{
					ImGui::TextColored(ImGui::ERROR_FONT_COLOR, "No Clip!");
				}
				// Load�p�{�^��
				if (ImGui::Button("Load"))
				{
					std::string filepath;
					if (MyFunc::FetchOpenFileName(filepath, TEXT("Motion(*.motion)\0*.motion\0"), TEXT("Load Motion"), TEXT(".\\Asset")))
					{
						AddClip(filepath);
					}
				}
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
			if(m_use_speed_parameter)
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
						if(itr->second->type != AnimatorControllerParameter::Type::Float)
						{
							continue;
						}
						// �I�����X�g�ɕ\��
						{
							ImGui::IDWrapped select_id(itr->second.get());
							if (ImGui::Selectable(itr->first.c_str()))
							{
								m_speed_parameter = itr->first;
							}
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
			int current_wrap_mode = static_cast<int>(m_wrap_mode);
			constexpr const char* wrap_mode_names[] =
			{
				"Once",
				"Loop",
				"Clamp",
				"Pingpong",
				"RevLoop",
				"RevClamp"
			};
			constexpr int name_size = std::size(wrap_mode_names);
			if (ImGui::Combo("##WrapMode", &current_wrap_mode, wrap_mode_names, name_size))
			{
				m_wrap_mode = static_cast<WrapMode>(current_wrap_mode);
			}
		}
		// Transitions
		OnGUITransition(blocks, parameters);

		// Event
		constexpr ImGuiTreeNodeFlags tree_flags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
		if (ImGui::TreeNodeEx("Event##Animator State", tree_flags))
		{
			ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
			{
				// Event::GUI���Ă�
				for (size_t i = 0; i < m_events.size(); ++i)
				{
					ImGui::IDWrapped event_id(i);
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

	void AnimatorState::AddClip(const std::string& motion_filepath)
	{
		m_clip.AddMotion(motion_filepath);
	}

	void AnimatorState::AddClip(const AnimationClip& clip)
	{
		m_clip = clip;
	}

	const char* AnimatorState::GetMotionName() const
	{
		return m_clip.GetMotionName();
	}

	void AnimatorState::Speed(const float speed)
	{
		m_speed = MyMath::Max(0.0f, speed);
	}

	float AnimatorState::Speed() const
	{
		return m_speed;
	}

	void AnimatorState::SetWrapMode(const WrapMode mode)
	{
		m_wrap_mode = mode;
	}

	void AnimatorState::AddEvent(const std::string& class_type_name, const std::string& func_name, const float normalize_time)
	{
		m_events.emplace_back(class_type_name, func_name, normalize_time);
	}
	void AnimatorState::AddEvent(const std::string& class_type_name, const std::string& func_name, const float normalize_time, int parameter)
	{
		m_events.emplace_back(class_type_name, func_name, normalize_time, parameter);
	}
	void AnimatorState::AddEvent(const std::string& class_type_name, const std::string& func_name, const float normalize_time, float parameter)
	{
		m_events.emplace_back(class_type_name, func_name, normalize_time, parameter);
	}

	void AnimatorState::RemoveEvent(const int index)
	{
		if (index < 0 || index >= static_cast<int>(m_events.size()))
			return;
		m_events.erase(m_events.begin() + index);
	}

	// Clip�̎擾��X�V�͂Ȃ��̂Ŗ߂�l��false
	bool AnimatorState::UpdateBlock(
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

	bool AnimatorState::UpdateKeyFrame(
		const bool is_root_layer,
		const bool is_additive,
		const float normalize_time,
		const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
		const std::unordered_map<std::string, int>* node_index,
		std::vector<KeyData>& dst_keydata
	)
	{
		// clip���Ȃ���Α������^�[��
		if (!m_clip.HasMotion())
		{
			return false;
		}

		const float length = m_clip.GetLength();
		const float rate = m_clip.GetSampleRate();
		const int div = static_cast<int>(normalize_time);
		const float mod = (normalize_time - div) * length;
		float key_time = 0.0f;

		switch (m_wrap_mode)
		{
			case WrapMode::Once:
				if (normalize_time >= 1.0f)
				{
					key_time = 0.0f;
				}
				else
				{
					key_time = length * normalize_time;
				}
				break;
			case WrapMode::Loop:
				key_time = mod;
				break;
			case WrapMode::Clamp:
				if (normalize_time >= 1.0f)
				{
					key_time = length;
				}
				else
				{
					key_time = length * normalize_time;
				}
				break;
			case WrapMode::Pingpong:
				// �����Ȃ琳����
				if ((div % 2) == 0)
				{
					key_time = mod;
				}
				// ��͖߂����
				else
				{
					key_time = length - mod;
				}
				break;
			case WrapMode::RevLoop:
				key_time = length - mod;
				break;
			case WrapMode::RevClamp:
				if (normalize_time >= 1.0f)
				{
					key_time = 0.0f;
				}
				else
				{
					key_time = length * (1.0f - normalize_time);
				}
				break;
		}

		// �A�j���[�V�������Ԃ̖��[���Đ����Ȃ�1�����A�j���[�V�����s����擾
		if (key_time >= length)
		{
			const int frame = static_cast<int>(length * rate);
			auto clip = m_clip.GetKeyFrame(frame);
			AlignKeyFrame(*clip, m_clip.GetKeyIndex(), node_index, dst_keydata);
		}
		// ���鎞�Ԃ��͂���2�̃A�j���[�V�����s����u�����h����
		else
		{
			// ���t���[���ڂ�key��
			const float key = key_time * rate;
			GetFrameBlendKey(m_clip, key, node_index, dst_keydata);
		}

		// ���Z���C���[�͍��������߂�
		if (!is_root_layer && is_additive)
		{
			GetKeyDifference(dst_keydata, *m_clip.GetKeyFrame(0), m_clip.GetKeyIndex(), node_index, dst_keydata);
		}

		// keydata�̎擾���ł���
		return true;
	}

	void AnimatorState::OnReady(const float offset)
	{
		// ���O�̍Đ����Ԃ�����������
		m_prev_normalize_time = offset;
		// �C�x���g�t���O�����ׂă��Z�b�g����
		for(auto& event : m_events)
		{
			event.ResetExecutedFlag();
		}
	}

	void AnimatorState::GetNormalizeTime(
		const float elapsed_time,
		const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
		const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters,
		float& weighted_normalize_time,
		float& normalize_time
	)
	{
		// �p�����[�^�ŃA�j���[�V�����̑����ɏ�Z������̂����邩���ׂ�
		float speed_param = 1.0f;
		if(m_use_speed_parameter)
		{
			// �p�����[�^�����猟��
			const auto itr_find = parameters.find(m_speed_parameter);
			if(itr_find != parameters.end())
			{
				// Float�ł��邱�Ƃ��m�F
				if(itr_find->second->type == AnimatorControllerParameter::Type::Float)
				{
					speed_param = MyMath::Max(itr_find->second->default_float, 0.0f);
				}
			}
		}
		// Speed����Z����
		speed_param *= m_speed;

		// Clip���Ȃ��ꍇ�͎����ԂŕԂ�
		if(!m_clip.HasMotion())
		{
			weighted_normalize_time = elapsed_time * speed_param;
			normalize_time = elapsed_time;
			return;
		}

		weighted_normalize_time = elapsed_time * speed_param * m_clip.GetRCPLength();
		normalize_time = elapsed_time * m_clip.GetRCPLength();
	}

	void AnimatorState::OnRemoveBlock(int removed_index, std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks)
	{
		// ���g�̃C���f�b�N�X���Ǘ�����X�e�[�g�}�V������폜
		std::static_pointer_cast<AnimatorStateMachine>(blocks.at(m_owner_statemachine_index))->RemoveChildStateAndTree(removed_index);
		// �ێ����Ă���C���f�b�N�X��Transition�̐�̃C���f�b�N�X���C���Aremoved_index�̍폜������
		OnRemoveBlockAllFixAndErase(removed_index, blocks);
	}

}// namespace TKGEngine::Animations
