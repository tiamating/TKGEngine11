#ifndef _BASIC_PBR_HLSLI_
#define _BASIC_PBR_HLSLI_

#include "../Shader_Include.hlsli"
#include "../Light.hlsli"
#include "PBRLight.hlsli"

// SRV
TEXTURE2D(albedo_tex, float4, TEXSLOT_RENDERER_BASECOLORMAP);
TEXTURE2D(normal_tex, float4, TEXSLOT_RENDERER_NORMALMAP);
TEXTURE2D(roughness_tex, float, TEXSLOT_RENDERER_ROUGHNESS);
TEXTURE2D(metalness_tex, float, TEXSLOT_RENDERER_METALNESS);
TEXTURECUBE(environment_tex, float4, TEXSLOT_GLOBALENVMAP);
// ~SRV

// —U“d‘Ì‚Ì‹¾–Ê”½ŽË—¦
static const float3 DIELECTRIC_REFLECTANCE = float3(0.04, 0.04, 0.04);

// Material
CBUFFER(CB_MATERIALNAME, CBS_MATERIAL)
{
	float3 color;
	float3 emissive_color;
	float emissive_intensity;
	float roughness;
	float metalness;
}
// ~Material

struct VS_Out
{
	float4 position : SV_Position;
	float3 w_position : Position;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float4 inst_color : COLOR;
	float2 uv : TEXCOORD;
};


#endif