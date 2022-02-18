#include "../inc/CLight.h"

#include "Components/interface/ICamera.h"
#include "Components/inc/CTransform.h"
#include "Application/inc/ProjectSetting.h"
#include "Managers/LightManager.h"
#include "Utility/inc/myfunc_file.h"

//////////////////////////////////
// Shadow Type Header
//////////////////////////////////
#include "Application/Objects/Shadow/PSSM/PSSM.h"
//////////////////////////////////

#ifdef USE_IMGUI
#include "Systems/inc/IGUI.h"
#endif // USE_IMGUI

REGISTERCOMPONENT(TKGEngine::Light);

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	Light::Light()
		: Behaviour(COMPONENT_TYPE::COMPONENT_GENERAL)
	{
		/* nothing */
	}

	Light::~Light()
	{
		/* nothing */
	}

#ifdef USE_IMGUI
	void Light::OnGUI()
	{
		ImGui::PushID(this);
		// Behaviour::OnGUIの実行
		Behaviour::OnGUI();

		// Type
		{
			int current_item = static_cast<int>(m_light_type);
			ImGui::Text("Type");
			ImGui::AlignedSameLine(0.5f);
			if (ImGui::Combo(
				"##Type", &current_item,
				"Directional\0Spot\0Point"
			))
			{
				SetType(static_cast<LightType>(current_item));
			}
		}
		// Color
		{
			ImGui::Text("Color");
			ImGui::AlignedSameLine(0.5f);
			ImGui::ColorEdit4("##Color", &m_light_color.x);
		}
		// Intensity
		{
			ImGui::Text("Intensity");
			ImGui::AlignedSameLine(0.5f);
			ImGui::DragFloat("##Intensity", &m_intensity, 0.1f, 0.0f, 100.0f, "(0 ~ 100) %.1f", ImGuiSliderFlags_AlwaysClamp);
		}
		// 平行光源以外のパラメータ
		if (m_light_type != LightType::Directional)
		{
			// Range
			ImGui::Text("Range");
			ImGui::AlignedSameLine(0.5f);
			ImGui::DragFloat("##Range", &m_range, 0.05f, 0.0f, 1000.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);

			// 減衰係数
			ImGui::Spacing();
			ImGui::Text("Attenuation");
			ImGui::SameLine();
			ImGui::HelpMarker("When the natural attenuation is quadratic = 25.");
			ImGui::Indent(ImGui::INDENT_VALUE);
			ImGui::Text("Const");
			ImGui::AlignedSameLine(0.5f);
			ImGui::DragFloat("##Const", &m_constant_attenuation, 0.1f, 0.0f, 100.0f, "(0 ~ 100) %.2f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::Text("Linear");
			ImGui::AlignedSameLine(0.5f);
			ImGui::DragFloat("##Linear", &m_linear_attenuation, 0.1f, 0.0f, 100.0f, "(0 ~ 100) %.2f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::Text("Quadratic");
			ImGui::AlignedSameLine(0.5f);
			ImGui::DragFloat("##Quadratic", &m_quadratic_attenuation, 0.1f, 0.01f, 100.0f, "(0 ~ 100) %.2f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::Unindent(ImGui::INDENT_VALUE);
		}
		// スポットライトのパラメータ
		if (m_light_type == LightType::Spot)
		{
			ImGui::Spacing();
			// Spot angle
			ImGui::Text("Spot angle (1.0 < Inner < Outer < 179.0)");
			{
				ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
				// 外側角
				ImGui::Text("Outer angle");
				ImGui::AlignedSameLine(0.5f);
				ImGui::DragFloat("##Outer angle", &m_spot_angle_outer, 0.1f, m_spot_angle_inner + 0.1f, 179.0f, "%.1f(deg)", ImGuiSliderFlags_AlwaysClamp);
				//内側角
				ImGui::Text("Inner angle");
				ImGui::AlignedSameLine(0.5f);
				ImGui::DragFloat("##Inner angle", &m_spot_angle_inner, 0.1f, 1.0f, m_spot_angle_outer - 0.1f, "%.1f(deg)", ImGuiSliderFlags_AlwaysClamp);
			}

			// 減衰係数
			ImGui::Text("Fall out");
			ImGui::AlignedSameLine(0.5f);
			ImGui::SliderFloat("##Fall out", &m_falloff, 0.1f, 5.0f, "(0.1 ~ 5.0) %.1f", ImGuiSliderFlags_AlwaysClamp);

			// マスクテクスチャ
			ImGui::Text("Use Mask Texture");
			ImGui::AlignedSameLine(0.5f);
			ImGui::Checkbox("##Use Mask Texture", &m_use_mask_texture);
			if (m_use_mask_texture)
			{
				ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
				// Path
				ImGui::Text("Path");
				ImGui::Text("\"%s\"", m_mask_texture_data.filepath.c_str());
				// Texture
				ImGui::Text("Texture");
				{
					ImGui::IndentWrapped tex_indent(ImGui::INDENT_VALUE);
					m_mask_texture.OnGUI();
				}
				if (ImGui::Button("Load"))
				{
					std::string filepath;
					if (MyFunc::FetchOpenFileName(
						filepath,
						TEXT("Texture(*.png,*.dds,*.jpg,*.jpeg,*.bmp,*.gif,*.tiff,*.tga,*.hdr)\0*.png;*.dds;*.jpg;*.jpeg;*.bmp;*.gif;*.tiff;*.tga;*.hdr\0"),
						TEXT("Load Texture"),
						TEXT(".\\Asset\\Textures")
					))
					{
						m_mask_texture.Load(filepath.c_str(), false);
						m_mask_texture_data.Set(filepath);
					}
				}
			}
		}
		// Shadow
		{
			// Create Shadow map
			ImGui::Text("Create Shadow Map");
			ImGui::AlignedSameLine(0.5f);
			bool do_create_shadow = m_do_create_shadow;
			if (ImGui::Checkbox("##Create Shadow Map", &do_create_shadow))
			{
				SetCreateShadow(do_create_shadow);
			}
			if (m_do_create_shadow)
			{
				// Draw Shadow
				ImGui::Text("Draw Shadow");
				ImGui::AlignedSameLine(0.5f);
				bool is_render_shadow = m_is_render_shadow;
				if (ImGui::Checkbox("##Draw Shadow", &is_render_shadow))
				{
					SetRenderShadow(is_render_shadow);
				}
				if (m_is_render_shadow)
				{
					// Shadow param
					ImGui::Separator();
					ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
					ImGui::Text("Strength");
					ImGui::AlignedSameLine(0.5f);
					ImGui::DragFloat("##Shadow Strength", &m_strength, 0.01f, 0.0f, 1.0f, "(0 ~ 1) %.2f", ImGuiSliderFlags_AlwaysClamp);
					ImGui::Text("Bias");
					ImGui::AlignedSameLine(0.5f);
					ImGui::DragFloat("##Shadow Bias", &m_bias, 0.00001f, 0.0f, 1.0f, "(0 ~ 1) %.5f", ImGuiSliderFlags_AlwaysClamp);
					ImGui::Text("Normal Bias");
					ImGui::AlignedSameLine(0.5f);
					ImGui::DragFloat("##Shadow Normal Bias", &m_normal_bias, 0.00001f, 0.0f, 1.0f, "(0 ~ 1) %.5f", ImGuiSliderFlags_AlwaysClamp);
					ImGui::Text("Near Plane");
					ImGui::AlignedSameLine(0.5f);
					ImGui::DragFloat("##Shadow Near", &m_shadow_near_plane, 0.01f, 0.01f, 10.0f, "(0.01 ~ 10) %.2f", ImGuiSliderFlags_AlwaysClamp);
					ImGui::Text("Far Plane");
					ImGui::AlignedSameLine(0.5f);
					ImGui::DragFloat("##Shadow Far", &m_max_shadow_far_plane, 0.02f, m_shadow_near_plane, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
					if (m_shadow_map)
					{
						m_shadow_map->OnGUI();
					}
				}
			}
		}

		// カリングレイヤー
		ImGui::Text("TODO : No using layer");
		OnGUICullingLayer();

		// Debug表示
		constexpr VECTOR3 debug_color = VECTOR3(0.7f, 0.7f, 0.0f);
		switch (m_light_type)
		{
			case LightType::Spot:
			{
				// DrawConeが下向き、中央座標指定のため補正が必要
				const auto transform = GetTransform();
				const float radius = tanf(MyMath::AngleToRadian(m_spot_angle_outer * 0.5f)) * m_range;
				const VECTOR3 forward = transform->Forward();
				const VECTOR3 spot_pos = transform->Position() + (0.5f * m_range * forward);
				const Quaternion rot_down_to_forward = Quaternion::FromToRotation(VECTOR3::Down, VECTOR3::Forward);
				IGUI::Get().DrawCone(spot_pos, rot_down_to_forward * transform->Rotation(), radius, m_range, debug_color);
			}
			break;
			case LightType::Point:
				IGUI::Get().DrawSphere(GetTransform()->Position(), m_range, debug_color);
				break;
		}

		ImGui::PopID();
	}

	void Light::OnGUICullingLayer()
	{
		// ボタンを押したらセレクトウィンドウを開く
		ImGui::Text("Select culling layer");
		if (ImGui::Button("Open"))
		{
			m_is_opened_culling_layer_window = true;
		}

		// カリングレイヤー選択ウィンドウ
		if (m_is_opened_culling_layer_window)
		{
			constexpr ImGuiWindowFlags flags =
				ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize ^ ImGuiWindowFlags_NoCollapse;
			const ImVec2 mouse_pos = ImGui::GetMousePos();
			const ImVec2 window_size(300.0f, 450.0f);
			const ImVec2 window_pos = ImVec2(mouse_pos.x - window_size.x * 0.5f, mouse_pos.y - window_size.y * 0.5f);
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
			// 親ウィンドウの作成
			if (ImGui::Begin("Select Light culling Layer##Light", &m_is_opened_culling_layer_window, flags))
			{
				constexpr ImGuiTreeNodeFlags tree_flags =
					ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
				constexpr ImGuiWindowFlags child_flags = ImGuiWindowFlags_HorizontalScrollbar;
				ImGui::IndentWrapped indent(window_size.x * 0.05f);
				ImGui::BeginChild("Child Window", ImVec2(window_size.x * 0.85f, window_size.y - ImGui::GetCursorPosY() - ImGui::ENABLE_SIZE_FROM_BOTTOM), true, child_flags);
				{
					constexpr int layer_num = static_cast<int>(Layer::MAX_USER_LAYER_NUM);
					// 全てのレイヤーを表示
					for (int i = 0; i < layer_num; ++i)
					{
						const int current_mask = (1 << i);

						// チェックと名前のクリックに対応
						ImGui::IDWrapped id(i);
						bool is_current_culling = current_mask & m_culling_layer;
						// チェックボックス
						if (ImGui::Checkbox("##culling layer", &is_current_culling))
						{
							if (is_current_culling)
								m_culling_layer |= current_mask;
							else
								m_culling_layer &= ~current_mask;
						}
						ImGui::SameLine();
						// 名前クリック
						ImGui::TreeNodeEx(NAMEOF_ENUM(static_cast<Layer>(i)).data(), tree_flags);
						if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
						{
							if (!is_current_culling)
								m_culling_layer |= current_mask;
							else
								m_culling_layer &= ~current_mask;
						}
					}
				}
				ImGui::EndChild();
			}
			ImGui::End();
		}
	}
#endif // USE_IMGUI

	void Light::SetType(const LightType type)
	{
		// 前の種類と変更がある
		if (type != m_light_type)
		{
			m_light_type = type;
			
			// シャドウマップを作りなおす
			if (m_do_create_shadow)
			{
				CreateShadowMapObject();
			}
			else
			{
				RemoveShadowMapObject();
			}
		}
	}

	LightType Light::GetType() const
	{
		return m_light_type;
	}

	void Light::SetColor(const VECTOR4& color)
	{
		m_light_color = color;
	}

	VECTOR4 Light::GetColor() const
	{
		return m_light_color * m_intensity;
	}

	void Light::SetIntensity(const float intensity)
	{
		m_intensity = MyMath::Max(0.0f, intensity);
	}

	void Light::SetRange(const float range)
	{
		m_range = MyMath::Max(0.0f, range);
	}

	float Light::GetRange() const
	{
		return m_range;
	}

	void Light::SetSpotAngleInner(const float angle)
	{
		m_spot_angle_inner = MyMath::Clamp(angle, 1.0f, m_spot_angle_outer);
	}

	float Light::GetSpotAngleInner() const
	{
		return m_spot_angle_inner;
	}

	void Light::SetSpotAngleOuter(const float angle)
	{
		m_spot_angle_outer = MyMath::Clamp(angle, m_spot_angle_inner, 179.0f);
	}

	float Light::GetSpotAngleOuter() const
	{
		return m_spot_angle_outer;
	}

	void Light::SetFalloff(const float falloff)
	{
		m_falloff = MyMath::Min(falloff, 0.1f);
	}

	float Light::GetFalloff() const
	{
		return m_falloff;
	}

	void Light::SetAttenuation(float constant, float linear, float quadratic)
	{
		m_constant_attenuation = MyMath::Max(0.0f, constant);
		m_linear_attenuation = MyMath::Max(0.0f, linear);
		m_quadratic_attenuation = MyMath::Max(0.01f, quadratic);
	}

	VECTOR3 Light::GetAttenuation() const
	{
		return VECTOR3(m_constant_attenuation, m_linear_attenuation, m_quadratic_attenuation);
	}

	void Light::SetCullingLayer(const int mask)
	{
		m_culling_layer = mask;
	}

	int Light::GetCullingLayer() const
	{
		return m_culling_layer;
	}

	void Light::SetCreateShadow(const bool do_create)
	{
		// 前の状態から変更があればシャドウマップオブジェクトの作成、破棄を行う
		if (m_do_create_shadow != do_create)
		{
			m_do_create_shadow = do_create;

			if (m_do_create_shadow)
			{
				CreateShadowMapObject();
			}
			else
			{
				RemoveShadowMapObject();
			}
		}
	}

	void Light::SetRenderShadow(const bool do_render)
	{
		m_is_render_shadow = do_render;
	}

	void Light::SetStrength(const float strength)
	{
		m_strength = MyMath::Clamp(strength, 0.0f, 1.0f);;
	}

	void Light::SetBias(const float bias)
	{
		m_bias = MyMath::Clamp(bias, 0.0f, 1.0f);
	}

	void Light::SetNormalBias(const float bias)
	{
		m_normal_bias = MyMath::Clamp(bias, 0.0f, 1.0f);
	}

	void Light::SetNearPlane(float near_plane)
	{
		m_shadow_near_plane = MyMath::Clamp(near_plane, 0.1f, 10.0f);
	}

	void Light::SetLightMaskTexture(ID3D11DeviceContext* context, const int slot) const
	{
		if (m_light_type == LightType::Spot || m_use_mask_texture)
			m_mask_texture.SetSRV(context, slot, ShaderVisibility::PS);
	}

	const MATRIX& Light::GetLVP(const int index) const
	{
		if (m_shadow_map == nullptr)
			return MATRIX::Identity;
		return m_shadow_map->GetLVP(index);
	}

	const MATRIX& Light::GetLV(const int index) const
	{
		if (m_shadow_map == nullptr)
			return MATRIX::Identity;
		return m_shadow_map->GetLV(index);
	}

	const MATRIX& Light::GetLP(const int index) const
	{
		if (m_shadow_map == nullptr)
			return MATRIX::Identity;
		return m_shadow_map->GetLP(index);
	}

	float Light::GetShadowSize(const int index) const
	{
		if (m_shadow_map == nullptr)
			return 1.0f;
		return m_shadow_map->GetShadowSize(index);
	}

	unsigned Light::GetCascadeNum() const
	{
		if (m_shadow_map == nullptr)
			return 0;
		return m_shadow_map->GetCascadeNum();
	}

	void Light::SetSRV(ID3D11DeviceContext* context, int slot, int index) const
	{
		if (m_shadow_map)
			m_shadow_map->SetSRV(context, slot, index);
	}

	void Light::OnCreate(const std::shared_ptr<Object>& p_self)
	{
		// マネージャークラスに登録
		LightManager::RegisterManager(std::static_pointer_cast<Light>(p_self));
	}

	void Light::OnDestroy()
	{
		// マネージャークラスから削除
		LightManager::UnregisterManager(GetOwnerID());
	}

	void Light::CreateShadowMapObject()
	{
		// 初期化
		m_shadow_map.reset();

		// TODO : シャドウマップの種類が少ないためLightTypeごとに指定している
		switch (m_light_type)
		{
			case LightType::Directional:
			{
				m_shadow_map = std::make_unique<PSSM>(m_light_type);
				m_shadow_map->Create();
			}
			break;
			case LightType::Spot:
			{
				// TODO : スポットライトの影対応
				SetRenderShadow(false);
			}
			break;
			case LightType::Point:
			{
				// TODO : ポイントライトは現在、影を描画しない
				LOG_ASSERT("Point light does not support shadow.");
				SetRenderShadow(false);
			}
			break;
		}
	}

	void Light::RemoveShadowMapObject()
	{
		m_shadow_map.reset();
	}

	int Light::CalculateShadowCaster(const std::vector<MainData>& scene_objects, const int object_num, const std::shared_ptr<ICamera>& camera) const
	{
		if (m_shadow_map == nullptr)
			return 0;
		LightShadowData light_data = {};
		const auto transform = GetTransform();
		light_data.light_up = transform->Up().Normalized();
		light_data.light_direction = transform->Forward().Normalized();
		light_data.near_plane = m_shadow_near_plane;
		// DirectionalならCameraのFarと比較
		if (m_light_type == LightType::Directional)
		{
			light_data.far_plane = MyMath::Min(camera->GetFar(), m_max_shadow_far_plane);
		}
		// それ以外ならRangeと比較
		else
		{
			light_data.far_plane = MyMath::Min(m_range, m_max_shadow_far_plane);
		}
		light_data.spot_angle = m_spot_angle_outer;
		return m_shadow_map->CalculateShadowCaster(scene_objects, object_num, camera, light_data);
	}

	bool Light::IsRenderShadowCaster() const
	{
		// TODO : ポイントライトは現在、影を描画しない
		if (m_light_type == LightType::Point)
			return false;
		// シャドウマップを作成済み、シャドウを描画する、シャドウを描画するオブジェクトが存在するならtrue
		return m_do_create_shadow && m_is_render_shadow && m_shadow_map->IsDrawShadowMap();
	}

	bool Light::IsRenderShadowCaster(int index) const
	{
		// TODO : ポイントライトは現在、影を描画しない
		if (m_light_type == LightType::Point)
			return false;
		// シャドウマップを作成済み、シャドウを描画する、シャドウを描画するオブジェクトが存在するならtrue
		return m_do_create_shadow && m_is_render_shadow && m_shadow_map->IsDrawShadowMap(index);
	}

	float Light::GetSamplingRadius() const
	{
		if (m_shadow_map == nullptr)
			return 0.0f;
		return m_shadow_map->GetSamplingRadius();
	}

	int Light::GetSamplingCount() const
	{
		if (m_shadow_map == nullptr)
			return 0;
		return m_shadow_map->GetSamplingCount();
	}

	void Light::RenderShadowCaster(ID3D11DeviceContext* context, VertexBuffer& instance_buffer) const
	{
		if (m_shadow_map == nullptr)
			return;
		m_shadow_map->RenderShadow(context, instance_buffer);
	}

}// namespace TKGEngine