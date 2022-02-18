#pragma once

#include "Animation_Defined.h"


namespace TKGEngine::Animations
{
	class AnimatorBlockBase;

	/// <summary>
	/// �X�e�[�g�}�V���̑J�ڒ�`�p�N���X
	/// </summary>
	class AnimatorTransition
	{
	public:
		/// <summary>
		///	AnimatorState�̑J�ڂɊ��荞�߂�J�ڏ���
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
		/// �A�j���[�V������J�ڂ����邩�`�F�b�N
		/// State�Ȃ�Exit Time�ł̑J�ڂ��l��
		/// StateMachine��Exit Time�������Ȃ��Ƃ���
		/// </summary>
		/// <param name="is_current">�`�F�b�N���Ă���u���b�N���J�ڌ����J�ڐ悩</param>
		/// <param name="state_transitioning">�J�ڒ����ǂ���</param>
		/// <param name="normalized_time">�o�߂������K������</param>
		/// <param name="owner_index">���g���Ǘ�����X�e�[�g�}�V���̃C���f�b�N�X</param>
		/// <param name="parameters">�J�ڃp�����[�^�̃}�b�v</param>
		/// <param name="blocks">�u���b�N���X�g</param>
		/// <param name="duration">�J�ڂɂ����鎞�Ԃ�Ԃ�</param>
		/// <param name="offset">�J�ڐ�̊J�n���K����</param>
		/// <returns>-1�Ȃ�J�ڂȂ�. 0�ȏ�Ȃ�J�ڐ�̃u���b�N�̃C���f�b�N�X</returns>
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

		// Condition�e�X�g�ɍ��i���邩�`�F�b�N
		bool ConditionCheckOnCheckTransition(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters);


		// ==============================================
		// private variables
		// ==============================================
		// �J�ڏ���
		std::vector<AnimatorCondition> m_conditions;
		// �J�ڒ��J�ڂ̎Q�Ƃ����߂�
		InterruptionSource m_interruption_source = InterruptionSource::None;
		// �J�ڐ�̃u���b�N
		int m_destination_block = INVALID_BLOCK_INDEX;
		// �I�[���ԑJ�ڂ��L����
		bool m_has_exit_time = false;
		// �J�ڂ̏I�����w�肷�鐳�K������ (1.0�����Ȃ疈���[�v�`�F�b�N����)
		float m_exit_time = 0.0f;
		// �J�ڂ̌p�����������
		float m_duration = 0.0f;
		// �J�ڐ�̃X�e�[�g���J�n����鎞�_�̐��K������
		float m_offset = 0.0f;
	};

}// namespace TKGEngine::Animations

CEREAL_CLASS_VERSION(TKGEngine::Animations::AnimatorTransition, 1);