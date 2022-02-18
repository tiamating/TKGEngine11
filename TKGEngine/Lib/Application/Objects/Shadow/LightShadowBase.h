#pragma once

#include "Systems/inc/TKGEngine_Defined.h"
#include "Application/Objects/inc/Light_Defined.h"

struct ID3D11DeviceContext;

namespace TKGEngine
{
	struct MainData;
	class ICamera;
	class Frustum;

	/// <summary>
	/// Light種類に応じたシャドウの計算処理を提供するベースクラス
	/// </summary>
	class LightShadowBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		LightShadowBase() = default;
		virtual ~LightShadowBase() = default;
		LightShadowBase(const LightShadowBase&) = delete;
		LightShadowBase& operator=(const LightShadowBase&) = delete;

		/// <summary>
		/// フラスタム内に影を落とすオブジェクトを検索
		/// </summary>
		/// <param name="scene_objects">影を描画するオブジェクトリスト</param>
		/// <param name="object_num">オブジェクトリスト内のオブジェクト数</param>
		/// <param name="camera">描画中のカメラの参照</param>
		/// <param name="data">影を出すライトの情報</param>
		/// <param name="frustum">フラスタム</param>
		/// <param name="test_accurate">テストの正確さ</param>
		/// <param name="casters">影を落とすオブジェクトリスト</param>
		virtual void FindCasters(
			const std::vector<MainData>& scene_objects,
			const int object_num,
			const std::shared_ptr<ICamera>& camera,
			const LightShadowData& data,
			const Frustum& frustum,
			bool test_accurate,
			std::vector<MainData>& casters
		) = 0;


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */

		
	private:
		// ==============================================
		// private methods
		// ==============================================
		/* nothing */


		// ==============================================
		// private variables
		// ==============================================
		/* nothing */


	};




}
