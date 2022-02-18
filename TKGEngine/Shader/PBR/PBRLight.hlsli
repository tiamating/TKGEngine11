#ifndef _PBR_LIGHT_HLSLI_
#define _PBR_LIGHT_HLSLI_

#include "../ConstantBuffer_Defined.hlsli"
#include "../ReflectionModel.hlsli"
#include "../Utility_Math.hlsli"
#include "../Shadow.hlsli"


///////////////////////////////////////////////
// ポイントライト
// 
// point_light			:	ポイントライト
// diffuse_reflectance	:	入射光のうち拡散反射になる割合
// F0					:	垂直入射時のフレネル反射色
// w_position			:	照射位置ワールド座標
// w_normal				:	照射位置ワールド法線(正規化済み)
// normal				:	法線ベクトル(正規化済み)
// eye					:	視線ベクトル(正規化済み)
// roughness			:	粗さ
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
	// 照射位置までの距離
	const float3 light_to_pos = w_position - point_light.pos.xyz;
	const float distance = length(light_to_pos);
	// 各種ベクトル
	const float3 v_light = -light_to_pos * InvertDivisionValue(distance);
	const float3 v_view = -eye;
	const float3 v_half = normalize(v_light + v_view);
	// 内積値の計算
	const float norm_dot_view = saturate(dot(normal, v_view));
	const float norm_dot_light = saturate(dot(normal, v_light));
	const float norm_dot_half = saturate(dot(normal, v_half));
	const float view_dot_half = saturate(dot(v_view, v_half));
	
	// 放射照度を放射輝度に変換
	float3 irradiance = point_light.color.rgb * norm_dot_light;

	// ライトカラーが物理量でない物はPIを乗算する
	// https://hanecci.hatenadiary.org/entry/20130604/p1
	irradiance *= PI;

	// 距離減衰率
	const float attenuation = CalcAttenuation(distance, point_light.range, point_light.attenuation);
	// 減衰を考慮
	irradiance *= attenuation;
	
	// 拡散反射BRDF(正規化ランバート)
	total_diffuse += max(0.0, NormalizedLambertDiffuseBRDF(diffuse_reflectance) * irradiance);
	// 鏡面反射BRDF(Cook-Torrance)
	total_specular += max(0.0, CookTorranceSpecularBRDF(norm_dot_view, norm_dot_light, norm_dot_half, view_dot_half, F0, roughness) * irradiance);
}

///////////////////////////////////////////////
// スポットライト
// 
// spot_light			:	スポットライト
// diffuse_reflectance	:	入射光のうち拡散反射になる割合
// F0					:	垂直入射時のフレネル反射色
// w_position			:	照射位置ワールド座標
// w_normal				:	照射位置ワールド法線(正規化済み)
// normal				:	法線ベクトル(正規化済み)
// eye					:	視線ベクトル(正規化済み)
// roughness			:	粗さ
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
	// 照射位置までの距離
	const float3 light_to_pos = w_position - spot_light.pos.xyz;
	const float distance = length(light_to_pos);
	// 各種ベクトル
	const float3 v_light = -light_to_pos * InvertDivisionValue(distance);
	const float3 v_view = -eye;
	const float3 v_half = normalize(v_light + v_view);
	// 内積値の計算
	const float norm_dot_view = saturate(dot(normal, v_view));
	const float norm_dot_light = saturate(dot(normal, v_light));
	const float norm_dot_half = saturate(dot(normal, v_half));
	const float view_dot_half = saturate(dot(v_view, v_half));
	
	// 放射照度を放射輝度に変換
	float3 irradiance = spot_light.color.rgb * norm_dot_light;

	// ライトカラーが物理量でない物はPIを乗算する
	// https://hanecci.hatenadiary.org/entry/20130604/p1
	irradiance *= PI;

	// 距離減衰率
	const float attenuation = CalcAttenuation(distance, spot_light.range, spot_light.attenuation);
	// 角度減衰率
	float angle_attenuation = saturate(dot(-v_light, spot_light.direction) - spot_light.outer_cone_cos) * InvertDivisionValue(spot_light.inner_cone_cos - spot_light.outer_cone_cos);
	angle_attenuation = pow(abs(angle_attenuation), spot_light.falloff);
	// 減衰を考慮
	irradiance = irradiance * attenuation * angle_attenuation;
	
	// 拡散反射BRDF(正規化ランバート)
	total_diffuse += max(0.0, NormalizedLambertDiffuseBRDF(diffuse_reflectance) * irradiance);
	// 鏡面反射BRDF(Cook-Torrance)
	total_specular += max(0.0, CookTorranceSpecularBRDF(norm_dot_view, norm_dot_light, norm_dot_half, view_dot_half, F0, roughness) * irradiance);
}

///////////////////////////////////////////////
// 平行ライト
// 
// dir_light			:	平行ライト
// diffuse_reflectance	:	入射光のうち拡散反射になる割合
// F0					:	垂直入射時のフレネル反射色
// w_position			:	照射位置ワールド座標
// w_normal				:	照射位置ワールド法線(正規化済み)
// normal				:	法線ベクトル(正規化済み)
// eye					:	視線ベクトル(正規化済み)
// roughness			:	粗さ
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
	// 各種ベクトル
	const float3 v_light = normalize(-dir_light.direction.xyz);
	const float3 v_view = -eye;
	const float3 v_half = normalize(v_light + v_view);
	// 内積値の計算
	const float norm_dot_view = saturate(dot(normal, v_view));
	const float norm_dot_light = saturate(dot(normal, v_light));
	const float norm_dot_half = saturate(dot(normal, v_half));
	const float view_dot_half = saturate(dot(v_view, v_half));

	// 放射照度を放射輝度に変換
	float3 irradiance = dir_light.color.rgb * norm_dot_light;

	// ライトカラーが物理量でない物はPIを乗算する
	// https://hanecci.hatenadiary.org/entry/20130604/p1
	irradiance *= PI;

	// Shadowの適用
	const float shadow_threshold = CalculateDirectionalLightShadow(dir_light, w_position, w_normal, v_light);
	const float light_threshold = 1.0 - saturate(shadow_threshold * dir_light.strength);
	irradiance *= light_threshold;
	// シャドウ部分は光源から遮蔽されている
	const float specular_masking = 1.0 - shadow_threshold;

	// 拡散反射BRDF(正規化ランバート)
	total_diffuse += max(0.0, NormalizedLambertDiffuseBRDF(diffuse_reflectance) * irradiance);
	// 鏡面反射BRDF(Cook-Torrance)
	total_specular += max(0.0, CookTorranceSpecularBRDF(norm_dot_view, norm_dot_light, norm_dot_half, view_dot_half, F0, roughness) * irradiance * specular_masking);
}

#endif// _PBR_LIGHT_HLSLI_