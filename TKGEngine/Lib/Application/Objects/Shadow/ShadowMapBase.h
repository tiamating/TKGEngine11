#pragma once

#include "Systems/inc/TKGEngine_Defined.h"
#include "Application/Objects/inc/Light_Defined.h"
#include "Application/Objects/Components/interface/IRenderer.h"
#include "Application/Objects/Shadow/LightShadowBase.h"

struct ID3D11DeviceContext;

namespace TKGEngine
{
	class ICamera;

	/// <summary>
	/// ShadowMap�̃x�[�X�N���X
	/// </summary>
	class ShadowMapBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		ShadowMapBase() = default;
		// LightType�ɉ�����LightShadowBase���쐬
		ShadowMapBase(LightType type);
		virtual ~ShadowMapBase() = default;
		ShadowMapBase(const ShadowMapBase&) = delete;
		ShadowMapBase& operator=(const ShadowMapBase&) = delete;

#ifdef USE_IMGUI
		virtual void OnGUI() = 0;
#endif // USE_IMGUI

		// ShadowMap�̍쐬
		virtual void Create() = 0;

		// �e��`�悷��I�u�W�F�N�g���v�Z�A�`��J�E���g��Ԃ�
		virtual int CalculateShadowCaster(const std::vector<MainData>& scene_objects, int object_num, const std::shared_ptr<ICamera>& camera, const LightShadowData& data) = 0;
		// Shadow�`��
		virtual void RenderShadow(ID3D11DeviceContext* context, VertexBuffer& instance_buffer) = 0;
		// ���\�[�X���Z�b�g
		virtual void SetSRV(ID3D11DeviceContext* context, int slot, int index = 0) const = 0;
		// ���C�g�s���Ԃ�
		virtual const MATRIX& GetLVP(int index = 0) const = 0;
		virtual const MATRIX& GetLV(int index = 0) const = 0;
		virtual const MATRIX& GetLP(int index = 0) const = 0;
		// �V���h�E�}�b�v�̃T�C�Y��Ԃ�
		virtual float GetShadowSize(int index = 0) const = 0;
		// �����V���h�E�}�b�v���g�p����ꍇ�̕�������Ԃ�
		virtual unsigned GetCascadeNum() const = 0;

		// ���C�g�̎�ނ�ύX����
		void ChangeLightType(LightType type);

		// ���݂̃t���[���ɃV���h�E�}�b�v�̕`������s���邩
		virtual bool IsDrawShadowMap() const = 0;
		virtual bool IsDrawShadowMap(int index) const = 0;

		// Shadow�̃T���v�����O����Ԃ�
		virtual float GetSamplingRadius() const = 0;
		virtual int GetSamplingCount() const = 0;

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	protected:
		// ==============================================
		// protected methods
		// ==============================================
		/* nothing */


		// ==============================================
		// protected variables
		// ==============================================

		// ���C�g�̌`�󂲂Ƃ̃V���h�E�v�Z�p�N���X
		std::unique_ptr<LightShadowBase> m_light_shadow_calculator = nullptr;

	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				//archive();
			}
		}
		// ==============================================
		// private methods
		// ==============================================

		/// <summary>
		/// ���C�g�̎�ނɉ������V���h�E�v�Z�N���X�𐶐�
		/// </summary>
		void CreateLightShadowCalculator(LightType type);


		// ==============================================
		// private variables
		// ==============================================
		/* nothing */

	};


}

CEREAL_CLASS_VERSION(TKGEngine::ShadowMapBase, 1)
