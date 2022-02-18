#pragma once

#include "myfunc_math.h"

#include <DirectXMath.h>
#include <algorithm>
#include <cmath>
#include <cfloat>

namespace TKGEngine::MyFunc
{
	// Backの超過割合指定用列挙体
	// ratio : 極大を取るときの時間割合の値
	enum class OverShoot
	{
		Over10 = 0,	// 10%超過, ratio : 0.4198938564947864
		Over20 = 1,	// 20%超過, ratio : 0.4810891345722781
		Over30 = 2,	// 30%超過, ratio : 0.5149464051556369
		Over40 = 3,	// 40%超過, ratio : 0.5373610645505098
		Over50 = 4,	// 50%超過, ratio : 0.5535737822176664
		Over60 = 5,	// 60%超過, ratio : 0.565955995546684
		Over70 = 6,	// 70%超過, ratio : 0.5757728624758618
		Over80 = 7,	// 80%超過, ratio : 0.5837730773933462
		Over90 = 8,	// 90%超過, ratio : 0.5904328361281195
		Over100 = 9	// 100%超過, ratio : 0.5960716379833215
	};

	// イージング基底クラス
	class IEase
	{

	protected:
		// 超過用定数
		static constexpr float OverShoots[10] =
		{
			1.701540198866824f,	// 10%
			2.5923889015163f,	// 20%
			3.3940516581445603f,// 30%
			4.155744652639194f,	// 40%
			4.894859521133737f,	// 50%
			5.619622918334311f,	// 60%
			6.334566669331999f,	// 70%
			7.042439379340938f,	// 80%
			7.745023855643343f,	// 90%
			8.443535601593252f	// 100%
		};

		// バウンス用定数
		static constexpr float B1 = 1.f / 2.75f;
		static constexpr float B2 = 2.f / 2.75f;
		static constexpr float B3 = 1.5f / 2.75f;
		static constexpr float B4 = 2.5f / 2.75f;
		static constexpr float B5 = 2.25f / 2.75f;
		static constexpr float B6 = 2.625f / 2.75f;
		static constexpr float B_Coef = 7.5625f;
	};


	// =============================================================
	// 
	//        No Clamped EaseIn Function
	// 
	// =============================================================
	// return			:	現在の時間における値
	// =============================================================
	// < Function 1 >
	// In	time		:	変化が開始してからの経過時間[s]
	// In	duration		:	変化させる総時間[s]
	// In	begin_value	:	開始時の値 (デフォルト値 : 0.0f)
	// In	change_value	:	開始時の値から変化させたい分の値 (デフォルト値 : 1.0f)
	// =============================================================
	// <Function 2>
	// =============================================================
	// 正規化した結果を返す簡略版
	// -------------------------------------------------------------
	// In	ratio		:	経過時間の割合 [ 0,1 ]
	// =============================================================
	class EaseIn : public IEase
	{
	public:
#pragma region four variables
		// 線形
		static float linear(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 二次
		static float quad(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 三次
		static float cubic(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 四次
		static float quart(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 五次
		static float quint(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 正弦
		static float sine(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 指数
		static float expo(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 円周
		static float circ(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 弾性
		static float elastic(float time, float duration, float begin_value = 0.f, float change_value = 1.f, float elastic_period = 0.3f, float elastic_amplitude = 1.0f);
		// 超過
		static float back(float time, float duration, float begin_value = 0.f, float change_value = 1.f, OverShoot overshoot = OverShoot::Over10);
		// バウンド
		static float bounce(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
#pragma endregion


#pragma region one variable
		// 線形
		static float linear(float ratio);
		// 二次
		static float quad(float ratio);
		// 三次
		static float cubic(float ratio);
		// 四次
		static float quart(float ratio);
		// 五次
		static float quint(float ratio);
		// 正弦
		static float sine(float ratio);
		// 指数
		static float expo(float ratio);
		// 円周
		static float circ(float ratio);
		// 弾性
		static float elastic(float ratio, float elastic_period = 0.3f, float elastic_amplitude = 1.0f);
		// 超過
		static float back(float ratio, OverShoot overshoot = OverShoot::Over10);
		// バウンド
		static float bounce(float ratio);
#pragma endregion
	};

	// =============================================================
	// 
	//        No Clamped EaseOut Function
	// 
	// =============================================================
	// return			:	現在の時間における値
	// =============================================================
	// < Function 1 >
	// In	time		:	変化が開始してからの経過時間[s]
	// In	duration		:	変化させる総時間[s]
	// In	begin_value	:	開始時の値 (デフォルト値 : 0.0f)
	// In	change_value	:	開始時の値から変化させたい分の値 (デフォルト値 : 1.0f)
	// =============================================================
	// <Function 2>
	// =============================================================
	// 正規化した結果を返す簡略版
	// -------------------------------------------------------------
	// In	ratio		:	経過時間の割合 [ 0,1 ]
	// =============================================================
	class EaseOut : public IEase
	{
	public:
#pragma region four variables
		// 線形
		static float linear(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 二次
		static float quad(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 三次
		static float cubic(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 四次
		static float quart(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 五次
		static float quint(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 正弦
		static float sine(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 指数
		static float expo(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 円周
		static float circ(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 弾性
		static float elastic(float time, float duration, float begin_value = 0.f, float change_value = 1.f, float elastic_period = 0.3f, float elastic_amplitude = 1.0f);
		// 超過
		static float back(float time, float duration, float begin_value = 0.f, float change_value = 1.f, OverShoot overshoot = OverShoot::Over10);
		// バウンド
		static float bounce(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
#pragma endregion


#pragma region one variable
		// 線形
		static float linear(float ratio);
		// 二次
		static float quad(float ratio);
		// 三次
		static float cubic(float ratio);
		// 四次
		static float quart(float ratio);
		// 五次
		static float quint(float ratio);
		// 正弦
		static float sine(float ratio);
		// 指数
		static float expo(float ratio);
		// 円周
		static float circ(float ratio);
		// 弾性
		static float elastic(float ratio, float elastic_period = 0.3f, float elastic_amplitude = 1.0f);
		// 超過
		static float back(float ratio, OverShoot overshoot = OverShoot::Over10);
		// バウンド
		static float bounce(float ratio);
#pragma endregion
	};


	// =============================================================
	// 
	//        No Clamped EaseInOut Function
	// 
	// =============================================================
	// return			:	現在の時間における値
	// =============================================================
	// < Function 1 >
	// In	time		:	変化が開始してからの経過時間[s]
	// In	duration		:	変化させる総時間[s]
	// In	begin_value	:	開始時の値 (デフォルト値 : 0.0f)
	// In	change_value	:	開始時の値から変化させたい分の値 (デフォルト値 : 1.0f)
	// =============================================================
	// <Function 2>
	// =============================================================
	// 正規化した結果を返す簡略版
	// -------------------------------------------------------------
	// In	ratio		:	経過時間の割合 [ 0,1 ]
	// =============================================================
	class EaseInOut : public IEase
	{
	public:
#pragma region four variables
		// 線形
		static float linear(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 二次
		static float quad(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 三次
		static float cubic(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 四次
		static float quart(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 五次
		static float quint(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 正弦
		static float sine(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 指数
		static float expo(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 円周
		static float circ(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
		// 弾性
		static float elastic(float time, float duration, float begin_value = 0.f, float change_value = 1.f, float elastic_period = 0.3f, float elastic_amplitude = 1.0f);
		// 超過
		static float back(float time, float duration, float begin_value = 0.f, float change_value = 1.f, OverShoot overshoot = OverShoot::Over10);
		// バウンド
		static float bounce(float time, float duration, float begin_value = 0.f, float change_value = 1.f);
#pragma endregion
		

#pragma region one variable
		// 線形
		static float linear(float ratio);
		// 二次
		static float quad(float ratio);
		// 三次
		static float cubic(float ratio);
		// 四次
		static float quart(float ratio);
		// 五次
		static float quint(float ratio);
		// 正弦
		static float sine(float ratio);
		// 指数
		static float expo(float ratio);
		// 円周
		static float circ(float ratio);
		// 弾性
		static float elastic(float ratio, float elastic_period = 0.3f, float elastic_amplitude = 1.0f);
		// 超過
		static float back(float ratio, OverShoot overshoot = OverShoot::Over10);
		// バウンド
		static float bounce(float ratio);
#pragma endregion
	};


	// ==============================================================================
	// 
	//                           inline
	// 
	// ==============================================================================

	// --------------------
	// EaseIn
	// --------------------
#pragma region Define EaseIn
	// four variables
	// 線形
	inline float EaseIn::linear(float time, float duration, float begin_value, float change_value)
	{
		return time / duration * change_value + begin_value;
	}
	// 二次
	inline float EaseIn::quad(float time, float duration, float begin_value, float change_value)
	{
		return (time /= duration) * time * change_value + begin_value;
	}
	// 三次
	inline float EaseIn::cubic(float time, float duration, float begin_value, float change_value)
	{
		return (time /= duration) * time * time * change_value + begin_value;
	}
	// 四次
	inline float EaseIn::quart(float time, float duration, float begin_value, float change_value)
	{
		return (time /= duration) * time * time * time * change_value + begin_value;
	}
	// 五次
	inline float EaseIn::quint(float time, float duration, float begin_value, float change_value)
	{
		return (time /= duration) * time * time * time * time * change_value + begin_value;
	}
	// 正弦
	inline float EaseIn::sine(float time, float duration, float begin_value, float change_value)
	{
		return -change_value * cosf(time / duration * DirectX::XM_PIDIV2) + change_value + begin_value;
	}
	// 指数
	inline float EaseIn::expo(float time, float duration, float begin_value, float change_value)
	{
		return (time == 0.f) ? begin_value : change_value * powf(2.f, 10.f * (time / duration - 1.f)) + begin_value;
	}
	// 円周
	inline float EaseIn::circ(float time, float duration, float begin_value, float change_value)
	{
		return -change_value * (sqrtf(1.f - (time /= duration) * time) - 1.f) + begin_value;
	}
	// 弾性
	inline float EaseIn::elastic(float time, float duration, float begin_value, float change_value, float elastic_period, float elastic_amplitude)
	{
		if (time == 0.f) return begin_value;
		if ((time /= duration) == 1.f) return begin_value + change_value;
		const float p = duration * elastic_period;
		const float s = p / 4.f;
		const float post_fix = elastic_amplitude * change_value * powf(2.f, 10.f * (time -= 1.f));
		return -(post_fix * sinf((time * duration - s) * asinf(1.f/ elastic_amplitude) * (2.f * DirectX::XM_PI) / p)) + begin_value;
	}
	// 超過
	inline float EaseIn::back(float time, float duration, float begin_value, float change_value, OverShoot overshoot)
	{
		return change_value * (time /= duration) * time * ((OverShoots[static_cast<int>(overshoot)] + 1.f) * time - OverShoots[static_cast<int>(overshoot)]) + begin_value;
	}
	// バウンド
	inline float EaseIn::bounce(float time, float duration, float begin_value, float change_value)
	{
		return change_value - EaseOut::bounce(duration - time, duration, 0.f, change_value) + begin_value;
	}

	// one variable
	// 線形
	inline float EaseIn::linear(float ratio)
	{
		return linear(ratio, 1.0f);
	}
	// 二次
	inline float EaseIn::quad(float ratio)
	{
		return quad(ratio, 1.0f);
	}
	// 三次
	inline float EaseIn::cubic(float ratio)
	{
		return cubic(ratio, 1.0f);
	}
	// 四次
	inline float EaseIn::quart(float ratio)
	{
		return quart(ratio, 1.0f);
	}
	// 五次
	inline float EaseIn::quint(float ratio)
	{
		return quint(ratio, 1.0f);
	}
	// 正弦
	inline float EaseIn::sine(float ratio)
	{
		return sine(ratio, 1.0f);
	}
	// 指数
	inline float EaseIn::expo(float ratio)
	{
		return circ(ratio, 1.0f);
	}
	// 円周
	inline float EaseIn::circ(float ratio)
	{
		return circ(ratio, 1.0f);
	}
	// 弾性
	inline float EaseIn::elastic(float ratio, float elastic_period, float elastic_amplitude)
	{
		return elastic(ratio, 1.0f, 0.0f, 1.0f, elastic_period, elastic_amplitude);
	}
	// 超過
	inline float EaseIn::back(float ratio, OverShoot overshoot)
	{
		return back(ratio, 1.0f, 0.0f, 1.0f, overshoot);
	}
	// バウンド
	inline float EaseIn::bounce(float ratio)
	{
		return bounce(ratio, 1.0f);
	}
#pragma endregion


	// --------------------
	// EaseOut
	// --------------------
#pragma region Define EaseOut
	// four variables
	// 線形
	inline float EaseOut::linear(float time, float duration, float begin_value, float change_value)
	{
		return time / duration * change_value + begin_value;
	}
	// 二次
	inline float EaseOut::quad(float time, float duration, float begin_value, float change_value)
	{
		return -change_value * (time /= duration) * (time - 2.f) + begin_value;
	}
	// 三次
	inline float EaseOut::cubic(float time, float duration, float begin_value, float change_value)
	{
		return change_value * ((time = time / duration - 1.f) * time * time + 1.f) + begin_value;
	}
	// 四次
	inline float EaseOut::quart(float time, float duration, float begin_value, float change_value)
	{
		return -change_value * ((time = time / duration - 1.f) * time * time * time - 1.f) + begin_value;
	}
	// 五次
	inline float EaseOut::quint(float time, float duration, float begin_value, float change_value)
	{
		return change_value * ((time = time / duration - 1.f) * time * time * time * time + 1.f) + begin_value;
	}
	// 正弦
	inline float EaseOut::sine(float time, float duration, float begin_value, float change_value)
	{
		return change_value * sinf(time / duration * DirectX::XM_PIDIV2) + begin_value;
	}
	// 指数
	inline float EaseOut::expo(float time, float duration, float begin_value, float change_value)
	{
		return (MyMath::CompareDecimal(time, duration) == 0) ? 
			begin_value + change_value : change_value * (-powf(2.f, -10.f * time / duration) + 1.f) + begin_value;
	}
	// 円周
	inline float EaseOut::circ(float time, float duration, float begin_value, float change_value)
	{
		return change_value * sqrt(1.f - (time = time / duration - 1.f) * time) + begin_value;
	}
	// 弾性
	inline float EaseOut::elastic(float time, float duration, float begin_value, float change_value, float elastic_period, float elastic_amplitude)
	{
		if (time == 0.f) return begin_value;
		if ((time /= duration) == 1.f) return begin_value + change_value;
		const float p = duration * elastic_period;
		const float s = p / 4.f;
		return (elastic_amplitude * change_value * powf(2.f, 10.f * time) * sinf((time * duration - s) * asinf(1.f / elastic_amplitude) * (2.f * DirectX::XM_PI) / p)) + begin_value;
	}
	// 超過
	inline float EaseOut::back(float time, float duration, float begin_value, float change_value, OverShoot overshoot)
	{
		return change_value * ((time = time / duration - 1.f) * time * ((OverShoots[static_cast<int>(overshoot)] + 1.f)* time + OverShoots[static_cast<int>(overshoot)]) + 1.f) + begin_value;
	}
	// バウンド
	inline float EaseOut::bounce(float time, float duration, float begin_value, float change_value)
	{
		if ((time /= duration) < B1) return change_value * B_Coef * time * time + begin_value;
		if (time < B2) return change_value * (B_Coef * (time -= B3) * time + 0.75f) + begin_value;
		if (time < B4) return change_value * (B_Coef * (time -= B5) * time + 0.9375f) + begin_value;
		return change_value * (B_Coef * (time -= B6) * time + 0.984375f) + begin_value;
	}

	// one variable
	// 線形
	inline float EaseOut::linear(float ratio)
	{
		return linear(ratio, 1.0f);
	}
	// 二次
	inline float EaseOut::quad(float ratio)
	{
		return quad(ratio, 1.0f);
	}
	// 三次
	inline float EaseOut::cubic(float ratio)
	{
		return cubic(ratio, 1.0f);
	}
	// 四次
	inline float EaseOut::quart(float ratio)
	{
		return quart(ratio, 1.0f);
	}
	// 五次
	inline float EaseOut::quint(float ratio)
	{
		return quint(ratio, 1.0f);
	}
	// 正弦
	inline float EaseOut::sine(float ratio)
	{
		return sine(ratio, 1.0f);
	}
	// 指数
	inline float EaseOut::expo(float ratio)
	{
		return expo(ratio, 1.0f);
	}
	// 円周
	inline float EaseOut::circ(float ratio)
	{
		return circ(ratio, 1.0f);
	}
	// 弾性
	inline float EaseOut::elastic(float ratio, float elastic_period, float elastic_amplitude)
	{
		return elastic(ratio, 1.0f, 0.0f, 1.0f, elastic_period, elastic_amplitude);
	}
	// 超過
	inline float EaseOut::back(float ratio, OverShoot overshoot)
	{
		return back(ratio, 1.0f, 0.0f, 1.0f, overshoot);
	}
	// バウンド
	inline float EaseOut::bounce(float ratio)
	{
		return bounce(ratio, 1.0f);
	}
#pragma endregion


	// --------------------
	// EaseInOut
	// --------------------
#pragma region Define EaseInOut
	// four variables
	// 線形
	inline float EaseInOut::linear(float time, float duration, float begin_value, float change_value)
	{
		return time / duration * change_value + begin_value;
	}
	// 二次
	inline float EaseInOut::quad(float time, float duration, float begin_value, float change_value)
	{
		if ((time /= duration / 2.f) < 1.f) return change_value / 2.f * time * time + begin_value;
		return -change_value / 2.f * (((--time) * (time - 2.f)) - 1.f) + begin_value;
	}
	// 三次
	inline float EaseInOut::cubic(float time, float duration, float begin_value, float change_value)
	{
		if ((time /= duration / 2.f) < 1.f) return change_value / 2.f * time * time * time + begin_value;
		return change_value / 2.f * ((time -= 2.f) * time * time + 2.f) + begin_value;
	}
	// 四次
	inline float EaseInOut::quart(float time, float duration, float begin_value, float change_value)
	{
		if ((time /= duration / 2.f) < 1.f) return change_value / 2.f * time * time * time * time + begin_value;
		return -change_value / 2.f * ((time -= 2.f) * time * time * time - 2.f) + begin_value;
	}
	// 五次
	inline float EaseInOut::quint(float time, float duration, float begin_value, float change_value)
	{
		if ((time = duration / 2.f) < 1.f)return change_value / 2.f * time * time * time * time * time + begin_value;
		return change_value / 2.f * ((time -= 2.f) * time * time * time * time + 2.f) + begin_value;
	}
	// 正弦
	inline float EaseInOut::sine(float time, float duration, float begin_value, float change_value)
	{
		return -change_value / 2.f * (cosf(DirectX::XM_PI * time / duration) - 1.f) + begin_value;
	}
	// 指数
	inline float EaseInOut::expo(float time, float duration, float begin_value, float change_value)
	{
		if (time == 0.f) return begin_value;
		if (MyMath::CompareDecimal(time, duration) == 0.f) return begin_value + change_value;
		if ((time /= duration / 2.f) < 1.f) return change_value / 2.f * powf(2.f, 10.f * (time - 1.f)) + begin_value;
		return change_value / 2.f * (-powf(2.f, -10.f * --time) + 2.f) + begin_value;
	}
	// 円周
	inline float EaseInOut::circ(float time, float duration, float begin_value, float change_value)
	{
		if ((time /= duration / 2.f) < 1.f) return -change_value / 2.f * (sqrt(1.f - time * time) - 1.f) + begin_value;
		return change_value / 2.f * (sqrt(1.f - time * (time -= 2)) + 1.f) + begin_value;
	}
	// 弾性
	inline float EaseInOut::elastic(float time, float duration, float begin_value, float change_value, float elastic_period, float elastic_amplitude)
	{
		if (time == 0.f) return begin_value;
		if ((time /= duration/2.f) == 2.f) return begin_value + change_value;
		const float p = duration * (elastic_period * 1.5f);
		const float s = p / 4.f;
		if (time < 1.f)
		{
			const float post_fix = change_value * powf(2.f, 10.f * (time -= 1.f));
			return -0.5f * (post_fix * sinf((time * duration - s) * (2.f * DirectX::XM_PI) / p)) + begin_value;
		}
		const float post_fix = change_value * powf(2.f, -10.f * (time -= 1.f));
		return post_fix * sinf((time * duration - s) * (2.f * DirectX::XM_PI) / p) * 0.5f + change_value + begin_value;
	}
	// 超過
	inline float EaseInOut::back(float time, float duration, float begin_value, float change_value, OverShoot overshoot)
	{
		float s = OverShoots[static_cast<int>(overshoot)];
		if ((time /= duration / 2.f) < 1.f) return change_value / 2.f * (time * time * ((s *= 1.525f) + 1.f) * time - s) + begin_value;
		return change_value / 2.f * ((time - 2.f) * time * (((s *= 1.525f) + 1.f) * time + s) + 2.f) + begin_value;
	}
	// バウンド
	inline float EaseInOut::bounce(float time, float duration, float begin_value, float change_value)
	{
		if (time < duration / 2.f) return EaseIn::bounce(time * 2.f, duration, 0.f, change_value) * 0.5f + begin_value;
		return EaseOut::bounce(time * 2.f - duration, duration, 0.f, change_value) * 0.5f + change_value * 0.5f + begin_value;
	}

	// one variable
	// 線形
	inline float EaseInOut::linear(float ratio)
	{
		return linear(ratio, 1.0f);
	}
	// 二次
	inline float EaseInOut::quad(float ratio)
	{
		return quad(ratio, 1.0f);
	}
	// 三次
	inline float EaseInOut::cubic(float ratio)
	{
		return cubic(ratio, 1.0f);
	}
	// 四次
	inline float EaseInOut::quart(float ratio)
	{
		return quart(ratio, 1.0f);
	}
	// 五次
	inline float EaseInOut::quint(float ratio)
	{
		return quint(ratio, 1.0f);
	}
	// 正弦
	inline float EaseInOut::sine(float ratio)
	{
		return sine(ratio, 1.0f);
	}
	// 指数
	inline float EaseInOut::expo(float ratio)
	{
		return expo(ratio, 1.0f);
	}
	// 円周
	inline float EaseInOut::circ(float ratio)
	{
		return circ(ratio, 1.0f);
	}
	// 弾性
	inline float EaseInOut::elastic(float ratio, float elastic_period, float elastic_amplitude)
	{
		return elastic(ratio, 1.0f, 0.0f, 1.0f, elastic_period, elastic_amplitude);
	}
	// 超過
	inline float EaseInOut::back(float ratio, OverShoot overshoot)
	{
		return back(ratio, 1.0f, 0.0f, 1.0f, overshoot);
	}
	// バウンド
	inline float EaseInOut::bounce(float ratio)
	{
		return bounce(ratio, 1.0f);
	}
#pragma endregion

}	// namespace TKGEngine::MyFunc