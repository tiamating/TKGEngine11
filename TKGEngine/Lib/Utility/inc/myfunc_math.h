#pragma once

#include <DirectXMath.h>
#include <algorithm>
#include <cmath>
#include <cfloat>

namespace TKGEngine
{
	// 算術関数群を定義したStatic構造体
	struct MyMath
	{
		// 円周率
		static constexpr float PI = DirectX::XM_PI;
		// floatの正方向無限大
		static constexpr float Infinity = INFINITY;
		// floatの1と1より大きい最小の数との差
		static constexpr float Epsilon = FLT_EPSILON;

		// =======================
		// Staticメンバ関数
		// =======================
		// ===================================================
		// 無限値であるか確かめる
		// ===================================================
		// v	:	確かめる値
		// ---------------------------------------------------
		// 戻り値	:	v == infinity : true
		//====================================================
		static bool IsInfinity(float v)
		{
			return std::isinf(v);
		}

		// ===================================================
		// 角度(°)->ラジアンに変換する
		// ===================================================
		// angle	:	ラジアンに変換する角度(°)
		// ---------------------------------------------------
		// 戻り値 float	:	ラジアン(rad)
		//====================================================
		static constexpr float AngleToRadian(float angle)
		{
			return DirectX::XMConvertToRadians(angle);
		}
		// ===================================================
		// ラジアン->角度(°)に変換する
		// ===================================================
		// radian	:	角度(°)に変換するラジアン
		// ---------------------------------------------------
		// 戻り値 float	:	角度(°)
		//====================================================
		static constexpr float RadianToAngle(float radian)
		{
			return DirectX::XMConvertToDegrees(radian);
		}

		// ===================================================
		// 絶対値を返す
		// ===================================================
		// v	:	絶対値にする値
		// ---------------------------------------------------
		// 戻り値	:	0以上の値
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
		// 符号を返す
		// ===================================================
		// value	:	符号を調べる値
		// ---------------------------------------------------
		// 戻り値	:	正:1 負:-1 0:0
		//====================================================
		static float Sign(float value)
		{
			return static_cast<float>(value > 0.0f) - static_cast<float>(value < 0.0f);
		}

		// ===================================================
		// xのy乗をを返す
		// ===================================================
		// x	:	底
		// y	:	指数
		// ---------------------------------------------------
		// 戻り値	:	x^y
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
		// x / yの余りを返す
		// ===================================================
		// x	:	分子
		// y	:	分母
		// ---------------------------------------------------
		// 戻り値	:	x/yの余り
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
		// 値比較して小さい値を返す
		// ===================================================
		// param1	:	比較する値１
		// param2	:	比較する値２
		// ---------------------------------------------------
		// 戻り値 Type	:	比較して小さい方の値
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
		// 値比較して大きい値を返す
		// ===================================================
		// param1	:	比較する値１
		// param2	:	比較する値２
		// ---------------------------------------------------
		// 戻り値 Type	:	比較して大きい方の値
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
		// 除算を積算に変換する
		// ===================================================
		// value	:	除数
		// ---------------------------------------------------
		// 戻り値 float	:	除数の逆数
		//====================================================
		static float InvertDivisionValue(const float value)
		{
			float sign_value = Sign(value);
			sign_value = sign_value * sign_value;
			// value == 0のとき 0 / -1となる
			return static_cast<float>(sign_value / (value + sign_value - 1.0f));
		}

		// ===================================================
		// 小数等式チェック関数(float, double, long double)
		// ===================================================
		// value		:	チェックする値
		// reference	:	比較基準値(真値)
		// max_abs_diff	:	最大絶対差(最大の絶対誤差) 値 - 真値
		// max_rel_diff	:	最大許容差(真値に対する絶対誤差の割合) 絶対誤差 / 真値
		// ---------------------------------------------------
		// 戻り値 bool	:	equal->true , not equal->false
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
		// 小数比較関数(float, double, long double)
		// ===================================================
		// v1	:	比較する浮動小数型
		// v2	:	比較する浮動小数型
		// ---------------------------------------------------
		// 戻り値 int	:	(v1 > v2)1、(v1 < v2)-1、(v1 == v2)0
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
		// クランプ関数 (low > highになってはいけない)
		// ===================================================
		// v	:	この値を[low, high]に収める
		// low	:	下限値
		// high	:	上限値
		// ---------------------------------------------------
		// 戻り値	:	[low, high]の範囲に収めた値
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
		// 値をラップアラウンド（範囲を繰り返す）させる
		// ====================================================
		// v	:	範囲を繰り返させたい値
		// lo	:	下限値
		// hi	:	上限値（loより大きい値でなければならない）
		//------------------------------------------------------
		//  戻り値	:	vをloからhiまでの範囲でラップアラウンドさせた数値
		//====================================================
		static int Wrap(const int v, const int lo, const int hi)
		{
			assert(hi > lo && "wrap func is low <= high");
			const int n = (v - lo) % (hi - lo); // 負数の剰余はc++11から使用可になった
			return n >= 0 ? (n + lo) : (n + hi);
		}
		static float Wrap(const float v, const float lo, const float hi)
		{
			assert(hi > lo && "wrap func is low <= high");
			const float n = std::fmod(v - lo, hi - lo);
			return n >= 0 ? (n + lo) : (n + hi);
		}

		// ====================================================
		// 三角関数
		// ====================================================
		// float rad		:	 ラジアン
		//------------------------------------------------------
		//  戻り値 float	:	三角関数の計算結果
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
		// 逆三角関数
		// ====================================================
		// f	:	 [-1.0f , +1.0f]
		//------------------------------------------------------
		//  戻り値	:	[-PI/2 , +PI/2]のラジアン (acosは[0, +PI])
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