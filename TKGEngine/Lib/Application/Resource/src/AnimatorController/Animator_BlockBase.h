#pragma once

#include "Animation_Defined.h"
#include "Animator_Transition.h"

#include <string>
#include <vector>
#include <unordered_map>

#include <memory>

namespace TKGEngine::Animations
{
	class AnimationClip;

	/// <summary>
	/// アニメーションブロックの基底クラス(AnimatorState, BlendTreeの基底クラス)
	/// </summary>
	class AnimatorBlockBase
	{
	public:
		// ==============================================
		// public enum
		// ==============================================
		// ブロックの種類
		enum class BlockType
		{
			State,
			StateMachine,
			BlendTree
		};
		// ブロックの更新状態
		enum class BlockState
		{
			Ready,
			Play,
			Transition,
			End
		};

	public:
		// ==============================================
		// public methods
		// ==============================================
		AnimatorBlockBase() = default;
		AnimatorBlockBase(BlockType type);
		AnimatorBlockBase(BlockType type, const char* name);
		virtual ~AnimatorBlockBase() = default;
		AnimatorBlockBase(const AnimatorBlockBase&) = delete;
		AnimatorBlockBase& operator=(const AnimatorBlockBase&) = delete;

#ifdef USE_IMGUI
		// 次階層のステートマシンインデックスを返す
		virtual int OnGUI(
			const GameObjectID goid,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters
		);
		void OnGUITransition(const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks, std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters);
		void OnGUIEventFunction(GameObjectID goid);
#endif// #ifdef USE_IMGUI

		// ブロック自身のインデックスと所属するステートマシンのインデックスを登録する
		void Initialize(const int self_index, const int owner_statemachine_index);
		// 自身のインデックスを取得する
		int GetBlockIndex() const;
		// 自身の所属するステートマシンのインデックスを取得する
		int GetOwnerStateMachineIndex() const;

		// 遷移関係
		// 自身の所属するステートマシンと同階層のブロックへの遷移まで認める
		std::shared_ptr<AnimatorTransition> AddTransition(const int destination_block_idx);
		std::shared_ptr<AnimatorTransition> AddTransition(const AnimatorTransition& transition);
		void RemoveTransition(int index);
		// ブロックの持つTransitionを返す
		std::vector<std::shared_ptr<AnimatorTransition>>* GetTransitions();

		// 名前
		const char* GetName() const;
		void SetName(const std::string& name);

		// ブロックの状態の更新(clipがなければfalse)
		virtual bool UpdateBlock(
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
		) = 0;
		// アニメーションの情報の更新(clipがなければfalse)
		virtual bool UpdateKeyFrame(
			const bool is_root_layer,
			const bool is_additive,
			const float normalize_time,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			const std::unordered_map<std::string, int>* node_index,
			std::vector<KeyData>& dst_keydata
		) = 0;

		// そのブロックのspeedとlengthを考慮したフレーム正規化経過時間と、speedを考慮しない正規化経過時間を返す
		virtual void GetNormalizeTime(
			const float elapsed_time,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters,
			float& weighted_normalize_time,
			float& normalize_time
		) = 0;
		// レイヤー内のいずれかのBlock削除時にブロックごとに削除時の処理を実行する
		virtual void OnRemoveBlock(int removed_index, std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks) = 0;
		// ブロックの開始時の準備
		virtual void OnReady(const float offset) = 0;
		/// <summary>
		/// アニメーションを遷移させるかチェック
		/// StateならExit Timeでの遷移も考慮
		/// StateMachineはExit Timeを持たないとする
		/// </summary>
		/// <param name="is_current">チェックしているブロックが遷移元か遷移先か</param>
		/// <param name="state_transitioning">遷移中かどうか</param>
		/// <param name="normalized_time">経過した正規化時間</param>
		/// <param name="owner_index">自身を管理するステートマシンのインデックス</param>
		/// <param name="parameters">遷移パラメータのマップ</param>
		/// <param name="blocks">ブロックリスト</param>
		/// <param name="duration">遷移にかける正規化時間を返す</param>
		/// <param name="offset">遷移先の開始正規時間</param>
		/// <returns>-1なら遷移なし. 0以上なら遷移先のブロックのインデックス</returns>
		int CheckTransition(
			const bool is_current,
			const bool state_transitioning,
			const float normalized_time,
			const int owner_index,
			const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			float& duration,
			float& offset
		);

		// Enter時のイベント関数を呼ぶ
		void ExecuteEnterFunction(GameObjectID goid) const;
		// Exit時のイベント関数を呼ぶ
		void ExecuteExitFunction(GameObjectID goid) const;


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */

	protected:
		// ==============================================
		// protected methods
		// ==============================================
		// 2つのアニメーション情報を割合でブレンドする
		void BlendKeyFrame(
			const std::vector<KeyData>& v1,
			const std::vector<KeyData>& v2,
			const float ratio,
			std::vector<KeyData>& dst
		);
		// 2つのアニメーション情報を割合でブレンドしながら、Avatar内のデータ順に直す
		void BlendKeyFrameAndAlign(
			const std::vector<KeyData>& v1,
			const std::vector<KeyData>& v2,
			const float ratio,
			const std::unordered_map<std::string, int>* key_index,	// Motionデータ内の配列探索
			const std::unordered_map<std::string, int>* node_index,	// Avatarデータ内の配列探索
			std::vector<KeyData>& dst
		);

		// Avatar内のデータ順に直す
		void AlignKeyFrame(
			const std::vector<KeyData>& v,
			const std::unordered_map<std::string, int>* key_index,	// Motionデータ内の配列探索
			const std::unordered_map<std::string, int>* node_index,	// Avatarデータ内の配列探索
			std::vector<KeyData>& dst
		);

		//フレーム間の補間をしたキーを取得する
		void GetFrameBlendKey(const AnimationClip& clip, const float key, const std::unordered_map<std::string, int>* node_index, std::vector<KeyData>& dst);

		// 2つのキーデータの差分(v - clip)を求める
		void GetKeyDifference(
			const std::vector<KeyData>& v,
			const std::vector<KeyData>& clip,
			const std::unordered_map<std::string, int>* key_index,	// Motionデータ内の配列探索
			const std::unordered_map<std::string, int>* node_index,	// Avatarデータ内の配列探索
			std::vector<KeyData>& dst
		);

		// ブロック削除時に呼ばれる
		// ***全てのブロックのインデックスの修正
		// ***自身の削除をする
		void OnRemoveBlockAllFixAndErase(const int removed_index, std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks);
		// 削除されるインデックスに応じてインデックスを修正する処理
		virtual void OnRemoveBlockFixIndex(const int removed_index, const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks);

		// ブロックの種類を返す
		BlockType GetBlockType() const;


		// ==============================================
		// protected variables
		// ==============================================
		// ブロックの実行状態
		BlockState m_block_state = BlockState::Ready;

		// 自身のブロックID
		int m_self_block_index = INVALID_BLOCK_INDEX;
		// 所属するステートマシンのブロックID
		int m_owner_statemachine_index = INVALID_BLOCK_INDEX;


	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			if (version == 2)
			{
				archive(
					CEREAL_NVP(m_name),
					CEREAL_NVP(m_transitions),
					CEREAL_NVP(m_block_type),
					CEREAL_NVP(m_self_block_index),
					CEREAL_NVP(m_owner_statemachine_index),
					CEREAL_NVP(m_call_enter_function),
					CEREAL_NVP(m_enter_class_name),
					CEREAL_NVP(m_enter_func_name),
					CEREAL_NVP(m_call_exit_function),
					CEREAL_NVP(m_exit_class_name),
					CEREAL_NVP(m_exit_func_name)
				);
			}
			else if (version == 1)
			{
				archive(
					CEREAL_NVP(m_name),
					CEREAL_NVP(m_transitions),
					CEREAL_NVP(m_block_type),
					CEREAL_NVP(m_self_block_index),
					CEREAL_NVP(m_owner_statemachine_index)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
		/* nothing */


		// ==============================================
		// private variables
		// ==============================================
		// ブロック名
		std::string m_name;
		// ブロックから出ているTransition
		std::vector<std::shared_ptr<AnimatorTransition>> m_transitions;
		// ブロックの属性
		BlockType m_block_type = BlockType::State;

		// Enterイベント関数
		bool m_call_enter_function = false;
		std::string m_enter_class_name;
		std::string m_enter_func_name;
		// Exitイベント関数
		bool m_call_exit_function = false;
		std::string m_exit_class_name;
		std::string m_exit_func_name;
	};

	// --------------------------------------------------------------
	// inline
	// --------------------------------------------------------------
	inline AnimatorBlockBase::BlockType AnimatorBlockBase::GetBlockType() const
	{
		return m_block_type;
	}


}// namespace TKGEngine::Animations

CEREAL_CLASS_VERSION(TKGEngine::Animations::AnimatorBlockBase, 2);