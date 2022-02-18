#include "SkyBox.hlsli"

TEXTURECUBE(env, float4, TEXSLOT_GLOBALENVMAP);

float4 main(PS_IN pin) : SV_TARGET
{
	return env.Sample(smp_aniso_wrap, pin.wpos.xyz);

}