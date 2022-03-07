
#include "Skinning_Defined.h"

RAWBUFFER(vertex_buffer_pos, SKINNINGSLOT_IN_VERTEX_POS);
RAWBUFFER(vertex_buffer_nor, SKINNINGSLOT_IN_VERTEX_NOR);
RAWBUFFER(vertex_buffer_tan, SKINNINGSLOT_IN_VERTEX_TAN);
RAWBUFFER(vertex_buffer_bon, SKINNINGSLOT_IN_VERTEX_BON);
RAWBUFFER(vertex_buffer_wei, SKINNINGSLOT_IN_VERTEX_WEI);

RWRAWBUFFER(out_buffer_pos, SKINNINGSLOT_OUT_VERTEX_POS);
RWRAWBUFFER(out_buffer_nor, SKINNINGSLOT_OUT_VERTEX_NOR);
RWRAWBUFFER(out_buffer_tan, SKINNINGSLOT_OUT_VERTEX_TAN);

// データ位置のStride
static const uint BUFFER_STRIDE_POS_NOR_TAN = 12;
static const uint BUFFER_STRIDE_BON_WEI = 16;

void Skinning(inout float3 pos, inout float3 norm, inout float3 tan, in uint4 bones, in float4 weights)
{
	float4 position = 0;
	float3 normal = 0;
	float3 tangent = 0;
	
	[loop]
	for (uint i = 0; i < 4; ++i)
	{
		const matrix anim_mat = bone_transforms[bones[i]];
		
		position += mul(anim_mat, float4(pos.xyz, 1.0)) * weights[i];
		//normal += mul(norm.xyz, (float3x3) anim_mat) * weights[i];
		//tangent += mul(tan.xyz, (float3x3) anim_mat) * weights[i];
		normal += mul((float3x3) anim_mat, norm.xyz) * weights[i];
		tangent += mul((float3x3) anim_mat, tan.xyz) * weights[i];
	}
	pos.xyz = position.xyz;
	norm.xyz = normal.xyz;
	tan.xyz = tangent.xyz;
}

[numthreads(SKINNING_COMPUTE_THREADCOUNT, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	const uint fetch_address_pos_nor_tan = DTid.x * BUFFER_STRIDE_POS_NOR_TAN;
	const uint fetch_address_bon_wei = DTid.x * BUFFER_STRIDE_BON_WEI;
	
	// 入力データを取り出す
	uint4 pos_u = vertex_buffer_pos.Load4(fetch_address_pos_nor_tan);
	uint4 norm_u = vertex_buffer_nor.Load4(fetch_address_pos_nor_tan);
	uint4 tan_u = vertex_buffer_tan.Load4(fetch_address_pos_nor_tan);
	uint4 bones = vertex_buffer_bon.Load4(fetch_address_bon_wei);
	uint4 weights_u = vertex_buffer_wei.Load4(fetch_address_bon_wei);
	// データ型にキャスト
	float3 pos = asfloat(pos_u.xyz);
	float3 norm = asfloat(norm_u.xyz);
	float3 tan = asfloat(tan_u.xyz);
	float4 weights = asfloat(weights_u);
	
	// スキニング計算
	Skinning(pos, norm, tan, bones, weights);
	
	// データをバッファ型にキャスト
	pos_u.xyz = asuint(pos.xyz);
	norm_u.xyz = asuint(norm.xyz);
	tan_u.xyz = asuint(tan.xyz);
	
	// データの保存
	out_buffer_pos.Store3(fetch_address_pos_nor_tan, pos_u.xyz);
	out_buffer_nor.Store3(fetch_address_pos_nor_tan, norm_u.xyz);
	out_buffer_tan.Store3(fetch_address_pos_nor_tan, tan_u.xyz);
}