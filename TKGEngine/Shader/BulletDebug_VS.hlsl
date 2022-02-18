#include "BulletDebug.hlsli"
#include "Utility_Func.hlsli"

VS_Out main(float3 pos : POSITION, float3 color : COLOR)
{
	VS_Out vout;
	
	vout.pos = mul(VP, float4(pos, 1.0));
	
	vout.color = float4(color, 1.0);
	
	return vout;
}