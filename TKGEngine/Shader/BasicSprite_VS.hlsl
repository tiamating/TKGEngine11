#include "BasicSprite.hlsli"
#include "Utility_Func.hlsli"

VS_Out main(float4 pos : POSITION, float4 norm : NORMAL, float2 uv : TEXCOORD, MainInstance inst)
{
	VS_Out vout;
	
	// Position
	vout.w_pos = mul(inst.transform_matrix, pos);
	vout.pos = mul(VP, vout.w_pos);
	// Texcoord
	vout.uv = uv * tex_param.tiling + tex_param.offset;
	// Color
	vout.inst_color = inst.instance_color;
	// Normal
	vout.norm = float4(norm.xyz, 0.0);
	
	return vout;
}