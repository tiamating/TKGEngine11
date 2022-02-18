#include "ActionArea.hlsli"


float4 main(VS_Out pin) : SV_TARGET
{
	float4 ret_color = float4(color, 1.0);

	// 側面のみ描画
	const float3 vec_up = float3(0.0, 1.0, 0.0);
	const float face_weight = 1.0 - abs(dot(vec_up, pin.norm.xyz));

	// ノイズを時間でスクロールする
	const float noise = noise_tex.Sample(smp_linear_wrap, pin.uv + float2(0.0, time.y)).r;
	
	ret_color.rgb = ret_color.rgb * pin.inst_color.rgb;
	ret_color.a = face_weight * noise;

	return ret_color;
}