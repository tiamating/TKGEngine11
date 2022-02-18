#pragma once

#include "Components/inc/CBehaviour.h"
#include "Shadow/ShadowMapBase.h"
#include "Application/Resource/inc/Texture.h"
#include "Application/Resource/inc/Asset_Defined.h"
#include "Application/Objects/inc/Light_Defined.h"

#include "Utility/inc/myfunc_vector.h"

namespace TKGEngine
{
	class ShadowMapBase;
	class LightManager;
	struct MainData;
	class ICamera;
	class VertexBuffer;

	/// <summary>
	/// Management Light parameter.
	/// </summary>
	class Light
		: public Behaviour
	{
		friend LightManager;

	public:
		// ==============================================
		// public methods
		// ==============================================
		Light(const Light&) = delete;
		Light& operator=(const Light&) = delete;

		Light();
		virtual ~Light();
		
		// Param
		// Type
		void SetType(const LightType type);
		LightType GetType() const;
		// Color (color = color * intensity)
		void SetColor(const VECTOR4& color);
		VECTOR4 GetColor() const;
		// Intensity
		void SetIntensity(float intensity);
		// Range
		void SetRange(float range);
		float GetRange() const;
		// Spot angle
		void SetSpotAngleInner(float angle/* �x */);
		float GetSpotAngleInner() const;
		void SetSpotAngleOuter(float angle/* �x */);
		float GetSpotAngleOuter() const;
		// Fall off
		void SetFalloff(float falloff);
		float GetFalloff() const;
		// Attenuation
		void SetAttenuation(float constant, float linear, float quadratic);
		VECTOR3 GetAttenuation() const;
		// Culling layer
		void SetCullingLayer(int mask);
		int GetCullingLayer() const;

		// ~Param

		// Shadow
		// �쐬���邩�̃t���O
		void SetCreateShadow(bool do_create);
		bool GetCreateShadow() const;
		// Render flag
		void SetRenderShadow(bool do_render);
		bool GetRenderShadow() const;
		// Strength
		void SetStrength(float strength);
		float GetStrength() const;
		// Bias
		void SetBias(float bias);
		float GetBias() const;
		// Normal Bias
		void SetNormalBias(float bias);
		float GetNormalBias() const;
		// Near plane
		void SetNearPlane(float near_plane);
		// SpotLight�p�A�}�X�N�e�N�X�`�����g�p���邩
		void UseLightMask(bool use);
		bool UseLightMask() const;
		// SpotLight�p�A�}�X�N�e�N�X�`�����o�C���h����
		void SetLightMaskTexture(ID3D11DeviceContext* context, int slot) const;
		// ���C�g�̍s���Ԃ�
		const MATRIX& GetLVP(int index = 0) const;
		const MATRIX& GetLV(int index = 0) const;
		const MATRIX& GetLP(int index = 0) const;
		// �V���h�E�}�b�v�̃T�C�Y��Ԃ�
		float GetShadowSize(int index = 0) const;
		// �����V���h�E�}�b�v���g�p����ꍇ�̕�������Ԃ�
		unsigned GetCascadeNum() const;
		// �V���h�E�}�b�v���V�F�[�_�[�X�e�[�W�Ƀo�C���h����
		void SetSRV(ID3D11DeviceContext* context, int slot, int index = 0) const;
		// ~Shadow


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		friend class cereal::access;
		template <class Archive>
		void load(Archive& archive, const std::uint32_t version)
		{
			if (version == 1)
			{
				// �����o�ǂݍ���
				archive(
					cereal::base_class<Behaviour>(this),
					CEREAL_NVP(m_light_type),
					CEREAL_NVP(m_light_color),
					CEREAL_NVP(m_intensity),
					CEREAL_NVP(m_range),
					CEREAL_NVP(m_spot_angle_outer),
					CEREAL_NVP(m_spot_angle_inner),
					CEREAL_NVP(m_falloff),
					CEREAL_NVP(m_constant_attenuation),
					CEREAL_NVP(m_linear_attenuation),
					CEREAL_NVP(m_quadratic_attenuation),
					CEREAL_NVP(m_culling_layer),
					CEREAL_NVP(m_use_mask_texture),
					CEREAL_NVP(m_mask_texture_data),
					CEREAL_NVP(m_do_create_shadow),
					CEREAL_NVP(m_is_render_shadow),
					CEREAL_NVP(m_strength),
					CEREAL_NVP(m_bias),
					CEREAL_NVP(m_normal_bias),
					CEREAL_NVP(m_shadow_near_plane),
					CEREAL_NVP(m_max_shadow_far_plane)
				);
				// ShadowMap�������o����Ă����烍�[�h
				if(m_do_create_shadow)
				{
					archive(m_shadow_map);
					m_shadow_map->Create();
					m_shadow_map->ChangeLightType(m_light_type);
				}
			}
		}
		template <class Archive>
		void save(Archive& archive, const std::uint32_t version) const
		{
			if (version == 1)
			{
				// �����o�ǂݍ���
				archive(
					cereal::base_class<Behaviour>(this),
					CEREAL_NVP(m_light_type),
					CEREAL_NVP(m_light_color),
					CEREAL_NVP(m_intensity),
					CEREAL_NVP(m_range),
					CEREAL_NVP(m_spot_angle_outer),
					CEREAL_NVP(m_spot_angle_inner),
					CEREAL_NVP(m_falloff),
					CEREAL_NVP(m_constant_attenuation),
					CEREAL_NVP(m_linear_attenuation),
					CEREAL_NVP(m_quadratic_attenuation),
					CEREAL_NVP(m_culling_layer),
					CEREAL_NVP(m_use_mask_texture),
					CEREAL_NVP(m_mask_texture_data),
					CEREAL_NVP(m_do_create_shadow),
					CEREAL_NVP(m_is_render_shadow),
					CEREAL_NVP(m_strength),
					CEREAL_NVP(m_bias),
					CEREAL_NVP(m_normal_bias),
					CEREAL_NVP(m_shadow_near_plane),
					CEREAL_NVP(m_max_shadow_far_plane)
				);
				// ShadowMap��ێ����Ă����珑���o��
				if(m_do_create_shadow)
				{
					archive(m_shadow_map);
				}
			}
		}

		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		void OnGUI() override;
		// �J�����O���C���[�I��GUI�p�֐�
		void OnGUICullingLayer();
#endif // USE_IMGUI

		void OnCreate(const std::shared_ptr<Object>& p_self) override;
		void OnDestroy() override;

		// Behaviour
		void OnEnabled() override {};
		void OnDisabled() override {};
		// ~Behaviour

		// �V���h�E�}�b�v�I�u�W�F�N�g���쐬
		void CreateShadowMapObject();
		// �V���h�E�}�b�v�I�u�W�F�N�g��j��
		void RemoveShadowMapObject();

		/// <summary>
		/// LightManager���ĂԁA�e�𗎂Ƃ��I�u�W�F�N�g�̌v�Z���s��
		/// </summary>
		/// <param name="scene_objects">�e��`�悷��I�u�W�F�N�g���X�g</param>
		/// <param name="object_num">�I�u�W�F�N�g���X�g���̃I�u�W�F�N�g��</param>
		/// <param name="camera">�`�撆�̃J�����̎Q��</param>
		/// <returns>�`�搔</returns>
		int CalculateShadowCaster(const std::vector<MainData>& scene_objects, int object_num, const std::shared_ptr<ICamera>& camera) const;

		// �e��`�悷��I�u�W�F�N�g���Ȃ����false
		bool IsRenderShadowCaster() const;
		bool IsRenderShadowCaster(int index) const;

		// Shadow�̃T���v�����O����Ԃ�
		float GetSamplingRadius() const;
		int GetSamplingCount() const;

		// LightManager���ĂԁA�e�`������s����
		void RenderShadowCaster(ID3D11DeviceContext* context, VertexBuffer& instance_buffer) const;


		// ==============================================
		// private variables
		// ==============================================
#ifdef USE_IMGUI
		bool m_is_opened_culling_layer_window = false;
#endif // USE_IMGUI

		// ���C�g�`��
		LightType m_light_type = LightType::Spot;
		// �J���[
		VECTOR4 m_light_color = VECTOR4::One;
		// ���x(0�ȏ�)
		float m_intensity = 1.0f;
		// �͈�(0�ȏ�)
		float m_range = 5.0f;
		// �X�|�b�g���C�g�̊O���p(�����p <= x <= 179)
		float m_spot_angle_outer = 30.0f;
		// �X�|�b�g���C�g�̓����p(1 <= x <= �O���p)
		float m_spot_angle_inner = 20.0f;

		// �����W��
		// (inner -> outer�ւ̌�������`)
		float m_falloff = 1.0f;
		// �萔
		float m_constant_attenuation = 0.0f;
		// ���`
		float m_linear_attenuation = 0.0f;
		// ��
		float m_quadratic_attenuation = 25.0f;
		
		// �J�����O���C���[
		int m_culling_layer = 0;

		// �}�X�N�e�N�X�`��
		bool m_use_mask_texture = false;
		Texture m_mask_texture;
		FileLoadStateData m_mask_texture_data;
		
		// Shadow
		// http://www.project-asura.com/program/d3d11/d3d11_008.html
		// https://docs.microsoft.com/ja-jp/windows/win32/dxtecharts/cascaded-shadow-maps
		// https://developer.nvidia.com/gpugems/gpugems3/part-ii-light-and-shadows/chapter-10-parallel-split-shadow-maps-programmable-gpus
		// ���C�g�ɑΉ������V���h�E�}�b�v�N���X
		std::unique_ptr<ShadowMapBase> m_shadow_map = nullptr;
		// �V���h�E�}�b�v���쐬���邩
		bool m_do_create_shadow = false;
		// �e��`�悷�邩
		bool m_is_render_shadow = false;
		// �e�̋���(0.0 <= x <= 1.0)
		float m_strength = 0.4f;
		// �o�C�A�X(0.0 <= x <= 1.0)
		float m_bias = 0.001f;
		// �@���o�C�A�X(0.0 <= x <= 1.0)
		float m_normal_bias = 0.005f;
		// ���C�g�r���[�̋ߕ���(0.01 <= x <= 10.0)
		float m_shadow_near_plane = 0.1f;
		// ���C�g�r���[�̉�����(min(range, far)����)
		float m_max_shadow_far_plane = 100.0f;
		// ~Shadow

	};

	// --------------------------------------------------------------
	// inline
	// --------------------------------------------------------------
	inline bool Light::GetCreateShadow() const
	{
		return m_do_create_shadow;
	}
	inline bool Light::GetRenderShadow() const
	{
		return m_is_render_shadow;
	}
	inline float Light::GetStrength() const
	{
		return m_strength;
	}
	inline float Light::GetBias() const
	{
		return m_bias;
	}
	inline float Light::GetNormalBias() const
	{
		return m_normal_bias;
	}
	inline void Light::UseLightMask(const bool use)
	{
		m_use_mask_texture = use;
	}
	inline bool Light::UseLightMask() const
	{
		return m_use_mask_texture;
	}
	
}// namespace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::Light, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::Light, "TKGEngine::Light")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Behaviour, TKGEngine::Light)
// archive����load save���g�p����B�������
CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(TKGEngine::Light, cereal::specialization::member_load_save)