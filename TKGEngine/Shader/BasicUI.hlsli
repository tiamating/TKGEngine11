#ifndef _BASICUI_HLSLI_
#define _BASICUI_HLSLI_

#include "Shader_Include.hlsli"

// SRV
TEXTURE2D(tex, float4, TEXSLOT_SPRITE);
// ~SRV

// Material
CBUFFER(CB_MATERIALNAME, CBS_MATERIAL)
{
	
	
}
// ~Material


struct VS_Out
{
	float4 pos : SV_Position;
	float4 norm : NORMAL;
	float4 inst_color : COLOR;
	float2 uv : TEXCOORD;
};

#endif// _BASICUI_HLSLI_