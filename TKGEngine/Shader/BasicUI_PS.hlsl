#include "BasicUI.hlsli"

float4 main(VS_Out pin) : SV_TARGET
{
	float4 ret_color = tex.Sample(smp_point_wrap, pin.uv) * pin.inst_color;

	return ret_color;
}