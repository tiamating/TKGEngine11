#include "../Utility_Math.hlsli"

///////////////////////////////////////////////
// �t���l����
// 
// F0				:	�������ˎ��̔��˗�
// view_dot_half	:	�����x�N�g���ƃn�[�t�x�N�g��
// 
// return	:	�t���l����
///////////////////////////////////////////////
float3 F_Schlick(in float3 F0, in float view_dot_half)
{
	return F0 + (1.0 - F0) * pow(1.0 - saturate(view_dot_half), 5.0);
}

///////////////////////////////////////////////
// �}�C�N���t�@�Z�b�g���z�֐�
// 
// roughness		:	�e��
// norm_dot_half	:	�@���x�N�g���ƃn�[�t�x�N�g��
// 
// return	:	�}�C�N���t�@�Z�b�g���z��
///////////////////////////////////////////////
float D_GGX(in float roughness, in float norm_dot_half)
{
	const float alpha = roughness * roughness;
	const float alpha2 = alpha * alpha;
	const float denom = (norm_dot_half * norm_dot_half) * (alpha2 - 1.0) + 1.0;
	return alpha2 * InvertDivisionValue(PI * denom * denom);
}

///////////////////////////////////////////////
// �􉽌�����
// https://zenn.dev/mebiusbox/books/619c81d2fbeafd/viewer/7c1069
// https://qiita.com/emadurandal/items/76348ad118c36317ec5c
// 
// roughness		:	�e��
// norm_dot_view	:	�@���x�N�g���Ǝ��_�ւ̃x�N�g��
// norm_dot_light	:	�@���x�N�g���ƌ����ւ̃x�N�g��
// 
// return	:	�}�C�N���t�@�Z�b�g���z�֐�
///////////////////////////////////////////////
float G_SmithSchlickGGX(in float roughness, in float norm_dot_view, in float norm_dot_light)
{
	const float k = roughness * SQRT_2_DIV_PI;
	const float shadowing = norm_dot_light * InvertDivisionValue(norm_dot_light * (1.0 - k) + k);
	const float masking = norm_dot_view * InvertDivisionValue(norm_dot_view * (1.0 - k) + k);
	return shadowing * masking;

}