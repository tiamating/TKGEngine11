#include "../PBR/BasicPBR.hlsli"

float4 main(VS_Out pin) : SV_TARGET
{
	float4 tex_clr = albedo_tex.Sample(smp_point_wrap, pin.uv);
	clip(tex_clr.a - tex_param.cutout_threshold - 0.0001);
	return tex_clr * pin.inst_color;
}