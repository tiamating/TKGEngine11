#ifndef _LIGHT_HLSLI_
#define _LIGHT_HLSLI_

#include "ConstantBuffer_Defined.hlsli"
#include "ReflectionModel.hlsli"
#include "Utility_Math.hlsli"
#include "Shadow.hlsli"


///////////////////////////////////////////////
// �������C�e�B���O
// 
// w_normal		:	�Ǝˈʒu���[���h�@��(���K���ς�)
// sky_color	:	������F
// ground_color	:	�������F
// 
// return	:	���ːF
///////////////////////////////////////////////
float3 AddHemiSphereLight(in float3 w_normal, in float3 sky_color, in float3 ground_color)
{
	return lerp(ground_color, sky_color, (w_normal.y + 1.0) * 0.5);
}

///////////////////////////////////////////////
// �|�C���g���C�g
// https://www.3dgep.com/texturing-lighting-directx-11/#Point_Lights
// 
// point_light	:	�|�C���g���C�g
// w_position	:	�Ǝˈʒu���[���h���W
// w_normal		:	�Ǝˈʒu���[���h�@��(���K���ς�)
// normal		:	�@���x�N�g��(���K���ς�)
// eye			:	�����x�N�g��(���K���ς�)
// Kd			:	�g�U���˗�(0.0 ~ 1.0)
// Ks			:	���ʔ��˗�(0.0 ~ 1.0)
// power		:	���ʔ��ˎw��
// 
// return	:	���ːF
///////////////////////////////////////////////
float3 AddPointLight(in PointLight point_light, in float3 w_position, in float3 w_normal, in float3 eye, in float Kd, in float Ks, in float power)
{
	float total_luminance = (float) 0;
	
	// �Ǝˈʒu�܂ł̋���
	const float3 light_to_pos = w_position - point_light.pos.xyz;
	const float distance = length(light_to_pos);
	
	// ���C�g�x�N�g��
	const float3 v_light = light_to_pos * InvertDivisionValue(distance);
	
	// ����������
	const float attenuation = CalcAttenuation(distance, point_light.range, point_light.attenuation);
	
	// �g�U����(�����o�[�g)
	total_luminance += Lambert(v_light, w_normal, Kd);
	// ���ʔ���(Phong)
	total_luminance += PhongSpecular(v_light, w_normal, eye, Ks, power);
	
	return max(0.0, total_luminance * attenuation) * point_light.color.rgb;
}

///////////////////////////////////////////////
// �X�|�b�g���C�g
// https://docs.microsoft.com/ja-jp/windows/uwp/graphics-concepts/light-types
// 
// spot_light	:	�X�|�b�g���C�g
// w_position	:	�Ǝˈʒu���[���h���W
// w_normal		:	�Ǝˈʒu���[���h�@��(���K���ς�)
// normal		:	�@���x�N�g��(���K���ς�)
// eye			:	�����x�N�g��(���K���ς�)
// Kd			:	�g�U���˗�(0.0 ~ 1.0)
// Ks			:	���ʔ��˗�(0.0 ~ 1.0)
// power		:	���ʔ��ˎw��
// 
// return	:	���ːF
///////////////////////////////////////////////
float3 AddSpotLight(in SpotLight spot_light, in float3 w_position, in float3 w_normal, in float3 eye, in float Kd, in float Ks, in float power)
{
	float total_luminance = (float) 0;
	
	// �Ǝˈʒu�܂ł̋���
	const float3 light_to_pos = w_position - spot_light.pos.xyz;
	const float distance = length(light_to_pos);
	
	// ���C�g�x�N�g��
	const float3 v_light = light_to_pos * InvertDivisionValue(distance);
	
	// ����������
	const float dist_attenuation = CalcAttenuation(distance, spot_light.range, spot_light.attenuation);
	// �p�x������
	float angle_attenuation = saturate(dot(v_light, spot_light.direction) - spot_light.outer_cone_cos) * InvertDivisionValue(spot_light.inner_cone_cos - spot_light.outer_cone_cos);
	angle_attenuation = pow(abs(angle_attenuation), spot_light.falloff);
	
	// �g�U����(�����o�[�g)
	total_luminance += Lambert(v_light, w_normal, Kd);
	// ���ʔ���(Blinn-Phong)
	total_luminance += PhongSpecular(v_light, w_normal, eye, Ks, power);
	
	return max(0.0, total_luminance * dist_attenuation * angle_attenuation) * spot_light.color.rgb;
}

///////////////////////////////////////////////
// ���s���C�g
// 
// dir_light	:	���s���C�g
// w_position	:	�Ǝˈʒu���[���h���W
// w_normal		:	�Ǝˈʒu���[���h�@��(���K���ς�)
// normal		:	�@���x�N�g��(���K���ς�)
// eye			:	�����x�N�g��(���K���ς�)
// Kd			:	�g�U���˗�(0.0 ~ 1.0)
// Ks			:	���ʔ��˗�(0.0 ~ 1.0)
// power		:	���ʔ��ˎw��
// 
// return	:	���ːF
///////////////////////////////////////////////
float3 AddDirectionalLight(in DirectionalLight dir_light, in float3 w_position, in float3 w_normal, in float3 eye, in float Kd, in float Ks, in float power)
{
	const float3 v_light = normalize(dir_light.direction);

	// �g�U����(Half�����o�[�g)
	const float diffuse = HalfLambert(v_light, w_normal, Kd);
	// ���ʔ���(Blinn-Phong)
	float specular = BlinnPhongSpecular(v_light, eye, Ks, power);

	// Shadow�̓K�p
	const float shadow_threshold = CalculateDirectionalLightShadow(dir_light, w_position, w_normal, v_light);
	const float light_threshold = 1.0 - saturate(shadow_threshold * dir_light.strength);
	// �V���h�E�����͌�������Օ�����Ă���
	specular *= (1.0 - shadow_threshold);

	return dir_light.color.rgb * max(0.0, (diffuse + specular) * light_threshold);
}


#endif// _LIGHT_HLSLI_