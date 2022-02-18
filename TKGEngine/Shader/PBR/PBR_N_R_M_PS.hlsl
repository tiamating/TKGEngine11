#include "BasicPBR.hlsli"

float4 main(VS_Out pin) : SV_TARGET0
{
	// Albedo
	const float4 albedo_color = albedo_tex.Sample(smp_linear_wrap, pin.uv) * pin.inst_color * float4(color, 1.0);
	// Roughness
	const float roughness_param = saturate(roughness_tex.Sample(smp_linear_wrap, pin.uv).r + roughness);
	// Metalness
	const float metalness_param = saturate(metalness_tex.Sample(smp_linear_wrap, pin.uv).r + metalness);
	// ���ˌ��̂����g�U���˂ɂȂ銄��
	const float3 diffuse_reflectance = lerp(albedo_color.rgb, 0.0, metalness_param);
	// �������ˎ��̃t���l�����˗�
	const float3 F0 = lerp(DIELECTRIC_REFLECTANCE, albedo_color.rgb, metalness_param);

	// �e��x�N�g��
	const float3 w_normal = normalize(pin.normal.xyz);
	const float3 v_eye = normalize(pin.w_position.xyz - camera.pos.xyz);
	// �@���}�b�s���O
	float3 v_normal;
	{
		const float3 tx = normalize(pin.tangent);
		const float3 ty = normalize(pin.binormal);
		const float3 tz = normalize(pin.normal);
		// ���[���h��Ԃ���ڋ�Ԃɕϊ�����s��
		const float3x3 world_to_tangent_matrix = { tx, ty, tz };
		v_normal = normal_tex.Sample(smp_linear_wrap, pin.uv).rgb;
		v_normal = v_normal * 2.0 - 1.0;
		// �@���e�N�X�`������ϊ�
		v_normal = normalize(mul(v_normal, world_to_tangent_matrix));
	}

	// ���ڌ��̊g�U���˂Ƌ��ʔ���
	float3 direct_diffuse = 0;
	float3 direct_specular = 0;
	{
		// Directional Light
		AddDirectionalLightBRDF(directional_light, diffuse_reflectance, F0, pin.w_position, pin.normal, v_normal, v_eye, roughness_param, direct_diffuse, direct_specular);
		// Spot Light
		int dynamic_light_index = 0;
		[unroll]
		for (dynamic_light_index = 0; dynamic_light_index < MAX_SPOTLIGHT; ++dynamic_light_index)
		{
			AddSpotLightBRDF(spot_lights[dynamic_light_index], diffuse_reflectance, F0, pin.w_position, pin.normal, v_normal, v_eye, roughness_param, direct_diffuse, direct_specular);
		}
		// Point Light
		[unroll]
		for (dynamic_light_index = 0; dynamic_light_index < MAX_POINTLIGHT; ++dynamic_light_index)
		{
			AddPointLightBRDF(point_lights[dynamic_light_index], diffuse_reflectance, F0, pin.w_position, pin.normal, v_normal, v_eye, roughness_param, direct_diffuse, direct_specular);
		}
	}

	// �Ԑڌ��̊g�U����
	float3 indirect_diffuse = 0;
	{
		const float3 indirect_irradiance = AddHemiSphereLight(w_normal, ambient_colors[0].rgb, ambient_colors[1].rgb);
		indirect_diffuse = NormalizedLambertDiffuseBRDF(diffuse_reflectance) * indirect_irradiance;
	}
	// �Ԑڌ��̋��ʔ���(�f�肱��)
	float3 indirect_specular = 0;
	{
		// �e�N�X�`������~�b�v�}�b�v�����擾
		uint environment_width;
		uint environment_height;
		uint environment_mip_level;
		environment_tex.GetDimensions(0, environment_width, environment_height, environment_mip_level);
		const int specular_mip_level = lerp(2, environment_mip_level - 1, roughness_param);
		// �L���[�u�}�b�v����f�肱�݂̏����擾
		const float3 environment_raddiance = environment_tex.SampleLevel(smp_linear_wrap, reflect(v_eye, v_normal), specular_mip_level).rgb;
		indirect_specular = EnvironmentBRDFApprox(F0, roughness_param, saturate(dot(v_normal, -v_eye))) * environment_raddiance;
	}

	// ���Ȕ���
	float3 emissive = 0;
	{
		emissive = emissive_color * pow(2.0, emissive_intensity - 1.0);
	}

	float4 total_out_light = 0;
	total_out_light.rgb = direct_diffuse + direct_specular + indirect_diffuse + indirect_specular + emissive;
	total_out_light.a = albedo_color.a;

	return total_out_light;
}