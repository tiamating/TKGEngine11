#pragma once

#include "Components/inc/CBehaviour.h"
#include "Shadow/ShadowMapBase.h"
#include "Application/Resource/inc/Texture.h"
#include "Application/Resource/inc/Asset_Defined.h"
#include "Application/Objects/inc/Light_Defined.h"

#include "Utility/inc/myfunc_vector.h"

namespace TKGEngine
{
	class ShadowMapBase;
	class LightManager;
	struct MainData;
	class ICamera;
	class VertexBuffer;

	/// <summary>
	/// Management Light parameter.
	/// </summary>
	class Light
		: public Behaviour
	{
		friend LightManager;

	public:
		// ==============================================
		// public methods
		// ==============================================
		Light(const Light&) = delete;
		Light& operator=(const Light&) = delete;

		Light();
		virtual ~Light();
		
		// Param
		// Type
		void SetType(const LightType type);
		LightType GetType() const;
		// Color (color = color * intensity)
		void SetColor(const VECTOR4& color);
		VECTOR4 GetColor() const;
		// Intensity
		void SetIntensity(float intensity);
		// Range
		void SetRange(float range);
		float GetRange() const;
		// Spot angle
		void SetSpotAngleInner(float angle/* 度 */);
		float GetSpotAngleInner() const;
		void SetSpotAngleOuter(float angle/* 度 */);
		float GetSpotAngleOuter() const;
		// Fall off
		void SetFalloff(float falloff);
		float GetFalloff() const;
		// Attenuation
		void SetAttenuation(float constant, float linear, float quadratic);
		VECTOR3 GetAttenuation() const;
		// Culling layer
		void SetCullingLayer(int mask);
		int GetCullingLayer() const;

		// ~Param

		// Shadow
		// 作成するかのフラグ
		void SetCreateShadow(bool do_create);
		bool GetCreateShadow() const;
		// Render flag
		void SetRenderShadow(bool do_render);
		bool GetRenderShadow() const;
		// Strength
		void SetStrength(float strength);
		float GetStrength() const;
		// Bias
		void SetBias(float bias);
		float GetBias() const;
		// Normal Bias
		void SetNormalBias(float bias);
		float GetNormalBias() const;
		// Near plane
		void SetNearPlane(float near_plane);
		// SpotLight用、マスクテクスチャを使用するか
		void UseLightMask(bool use);
		bool UseLightMask() const;
		// SpotLight用、マスクテクスチャをバインドする
		void SetLightMaskTexture(ID3D11DeviceContext* context, int slot) const;
		// ライトの行列を返す
		const MATRIX& GetLVP(int index = 0) const;
		const MATRIX& GetLV(int index = 0) const;
		const MATRIX& GetLP(int index = 0) const;
		// シャドウマップのサイズを返す
		float GetShadowSize(int index = 0) const;
		// 分割シャドウマップを使用する場合の分割数を返す
		unsigned GetCascadeNum() const;
		// シャドウマップをシェーダーステージにバインドする
		void SetSRV(ID3D11DeviceContext* context, int slot, int index = 0) const;
		// ~Shadow


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		friend class cereal::access;
		template <class Archive>
		void load(Archive& archive, const std::uint32_t version)
		{
			if (version == 1)
			{
				// メンバ読み込み
				archive(
					cereal::base_class<Behaviour>(this),
					CEREAL_NVP(m_light_type),
					CEREAL_NVP(m_light_color),
					CEREAL_NVP(m_intensity),
					CEREAL_NVP(m_range),
					CEREAL_NVP(m_spot_angle_outer),
					CEREAL_NVP(m_spot_angle_inner),
					CEREAL_NVP(m_falloff),
					CEREAL_NVP(m_constant_attenuation),
					CEREAL_NVP(m_linear_attenuation),
					CEREAL_NVP(m_quadratic_attenuation),
					CEREAL_NVP(m_culling_layer),
					CEREAL_NVP(m_use_mask_texture),
					CEREAL_NVP(m_mask_texture_data),
					CEREAL_NVP(m_do_create_shadow),
					CEREAL_NVP(m_is_render_shadow),
					CEREAL_NVP(m_strength),
					CEREAL_NVP(m_bias),
					CEREAL_NVP(m_normal_bias),
					CEREAL_NVP(m_shadow_near_plane),
					CEREAL_NVP(m_max_shadow_far_plane)
				);
				// ShadowMapが書き出されていたらロード
				if(m_do_create_shadow)
				{
					archive(m_shadow_map);
					m_shadow_map->Create();
					m_shadow_map->ChangeLightType(m_light_type);
				}
			}
		}
		template <class Archive>
		void save(Archive& archive, const std::uint32_t version) const
		{
			if (version == 1)
			{
				// メンバ読み込み
				archive(
					cereal::base_class<Behaviour>(this),
					CEREAL_NVP(m_light_type),
					CEREAL_NVP(m_light_color),
					CEREAL_NVP(m_intensity),
					CEREAL_NVP(m_range),
					CEREAL_NVP(m_spot_angle_outer),
					CEREAL_NVP(m_spot_angle_inner),
					CEREAL_NVP(m_falloff),
					CEREAL_NVP(m_constant_attenuation),
					CEREAL_NVP(m_linear_attenuation),
					CEREAL_NVP(m_quadratic_attenuation),
					CEREAL_NVP(m_culling_layer),
					CEREAL_NVP(m_use_mask_texture),
					CEREAL_NVP(m_mask_texture_data),
					CEREAL_NVP(m_do_create_shadow),
					CEREAL_NVP(m_is_render_shadow),
					CEREAL_NVP(m_strength),
					CEREAL_NVP(m_bias),
					CEREAL_NVP(m_normal_bias),
					CEREAL_NVP(m_shadow_near_plane),
					CEREAL_NVP(m_max_shadow_far_plane)
				);
				// ShadowMapを保持していたら書き出す
				if(m_do_create_shadow)
				{
					archive(m_shadow_map);
				}
			}
		}

		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		void OnGUI() override;
		// カリングレイヤー選択GUI用関数
		void OnGUICullingLayer();
#endif // USE_IMGUI

		void OnCreate(const std::shared_ptr<Object>& p_self) override;
		void OnDestroy() override;

		// Behaviour
		void OnEnabled() override {};
		void OnDisabled() override {};
		// ~Behaviour

		// シャドウマップオブジェクトを作成
		void CreateShadowMapObject();
		// シャドウマップオブジェクトを破棄
		void RemoveShadowMapObject();

		/// <summary>
		/// LightManagerが呼ぶ、影を落とすオブジェクトの計算を行う
		/// </summary>
		/// <param name="scene_objects">影を描画するオブジェクトリスト</param>
		/// <param name="object_num">オブジェクトリスト内のオブジェクト数</param>
		/// <param name="camera">描画中のカメラの参照</param>
		/// <returns>描画数</returns>
		int CalculateShadowCaster(const std::vector<MainData>& scene_objects, int object_num, const std::shared_ptr<ICamera>& camera) const;

		// 影を描画するオブジェクトがなければfalse
		bool IsRenderShadowCaster() const;
		bool IsRenderShadowCaster(int index) const;

		// Shadowのサンプリング情報を返す
		float GetSamplingRadius() const;
		int GetSamplingCount() const;

		// LightManagerが呼ぶ、影描画を実行する
		void RenderShadowCaster(ID3D11DeviceContext* context, VertexBuffer& instance_buffer) const;


		// ==============================================
		// private variables
		// ==============================================
#ifdef USE_IMGUI
		bool m_is_opened_culling_layer_window = false;
#endif // USE_IMGUI

		// ライト形状
		LightType m_light_type = LightType::Spot;
		// カラー
		VECTOR4 m_light_color = VECTOR4::One;
		// 強度(0以上)
		float m_intensity = 1.0f;
		// 範囲(0以上)
		float m_range = 5.0f;
		// スポットライトの外側角(内側角 <= x <= 179)
		float m_spot_angle_outer = 30.0f;
		// スポットライトの内側角(1 <= x <= 外側角)
		float m_spot_angle_inner = 20.0f;

		// 減衰係数
		// (inner -> outerへの減衰する形)
		float m_falloff = 1.0f;
		// 定数
		float m_constant_attenuation = 0.0f;
		// 線形
		float m_linear_attenuation = 0.0f;
		// 二次
		float m_quadratic_attenuation = 25.0f;
		
		// カリングレイヤー
		int m_culling_layer = 0;

		// マスクテクスチャ
		bool m_use_mask_texture = false;
		Texture m_mask_texture;
		FileLoadStateData m_mask_texture_data;
		
		// Shadow
		// http://www.project-asura.com/program/d3d11/d3d11_008.html
		// https://docs.microsoft.com/ja-jp/windows/win32/dxtecharts/cascaded-shadow-maps
		// https://developer.nvidia.com/gpugems/gpugems3/part-ii-light-and-shadows/chapter-10-parallel-split-shadow-maps-programmable-gpus
		// ライトに対応したシャドウマップクラス
		std::unique_ptr<ShadowMapBase> m_shadow_map = nullptr;
		// シャドウマップを作成するか
		bool m_do_create_shadow = false;
		// 影を描画するか
		bool m_is_render_shadow = false;
		// 影の強さ(0.0 <= x <= 1.0)
		float m_strength = 0.4f;
		// バイアス(0.0 <= x <= 1.0)
		float m_bias = 0.001f;
		// 法線バイアス(0.0 <= x <= 1.0)
		float m_normal_bias = 0.005f;
		// ライトビューの近平面(0.01 <= x <= 10.0)
		float m_shadow_near_plane = 0.1f;
		// ライトビューの遠平面(min(range, far)する)
		float m_max_shadow_far_plane = 100.0f;
		// ~Shadow

	};

	// --------------------------------------------------------------
	// inline
	// --------------------------------------------------------------
	inline bool Light::GetCreateShadow() const
	{
		return m_do_create_shadow;
	}
	inline bool Light::GetRenderShadow() const
	{
		return m_is_render_shadow;
	}
	inline float Light::GetStrength() const
	{
		return m_strength;
	}
	inline float Light::GetBias() const
	{
		return m_bias;
	}
	inline float Light::GetNormalBias() const
	{
		return m_normal_bias;
	}
	inline void Light::UseLightMask(const bool use)
	{
		m_use_mask_texture = use;
	}
	inline bool Light::UseLightMask() const
	{
		return m_use_mask_texture;
	}
	
}// namespace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::Light, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::Light, "TKGEngine::Light")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Behaviour, TKGEngine::Light)
// archive時にload saveを使用する曖昧さ回避
CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(TKGEngine::Light, cereal::specialization::member_load_save)