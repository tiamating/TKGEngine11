#pragma once

#include "Animator_BlockBase.h"

#include "Application/Resource/inc/Shader_Defined.h"

#include <unordered_set>

namespace TKGEngine::Animations
{
	class AnimatorState;
	//class BlendTree;

	/// <summary>
	/// �X�e�[�g�𑊌ݍ�p�ɐ��䂷��. �e�X�e�[�g��Motion���Q�Ƃ��Ă���
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
		// GUI��p:�X�e�[�g�}�V�����Ǘ�����u���b�N���X�g���\�[�g���ĕԂ�
		void GetManageBlockList(std::list<int>& block_list);
		int OnGUIStateMachine(
			const GameObjectID goid,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters
		);
		void OnGUIEntry(const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks, std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters);
#endif// #ifdef USE_IMGUI

		// Entry����̃f�t�H���g�J�ڐ���w�肷��
		void SetEntryBlock(int index);
		// Entry����̑J�ڐ��ǉ�����
		std::shared_ptr<AnimatorTransition> AddEntryTransition(const int destination_block_idx);

		// Entry�����Transition�ɂ��J�ڂ��`�F�b�N
		int CheckEntryTransition(
			const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			float& offset
		);

		// �J�ڐ�̃f�[�^��o�^����
		void SetNextData(
			const GameObjectID goid,
			const int next,
			const float duration,
			const float offset,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks
		);

		// �q�u���b�N�ǉ����ɊǗ����Ă���u���b�N�̃C���f�b�N�X��o�^����
		void AddChildStateAndTree(const int index);
		void RemoveChildStateAndTree(const int index);
		// �q�u���b�N�ǉ����ɊǗ����Ă���X�e�[�g�}�V���̃C���f�b�N�X��o�^����
		void AddChildStateMachine(const int index);
		void RemoveChildStateMachine(const int index);

		// ���݂̃L�[�f�[�^��Ԃ�
		const std::vector<KeyData>* GetKeyFrame() const;

		// ���[�g���C���[�Ȃ�L�[�f�[�^���㏑���A�q���C���[�Ȃ�L�[�f�[�^���u�����h���鏈��
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
		// GUI��p�A���ݑI�𒆂̃u���b�Nindex
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

		// �X�e�[�g�}�V���̍X�V���
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

		// ���C���[���̂����ꂩ��Block�폜���ɔԍ��̊m�F�ƏC��������
		void OnRemoveBlock(int removed_index, std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks) override;

		// ���̃u���b�N��speed��length���l�������t���[�����K���o�ߎ��ԂƁAspeed���l�����Ȃ����K���o�ߎ��Ԃ�Ԃ�
		void GetNormalizeTime(
			const float elapsed_time,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters,
			float& weighted_normalize_time,
			float& normalize_time
		) override;

		// �폜�����C���f�b�N�X�ɉ����ăC���f�b�N�X���C�����鏈��
		void OnRemoveBlockFixIndex(const int removed_index, const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks) override;
		
		// Root State Machine���Ԃ��f�[�^�����肷�鏈��
		void ApplyRootKeyData(
			const bool is_root_layer,
			const bool is_additive,
			const float layer_weight,
			const float(&weights)[MAX_BONES],
			const bool(&masks)[MAX_BONES],
			const std::vector<int>& enable_nodes,
			std::vector<KeyData>& dst_data
		);
		// ApplyRootKeyData���Ń��C���[�ɉ������u�����h����������
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
		// ���̃X�e�[�g�}�V�����Ǘ�����u���b�N
		std::unordered_set<int> m_states_and_trees;
		std::unordered_set<int> m_statemachines;

		// �n�߂Ɏ��s�����u���b�N��I������J�ڏ���
		std::vector<std::shared_ptr<AnimatorTransition>> m_entry_transitions;
		// �n�߂Ɏ��s�����u���b�N�̃C���f�b�N�X
		int m_entry_index = INVALID_BLOCK_INDEX;

		// �X�e�[�g�}�V���̌����
		StateMachineState m_state_machine_state = StateMachineState::Entry;
		// ���݂̃u���b�N
		int m_current_block_index = INVALID_BLOCK_INDEX;
		float m_current_normalize_time = 0.0f;
		// ���荞�ݑJ�ڂ��ꂽ�Ƃ��̑J�ڐ�̃u���b�N�C���f�b�N�X
		int m_interrupted_next_block_index = INVALID_BLOCK_INDEX;
		// �J�ڐ�̃u���b�N
		int m_next_block_index = INVALID_BLOCK_INDEX;
		float m_next_normalize_time = 0.0f;
		// Transition���̎c��J�ڐ��K������
		float m_duration_normalize_timer = 0.0f;
		// Transition�̑J�ڎ���
		float m_duration_start_time = 0.0f;

		// �J�ڂ����t���[����
		bool m_on_transition_frame = false;
		// ���s���̃u���b�N�̍X�V���L�����ǂ���
		bool m_current_valid_update = false;
		bool m_next_valid_update = false;

		// �o�͂���錻�݂̃A�j���[�V�������
		std::vector<KeyData> m_keyframe;
		// �v�Z�Ɏg�p�����A�j���[�V�������
		std::vector<KeyData> m_use_calc_keyframes[2];
	};


}// namespace TKGEngine::Animations

CEREAL_CLASS_VERSION(TKGEngine::Animations::AnimatorStateMachine, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::Animations::AnimatorStateMachine, "TKGEngine::AnimatorStateMachine")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Animations::AnimatorBlockBase, TKGEngine::Animations::AnimatorStateMachine)