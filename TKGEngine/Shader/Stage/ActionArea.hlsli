#ifndef _ACTION_AREA_HLSLI_
#define _ACTION_AREA_HLSLI_

#include "../Shader_Include.hlsli"
#include "../Utility_Math.hlsli"

// SRV
TEXTURE2D(noise_tex, float4, TEXSLOT_RENDERER_ONDEMAND17);
// ~SRV

// Material
CBUFFER(CB_MATERIALNAME, CBS_MATERIAL)
{
	float3 color;
	float intensity;
}
// ~Material


struct VS_Out
{
	float4 pos : SV_Position;
	float4 w_pos : Position;
	float4 norm : NORMAL;
	float4 inst_color : COLOR;
	float2 uv : TEXCOORD;
};


#endif