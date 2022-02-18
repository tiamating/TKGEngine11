#ifndef _REFLECTION_MODEL_HLSLI_
#define _REFLECTION_MODEL_HLSLI_

#include "Utility_Math.hlsli"
#include "PBR/CookTorrance.hlsli"

///////////////////////////////////////////////
// ランバート反射
// 
// light	:	入射ベクトル(正規化済み)
// normal	:	法線ベクトル(正規化済み)
// Kd		:	拡散反射率(0.0 ~ 1.0)
// 
// return	:	拡散反射輝度
///////////////////////////////////////////////
float Lambert(in float3 light, in float3 normal, in float Kd)
{
	// 内積負は0とする
	return (max(0.0, dot(normal, -light)) * Kd);
}

///////////////////////////////////////////////
// ハーフ・ランバート反射
// 
// light	:	入射ベクトル(正規化済み)
// normal	:	法線ベクトル(正規化済み)
// Kd		:	拡散反射率(0.0 ~ 1.0)
// 
// return	:	拡散反射輝度
///////////////////////////////////////////////
float HalfLambert(in float3 light, in float3 normal, in float Kd)
{
	float half_light = 0.5 * dot(normal, -light) + 0.5;
	return (half_light * half_light * Kd);
}

///////////////////////////////////////////////
// 正規化ランバート拡散反射BRDF
// 
// diffuse_reflectance	:	入射光のうち拡散反射になる割合
// 
// return	:	拡散反射の反射光割合
///////////////////////////////////////////////
float3 NormalizedLambertDiffuseBRDF(in float3 diffuse_reflectance)
{
	return diffuse_reflectance / PI;
}

///////////////////////////////////////////////
// オーレン・ネイヤー反射
// https://ja.wikipedia.org/wiki/%E3%82%AA%E3%83%BC%E3%83%AC%E3%83%B3%E3%83%BB%E3%83%8D%E3%82%A4%E3%83%A4%E3%83%BC%E5%8F%8D%E5%B0%84
// 
// return	:	拡散反射輝度
///////////////////////////////////////////////





///////////////////////////////////////////////
// Phong鏡面反射
// 
// light	:	入射ベクトル(正規化済み)
// normal	:	法線ベクトル(正規化済み)
// eye		:	視線ベクトル(正規化済み)
// Ks		:	鏡面反射率(0.0 ~ 1.0)
// power	:	鏡面反射指数
// 
// return	:	鏡面反射輝度
///////////////////////////////////////////////
float PhongSpecular(in float3 light, in float3 normal, in float3 eye, in float Ks, in float power)
{
	float3 v_reflect = reflect(light, normal);
	float specular = max(0.0, dot(v_reflect, -eye));
	return pow(specular, power) * Ks;
}

///////////////////////////////////////////////
// Blinn-Phong鏡面反射
// 
// light	:	入射ベクトル(正規化済み)
// eye		:	視線ベクトル(正規化済み)
// Ks		:	鏡面反射率(0.0 ~ 1.0)
// power	:	鏡面反射指数
// 
// return	:	鏡面反射輝度
///////////////////////////////////////////////
float BlinnPhongSpecular(in float3 light, in float3 eye, in float Ks, in float power)
{
	float3 v_view = -eye;
	float3 v_half = HalfVector(v_view, light);
	float specular = max(0.0, dot(v_half, v_view));
	return pow(specular, power) * Ks;
}

///////////////////////////////////////////////
// Blinn鏡面反射
// https://www.slis.tsukuba.ac.jp/~fujisawa.makoto.fu/cgi-bin/wiki/index.php?GLSL
// 
// return	:	鏡面反射輝度
///////////////////////////////////////////////




///////////////////////////////////////////////
// Cook-Torrance鏡面反射
// https://www.slis.tsukuba.ac.jp/~fujisawa.makoto.fu/cgi-bin/wiki/index.php?GLSL
//
// norm_dot_view	:	法線ベクトルと視点へのベクトルの内積
// norm_dot_light	:	法線ベクトルと光源へのベクトルの内積
// norm_dot_half	:	法線ベクトルとハーフベクトルの内積
// view_dot_half	:	視点へのベクトルとハーフベクトルの内積
// fresnel_F0		:	垂直入射時のフレネル反射色
// roughness		:	粗さ
// 
// return	:	鏡面反射輝度
///////////////////////////////////////////////
float3 CookTorranceSpecularBRDF(
in float norm_dot_view,
in float norm_dot_light,
in float norm_dot_half,
in float view_dot_half,
in float3 fresnel_F0,
in float roughness)
{
	// D項
	const float D = D_GGX(roughness, norm_dot_half);
	// G項
	const float G = G_SmithSchlickGGX(roughness, norm_dot_view, norm_dot_light);
	// F項
	const float3 F = F_Schlick(fresnel_F0, view_dot_half);

	return (F * (D * G)) / max(EPSILON, 4.0 * norm_dot_light * norm_dot_view);
}

///////////////////////////////////////////////
// 環境光鏡面反射BRDF
// https://www.unrealengine.com/en-US/blog/physically-based-shading-on-mobile
//
// norm_dot_view	:	法線ベクトルと視点へのベクトルの内積
// fresnel_F0		:	垂直入射時のフレネル反射色
// roughness		:	粗さ
// 
// return	:	鏡面反射輝度
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