#pragma once

#include "myfunc_vector.h"
#include "myfunc_math.h"

#include <cmath>
#include <assert.h>
#include <time.h>
#include <memory>
#include <random>
#include <limits.h>
#include <DirectXMath.h>


namespace TKGEngine
{
	// ===========================================
	// 
	// ����������Static�N���X
	// 
	// ===========================================
	class Random
	{
	private:
		static std::random_device random_device;
		//static std::unique_ptr<std::mt19937> random_engine;
		static std::mt19937 random_engine;
		static std::unique_ptr<std::uniform_real_distribution<double>> dist_real;
		static std::unique_ptr<std::uniform_real_distribution<double>> dist_real_11;
		static std::unique_ptr<std::uniform_int_distribution<int>> dist_int;
		static int seed;

	public:
		~Random();

		// ==========================================================
		// �������֐�(seed�l�����)
		// ==========================================================
		static void Initialize();

		// ==========================================================
		// seed�l�̍Đݒ�
		// ==========================================================
		static void SetSeed(int s)
		{
			seed = s;
			//random_engine.reset(new std::mt19937(seed));
			random_engine.seed(seed);
		}

		// ==========================================================
		// [min, max]�͈̔͂̒l���擾����
		// ==========================================================
		static int Range(int min, int max)
		{
			return (*dist_int)(random_engine) % (max - min + 1) + min;
		}
		static float Range(float min, float max)
		{
			return static_cast<float>(/* double�Ōv�Z����float�ɒ����Ă��� */(*dist_real)(random_engine) * (static_cast<double>(max) - static_cast<double>(min))) + min;
		}

		// ==========================================================
		// [0.0f,1.0f]�͈̔͂������_���ŕԂ�
		// ==========================================================
		static float Value()
		{
			return static_cast<float>((*dist_real)(random_engine));
		}
		// ==========================================================
		// [-1.0f,1.0f]�͈̔͂������_���ŕԂ�
		// ==========================================================
		static float Value_11()
		{
			return static_cast<float>((*dist_real_11)(random_engine));
		}

		// ==========================================================
		// ���a�P�̉~�̓����̃����_���̓_��Ԃ�
		// ==========================================================
		static VECTOR2 InsideUnitCircle()
		{
			float radius = Value_11();
			float radian = Value_11() * MyMath::PI / 2.0f;
			VECTOR2 result;
			result.x = MyMath::Cos(radian) * radius;
			result.y = MyMath::Sin(radian) * radius;
			return result;
		}

		// ==========================================================
		// ���a�P�̋��̓����̃����_���̓_��Ԃ�
		// ==========================================================
		static VECTOR3 InsideUnitSphere()
		{
			using namespace DirectX;

			float radius = Value_11();
			float radian_Y = Value_11() * MyMath::PI / 2.0f;
			float radian_X = Value_11() * MyMath::PI;
			VECTOR3 result;
			result.x = MyMath::Cos(radian_Y) * radius;
			result.y = MyMath::Sin(radian_Y) * radius;

			XMMATRIX M = XMMatrixRotationNormal(VECTOR3::Up, radian_X);
			return result.TransformMatrix(result, M);
		}

		// ==========================================================
		// ���a�P�̋��̕\�ʂ̃����_���̓_��Ԃ�
		// ==========================================================
		static VECTOR3 OnUnitSphere()
		{
			VECTOR3 result(
				Value_11(),
				Value_11(),
				Value_11());
			result.Normalized();
			if (MyMath::Approximately(result.Length(), 0.0f))
			{
				result = VECTOR3(1.0f, 0.f, 0.f);
			}
			return result;
		}

		//// ==========================================================
		//// �傫��1�̃����_���̃N�H�[�^�j�I����Ԃ�(�����x�N�g���������_���ɂƂ�)
		//// ==========================================================
		//static Quaternion Rotation()
		//{
		//	VECTOR3 v(
		//		Value_11(),
		//		Value_11(),
		//		Value_11());
		//	v.Normalized();
		//	if (MyMath::Approximately(v.Length(), 0.0f))
		//	{
		//		v = VECTOR3(1.0f, 0.f, 0.f);
		//	}
		//	return Quaternion::FromToRotation(VECTOR3::Forward, v);
		//}
		// ==========================================================
		// �傫��1�̃����_���̃N�H�[�^�j�I����Ԃ�(�I�C���[�p�������_���ɂƂ邽��normarized()���Ă΂�Ȃ�)
		// ==========================================================
		static Quaternion Rotation()
		{
			VECTOR3 angles(
				Value_11() * 180.0f,
				Value_11() * 180.0f,
				Value_11() * 180.0f);
			return Quaternion::EulerToQuaternion(angles);
		}
	};

}
