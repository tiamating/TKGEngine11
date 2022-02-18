
#include "FullscreenQuad.hlsli"

float4 main(PSInput pin) : SV_TARGET
{
	float4 color = output_tex.Sample(smp_point_clamp, pin.uv);

	return color;
}