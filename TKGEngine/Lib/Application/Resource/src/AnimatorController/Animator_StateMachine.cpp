
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
		// ボーン数分の要素確保
		m_keyframe.resize(MAX_BONES);
		for (auto& calc_keyframe : m_use_calc_keyframes)
		{
			calc_keyframe.resize(MAX_BONES);
		}
	}

	AnimatorStateMachine::AnimatorStateMachine(const char* name)
		: AnimatorBlockBase(BlockType::StateMachine, name)
	{
		// ボーン数分の要素確保
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
		// ソート
		block_list.sort();
	}
	int AnimatorStateMachine::OnGUIStateMachine(
		const GameObjectID goid,
		const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
		std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters
	)
	{
		// 表示するブロックを選択する
		{
			// ImGuiのフィルター機能
			static ImGuiTextFilter im_filter;
			ImGui::Text("Filter :");
			ImGui::AlignedSameLine(0.5f);
			im_filter.Draw("##Show Block filter");
			ImGui::Text("Select Block Name :");
			ImGui::AlignedSameLine(0.5f);
			OnGUISelectBlock(im_filter, blocks, m_gui_current_select_block_index);
		}
		// 再生中のブロックを表示する
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
		// 選択中のブロックごとのGUI表示
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

		// 階層を移動した時に、次に表示するブロックのインデックス
		int next_state_machine_index = INVALID_BLOCK_INDEX;

		// Name
		if (m_self_block_index != ROOT_STATEMACHINE_BLOCK_INDEX)
		{
			// BlockBaseのOnGUI(ブロック名変更と表示)
			AnimatorBlockBase::OnGUI(goid, blocks, parameters);
		}
		else
		{
			// Rootは名前表示のみ
			ImGui::Text("Name:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			static std::string str = GetName();
			ImGui::InputText("##Animator Block name", &str, ImGuiInputTextFlags_ReadOnly);
		}
		// ステートマシンの階層を移動するボタン
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
		// ImGuiのフィルター機能
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
		// GUIによる選択
		bool do_select = false;
		if (ImGui::BeginCombo("##select block", current_index == INVALID_BLOCK_INDEX ? "" : blocks.at(current_index)->GetName()))
		{
			// このステートマシンが管理するブロックのインデックスをソートする
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
				// ソート
				manage_block_list.sort();
			}

			const auto itr_end = manage_block_list.end();
			for (auto itr = manage_block_list.begin(); itr != itr_end; ++itr)
			{
				const int index = *itr;
				const auto& block = blocks.at(index);
				// フィルターを通過するかチェック
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
			// 遷移条件管理用タブ
			constexpr ImGuiTabBarFlags tab_bar_flags =
				ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_FittingPolicyResizeDown;
			if (ImGui::BeginTabBar("Transition Bar", tab_bar_flags))
			{
				// 追加用のボタン
				{
					if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
					{
						ImGui::OpenPopup("Add Entry Transition popup");
					}
					if (ImGui::BeginPopup("Add Entry Transition popup"))
					{
						// 遷移先の選択をしてからTransition追加
						if (ImGui::BeginMenu("Current StateMachine##AnimatorStateMachine"))
						{
							// ImGuiのフィルター機能
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
				// タブ表示
				constexpr ImGuiTabItemFlags tab_item_flags = ImGuiTabItemFlags_None;
				for (size_t i = 0; i < m_entry_transitions.size(); ++i)
				{
					ImGui::IDWrapped entry_transition_id(i);
					bool is_open = true;
					if (ImGui::BeginTabItem(blocks.at(m_entry_transitions.at(i)->GetDestinationBlock())->GetName(), &is_open, tab_item_flags))
					{
						// 各TransitionごとのGUI
						m_entry_transitions.at(i)->OnGUI(parameters, false);

						ImGui::EndTabItem();
					}
					// タブを閉じたときは、Transitionを削除する
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
		// 最初に実行されるブロックに設定
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
		// Entry Transitionを順に確認していき、条件を満たしたら遷移
		float duration = 0.0f;	// 遷移時間はEntryにはない
		for (const auto& transition : m_entry_transitions)
		{
			// 遷移可能かチェックする
			const int entry_index = transition->CheckTransition(true, false, 0.0f, m_owner_statemachine_index, parameters, blocks, duration, offset);
			if (entry_index == INVALID_BLOCK_INDEX)
				continue;
			return entry_index;
		}
		// Entry Transitionで遷移しないならDefaultEntryIndexに遷移
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
		// 現在の遷移先と次の遷移先が同じならスキップ
		if (m_next_block_index == next)
			return;

		// 遷移中に遷移する場合
		if (m_state_machine_state == StateMachineState::Transition || m_state_machine_state == StateMachineState::InterruptTransition)
		{
			// 割り込み遷移前の遷移先情報を保持する
			if (m_state_machine_state == StateMachineState::Transition)
			{
				m_interrupted_next_block_index = m_next_block_index;
				// 割り込み遷移前の遷移先ブロックのExitを呼ぶ
				blocks.at(m_next_block_index)->ExecuteExitFunction(goid);
			}
			// 現在の遷移中KeyDataを保持して使用する
			std::copy(m_keyframe.begin(), m_keyframe.end(), m_use_calc_keyframes[0].begin());
			// 割り込み情報をセットする
			m_state_machine_state = StateMachineState::InterruptTransition;
		}
		else
		{
			// 遷移状態にする
			m_state_machine_state = StateMachineState::Transition;
		}
		// 遷移先のブロックを準備状態にする
		blocks.at(next)->OnReady(offset);
		// nextブロックのEnterを呼ぶ
		blocks.at(next)->ExecuteEnterFunction(goid);
		// 遷移先と遷移の情報をセット
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
		// ステートマシン内で管理するブロックを更新する
		UpdateBlock(goid, is_root_layer, is_additive, false, false, false, elapsed_time, 0.0f, blocks, parameters, node_index);
		// Rootステートマシンが返すKeyDataをdstに適用する処理
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
		// フレーム初期化
		m_current_valid_update = false;
		m_next_valid_update = false;
		m_on_transition_frame = false;

		// ステートマシンの開始時
		if (m_state_machine_state == StateMachineState::Entry)
		{
			// Entryブロックのインデックスを探索する
			float offset = 0.0f;
			m_current_block_index = CheckEntryTransition(parameters, blocks, offset);
			// 作成時などにはEntryが未設定のため
			if (m_current_block_index == INVALID_BLOCK_INDEX)
			{
				return false;
			}
			m_current_normalize_time = offset;
			m_state_machine_state = StateMachineState::Play;
			// EntryブロックのEnterを呼ぶ
			blocks.at(m_current_block_index)->ExecuteEnterFunction(goid);
		}

		// 現在のブロックによる時間の更新
		// StateがPlayか次のブロックが別のステートマシン所属の場合
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
				// 割り込み遷移中は元のモーションは動かない
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

			// 遷移完了したらcurrentブロックを移動する
			if (m_duration_normalize_timer <= 0.0f)
			{
				// currentブロックのExitを呼ぶ
				blocks.at(m_current_block_index)->ExecuteExitFunction(goid);
				// nextの情報をcurrentにコピーする
				m_state_machine_state = StateMachineState::Play;
				m_current_block_index = m_next_block_index;
				m_next_block_index = INVALID_BLOCK_INDEX;
				m_interrupted_next_block_index = INVALID_BLOCK_INDEX;
				m_current_normalize_time = m_next_normalize_time;
				m_duration_normalize_timer = 0.0f;
			}
		}
		// ブロックの更新
		// StateがPlayか次のブロックが別のステートマシン所属の場合
		if (m_state_machine_state == StateMachineState::Play || m_next_block_index == INVALID_BLOCK_INDEX)
		{
			// current
			m_current_valid_update = blocks.at(m_current_block_index)->UpdateBlock(goid, is_root_layer, is_additive, true, true, false, elapsed_time, m_current_normalize_time, blocks, parameters, node_index);
			m_current_valid_update |= blocks.at(m_current_block_index)->UpdateKeyFrame(is_root_layer, is_additive, m_current_normalize_time, blocks, node_index, m_keyframe);
			// 更新して遷移状態になったか
			if (m_state_machine_state == StateMachineState::Transition)
			{
				m_on_transition_frame = true;
			}
		}
		else if (m_state_machine_state == StateMachineState::Transition)
		{
			// current
			m_current_valid_update = blocks.at(m_current_block_index)->UpdateBlock(goid, is_root_layer, is_additive, true, true, true, elapsed_time, m_current_normalize_time, blocks, parameters, node_index);
			// 割り込み遷移が発生していたらブレンドする相手を変更する
			if (m_state_machine_state == StateMachineState::InterruptTransition)
			{
				// currentと元の遷移先のTransitionチェックのみ行い、割り込み遷移先はモーションの更新のみ行う
				m_current_valid_update |= blocks.at(m_interrupted_next_block_index)->UpdateBlock(goid, is_root_layer, is_additive, true, false, true, elapsed_time, m_current_normalize_time, blocks, parameters, node_index);
				// next
				m_next_valid_update = blocks.at(m_next_block_index)->UpdateBlock(goid, is_root_layer, is_additive, false, false, true, elapsed_time, m_next_normalize_time, blocks, parameters, node_index);
				m_next_valid_update |= blocks.at(m_next_block_index)->UpdateKeyFrame(is_root_layer, is_additive, m_next_normalize_time, blocks, node_index, m_use_calc_keyframes[1]);
			}
			else
			{
				// currentのモーション更新、nextのTransitionチェックとモーション更新を行う
				m_current_valid_update |= blocks.at(m_current_block_index)->UpdateKeyFrame(is_root_layer, is_additive, m_current_normalize_time, blocks, node_index, m_use_calc_keyframes[0]);
				// next
				m_next_valid_update = blocks.at(m_next_block_index)->UpdateBlock(goid, is_root_layer, is_additive, true, false, true, elapsed_time, m_next_normalize_time, blocks, parameters, node_index);
				m_next_valid_update |= blocks.at(m_next_block_index)->UpdateKeyFrame(is_root_layer, is_additive, m_next_normalize_time, blocks, node_index, m_use_calc_keyframes[1]);
			}
		}
		else if (m_state_machine_state == StateMachineState::InterruptTransition)
		{
			// [0]に割り込み時の情報が存在するため、有効にする
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
		// 自身の現在の状態をコピーして返す
		std::copy(m_keyframe.begin(), m_keyframe.end(), dst_keydata.begin());

		return false;
	}

	void AnimatorStateMachine::OnReady(const float offset)
	{
		m_state_machine_state = StateMachineState::Entry;
	}

	void AnimatorStateMachine::OnRemoveBlock(int removed_index, std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks)
	{
		// 先頭要素を消していって、空になったら終了
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
		// 自身のインデックスを管理するステートマシンから削除
		std::static_pointer_cast<AnimatorStateMachine>(blocks.at(m_owner_statemachine_index))->RemoveChildStateMachine(removed_index);
		// 保持しているインデックスとTransitionの先のインデックスを修正、removed_indexの削除をする
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
		// ステートマシンのエントリーにステートマシンがセットされている場合はUpdateより先に呼ばれるため、回避用
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
		// Block共通の処理
		AnimatorBlockBase::OnRemoveBlockFixIndex(removed_index, blocks);

		// Entryのインデックスの修正
		if (m_entry_index == removed_index)
		{
			m_entry_index = INVALID_BLOCK_INDEX;
		}
		else if (m_entry_index > removed_index)
		{
			--m_entry_index;
		}
		// 実行中のインデックスの修正
		if (m_current_block_index == removed_index)
		{
			// 再生中のブロックがなくなる
			m_state_machine_state = StateMachineState::Entry;
			m_current_block_index = INVALID_BLOCK_INDEX;
		}
		else if (m_current_block_index > removed_index)
		{
			--m_current_block_index;
		}
		// 遷移中なら遷移先インデックスの修正
		if (m_state_machine_state == StateMachineState::Transition)
		{
			if (m_next_block_index == removed_index)
			{
				// 遷移先のブロックがなくなる
				m_state_machine_state = StateMachineState::Play;
				m_next_block_index = INVALID_BLOCK_INDEX;
			}
			else if (m_next_block_index > removed_index)
			{
				--m_next_block_index;
			}
		}

		// Entry Transitionの遷移先を確認、修正
		{
			for (size_t i = 0; i < m_entry_transitions.size();)
			{
				const int destination_block_index = m_entry_transitions.at(i)->GetDestinationBlock();

				// 削除されたブロックを参照している場合は削除する
				if (destination_block_index == removed_index)
				{
					m_entry_transitions.erase(m_entry_transitions.begin() + i);
					continue;
				}
				// 削除されたブロックより大きいときはデクリメントする
				else if (destination_block_index > removed_index)
				{
					m_entry_transitions.at(i)->SetDestinationBlock(destination_block_index - 1);
				}
				++i;
			}
		}

		// 削除するブロック番号より大きいならsetの中の値を修正する
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
		// Play時もしくはちょうど遷移したフレーム
		if (m_state_machine_state == StateMachineState::Play || m_on_transition_frame)
		{
			// KeyDataの取得に失敗したらならKeyなし
			if (!m_current_valid_update)
			{
				return;
			}
			// dst_dataに適用
			OnApplyRootData(is_root_layer, is_additive, layer_weight, weights, masks, enable_nodes, dst_data);
		}
		// 遷移時
		else if (m_state_machine_state == StateMachineState::Transition || m_state_machine_state == StateMachineState::InterruptTransition)
		{
			// KeyDataの取得に失敗したらならKeyなし
			if (!m_current_valid_update && !m_next_valid_update)
			{
				return;
			}

			// 遷移割合からブレンド率を計算
			float blend_ratio = 1.0f - (m_duration_normalize_timer * MyMath::InvertDivisionValue(m_duration_start_time));
			blend_ratio = MyMath::Clamp(blend_ratio, 0.0f, 1.0f);

			// どちらも取得成功
			if (m_current_valid_update && m_next_valid_update)
			{
				// 遷移元と遷移先のブレンド
				BlendKeyFrame(m_use_calc_keyframes[0], m_use_calc_keyframes[1], blend_ratio, m_keyframe);
				// dst_dataに適用
				OnApplyRootData(is_root_layer, is_additive, layer_weight, weights, masks, enable_nodes, dst_data);
				return;
			}

			// Currentの取得のみ成功
			if (!m_next_valid_update)
			{
				// CurrentのKeyData[0]をコピー
				std::copy(m_use_calc_keyframes[0].begin(), m_use_calc_keyframes[0].end(), m_keyframe.begin());
				// dst_dataとブレンド
				OnApplyRootData(is_root_layer, is_additive, (1.0f - blend_ratio) * layer_weight, weights, masks, enable_nodes, dst_data);
			}
			// Nextの取得のみ成功
			else if (!m_current_valid_update)
			{
				// NextのKeyData[1]をコピー
				std::copy(m_use_calc_keyframes[1].begin(), m_use_calc_keyframes[1].end(), m_keyframe.begin());
				// dst_dataとブレンド
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
		// 取得したキーデータを現在持っているキーデータ配列に適用していく
		const size_t data_size = enable_nodes.size();

		// Rootレイヤーならコピー
		if (is_root_layer)
		{
			for (size_t i = 0; i < data_size; ++i)
			{
				const int data_index = enable_nodes.at(i);

				// スキンが参照しないボーンなら無視
				if (data_index == -1)
					continue;
				// キーデータを代入する
				dst_data.at(data_index) = m_keyframe.at(i);
			}
		}
		// Maskレイヤー
		else
		{
			for (size_t i = 0; i < data_size; ++i)
			{
				const int data_index = enable_nodes.at(i);

				// スキンが参照しないボーンなら無視
				if (data_index == -1)
					continue;
				// マスクがfalseなら無視
				if (!masks[data_index])
					continue;

				// ブレンド重み
				const float weight = layer_weight * weights[data_index];

				auto& dst_key = dst_data.at(data_index);
				auto& src_key = m_keyframe.at(i);
				// Additive
				if (is_additive)
				{
					// 重み分を加算
					// TODO : 平行移動をどのように加算するか
					//dst_key.translate += VECTOR3::Lerp(VECTOR3::Zero, src_key.translate, weight);
					// TODO : スケールにはweightをどのようにかけるか
					//dst_key.scale += VECTOR3::Lerp(VECTOR3::Zero, src_key.scale - VECTOR3::One, weight);

					dst_key.rotate = Quaternion::Slerp(Quaternion::Identity, src_key.rotate, weight) * dst_key.rotate;
				}
				// Override
				else
				{
					// 重みによるブレンド
					dst_key.translate = VECTOR3::Lerp(dst_key.translate, src_key.translate, weight);
					dst_key.scale = VECTOR3::Lerp(dst_key.scale, src_key.scale, weight);
					dst_key.rotate = Quaternion::Slerp(dst_key.rotate, src_key.rotate, weight);
				}
			}
		}
	}

}// namespace TKGEngine::Animations
