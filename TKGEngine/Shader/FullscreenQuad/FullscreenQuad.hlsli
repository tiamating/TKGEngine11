#include "../Shader_Include.hlsli"

TEXTURE2D(output_tex, float4, TEXSLOT_COLOR);

struct PSInput
{
	float4 sv_pos : SV_POSITION;
	float2 uv : TEXCOORD0;
};