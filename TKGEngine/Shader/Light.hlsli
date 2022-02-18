#ifndef _LIGHT_HLSLI_
#define _LIGHT_HLSLI_

#include "ConstantBuffer_Defined.hlsli"
#include "ReflectionModel.hlsli"
#include "Utility_Math.hlsli"
#include "Shadow.hlsli"


///////////////////////////////////////////////
// 半球ライティング
// 
// w_normal		:	照射位置ワールド法線(正規化済み)
// sky_color	:	上方向色
// ground_color	:	下方向色
// 
// return	:	反射色
///////////////////////////////////////////////
float3 AddHemiSphereLight(in float3 w_normal, in float3 sky_color, in float3 ground_color)
{
	return lerp(ground_color, sky_color, (w_normal.y + 1.0) * 0.5);
}

///////////////////////////////////////////////
// ポイントライト
// https://www.3dgep.com/texturing-lighting-directx-11/#Point_Lights
// 
// point_light	:	ポイントライト
// w_position	:	照射位置ワールド座標
// w_normal		:	照射位置ワールド法線(正規化済み)
// normal		:	法線ベクトル(正規化済み)
// eye			:	視線ベクトル(正規化済み)
// Kd			:	拡散反射率(0.0 ~ 1.0)
// Ks			:	鏡面反射率(0.0 ~ 1.0)
// power		:	鏡面反射指数
// 
// return	:	反射色
///////////////////////////////////////////////
float3 AddPointLight(in PointLight point_light, in float3 w_position, in float3 w_normal, in float3 eye, in float Kd, in float Ks, in float power)
{
	float total_luminance = (float) 0;
	
	// 照射位置までの距離
	const float3 light_to_pos = w_position - point_light.pos.xyz;
	const float distance = length(light_to_pos);
	
	// ライトベクトル
	const float3 v_light = light_to_pos * InvertDivisionValue(distance);
	
	// 距離減衰率
	const float attenuation = CalcAttenuation(distance, point_light.range, point_light.attenuation);
	
	// 拡散反射(ランバート)
	total_luminance += Lambert(v_light, w_normal, Kd);
	// 鏡面反射(Phong)
	total_luminance += PhongSpecular(v_light, w_normal, eye, Ks, power);
	
	return max(0.0, total_luminance * attenuation) * point_light.color.rgb;
}

///////////////////////////////////////////////
// スポットライト
// https://docs.microsoft.com/ja-jp/windows/uwp/graphics-concepts/light-types
// 
// spot_light	:	スポットライト
// w_position	:	照射位置ワールド座標
// w_normal		:	照射位置ワールド法線(正規化済み)
// normal		:	法線ベクトル(正規化済み)
// eye			:	視線ベクトル(正規化済み)
// Kd			:	拡散反射率(0.0 ~ 1.0)
// Ks			:	鏡面反射率(0.0 ~ 1.0)
// power		:	鏡面反射指数
// 
// return	:	反射色
///////////////////////////////////////////////
float3 AddSpotLight(in SpotLight spot_light, in float3 w_position, in float3 w_normal, in float3 eye, in float Kd, in float Ks, in float power)
{
	float total_luminance = (float) 0;
	
	// 照射位置までの距離
	const float3 light_to_pos = w_position - spot_light.pos.xyz;
	const float distance = length(light_to_pos);
	
	// ライトベクトル
	const float3 v_light = light_to_pos * InvertDivisionValue(distance);
	
	// 距離減衰率
	const float dist_attenuation = CalcAttenuation(distance, spot_light.range, spot_light.attenuation);
	// 角度減衰率
	float angle_attenuation = saturate(dot(v_light, spot_light.direction) - spot_light.outer_cone_cos) * InvertDivisionValue(spot_light.inner_cone_cos - spot_light.outer_cone_cos);
	angle_attenuation = pow(abs(angle_attenuation), spot_light.falloff);
	
	// 拡散反射(ランバート)
	total_luminance += Lambert(v_light, w_normal, Kd);
	// 鏡面反射(Blinn-Phong)
	total_luminance += PhongSpecular(v_light, w_normal, eye, Ks, power);
	
	return max(0.0, total_luminance * dist_attenuation * angle_attenuation) * spot_light.color.rgb;
}

///////////////////////////////////////////////
// 平行ライト
// 
// dir_light	:	平行ライト
// w_position	:	照射位置ワールド座標
// w_normal		:	照射位置ワールド法線(正規化済み)
// normal		:	法線ベクトル(正規化済み)
// eye			:	視線ベクトル(正規化済み)
// Kd			:	拡散反射率(0.0 ~ 1.0)
// Ks			:	鏡面反射率(0.0 ~ 1.0)
// power		:	鏡面反射指数
// 
// return	:	反射色
///////////////////////////////////////////////
float3 AddDirectionalLight(in DirectionalLight dir_light, in float3 w_position, in float3 w_normal, in float3 eye, in float Kd, in float Ks, in float power)
{
	const float3 v_light = normalize(dir_light.direction);

	// 拡散反射(Halfランバート)
	const float diffuse = HalfLambert(v_light, w_normal, Kd);
	// 鏡面反射(Blinn-Phong)
	float specular = BlinnPhongSpecular(v_light, eye, Ks, power);

	// Shadowの適用
	const float shadow_threshold = CalculateDirectionalLightShadow(dir_light, w_position, w_normal, v_light);
	const float light_threshold = 1.0 - saturate(shadow_threshold * dir_light.strength);
	// シャドウ部分は光源から遮蔽されている
	specular *= (1.0 - shadow_threshold);

	return dir_light.color.rgb * max(0.0, (diffuse + specular) * light_threshold);
}


#endif// _LIGHT_HLSLI_