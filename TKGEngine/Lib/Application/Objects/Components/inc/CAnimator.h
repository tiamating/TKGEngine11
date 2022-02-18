#pragma once

#include "Components/inc/CBehaviour.h"
#include "../interface/IAnimator.h"

#include "Application/Resource/src/AnimatorController/Animation_Defined.h"
#include "Application/Resource/inc/AnimatorController.h"
#include "Application/Resource/inc/Avatar.h"
#include "Application/Resource/inc/ConstantBuffer.h"


struct ID3D11DeviceContext;

namespace TKGEngine
{
	class Transform;
	class IGameObject;


	/// <summary>
	/// Animation module Base.
	/// </summary>
	class Animator
		: public Behaviour
		, public IAnimator
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Animator(const Animator&) = delete;
		Animator& operator=(const Animator&) = delete;

		Animator();
		virtual ~Animator();


		void SetCB(ID3D11DeviceContext* p_context, int slot, bool set_cs);
		static void SetDefaultCB(ID3D11DeviceContext* p_context, int slot, bool set_cs);

		AnimatorController& GetController();
		void SetController(const std::string& filepath);
		void SetAvatar(const std::string& filepath);
		void SetAvatar(const Avatar& avatar);
		// Avatar�̓Z�b�g������폜���Ȃ��A���[�h������Ԃ̂܂܎g�p������̂Ƃ���
		//void RemoveAvatar();

		inline void SetCullingMode(CullingMode mode);
		inline CullingMode GetCullingMode() const override;

		// Bone��Transform���X�g��Ԃ�
		std::vector<std::shared_ptr<Transform>>& GetBoneReferences();

		int GetParameterCount() const;
		std::shared_ptr<Animations::AnimatorControllerParameter> GetParameter(const std::string& param_name);

		void SetBool(const std::string& param_name, bool param);
		void SetInteger(const std::string& param_name, int param);
		void SetFloat(const std::string& param_name, float param);
		void SetTrigger(const std::string& param_name);

		bool GetBool(const std::string& param_name);
		int GetInteger(const std::string& param_name);
		float GetFloat(const std::string& param_name);
		void ResetTrigger(const std::string& param_name);

		// �A�j���[�V�����̍X�V�Ɏg�p����^�C���X�P�[���̐ݒ�
		void SetUpdateMode(UpdateMode mode);


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
					cereal::base_class<Behaviour>(this),
					CEREAL_NVP(m_controller_filedata),
					CEREAL_NVP(m_avatar_filedata),
					CEREAL_NVP(m_root_bones),
					CEREAL_NVP(m_root_index),
					CEREAL_NVP(m_bone_references),
					CEREAL_NVP(m_node_references),
					CEREAL_NVP(m_update_mode),
					CEREAL_NVP(m_culling_mode),
					CEREAL_NVP(m_root_bone_index),
					CEREAL_NVP(m_root_bone_init_translate),
					CEREAL_NVP(m_root_bone_init_rotation),
					CEREAL_NVP(m_root_bone_init_scale),
					CEREAL_NVP(m_ignore_root_motion)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		void OnGUI() override;
#endif // USE_IMGUI

		void OnCreate(const std::shared_ptr<Object>& p_self) override;
		void OnDestroy() override;

		static void OnInitOnce();

		void OnAddAvatar();
		void OnRemoveAvatar();
		// OnAddAvatar���Ŏg�p����ċA����
		void RecurseCreateFromBone(const Node* node, const int self_idx, const std::shared_ptr<IGameObject>& parent, const bool is_root, int& counter);

		// Animation Matrix��RootBone���珇�Ɍv�Z���Ă����ċA����
		void RecurseCalculateAnimationMatrix(const Node* node, const int self_idx, const MATRIX& matrix);

		// IAnimator
		void ApplyAnimationTransform() override;
		void UpdateAnimationMatrix() override;
		bool IsActiveAnimator() override;
		// ~IAnimator

		// Behaviour
		void OnEnabled() override {};
		void OnDisabled() override {};
		// ~Behaviour


		// ==============================================
		// private variables
		// ==============================================
		// Manager�ɓo�^����Ă���ID
		int m_anim_id = -1;

		// �A�j���[�^�[�R���g���[���̏��
		AnimatorController m_controller;
		FileLoadStateData m_controller_filedata;

		Avatar m_avatar;
		FileLoadStateData m_avatar_filedata;
		std::vector<std::shared_ptr<IGameObject>> m_root_bones;
		
		// node : avatar����index, bone : animation�z���index
		std::vector<int> m_root_index;
		std::vector<std::shared_ptr<Transform>> m_bone_references;
		std::vector<int> m_node_references;
		std::vector<Animations::KeyData> m_current_key_data;

		// �X�V�Ɏg�p����^�C���X�P�[��
		UpdateMode m_update_mode = UpdateMode::Normal;
		// TODO : ��ʊO�ŃA�j���[�V�������s�����邩�̐ݒ�
		CullingMode m_culling_mode = CullingMode::AlwaysAnimation;
		// �I�������{�[���̃A�j���[�V������K�p���Ȃ�����ݒ�
		int m_root_bone_index = -1;
		VECTOR3 m_root_bone_init_translate = VECTOR3::Zero;
		Quaternion m_root_bone_init_rotation = Quaternion::Identity;
		VECTOR3 m_root_bone_init_scale = VECTOR3::One;
		bool m_ignore_root_motion = false;

		std::vector<MATRIX> m_cb_matrix_data;
		ConstantBuffer m_cb_model;
		static ConstantBuffer m_cb_default_model;
		static bool m_is_initialized;

	};

	// ------------------------------------------------------
	// inline
	// ------------------------------------------------------
	inline void Animator::SetCullingMode(CullingMode mode)
	{
		m_culling_mode = mode;
	}

	inline CullingMode Animator::GetCullingMode() const
	{
		return m_culling_mode;
	}

}// namespaace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::Animator, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::Animator, "TKGEngine::Animator")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Behaviour, TKGEngine::Animator)
