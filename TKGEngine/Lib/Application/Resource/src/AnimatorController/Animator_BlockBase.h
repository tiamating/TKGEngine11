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
	/// �A�j���[�V�����u���b�N�̊��N���X(AnimatorState, BlendTree�̊��N���X)
	/// </summary>
	class AnimatorBlockBase
	{
	public:
		// ==============================================
		// public enum
		// ==============================================
		// �u���b�N�̎��
		enum class BlockType
		{
			State,
			StateMachine,
			BlendTree
		};
		// �u���b�N�̍X�V���
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
		// ���K�w�̃X�e�[�g�}�V���C���f�b�N�X��Ԃ�
		virtual int OnGUI(
			const GameObjectID goid,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters
		);
		void OnGUITransition(const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks, std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters);
		void OnGUIEventFunction(GameObjectID goid);
#endif// #ifdef USE_IMGUI

		// �u���b�N���g�̃C���f�b�N�X�Ə�������X�e�[�g�}�V���̃C���f�b�N�X��o�^����
		void Initialize(const int self_index, const int owner_statemachine_index);
		// ���g�̃C���f�b�N�X���擾����
		int GetBlockIndex() const;
		// ���g�̏�������X�e�[�g�}�V���̃C���f�b�N�X���擾����
		int GetOwnerStateMachineIndex() const;

		// �J�ڊ֌W
		// ���g�̏�������X�e�[�g�}�V���Ɠ��K�w�̃u���b�N�ւ̑J�ڂ܂ŔF�߂�
		std::shared_ptr<AnimatorTransition> AddTransition(const int destination_block_idx);
		std::shared_ptr<AnimatorTransition> AddTransition(const AnimatorTransition& transition);
		void RemoveTransition(int index);
		// �u���b�N�̎���Transition��Ԃ�
		std::vector<std::shared_ptr<AnimatorTransition>>* GetTransitions();

		// ���O
		const char* GetName() const;
		void SetName(const std::string& name);

		// �u���b�N�̏�Ԃ̍X�V(clip���Ȃ����false)
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
		// �A�j���[�V�����̏��̍X�V(clip���Ȃ����false)
		virtual bool UpdateKeyFrame(
			const bool is_root_layer,
			const bool is_additive,
			const float normalize_time,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			const std::unordered_map<std::string, int>* node_index,
			std::vector<KeyData>& dst_keydata
		) = 0;

		// ���̃u���b�N��speed��length���l�������t���[�����K���o�ߎ��ԂƁAspeed���l�����Ȃ����K���o�ߎ��Ԃ�Ԃ�
		virtual void GetNormalizeTime(
			const float elapsed_time,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters,
			float& weighted_normalize_time,
			float& normalize_time
		) = 0;
		// ���C���[���̂����ꂩ��Block�폜���Ƀu���b�N���Ƃɍ폜���̏��������s����
		virtual void OnRemoveBlock(int removed_index, std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks) = 0;
		// �u���b�N�̊J�n���̏���
		virtual void OnReady(const float offset) = 0;
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
		/// <param name="duration">�J�ڂɂ����鐳�K�����Ԃ�Ԃ�</param>
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

		// Enter���̃C�x���g�֐����Ă�
		void ExecuteEnterFunction(GameObjectID goid) const;
		// Exit���̃C�x���g�֐����Ă�
		void ExecuteExitFunction(GameObjectID goid) const;


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */

	protected:
		// ==============================================
		// protected methods
		// ==============================================
		// 2�̃A�j���[�V�������������Ńu�����h����
		void BlendKeyFrame(
			const std::vector<KeyData>& v1,
			const std::vector<KeyData>& v2,
			const float ratio,
			std::vector<KeyData>& dst
		);
		// 2�̃A�j���[�V�������������Ńu�����h���Ȃ���AAvatar���̃f�[�^���ɒ���
		void BlendKeyFrameAndAlign(
			const std::vector<KeyData>& v1,
			const std::vector<KeyData>& v2,
			const float ratio,
			const std::unordered_map<std::string, int>* key_index,	// Motion�f�[�^���̔z��T��
			const std::unordered_map<std::string, int>* node_index,	// Avatar�f�[�^���̔z��T��
			std::vector<KeyData>& dst
		);

		// Avatar���̃f�[�^���ɒ���
		void AlignKeyFrame(
			const std::vector<KeyData>& v,
			const std::unordered_map<std::string, int>* key_index,	// Motion�f�[�^���̔z��T��
			const std::unordered_map<std::string, int>* node_index,	// Avatar�f�[�^���̔z��T��
			std::vector<KeyData>& dst
		);

		//�t���[���Ԃ̕�Ԃ������L�[���擾����
		void GetFrameBlendKey(const AnimationClip& clip, const float key, const std::unordered_map<std::string, int>* node_index, std::vector<KeyData>& dst);

		// 2�̃L�[�f�[�^�̍���(v - clip)�����߂�
		void GetKeyDifference(
			const std::vector<KeyData>& v,
			const std::vector<KeyData>& clip,
			const std::unordered_map<std::string, int>* key_index,	// Motion�f�[�^���̔z��T��
			const std::unordered_map<std::string, int>* node_index,	// Avatar�f�[�^���̔z��T��
			std::vector<KeyData>& dst
		);

		// �u���b�N�폜���ɌĂ΂��
		// ***�S�Ẵu���b�N�̃C���f�b�N�X�̏C��
		// ***���g�̍폜������
		void OnRemoveBlockAllFixAndErase(const int removed_index, std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks);
		// �폜�����C���f�b�N�X�ɉ����ăC���f�b�N�X���C�����鏈��
		virtual void OnRemoveBlockFixIndex(const int removed_index, const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks);

		// �u���b�N�̎�ނ�Ԃ�
		BlockType GetBlockType() const;


		// ==============================================
		// protected variables
		// ==============================================
		// �u���b�N�̎��s���
		BlockState m_block_state = BlockState::Ready;

		// ���g�̃u���b�NID
		int m_self_block_index = INVALID_BLOCK_INDEX;
		// ��������X�e�[�g�}�V���̃u���b�NID
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
		// �u���b�N��
		std::string m_name;
		// �u���b�N����o�Ă���Transition
		std::vector<std::shared_ptr<AnimatorTransition>> m_transitions;
		// �u���b�N�̑���
		BlockType m_block_type = BlockType::State;

		// Enter�C�x���g�֐�
		bool m_call_enter_function = false;
		std::string m_enter_class_name;
		std::string m_enter_func_name;
		// Exit�C�x���g�֐�
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