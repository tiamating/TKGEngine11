#ifndef _BASICSPRITE_HLSLI_
#define _BASICSPRITE_HLSLI_

#include "Shader_Include.hlsli"
#include "Light.hlsli"

// SRV
TEXTURE2D(tex, float4, TEXSLOT_SPRITE);
// ~SRV

// Material
CBUFFER(CB_MATERIALNAME, CBS_MATERIAL)
{
	float3 color;
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

#endif// _BASICSPRITE_HLSLI_