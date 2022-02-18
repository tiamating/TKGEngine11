#include "DebugGrid.hlsli"

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	float4 vout = mul(VP, pos);
	
	return vout;
}