#pragma once

#include "Animator_BlockBase.h"

#include "Animator_Clip.h"
#include "Animator_Event.h"

namespace TKGEngine::Animations
{
	/// <summary>
	/// �X�e�[�g�}�V���̊�{�I�ȃr���f�B���O�u���b�N
	/// </summary>
	class AnimatorState
		: public AnimatorBlockBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		AnimatorState();
		AnimatorState(const char* name);
		virtual ~AnimatorState() = default;
		AnimatorState(const AnimatorState&) = delete;
		AnimatorState& operator=(const AnimatorState&) = delete;

		// �A�j���[�V�����N���b�v�̒ǉ�
		void AddClip(const std::string& motion_filepath);
		void AddClip(const AnimationClip& clip);

		const char* GetMotionName() const;

		// �Đ����x(0�ȏ�)
		void Speed(const float speed);
		float Speed() const;

		// �J��Ԃ���
		void SetWrapMode(WrapMode mode);

		// �C�x���g
		void AddEvent(const std::string& class_type_name, const std::string& func_name, const float normalize_time);
		void AddEvent(const std::string& class_type_name, const std::string& func_name, const float normalize_time, int parameter);
		void AddEvent(const std::string& class_type_name, const std::string& func_name, const float normalize_time, float parameter);
		void RemoveEvent(int index);

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */

	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			if (version == 1)
			{
				archive(
					cereal::base_class<AnimatorBlockBase>(this),
					CEREAL_NVP(m_clip),
					CEREAL_NVP(m_speed),
					CEREAL_NVP(m_use_speed_parameter),
					CEREAL_NVP(m_speed_parameter),
					CEREAL_NVP(m_wrap_mode),
					CEREAL_NVP(m_events)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		int OnGUI(
			const GameObjectID goid,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters
		) override;
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

		// ���̃u���b�N��speed��length���l�������t���[�����K���o�ߎ��ԂƁAspeed���l�����Ȃ����K���o�ߎ��Ԃ�Ԃ�
		void GetNormalizeTime(
			const float elapsed_time,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters,
			float& weighted_normalize_time,
			float& normalize_time
		) override;

		// ���C���[���̂����ꂩ��Block�폜���ɔԍ��̊m�F�ƏC��������
		void OnRemoveBlock(int removed_index, std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks) override;


		// ==============================================
		// private variables
		// ==============================================
		// ���[�V�����f�[�^
		AnimationClip m_clip;
		// 0.0f�ȏ�̒l��Motion�̍Đ����x
		float m_speed = 1.0f;
		// Speed�Ƀp�����[�^����Z���邩
		bool m_use_speed_parameter = false;
		// Speed�ɏ�Z����p�����[�^��
		std::string m_speed_parameter;
		// �J��Ԃ���
		WrapMode m_wrap_mode = WrapMode::Loop;
		// �C�x���g
		std::vector<AnimatorEvent> m_events;
		// �C�x���g��wrap�ɂ�锭�΃~�X���Ȃ������߂ɑO�t���[����normalize_time��ێ�����
		float m_prev_normalize_time = 0.0f;
	};

}// namespace TKGEngine::Animations

CEREAL_REGISTER_TYPE(TKGEngine::Animations::AnimatorState)
CEREAL_CLASS_VERSION(TKGEngine::Animations::AnimatorState, 1)
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Animations::AnimatorBlockBase, TKGEngine::Animations::AnimatorState)