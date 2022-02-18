
#include "Animator_StateMachine.h"

#include "Animator_State.h"

#include "Systems/inc/ITime.h"
#include "Systems/inc/LogSystem.h"
#include "Application/Resource/inc/Shader_Defined.h"

#include <list>

namespace TKGEngine::Animations
{
	AnimatorStateMachine::AnimatorStateMachine()
		: AnimatorBlockBase(BlockType::StateMachine)
	{
		// �{�[�������̗v�f�m��
		m_keyframe.resize(MAX_BONES);
		for (auto& calc_keyframe : m_use_calc_keyframes)
		{
			calc_keyframe.resize(MAX_BONES);
		}
	}

	AnimatorStateMachine::AnimatorStateMachine(const char* name)
		: AnimatorBlockBase(BlockType::StateMachine, name)
	{
		// �{�[�������̗v�f�m��
		m_keyframe.resize(MAX_BONES);
		for (auto& calc_keyframe : m_use_calc_keyframes)
		{
			calc_keyframe.resize(MAX_BONES);
		}
	}

#ifdef USE_IMGUI
	void AnimatorStateMachine::GetManageBlockList(std::list<int>& block_list)
	{
		block_list.clear();
		// State and Tree
		for (auto& index : m_states_and_trees)
		{
			block_list.emplace_back(index);
		}
		// StateMachine
		for (auto& index : m_statemachines)
		{
			block_list.emplace_back(index);
		}
		// �\�[�g
		block_list.sort();
	}
	int AnimatorStateMachine::OnGUIStateMachine(
		const GameObjectID goid,
		const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
		std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters
	)
	{
		// �\������u���b�N��I������
		{
			// ImGui�̃t�B���^�[�@�\
			static ImGuiTextFilter im_filter;
			ImGui::Text("Filter :");
			ImGui::AlignedSameLine(0.5f);
			im_filter.Draw("##Show Block filter");
			ImGui::Text("Select Block Name :");
			ImGui::AlignedSameLine(0.5f);
			OnGUISelectBlock(im_filter, blocks, m_gui_current_select_block_index);
		}
		// �Đ����̃u���b�N��\������
		{
			if (ImGui::BeginChild("Playing State data", ImVec2(0.0f, 80.0f), true))
			{
				// State
				ImGui::Text("State : ");
				ImGui::AlignedSameLine(0.7f);
				switch (m_state_machine_state)
				{
					case StateMachineState::Entry:
						ImGui::Text("Entry");
						break;
					case StateMachineState::Play:
						ImGui::Text("Play");
						break;
					case StateMachineState::Transition:
						ImGui::Text("Transition");
						break;
					case StateMachineState::InterruptTransition:
						ImGui::Text("Interruption Transition");
						break;
				}

				// current
				ImGui::Text("Current : ");
				ImGui::AlignedSameLine(0.7f);
				std::string current_data = (m_current_block_index != INVALID_BLOCK_INDEX) ? blocks.at(m_current_block_index)->GetName() : "";
				current_data += " (" + std::to_string(m_current_normalize_time) + ')';
				ImGui::Text(current_data.c_str());
				// next
				ImGui::Text("Next : ");
				ImGui::AlignedSameLine(0.7f);
				if (m_next_block_index == INVALID_BLOCK_INDEX)
				{
					ImGui::Text("");
				}
				else
				{
					std::string next_data = blocks.at(m_next_block_index)->GetName();
					next_data += " (" + std::to_string(m_next_normalize_time) + ')';
					ImGui::Text(next_data.c_str());
				}

				ImGui::EndChild();
			}
		}
		// �I�𒆂̃u���b�N���Ƃ�GUI�\��
		int next_state_machine_index = INVALID_BLOCK_INDEX;
		if (ImGui::BeginChild("BlockData", ImVec2(0.f, 0.f), true))
		{
			if (m_gui_current_select_block_index != INVALID_BLOCK_INDEX)
			{
				next_state_machine_index = blocks.at(m_gui_current_select_block_index)->OnGUI(goid, blocks, parameters);
				blocks.at(m_gui_current_select_block_index)->OnGUIEventFunction(goid);
			}
			ImGui::EndChild();
		}

		return next_state_machine_index;
	}
	int AnimatorStateMachine::OnGUI(
		const GameObjectID goid,
		const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
		std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters
	)
	{
		ImGui::IDWrapped id(this);

		// �K�w���ړ��������ɁA���ɕ\������u���b�N�̃C���f�b�N�X
		int next_state_machine_index = INVALID_BLOCK_INDEX;

		// Name
		if (m_self_block_index != ROOT_STATEMACHINE_BLOCK_INDEX)
		{
			// BlockBase��OnGUI(�u���b�N���ύX�ƕ\��)
			AnimatorBlockBase::OnGUI(goid, blocks, parameters);
		}
		else
		{
			// Root�͖��O�\���̂�
			ImGui::Text("Name:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			static std::string str = GetName();
			ImGui::InputText("##Animator Block name", &str, ImGuiInputTextFlags_ReadOnly);
		}
		// �X�e�[�g�}�V���̊K�w���ړ�����{�^��
		ImGui::NewLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth());
		if (ImGui::Button("Open StateMachine##AnimatorStateMachine"))
		{
			next_state_machine_index = m_self_block_index;
			m_gui_current_select_block_index = INVALID_BLOCK_INDEX;
		}

		// Transitions
		OnGUITransition(blocks, parameters);

		return next_state_machine_index;
	}
	void AnimatorStateMachine::OnGUIEntry(const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks, std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters)
	{
		ImGui::IDWrapped id(this);
		// ImGui�̃t�B���^�[�@�\
		static ImGuiTextFilter im_filter;
		ImGui::Text("Filter :");
		ImGui::AlignedSameLine(0.5f);
		im_filter.Draw("##Entry Block filter");
		ImGui::Text("Entry Block:");
		ImGui::AlignedSameLine(0.5f);
		// Entry Block
		OnGUISelectBlock(im_filter, blocks, m_entry_index);
		if (m_entry_index == INVALID_BLOCK_INDEX)
		{
			ImGui::TextColored(ImGui::ERROR_FONT_COLOR, "Don't have Set Entry Block!");
		}
		// Entry Transition
		ImGui::Text("Entry Transitions");
		OnGUIEntryTransition(blocks, parameters);
	}
	bool AnimatorStateMachine::OnGUISelectBlock(const ImGuiTextFilter& im_filter, const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks, int& current_index)
	{
		// GUI�ɂ��I��
		bool do_select = false;
		if (ImGui::BeginCombo("##select block", current_index == INVALID_BLOCK_INDEX ? "" : blocks.at(current_index)->GetName()))
		{
			// ���̃X�e�[�g�}�V�����Ǘ�����u���b�N�̃C���f�b�N�X���\�[�g����
			std::list<int> manage_block_list;
			{
				// state and tree
				{
					const auto itr_end = m_states_and_trees.end();
					for (auto itr = m_states_and_trees.begin(); itr != itr_end; ++itr)
					{
						manage_block_list.emplace_back(*itr);
					}
				}
				// state machine
				{
					const auto itr_end = m_statemachines.end();
					for (auto itr = m_statemachines.begin(); itr != itr_end; ++itr)
					{
						manage_block_list.emplace_back(*itr);
					}
				}
				// �\�[�g
				manage_block_list.sort();
			}

			const auto itr_end = manage_block_list.end();
			for (auto itr = manage_block_list.begin(); itr != itr_end; ++itr)
			{
				const int index = *itr;
				const auto& block = blocks.at(index);
				// �t�B���^�[��ʉ߂��邩�`�F�b�N
				if (im_filter.PassFilter(block->GetName()))
				{
					ImGui::IDWrapped id(index);
					if (ImGui::Selectable(block->GetName()))
					{
						current_index = index;
						do_select = true;
					}
				}
			}
			ImGui::EndCombo();
		}

		return do_select;
	}
	void AnimatorStateMachine::OnGUIEntryTransition(const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks, std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters)
	{
		constexpr ImGuiTreeNodeFlags tree_flags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
		if (ImGui::TreeNodeEx("Entry Transitions##AnimatorStateMachine", tree_flags))
		{
			ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
			// �J�ڏ����Ǘ��p�^�u
			constexpr ImGuiTabBarFlags tab_bar_flags =
				ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_FittingPolicyResizeDown;
			if (ImGui::BeginTabBar("Transition Bar", tab_bar_flags))
			{
				// �ǉ��p�̃{�^��
				{
					if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
					{
						ImGui::OpenPopup("Add Entry Transition popup");
					}
					if (ImGui::BeginPopup("Add Entry Transition popup"))
					{
						// �J�ڐ�̑I�������Ă���Transition�ǉ�
						if (ImGui::BeginMenu("Current StateMachine##AnimatorStateMachine"))
						{
							// ImGui�̃t�B���^�[�@�\
							static ImGuiTextFilter im_filter;
							ImGui::Text("Filter");
							ImGui::AlignedSameLine(0.5f);
							im_filter.Draw("##Entry Transition filter");
							int select_index = INVALID_BLOCK_INDEX;
							if (OnGUISelectBlock(im_filter, blocks, select_index))
							{
								if (select_index != INVALID_BLOCK_INDEX)
								{
									AddEntryTransition(select_index);
								}
							}
							ImGui::EndMenu();
						}
						ImGui::EndPopup();
					}
				}
				// �^�u�\��
				constexpr ImGuiTabItemFlags tab_item_flags = ImGuiTabItemFlags_None;
				for (size_t i = 0; i < m_entry_transitions.size(); ++i)
				{
					ImGui::IDWrapped entry_transition_id(i);
					bool is_open = true;
					if (ImGui::BeginTabItem(blocks.at(m_entry_transitions.at(i)->GetDestinationBlock())->GetName(), &is_open, tab_item_flags))
					{
						// �eTransition���Ƃ�GUI
						m_entry_transitions.at(i)->OnGUI(parameters, false);

						ImGui::EndTabItem();
					}
					// �^�u������Ƃ��́ATransition���폜����
					if (!is_open)
					{
						m_entry_transitions.erase(m_entry_transitions.begin() + i);
					}
				}
				ImGui::EndTabBar();
			}
			ImGui::TreePop();
		}
	}
#endif// #ifdef USE_IMGUI
	void AnimatorStateMachine::SetEntryBlock(const int index)
	{
		// �ŏ��Ɏ��s�����u���b�N�ɐݒ�
		m_entry_index = index;
	}

	std::shared_ptr<AnimatorTransition> AnimatorStateMachine::AddEntryTransition(const int destination_block_idx)
	{
		std::shared_ptr<AnimatorTransition> tr = std::make_shared<AnimatorTransition>();
		m_entry_transitions.emplace_back(tr);
		tr->SetDestinationBlock(destination_block_idx);
		return tr;
	}

	int AnimatorStateMachine::CheckEntryTransition(
		const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters,
		const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
		float& offset
	)
	{
		// Entry Transition�����Ɋm�F���Ă����A�����𖞂�������J��
		float duration = 0.0f;	// �J�ڎ��Ԃ�Entry�ɂ͂Ȃ�
		for (const auto& transition : m_entry_transitions)
		{
			// �J�ډ\���`�F�b�N����
			const int entry_index = transition->CheckTransition(true, false, 0.0f, m_owner_statemachine_index, parameters, blocks, duration, offset);
			if (entry_index == INVALID_BLOCK_INDEX)
				continue;
			return entry_index;
		}
		// Entry Transition�őJ�ڂ��Ȃ��Ȃ�DefaultEntryIndex�ɑJ��
		return m_entry_index;
	}

	void AnimatorStateMachine::SetNextData(
		const GameObjectID goid,
		const int next,
		const float duration,
		const float offset,
		const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks
	)
	{
		// ���݂̑J�ڐ�Ǝ��̑J�ڐ悪�����Ȃ�X�L�b�v
		if (m_next_block_index == next)
			return;

		// �J�ڒ��ɑJ�ڂ���ꍇ
		if (m_state_machine_state == StateMachineState::Transition || m_state_machine_state == StateMachineState::InterruptTransition)
		{
			// ���荞�ݑJ�ڑO�̑J�ڐ����ێ�����
			if (m_state_machine_state == StateMachineState::Transition)
			{
				m_interrupted_next_block_index = m_next_block_index;
				// ���荞�ݑJ�ڑO�̑J�ڐ�u���b�N��Exit���Ă�
				blocks.at(m_next_block_index)->ExecuteExitFunction(goid);
			}
			// ���݂̑J�ڒ�KeyData��ێ����Ďg�p����
			std::copy(m_keyframe.begin(), m_keyframe.end(), m_use_calc_keyframes[0].begin());
			// ���荞�ݏ����Z�b�g����
			m_state_machine_state = StateMachineState::InterruptTransition;
		}
		else
		{
			// �J�ڏ�Ԃɂ���
			m_state_machine_state = StateMachineState::Transition;
		}
		// �J�ڐ�̃u���b�N��������Ԃɂ���
		blocks.at(next)->OnReady(offset);
		// next�u���b�N��Enter���Ă�
		blocks.at(next)->ExecuteEnterFunction(goid);
		// �J�ڐ�ƑJ�ڂ̏����Z�b�g
		m_next_block_index = next;
		m_next_normalize_time = offset;
		m_duration_normalize_timer = duration;
		m_duration_start_time = m_duration_normalize_timer;
	}

	void AnimatorStateMachine::AddChildStateAndTree(const int index)
	{
		m_states_and_trees.emplace(index);
	}

	void AnimatorStateMachine::RemoveChildStateAndTree(const int index)
	{
		m_states_and_trees.erase(index);
	}

	void AnimatorStateMachine::AddChildStateMachine(const int index)
	{
		m_statemachines.emplace(index);
	}

	void AnimatorStateMachine::RemoveChildStateMachine(const int index)
	{
		m_statemachines.erase(index);
	}

	const std::vector<KeyData>* AnimatorStateMachine::GetKeyFrame() const
	{
		return &m_keyframe;
	}

	void AnimatorStateMachine::ApplyKeyFrameData(
		const GameObjectID goid,
		const bool is_root_layer,
		const bool is_additive,
		const float elapsed_time,
		const float layer_weight,
		const float(&weights)[MAX_BONES],
		const bool(&masks)[MAX_BONES],
		const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
		const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters,
		const std::unordered_map<std::string, int>* node_index,
		const std::vector<int>& enable_nodes,
		std::vector<KeyData>& dst_data
	)
	{
		// �X�e�[�g�}�V�����ŊǗ�����u���b�N���X�V����
		UpdateBlock(goid, is_root_layer, is_additive, false, false, false, elapsed_time, 0.0f, blocks, parameters, node_index);
		// Root�X�e�[�g�}�V�����Ԃ�KeyData��dst�ɓK�p���鏈��
		ApplyRootKeyData(is_root_layer, is_additive, layer_weight, weights, masks, enable_nodes, dst_data);
	}

	bool AnimatorStateMachine::UpdateBlock(
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
		// �t���[��������
		m_current_valid_update = false;
		m_next_valid_update = false;
		m_on_transition_frame = false;

		// �X�e�[�g�}�V���̊J�n��
		if (m_state_machine_state == StateMachineState::Entry)
		{
			// Entry�u���b�N�̃C���f�b�N�X��T������
			float offset = 0.0f;
			m_current_block_index = CheckEntryTransition(parameters, blocks, offset);
			// �쐬���Ȃǂɂ�Entry�����ݒ�̂���
			if (m_current_block_index == INVALID_BLOCK_INDEX)
			{
				return false;
			}
			m_current_normalize_time = offset;
			m_state_machine_state = StateMachineState::Play;
			// Entry�u���b�N��Enter���Ă�
			blocks.at(m_current_block_index)->ExecuteEnterFunction(goid);
		}

		// ���݂̃u���b�N�ɂ�鎞�Ԃ̍X�V
		// State��Play�����̃u���b�N���ʂ̃X�e�[�g�}�V�������̏ꍇ
		if (m_state_machine_state == StateMachineState::Play || m_next_block_index == INVALID_BLOCK_INDEX)
		{
			// current
			float weighted_norm_time, norm_time;
			blocks.at(m_current_block_index)->GetNormalizeTime(elapsed_time, blocks, parameters, weighted_norm_time, norm_time);
			m_current_normalize_time += weighted_norm_time;
		}
		else if (
			m_state_machine_state == StateMachineState::Transition ||
			m_state_machine_state == StateMachineState::InterruptTransition
			)
		{
			float weighted_norm_time, norm_time;
			blocks.at(m_current_block_index)->GetNormalizeTime(elapsed_time, blocks, parameters, weighted_norm_time, norm_time);
			// current
			if (m_state_machine_state != StateMachineState::InterruptTransition)
			{
				// ���荞�ݑJ�ڒ��͌��̃��[�V�����͓����Ȃ�
				m_current_normalize_time += weighted_norm_time;
			}
			// next
			{
				float weighted_next_norm_time, next_norm_time;
				blocks.at(m_next_block_index)->GetNormalizeTime(elapsed_time, blocks, parameters, weighted_next_norm_time, next_norm_time);
				m_next_normalize_time += weighted_next_norm_time;
			}
			// duration
			m_duration_normalize_timer -= norm_time;

			// �J�ڊ���������current�u���b�N���ړ�����
			if (m_duration_normalize_timer <= 0.0f)
			{
				// current�u���b�N��Exit���Ă�
				blocks.at(m_current_block_index)->ExecuteExitFunction(goid);
				// next�̏���current�ɃR�s�[����
				m_state_machine_state = StateMachineState::Play;
				m_current_block_index = m_next_block_index;
				m_next_block_index = INVALID_BLOCK_INDEX;
				m_interrupted_next_block_index = INVALID_BLOCK_INDEX;
				m_current_normalize_time = m_next_normalize_time;
				m_duration_normalize_timer = 0.0f;
			}
		}
		// �u���b�N�̍X�V
		// State��Play�����̃u���b�N���ʂ̃X�e�[�g�}�V�������̏ꍇ
		if (m_state_machine_state == StateMachineState::Play || m_next_block_index == INVALID_BLOCK_INDEX)
		{
			// current
			m_current_valid_update = blocks.at(m_current_block_index)->UpdateBlock(goid, is_root_layer, is_additive, true, true, false, elapsed_time, m_current_normalize_time, blocks, parameters, node_index);
			m_current_valid_update |= blocks.at(m_current_block_index)->UpdateKeyFrame(is_root_layer, is_additive, m_current_normalize_time, blocks, node_index, m_keyframe);
			// �X�V���đJ�ڏ�ԂɂȂ�����
			if (m_state_machine_state == StateMachineState::Transition)
			{
				m_on_transition_frame = true;
			}
		}
		else if (m_state_machine_state == StateMachineState::Transition)
		{
			// current
			m_current_valid_update = blocks.at(m_current_block_index)->UpdateBlock(goid, is_root_layer, is_additive, true, true, true, elapsed_time, m_current_normalize_time, blocks, parameters, node_index);
			// ���荞�ݑJ�ڂ��������Ă�����u�����h���鑊���ύX����
			if (m_state_machine_state == StateMachineState::InterruptTransition)
			{
				// current�ƌ��̑J�ڐ��Transition�`�F�b�N�̂ݍs���A���荞�ݑJ�ڐ�̓��[�V�����̍X�V�̂ݍs��
				m_current_valid_update |= blocks.at(m_interrupted_next_block_index)->UpdateBlock(goid, is_root_layer, is_additive, true, false, true, elapsed_time, m_current_normalize_time, blocks, parameters, node_index);
				// next
				m_next_valid_update = blocks.at(m_next_block_index)->UpdateBlock(goid, is_root_layer, is_additive, false, false, true, elapsed_time, m_next_normalize_time, blocks, parameters, node_index);
				m_next_valid_update |= blocks.at(m_next_block_index)->UpdateKeyFrame(is_root_layer, is_additive, m_next_normalize_time, blocks, node_index, m_use_calc_keyframes[1]);
			}
			else
			{
				// current�̃��[�V�����X�V�Anext��Transition�`�F�b�N�ƃ��[�V�����X�V���s��
				m_current_valid_update |= blocks.at(m_current_block_index)->UpdateKeyFrame(is_root_layer, is_additive, m_current_normalize_time, blocks, node_index, m_use_calc_keyframes[0]);
				// next
				m_next_valid_update = blocks.at(m_next_block_index)->UpdateBlock(goid, is_root_layer, is_additive, true, false, true, elapsed_time, m_next_normalize_time, blocks, parameters, node_index);
				m_next_valid_update |= blocks.at(m_next_block_index)->UpdateKeyFrame(is_root_layer, is_additive, m_next_normalize_time, blocks, node_index, m_use_calc_keyframes[1]);
			}
		}
		else if (m_state_machine_state == StateMachineState::InterruptTransition)
		{
			// [0]�Ɋ��荞�ݎ��̏�񂪑��݂��邽�߁A�L���ɂ���
			m_current_valid_update = true;
			// current-current
			blocks.at(m_current_block_index)->UpdateBlock(goid, is_root_layer, is_additive, true, true, true, elapsed_time, m_current_normalize_time, blocks, parameters, node_index);
			// current-next
			blocks.at(m_interrupted_next_block_index)->UpdateBlock(goid, is_root_layer, is_additive, true, false, true, elapsed_time, m_current_normalize_time, blocks, parameters, node_index);
			// next
			m_next_valid_update = blocks.at(m_next_block_index)->UpdateBlock(goid, is_root_layer, is_additive, false, false, true, elapsed_time, m_next_normalize_time, blocks, parameters, node_index);
			m_next_valid_update |= blocks.at(m_next_block_index)->UpdateKeyFrame(is_root_layer, is_additive, m_next_normalize_time, blocks, node_index, m_use_calc_keyframes[1]);
		}

		return m_current_valid_update || m_next_valid_update;
	}

	bool AnimatorStateMachine::UpdateKeyFrame(
		const bool is_root_layer,
		const bool is_additive,
		const float normalize_time,
		const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
		const std::unordered_map<std::string, int>* node_index,
		std::vector<KeyData>& dst_keydata
	)
	{
		// ���g�̌��݂̏�Ԃ��R�s�[���ĕԂ�
		std::copy(m_keyframe.begin(), m_keyframe.end(), dst_keydata.begin());

		return false;
	}

	void AnimatorStateMachine::OnReady(const float offset)
	{
		m_state_machine_state = StateMachineState::Entry;
	}

	void AnimatorStateMachine::OnRemoveBlock(int removed_index, std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks)
	{
		// �擪�v�f�������Ă����āA��ɂȂ�����I��
		{
			// State or BlendTree
			while (!m_states_and_trees.empty())
			{
				const int index = *m_states_and_trees.begin();
				blocks.at(index)->OnRemoveBlock(index, blocks);
			}
			// StateMachine
			while (!m_statemachines.empty())
			{
				const int index = *m_statemachines.begin();
				blocks.at(index)->OnRemoveBlock(index, blocks);
			}
		}
		// ���g�̃C���f�b�N�X���Ǘ�����X�e�[�g�}�V������폜
		std::static_pointer_cast<AnimatorStateMachine>(blocks.at(m_owner_statemachine_index))->RemoveChildStateMachine(removed_index);
		// �ێ����Ă���C���f�b�N�X��Transition�̐�̃C���f�b�N�X���C���Aremoved_index�̍폜������
		OnRemoveBlockAllFixAndErase(removed_index, blocks);
	}

	void AnimatorStateMachine::GetNormalizeTime(
		const float elapsed_time,
		const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
		const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters,
		float& weighted_normalize_time,
		float& normalize_time
	)
	{
		// �X�e�[�g�}�V���̃G���g���[�ɃX�e�[�g�}�V�����Z�b�g����Ă���ꍇ��Update����ɌĂ΂�邽�߁A���p
		if (m_state_machine_state == StateMachineState::Entry)
		{
			weighted_normalize_time = 0.0f;
			normalize_time = 0.0f;
			return;
		}

		blocks.at(m_current_block_index)->GetNormalizeTime(elapsed_time, blocks, parameters, weighted_normalize_time, normalize_time);
	}

	void AnimatorStateMachine::OnRemoveBlockFixIndex(const int removed_index, const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks)
	{
		// Block���ʂ̏���
		AnimatorBlockBase::OnRemoveBlockFixIndex(removed_index, blocks);

		// Entry�̃C���f�b�N�X�̏C��
		if (m_entry_index == removed_index)
		{
			m_entry_index = INVALID_BLOCK_INDEX;
		}
		else if (m_entry_index > removed_index)
		{
			--m_entry_index;
		}
		// ���s���̃C���f�b�N�X�̏C��
		if (m_current_block_index == removed_index)
		{
			// �Đ����̃u���b�N���Ȃ��Ȃ�
			m_state_machine_state = StateMachineState::Entry;
			m_current_block_index = INVALID_BLOCK_INDEX;
		}
		else if (m_current_block_index > removed_index)
		{
			--m_current_block_index;
		}
		// �J�ڒ��Ȃ�J�ڐ�C���f�b�N�X�̏C��
		if (m_state_machine_state == StateMachineState::Transition)
		{
			if (m_next_block_index == removed_index)
			{
				// �J�ڐ�̃u���b�N���Ȃ��Ȃ�
				m_state_machine_state = StateMachineState::Play;
				m_next_block_index = INVALID_BLOCK_INDEX;
			}
			else if (m_next_block_index > removed_index)
			{
				--m_next_block_index;
			}
		}

		// Entry Transition�̑J�ڐ���m�F�A�C��
		{
			for (size_t i = 0; i < m_entry_transitions.size();)
			{
				const int destination_block_index = m_entry_transitions.at(i)->GetDestinationBlock();

				// �폜���ꂽ�u���b�N���Q�Ƃ��Ă���ꍇ�͍폜����
				if (destination_block_index == removed_index)
				{
					m_entry_transitions.erase(m_entry_transitions.begin() + i);
					continue;
				}
				// �폜���ꂽ�u���b�N���傫���Ƃ��̓f�N�������g����
				else if (destination_block_index > removed_index)
				{
					m_entry_transitions.at(i)->SetDestinationBlock(destination_block_index - 1);
				}
				++i;
			}
		}

		// �폜����u���b�N�ԍ����傫���Ȃ�set�̒��̒l���C������
		// State and BlendTree
		const std::unordered_set<int> buf_state = std::move(m_states_and_trees);
		m_states_and_trees.clear();
		for (const auto index : buf_state)
		{
			if (index > removed_index)
			{
				m_states_and_trees.emplace(index - 1);
			}
			else
			{
				m_states_and_trees.emplace(index);
			}
		}
		// StateMachine
		const std::unordered_set<int> buf_sm = std::move(m_statemachines);
		m_statemachines.clear();
		for (const auto index : buf_sm)
		{
			if (index > removed_index)
			{
				m_statemachines.emplace(index - 1);
			}
			else
			{
				m_statemachines.emplace(index);
			}
		}
	}

	void AnimatorStateMachine::ApplyRootKeyData(
		const bool is_root_layer,
		const bool is_additive,
		const float layer_weight,
		const float(&weights)[MAX_BONES],
		const bool(&masks)[MAX_BONES],
		const std::vector<int>& enable_nodes,
		std::vector<KeyData>& dst_data
	)
	{
		// Play���������͂��傤�ǑJ�ڂ����t���[��
		if (m_state_machine_state == StateMachineState::Play || m_on_transition_frame)
		{
			// KeyData�̎擾�Ɏ��s������Ȃ�Key�Ȃ�
			if (!m_current_valid_update)
			{
				return;
			}
			// dst_data�ɓK�p
			OnApplyRootData(is_root_layer, is_additive, layer_weight, weights, masks, enable_nodes, dst_data);
		}
		// �J�ڎ�
		else if (m_state_machine_state == StateMachineState::Transition || m_state_machine_state == StateMachineState::InterruptTransition)
		{
			// KeyData�̎擾�Ɏ��s������Ȃ�Key�Ȃ�
			if (!m_current_valid_update && !m_next_valid_update)
			{
				return;
			}

			// �J�ڊ�������u�����h�����v�Z
			float blend_ratio = 1.0f - (m_duration_normalize_timer * MyMath::InvertDivisionValue(m_duration_start_time));
			blend_ratio = MyMath::Clamp(blend_ratio, 0.0f, 1.0f);

			// �ǂ�����擾����
			if (m_current_valid_update && m_next_valid_update)
			{
				// �J�ڌ��ƑJ�ڐ�̃u�����h
				BlendKeyFrame(m_use_calc_keyframes[0], m_use_calc_keyframes[1], blend_ratio, m_keyframe);
				// dst_data�ɓK�p
				OnApplyRootData(is_root_layer, is_additive, layer_weight, weights, masks, enable_nodes, dst_data);
				return;
			}

			// Current�̎擾�̂ݐ���
			if (!m_next_valid_update)
			{
				// Current��KeyData[0]���R�s�[
				std::copy(m_use_calc_keyframes[0].begin(), m_use_calc_keyframes[0].end(), m_keyframe.begin());
				// dst_data�ƃu�����h
				OnApplyRootData(is_root_layer, is_additive, (1.0f - blend_ratio) * layer_weight, weights, masks, enable_nodes, dst_data);
			}
			// Next�̎擾�̂ݐ���
			else if (!m_current_valid_update)
			{
				// Next��KeyData[1]���R�s�[
				std::copy(m_use_calc_keyframes[1].begin(), m_use_calc_keyframes[1].end(), m_keyframe.begin());
				// dst_data�ƃu�����h
				OnApplyRootData(is_root_layer, is_additive, blend_ratio * layer_weight, weights, masks, enable_nodes, dst_data);
			}
		}
	}

	void AnimatorStateMachine::OnApplyRootData(
		const bool is_root_layer,
		const bool is_additive,
		const float layer_weight,
		const float(&weights)[MAX_BONES],
		const bool(&masks)[MAX_BONES],
		const std::vector<int>& enable_nodes,
		std::vector<KeyData>& dst_data
	)
	{
		// �擾�����L�[�f�[�^�����ݎ����Ă���L�[�f�[�^�z��ɓK�p���Ă���
		const size_t data_size = enable_nodes.size();

		// Root���C���[�Ȃ�R�s�[
		if (is_root_layer)
		{
			for (size_t i = 0; i < data_size; ++i)
			{
				const int data_index = enable_nodes.at(i);

				// �X�L�����Q�Ƃ��Ȃ��{�[���Ȃ疳��
				if (data_index == -1)
					continue;
				// �L�[�f�[�^��������
				dst_data.at(data_index) = m_keyframe.at(i);
			}
		}
		// Mask���C���[
		else
		{
			for (size_t i = 0; i < data_size; ++i)
			{
				const int data_index = enable_nodes.at(i);

				// �X�L�����Q�Ƃ��Ȃ��{�[���Ȃ疳��
				if (data_index == -1)
					continue;
				// �}�X�N��false�Ȃ疳��
				if (!masks[data_index])
					continue;

				// �u�����h�d��
				const float weight = layer_weight * weights[data_index];

				auto& dst_key = dst_data.at(data_index);
				auto& src_key = m_keyframe.at(i);
				// Additive
				if (is_additive)
				{
					// �d�ݕ������Z
					// TODO : ���s�ړ����ǂ̂悤�ɉ��Z���邩
					//dst_key.translate += VECTOR3::Lerp(VECTOR3::Zero, src_key.translate, weight);
					// TODO : �X�P�[���ɂ�weight���ǂ̂悤�ɂ����邩
					//dst_key.scale += VECTOR3::Lerp(VECTOR3::Zero, src_key.scale - VECTOR3::One, weight);

					dst_key.rotate = Quaternion::Slerp(Quaternion::Identity, src_key.rotate, weight) * dst_key.rotate;
				}
				// Override
				else
				{
					// �d�݂ɂ��u�����h
					dst_key.translate = VECTOR3::Lerp(dst_key.translate, src_key.translate, weight);
					dst_key.scale = VECTOR3::Lerp(dst_key.scale, src_key.scale, weight);
					dst_key.rotate = Quaternion::Slerp(dst_key.rotate, src_key.rotate, weight);
				}
			}
		}
	}

}// namespace TKGEngine::Animations
