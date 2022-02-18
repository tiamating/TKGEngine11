
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

		// BlockBaseのOnGUI(ブロック名変更と表示)
		AnimatorBlockBase::OnGUI(goid, blocks, parameters);

		// Clip
		{
			ImGui::Text("Clip");
			ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
			{
				// モーションデータを既に持っているなら情報を表示
				if (m_clip.HasMotion())
				{
					if (ImGui::BeginChild("Motion Data", ImVec2(0.0f, ImGui::GetFontSize() * 6), true))
					{
						m_clip.OnGUI();
					}
					ImGui::EndChild();
				}
				// なければ未所持を強調
				else
				{
					ImGui::TextColored(ImGui::ERROR_FONT_COLOR, "No Clip!");
				}
				// Load用ボタン
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
				// 現在選択中のパラメータ名の存在チェック
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
						// Float以外は表示しない
						if(itr->second->type != AnimatorControllerParameter::Type::Float)
						{
							continue;
						}
						// 選択リストに表示
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
				// Event::GUIを呼ぶ
				for (size_t i = 0; i < m_events.size(); ++i)
				{
					ImGui::IDWrapped event_id(i);
					// 削除ボタン
					if (ImGui::ButtonColorSettable("X##Erase Button", ImVec4(0.75f, 0.2f, 0.2f, 1.0f), ImVec4(0.55f, 0.2f, 0.2f, 1.0f), ImVec4(0.4f, 0.05f, 0.05f, 1.0f)))
					{
						RemoveEvent(i);
						break;
					}
					// Event::GUI
					ImGui::SameLine();
					m_events.at(i).OnGUI(goid);
				}
				// Addボタン
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

	// Clipの取得や更新はないので戻り値はfalse
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
		// Transitionによる遷移チェック
		if (check_transition)
		{
			float duration = 0.0f;
			float offset = 0.0f;
			const int next = CheckTransition(is_current, state_transitioning, normalize_time, m_owner_statemachine_index, parameters, blocks, duration, offset);
			// 遷移条件を満たしている時
			if (next != INVALID_BLOCK_INDEX)
			{
				// 遷移先が所属するステートマシンの遷移データをセット
				const int next_block_owner_index = blocks.at(next)->GetOwnerStateMachineIndex();
				std::static_pointer_cast<AnimatorStateMachine>(blocks.at(next_block_owner_index))->SetNextData(goid, next, duration, offset, blocks);
			}
		}
		// イベントの発火チェック
		{
			const size_t event_num = m_events.size();
			if (event_num == 0)
				return false;

			// 0 ~ 1.0にまるめた正規化時間
			const float wrapped_time = normalize_time - static_cast<int>(normalize_time);
			const float prev_wrapped_time = m_prev_normalize_time - static_cast<int>(m_prev_normalize_time);

			// 1周しているなら全てのイベントを実行する
			const bool is_next_loop = (wrapped_time < prev_wrapped_time) ? true : false;
			// イベントごとにチェックしながら実行する
			for (size_t i = 0; i < event_num; ++i)
			{
				auto& event = m_events.at(i);
				// 実行済みフラグの確認
				if (event.IsExecutedEvent())
				{
					continue;
				}

				// 1周しているなら全てのイベントを実行する
				if (is_next_loop)
				{
					// 実行
					event.ExecuteEvent(goid);
				}
				// wrapされていないなら経過時間の確認をする
				else
				{
					if (event.CheckTime(wrapped_time))
					{
						// 実行
						event.ExecuteEvent(goid);
					}
				}
			}

			// 1周したらイベントフラグをリセットする
			if (is_next_loop)
			{
				for (auto& event : m_events)
				{
					event.ResetExecutedFlag();
				}
			}

			// wrap時に1.0が取れない問題を解決するために値を保持する
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
		// clipがなければ早期リターン
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
				// 偶数なら正方向
				if ((div % 2) == 0)
				{
					key_time = mod;
				}
				// 奇数は戻る方向
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

		// アニメーション時間の末端を再生中なら1つだけアニメーション行列を取得
		if (key_time >= length)
		{
			const int frame = static_cast<int>(length * rate);
			auto clip = m_clip.GetKeyFrame(frame);
			AlignKeyFrame(*clip, m_clip.GetKeyIndex(), node_index, dst_keydata);
		}
		// ある時間をはさむ2つのアニメーション行列をブレンドする
		else
		{
			// 何フレーム目のkeyか
			const float key = key_time * rate;
			GetFrameBlendKey(m_clip, key, node_index, dst_keydata);
		}

		// 加算レイヤーは差分を求める
		if (!is_root_layer && is_additive)
		{
			GetKeyDifference(dst_keydata, *m_clip.GetKeyFrame(0), m_clip.GetKeyIndex(), node_index, dst_keydata);
		}

		// keydataの取得ができた
		return true;
	}

	void AnimatorState::OnReady(const float offset)
	{
		// 直前の再生時間を初期化する
		m_prev_normalize_time = offset;
		// イベントフラグをすべてリセットする
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
		// パラメータでアニメーションの速さに乗算するものがあるか調べる
		float speed_param = 1.0f;
		if(m_use_speed_parameter)
		{
			// パラメータ名から検索
			const auto itr_find = parameters.find(m_speed_parameter);
			if(itr_find != parameters.end())
			{
				// Floatであることを確認
				if(itr_find->second->type == AnimatorControllerParameter::Type::Float)
				{
					speed_param = MyMath::Max(itr_find->second->default_float, 0.0f);
				}
			}
		}
		// Speedを乗算する
		speed_param *= m_speed;

		// Clipがない場合は実時間で返す
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
		// 自身のインデックスを管理するステートマシンから削除
		std::static_pointer_cast<AnimatorStateMachine>(blocks.at(m_owner_statemachine_index))->RemoveChildStateAndTree(removed_index);
		// 保持しているインデックスとTransitionの先のインデックスを修正、removed_indexの削除をする
		OnRemoveBlockAllFixAndErase(removed_index, blocks);
	}

}// namespace TKGEngine::Animations
