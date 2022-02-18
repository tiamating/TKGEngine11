#include "ShadowMapBase.h"

//////////////////////////////////
// Shadow Type Header
//////////////////////////////////
#include "Application/Objects/Shadow/DirectionalLight/DirectionalLightShadow.h"
//////////////////////////////////

namespace TKGEngine
{
	ShadowMapBase::ShadowMapBase(LightType type)
	{
		CreateLightShadowCalculator(type);
	}

	void ShadowMapBase::ChangeLightType(LightType type)
	{
		CreateLightShadowCalculator(type);
	}

	void ShadowMapBase::CreateLightShadowCalculator(LightType type)
	{
		switch (type)
		{
			case LightType::Directional:
			{
				m_light_shadow_calculator = std::make_unique<DirectionalLightShadow>();
			}
			break;
			case LightType::Spot:
			{
				// TODO : �X�|�b�g���C�g�͗v�Ή�
			}
			break;
			case LightType::Point:
			{
				// TODO : �|�C���g���C�g�͖��Ή�

			}
			break;
		}
	}

}
