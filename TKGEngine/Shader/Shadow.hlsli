#ifndef _SHADOW_HLSLI_
#define _SHADOW_HLSLI_

#include "ConstantBuffer_Defined.hlsli"
#include "Utility_Math.hlsli"
#include "Register_Defined.hlsli"


///////////////////////////////////////////////
// PCF�t�B���^
///////////////////////////////////////////////
static const int MAX_FILTER = 9;
static const float DIST_SQRT_2_WEIGHT = 1.17157287525;
static const float3 PCF_FILTER[MAX_FILTER] =
{
	float3(+0.0, +0.0, 4.0),
	float3(+1.0, +0.0, 2.0),
	float3(-1.0, +0.0, 2.0),
	float3(+0.0, +1.0, 2.0),
	float3(+0.0, -1.0, 2.0),
	float3(+1.0, +1.0, DIST_SQRT_2_WEIGHT),
	float3(-1.0, -1.0, DIST_SQRT_2_WEIGHT),
	float3(+1.0, -1.0, DIST_SQRT_2_WEIGHT),
	float3(-1.0, +1.0, DIST_SQRT_2_WEIGHT)
};

///////////////////////////////////////////////
// ���s���e�V���h�E�}�b�v�̓K���I�[�x�o�C�A�X���v�Z����
//
// shadow_tex			:	�V���h�E�}�b�v
// shadow_uv			:	�V���h�E�}�b�v���(xy UV���W, z �[�x���)
// ls_position			:	���C�g��Ԃł̍��W
// ls_normal			:	���C�g��Ԃł̖@��(���K���ς�)
// constant_bias		:	�[�x�o�C�A�X
// shadow_tex_res		:	�V���h�E�}�b�v�̃T�C�Y
// inv_shadow_tex_res	:	�V���h�E�}�b�v�̃T�C�Y�̋t��
// light_projection		:	���C�g�v���W�F�N�V�����s��
// offset				:	�I�t�Z�b�g�e�N�Z��
// 
// return			:	�e��臒l
///////////////////////////////////////////////
float CalculateAdaptiveDepthBiasForOrthographic
(
	in Texture2D<float> shadow_tex,
	in float3 shadow_uv,
	in float3 ls_position,
	in float3 ls_normal,
	in float constant_bias,
	in float shadow_tex_res,
	in float inv_shadow_tex_res,
	in matrix light_projection,
	in uint2 offset = uint2(0, 0)
)
{
	// ���K�����ꂽ�V���h�E�}�b�v�̐[�x
	const float shadow_depth = shadow_tex.Sample(smp_linear_wrap, shadow_uv.xy).r;

	//// �o�C�A�X�ɉ��Z����œK�ȃC�v�V�������v�Z����
	// �[�x�l�𐳋K������[�x���k�֐�����float�덷����菜���C�v�V�������v�Z����
	const float A = light_projection._33;
	const float B = light_projection._34;
	const float adaptive_epsilon = 0.5 * pow(1.0 - A - 2.0 * shadow_depth, 2.0) * constant_bias / B;

	//// ���C�g��Ԃ̃V���h�E�}�b�v�̃e�N�Z�����S�̍��W���v�Z����
	// �V���h�E�}�b�v�̃T���v���e�N�Z���ɕϊ�
	const float2 shadow_texel = floor(shadow_uv.xy * shadow_tex_res) + offset;
	// �e�N�Z���̃O���b�h���S�Ɉړ����Đ��K������(0.0, 1.0)
	const float2 norm_ls_grid_center = saturate((shadow_texel + float2(0.5, 0.5)) * inv_shadow_tex_res);
	// ���C�g��ԍ��W�ɕϊ�(-width, +width) (-height, +height)
	const float2 view_bound = float2(InvertDivisionValue(light_projection._11), InvertDivisionValue(light_projection._22));
	float2 ls_grid_center = view_bound * (norm_ls_grid_center * 2.0 - float2(1.0, 1.0));
	ls_grid_center.y = -ls_grid_center.y;

	//// �V���h�E�e�N�Z���̒��S�����Ray�ƕ��ʂ̌����_���v�Z
	// ��������̃��C(���s�����̂���Forward)
	const float3 ls_grid_line_dir = float3(0.0, 0.0, -1.0);
	// �V�F�[�f�B���O�s�N�Z���̃I�N���[�_�[���ʒu
	const float3 center_pos = float3(ls_grid_center, 0.0);
	const float denom_dist = dot(ls_normal, ls_grid_line_dir);
	const float inv_denom_dist = InvertDivisionValue(denom_dist);
	const float ls_hit_dist = dot(ls_position - center_pos, ls_normal) * inv_denom_dist;
	const float3 ls_hit_pos = ls_hit_dist * ls_grid_line_dir + center_pos;

	//// �����_��V�����V�F�[�f�B���O�s�N�Z���̐��ݓI�I�N���[�_�[�Ƃ��Đ[�x�l���v�Z����
	float4 ls_potential_occluder = mul(light_projection, float4(ls_hit_pos, 1.0));
	ls_potential_occluder = ls_potential_occluder / ls_potential_occluder.w;
	ls_potential_occluder.xy = (ls_potential_occluder.xy + float2(1.0, 1.0)) * 0.5;
	ls_potential_occluder.y = -ls_potential_occluder.y;
	// �@���ƃ��C�g��������������Ƃ������_��Z��0�ɂȂ邽�߁A�e�Ƃ���
	ls_potential_occluder.z = step(IGNORE_COS_DEGREE, denom_dist) * ls_potential_occluder.z;

	//// �[�x�l�̍����o�C�A�X�Ƃ��Ďg�p����
	const float actual_depth_bias = max(0.0, ls_potential_occluder.z - shadow_uv.z);
	return actual_depth_bias + adaptive_epsilon;
}

///////////////////////////////////////////////
// ���[���h���W�����C�g��ԍ��W�ɕϊ�
//
// vp			:	���C�g�r���[�v���W�F�N�V�����s��
// w_position	:	�Ǝˈʒu���[���h���W
// 
// return		:	�V���h�E�}�b�v���(xy UV���W, z �[�x���)
///////////////////////////////////////////////
float3 GetShadowUV(in float4x4 vp, in float3 w_position)
{
	// NDC
	float4 wvp_pos;
	wvp_pos.xyz = w_position;
	wvp_pos.w = 1.0;
	wvp_pos = mul(vp, wvp_pos);
	wvp_pos /= wvp_pos.w;
	// UV
	wvp_pos.y = -wvp_pos.y;
	wvp_pos.xy = 0.5 * wvp_pos.xy + 0.5;
	return wvp_pos.xyz;
}

///////////////////////////////////////////////
// �e��臒l��Ԃ�
//
// shadow_tex		:	�V���h�E�}�b�v
// tex_size			:	�V���h�E�}�b�v�̃T�C�Y
// inv_tex_size		:	�V���h�E�}�b�v�̃T�C�Y�̋t��
// shadow_uv		:	�V���h�E�}�b�v���(xy UV���W, z �[�x���)
// constant_bias	:	�[�x�o�C�A�X
// ls_position		:	���C�g��Ԃł̍��W
// ls_normal		:	���C�g��Ԃł̖@��(���K���ς�)
// light_projection	:	���C�g�v���W�F�N�V�����s��
// sample_count		:	�T���v�����O��
// filter_radius_uv	:	�t�B���^��UV���a
// 
// return			:	�e��臒l
///////////////////////////////////////////////
float GetShadow(
in Texture2D<float> shadow_tex,
in float tex_size,
in float inv_tex_size,
in float3 shadow_uv,
in float constant_bias,
in float3 ls_position,
in float3 ls_normal,
in matrix light_projection,
in uint sample_count,
in float filter_radius_uv
)
{
	// PCF�t�B���^�ɑΉ������K���I�[�x�o�C�A�X
	const float pixel_bias =
		CalculateAdaptiveDepthBiasForOrthographic(shadow_tex, shadow_uv, ls_position, ls_normal, constant_bias, tex_size, inv_tex_size, light_projection);
	const float delta_x =
		CalculateAdaptiveDepthBiasForOrthographic(shadow_tex, shadow_uv, ls_position, ls_normal, constant_bias, tex_size, inv_tex_size, light_projection, uint2(1, 0)) - pixel_bias;
	const float delta_y =
		CalculateAdaptiveDepthBiasForOrthographic(shadow_tex, shadow_uv, ls_position, ls_normal, constant_bias, tex_size, inv_tex_size, light_projection, uint2(0, 1)) - pixel_bias;

	float sum_threshold = 0.0;
	float sum_weight = 0.0;
	// PCF�T���v�����O
	[unroll]
	for (uint i = 0; i < sample_count; ++i)
	{
		const float3 current_filter = PCF_FILTER[i];
		const float2 offset = current_filter.xy * filter_radius_uv * inv_tex_size;
		const float weight = current_filter.z;
		const float bias = pixel_bias + delta_x * current_filter.x + delta_y * current_filter.y;
		//sum_threshold += (shadow_tex.Sample(smp_linear_wrap, shadow_uv.xy + offset).r > shadow_uv.z + bias ? 1.0 : 0.0) * weight;
		sum_threshold += shadow_tex.SampleCmp(smp_comp_shadow, shadow_uv.xy + offset, shadow_uv.z + bias).r * weight;
		sum_weight += weight;
	}
	return sum_threshold * InvertDivisionValue(sum_weight);
}

///////////////////////////////////////////////
// ���s���C�g�̉e�̊������v�Z
// 
// dir_light	:	���s���C�g
// w_position	:	�Ǝˈʒu���[���h���W
// w_normal		:	�Ǝˈʒu���[���h�@��(���K���ς�)
// light_dir	:	���C�g�����x�N�g��(���K���ς�)
// 
// return	:	�e�̊���
///////////////////////////////////////////////
float CalculateDirectionalLightShadow(in DirectionalLight dir_light, in float3 w_position, in float3 w_normal, in float3 light_dir)
{
	float shadow_threshold = 0.0f;
	// �V���h�E��`�悷�邩
	if (dir_light.use_shadow_map)
	{
		bool cascade_found = false;
		uint cascade_index = 0;
		float3 shadow_uv;
	//�ǂ̃J�X�P�[�h�Ɋ܂܂�Ă��邩
		for (uint i = 0; i < dir_light.cascade_num && !cascade_found; ++i)
		{
			if (dir_light.has_caster[i])
			{
			// �@���o�C�A�X��K�p���ăV���h�E�e�N�X�`����Ԃɕϊ�����
				const float slope_scale = saturate(1.0 - dot(light_dir, w_normal));
				shadow_uv = GetShadowUV(dir_light.LVPs[i], w_position + w_normal * slope_scale * dir_light.normal_bias);
				if (min(shadow_uv.x, shadow_uv.y) > 0.0
					&& max(shadow_uv.x, shadow_uv.y) < 1.0
					&& shadow_uv.z <= 1.0
					&& shadow_uv.z >= 0.0)
				{
					cascade_found = true;
					cascade_index = i;
				}
			}
		}
		if (cascade_found)
		{
		// �[�x�o�C�A�X��אڃs�N�Z������v�Z
		// http://project-asura.com/blog/archives/4271

		// ���C�g��Ԃ̃s�N�Z�����W
			const float3 ls_position = mul(dir_light.light_views[cascade_index], float4(w_position, 1.0)).xyz;
		// ���C�g��Ԃ̃s�N�Z���@��
			const float3 ls_normal = normalize(mul((float3x3) dir_light.light_views[cascade_index], w_normal));
		
		// �e��臒l���擾
			switch (cascade_index)
			{
				case 0:
					{
						shadow_threshold = GetShadow(
							shadow_cascade_0, dir_light.shadow_size.x, dir_light.inv_shadow_size.x,
							shadow_uv, dir_light.bias, ls_position, ls_normal, dir_light.light_projs[cascade_index],
							dir_light.sampling_count, dir_light.filter_radius_uv
						);
					}
					break;
				case 1:
					{
						shadow_threshold = GetShadow(
							shadow_cascade_1, dir_light.shadow_size.y, dir_light.inv_shadow_size.y,
							shadow_uv, dir_light.bias, ls_position, ls_normal, dir_light.light_projs[cascade_index],
							dir_light.sampling_count, dir_light.filter_radius_uv
						);
					}
					break;
				case 2:
					{
						shadow_threshold = GetShadow(
							shadow_cascade_2, dir_light.shadow_size.z, dir_light.inv_shadow_size.z,
							shadow_uv, dir_light.bias, ls_position, ls_normal, dir_light.light_projs[cascade_index],
							dir_light.sampling_count, dir_light.filter_radius_uv
						);
					}
					break;
				case 3:
					{
						shadow_threshold = GetShadow(
							shadow_cascade_3, dir_light.shadow_size.w, dir_light.inv_shadow_size.w,
							shadow_uv, dir_light.bias, ls_position, ls_normal, dir_light.light_projs[cascade_index],
							dir_light.sampling_count, dir_light.filter_radius_uv
						);
					}
					break;
			}
		}
	}
	return shadow_threshold;
}

#endif// _SHADOW_HLSLI_