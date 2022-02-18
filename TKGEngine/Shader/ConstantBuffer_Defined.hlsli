#ifndef _CONSTANTBUFFER_DEFINED_HLSLI_
#define _CONSTANTBUFFER_DEFINED_HLSLI_

#include "../Lib/Application/Resource/inc/Shader_Defined.h"
#include "Register_Defined.hlsli"

// CB_ENVIRONMENT (0)
struct Fog
{
	float4 color;
	float density;
	float start_distance;
	float end_distance;
	uint fog_type;
	bool use_fog;
};
CBUFFER(CB_ENVIRONMENT, CBS_ENVIRONMENT)
{
	float4 ambient_colors[2];

	Fog fog;
}

// CB_TIME (1)
CBUFFER(CB_TIME, CBS_TIME)
{
	float4 time;		// ( t/20, t, 2t, 3t )
	float4 sin_time;	// ( t/8, t/4, t/2, t ) 1����
	float4 cos_time;	// ( t/8, t/4, t/2, t ) 1����
	float4 delta_time; // ( dt, 1/dt, unscaled_dt, 1/unscaled_dt )
}

// CB_CAMERA (2)
struct Camera
{
	matrix V;
	matrix P;
	matrix IV;
	matrix IP;
	matrix IVP;
	matrix prev_VP;
	float4 param; // ( near, far, (far - near), 1 / (far - near) )
	float3 pos;
	float dummy;
	float3 dir;
	float dummy2;
	float target_width;
	float target_height;
	float dummy3;
	float dummy4;
};
CBUFFER(CB_CAMERA, CBS_CAMERA)
{
	Camera camera;
}

// CB_VP (3)
CBUFFER(CB_VP, CBS_VP)
{
	matrix VP;
}

// CB_LIGHT (4)
struct DirectionalLight
{
	matrix LVPs[MAX_CASCADE];// ���C�g�r���[�v���W�F�N�V����
	matrix light_views[MAX_CASCADE];// ���C�g�r���[
	matrix light_projs[MAX_CASCADE]; // ���C�g�v���W�F�N�V����
	float4 color;			// ���C�g�F
	float3 direction;		// ���C�g����
	float strength;			// �e�F�̋��x
	float4 shadow_size;		// �V���h�E�}�b�v�̃T�C�Y
	float4 inv_shadow_size; // �V���h�E�}�b�v�̃T�C�Y�̋t��
	float bias;				// �V���h�E�o�C�A�X
	float normal_bias;		// �@�������̃V���h�E�o�C�A�X
	bool use_shadow_map;	// �V���h�E�}�b�v���g�p���邩
	uint cascade_num;		// ������
	bool4 has_caster;		// �L���X�^�[�������Ă��邩
	float filter_radius_uv;	// �T���v�����O�t�B���^��UV���a
	uint sampling_count;	// �T���v�����O��(�ő�FMAX_POISSON)
	float dummy1;
	float dummy2;
};
struct SpotLight
{
	matrix LVP;				// ���C�g�r���[�v���W�F�N�V����
	matrix light_view;		// ���C�g�r���[
	matrix light_proj;		// ���C�g�v���W�F�N�V����
	float4 pos;				// ���C�g�ʒu
	float4 color;			// ���C�g�F
	float3 direction;		// ���C�g����
	float range;			// �͈�(0�ȉ��Ŗ����ȃ��C�g)
	float3 attenuation;		// �����W��(x:�萔 y:�ꎟ z:��)
	float falloff;			// �����W��(inner --> outer�ւ̌�������`)
	float inner_cone_cos;	// cos(��/2) (��:�����~���̍L����p)
	float outer_cone_cos;	// cos(��/2) (��:�O���~���̍L����p)
	bool use_light_mask;	// ���C�g�}�X�N�e�N�X�`�����g�p���邩
	bool use_shadow_map;	// �V���h�E�}�b�v���g�p���邩
	float strength;			// �e�F�̋��x
	float bias;				// �V���h�E�o�C�A�X
	float normal_bias;		// �@�������̃V���h�E�o�C�A�X
	float shadow_size;	// �V���h�E�}�b�v�̃T�C�Y
	float inv_shadow_size;	// �V���h�E�}�b�v�̃T�C�Y�̋t��
	float dummy1;
	float dummy2;
	float dummy3;
};
struct PointLight
{
	matrix LVPs[2];		// ���C�g�r���[�v���W�F�N�V����
	matrix light_views[2];	// ���C�g�r���[
	matrix light_projs[2]; // ���C�g�v���W�F�N�V����
	float4 pos;			// ���C�g�ʒu
	float4 color;		// ���C�g�F
	float3 attenuation; // �����W��(x:�萔 y:�ꎟ z:��)
	float range;		// �͈�(0�ȉ��Ŗ����ȃ��C�g)
	bool use_shadow_map;// �V���h�E�}�b�v���g�p���邩
	float strength;		// �e�F�̋��x
	float bias;			// �V���h�E�o�C�A�X
	float normal_bias; // �@�������̃V���h�E�o�C�A�X
	float2 shadow_size; // �V���h�E�}�b�v�̃T�C�Y
	float2 inv_shadow_size; // �V���h�E�}�b�v�̃T�C�Y�̋t��
};
CBUFFER(CB_LIGHT, CBS_LIGHT)
{
	DirectionalLight directional_light;
	SpotLight spot_lights[MAX_SPOTLIGHT];
	PointLight point_lights[MAX_POINTLIGHT];
}

// CB_MODEL (5)
CBUFFER(CB_MODEL, CBS_MODEL)
{
	matrix bone_transforms[MAX_BONES];
}

// CB_TEXTURE (7)
struct Texture
{
	float2 offset;
	float2 tiling;
	float cutout_threshold;
	float dummy0;
	float dummy1;
	float dummy2;
};
CBUFFER(CB_TEXTURE, CBS_TEXTURE)
{
	Texture tex_param;
}


#endif// _CONSTANTBUFFER_DEFINED_HLSLI_