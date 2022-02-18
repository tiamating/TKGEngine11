#pragma once

#include "Systems/inc/TKGEngine_Defined.h"
#include "Utility/inc/myfunc_vector.h"
#include "Application/Resource/inc/Asset_Defined.h"
#include "Texture.h"

#include <string>
#include <memory>


struct ID3D11DeviceContext;

namespace TKGEngine
{
	enum class GradientType
	{
		Single = 0,		// 単色時用
		Upper = Single,	// 上方
		Lower,			// 下方

		MAX_NUM_GradientType
	};

	struct CB_Environment
	{
		VECTOR4 ambient_colors[static_cast<int>(GradientType::MAX_NUM_GradientType)];

		VECTOR4 fog_color;
		float fog_density;
		float fog_start_distance;
		float fog_end_distance;
		unsigned fog_type;
		bool use_fog;
	};


	class Environment
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Environment();
		virtual ~Environment();
		Environment(const Environment&);
		Environment& operator=(const Environment&);

#ifdef USE_IMGUI
		void OnGUI();
#endif// #ifdef USE_IMGUI

		bool SetSkyMap(ID3D11DeviceContext* p_context, int slot) const;
		void RenderSkyBox(ID3D11DeviceContext* p_context) const;

		void SetParam(CB_Environment& cb);

		// SkyBox
		void LoadSkyBox(const std::string& filepath, bool force_srgb = false);
		void LoadAsyncSkyBox(const std::string& filepath, bool force_srgb = false);
		// ~SkyBox

		// Ambient
		void SetAmbientLightColorUpper(const VECTOR4& color);
		void SetAmbientLightColorLower(const VECTOR4& color);
		void SetAmbientLightColor(const VECTOR4& color);
		// ~Ambient

		// Fog
		void SetFogActive(bool active);
		void SetFogColor(const VECTOR4& color);
		void SetFogDensity(float density);
		void SetFogStartDistance(float distance);
		void SetFogEndDistance(float distance);
		// ~Fog


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		friend class cereal::access;
		template <class Archive>
		void load(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(m_skybox_file_data),
					CEREAL_NVP(m_ambient_light_type),
					CEREAL_NVP(m_force_srgb),
					CEREAL_NVP(m_use_skybox_texture),
					CEREAL_NVP(m_ambient_light_colors[static_cast<int>(GradientType::Upper)]),
					CEREAL_NVP(m_ambient_light_colors[static_cast<int>(GradientType::Lower)]),
					CEREAL_NVP(m_use_fog),
					CEREAL_NVP(m_fog_type),
					CEREAL_NVP(m_fog_color),
					CEREAL_NVP(m_fog_density),
					CEREAL_NVP(m_fog_start_distance),
					CEREAL_NVP(m_fog_end_distance)
				);
				// スカイボックスが書き出されていたらロードする
				if(m_skybox_file_data.HasData())
				{
					LoadSkyBox(m_skybox_file_data.filepath, m_force_srgb);
				}
			}
		}
		template <class Archive>
		void save(Archive& archive, const std::uint32_t version) const
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(m_skybox_file_data),
					CEREAL_NVP(m_ambient_light_type),
					CEREAL_NVP(m_force_srgb),
					CEREAL_NVP(m_use_skybox_texture),
					CEREAL_NVP(m_ambient_light_colors[static_cast<int>(GradientType::Upper)]),
					CEREAL_NVP(m_ambient_light_colors[static_cast<int>(GradientType::Lower)]),
					CEREAL_NVP(m_use_fog),
					CEREAL_NVP(m_fog_type),
					CEREAL_NVP(m_fog_color),
					CEREAL_NVP(m_fog_density),
					CEREAL_NVP(m_fog_start_distance),
					CEREAL_NVP(m_fog_end_distance)
				);
			}
		}

		// ==============================================
		// private struct
		// ==============================================
		enum class AmbientLightType
		{
			HemiSphere = 0,	// Upper<-->Lower
			Color,			// single

			MAX_NUM_AmbientLightType
		};

		enum class FogType
		{
			Linear = 0,			// リニア
			Exponential,		// 指数
			ExponentialSquared,	// 乗指数

			MAX_NUM_FOG_TYPE
		};

		// ==============================================
		// private methods
		// ==============================================
		/* nothing */


		// ==============================================
		// private variables
		// ==============================================
		// SkyBox
		FileLoadStateData m_skybox_file_data;
		Texture m_skybox_texture;
		bool m_force_srgb = false;
		bool m_use_skybox_texture = true;	// TODO : SkyBoxの処理をシェーダーで行うかの変更
		// ~SkyBox

		// Ambient
		AmbientLightType m_ambient_light_type = AmbientLightType::Color;
		VECTOR4 m_ambient_light_colors[static_cast<int>(GradientType::MAX_NUM_GradientType)];
		// ~Ambient

		// Fog
		bool m_use_fog = false;
		FogType m_fog_type = FogType::ExponentialSquared;
		VECTOR4 m_fog_color;
		float m_fog_density = 0.0f;	// Fog密度
		float m_fog_start_distance = 0.3f;	// Fogが始まる正規化深度
		float m_fog_end_distance = 0.8f;	// Fogが終わる正規化深度
		// ~Fog
	};

}// namespace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::Environment, 1);
// archive時にload saveを使用する曖昧さ回避
CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(TKGEngine::Environment, cereal::specialization::member_load_save)