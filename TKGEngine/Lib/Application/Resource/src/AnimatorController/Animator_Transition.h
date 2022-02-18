#pragma once

#include "Animation_Defined.h"


namespace TKGEngine::Animations
{
	class AnimatorBlockBase;

	/// <summary>
	/// ステートマシンの遷移定義用クラス
	/// </summary>
	class AnimatorTransition
	{
	public:
		/// <summary>
		///	AnimatorStateの遷移に割り込める遷移条件
		/// </summary>
		enum class InterruptionSource
		{
			None,
			Current,
			Next,
			//SourceThenDestination,
			//DestinationThenSource,

			Max_InterruptionSource
		};

		// ==============================================
		// public methods
		// ==============================================
		AnimatorTransition() = default;
		virtual ~AnimatorTransition() = default;
		AnimatorTransition(const AnimatorTransition&);
		AnimatorTransition& operator=(const AnimatorTransition&);

#ifdef USE_IMGUI
		void OnGUI(std::unordered_map<std::string, std::shared_ptr<Animations::AnimatorControllerParameter>>& parameters, const bool show_exit_time);
#endif// #ifdef USE_IMGUI

		int GetDestinationBlock() const;
		void SetDestinationBlock(int destination_block_idx);

		void AddCondition(AnimatorCondition::Mode mode, const char* parameter, float threshold);
		void AddCondition(AnimatorCondition::Mode mode, const char* parameter, int threshold);
		void AddCondition(AnimatorCondition::Mode mode, const char* parameter, bool threshold);
		void RemoveCondition(int index);

		void HasExitTime(bool use);
		void ExitTime(float time);
		float ExitTime() const;

		void Duration(float duration);
		float Duration() const;

		void Offset(float offset);
		float Offset() const;

		void SetInterruptionSource(InterruptionSource interruption);

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
		/// <param name="duration">遷移にかける時間を返す</param>
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

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */

	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(m_conditions),
					CEREAL_NVP(m_interruption_source),
					CEREAL_NVP(m_destination_block),
					CEREAL_NVP(m_has_exit_time),
					CEREAL_NVP(m_exit_time),
					CEREAL_NVP(m_duration),
					CEREAL_NVP(m_offset)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		void OnGUICondition(std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters);
#endif// #ifdef USE_IMGUI

		// Conditionテストに合格するかチェック
		bool ConditionCheckOnCheckTransition(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters);


		// ==============================================
		// private variables
		// ==============================================
		// 遷移条件
		std::vector<AnimatorCondition> m_conditions;
		// 遷移中遷移の参照を決める
		InterruptionSource m_interruption_source = InterruptionSource::None;
		// 遷移先のブロック
		int m_destination_block = INVALID_BLOCK_INDEX;
		// 終端時間遷移が有効か
		bool m_has_exit_time = false;
		// 遷移の終了を指定する正規化時間 (1.0未満なら毎ループチェックする)
		float m_exit_time = 0.0f;
		// 遷移の継続する実時間
		float m_duration = 0.0f;
		// 遷移先のステートが開始される時点の正規化時間
		float m_offset = 0.0f;
	};

}// namespace TKGEngine::Animations

CEREAL_CLASS_VERSION(TKGEngine::Animations::AnimatorTransition, 1);