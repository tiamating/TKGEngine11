#include "BasicHeader.hlsli"

TEXTURE2D(color_tex, float4, TEXSLOT_COLOR);
TEXTURE2D(depth_tex, float, TEXSLOT_DEPTH);

float4 main(VS_Out pin) : SV_TARGET
{
	float4 tex_clr = color_tex.Sample(smp_linear_clamp, pin.uv);
	return tex_clr * pin.inst_color;
}