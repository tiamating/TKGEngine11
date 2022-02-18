#pragma once

#include "Animator_BlockBase.h"

#include "Application/Resource/inc/Shader_Defined.h"

#include <unordered_set>

namespace TKGEngine::Animations
{
	class AnimatorState;
	//class BlendTree;

	/// <summary>
	/// ステートを相互作用に制御する. 各ステートはMotionを参照している
	/// </summary>
	class AnimatorStateMachine
		: public AnimatorBlockBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		AnimatorStateMachine();
		AnimatorStateMachine(const char* name);
		virtual ~AnimatorStateMachine() = default;
		AnimatorStateMachine(const AnimatorStateMachine&) = delete;
		AnimatorStateMachine& operator=(const AnimatorStateMachine&) = delete;

#ifdef USE_IMGUI
		// GUI専用:ステートマシンが管理するブロックリストをソートして返す
		void GetManageBlockList(std::list<int>& block_list);
		int OnGUIStateMachine(
			const GameObjectID goid,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters
		);
		void OnGUIEntry(const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks, std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters);
#endif// #ifdef USE_IMGUI

		// Entryからのデフォルト遷移先を指定する
		void SetEntryBlock(int index);
		// Entryからの遷移先を追加する
		std::shared_ptr<AnimatorTransition> AddEntryTransition(const int destination_block_idx);

		// EntryからのTransitionによる遷移をチェック
		int CheckEntryTransition(
			const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			float& offset
		);

		// 遷移先のデータを登録する
		void SetNextData(
			const GameObjectID goid,
			const int next,
			const float duration,
			const float offset,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks
		);

		// 子ブロック追加時に管理しているブロックのインデックスを登録する
		void AddChildStateAndTree(const int index);
		void RemoveChildStateAndTree(const int index);
		// 子ブロック追加時に管理しているステートマシンのインデックスを登録する
		void AddChildStateMachine(const int index);
		void RemoveChildStateMachine(const int index);

		// 現在のキーデータを返す
		const std::vector<KeyData>* GetKeyFrame() const;

		// ルートレイヤーならキーデータを上書き、子レイヤーならキーデータをブレンドする処理
		void ApplyKeyFrameData(
			const GameObjectID goid,
			const bool is_root_layer,
			const bool is_additive,
			const float elapsed_time,
			const float layer_weight,
			const float (&weights)[MAX_BONES],
			const bool (&masks)[MAX_BONES],
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters,
			const std::unordered_map<std::string, int>* node_index,
			const std::vector<int>& enable_nodes,
			std::vector<KeyData>& dst_data
		);

		// ==============================================
		// public variables
		// ==============================================
#ifdef USE_IMGUI
		// GUI専用、現在選択中のブロックindex
		int m_gui_current_select_block_index = INVALID_BLOCK_INDEX;
#endif// #ifdef USE_IMGUI


	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					cereal::base_class<AnimatorBlockBase>(this),
					CEREAL_NVP(m_entry_index),
					CEREAL_NVP(m_entry_transitions),
					CEREAL_NVP(m_states_and_trees),
					CEREAL_NVP(m_statemachines)
				);
			}
		}

		// ステートマシンの更新状態
		enum class StateMachineState
		{
			Entry,
			Play,
			Transition,
			InterruptTransition
		};
		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		int OnGUI(
			const GameObjectID goid,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters
		) override;
		bool OnGUISelectBlock(const ImGuiTextFilter& im_filter, const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks, int& current_index);
		void OnGUIEntryTransition(const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks, std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters);
#endif// #ifdef USE_IMGUI

		bool UpdateBlock(
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
		) override;
		bool UpdateKeyFrame(
			const bool is_root_layer,
			const bool is_additive,
			const float normalize_time,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			const std::unordered_map<std::string, int>* node_index,
			std::vector<KeyData>& dst_keydata
		) override;
		void OnReady(const float offset) override;

		// レイヤー内のいずれかのBlock削除時に番号の確認と修正をする
		void OnRemoveBlock(int removed_index, std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks) override;

		// そのブロックのspeedとlengthを考慮したフレーム正規化経過時間と、speedを考慮しない正規化経過時間を返す
		void GetNormalizeTime(
			const float elapsed_time,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters,
			float& weighted_normalize_time,
			float& normalize_time
		) override;

		// 削除されるインデックスに応じてインデックスを修正する処理
		void OnRemoveBlockFixIndex(const int removed_index, const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks) override;
		
		// Root State Machineが返すデータを決定する処理
		void ApplyRootKeyData(
			const bool is_root_layer,
			const bool is_additive,
			const float layer_weight,
			const float(&weights)[MAX_BONES],
			const bool(&masks)[MAX_BONES],
			const std::vector<int>& enable_nodes,
			std::vector<KeyData>& dst_data
		);
		// ApplyRootKeyData内でレイヤーに応じたブレンド処理をする
		void OnApplyRootData(
			const bool is_root_layer,
			const bool is_additive,
			const float layer_weight,
			const float(&weights)[MAX_BONES],
			const bool(&masks)[MAX_BONES],
			const std::vector<int>& enable_nodes,
			std::vector<KeyData>& dst_data
		);


		// ==============================================
		// private variables
		// ==============================================
		// このステートマシンが管理するブロック
		std::unordered_set<int> m_states_and_trees;
		std::unordered_set<int> m_statemachines;

		// 始めに実行されるブロックを選択する遷移条件
		std::vector<std::shared_ptr<AnimatorTransition>> m_entry_transitions;
		// 始めに実行されるブロックのインデックス
		int m_entry_index = INVALID_BLOCK_INDEX;

		// ステートマシンの現状態
		StateMachineState m_state_machine_state = StateMachineState::Entry;
		// 現在のブロック
		int m_current_block_index = INVALID_BLOCK_INDEX;
		float m_current_normalize_time = 0.0f;
		// 割り込み遷移されたときの遷移先のブロックインデックス
		int m_interrupted_next_block_index = INVALID_BLOCK_INDEX;
		// 遷移先のブロック
		int m_next_block_index = INVALID_BLOCK_INDEX;
		float m_next_normalize_time = 0.0f;
		// Transition時の残り遷移正規化時間
		float m_duration_normalize_timer = 0.0f;
		// Transitionの遷移時間
		float m_duration_start_time = 0.0f;

		// 遷移したフレームか
		bool m_on_transition_frame = false;
		// 実行中のブロックの更新が有効かどうか
		bool m_current_valid_update = false;
		bool m_next_valid_update = false;

		// 出力される現在のアニメーション情報
		std::vector<KeyData> m_keyframe;
		// 計算に使用されるアニメーション情報
		std::vector<KeyData> m_use_calc_keyframes[2];
	};


}// namespace TKGEngine::Animations

CEREAL_CLASS_VERSION(TKGEngine::Animations::AnimatorStateMachine, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::Animations::AnimatorStateMachine, "TKGEngine::AnimatorStateMachine")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Animations::AnimatorBlockBase, TKGEngine::Animations::AnimatorStateMachine)