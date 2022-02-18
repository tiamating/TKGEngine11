#pragma once

#include "Animation_Defined.h"
#include "Animator_AvatarMask.h"
#include "Animator_State.h"
#include "Animator_StateMachine.h"
#include "Animator_BlendTree.h"

namespace TKGEngine::Animations
{
	/// <summary>
	/// ���f���ɓK�p����A�j���[�V�����̃}�X�N�ƃX�e�[�g�}�V���������C���[
	/// </summary>
	class AnimatorControllerLayer
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		AnimatorControllerLayer();
		AnimatorControllerLayer(const std::string& name);
		virtual ~AnimatorControllerLayer() = default;
		AnimatorControllerLayer(const AnimatorControllerLayer&) = delete;
		AnimatorControllerLayer& operator=(const AnimatorControllerLayer&) = delete;

#ifdef USE_IMGUI
		void OnGUIStateMachine(const GameObjectID goid, std::unordered_map<std::string, std::shared_ptr<Animations::AnimatorControllerParameter>>& parameters);
		void OnGUIMask(const std::vector<std::string>* bone_names, std::unordered_map<std::string, std::shared_ptr<Animations::AnimatorControllerParameter>>& parameters);
#endif // USE_IMGUI

		// ���C���[�̎��X�e�[�g�}�V����Ԃ�
		std::shared_ptr<AnimatorStateMachine> GetStateMachine();

		// �X�e�[�g�}�V����State��ǉ�����
		std::shared_ptr<AnimatorState> AddMotion(const std::string& motion_filepath);
		std::shared_ptr<AnimatorState> AddState(const std::string& name);
		std::shared_ptr<AnimatorState> AddState(int sm_index, const std::string& name);
		// �X�e�[�g�}�V����BlendTree��ǉ�����
		std::shared_ptr<BlendTree> AddBlendTree(const std::string& name);
		std::shared_ptr<BlendTree> AddBlendTree(int sm_index, const std::string& name);
		// �X�e�[�g�}�V����StateMachine��ǉ�����
		//std::shared_ptr<AnimatorStateMachine> AddStateMachine(const std::string& name);
		//std::shared_ptr<AnimatorStateMachine> AddStateMachine(int sm_index, const std::string& name);
		// �u���b�N�̍폜
		void RemoveBlock(const int index);

		// �A�o�^�[�}�X�N
		void SetAvatarMask(const AvatarMask& mask);
		// ���C���[�̏d�˕���ݒ肷��
		void SetBlendingMode(BlendingMode mode);
		// ���C���[��
		const char* GetName() const;
		void SetName(const std::string& name);
		// ���݂̃L�[�t���[���f�[�^���v�Z���ĕԂ�
		void ApplyAnimation(
			const GameObjectID goid,
			const float elapsed_time,
			const bool is_root,
			const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters,
			const std::unordered_map<std::string, int>* node_index,
			const std::vector<int>& enable_nodes,
			std::vector<KeyData>& data
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
			if (version == 1)
			{
				archive(
					CEREAL_NVP(m_use_weight_parameter),
					CEREAL_NVP(m_weight_parameter),
					CEREAL_NVP(m_layer_name),
					CEREAL_NVP(m_avatar_mask),
					CEREAL_NVP(m_blending_mode),
					CEREAL_NVP(m_blocks)
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
#ifdef USE_IMGUI
		// GUI��p�A���ݑI�𒆂̃X�e�[�g�}�V��index
		int m_gui_current_select_state_machine_index = ROOT_STATEMACHINE_BLOCK_INDEX;
#endif// #ifdef USE_IMGUI

		// Weight�Ƀp�����[�^����Z���邩
		bool m_use_weight_parameter = false;
		// Weight�ɏ�Z����p�����[�^��
		std::string m_weight_parameter;
		// ���C���[��
		std::string m_layer_name;
		// ���C���[�������̃}�X�N�f�[�^
		AvatarMask m_avatar_mask;
		// �u�����f�B���O���[�h�͐擪�̃��C���[�ł͖��������
		BlendingMode m_blending_mode = BlendingMode::Override;

		// ���C���[���ɑ��݂���S�ẴA�j���[�V�����u���b�N
		std::vector<std::shared_ptr<AnimatorBlockBase>> m_blocks;
	};


}// namespace TKGEngine::Animations

CEREAL_CLASS_VERSION(TKGEngine::Animations::AnimatorControllerLayer, 1);