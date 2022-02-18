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
	// 乱数を扱うStaticクラス
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
		// 初期化関数(seed値を作る)
		// ==========================================================
		static void Initialize();

		// ==========================================================
		// seed値の再設定
		// ==========================================================
		static void SetSeed(int s)
		{
			seed = s;
			//random_engine.reset(new std::mt19937(seed));
			random_engine.seed(seed);
		}

		// ==========================================================
		// [min, max]の範囲の値を取得する
		// ==========================================================
		static int Range(int min, int max)
		{
			return (*dist_int)(random_engine) % (max - min + 1) + min;
		}
		static float Range(float min, float max)
		{
			return static_cast<float>(/* doubleで計算してfloatに直している */(*dist_real)(random_engine) * (static_cast<double>(max) - static_cast<double>(min))) + min;
		}

		// ==========================================================
		// [0.0f,1.0f]の範囲をランダムで返す
		// ==========================================================
		static float Value()
		{
			return static_cast<float>((*dist_real)(random_engine));
		}
		// ==========================================================
		// [-1.0f,1.0f]の範囲をランダムで返す
		// ==========================================================
		static float Value_11()
		{
			return static_cast<float>((*dist_real_11)(random_engine));
		}

		// ==========================================================
		// 半径１の円の内部のランダムの点を返す
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
		// 半径１の球の内部のランダムの点を返す
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
		// 半径１の球の表面のランダムの点を返す
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
		//// 大きさ1のランダムのクォータニオンを返す(方向ベクトルをランダムにとる)
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
		// 大きさ1のランダムのクォータニオンを返す(オイラー角をランダムにとるためnormarized()が呼ばれない)
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
