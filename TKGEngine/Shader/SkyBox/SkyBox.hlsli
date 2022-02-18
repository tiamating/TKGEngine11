#ifndef _SKYBOX_HLSLI_
#define _SKYBOX_HLSLI_

#include "../Shader_Include.hlsli"

struct PS_IN
{
	float4 sv_pos : SV_POSITION;
	float4 wpos : POSITION;
};

#endif// _CONSTANTBUFFER_DEFINED_HLSLI_