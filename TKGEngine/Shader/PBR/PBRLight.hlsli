#ifndef _PBR_LIGHT_HLSLI_
#define _PBR_LIGHT_HLSLI_

#include "../ConstantBuffer_Defined.hlsli"
#include "../ReflectionModel.hlsli"
#include "../Utility_Math.hlsli"
#include "../Shadow.hlsli"


///////////////////////////////////////////////
// �|�C���g���C�g
// 
// point_light			:	�|�C���g���C�g
// diffuse_reflectance	:	���ˌ��̂����g�U���˂ɂȂ銄��
// F0					:	�������ˎ��̃t���l�����ːF
// w_position			:	�Ǝˈʒu���[���h���W
// w_normal				:	�Ǝˈʒu���[���h�@��(���K���ς�)
// normal				:	�@���x�N�g��(���K���ς�)
// eye					:	�����x�N�g��(���K���ς�)
// roughness			:	�e��
///////////////////////////////////////////////
void AddPointLightBRDF(
in PointLight point_light,
in float3 diffuse_reflectance,
in float3 F0,
in float3 w_position,
in float3 w_normal,
in float3 normal,
in float3 eye,
in float roughness,
inout float3 total_diffuse,
inout float3 total_specular)
{
	// �Ǝˈʒu�܂ł̋���
	const float3 light_to_pos = w_position - point_light.pos.xyz;
	const float distance = length(light_to_pos);
	// �e��x�N�g��
	const float3 v_light = -light_to_pos * InvertDivisionValue(distance);
	const float3 v_view = -eye;
	const float3 v_half = normalize(v_light + v_view);
	// ���ϒl�̌v�Z
	const float norm_dot_view = saturate(dot(normal, v_view));
	const float norm_dot_light = saturate(dot(normal, v_light));
	const float norm_dot_half = saturate(dot(normal, v_half));
	const float view_dot_half = saturate(dot(v_view, v_half));
	
	// ���ˏƓx����ˋP�x�ɕϊ�
	float3 irradiance = point_light.color.rgb * norm_dot_light;

	// ���C�g�J���[�������ʂłȂ�����PI����Z����
	// https://hanecci.hatenadiary.org/entry/20130604/p1
	irradiance *= PI;

	// ����������
	const float attenuation = CalcAttenuation(distance, point_light.range, point_light.attenuation);
	// �������l��
	irradiance *= attenuation;
	
	// �g�U����BRDF(���K�������o�[�g)
	total_diffuse += max(0.0, NormalizedLambertDiffuseBRDF(diffuse_reflectance) * irradiance);
	// ���ʔ���BRDF(Cook-Torrance)
	total_specular += max(0.0, CookTorranceSpecularBRDF(norm_dot_view, norm_dot_light, norm_dot_half, view_dot_half, F0, roughness) * irradiance);
}

///////////////////////////////////////////////
// �X�|�b�g���C�g
// 
// spot_light			:	�X�|�b�g���C�g
// diffuse_reflectance	:	���ˌ��̂����g�U���˂ɂȂ銄��
// F0					:	�������ˎ��̃t���l�����ːF
// w_position			:	�Ǝˈʒu���[���h���W
// w_normal				:	�Ǝˈʒu���[���h�@��(���K���ς�)
// normal				:	�@���x�N�g��(���K���ς�)
// eye					:	�����x�N�g��(���K���ς�)
// roughness			:	�e��
///////////////////////////////////////////////
void AddSpotLightBRDF(
in SpotLight spot_light,
in float3 diffuse_reflectance,
in float3 F0,
in float3 w_position,
in float3 w_normal,
in float3 normal,
in float3 eye,
in float roughness,
inout float3 total_diffuse,
inout float3 total_specular)
{
	// �Ǝˈʒu�܂ł̋���
	const float3 light_to_pos = w_position - spot_light.pos.xyz;
	const float distance = length(light_to_pos);
	// �e��x�N�g��
	const float3 v_light = -light_to_pos * InvertDivisionValue(distance);
	const float3 v_view = -eye;
	const float3 v_half = normalize(v_light + v_view);
	// ���ϒl�̌v�Z
	const float norm_dot_view = saturate(dot(normal, v_view));
	const float norm_dot_light = saturate(dot(normal, v_light));
	const float norm_dot_half = saturate(dot(normal, v_half));
	const float view_dot_half = saturate(dot(v_view, v_half));
	
	// ���ˏƓx����ˋP�x�ɕϊ�
	float3 irradiance = spot_light.color.rgb * norm_dot_light;

	// ���C�g�J���[�������ʂłȂ�����PI����Z����
	// https://hanecci.hatenadiary.org/entry/20130604/p1
	irradiance *= PI;

	// ����������
	const float attenuation = CalcAttenuation(distance, spot_light.range, spot_light.attenuation);
	// �p�x������
	float angle_attenuation = saturate(dot(-v_light, spot_light.direction) - spot_light.outer_cone_cos) * InvertDivisionValue(spot_light.inner_cone_cos - spot_light.outer_cone_cos);
	angle_attenuation = pow(abs(angle_attenuation), spot_light.falloff);
	// �������l��
	irradiance = irradiance * attenuation * angle_attenuation;
	
	// �g�U����BRDF(���K�������o�[�g)
	total_diffuse += max(0.0, NormalizedLambertDiffuseBRDF(diffuse_reflectance) * irradiance);
	// ���ʔ���BRDF(Cook-Torrance)
	total_specular += max(0.0, CookTorranceSpecularBRDF(norm_dot_view, norm_dot_light, norm_dot_half, view_dot_half, F0, roughness) * irradiance);
}

///////////////////////////////////////////////
// ���s���C�g
// 
// dir_light			:	���s���C�g
// diffuse_reflectance	:	���ˌ��̂����g�U���˂ɂȂ銄��
// F0					:	�������ˎ��̃t���l�����ːF
// w_position			:	�Ǝˈʒu���[���h���W
// w_normal				:	�Ǝˈʒu���[���h�@��(���K���ς�)
// normal				:	�@���x�N�g��(���K���ς�)
// eye					:	�����x�N�g��(���K���ς�)
// roughness			:	�e��
///////////////////////////////////////////////
void AddDirectionalLightBRDF(
in DirectionalLight dir_light,
in float3 diffuse_reflectance,
in float3 F0,
in float3 w_position,
in float3 w_normal,
in float3 normal,
in float3 eye,
in float roughness,
inout float3 total_diffuse,
inout float3 total_specular)
{
	// �e��x�N�g��
	const float3 v_light = normalize(-dir_light.direction.xyz);
	const float3 v_view = -eye;
	const float3 v_half = normalize(v_light + v_view);
	// ���ϒl�̌v�Z
	const float norm_dot_view = saturate(dot(normal, v_view));
	const float norm_dot_light = saturate(dot(normal, v_light));
	const float norm_dot_half = saturate(dot(normal, v_half));
	const float view_dot_half = saturate(dot(v_view, v_half));

	// ���ˏƓx����ˋP�x�ɕϊ�
	float3 irradiance = dir_light.color.rgb * norm_dot_light;

	// ���C�g�J���[�������ʂłȂ�����PI����Z����
	// https://hanecci.hatenadiary.org/entry/20130604/p1
	irradiance *= PI;

	// Shadow�̓K�p
	const float shadow_threshold = CalculateDirectionalLightShadow(dir_light, w_position, w_normal, v_light);
	const float light_threshold = 1.0 - saturate(shadow_threshold * dir_light.strength);
	irradiance *= light_threshold;
	// �V���h�E�����͌�������Օ�����Ă���
	const float specular_masking = 1.0 - shadow_threshold;

	// �g�U����BRDF(���K�������o�[�g)
	total_diffuse += max(0.0, NormalizedLambertDiffuseBRDF(diffuse_reflectance) * irradiance);
	// ���ʔ���BRDF(Cook-Torrance)
	total_specular += max(0.0, CookTorranceSpecularBRDF(norm_dot_view, norm_dot_light, norm_dot_half, view_dot_half, F0, roughness) * irradiance * specular_masking);
}

#endif// _PBR_LIGHT_HLSLI_