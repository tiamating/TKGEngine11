#ifndef _REFLECTION_MODEL_HLSLI_
#define _REFLECTION_MODEL_HLSLI_

#include "Utility_Math.hlsli"
#include "PBR/CookTorrance.hlsli"

///////////////////////////////////////////////
// �����o�[�g����
// 
// light	:	���˃x�N�g��(���K���ς�)
// normal	:	�@���x�N�g��(���K���ς�)
// Kd		:	�g�U���˗�(0.0 ~ 1.0)
// 
// return	:	�g�U���ˋP�x
///////////////////////////////////////////////
float Lambert(in float3 light, in float3 normal, in float Kd)
{
	// ���ϕ���0�Ƃ���
	return (max(0.0, dot(normal, -light)) * Kd);
}

///////////////////////////////////////////////
// �n�[�t�E�����o�[�g����
// 
// light	:	���˃x�N�g��(���K���ς�)
// normal	:	�@���x�N�g��(���K���ς�)
// Kd		:	�g�U���˗�(0.0 ~ 1.0)
// 
// return	:	�g�U���ˋP�x
///////////////////////////////////////////////
float HalfLambert(in float3 light, in float3 normal, in float Kd)
{
	float half_light = 0.5 * dot(normal, -light) + 0.5;
	return (half_light * half_light * Kd);
}

///////////////////////////////////////////////
// ���K�������o�[�g�g�U����BRDF
// 
// diffuse_reflectance	:	���ˌ��̂����g�U���˂ɂȂ銄��
// 
// return	:	�g�U���˂̔��ˌ�����
///////////////////////////////////////////////
float3 NormalizedLambertDiffuseBRDF(in float3 diffuse_reflectance)
{
	return diffuse_reflectance / PI;
}

///////////////////////////////////////////////
// �I�[�����E�l�C���[����
// https://ja.wikipedia.org/wiki/%E3%82%AA%E3%83%BC%E3%83%AC%E3%83%B3%E3%83%BB%E3%83%8D%E3%82%A4%E3%83%A4%E3%83%BC%E5%8F%8D%E5%B0%84
// 
// return	:	�g�U���ˋP�x
///////////////////////////////////////////////





///////////////////////////////////////////////
// Phong���ʔ���
// 
// light	:	���˃x�N�g��(���K���ς�)
// normal	:	�@���x�N�g��(���K���ς�)
// eye		:	�����x�N�g��(���K���ς�)
// Ks		:	���ʔ��˗�(0.0 ~ 1.0)
// power	:	���ʔ��ˎw��
// 
// return	:	���ʔ��ˋP�x
///////////////////////////////////////////////
float PhongSpecular(in float3 light, in float3 normal, in float3 eye, in float Ks, in float power)
{
	float3 v_reflect = reflect(light, normal);
	float specular = max(0.0, dot(v_reflect, -eye));
	return pow(specular, power) * Ks;
}

///////////////////////////////////////////////
// Blinn-Phong���ʔ���
// 
// light	:	���˃x�N�g��(���K���ς�)
// eye		:	�����x�N�g��(���K���ς�)
// Ks		:	���ʔ��˗�(0.0 ~ 1.0)
// power	:	���ʔ��ˎw��
// 
// return	:	���ʔ��ˋP�x
///////////////////////////////////////////////
float BlinnPhongSpecular(in float3 light, in float3 eye, in float Ks, in float power)
{
	float3 v_view = -eye;
	float3 v_half = HalfVector(v_view, light);
	float specular = max(0.0, dot(v_half, v_view));
	return pow(specular, power) * Ks;
}

///////////////////////////////////////////////
// Blinn���ʔ���
// https://www.slis.tsukuba.ac.jp/~fujisawa.makoto.fu/cgi-bin/wiki/index.php?GLSL
// 
// return	:	���ʔ��ˋP�x
///////////////////////////////////////////////




///////////////////////////////////////////////
// Cook-Torrance���ʔ���
// https://www.slis.tsukuba.ac.jp/~fujisawa.makoto.fu/cgi-bin/wiki/index.php?GLSL
//
// norm_dot_view	:	�@���x�N�g���Ǝ��_�ւ̃x�N�g���̓���
// norm_dot_light	:	�@���x�N�g���ƌ����ւ̃x�N�g���̓���
// norm_dot_half	:	�@���x�N�g���ƃn�[�t�x�N�g���̓���
// view_dot_half	:	���_�ւ̃x�N�g���ƃn�[�t�x�N�g���̓���
// fresnel_F0		:	�������ˎ��̃t���l�����ːF
// roughness		:	�e��
// 
// return	:	���ʔ��ˋP�x
///////////////////////////////////////////////
float3 CookTorranceSpecularBRDF(
in float norm_dot_view,
in float norm_dot_light,
in float norm_dot_half,
in float view_dot_half,
in float3 fresnel_F0,
in float roughness)
{
	// D��
	const float D = D_GGX(roughness, norm_dot_half);
	// G��
	const float G = G_SmithSchlickGGX(roughness, norm_dot_view, norm_dot_light);
	// F��
	const float3 F = F_Schlick(fresnel_F0, view_dot_half);

	return (F * (D * G)) / max(EPSILON, 4.0 * norm_dot_light * norm_dot_view);
}

///////////////////////////////////////////////
// �������ʔ���BRDF
// https://www.unrealengine.com/en-US/blog/physically-based-shading-on-mobile
//
// norm_dot_view	:	�@���x�N�g���Ǝ��_�ւ̃x�N�g���̓���
// fresnel_F0		:	�������ˎ��̃t���l�����ːF
// roughness		:	�e��
// 
// return	:	���ʔ��ˋP�x
///////////////////////////////////////////////
float3 EnvironmentBRDFApprox(in float3 fresnel_F0, in float roughness, in float norm_dot_view)
{
	const float4 c0 = float4(-1.0, -0.0275, -0.572, 0.022);
	const float4 c1 = float4(1.0, 0.0425, 1.04, -0.04);

	float4 r = roughness * c0 + c1;
	float a004 = min(r.x * r.x, exp2(-9.28 * norm_dot_view)) * r.x + r.y;
	float2 AB = float2(-1.04, 1.04) * a004 + r.zw;
	return fresnel_F0 * AB.x + AB.y;
}

#endif// _REFLECTION_MODEL_HLSLI_