#include "BasicUI.hlsli"

VS_Out main(float4 pos : POSITION, float4 norm : NORMAL, float2 uv : TEXCOORD, UIInstance inst)
{
	VS_Out vout;
	
	// Position
	vout.pos = mul(inst.transform_matrix, pos);
	vout.pos.z = 1.0;
	// Texcoord
	vout.uv = (uv * inst.texture_transform.zw) + inst.texture_transform.xy;
	// Color
	vout.inst_color = inst.instance_color;
	// Normal
	vout.norm = float4(norm.xyz, 0.0);
	
	return vout;
}