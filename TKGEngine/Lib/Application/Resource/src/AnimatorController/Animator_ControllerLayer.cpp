
#include "Animator_ControllerLayer.h"


namespace TKGEngine::Animations
{
	AnimatorControllerLayer::AnimatorControllerLayer()
	{
		// Rootステートマシンの作成
		m_blocks.emplace_back(std::make_shared<AnimatorStateMachine>("Root"));
		m_blocks.at(ROOT_STATEMACHINE_BLOCK_INDEX)->Initialize(ROOT_STATEMACHINE_BLOCK_INDEX, INVALID_BLOCK_INDEX);
		// Layer名
		m_layer_name = "Animator Layer";
	}

	AnimatorControllerLayer::AnimatorControllerLayer(const std::string& name)
	{
		// Rootステートマシンの作成
		m_blocks.emplace_back(std::make_shared<AnimatorStateMachine>("Root"));
		m_blocks.at(ROOT_STATEMACHINE_BLOCK_INDEX)->Initialize(ROOT_STATEMACHINE_BLOCK_INDEX, INVALID_BLOCK_INDEX);
		// Layer名
		m_layer_name = name;
	}

#ifdef USE_IMGUI
	void AnimatorControllerLayer::OnGUIStateMachine(const GameObjectID goid, std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters)
	{
		// Entryの設定
		std::static_pointer_cast<AnimatorStateMachine>(m_blocks.at(m_gui_current_select_state_machine_index))->OnGUIEntry(m_blocks, parameters);

		// Block管理タブバー表示
		constexpr ImGuiTabBarFlags tab_bar_flags =
			ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_FittingPolicyResizeDown;
		if (ImGui::BeginTabBar("BlockTabBar", tab_bar_flags))
		{
			// 自身の親ステートマシンの階層に移動するボタン
			if (m_gui_current_select_state_machine_index != ROOT_STATEMACHINE_BLOCK_INDEX)
			{
				if (ImGui::TabItemButton("<Owner StateMachine>", ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip))
				{
					m_gui_current_select_state_machine_index = m_blocks.at(m_gui_current_select_state_machine_index)->GetOwnerStateMachineIndex();
				}
			}
			// タブ表示
			constexpr ImGuiTabItemFlags tab_item_flags = ImGuiTabItemFlags_None;
			{
				ImGui::IDWrapped id(m_gui_current_select_state_machine_index);
				if (ImGui::BeginTabItem(m_blocks.at(m_gui_current_select_state_machine_index)->GetName(), nullptr, tab_item_flags))
				{
					// Addボタン
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.5f);
					if (ImGui::Button("Add Block##AnimatorLayer"))
					{
						ImGui::OpenPopup("Select Add Block popup");
					}
					if (ImGui::BeginPopup("Select Add Block popup"))
					{
						// State
						if (ImGui::Selectable("State##Add popup"))
						{
							AddState(m_gui_current_select_state_machine_index, "New State");
						}
						//// State Machine
						//if (ImGui::Selectable("StateMachine##Add popup"))
						//{
						//	AddStateMachine(m_gui_current_select_state_machine_index, "New StateMachine");
						//}
						// Blend Tree
						if (ImGui::Selectable("Blend Tree##Add popup"))
						{
							AddBlendTree(m_gui_current_select_state_machine_index, "New Tree");
						}

						ImGui::EndPopup();
					}
					// Removeボタン
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					if (ImGui::Button("Remove Block##AnimatorLayer"))
					{
						// 現在選択されているステートマシンの表示しているブロックのインデックスを削除
						int& current_select_index = std::static_pointer_cast<AnimatorStateMachine>(m_blocks.at(m_gui_current_select_state_machine_index))->m_gui_current_select_block_index;
						if (current_select_index != INVALID_BLOCK_INDEX)
						{
							RemoveBlock(current_select_index);
							// 消去したインデックスを参照しないように初期値を代入する
							current_select_index = INVALID_BLOCK_INDEX;
							m_gui_current_select_state_machine_index = ROOT_STATEMACHINE_BLOCK_INDEX;
						}
					}
					// 選択中ステートマシンのGUI表示
					const int select_sm_index = std::static_pointer_cast<AnimatorStateMachine>(m_blocks.at(m_gui_current_select_state_machine_index))->OnGUIStateMachine(goid, m_blocks, parameters);
					// 現在選択されているステートマシンのインデックスを更新する
					if (select_sm_index != INVALID_BLOCK_INDEX)
					{
						m_gui_current_select_state_machine_index = select_sm_index;
					}

					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
	}
	void AnimatorControllerLayer::OnGUIMask(const std::vector<std::string>* bone_names, std::unordered_map<std::string, std::shared_ptr<Animations::AnimatorControllerParameter>>& parameters)
	{
		// Blending Mode
		ImGui::Text("Blending Mode");
		ImGui::AlignedSameLine(0.5f);
		ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f);
		ImGui::ComboEnum<BlendingMode, BlendingMode::Max_BlendingMode>("##Select Blending mode", &m_blending_mode);
		// Weight Parameter
		{
			ImGui::Text("Weight Parameter");
			ImGui::AlignedSameLine(0.5f);
			ImGui::Checkbox("##Weight Parameter", &m_use_weight_parameter);
			if (m_use_weight_parameter)
			{
				// 現在選択中のパラメータ名の存在チェック
				const auto itr_find = parameters.find(m_weight_parameter);
				const auto itr_end = parameters.end();
				bool is_registered = true;
				if (itr_find == itr_end)
				{
					is_registered = false;
					m_weight_parameter.clear();
				}
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
				if (ImGui::BeginCombo("##Select Weight Parameter", is_registered ? m_weight_parameter.c_str() : ""))
				{
					for (auto itr = parameters.begin(); itr != itr_end; ++itr)
					{
						// Float以外は表示しない
						if (itr->second->type != AnimatorControllerParameter::Type::Float)
						{
							continue;
						}
						// 選択リストに表示
						ImGui::IDWrapped id(itr->second.get());
						if (ImGui::Selectable(itr->first.c_str()))
						{
							m_weight_parameter = itr->first;
						}
					}
					ImGui::EndCombo();
				}
			}
		}
		ImGui::Separator();

		// Avatar Mask
		m_avatar_mask.OnGUI(bone_names);
	}
#endif// #ifdef USE_IMGUI

	std::shared_ptr<AnimatorStateMachine> AnimatorControllerLayer::GetStateMachine()
	{
		return std::static_pointer_cast<AnimatorStateMachine>(m_blocks.at(ROOT_STATEMACHINE_BLOCK_INDEX));
	}

	std::shared_ptr<AnimatorState> AnimatorControllerLayer::AddMotion(const std::string& motion_filepath)
	{
		// 新しいStateの作成
		std::shared_ptr<AnimatorState> state = std::make_shared<AnimatorState>();
		const int new_block_index = static_cast<int>(m_blocks.size());
		state->Initialize(new_block_index, ROOT_STATEMACHINE_BLOCK_INDEX);
		state->AddClip(motion_filepath);
		state->SetName(state->GetMotionName());
		// Owner登録
		std::static_pointer_cast<AnimatorStateMachine>(m_blocks.at(state->GetOwnerStateMachineIndex()))->AddChildStateAndTree(new_block_index);
		// Blockリストに追加
		m_blocks.emplace_back(state);

		return state;
	}

	std::shared_ptr<AnimatorState> AnimatorControllerLayer::AddState(const std::string& name)
	{
		return AddState(ROOT_STATEMACHINE_BLOCK_INDEX, name);
	}

	std::shared_ptr<AnimatorState> AnimatorControllerLayer::AddState(int sm_index, const std::string& name)
	{
		// 新しいStateの作成
		std::shared_ptr<AnimatorState> state = std::make_shared<AnimatorState>(name.c_str());
		const int new_block_index = static_cast<int>(m_blocks.size());
		state->Initialize(new_block_index, sm_index);
		// Owner登録
		std::static_pointer_cast<AnimatorStateMachine>(m_blocks.at(state->GetOwnerStateMachineIndex()))->AddChildStateAndTree(new_block_index);
		// Blockリストに追加
		m_blocks.emplace_back(state);

		return state;
	}

	std::shared_ptr<BlendTree> AnimatorControllerLayer::AddBlendTree(const std::string& name)
	{
		return AddBlendTree(ROOT_STATEMACHINE_BLOCK_INDEX, name);
	}

	std::shared_ptr<BlendTree> AnimatorControllerLayer::AddBlendTree(int sm_index, const std::string& name)
	{
		// 新しいBlendTreeの作成
		std::shared_ptr<BlendTree> tree = std::make_shared<BlendTree>(name.c_str());
		const int new_block_index = static_cast<int>(m_blocks.size());
		tree->Initialize(new_block_index, sm_index);
		// Owner登録
		std::static_pointer_cast<AnimatorStateMachine>(m_blocks.at(tree->GetOwnerStateMachineIndex()))->AddChildStateAndTree(new_block_index);
		// Blockリストに追加
		m_blocks.emplace_back(tree);

		return tree;
	}

	//std::shared_ptr<AnimatorStateMachine> AnimatorControllerLayer::AddStateMachine(const std::string& name)
	//{
	//	return AddStateMachine(ROOT_STATEMACHINE_BLOCK_INDEX, name);
	//}

	//std::shared_ptr<AnimatorStateMachine> AnimatorControllerLayer::AddStateMachine(int sm_index, const std::string& name)
	//{
	//	// 新しいステートマシンの作成
	//	std::shared_ptr<AnimatorStateMachine> state_machine = std::make_shared<AnimatorStateMachine>(name.c_str());
	//	const int new_block_index = static_cast<int>(m_blocks.size());
	//	state_machine->Initialize(new_block_index, sm_index);
	//	// Owner登録
	//	std::static_pointer_cast<AnimatorStateMachine>(m_blocks.at(state_machine->GetOwnerStateMachineIndex()))->AddChildStateMachine(new_block_index);
	//	// Blockリストに追加
	//	m_blocks.emplace_back(state_machine);

	//	return state_machine;
	//}

	void AnimatorControllerLayer::RemoveBlock(const int index)
	{
		// Rootステートマシンは削除不可
		if (index == ROOT_STATEMACHINE_BLOCK_INDEX)
		{
			assert(0 && "failed AnimatorControllerLayer::RemoveBlock(). Root StateMachine(index 0) can not remove.");
			return;
		}
		// 範囲外チェック
		if (index >= static_cast<int>(m_blocks.size()))
		{
			assert(0 && "failed AnimatorControllerLayer::RemoveBlock(). out of block size.");
			return;
		}
		// 削除時の処理を実行する
		const auto& removed_block = m_blocks.at(index);
		removed_block->OnRemoveBlock(index, m_blocks);
	}

	void AnimatorControllerLayer::SetAvatarMask(const AvatarMask& mask)
	{
		m_avatar_mask = mask;
	}

	void AnimatorControllerLayer::SetBlendingMode(BlendingMode mode)
	{
		m_blending_mode = mode;
	}

	const char* AnimatorControllerLayer::GetName() const
	{
		return m_layer_name.c_str();
	}

	void AnimatorControllerLayer::SetName(const std::string& name)
	{
		m_layer_name = name;
	}

	void AnimatorControllerLayer::ApplyAnimation(
		const GameObjectID goid,
		const float elapsed_time,
		const bool is_root,
		const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters,
		const std::unordered_map<std::string, int>* node_index,
		const std::vector<int>& enable_nodes,
		std::vector<KeyData>& data
	)
	{
		// RootStateMachineしかブロックが存在しなければ更新しない
		if (m_blocks.size() <= 1)
			return;

		const auto& root_statemachine = std::static_pointer_cast<AnimatorStateMachine>(m_blocks.at(ROOT_STATEMACHINE_BLOCK_INDEX));

		// パラメータでアニメーションの速さに乗算するものがあるか調べる
		float weight_param = 1.0f;
		if (m_use_weight_parameter)
		{
			// パラメータ名から検索
			const auto itr_find = parameters.find(m_weight_parameter);
			if (itr_find != parameters.end())
			{
				// Floatであることを確認
				if (itr_find->second->type == AnimatorControllerParameter::Type::Float)
				{
					weight_param = MyMath::Clamp(itr_find->second->default_float, 0.0f, 1.0f);
				}
			}
		}
		// RootステートマシンをKeydataに適用する
		root_statemachine->ApplyKeyFrameData
		(
			goid,
			is_root,
			m_blending_mode == BlendingMode::Additive ? true : false,
			elapsed_time,
			weight_param,
			m_avatar_mask.GetWeights(),
			m_avatar_mask.GetMasks(),
			m_blocks,
			parameters,
			node_index,
			enable_nodes,
			data
		);
	}

}// namespace TKGEngine::Animations