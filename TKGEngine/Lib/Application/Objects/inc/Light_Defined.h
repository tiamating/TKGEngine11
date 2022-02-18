#pragma once

#include "Utility/inc/myfunc_vector.h"

namespace TKGEngine
{
	// ==============================================
	// ライトの形状
	// ==============================================
	enum class LightType
	{
		Directional = 0,
		Spot,
		Point,
	};

	// ==============================================
	// ライトが影を作るときの情報
	// ==============================================
	struct LightShadowData
	{
		VECTOR3 light_direction = VECTOR3::Forward;	// ライト方向
		VECTOR3 light_up = VECTOR3::Up;	// ライトのアップベクトル
		float near_plane = 0.1f;	// 影を描画する近平面
		float far_plane = 100.0f;	// 影を描画する遠平面(平行光源ではフラスタムの遠平面)
		float spot_angle = 30.0f;	// スポットライトの広がり角度
	};

	
}
