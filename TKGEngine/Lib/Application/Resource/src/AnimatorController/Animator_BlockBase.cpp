
#include "Animator_BlockBase.h"

#include "Animator_Clip.h"
#include "Animator_StateMachine.h"

#include "Managers/MonoBehaviourManager.h"

#include <algorithm>

namespace TKGEngine::Animations
{
	AnimatorBlockBase::AnimatorBlockBase(BlockType type)
		: m_block_type(type)
	{
		/* nothing */
	}

	AnimatorBlockBase::AnimatorBlockBase(BlockType type, const char* name)
		: m_block_type(type)
	{
		m_name = name;
	}

#ifdef USE_IMGUI
	int AnimatorBlockBase::OnGUI(
		const GameObjectID goid,
		const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
		std::unordered_map<std::string, std::shared_ptr<Animations::AnimatorControllerParameter>>& parameters
	)
	{
		// 継承先のOnGUI内で呼び出す

		// 名前
		ImGui::Text("Name:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		std::string str = GetName();
		if (ImGui::InputText("##Animator Block name", &str, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			SetName(str);
		}

		return INVALID_BLOCK_INDEX;
	}
	void AnimatorBlockBase::OnGUITransition(const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks, std::unordered_map<std::string, std::shared_ptr<Animations::AnimatorControllerParameter>>& parameters)
	{
		// StateMachineは遷移を持たない
		if (m_block_type == BlockType::StateMachine)
			return;

		constexpr ImGuiTreeNodeFlags tree_flags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
		if (ImGui::TreeNodeEx("Transitions##AnimatorBlockBase", tree_flags))
		{
			ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
			// 遷移条件管理用タブ
			constexpr ImGuiTabBarFlags tab_bar_flags =
				ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_FittingPolicyResizeDown;
			if (ImGui::BeginTabBar("Transition Bar", tab_bar_flags))
			{
				// 追加用のボタン
				{
					if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
					{
						ImGui::OpenPopup("Add Transition popup");
					}
					if (ImGui::BeginPopup("Add Transition popup"))
					{
						// 遷移先の選択をしてからTransition追加
						{
							// 自身と同階層のブロックへの遷移
							if (ImGui::BeginMenu("Current StateMachine##Transition"))
							{
								// 自身を管理するステートマシンが管理する全てのブロックを取得
								std::list<int> manage_block_list;
								std::static_pointer_cast<AnimatorStateMachine>(blocks.at(m_owner_statemachine_index))->GetManageBlockList(manage_block_list);
								// 順に表示
								for (const auto index : manage_block_list)
								{
									// 自身への遷移は無効
									if (index == m_self_block_index)
										continue;

									ImGui::IDWrapped id(index);
									if (ImGui::Selectable(blocks.at(index)->GetName()))
									{
										AddTransition(index);
									}
								}
								ImGui::EndMenu();
							}
							// 自身の一つ上の階層のブロックへの遷移
							if (m_owner_statemachine_index != ROOT_STATEMACHINE_BLOCK_INDEX)
							{
								if (ImGui::BeginMenu("Upper StateMachine##Transition"))
								{
									// 自身を管理するステートマシンを管理するステートマシンが管理する全てのブロックを取得
									std::list<int> manage_block_list;
									const int owner_owner_index = blocks.at(m_owner_statemachine_index)->GetOwnerStateMachineIndex();
									std::static_pointer_cast<AnimatorStateMachine>(blocks.at(owner_owner_index))->GetManageBlockList(manage_block_list);
									// 順に表示
									for (const auto index : manage_block_list)
									{
										ImGui::IDWrapped id(index);
										if (ImGui::Selectable(blocks.at(index)->GetName()))
										{
											AddTransition(index);
										}
									}
									ImGui::EndMenu();
								}
							}
						}
						ImGui::EndPopup();
					}
				}
				// タブ表示
				constexpr ImGuiTabItemFlags tab_item_flags = ImGuiTabItemFlags_None;
				for (size_t i = 0; i < m_transitions.size(); ++i)
				{
					ImGui::IDWrapped id(i);
					bool is_open = true;
					const int dst_index = m_transitions.at(i)->GetDestinationBlock();
					if (ImGui::BeginTabItem(blocks.at(dst_index)->GetName(), &is_open, tab_item_flags))
					{
						// Transitionの順番入れ替え
						if (ImGui::Button("Prev"))
						{
							if (i > 0)
							{
								std::iter_swap(m_transitions.begin() + i - 1, m_transitions.begin() + i);
							}
						}
						ImGui::SameLine();
						if (ImGui::Button("Next"))
						{
							if (i < m_transitions.size() - 1)
							{
								std::iter_swap(m_transitions.begin() + i, m_transitions.begin() + i + 1);
							}
						}

						// 各TransitionごとのGUI
						m_transitions.at(i)->OnGUI(parameters, m_block_type == BlockType::StateMachine ? false : true);

						ImGui::EndTabItem();
					}
					// タブを閉じたときは、Transitionを削除する
					if (!is_open)
					{
						RemoveTransition(i);
					}
				}
				ImGui::EndTabBar();
			}
			ImGui::TreePop();
		}
	}
	void AnimatorBlockBase::OnGUIEventFunction(const GameObjectID goid)
	{
		constexpr ImGuiTreeNodeFlags tree_flags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
		if (ImGui::TreeNodeEx("State Event Function", tree_flags))
		{
			ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
			// Enter
			{
				ImGui::Text("Enter");
				ImGui::Checkbox("##Call Entry Func", &m_call_enter_function);
				if (m_call_enter_function)
				{
					// Class
					ImGui::AlignedSameLine(0.8f);
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.4f);
					if (ImGui::BeginCombo("##Select Enter Class", m_enter_class_name.c_str()))
					{
						// GameObjectにアタッチされたComponent名リストを取得する
						const auto& comp_names = ComponentManager::GetComponentNameList(goid);
						const auto itr_end = comp_names.end();
						for (auto itr = comp_names.begin(); itr != itr_end; ++itr)
						{
							if (ImGui::Selectable(itr->c_str(), (*itr) == m_enter_class_name ? true : false))
							{
								m_enter_class_name = *itr;
								m_enter_func_name.clear();
							}
						}
						ImGui::EndCombo();
					}
					// Func
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
					if (ImGui::BeginCombo("##Select Enter Func", m_enter_func_name.c_str()))
					{
						std::list<std::string> func_names;
						MonoBehaviourManager::GetFunctionVoidNames(m_enter_class_name, func_names);
						for (auto& func_name : func_names)
						{
							// 選択された関数名とパラメータの型をセットする
							if (ImGui::Selectable(func_name.c_str(), func_name == m_enter_func_name ? true : false))
							{
								m_enter_func_name = func_name;
							}
						}
						ImGui::EndCombo();
					}
				}
			}
			// Exit
			{
				ImGui::Text("Exit");
				ImGui::Checkbox("##Call Exit Func", &m_call_exit_function);
				if (m_call_exit_function)
				{
					// Class
					ImGui::AlignedSameLine(0.8f);
					ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.4f);
					if (ImGui::BeginCombo("##Select Exit Class", m_exit_class_name.c_str()))
					{
						// GameObjectにアタッチされたComponent名リストを取得する
						const auto& comp_names = ComponentManager::GetComponentNameList(goid);
						const auto itr_end = comp_names.end();
						for (auto itr = comp_names.begin(); itr != itr_end; ++itr)
						{
							if (ImGui::Selectable(itr->c_str(), (*itr) == m_exit_class_name ? true : false))
							{
								m_exit_class_name = *itr;
								m_exit_func_name.clear();
							}
						}
						ImGui::EndCombo();
					}
					// Func
					ImGui::SameLine();
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
					if (ImGui::BeginCombo("##Select Exit Func", m_exit_func_name.c_str()))
					{
						std::list<std::string> func_names;
						MonoBehaviourManager::GetFunctionVoidNames(m_exit_class_name, func_names);
						for (auto& func_name : func_names)
						{
							// 選択された関数名とパラメータの型をセットする
							if (ImGui::Selectable(func_name.c_str(), func_name == m_exit_func_name ? true : false))
							{
								m_exit_func_name = func_name;
							}
						}
						ImGui::EndCombo();
					}
				}
			}
			ImGui::TreePop();
		}
	}
#endif// #ifdef USE_IMGUI

	void AnimatorBlockBase::Initialize(const int self_index, const int owner_statemachine_index)
	{
		m_self_block_index = self_index;
		m_owner_statemachine_index = owner_statemachine_index;
	}

	int AnimatorBlockBase::GetBlockIndex() const
	{
		return m_self_block_index;
	}

	int AnimatorBlockBase::GetOwnerStateMachineIndex() const
	{
		return m_owner_statemachine_index;
	}

	std::shared_ptr<AnimatorTransition> AnimatorBlockBase::AddTransition(const int destination_block_idx)
	{
		// StateMachineは遷移を持たない
		if (m_block_type == BlockType::StateMachine)
			return std::shared_ptr<AnimatorTransition>();

		// 自身への遷移は無効
		if (destination_block_idx == m_self_block_index)
			return std::shared_ptr<AnimatorTransition>();

		std::shared_ptr<AnimatorTransition> tr = std::make_shared<AnimatorTransition>();
		m_transitions.emplace_back(tr);
		tr->SetDestinationBlock(destination_block_idx);
		return tr;
	}

	std::shared_ptr<AnimatorTransition> AnimatorBlockBase::AddTransition(const AnimatorTransition& transition)
	{
		// StateMachineは遷移を持たない
		if (m_block_type == BlockType::StateMachine)
			return std::shared_ptr<AnimatorTransition>();

		std::shared_ptr<AnimatorTransition> tr;
		*tr = transition;
		m_transitions.emplace_back(tr);
		return tr;
	}

	void AnimatorBlockBase::RemoveTransition(const int index)
	{
		const int vec_size = static_cast<int>(m_transitions.size());
		if (index >= vec_size)
			return;

		m_transitions.erase(m_transitions.begin() + index);
	}

	std::vector<std::shared_ptr<AnimatorTransition>>* AnimatorBlockBase::GetTransitions()
	{
		return &m_transitions;
	}

	const char* AnimatorBlockBase::GetName() const
	{
		return m_name.c_str();
	}

	void AnimatorBlockBase::SetName(const std::string& name)
	{
		m_name = name;
	}
	int AnimatorBlockBase::CheckTransition(
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
		// 遷移条件を満たすかすべて確認する
		for (auto&& transition : m_transitions)
		{
			const int next_block = transition->CheckTransition(
				is_current,
				state_transitioning,
				normalized_time,
				owner_index,
				parameters,
				blocks,
				duration,
				offset
			);
			// 遷移条件を満たしていないならループを継続
			if (next_block == INVALID_BLOCK_INDEX)
				continue;

			// 遷移条件を満たした
			return next_block;
		}

		return INVALID_BLOCK_INDEX;
	}

	void AnimatorBlockBase::ExecuteEnterFunction(GameObjectID goid) const
	{
		if (!m_call_enter_function)
			return;
		MonoBehaviourManager::ExecuteFunction(goid, m_enter_class_name, m_enter_func_name);
	}

	void AnimatorBlockBase::ExecuteExitFunction(GameObjectID goid) const
	{
		if (!m_call_exit_function)
			return;
		MonoBehaviourManager::ExecuteFunction(goid, m_exit_class_name, m_exit_func_name);
	}

	void AnimatorBlockBase::BlendKeyFrame(const std::vector<KeyData>& v1, const std::vector<KeyData>& v2, const float ratio, std::vector<KeyData>& dst)
	{
		const size_t v1_size = v1.size();
		const size_t v2_size = v2.size();
		assert(v1_size == v2_size);

		for (size_t i = 0; i < v1_size; ++i)
		{
			const auto& key_1 = v1.at(i);
			const auto& key_2 = v2.at(i);

			dst.at(i).translate = VECTOR3::Lerp(key_1.translate, key_2.translate, ratio);
			dst.at(i).scale = VECTOR3::Lerp(key_1.scale, key_2.scale, ratio);
			dst.at(i).rotate = Quaternion::Slerp(key_1.rotate, key_2.rotate, ratio);
		}
	}

	void AnimatorBlockBase::BlendKeyFrameAndAlign(
		const std::vector<KeyData>& v1,
		const std::vector<KeyData>& v2,
		const float ratio,
		const std::unordered_map<std::string, int>* key_index,
		const std::unordered_map<std::string, int>* node_index,
		std::vector<KeyData>& dst
	)
	{
		const size_t v1_size = v1.size();
		const size_t v2_size = v2.size();
		assert(v1_size == v2_size && "The number of elements does not match. AnimatorBlockBase::BlendKeyFrameAndAlign()");

		// Motion側のボーン名からAvatar側のボーンの順番に直していく
		const auto itr_end = node_index->end();
		const auto itr_find_end = key_index->end();
		for (auto itr = node_index->begin(); itr != itr_end; ++itr)
		{
			auto itr_find = key_index->find(itr->first);
			if (itr_find == itr_find_end)
				continue;

			const int dst_idx = itr->second;
			const int key_idx = itr_find->second;

			const auto& key_1 = v1.at(key_idx);
			const auto& key_2 = v2.at(key_idx);

			dst.at(dst_idx).translate = VECTOR3::Lerp(key_1.translate, key_2.translate, ratio);
			dst.at(dst_idx).scale = VECTOR3::Lerp(key_1.scale, key_2.scale, ratio);
			dst.at(dst_idx).rotate = Quaternion::Slerp(key_1.rotate, key_2.rotate, ratio);
		}
	}

	void AnimatorBlockBase::AlignKeyFrame(
		const std::vector<KeyData>& v,
		const std::unordered_map<std::string, int>* key_index,
		const std::unordered_map<std::string, int>* node_index,
		std::vector<KeyData>& dst
	)
	{
		// Motion側のボーン名からAvatar側のボーンの順番に直していく
		const auto itr_end = node_index->end();
		const auto itr_find_end = key_index->end();
		for (auto itr = node_index->begin(); itr != itr_end; ++itr)
		{
			auto itr_find = key_index->find(itr->first);
			if (itr_find == itr_find_end)
				continue;

			const int dst_idx = itr->second;
			const int key_idx = itr_find->second;

			dst.at(dst_idx).translate = v.at(key_idx).translate;
			dst.at(dst_idx).scale = v.at(key_idx).scale;
			dst.at(dst_idx).rotate = v.at(key_idx).rotate;
		}
	}

	void AnimatorBlockBase::GetFrameBlendKey(const AnimationClip& clip, const float key, const std::unordered_map<std::string, int>* node_index, std::vector<KeyData>& dst)
	{
		// キーをはさむフレームを取得
		const int frame_1 = static_cast<int>(key);
		const int frame_2 = frame_1 + 1;
		// ブレンドする割合
		const float blend_ratio = key - static_cast<float>(frame_1); // 0 ~ 1
		// ブレンドの実行
		const auto clip_1 = clip.GetKeyFrame(frame_1);
		const auto clip_2 = clip.GetKeyFrame(frame_2);
		BlendKeyFrameAndAlign(*clip_1, *clip_2, blend_ratio, clip.GetKeyIndex(), node_index, dst);
	}

	void AnimatorBlockBase::GetKeyDifference(
		const std::vector<KeyData>& v,
		const std::vector<KeyData>& clip,
		const std::unordered_map<std::string, int>* key_index,
		const std::unordered_map<std::string, int>* node_index,
		std::vector<KeyData>& dst
	)
	{
		// Motion側のボーン名からAvatar側のボーンの順番に直していく
		const auto itr_end = node_index->end();
		const auto itr_find_end = key_index->end();
		for (auto itr = node_index->begin(); itr != itr_end; ++itr)
		{
			auto itr_find = key_index->find(itr->first);
			if (itr_find == itr_find_end)
				continue;

			const int dst_idx = itr->second;
			const int key_idx = itr_find->second;

			// TODO : どのように差分を求めるか
			//dst.at(dst_idx).translate = v.at(dst_idx).translate - clip.at(key_idx).translate;
			//dst.at(dst_idx).scale = v.at(dst_idx).scale / clip.at(key_idx).scale;
			dst.at(dst_idx).rotate = v.at(dst_idx).rotate / clip.at(key_idx).rotate;
		}
	}

	void AnimatorBlockBase::OnRemoveBlockAllFixAndErase(const int removed_index, std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks)
	{
		// 全てのブロックにおいて、削除されるブロックのインデックスの影響を修正する
		for (const auto& block : blocks)
		{
			block->OnRemoveBlockFixIndex(removed_index, blocks);
		}
		// ブロックを削除する
		blocks.erase(blocks.begin() + removed_index);
	}

	void AnimatorBlockBase::OnRemoveBlockFixIndex(const int removed_index, const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks)
	{
		// 自身が削除されるブロックなら早期リターン
		if (m_self_block_index == removed_index)
			return;

		// Self index
		if (m_self_block_index > removed_index)
		{
			// 削除されたブロックより大きいときはデクリメントする
			m_self_block_index -= 1;
		}
		// Owner index
		if (m_owner_statemachine_index > removed_index)
		{
			// 削除されたブロックより大きいときはデクリメントする
			m_owner_statemachine_index -= 1;
		}
		// Transition
		for (size_t i = 0; i < m_transitions.size();)
		{
			const int destination_block_index = m_transitions.at(i)->GetDestinationBlock();

			// 削除されたブロックを参照している場合は削除する
			if (destination_block_index == removed_index)
			{
				m_transitions.erase(m_transitions.begin() + i);
				continue;
			}
			// 削除されたブロックより大きいときはデクリメントする
			else if (destination_block_index > removed_index)
			{
				m_transitions.at(i)->SetDestinationBlock(destination_block_index - 1);
			}
			++i;
		}
	}
}// namespace TKGEngine::Animations
