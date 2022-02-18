#include "BasicHeader.hlsli"

float4 main(VS_Out pin) : SV_TARGET
{
	const float3 normal = normalize(pin.norm.xyz);
	const float3 v_camera = normalize(pin.w_pos.xyz - camera.pos.xyz);

	float4 tex_color = tex.Sample(smp_linear_wrap, pin.uv) * pin.inst_color * float4(color, 1.0);

	// Ambient Light
	const float3 ambient_light = AddHemiSphereLight(normal, ambient_colors[0].rgb, ambient_colors[1].rgb);
	
	// Dir Light
	float3 dir_light = AddDirectionalLight(directional_light, pin.w_pos.xyz, normal, v_camera, 1.0, 1.0, 20.0);

	// Spot Light
	float3 add_light_color = (float3) 0;
	int i = 0;
	[unroll]
	for (i = 0; i < MAX_SPOTLIGHT; ++i)
	{
		add_light_color += float3(AddSpotLight(spot_lights[i], pin.w_pos.xyz, normal, v_camera, 1.0, 1.0, 20.0));
	}
	// Point Light
	[unroll]
	for (i = 0; i < MAX_POINTLIGHT; ++i)
	{
		add_light_color += float3(AddPointLight(point_lights[i], pin.w_pos.xyz, normal, v_camera, 1.0, 1.0, 20.0));
	}

	return max(0.0, tex_color) * float4(dir_light, 1.0) + float4(add_light_color, 0.0) + float4(ambient_light, 0.0);
}