#ifndef _CONSTANTBUFFER_DEFINED_HLSLI_
#define _CONSTANTBUFFER_DEFINED_HLSLI_

#include "../Lib/Application/Resource/inc/Shader_Defined.h"
#include "Register_Defined.hlsli"

// CB_ENVIRONMENT (0)
struct Fog
{
	float4 color;
	float density;
	float start_distance;
	float end_distance;
	uint fog_type;
	bool use_fog;
};
CBUFFER(CB_ENVIRONMENT, CBS_ENVIRONMENT)
{
	float4 ambient_colors[2];

	Fog fog;
}

// CB_TIME (1)
CBUFFER(CB_TIME, CBS_TIME)
{
	float4 time;		// ( t/20, t, 2t, 3t )
	float4 sin_time;	// ( t/8, t/4, t/2, t ) 1周期
	float4 cos_time;	// ( t/8, t/4, t/2, t ) 1周期
	float4 delta_time; // ( dt, 1/dt, unscaled_dt, 1/unscaled_dt )
}

// CB_CAMERA (2)
struct Camera
{
	matrix V;
	matrix P;
	matrix IV;
	matrix IP;
	matrix IVP;
	matrix prev_VP;
	float4 param; // ( near, far, (far - near), 1 / (far - near) )
	float3 pos;
	float dummy;
	float3 dir;
	float dummy2;
	float target_width;
	float target_height;
	float dummy3;
	float dummy4;
};
CBUFFER(CB_CAMERA, CBS_CAMERA)
{
	Camera camera;
}

// CB_VP (3)
CBUFFER(CB_VP, CBS_VP)
{
	matrix VP;
}

// CB_LIGHT (4)
struct DirectionalLight
{
	matrix LVPs[MAX_CASCADE];// ライトビュープロジェクション
	matrix light_views[MAX_CASCADE];// ライトビュー
	matrix light_projs[MAX_CASCADE]; // ライトプロジェクション
	float4 color;			// ライト色
	float3 direction;		// ライト方向
	float strength;			// 影色の強度
	float4 shadow_size;		// シャドウマップのサイズ
	float4 inv_shadow_size; // シャドウマップのサイズの逆数
	float bias;				// シャドウバイアス
	float normal_bias;		// 法線方向のシャドウバイアス
	bool use_shadow_map;	// シャドウマップを使用するか
	uint cascade_num;		// 分割数
	bool4 has_caster;		// キャスターを持っているか
	float filter_radius_uv;	// サンプリングフィルタのUV半径
	uint sampling_count;	// サンプリング数(最大：MAX_POISSON)
	float dummy1;
	float dummy2;
};
struct SpotLight
{
	matrix LVP;				// ライトビュープロジェクション
	matrix light_view;		// ライトビュー
	matrix light_proj;		// ライトプロジェクション
	float4 pos;				// ライト位置
	float4 color;			// ライト色
	float3 direction;		// ライト方向
	float range;			// 範囲(0以下で無効なライト)
	float3 attenuation;		// 減衰係数(x:定数 y:一次 z:二次)
	float falloff;			// 減衰係数(inner --> outerへの減衰する形)
	float inner_cone_cos;	// cos(θ/2) (θ:内側円錐の広がり角)
	float outer_cone_cos;	// cos(θ/2) (θ:外側円錐の広がり角)
	bool use_light_mask;	// ライトマスクテクスチャを使用するか
	bool use_shadow_map;	// シャドウマップを使用するか
	float strength;			// 影色の強度
	float bias;				// シャドウバイアス
	float normal_bias;		// 法線方向のシャドウバイアス
	float shadow_size;	// シャドウマップのサイズ
	float inv_shadow_size;	// シャドウマップのサイズの逆数
	float dummy1;
	float dummy2;
	float dummy3;
};
struct PointLight
{
	matrix LVPs[2];		// ライトビュープロジェクション
	matrix light_views[2];	// ライトビュー
	matrix light_projs[2]; // ライトプロジェクション
	float4 pos;			// ライト位置
	float4 color;		// ライト色
	float3 attenuation; // 減衰係数(x:定数 y:一次 z:二次)
	float range;		// 範囲(0以下で無効なライト)
	bool use_shadow_map;// シャドウマップを使用するか
	float strength;		// 影色の強度
	float bias;			// シャドウバイアス
	float normal_bias; // 法線方向のシャドウバイアス
	float2 shadow_size; // シャドウマップのサイズ
	float2 inv_shadow_size; // シャドウマップのサイズの逆数
};
CBUFFER(CB_LIGHT, CBS_LIGHT)
{
	DirectionalLight directional_light;
	SpotLight spot_lights[MAX_SPOTLIGHT];
	PointLight point_lights[MAX_POINTLIGHT];
}

// CB_MODEL (5)
CBUFFER(CB_MODEL, CBS_MODEL)
{
	matrix bone_transforms[MAX_BONES];
}

// CB_TEXTURE (7)
struct Texture
{
	float2 offset;
	float2 tiling;
	float cutout_threshold;
	float dummy0;
	float dummy1;
	float dummy2;
};
CBUFFER(CB_TEXTURE, CBS_TEXTURE)
{
	Texture tex_param;
}


#endif// _CONSTANTBUFFER_DEFINED_HLSLI_