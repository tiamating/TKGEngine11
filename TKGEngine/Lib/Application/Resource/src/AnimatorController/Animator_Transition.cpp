
#include "Animator_Transition.h"

#include "Animator_StateMachine.h"

namespace TKGEngine::Animations
{
	// ====================================================
	// AnimatorTransition
	// ====================================================
	AnimatorTransition::AnimatorTransition(const AnimatorTransition& tr)
	{
		m_conditions = tr.m_conditions;
		m_destination_block = tr.m_destination_block;
		m_exit_time = tr.m_exit_time;
		m_duration = tr.m_duration;
		m_offset = tr.m_offset;
	}

	AnimatorTransition& AnimatorTransition::operator=(const AnimatorTransition& tr)
	{
		m_conditions = tr.m_conditions;
		m_destination_block = tr.m_destination_block;
		m_exit_time = tr.m_exit_time;
		m_duration = tr.m_duration;
		m_offset = tr.m_offset;
		return *this;
	}

#ifdef USE_IMGUI
	void AnimatorTransition::OnGUI(std::unordered_map<std::string, std::shared_ptr<Animations::AnimatorControllerParameter>>& parameters, const bool show_exit_time)
	{
		if (show_exit_time)
		{
			// Exit time
			ImGui::Text("Has Exit Time");
			ImGui::AlignedSameLine(0.5f);
			ImGui::Checkbox("##HasExitTime", &m_has_exit_time);
			if (m_has_exit_time)
			{
				ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
				ImGui::Text("Exit Time");
				ImGui::AlignedSameLine(0.5f);
				ImGui::DragFloat("##ExitTime", &m_exit_time, 0.002f, 0.0f, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);
			}
		}
		// Duration
		ImGui::Text("Duration Time");
		ImGui::AlignedSameLine(0.5f);
		ImGui::DragFloat("##Duration", &m_duration, 0.002f, 0.0f, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);
		// Offset
		ImGui::Text("Transition Offset");
		ImGui::AlignedSameLine(0.5f);
		ImGui::SliderFloat("##Offset", &m_offset, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
		// Interruption Source
		ImGui::Text("Interruption Source");
		ImGui::SameLine();
		ImGui::HelpMarker("Controls how the transition is interrupted.");
		ImGui::AlignedSameLine(0.5f);
		ImGui::ComboEnum<InterruptionSource, InterruptionSource::Max_InterruptionSource>("##InterruptionSource", &m_interruption_source);
		// Conditions
		OnGUICondition(parameters);
	}
	void AnimatorTransition::OnGUICondition(std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters)
	{
		constexpr ImGuiTreeNodeFlags tree_flags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
		if (ImGui::TreeNodeEx("Conditions##AnimatorTransition", tree_flags))
		{
			ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
			// Add�{�^��
			if (ImGui::Button("Add##Conditions"))
			{
				AddCondition(AnimatorCondition::Mode::Greater, "", 0.0f);
			}
			// �eCondition��GUI
			const auto itr_params_end = parameters.end();
			for (size_t i = 0; i < m_conditions.size(); ++i)
			{
				ImGui::IDWrapped condition_id(i);
				auto& condition = m_conditions.at(i);
				// Remove�p�{�^��
				if (ImGui::ButtonColorSettable("X##Condition Remove Button", ImVec4(0.8f, 0.1f, 0.1f, 1.0f), ImVec4(0.8f, 0.4f, 0.4f, 1.0f), ImVec4(0.5f, 0.3f, 0.3f, 1.0f)))
				{
					m_conditions.erase(m_conditions.begin() + i);
					continue;
				}
				// Parameter�I��
				{
					// �ݒ蒆�̕�����\��
					{
						ImGui::SameLine();
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.3f);
						ImGui::IDWrapped id(this);
						std::string str = condition.parameter_name;
						ImGui::InputText("##Parameter Name", &str, ImGuiInputTextFlags_ReadOnly);
					}
					// Parameters���疼�O��I������
					ImGui::SameLine();
					if (ImGui::SmallButton("=##Select Parameter Name"))
					{
						ImGui::OpenPopup("SelectParameterName");
					}
					if (ImGui::BeginPopup("SelectParameterName"))
					{
						// Parameters���ɑ��݂���S�Ă̖��O���擾
						std::list<const char*> parameter_name_list;
						for (auto itr_params = parameters.begin(); itr_params != itr_params_end; ++itr_params)
						{
							parameter_name_list.emplace_back(itr_params->first.c_str());
						}
						// ���X�g���̖��O�����ɕ\��
						const auto itr_list_end = parameter_name_list.end();
						for (auto itr_list = parameter_name_list.begin(); itr_list != itr_list_end; ++itr_list)
						{
							// �I�������p�����[�^����������
							if (ImGui::Selectable(*itr_list))
							{
								condition.parameter_name = *itr_list;
								break;
							}
						}
						ImGui::EndPopup();
					}
				}
				// parameters����p�����[�^���擾
				auto itr_params = parameters.find(condition.parameter_name);
				if (itr_params == itr_params_end)
				{
					continue;
				}
				// 臒l�ݒ�
				{
					// �^���Ƃ�GUI���o��
					switch (itr_params->second->type)
					{
						case AnimatorControllerParameter::Type::Float:
						{
							// < or >
							ImGui::SameLine();
							ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
							constexpr int GREATER_VALUE = 0;
							constexpr int LESS_VALUE = 1;
							constexpr const char* names[] =
							{
								"Greater",
								"Less"
							};
							constexpr AnimatorCondition::Mode modes[] =
							{
								AnimatorCondition::Mode::Greater,
								AnimatorCondition::Mode::Less
							};
							// Condition�̏�����ݒ�
							if (condition.mode != AnimatorCondition::Mode::Greater && condition.mode != AnimatorCondition::Mode::Less)
							{
								condition.mode = AnimatorCondition::Mode::Greater;
							}
							int current = condition.mode == AnimatorCondition::Mode::Greater ? GREATER_VALUE : LESS_VALUE;
							constexpr int num = _countof(names);
							if (ImGui::Combo("##Float", &current, names, num))
							{
								condition.mode = modes[current];
							}
							// 臒l
							ImGui::SameLine();
							ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
							ImGui::DragFloat("Float threshold", &condition.threshold, 0.002f);
						}
						break;
						case AnimatorControllerParameter::Type::Int:
						{
							// == or != or < or >
							ImGui::SameLine();
							ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
							constexpr int EQUAL_VALUE = 0;
							constexpr int NOT_EQUAL_VALUE = 1;
							constexpr int GREATER_VALUE = 2;
							constexpr int LESS_VALUE = 3;
							constexpr const char* names[] =
							{
								"Equal",
								"NotEqual",
								"Greater",
								"Less"
							};
							constexpr AnimatorCondition::Mode modes[] =
							{
								AnimatorCondition::Mode::Equal,
								AnimatorCondition::Mode::NotEqual,
								AnimatorCondition::Mode::Greater,
								AnimatorCondition::Mode::Less
							};
							// Condition�̏�����ݒ�
							if (!(condition.mode == AnimatorCondition::Mode::Equal || condition.mode == AnimatorCondition::Mode::NotEqual
								|| condition.mode == AnimatorCondition::Mode::Greater || condition.mode == AnimatorCondition::Mode::Less)
								)
							{
								condition.mode = AnimatorCondition::Mode::Equal;
							}
							int current =
								condition.mode == AnimatorCondition::Mode::Equal ? EQUAL_VALUE :
								condition.mode == AnimatorCondition::Mode::NotEqual ? NOT_EQUAL_VALUE :
								condition.mode == AnimatorCondition::Mode::Greater ? GREATER_VALUE : LESS_VALUE;
							constexpr int num = _countof(names);
							if (ImGui::Combo("##Int", &current, names, num))
							{
								condition.mode = modes[current];
							}
							// 臒l
							int current_threshold = static_cast<int>(condition.threshold);
							ImGui::SameLine();
							ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
							ImGui::DragInt("##Int threshold", &current_threshold, 0.005f);
							condition.threshold = static_cast<float>(current_threshold);
						}
						break;
						case AnimatorControllerParameter::Type::Bool:
						{
							// True or False
							ImGui::SameLine();
							ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
							constexpr int TRUE_VALUE = 1;
							constexpr int FALSE_VALUE = 0;
							// Condition�̏�����ݒ�
							condition.mode = AnimatorCondition::Mode::If;
							int current = MyMath::Approximately(condition.threshold, 0.0f) ? FALSE_VALUE : TRUE_VALUE;
							constexpr const char* names[] =
							{
								"False",
								"True"
							};
							constexpr int num = _countof(names);
							ImGui::Combo("##Bool", &current, names, num);
							// 臒l
							if (current == TRUE_VALUE)
							{
								condition.threshold = static_cast<float>(TRUE_VALUE);
							}
							else
							{
								condition.threshold = static_cast<float>(FALSE_VALUE);
							}
						}
						break;
					}
				}
			}
			ImGui::TreePop();
		}
	}
#endif// #ifdef USE_IMGUI

	int AnimatorTransition::GetDestinationBlock() const
	{
		return m_destination_block;
	}

	void AnimatorTransition::SetDestinationBlock(const int destination_block_idx)
	{
		m_destination_block = destination_block_idx;
	}

	void AnimatorTransition::AddCondition(AnimatorCondition::Mode mode, const char* parameter, const float threshold)
	{
		m_conditions.emplace_back(AnimatorCondition(mode, threshold, parameter));
	}

	void AnimatorTransition::AddCondition(AnimatorCondition::Mode mode, const char* parameter, const int threshold)
	{
		m_conditions.emplace_back(AnimatorCondition(mode, static_cast<float>(threshold), parameter));
	}

	void AnimatorTransition::AddCondition(AnimatorCondition::Mode mode, const char* parameter, const bool threshold)
	{
		m_conditions.emplace_back(AnimatorCondition(mode, static_cast<float>(threshold), parameter));
	}

	void AnimatorTransition::RemoveCondition(const int index)
	{
		if (index >= static_cast<int>(m_conditions.size()))
			return;

		m_conditions.erase(m_conditions.begin() + index);
	}

	void AnimatorTransition::HasExitTime(const bool use)
	{
		m_has_exit_time = use;
	}

	void AnimatorTransition::ExitTime(const float time)
	{
		m_exit_time = time;
	}

	float AnimatorTransition::ExitTime() const
	{
		return m_exit_time;
	}

	void AnimatorTransition::Duration(const float duration)
	{
		m_duration = duration;
	}

	float AnimatorTransition::Duration() const
	{
		return m_duration;
	}

	void AnimatorTransition::Offset(const float offset)
	{
		m_offset = offset;
	}

	float AnimatorTransition::Offset() const
	{
		return m_offset;
	}

	void AnimatorTransition::SetInterruptionSource(const InterruptionSource interruption)
	{
		m_interruption_source = interruption;
	}

	int AnimatorTransition::CheckTransition(
		const bool is_current,
		const bool state_transitioning,
		const float normalized_time,
		const int owner_index,
		const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters,
		const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
		float& duration,
		float& offset
	)
	{
		// �J�ڐ�̏�񂪂Ȃ���Α������^�[��
		if (m_destination_block == INVALID_BLOCK_INDEX)
			return INVALID_BLOCK_INDEX;

		// �J�ڒ��J�ڂ��\���`�F�b�N
		switch (m_interruption_source)
		{
			case InterruptionSource::None:
				// �J�ڒ��͑J�ڂ��Ȃ�
				if (state_transitioning)
				{
					return INVALID_BLOCK_INDEX;
				}
				break;
			case InterruptionSource::Current:
				// �J�ڒ��͌��̃u���b�N���Q��
				if (state_transitioning)
				{
					if (!is_current)
						return INVALID_BLOCK_INDEX;
				}
				break;
			case InterruptionSource::Next:
				// �J�ڒ��͐�̃u���b�N���Q��
				if (state_transitioning)
				{
					if (is_current)
						return INVALID_BLOCK_INDEX;
				}
				break;
		}

		// �X�e�[�g�}�V����Condition�ł̂ݑJ�ځA�܂��J�ڒ��͎��ԎQ�Ƃ͂��Ȃ�
		if (!state_transitioning)
		{
			// ExitTime�ł̑J�ڂ��L���Ő��K�����Ԃ��������l�̎�
			if (m_has_exit_time && normalized_time >= 0.0f)
			{
				// ExitTime��1.0�����Ȃ疈���[�v�`�F�b�N����
				if (m_exit_time < 1.0f)
				{
					const float mod = MyMath::Mod(normalized_time, 1.0f);
					if (mod < m_exit_time)
					{
						return INVALID_BLOCK_INDEX;
					}
				}
				else
				{
					// �Đ����Ԃ�ExitTime�𒴂���܂őJ�ڂ��Ȃ�
					if (normalized_time < m_exit_time)
					{
						return INVALID_BLOCK_INDEX;
					}
				}
			}
		}

		// �J�ڒ���condition���Ȃ��ꍇ��false��Ԃ�
		if (m_conditions.size() == 0 && state_transitioning)
			return INVALID_BLOCK_INDEX;

		// ExitTime�������Ȃ����AExitTime�𖞂������ꍇ��Condition���`�F�b�N����
		if (ConditionCheckOnCheckTransition(parameters))
		{
			duration = m_duration;
			offset = m_offset;
			return m_destination_block;
		}

		return INVALID_BLOCK_INDEX;
	}

	bool AnimatorTransition::ConditionCheckOnCheckTransition(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters)
	{
		// Trigger�𖞂����������肷�邽�߂̕ϐ�
		bool has_trigger = false;
		std::vector<std::shared_ptr<AnimatorControllerParameter>> trigger_index;

		const size_t cond_cnt = m_conditions.size();
		if (cond_cnt == 0)
		{
			// condition���Ȃ����true
			return true;
		}
		// Condition��S�Ė��������`�F�b�N�A�������Ȃ�����������Α������^�[��
		const auto itr_end = parameters.end();
		for (size_t i = 0; i < cond_cnt; ++i)
		{
			AnimatorCondition& condition = m_conditions.at(i);

			// �p�����[�^�ɓ����Ă��Ȃ����O�ł���ꍇ�͖�������
			const auto itr_find = parameters.find(condition.parameter_name);
			if (itr_find == itr_end)
				continue;

			// �p�����[�^�`�F�b�N
			const auto mode = condition.mode;
			const std::shared_ptr<AnimatorControllerParameter>& param = itr_find->second;
			switch ((param)->type)
			{
				case AnimatorControllerParameter::Type::Float:
					switch (mode)
					{
						case AnimatorCondition::Mode::Greater:
							if ((param)->default_float <= condition.threshold)
								return false;
							break;
						case AnimatorCondition::Mode::Less:
							if ((param)->default_float >= condition.threshold)
								return false;
							break;
						default:
							return false;
					}
					break;
				case AnimatorControllerParameter::Type::Int:
					switch (mode)
					{
						case AnimatorCondition::Mode::Greater:
							if ((param)->default_int <= static_cast<int>(condition.threshold))
								return false;
							break;
						case AnimatorCondition::Mode::Less:
							if ((param)->default_int >= static_cast<int>(condition.threshold))
								return false;
							break;
						case AnimatorCondition::Mode::Equal:
							if ((param)->default_int != static_cast<int>(condition.threshold))
								return false;
							break;
						case AnimatorCondition::Mode::NotEqual:
							if ((param)->default_int == static_cast<int>(condition.threshold))
								return false;
							break;
						default:
							return false;
					}
					break;
				case AnimatorControllerParameter::Type::Bool:
					switch (mode)
					{
						case AnimatorCondition::Mode::If:
							if ((param)->default_bool)
							{
								if (MyMath::Approximately(condition.threshold, 0.0f))
									return false;
							}
							else
							{
								if (condition.threshold > 0.0f)
									return false;
							}
							break;
						default:
							return false;
					}
					break;
				case AnimatorControllerParameter::Type::Trigger:
					if (!(param)->default_bool)
					{
						return false;
					}
					has_trigger = true;
					trigger_index.emplace_back(param);
					break;
			}
		}
		// Trigger���g�p�����ꍇ�̓t���O�����낵�Ă���
		if (has_trigger)
		{
			for (auto&& param : trigger_index)
			{
				if ((param)->type == AnimatorControllerParameter::Type::Trigger)
				{
					(param)->default_bool = false;
				}
			}
		}

		// Condition�����ׂĖ��������̂őJ�ډ\
		return true;
	}


}// namespace TKGEngine::Animations
