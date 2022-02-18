#pragma once

#include "Animator_BlockBase.h"

#include "Animator_Clip.h"
#include "Animator_Event.h"

namespace TKGEngine::Animations
{
	class BlendTree;

	/// <summary>
	/// �����̃A�j���[�V�����N���b�v�𐳋K�����ԂŃu�����h����r���f�B���O�u���b�N
	/// </summary>
	class BlendTree
		: public AnimatorBlockBase
	{
		struct BlendData;
	public:
		// ==============================================
		// public enum
		// ==============================================
		/// <summary>
		/// �u�����h�c���[�Ŏg�p����u�����f�B���O�̎��
		/// </summary>
		enum class BlendTreeType
		{
			Simple1D,	// 1D
			Simple2D,	// ���������̃��[�V�����������Ȃ�2D
			FreeformCartesian2D,	// ���z�o���h��Ԃ���2D (http://runevision.com/thesis/rune_skovbo_johansen_thesis.pdf)
			FreeformCartesian3D,	// ���z�o���h��Ԃ���3D

			Max_BlendTreeType
		};

		// ==============================================
		// public methods
		// ==============================================
		BlendTree();
		BlendTree(const char* name);
		virtual ~BlendTree() = default;
		BlendTree(const BlendTree&) = delete;
		BlendTree& operator=(const BlendTree&) = delete;

		// �p�����[�^�����Z�b�g
		void SetParameter(const std::string& parameter);
		void SetParameterY(const std::string& parameter_y);
		void SetParameterZ(const std::string& parameter_z);

		// �q���[�V�����̒ǉ�
		void AddChild(const std::string& motion_filepath);
		void AddChild(const std::string& motion_filepath, float threshold);
		void AddChild(const std::string& motion_filepath, const VECTOR2& position);
		void AddChild(const std::string& motion_filepath, const VECTOR3& position);
		// �q���[�V�����̍폜
		void RemoveChild(int index);

		// �u�����h�c���[�̎��
		void SetBlendTreeType(BlendTreeType type);

		// �Đ����x(0�ȏ�)
		void Speed(const float speed);
		float Speed() const;

		// �J��Ԃ���
		// BlendTree�ł�Loop��Clamp�̂ݗL��
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
		// ==============================================
		// private struct
		// ==============================================

		/// <summary>
		/// �u�����h�Ɏg�p����q���[�V�����̃f�[�^
		/// </summary>
		struct BlendChildClip
		{
			// 臒l
			VECTOR3 threshold = VECTOR3::Zero;
			// �A�j���[�V�����N���b�v
			AnimationClip clip;

#ifdef USE_IMGUI
			void OnGUI(BlendTreeType blend_type, const std::vector<BlendData>& blend_data, const int self_index);
#endif// #ifdef USE_IMGUI

		private:
			friend class cereal::access;
			template <class Archive>
			void serialize(Archive& archive)
			{
				archive(CEREAL_NVP(threshold), CEREAL_NVP(clip));
			}
		};

		friend class cereal::access;
		template <class Archive>
		void load(Archive& archive, const std::uint32_t version)
		{
			if (version == 1)
			{
				archive(
					cereal::base_class<AnimatorBlockBase>(this),
					CEREAL_NVP(m_parameter),
					CEREAL_NVP(m_parameter_y),
					CEREAL_NVP(m_parameter_z),
					CEREAL_NVP(m_blend_tree_type),
					CEREAL_NVP(m_wrap_mode),
					CEREAL_NVP(m_speed),
					CEREAL_NVP(m_use_speed_parameter),
					CEREAL_NVP(m_speed_parameter),
					CEREAL_NVP(m_children),
					CEREAL_NVP(m_events)
				);
			}
			const auto child_num = m_children.size();
			// �u�����h�f�[�^�z������T�C�Y
			m_blend_data.resize(child_num);
			// �e���x�o�b�t�@�̃T�C�Y�ύX
			m_influences.resize(child_num);
		}
		template <class Archive>
		void save(Archive& archive, const std::uint32_t version) const
		{
			if (version == 1)
			{
				archive(
					cereal::base_class<AnimatorBlockBase>(this),
					CEREAL_NVP(m_parameter),
					CEREAL_NVP(m_parameter_y),
					CEREAL_NVP(m_parameter_z),
					CEREAL_NVP(m_blend_tree_type),
					CEREAL_NVP(m_wrap_mode),
					CEREAL_NVP(m_speed),
					CEREAL_NVP(m_use_speed_parameter),
					CEREAL_NVP(m_speed_parameter),
					CEREAL_NVP(m_children),
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
			std::unordered_map<std::string, std::shared_ptr<Animations::AnimatorControllerParameter>>& parameters
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

		// Simple 1D�̃u�����h���v�Z
		void CalculateBlendRatioSimple1D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters);
		// Simple 2D�̃u�����h���v�Z
		void CalculateBlendRatioSimple2D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters);
		// FreeformCartesian2D�̃u�����h���v�Z
		void CalculateBlendRatioFreeformCartesian2D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters);
		// FreeformCartesian3D�̃u�����h���v�Z
		void CalculateBlendRatioFreeformCartesian3D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters);
		// ���z�o���h��Ԃɂ��e�N���b�v�̉e�������߂�
		float CalculateClipInfluence(const VECTOR2& param, const int child_idx);
		float CalculateClipInfluence(const VECTOR3& param, const int child_idx);

		// �p�����[�^�f�[�^�T���A�擾�p
		float GetParameter1D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters) const;
		VECTOR2 GetParameter2D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters) const;
		VECTOR3 GetParameter3D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters) const;


		// ==============================================
		// private variables
		// ==============================================
		// �s����Children�̎Q�ƃC���f�b�N�X
		static constexpr int INVALID_CHILD_INDEX = -1;

		// �Q�Ƃ���p�����[�^��
		std::string m_parameter;
		std::string m_parameter_y;
		std::string m_parameter_z;
		// �u�����h�c���[�̎��
		BlendTreeType m_blend_tree_type = BlendTreeType::Simple1D;
		// �J��Ԃ���
		WrapMode m_wrap_mode = WrapMode::Loop;
		// 0.0f�ȏ�̒l��Motion�̍Đ����x
		float m_speed = 1.0f;
		// Speed�Ƀp�����[�^����Z���邩
		bool m_use_speed_parameter = false;
		// Speed�ɏ�Z����p�����[�^��
		std::string m_speed_parameter;
		// �A�j���[�V�����N���b�v���܂ރc���[�̎q���
		std::vector<BlendChildClip> m_children;
		// �C�x���g
		std::vector<AnimatorEvent> m_events;

		// �C�x���g��wrap�ɂ�锭�΃~�X���Ȃ������߂ɑO�t���[����normalize_time��ێ�����
		float m_prev_normalize_time = 0.0f;

		/// <summary>
		/// �u�����h����f�[�^
		/// </summary>
		struct BlendData
		{
			int child_index = INVALID_CHILD_INDEX;
			float weight = 0.0f;

			void Clear()
			{
				child_index = INVALID_CHILD_INDEX;
				weight = 0.0f;
			}
		};
		std::vector<BlendData> m_blend_data = std::vector<BlendData>();
		// �u�����h����f�[�^��
		int m_blend_data_num = 0;
		// ���z�o���h��Ԏ��ɉe���x�����Ă����o�b�t�@
		std::vector<float> m_influences = std::vector<float>();
		// �q���[�V�����f�[�^����擾�����A�j���[�V�������u�����h���邽�߂̃o�b�t�@
		std::vector<KeyData> m_calc_child_keyframes[2];
	};

}// namespace TKGEngine::Animations

// BlendTree
CEREAL_REGISTER_TYPE(TKGEngine::Animations::BlendTree)
CEREAL_CLASS_VERSION(TKGEngine::Animations::BlendTree, 1)
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Animations::AnimatorBlockBase, TKGEngine::Animations::BlendTree)
// archive����load save���g�p����B�������
CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(TKGEngine::Animations::BlendTree, cereal::specialization::member_load_save)
