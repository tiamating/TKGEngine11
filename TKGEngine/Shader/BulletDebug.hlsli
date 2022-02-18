#ifndef _BULLETDEBUG_HLSLI_
#define _BULLETDEBUG_HLSLI_


#include "Shader_Include.hlsli"

// SRV
//TEXTURE2D(tex, float4, TEXSLOT_SPRITE);
// ~SRV

// Material
cbuffer CB_MATERIAL : register(b6)
{
	
}
// ~Material


struct VS_Out
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

#endif// _BULLETDEBUG_HLSLI_