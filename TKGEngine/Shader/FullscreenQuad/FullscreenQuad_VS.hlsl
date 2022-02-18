
#include "FullscreenQuad.hlsli"

PSInput main(uint idx : SV_VertexID)
{
	PSInput vout = (PSInput) 0;

	// ’¸“_¶¬
	// 0:(-1,-1) 1:(+1,-1) 2:(-1,+1) 3:(+1,+1)
	vout.sv_pos = float4(((idx >> 0) & 0x01) * 2.0 - 1.0, ((idx >> 1) & 0x01) * 2.0 - 1.0, 1.0, 1.0);
	// UVŒvZ
	// 0:(0,1) 1:(1,1) 2:(0,0) 3:(1,0)
	vout.uv = float2(idx & 0x01, 1.0 - (idx >> 1 & 0x01));

	return vout;
}