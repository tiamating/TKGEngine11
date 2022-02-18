#include "BasicHeader.hlsli"


VS_Out main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, MainInstance instance)
{
	VS_Out vout;
	
	vout.w_pos = mul(instance.transform_matrix, pos);
	vout.pos = mul(VP, vout.w_pos);

	vout.norm = float4(mul(float4(normalize(normal), 0.0), Invert(instance.transform_matrix)).xyz, 0.0);
	vout.uv = uv * tex_param.tiling + tex_param.offset;
	vout.inst_color = instance.instance_color;
	return vout;
}