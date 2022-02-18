
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
		// ボーン数分の要素確保
		for (auto& calc_keyframe : m_calc_child_keyframes)
		{
			calc_keyframe.resize(MAX_BONES);
		}
	}

	BlendTree::BlendTree(const char* name)
		: AnimatorBlockBase(BlockType::BlendTree, name)
	{
		// ボーン数分の要素確保
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

		// BlockBaseのOnGUI(ブロック名変更と表示)
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
					// Parameters.firstを羅列
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth() * 0.4f);
					if (ImGui::BeginCombo("##Select Parameter", m_parameter.c_str()))
					{
						const auto itr_end = parameters.end();
						for (auto itr = parameters.begin(); itr != itr_end; ++itr)
						{
							// パラメータがFloatのものだけ表示
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
							// パラメータがFloatのものだけ表示
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
							// パラメータがFloatのものだけ表示
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
							// パラメータがFloatのものだけ表示
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
							// パラメータがFloatのものだけ表示
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
							// パラメータがFloatのものだけ表示
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
						if (itr->second->type != AnimatorControllerParameter::Type::Float)
						{
							continue;
						}
						// 選択リストに表示
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
				// ChildClip::GUIを呼ぶ
				for (size_t i = 0; i < m_children.size(); ++i)
				{
					ImGui::IDWrapped child_id(static_cast<int>(i));
					// 削除ボタン
					if (ImGui::ButtonColorSettable("X##Remove ChildClip Button", ImVec4(0.75f, 0.2f, 0.2f, 1.0f), ImVec4(0.55f, 0.2f, 0.2f, 1.0f), ImVec4(0.4f, 0.05f, 0.05f, 1.0f)))
					{
						RemoveChild(i);
						break;
					}
					// ChildClip::GUI
					ImGui::SameLine();
					m_children.at(i).OnGUI(m_blend_tree_type, m_blend_data, i);
				}
				// Addボタン
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
				// Event::GUIを呼ぶ
				for (size_t i = 0; i < m_events.size(); ++i)
				{
					ImGui::IDWrapped event_id(static_cast<int>(i));
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
		// ブレンドに使用するデータ数の変更
		m_blend_data.resize(child_num);
		// 影響度バッファのサイズ変更
		m_influences.resize(child_num);
	}

	void BlendTree::RemoveChild(const int index)
	{
		if (index < 0 || index >= static_cast<int>(m_children.size()))
			return;
		m_children.erase(m_children.begin() + index);

		// ブレンドに使用するデータ数の変更
		m_blend_data.pop_back();
		// 影響度バッファのサイズ変更
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

	// Clipの取得や更新はないので戻り値はfalse
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

	bool BlendTree::UpdateKeyFrame(
		const bool is_root_layer,
		const bool is_additive,
		const float normalize_time,
		const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
		const std::unordered_map<std::string, int>* node_index,
		std::vector<KeyData>& dst_keydata
	)
	{
		// 参照インデックスのチェック
		if (m_blend_data_num == 0 || m_blend_data.at(0).child_index == INVALID_CHILD_INDEX)
		{
			// keydataの取得に失敗
			return false;
		}
		// WrapModeを考慮して0 ~ 1にwrapする
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

		// 1つしか参照していないとき
		if (m_blend_data_num == 1 || m_blend_data.at(1).child_index == INVALID_CHILD_INDEX)
		{
			const auto& clip = m_children.at(m_blend_data.at(0).child_index).clip;
			const float length = clip.GetLength();
			const float rate = clip.GetSampleRate();
			// アニメーション時間の末端時
			if (is_clamped)
			{
				const int frame = static_cast<int>(length * rate);
				auto clip_keyframe = clip.GetKeyFrame(frame);
				AlignKeyFrame(*clip_keyframe, clip.GetKeyIndex(), node_index, dst_keydata);
			}
			else
			{
				// 何フレーム目のkeyか
				const float key = wrapped_normalize_time * length * rate;
				GetFrameBlendKey(clip, key, node_index, dst_keydata);
			}

			// 加算レイヤーは差分を求める
			if (!is_root_layer && is_additive)
			{
				GetKeyDifference(dst_keydata, *clip.GetKeyFrame(0), clip.GetKeyIndex(), node_index, dst_keydata);
			}
		}
		// 2つ以上のブレンドのとき
		else
		{
			float total_weight = 0.0f;

			// 始めのデータを取得する
			{
				const auto& first_blend_data = m_blend_data.at(0);
				const auto& clip = m_children.at(first_blend_data.child_index).clip;
				const float length = clip.GetLength();
				const float rate = clip.GetSampleRate();
				// アニメーション時間の末端時
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

				// 加算レイヤーは差分を求める
				if (!is_root_layer && is_additive)
				{
					GetKeyDifference(m_calc_child_keyframes[0], *clip.GetKeyFrame(0), clip.GetKeyIndex(), node_index, m_calc_child_keyframes[0]);
				}

				// Weightを加算
				total_weight += first_blend_data.weight;
			}

			// BlendData.child_index == INVALID_CHILD_INDEXになるまでブレンドする
			int data_index = 1;
			while (true)
			{
				// インデックスの範囲と有効値チェック
				if (m_blend_data_num <= data_index || m_blend_data.at(data_index).child_index == INVALID_CHILD_INDEX)
				{
					break;
				}
				const auto& blend_data = m_blend_data.at(data_index);

				// data_indexに位置する情報を取得
				const auto& clip = m_children.at(blend_data.child_index).clip;
				const float length = clip.GetLength();
				const float rate = clip.GetSampleRate();
				// アニメーション時間の末端時
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

				// 加算レイヤーは差分を求める
				if (!is_root_layer && is_additive)
				{
					GetKeyDifference(m_calc_child_keyframes[1], *clip.GetKeyFrame(0), clip.GetKeyIndex(), node_index, m_calc_child_keyframes[1]);
				}

				// Weightを加算
				const float prev_total_weight = total_weight;
				total_weight += blend_data.weight;

				// 重みでブレンドする
				BlendKeyFrame(m_calc_child_keyframes[1], m_calc_child_keyframes[0], prev_total_weight / total_weight, m_calc_child_keyframes[0]);

				// 参照するインデックスを1つ先に進める
				++data_index;
			}
			std::copy(m_calc_child_keyframes[0].begin(), m_calc_child_keyframes[0].end(), dst_keydata.begin());
		}

		// keydataの取得ができた
		return true;
	}

	void BlendTree::OnReady(const float offset)
	{
		// 直前の再生時間を初期化する
		m_prev_normalize_time = offset;
		// イベントフラグをすべてリセットする
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
		// 前フレームの情報のクリア
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

		// Blend ratioの計算
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
		// 正規化経過時間の計算
		{
			// ブレンドするクリップが存在しない
			if (m_blend_data_num == 0 || m_blend_data.at(0).child_index == INVALID_CHILD_INDEX)
			{
				// childrenが存在しない
				weighted_normalize_time = 0.0f;
				normalize_time = 0.0f;
				return;
			}

			// パラメータでアニメーションの速さに乗算するものがあるか調べる
			float speed_param = 1.0f;
			if (m_use_speed_parameter)
			{
				// パラメータ名から検索
				const auto itr_find = parameters.find(m_speed_parameter);
				if (itr_find != parameters.end())
				{
					// Floatであることを確認
					if (itr_find->second->type == AnimatorControllerParameter::Type::Float)
					{
						speed_param = MyMath::Max(itr_find->second->default_float, 0.0f);
					}
				}
			}
			// Speedを乗算する
			speed_param *= m_speed;

			// 1つだけ使用する状態
			if (m_blend_data_num == 1 || m_blend_data.at(1).child_index == INVALID_CHILD_INDEX)
			{
				// childrenの1つしか参照しない
				const float rcp_length = m_children.at(m_blend_data.at(0).child_index).clip.GetRCPLength();
				weighted_normalize_time = rcp_length * elapsed_time * speed_param;
				normalize_time = rcp_length * elapsed_time;
				return;
			}
			// ブレンド率を考慮して時間を計算

			// 実時間に掛ける係数を求める
			float total_multiply_value = 0.0f;
			{
				float total_weight = 0.0f;
				// 始めのデータを取り出しておく
				total_multiply_value += m_children.at(m_blend_data.at(0).child_index).clip.GetRCPLength();
				total_weight += m_blend_data.at(0).weight;
				// BlendData.child_index == INVALID_CHILD_INDEXになるまでブレンドする
				int data_index = 1;
				while (true)
				{
					// インデックスの範囲と有効値チェック
					if (m_blend_data_num <= data_index || m_blend_data.at(data_index).child_index == INVALID_CHILD_INDEX)
					{
						break;
					}
					const auto& blend_data = m_blend_data.at(data_index);

					// 係数をブレンドする
					const float prev_weight = total_weight;
					total_weight += blend_data.weight;
					const float ratio = prev_weight / total_weight;
					// Calculate : elapsed / length_1 * ratio + elapsed / length_2 * (1 - ratio)
					total_multiply_value = total_multiply_value * ratio + m_children.at(blend_data.child_index).clip.GetRCPLength() * (1.0f - ratio);

					// 参照するインデックスを1つ先に進める
					++data_index;
				}
			}

			weighted_normalize_time = elapsed_time * speed_param * total_multiply_value;
			normalize_time = elapsed_time * total_multiply_value;
		}
	}

	void BlendTree::OnRemoveBlock(int removed_index, std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks)
	{
		// 自身のインデックスを管理するステートマシンから削除
		std::static_pointer_cast<AnimatorStateMachine>(blocks.at(m_owner_statemachine_index))->RemoveChildStateAndTree(removed_index);
		// 保持しているインデックスとTransitionの先のインデックスを修正、removed_indexの削除をする
		OnRemoveBlockAllFixAndErase(removed_index, blocks);
	}

	void BlendTree::CalculateBlendRatioSimple1D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters)
	{
		// Childrenが1つ以下の場合は早期リターン
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

		// Parameterの取得
		const float param = GetParameter1D(parameters);

		// 現在の値から大きいものと小さいものからそれぞれ、最も近いClipを取得
		int less_index = INVALID_CHILD_INDEX;
		int greater_index = INVALID_CHILD_INDEX;
		{
			float less_dist = FLT_MAX;
			float greater_dist = FLT_MAX;
			// paramに近いものを探索
			for (size_t i = 0; i < child_num; ++i)
			{
				const auto& child = m_children.at(i);

				// Clipの存在確認
				if (!child.clip.HasMotion())
				{
					continue;
				}

				// 2点間の距離
				const float dist = MyMath::Abs(param - child.threshold.x);
				// paramより小さい
				if (child.threshold.x <= param)
				{
					// 現在保存している距離と比較
					if (dist < less_dist)
					{
						less_dist = dist;
						less_index = static_cast<int>(i);
					}
				}
				// paramより大きい
				else
				{
					// 現在保存している距離と比較
					if (dist < greater_dist)
					{
						greater_dist = dist;
						greater_index = static_cast<int>(i);
					}
				}
			}
		}
		// ブレンド率を計算
		{
			// less,greaterの両方がInvalidなら早期リターン
			if (less_index == INVALID_CHILD_INDEX && greater_index == INVALID_CHILD_INDEX)
			{
				return;
			}

			// less,greaterの一方しか存在しないときは存在するものを100%適用する
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
			// childとparamの距離からブレンド率を計算する
			//    | --------------length-------------------|
			// [less] ---------- (param) -----dist----- [greater]
			//    | --- (ratio) --- | ---- (1 - ratio) ----|
			const float less_param = m_children.at(less_index).threshold.x;
			const float greater_param = m_children.at(greater_index).threshold.x;
			const float length = greater_param - less_param;
			// lengthが0の対応
			if (MyMath::Approximately(length, 0.0f))
			{
				m_blend_data.at(0).child_index = less_index;
				m_blend_data.at(0).weight = 1.0f;
				m_blend_data_num = 1;
			}
			else
			{
				// lessのデータ
				auto& less_data = m_blend_data.at(0);
				less_data.child_index = less_index;
				less_data.weight = (greater_param - param) / length;
				// greaterのデータ
				auto& greater_data = m_blend_data.at(1);
				greater_data.child_index = greater_index;
				greater_data.weight = (param - less_param) / length;
				// ブレンドするデータ数
				m_blend_data_num = 2;
			}
		}
	}

	void BlendTree::CalculateBlendRatioSimple2D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters)
	{
		// Childrenが1つ以下の場合は早期リターン
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

		// Parameterの取得
		const VECTOR2 param = GetParameter2D(parameters);

		// 現在の値の最近傍値を2つ取得
		// [0]:1番近い [1]:2番目に近い
		int near_indices[2] = { INVALID_CHILD_INDEX, INVALID_CHILD_INDEX };
		{
			float near_square_distances[2] = { FLT_MAX, FLT_MAX };
			for (size_t i = 0; i < child_num; ++i)
			{
				const auto& child = m_children.at(i);

				// Clipの存在確認
				if (!child.clip.HasMotion())
				{
					continue;
				}
				// 距離2乗の計算
				const float sq_dist = VECTOR2::DistanceSq(param, VECTOR2(child.threshold.x, child.threshold.y));
				// 現在保持している距離2乗と比較
				if (near_square_distances[1] > sq_dist)
				{
					// 1番近い
					if (sq_dist < near_square_distances[0])
					{
						near_indices[1] = near_indices[0];
						near_square_distances[1] = near_square_distances[0];
						near_indices[0] = static_cast<int>(i);
						near_square_distances[0] = sq_dist;
					}
					// 2番目に近い
					else if (sq_dist < near_square_distances[1])
					{
						near_indices[1] = static_cast<int>(i);
						near_square_distances[1] = sq_dist;
					}
				}
			}

			// 有効なChildを1個、もしくは0個しか取得できなかった場合
			if (near_indices[1] == INVALID_CHILD_INDEX)
			{
				// 0個の場合
				if (near_indices[0] == INVALID_CHILD_INDEX)
				{
					return;
				}
				// 1個の場合
				else
				{
					m_blend_data.at(0).child_index = near_indices[0];
					m_blend_data.at(0).weight = 1.0f;
					m_blend_data_num = 1;

					return;
				}
			}
		}

		// ブレンド率を計算
		{
			// [0] -----> [1]
			// 
			//        P
			// vec(0 to 1)にvec(0 to P)を射影してlength(0 to 1)との比率を求める
			const VECTOR3 threshold_0 = m_children.at(near_indices[0]).threshold;
			const VECTOR3 threshold_1 = m_children.at(near_indices[1]).threshold;
			const VECTOR2 vec_0_to_param = param - VECTOR2(threshold_0.x, threshold_0.y);
			const VECTOR2 vec_0_to_1 = VECTOR2(threshold_1.x, threshold_1.y) - VECTOR2(threshold_0.x, threshold_0.y);
			const VECTOR2 norm_0_to_1 = vec_0_to_1.Normalized();
			const float length_0_to_1 = vec_0_to_1.Length();
			const float dot_0P_to_01 = vec_0_to_param.Dot(norm_0_to_1);
			// 内積が負の時は[0]を100%
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
			// 値の保存
			auto& first_data = m_blend_data.at(0);
			first_data.child_index = near_indices[0];
			first_data.weight = 1.0f - ratio;
			auto& second_data = m_blend_data.at(1);
			second_data.child_index = near_indices[1];
			second_data.weight = ratio;
			// ブレンドするデータ数
			m_blend_data_num = 2;
		}
	}

	void BlendTree::CalculateBlendRatioFreeformCartesian2D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters)
	{
		// Childrenが1つ以下の場合は早期リターン
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

		// Childrenごとの影響度を計算してWeightを計算する

		// Parameterの取得
		const VECTOR2 param = GetParameter2D(parameters);

		// 各クリップの影響を求める
		for (int i = 0; i < child_num; ++i)
		{
			// クリップの存在確認
			if (!m_children.at(i).clip.HasMotion())
				continue;

			// 影響度を計算する
			m_influences.at(i) = CalculateClipInfluence(param, i);
		}
		// 総和を求める
		float sum_influence = 0.0f;
		{
			for (const float influence : m_influences)
			{
				sum_influence += influence;
			}
		}

		// それぞれのchildによる重みを求める
		for (int i = 0; i < child_num; ++i)
		{
			// 0以下の影響度のものは影響を受けない
			if (MyMath::Approximately(m_influences.at(i), 0.0f))
			{
				continue;
			}

			// ブレンドする要素にセットする
			m_blend_data.at(m_blend_data_num).child_index = i;
			// 重みを正規化する
			m_blend_data.at(m_blend_data_num).weight = m_influences.at(i) / sum_influence;

			// セットする場所を移動する
			++m_blend_data_num;
		}
	}

	void BlendTree::CalculateBlendRatioFreeformCartesian3D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters)
	{
		// Childrenが1つ以下の場合は早期リターン
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

		// Childrenごとの影響度を計算してWeightを計算する

		// Parameterの取得
		const VECTOR3 param = GetParameter3D(parameters);

		// 各クリップの影響を求める
		for (int i = 0; i < child_num; ++i)
		{
			// クリップの存在確認
			if (!m_children.at(i).clip.HasMotion())
				continue;

			// 影響度を計算する
			m_influences.at(i) = CalculateClipInfluence(param, i);
		}
		// 総和を求める
		float sum_influence = 0.0f;
		{
			for (const float influence : m_influences)
			{
				sum_influence += influence;
			}
		}

		// それぞれのchildによる重みを求める
		for (int i = 0; i < child_num; ++i)
		{
			// 0以下の影響度のものは影響を受けない
			if (MyMath::Approximately(m_influences.at(i), 0.0f))
			{
				continue;
			}

			// ブレンドする要素にセットする
			m_blend_data.at(m_blend_data_num).child_index = i;
			// 重みを正規化する
			m_blend_data.at(m_blend_data_num).weight = m_influences.at(i) / sum_influence;

			// セットする場所を移動する
			++m_blend_data_num;
		}
	}

	float BlendTree::CalculateClipInfluence(const VECTOR2& param, const int child_idx)
	{
		const int child_num = static_cast<int>(m_children.size());
		const VECTOR2 child_threshold = VECTOR2(m_children.at(child_idx).threshold.x, m_children.at(child_idx).threshold.y);

		// 現在の最小影響度
		float min_influence = FLT_MAX;
		// 値の更新がされたか
		bool set_min_value = false;

		for (int i = 0; i < child_num; ++i)
		{
			// 自身と同じときは計算しない
			if (i == child_idx)
				continue;

			// クリップの存在確認
			if (!m_children.at(i).clip.HasMotion())
				continue;

			// 1 - (dot(PiP,PiPj)/dot(PiPj,PiPj))
			const VECTOR2 threshold = VECTOR2(m_children.at(i).threshold.x, m_children.at(i).threshold.y);
			const VECTOR2 pi_to_pj = threshold - child_threshold;
			const float dot_pi_p_vs_pi_pj = VECTOR2::Dot(param - child_threshold, pi_to_pj);
			const float dot_pi_pj_vs_pi_pj = VECTOR2::Dot(pi_to_pj, pi_to_pj);
			const float influence = 1.0f - (dot_pi_p_vs_pi_pj * MyMath::InvertDivisionValue(dot_pi_pj_vs_pi_pj));

			// 最小の値かチェック
			if (min_influence > influence)
			{
				min_influence = influence;
				set_min_value = true;
			}
		}

		// 自分以外にクリップが存在しないときは1.0を返す
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

		// 現在の最小影響度
		float min_influence = FLT_MAX;
		// 値の更新がされたか
		bool set_min_value = false;

		for (int i = 0; i < child_num; ++i)
		{
			// 自身と同じときは計算しない
			if (i == child_idx)
				continue;

			// クリップの存在確認
			if (!m_children.at(i).clip.HasMotion())
				continue;

			// 1 - (dot(PiP,PiPj)/dot(PiPj,PiPj))
			const VECTOR3 threshold = m_children.at(i).threshold;
			const VECTOR3 pi_to_pj = threshold - child_threshold;
			const float dot_pi_p_vs_pi_pj = VECTOR3::Dot(param - child_threshold, pi_to_pj);
			const float dot_pi_pj_vs_pi_pj = VECTOR3::Dot(pi_to_pj, pi_to_pj);
			const float influence = 1.0f - (dot_pi_p_vs_pi_pj * MyMath::InvertDivisionValue(dot_pi_pj_vs_pi_pj));

			// 最小の値かチェック
			if (min_influence > influence)
			{
				min_influence = influence;
				set_min_value = true;
			}
		}

		// 自分以外にクリップが存在しないときは1.0を返す
		if (!set_min_value)
		{
			return 1.0f;
		}

		return MyMath::Max(0.0f, min_influence);
	}

	float BlendTree::GetParameter1D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters) const
	{
		float param = 0.0f;
		// map探索
		const auto itr_find = parameters.find(m_parameter);
		if (itr_find == parameters.end())
		{
			param = 0.0f;
		}
		else
		{
			// ParameterのTypeがFloatでないときはparam = 0.0
			if (itr_find->second->type != AnimatorControllerParameter::Type::Float)
			{
				param = 0.0f;
			}
			else
			{
				// Parameterの値を取得
				param = itr_find->second->default_float;
			}
		}

		return param;
	}

	VECTOR2 BlendTree::GetParameter2D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters) const
	{
		VECTOR2 param = VECTOR2::Zero;
		// map探索
		const auto itr_find_x = parameters.find(m_parameter);
		const auto itr_find_y = parameters.find(m_parameter_y);
		// X
		if (itr_find_x == parameters.end())
		{
			param.x = 0.0f;
		}
		else
		{
			// ParameterのTypeがFloatでないときはparam = 0.0
			if (itr_find_x->second->type != AnimatorControllerParameter::Type::Float)
			{
				param.x = 0.0f;
			}
			else
			{
				// Parameterの値を取得
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
			// ParameterのTypeがFloatでないときはparam = 0.0
			if (itr_find_y->second->type != AnimatorControllerParameter::Type::Float)
			{
				param.y = 0.0f;
			}
			else
			{
				// Parameterの値を取得
				param.y = itr_find_y->second->default_float;
			}
		}

		return param;
	}

	VECTOR3 BlendTree::GetParameter3D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters) const
	{
		VECTOR3 param = VECTOR3::Zero;
		// map探索
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
			// ParameterのTypeがFloatでないときはparam = 0.0
			if (itr_find_x->second->type != AnimatorControllerParameter::Type::Float)
			{
				param.x = 0.0f;
			}
			else
			{
				// Parameterの値を取得
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
			// ParameterのTypeがFloatでないときはparam = 0.0
			if (itr_find_y->second->type != AnimatorControllerParameter::Type::Float)
			{
				param.y = 0.0f;
			}
			else
			{
				// Parameterの値を取得
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
			// ParameterのTypeがFloatでないときはparam = 0.0
			if (itr_find_z->second->type != AnimatorControllerParameter::Type::Float)
			{
				param.z = 0.0f;
			}
			else
			{
				// Parameterの値を取得
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
						// 終端まで来たら発見できなかった
						if (data.child_index == INVALID_CHILD_INDEX)
							break;
						// 自身のインデックスと一致したら重みを返す
						if (data.child_index == self_index)
						{
							weight = data.weight;
							break;
						}
					}
				}
				// プログレスバーで重ねて表示
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
