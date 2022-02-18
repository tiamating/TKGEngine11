#include "../Utility_Math.hlsli"

///////////////////////////////////////////////
// フレネル項
// 
// F0				:	垂直入射時の反射率
// view_dot_half	:	視線ベクトルとハーフベクトル
// 
// return	:	フレネル項
///////////////////////////////////////////////
float3 F_Schlick(in float3 F0, in float view_dot_half)
{
	return F0 + (1.0 - F0) * pow(1.0 - saturate(view_dot_half), 5.0);
}

///////////////////////////////////////////////
// マイクロファセット分布関数
// 
// roughness		:	粗さ
// norm_dot_half	:	法線ベクトルとハーフベクトル
// 
// return	:	マイクロファセット分布項
///////////////////////////////////////////////
float D_GGX(in float roughness, in float norm_dot_half)
{
	const float alpha = roughness * roughness;
	const float alpha2 = alpha * alpha;
	const float denom = (norm_dot_half * norm_dot_half) * (alpha2 - 1.0) + 1.0;
	return alpha2 * InvertDivisionValue(PI * denom * denom);
}

///////////////////////////////////////////////
// 幾何減衰項
// https://zenn.dev/mebiusbox/books/619c81d2fbeafd/viewer/7c1069
// https://qiita.com/emadurandal/items/76348ad118c36317ec5c
// 
// roughness		:	粗さ
// norm_dot_view	:	法線ベクトルと視点へのベクトル
// norm_dot_light	:	法線ベクトルと光源へのベクトル
// 
// return	:	マイクロファセット分布関数
///////////////////////////////////////////////
float G_SmithSchlickGGX(in float roughness, in float norm_dot_view, in float norm_dot_light)
{
	const float k = roughness * SQRT_2_DIV_PI;
	const float shadowing = norm_dot_light * InvertDivisionValue(norm_dot_light * (1.0 - k) + k);
	const float masking = norm_dot_view * InvertDivisionValue(norm_dot_view * (1.0 - k) + k);
	return shadowing * masking;

}