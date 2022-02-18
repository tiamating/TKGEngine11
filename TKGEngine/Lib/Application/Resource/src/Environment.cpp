

#include "../inc/Environment.h"
#include "Systems/inc/StateManager.h"
#include "Utility/inc/myfunc_file.h"
#include "Application/Objects/Components/interface/ICamera.h"


namespace TKGEngine
{
	Environment::Environment()
	{
		/* nothing */
	}

	Environment::~Environment()
	{
		/* nothing */
	}

	Environment::Environment(const Environment& env)
	{
		m_skybox_file_data = env.m_skybox_file_data;
		m_skybox_texture = env.m_skybox_texture;

		m_ambient_light_type = env.m_ambient_light_type;
		for (int i = 0; i < static_cast<int>(GradientType::MAX_NUM_GradientType); ++i)
		{
			m_ambient_light_colors[i] = env.m_ambient_light_colors[i];
		}

		m_use_fog = env.m_use_fog;
		m_fog_type = env.m_fog_type;
		m_fog_color = env.m_fog_color;
		m_fog_density = env.m_fog_density;
		m_fog_start_distance = env.m_fog_start_distance;
		m_fog_end_distance = env.m_fog_end_distance;
	}

	Environment& Environment::operator=(const Environment& env)
	{
		m_skybox_file_data = env.m_skybox_file_data;
		m_skybox_texture = env.m_skybox_texture;

		m_ambient_light_type = env.m_ambient_light_type;
		for (int i = 0; i < static_cast<int>(GradientType::MAX_NUM_GradientType); ++i)
		{
			m_ambient_light_colors[i] = env.m_ambient_light_colors[i];
		}

		m_use_fog = env.m_use_fog;
		m_fog_type = env.m_fog_type;
		m_fog_color = env.m_fog_color;
		m_fog_density = env.m_fog_density;
		m_fog_start_distance = env.m_fog_start_distance;
		m_fog_end_distance = env.m_fog_end_distance;

		return *this;
	}

#ifdef USE_IMGUI
	void Environment::OnGUI()
	{
		// SkyBox
		ImGui::Text("SkyBox");
		ImGui::Indent(ImGui::INDENT_VALUE);
		{
			// sRGB
			ImGui::Text("Force sRGB");
			ImGui::AlignedSameLine(0.6f);
			ImGui::Checkbox("##force sRGB", &m_force_srgb);
			// Path
			ImGui::Text("Path");
			ImGui::Text("\"%s\"", m_skybox_file_data.filepath.c_str());
			// Texture
			if (ImGui::Button("Load"))
			{
				std::string filepath;
				if (MyFunc::FetchOpenFileName(
					filepath,
					TEXT("SkyBox Texture(*.dds)\0*.dds;\0"),
					TEXT("Load Texture"),
					TEXT(".\\Asset\\Textures")
				))
				{
					LoadSkyBox(filepath, m_force_srgb);
				}
			}
		}
		ImGui::Unindent(ImGui::INDENT_VALUE);
		ImGui::Separator();
		// Ambient
		ImGui::Text("Ambient");
		ImGui::Indent(ImGui::INDENT_VALUE);
		{
			// Type
			{
				constexpr const char* ambient_type_names[] =
				{
					"HemiSphere",
					"Color"
				};
				constexpr int type_array_size = std::size(ambient_type_names);
				int current_type = static_cast<int>(m_ambient_light_type);
				ImGui::Text("Type");
				ImGui::AlignedSameLine(0.6f);
				if (ImGui::Combo("##Ambient Type", &current_type, ambient_type_names, type_array_size))
				{
					m_ambient_light_type = static_cast<AmbientLightType>(current_type);
				}
			}
			// Color
			switch (m_ambient_light_type)
			{
				case AmbientLightType::Color:
				{
					ImGui::Text("Color");
					ImGui::AlignedSameLine(0.6f);

					ImGui::ColorEdit3("##Ambient Color", &m_ambient_light_colors[static_cast<int>(GradientType::Single)].x);
				}
				break;
				case AmbientLightType::HemiSphere:
				{
					ImGui::Text("Upper");
					ImGui::AlignedSameLine(0.6f);
					ImGui::ColorEdit3("##Ambient Color Upper", &m_ambient_light_colors[static_cast<int>(GradientType::Upper)].x);
					ImGui::Text("Lower");
					ImGui::AlignedSameLine(0.6f);
					ImGui::ColorEdit3("##Ambient Color Lower", &m_ambient_light_colors[static_cast<int>(GradientType::Lower)].x);
				}
				break;
			}
		}
		ImGui::Unindent(ImGui::INDENT_VALUE);
		ImGui::Separator();
		// Fog
		ImGui::Text("Fog");
		ImGui::AlignedSameLine(0.6f);
		ImGui::Checkbox("##Use Fog Flag", &m_use_fog);
		ImGui::Indent(ImGui::INDENT_VALUE);
		{
			// Color
			ImGui::Text("Color");
			ImGui::AlignedSameLine(0.6f);
			ImGui::ColorEdit3("##Fog Color", &m_fog_color.x);
			// Type
			{
				constexpr const char* fog_type_name[] =
				{
					"Linear",
					"Exponential",
					"Exponential Squared"
				};
				constexpr int type_array_size = std::size(fog_type_name);
				int current_type = static_cast<int>(m_fog_type);
				ImGui::Text("Type");
				ImGui::AlignedSameLine(0.6f);
				if (ImGui::Combo("##Fog Type", &current_type, fog_type_name, type_array_size))
				{
					m_fog_type = static_cast<FogType>(current_type);
				}
			}
			// 距離または密度
			switch (m_fog_type)
			{
				case FogType::Linear:
				{
					// Distance
					ImGui::Text("Fog Begin");
					ImGui::AlignedSameLine(0.6f);
					ImGui::DragFloat("##Fog Begin Distance", &m_fog_start_distance, 0.002f,
						0.0f, m_fog_end_distance, "%.3f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_Logarithmic);
					ImGui::Text("Fog End");
					ImGui::AlignedSameLine(0.6f);
					ImGui::DragFloat("##Fog End Distance", &m_fog_end_distance, 0.002f,
						m_fog_start_distance, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_Logarithmic);
				}
				break;
				case FogType::Exponential:
				case FogType::ExponentialSquared:
				{
					// Density
					ImGui::Text("Density");
					ImGui::AlignedSameLine(0.6f);
					ImGui::DragFloat("##Fog Density", &m_fog_density, 0.002f,
						0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat);
				}
				break;
			}

			// TODO : ミー散乱のパラメータの定義
			// https://qiita.com/neusstudio/items/844f87e472f65fc21572
		}
		ImGui::Unindent(ImGui::INDENT_VALUE);
	}
#endif// #ifdef USE_IMGUI

	bool Environment::SetSkyMap(ID3D11DeviceContext* p_context, int slot) const
	{
		if (!m_skybox_texture.HasTexture())
			return false;

		m_skybox_texture.SetSRV(p_context, slot, ShaderVisibility::PS);
		return true;
	}

	void Environment::RenderSkyBox(ID3D11DeviceContext* p_context) const
	{
		if (!SetSkyMap(p_context, TEXSLOT_GLOBALENVMAP))
			return;

		StateManager::SetState(p_context, StateManager::BS::Opaque);
		StateManager::SetState(p_context, StateManager::RS::FillBack);
		StateManager::SetState(p_context, StateManager::DS::BackGround, Stencil::Mask::None, false);

		p_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		p_context->Draw(4, 0);
	}

	void Environment::SetParam(CB_Environment& cb)
	{
		switch (m_ambient_light_type)
		{
			case AmbientLightType::Color:
			{
				cb.ambient_colors[static_cast<int>(GradientType::Upper)]
					= cb.ambient_colors[static_cast<int>(GradientType::Lower)]
					= m_ambient_light_colors[static_cast<int>(GradientType::Single)];
			}
			break;
			case AmbientLightType::HemiSphere:
			{
				cb.ambient_colors[static_cast<int>(GradientType::Upper)] = m_ambient_light_colors[static_cast<int>(GradientType::Upper)];
				cb.ambient_colors[static_cast<int>(GradientType::Lower)] = m_ambient_light_colors[static_cast<int>(GradientType::Lower)];
			}
			break;
		}
		// Fogの計算用URL
		// https://light11.hatenadiary.com/entry/2019/04/04/214958
		cb.fog_color = m_fog_color;
		cb.fog_density = m_fog_density;
		cb.fog_start_distance = m_fog_start_distance;
		cb.fog_end_distance = m_fog_end_distance;
		cb.fog_type = static_cast<unsigned>(m_fog_type);
		cb.use_fog = m_use_fog;

	}

	void Environment::LoadSkyBox(const std::string& filepath, bool force_srgb)
	{
		m_skybox_texture.Load(filepath, force_srgb);
		// Loadしたテクスチャがキューブマップでなければ破棄
		if (m_skybox_texture.HasTexture() && m_skybox_texture.GetData()->option != TEXTURE_OPTION::TEXTURE_OPTION_CUBEMAP)
		{
			m_skybox_texture.Release();
		}
		else
		{
			m_skybox_file_data.Set(filepath);
		}
	}
	void Environment::LoadAsyncSkyBox(const std::string& filepath, bool force_srgb)
	{
		m_skybox_texture.LoadAsync(filepath, force_srgb);
		m_skybox_file_data.Set(filepath);
		m_force_srgb = force_srgb;
	}

	void Environment::SetAmbientLightColorUpper(const VECTOR4& color)
	{
		m_ambient_light_colors[static_cast<int>(GradientType::Upper)] = color;
	}

	void Environment::SetAmbientLightColorLower(const VECTOR4& color)
	{
		m_ambient_light_colors[static_cast<int>(GradientType::Lower)] = color;
	}


	void Environment::SetAmbientLightColor(const VECTOR4& color)
	{
		m_ambient_light_colors[static_cast<int>(GradientType::Single)] = color;
	}

	void Environment::SetFogActive(bool active)
	{
		m_use_fog = active;
	}

	void Environment::SetFogColor(const VECTOR4& color)
	{
		m_fog_color = color;
	}

	void Environment::SetFogDensity(float density)
	{
		m_fog_density = density;
	}

	void Environment::SetFogStartDistance(float distance)
	{
		m_fog_start_distance = distance;
	}

	void Environment::SetFogEndDistance(float distance)
	{
		m_fog_end_distance = distance;
	}

}// namespace TKGEngine