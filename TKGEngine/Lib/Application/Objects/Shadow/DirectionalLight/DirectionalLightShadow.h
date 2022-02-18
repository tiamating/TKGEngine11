#pragma once

#include "Application/Objects/Shadow/LightShadowBase.h"

namespace TKGEngine
{
	/// <summary>
	/// 平行光源の影計算処理クラス
	/// </summary>
	class DirectionalLightShadow
		: public LightShadowBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		DirectionalLightShadow() = default;
		virtual ~DirectionalLightShadow() = default;
		DirectionalLightShadow(const DirectionalLightShadow&) = delete;
		DirectionalLightShadow& operator=(const DirectionalLightShadow&) = delete;

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
			const std::vector<MainData>&scene_objects,
			const int object_num,
			const std::shared_ptr<ICamera>&camera,
			const LightShadowData& data,
			const Frustum& frustum,
			bool test_accurate,
			std::vector<MainData>& casters
		) override;
		

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================


		// ==============================================
		// private variables
		// ==============================================
		/* nothing */

	};


}
