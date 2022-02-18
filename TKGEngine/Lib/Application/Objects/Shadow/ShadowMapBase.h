#pragma once

#include "Systems/inc/TKGEngine_Defined.h"
#include "Application/Objects/inc/Light_Defined.h"
#include "Application/Objects/Components/interface/IRenderer.h"
#include "Application/Objects/Shadow/LightShadowBase.h"

struct ID3D11DeviceContext;

namespace TKGEngine
{
	class ICamera;

	/// <summary>
	/// ShadowMapのベースクラス
	/// </summary>
	class ShadowMapBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		ShadowMapBase() = default;
		// LightTypeに応じたLightShadowBaseを作成
		ShadowMapBase(LightType type);
		virtual ~ShadowMapBase() = default;
		ShadowMapBase(const ShadowMapBase&) = delete;
		ShadowMapBase& operator=(const ShadowMapBase&) = delete;

#ifdef USE_IMGUI
		virtual void OnGUI() = 0;
#endif // USE_IMGUI

		// ShadowMapの作成
		virtual void Create() = 0;

		// 影を描画するオブジェクトを計算、描画カウントを返す
		virtual int CalculateShadowCaster(const std::vector<MainData>& scene_objects, int object_num, const std::shared_ptr<ICamera>& camera, const LightShadowData& data) = 0;
		// Shadow描画
		virtual void RenderShadow(ID3D11DeviceContext* context, VertexBuffer& instance_buffer) = 0;
		// リソースをセット
		virtual void SetSRV(ID3D11DeviceContext* context, int slot, int index = 0) const = 0;
		// ライト行列を返す
		virtual const MATRIX& GetLVP(int index = 0) const = 0;
		virtual const MATRIX& GetLV(int index = 0) const = 0;
		virtual const MATRIX& GetLP(int index = 0) const = 0;
		// シャドウマップのサイズを返す
		virtual float GetShadowSize(int index = 0) const = 0;
		// 分割シャドウマップを使用する場合の分割数を返す
		virtual unsigned GetCascadeNum() const = 0;

		// ライトの種類を変更する
		void ChangeLightType(LightType type);

		// 現在のフレームにシャドウマップの描画を実行するか
		virtual bool IsDrawShadowMap() const = 0;
		virtual bool IsDrawShadowMap(int index) const = 0;

		// Shadowのサンプリング情報を返す
		virtual float GetSamplingRadius() const = 0;
		virtual int GetSamplingCount() const = 0;

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	protected:
		// ==============================================
		// protected methods
		// ==============================================
		/* nothing */


		// ==============================================
		// protected variables
		// ==============================================

		// ライトの形状ごとのシャドウ計算用クラス
		std::unique_ptr<LightShadowBase> m_light_shadow_calculator = nullptr;

	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				//archive();
			}
		}
		// ==============================================
		// private methods
		// ==============================================

		/// <summary>
		/// ライトの種類に応じたシャドウ計算クラスを生成
		/// </summary>
		void CreateLightShadowCalculator(LightType type);


		// ==============================================
		// private variables
		// ==============================================
		/* nothing */

	};


}

CEREAL_CLASS_VERSION(TKGEngine::ShadowMapBase, 1)
