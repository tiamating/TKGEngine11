#include "BasicPBR.hlsli"

VS_Out main(float3 position : POSITION, float3 normal : NORMAL, float3 tangent : TANGENT, float2 uv : TEXCOORD0, MainInstance instance)
{
	VS_Out vout = (VS_Out) 0;

	// 座標
	vout.w_position = mul(instance.transform_matrix, float4(position.xyz, 1.0)).xyz;
	vout.position = mul(VP, float4(vout.w_position, 1.0));
	// 接ベクトル
	const float3x3 inv_transform_matrix_3x3 = (float3x3) Invert(instance.transform_matrix);
	vout.normal = normalize(mul(normalize(normal), inv_transform_matrix_3x3).xyz);
	vout.tangent = normalize(mul(normalize(tangent), inv_transform_matrix_3x3).xyz);
	vout.binormal = normalize(cross(vout.tangent, vout.normal));

	vout.uv = uv * tex_param.tiling + tex_param.offset;
	vout.inst_color = instance.instance_color;

	return vout;
}