#pragma once

#include "Application/Objects/Shadow/ShadowMapBase.h"
#include "Application/Resource/inc/Shader_Defined.h"
#include "Application/Resource/inc/ITarget.h"
#include "Application/Resource/inc/ConstantBuffer.h"
#include "Utility/inc/template_thread.h"

namespace TKGEngine
{
	/// <summary>
	/// ���s�����V���h�E�}�b�v
	/// </summary>
	class PSSM
		: public ShadowMapBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		PSSM() = default;
		PSSM(LightType type);
		virtual ~PSSM() = default;
		PSSM(const PSSM&) = delete;
		PSSM& operator=(const PSSM&) = delete;


		// ShadowMap�̍쐬
		virtual void Create() override;

		// �e��`�悷��I�u�W�F�N�g���v�Z����
		virtual int CalculateShadowCaster(const std::vector<MainData>& scene_objects, int object_num, const std::shared_ptr<ICamera>& camera, const LightShadowData& data) override;
		// Shadow�`��
		virtual void RenderShadow(ID3D11DeviceContext* context, VertexBuffer& instance_buffer) override;
		// ���\�[�X���Z�b�g
		virtual void SetSRV(ID3D11DeviceContext* context, int slot, int index = 0) const override;
		// ���C�g�s���Ԃ�
		virtual const MATRIX& GetLVP(int index = 0) const override;
		virtual const MATRIX& GetLV(int index = 0) const override;
		virtual const MATRIX& GetLP(int index = 0) const override;
		// �V���h�E�}�b�v�̃T�C�Y��Ԃ�
		virtual float GetShadowSize(int index = 0) const override;
		// �����V���h�E�}�b�v���g�p����ꍇ�̕�������Ԃ�
		virtual unsigned GetCascadeNum() const override;
		// ���݂̃t���[���ɃV���h�E�}�b�v�̕`������s���邩
		virtual bool IsDrawShadowMap() const override;
		virtual bool IsDrawShadowMap(int index) const override;

		// Shadow�̃T���v�����O����Ԃ�
		virtual float GetSamplingRadius() const override;
		virtual int GetSamplingCount() const override;


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			if (version == 2)
			{
				archive(
					cereal::base_class<ShadowMapBase>(this),
					CEREAL_NVP(m_cascade_num),
					CEREAL_NVP(m_test_accurate),
					CEREAL_NVP(m_lambda),
					CEREAL_NVP(m_sampling_radius_uv),
					CEREAL_NVP(m_sampling_count)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		virtual void OnGUI() override;
#endif // USE_IMGUI

		// �������̃Z�b�g
		void SetCascadeNum(int num);
		// �V���h�E�}�b�v�̃N���A
		void ClearFrameData(ID3D11DeviceContext* context);
		// ���C�g�̃r���[�s������߂�
		MATRIX CalculateLightView(const VECTOR3& light_dir, const VECTOR3& light_up, const VECTOR3& light_pos);
		// �����ʒu�����߂�
		void CalculateSplitPosition(float near_plane, float far_plane);
		// ���C�g�r���[��Ԃł̉e�𗎂Ƃ��I�u�W�F�N�g���ŏ��Ŏ��܂�AABB���v�Z���� (�I�u�W�F�N�g�����݂��Ȃ��Ȃ�t���X�^����AABB��Ԃ�)
		Bounds CalculateCasterAABBInLightViewSpace(
			const std::vector<MainData>& casters,
			const Frustum& frustum,
			const MATRIX& light_view
		);
		// �L���X�^�[AABB���烉�C�g�v���W�F�N�V�����s����v�Z����
		MATRIX CalculateLightProjectionMatrix(const Bounds& caster_aabb, const LightShadowData& data);

		// �N���b�vAABB�ɓK������悤�Ƀ��C�g�r���[�s��ƃ��C�g�v���W�F�N�V�����s����C������
		void FixLightViewProjectionMatrix(const Frustum& split_frustum, const std::vector<MainData>& casters, MATRIX& view, MATRIX& proj, MATRIX& LVP);
		// �N���b�vAABB�ɍ����悤�ɍs����C������
		void AdjustCropAABB(const Bounds& crop_aabb, MATRIX& view, MATRIX& proj, MATRIX& LVP);
		// �ŏI�I��LVP�ŃL���X�^�[�����߂�
		void CalculateCastersWithLVP(const std::vector<MainData>& scene_objects, const int object_num, std::vector<MainData>& casters, MATRIX& view, MATRIX& proj, MATRIX& LVP);
		// �����͈͂��Ƃ�LVP�����߂�
		void CalculateSplitLVP(int index, const MATRIX& light_view, const std::vector<MainData>& scene_objects, int object_num, const std::shared_ptr<ICamera>& camera, const LightShadowData& data);


		// ==============================================
		// private variables
		// ==============================================
#ifdef USE_IMGUI
		bool m_on_gui_is_init = false;
		int m_on_gui_prev_cascade_num = MAX_CASCADE;
#endif // USE_IMGUI

		// �V���h�E�}�b�v
		//std::unique_ptr<IColorTarget> m_color_targets[MAX_CASCADE];
		std::unique_ptr<IDepthTarget> m_depth_targets[MAX_CASCADE];
		// LVP�pCBuffer
		ConstantBuffer m_cb_LVPs[MAX_CASCADE];
		// ������������̉e�𗎂Ƃ��I�u�W�F�N�g���X�g
		std::vector<MainData> m_casters[MAX_CASCADE];
		// ������̃��C�g�s��
		MATRIX m_split_view_projection_matrices[MAX_CASCADE];
		MATRIX m_split_view_matrices[MAX_CASCADE];
		MATRIX m_split_projection_matrices[MAX_CASCADE];
		// ������(1 <= x <= MAX_CASCADE)
		int m_cascade_num = MAX_CASCADE;
		// �������ʂ̈ʒu
		float m_split_positions[MAX_CASCADE + 1] = { 0.0f };
		// �V���h�E�}�b�v�ɕ`�悳��Ă��邩
		bool m_is_rendered_target[MAX_CASCADE] = { false };
		// �V���h�E�}�b�v��`�悷�邩
		bool m_draw_shadow_map[MAX_CASCADE] = { false };

		// �e�X�g�̐��m��
		bool m_test_accurate = false;
		// �ΐ������X�L�[���Ƌψꕪ���X�L�[���̓K�p����(0 < �� < 1)
		float m_lambda = 0.5f;
		// �T���v�����O���
		float m_sampling_radius_uv = 1.0f;
		int m_sampling_count = 1;

		// Split���ƂɌv�Z���邽�߂̃X���b�h
		ThreadPool m_calculate_split_LVP_thread = ThreadPool(MAX_CASCADE);
		std::vector<std::future<void>> m_thread_result = std::vector<std::future<void>>();
	};
}

CEREAL_CLASS_VERSION(TKGEngine::PSSM, 2)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::PSSM, "TKGEngine::PSSM")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::ShadowMapBase, TKGEngine::PSSM)