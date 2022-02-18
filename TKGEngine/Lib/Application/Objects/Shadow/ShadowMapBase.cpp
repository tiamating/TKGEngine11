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
				// TODO : スポットライトは要対応
			}
			break;
			case LightType::Point:
			{
				// TODO : ポイントライトは未対応

			}
			break;
		}
	}

}
