#ifndef _UTILITY_MATH_HLSLI_
#define _UTILITY_MATH_HLSLI_

///////////////////////////////////////////////
// �ϐ���`
///////////////////////////////////////////////
// Float�C�v�V����
static const float EPSILON = 1.0e-4;
// ���ώ���Cos86�x�ȏ�𔻒肷�邽�߂Ɏg�p
static const float IGNORE_COS_DEGREE = 0.06975647374;
// �~����
static const float PI = 3.14159265f;
// sqrt(2/PI)
static const float SQRT_2_DIV_PI = 0.7978845608;


///////////////////////////////////////////////
// ���Z��ώZ�ɕϊ�����
// https://stackoverflow.com/questions/5802351/what-happens-when-you-divide-by-0-in-a-shader
// 
// value	:	����
// 
// return	:	�����̋t��
///////////////////////////////////////////////
float InvertDivisionValue(in float value)
{
	float sign_value = sign(value);
	sign_value = sign_value * sign_value;
	// value == 0�̂Ƃ� 0 / -1�ƂȂ�
	return sign_value / (value + sign_value - 1.0);
}

///////////////////////////////////////////////
// �t���l���̋ߎ���
// https://ja.wikipedia.org/wiki/%E3%83%95%E3%83%AC%E3%83%8D%E3%83%AB%E3%81%AE%E5%BC%8F
// 
// eye		:	�����x�N�g��(���K���ς�)
// normal	:	�@���x�N�g��(���K���ς�)
// n1		:	���ˑ��̋��ܗ�
// n2		:	���ܑ��̋��ܗ�
// 
// return	:	���ˌW��
///////////////////////////////////////////////
float CalcFresnel(in float3 eye, in float3 normal, in float n1, in float n2)
{
	const float n1_sub_n2 = n1 - n2;
	const float n1_add_n2 = n1 + n2;
	float f0 = (n1_sub_n2 * n1_sub_n2) / (n1_add_n2 * n1_add_n2);
	return f0 + (1.0 - f0) * pow(1.0 - dot(-eye, normal), 5.0);
}

///////////////////////////////////////////////
// �������̌v�Z
// �����Ȃ��̎� d = 0�̎�1, d = r�̎�0��Ԃ�
// https://qiita.com/uechoco@github/items/95b564f2f5eea8af5d33
// https://stackoverflow.com/questions/4157401/hlsl-point-light-problems
// 
// distance		:	����
// range		:	�e���͈�
// attenuation	:	x:�萔 y:�ꎟ z:��
// 
// return	:	������(0 ~ 1)
///////////////////////////////////////////////
float CalcAttenuation(in float distance, in float range, in float3 attenuation)
{
	float ratio = distance * InvertDivisionValue(range);
	float att = InvertDivisionValue(attenuation.x + (attenuation.y * ratio) + (attenuation.z * ratio * ratio));
	// �������̃N�����v
	return saturate(att);
}

///////////////////////////////////////////////
// �n�[�t�x�N�g��
// http://www.slis.tsukuba.ac.jp/~fujisawa.makoto.fu/cgi-bin/wiki/index.php?GLSL%A4%CB%A4%E8%A4%EB%A5%D5%A5%A9%A5%F3%A5%B7%A5%A7%A1%BC%A5%C7%A5%A3%A5%F3%A5%B0
// 
// view		:	���_�ւ̕����x�N�g��(���K���ς�)
// light	:	���C�g�x�N�g��(���K���ς�)
// 
// return	:	�n�[�t�x�N�g��
///////////////////////////////////////////////
float3 HalfVector(in float3 view, in float3 light)
{
	return normalize(view + light);
}

///////////////////////////////////////////////
// �]�@��
// 
// normal	:	�@��(���K���ς�)
// tangent	:	�ڐ�(���K���ς�)
// 
// return	:	�]�@��(���K���ς�)
///////////////////////////////////////////////
float3 Binormal(in float3 normal, in float3 tangent)
{
	return normalize(cross(normal, tangent));
}

///////////////////////////////////////////////
// �t�s�񐶐�
// 
// m	:	���̍s��
// 
// return	:	�t�s��
///////////////////////////////////////////////
float4x4 Invert(in float4x4 m)
{
	float n11 = m[0][0], n12 = m[1][0], n13 = m[2][0], n14 = m[3][0];
	float n21 = m[0][1], n22 = m[1][1], n23 = m[2][1], n24 = m[3][1];
	float n31 = m[0][2], n32 = m[1][2], n33 = m[2][2], n34 = m[3][2];
	float n41 = m[0][3], n42 = m[1][3], n43 = m[2][3], n44 = m[3][3];

	float t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
	float t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
	float t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
	float t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

	float det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;
	float idet = 1.0f / det;

	float4x4 ret;

	ret[0][0] = t11 * idet;
	ret[0][1] = (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44) * idet;
	ret[0][2] = (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44) * idet;
	ret[0][3] = (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43) * idet;

	ret[1][0] = t12 * idet;
	ret[1][1] = (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44) * idet;
	ret[1][2] = (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44) * idet;
	ret[1][3] = (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43) * idet;

	ret[2][0] = t13 * idet;
	ret[2][1] = (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44) * idet;
	ret[2][2] = (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44) * idet;
	ret[2][3] = (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43) * idet;

	ret[3][0] = t14 * idet;
	ret[3][1] = (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34) * idet;
	ret[3][2] = (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34) * idet;
	ret[3][3] = (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33) * idet;

	return ret;
}



#endif// _UTILITY_MATH_HLSLI_