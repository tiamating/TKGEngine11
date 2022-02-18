#pragma once

#include <DirectXMath.h>
#include <algorithm>
#include <cmath>
#include <cfloat>

namespace TKGEngine
{
	// �Z�p�֐��Q���`����Static�\����
	struct MyMath
	{
		// �~����
		static constexpr float PI = DirectX::XM_PI;
		// float�̐�����������
		static constexpr float Infinity = INFINITY;
		// float��1��1���傫���ŏ��̐��Ƃ̍�
		static constexpr float Epsilon = FLT_EPSILON;

		// =======================
		// Static�����o�֐�
		// =======================
		// ===================================================
		// �����l�ł��邩�m���߂�
		// ===================================================
		// v	:	�m���߂�l
		// ---------------------------------------------------
		// �߂�l	:	v == infinity : true
		//====================================================
		static bool IsInfinity(float v)
		{
			return std::isinf(v);
		}

		// ===================================================
		// �p�x(��)->���W�A���ɕϊ�����
		// ===================================================
		// angle	:	���W�A���ɕϊ�����p�x(��)
		// ---------------------------------------------------
		// �߂�l float	:	���W�A��(rad)
		//====================================================
		static constexpr float AngleToRadian(float angle)
		{
			return DirectX::XMConvertToRadians(angle);
		}
		// ===================================================
		// ���W�A��->�p�x(��)�ɕϊ�����
		// ===================================================
		// radian	:	�p�x(��)�ɕϊ����郉�W�A��
		// ---------------------------------------------------
		// �߂�l float	:	�p�x(��)
		//====================================================
		static constexpr float RadianToAngle(float radian)
		{
			return DirectX::XMConvertToDegrees(radian);
		}

		// ===================================================
		// ��Βl��Ԃ�
		// ===================================================
		// v	:	��Βl�ɂ���l
		// ---------------------------------------------------
		// �߂�l	:	0�ȏ�̒l
		//====================================================
		static float Abs(float v)
		{
			return std::fabsf(v);
		}
		static int Abs(int v)
		{
			return std::abs(v);
		}

		// ===================================================
		// ������Ԃ�
		// ===================================================
		// value	:	�����𒲂ׂ�l
		// ---------------------------------------------------
		// �߂�l	:	��:1 ��:-1 0:0
		//====================================================
		static float Sign(float value)
		{
			return static_cast<float>(value > 0.0f) - static_cast<float>(value < 0.0f);
		}

		// ===================================================
		// x��y�����Ԃ�
		// ===================================================
		// x	:	��
		// y	:	�w��
		// ---------------------------------------------------
		// �߂�l	:	x^y
		//====================================================
		static int Pow(int x, int y)
		{
			return static_cast<int>(std::pow(x, y));
		}
		static float Pow(float x, float y)
		{
			return std::powf(x, y);
		}
		static double Pow(double x, double y)
		{
			return std::pow(x, y);
		}

		// ===================================================
		// x / y�̗]���Ԃ�
		// ===================================================
		// x	:	���q
		// y	:	����
		// ---------------------------------------------------
		// �߂�l	:	x/y�̗]��
		//====================================================
		static float Mod(float x, float y)
		{
			return fmodf(x, y);
		}
		static double Mod(double x, double y)
		{
			return fmod(x, y);
		}

		// ===================================================
		// �l��r���ď������l��Ԃ�
		// ===================================================
		// param1	:	��r����l�P
		// param2	:	��r����l�Q
		// ---------------------------------------------------
		// �߂�l Type	:	��r���ď��������̒l
		//====================================================
		static constexpr float Min(float param1, float param2)
		{
			return (std::min)(param1, param2);
		}
		static constexpr int Min(int param1, int param2)
		{
			return (std::min)(param1, param2);
		}
		static constexpr unsigned Min(unsigned param1, unsigned param2)
		{
			return (std::min)(param1, param2);
		}
		static constexpr DirectX::XMFLOAT2 Min(const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
		{
			return DirectX::XMFLOAT2((std::min)(v1.x, v2.x), (std::min)(v1.y, v2.y));
		}
		static constexpr DirectX::XMFLOAT3 Min(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
		{
			return DirectX::XMFLOAT3((std::min)(v1.x, v2.x), (std::min)(v1.y, v2.y), (std::min)(v1.z, v2.z));
		}
		static constexpr DirectX::XMFLOAT4 Min(const DirectX::XMFLOAT4& v1, const DirectX::XMFLOAT4& v2)
		{
			return DirectX::XMFLOAT4((std::min)(v1.x, v2.x), (std::min)(v1.y, v2.y), (std::min)(v1.z, v2.z), (std::min)(v1.w, v2.w));
		}
		// ===================================================
		// �l��r���đ傫���l��Ԃ�
		// ===================================================
		// param1	:	��r����l�P
		// param2	:	��r����l�Q
		// ---------------------------------------------------
		// �߂�l Type	:	��r���đ傫�����̒l
		//====================================================
		static constexpr float Max(float param1, float param2)
		{
			return (std::max)(param1, param2);
		}
		static constexpr int Max(int param1, int param2)
		{
			return (std::max)(param1, param2);
		}
		static constexpr unsigned Max(unsigned param1, unsigned param2)
		{
			return (std::max)(param1, param2);
		}
		static constexpr DirectX::XMFLOAT2 Max(const DirectX::XMFLOAT2& v1, const DirectX::XMFLOAT2& v2)
		{
			return DirectX::XMFLOAT2((std::max)(v1.x, v2.x), (std::max)(v1.y, v2.y));
		}
		static constexpr DirectX::XMFLOAT3 Max(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
		{
			return DirectX::XMFLOAT3((std::max)(v1.x, v2.x), (std::max)(v1.y, v2.y), (std::max)(v1.z, v2.z));
		}
		static constexpr DirectX::XMFLOAT4 Max(const DirectX::XMFLOAT4& v1, const DirectX::XMFLOAT4& v2)
		{
			return DirectX::XMFLOAT4((std::max)(v1.x, v2.x), (std::max)(v1.y, v2.y), (std::max)(v1.z, v2.z), (std::max)(v1.w, v2.w));
		}
		
		// ===================================================
		// ���Z��ώZ�ɕϊ�����
		// ===================================================
		// value	:	����
		// ---------------------------------------------------
		// �߂�l float	:	�����̋t��
		//====================================================
		static float InvertDivisionValue(const float value)
		{
			float sign_value = Sign(value);
			sign_value = sign_value * sign_value;
			// value == 0�̂Ƃ� 0 / -1�ƂȂ�
			return static_cast<float>(sign_value / (value + sign_value - 1.0f));
		}

		// ===================================================
		// ���������`�F�b�N�֐�(float, double, long double)
		// ===================================================
		// value		:	�`�F�b�N����l
		// reference	:	��r��l(�^�l)
		// max_abs_diff	:	�ő��΍�(�ő�̐�Ό덷) �l - �^�l
		// max_rel_diff	:	�ő勖�e��(�^�l�ɑ΂����Ό덷�̊���) ��Ό덷 / �^�l
		// ---------------------------------------------------
		// �߂�l bool	:	equal->true , not equal->false
		//====================================================
		static bool Approximately(float value, float reference, float max_abs_diff = FLT_EPSILON, float max_rel_diff = 0.0001f)
		{
			if (fabsf(value - reference) > max_abs_diff)
			{
				if (fabsf(value - reference) > (max_rel_diff * reference))
				{
					return false;
				}
			}
			return true;
		}
		static bool Approximately(double value, double reference, double max_abs_diff = DBL_EPSILON, double max_rel_diff = 0.0001)
		{
			if (::abs(value - reference) > max_abs_diff)
			{
				if (::abs(value - reference) / reference > max_rel_diff)
				{
					return false;
				}
			}
			return true;
		}

		// ===================================================
		// ������r�֐�(float, double, long double)
		// ===================================================
		// v1	:	��r���镂�������^
		// v2	:	��r���镂�������^
		// ---------------------------------------------------
		// �߂�l int	:	(v1 > v2)1�A(v1 < v2)-1�A(v1 == v2)0
		//====================================================
		static int CompareDecimal(float v1, float v2)
		{
			return Approximately(v1, v2) ? 0 : (v1 > v2 ? 1 : -1);
		}
		static int CompareDecimal(double v1, double v2)
		{
			return Approximately(v1, v2) ? 0 : (v1 > v2 ? 1 : -1);
		}

		// ===================================================
		// �N�����v�֐� (low > high�ɂȂ��Ă͂����Ȃ�)
		// ===================================================
		// v	:	���̒l��[low, high]�Ɏ��߂�
		// low	:	�����l
		// high	:	����l
		// ---------------------------------------------------
		// �߂�l	:	[low, high]�͈̔͂Ɏ��߂��l
		//====================================================
		static constexpr float Clamp(const float v, const float low, const float high)
		{
			assert((low <= high) && "Clamp func is Low <= High");
			return (std::max)((std::min)(v, high), low);
		}
		static constexpr int Clamp(const int v, const int low, const int high)
		{
			assert((low <= high) && "Clamp func is Low <= High");
			return (std::max)((std::min)(v, high), low);
		}

		// ====================================================
		// �l�����b�v�A���E���h�i�͈͂��J��Ԃ��j������
		// ====================================================
		// v	:	�͈͂��J��Ԃ��������l
		// lo	:	�����l
		// hi	:	����l�ilo���傫���l�łȂ���΂Ȃ�Ȃ��j
		//------------------------------------------------------
		//  �߂�l	:	v��lo����hi�܂ł͈̔͂Ń��b�v�A���E���h���������l
		//====================================================
		static int Wrap(const int v, const int lo, const int hi)
		{
			assert(hi > lo && "wrap func is low <= high");
			const int n = (v - lo) % (hi - lo); // �����̏�]��c++11����g�p�ɂȂ���
			return n >= 0 ? (n + lo) : (n + hi);
		}
		static float Wrap(const float v, const float lo, const float hi)
		{
			assert(hi > lo && "wrap func is low <= high");
			const float n = std::fmod(v - lo, hi - lo);
			return n >= 0 ? (n + lo) : (n + hi);
		}

		// ====================================================
		// �O�p�֐�
		// ====================================================
		// float rad		:	 ���W�A��
		//------------------------------------------------------
		//  �߂�l float	:	�O�p�֐��̌v�Z����
		//====================================================
		static float Sin(const float rad)
		{
			return std::sinf(rad);
		}
		static float Cos(const float rad)
		{
			return std::cosf(rad);
		}
		static float Tan(const float rad)
		{
			return std::tanf(rad);
		}

		// ====================================================
		// �t�O�p�֐�
		// ====================================================
		// f	:	 [-1.0f , +1.0f]
		//------------------------------------------------------
		//  �߂�l	:	[-PI/2 , +PI/2]�̃��W�A�� (acos��[0, +PI])
		//====================================================
		static float Asin(const float f)
		{
			return std::asinf(f);
		}
		static float Acos(const float f)
		{
			return std::acosf(f);
		}
		static float Atan(const float f)
		{
			return std::atanf(f);
		}

	};
}	// namespace TKGEngine