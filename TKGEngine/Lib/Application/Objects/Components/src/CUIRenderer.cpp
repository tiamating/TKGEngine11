
#include "../inc/CUIRenderer.h"

#include "../interface/ICamera.h"
#include "Components/inc/CTransform.h"
#include "Systems/inc/IWindow.h"
#include "Application/Resource/inc/VertexBuffer.h"
#include "Utility/inc/myfunc_file.h"

#include <vector>

#include <d3d11.h>

REGISTERCOMPONENT(TKGEngine::UIRenderer);

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	UIRenderer::UIRenderer()
		: Renderer(RendererType::UI)
	{
		/* nothing */
	}

	UIRenderer::~UIRenderer()
	{
		/* nothing */
	}

#ifdef USE_IMGUI
	void UIRenderer::OnGUI()
	{
		ImGui::IDWrapped id(this);

		// Renderer::OnGUI
		Renderer::OnGUI();

		//// UIRenderer
		// Depth
		ImGui::Text("Depth");
		ImGui::SameLine();
		ImGui::HelpMarker("First <<< 0 <<< Last");
		ImGui::AlignedSameLine(0.5f);
		ImGui::DragInt("##Depth", &m_depth, 0.01f);
		// Pivot
		ImGui::Text("Pivot");
		ImGui::AlignedSameLine(0.5f);
		ImGui::ComboEnum<Pivot, Pivot::Max_Pivot>("##Pivot", &m_pivot);
		// 画面内
		ImGui::Text("On Viewport [normalize]");
		{
			ImGui::Text("Position");
			ImGui::AlignedSameLine(0.5f);
			ImGui::DragFloat2("##Position Draw", &m_draw_pos.x, 0.001f);
			ImGui::Text("Size");
			ImGui::AlignedSameLine(0.5f);
			ImGui::DragFloat2("##Size Draw", &m_draw_size.x, 0.001f);
		}
		// テクスチャ内
		ImGui::Text("On Texture [texel]");
		{
			ImGui::Text("Position");
			ImGui::AlignedSameLine(0.5f);
			ImGui::DragFloat2("##Position Texture", &m_sample_pos.x, 0.1f);
			ImGui::Text("Size");
			ImGui::AlignedSameLine(0.5f);
			ImGui::DragFloat2("##Size Texture", &m_sample_size.x, 0.1f);
		}
		// 回転
		ImGui::Text("Rotation[deg]");
		ImGui::AlignedSameLine(0.5f);
		if (ImGui::DragFloat3("##Rotation", &m_euler_angles.x, 0.5f, 0.0f, 0.0f, "%.2f"))
		{
			m_rotation = m_euler_angles.ToQuaternion();
		}
		// テクスチャ
		constexpr ImGuiTreeNodeFlags tree_flags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
		if (ImGui::TreeNodeEx("UI Texture", tree_flags))
		{
			// Slot
			ImGui::Text("Slot : TEXSLOT_SPRITE");
			// sRGB
			ImGui::Text("Force sRGB");
			ImGui::SameLine();
			ImGui::Checkbox("##force sRGB", &m_force_srgb);
			// Path
			ImGui::Text("Path");
			ImGui::Text("%s", m_texture_filedata.filepath.c_str());
			// Texture
			ImGui::Text("Texture");
			m_texture.OnGUI();
			if (ImGui::Button("Load"))
			{
				std::string filepath;
				if (MyFunc::FetchOpenFileName(filepath,
					TEXT("Texture(*.png,*.dds,*.jpg,*.jpeg,*.bmp,*.gif,*.tiff,*.tga,*.hdr)\0*.png;*.dds;*.jpg;*.jpeg;*.bmp;*.gif;*.tiff;*.tga;*.hdr\0"),
					TEXT("Load Texture"),
					TEXT(".\\Asset\\Textures")
				))
				{
					SetTexture(filepath.c_str(), m_force_srgb);
				}
			}
			ImGui::TreePop();
		}

		// マテリアル
		ImGui::Separator();
		OnGUIMaterial();
	}
#endif // USE_IMGUI

	bool UIRenderer::SetTexture(const char* filepath, bool force_srgb)
	{
		m_texture.Load(filepath, force_srgb);
		if (!m_texture.HasTexture())
			return false;
		// ファイルロード可能なテクスチャデータを持つ
		m_texture_filedata.Set(filepath);
		m_force_srgb = force_srgb;
		// テクスチャサイズを表示サイズに適用する
		m_sample_size.x = static_cast<float>(m_texture.GetData()->width);
		m_sample_size.y = static_cast<float>(m_texture.GetData()->height);
		return true;
	}

	bool UIRenderer::SetTexture(const Texture& texture)
	{
		m_texture = texture;
		if (!m_texture.HasTexture())
			return false;
		// 実行中に作成されたテクスチャデータのためデシリアライズ時にはロード不可
		m_texture_filedata.Clear();
		m_force_srgb = m_texture.GetForceSRGB();
		// テクスチャサイズを表示サイズに適用する
		m_sample_size.x = static_cast<float>(m_texture.GetData()->width);
		m_sample_size.y = static_cast<float>(m_texture.GetData()->height);
		return true;
	}

	bool UIRenderer::HasTexture() const
	{
		return m_texture.HasTexture();
	}

#pragma region Getter / Setter
	void UIRenderer::Depth(int depth)
	{
		m_depth = depth;
	}
	int UIRenderer::Depth() const
	{
		return m_depth;
	}

	void UIRenderer::SetPivot(Pivot pivot)
	{
		m_pivot = pivot;
	}

	void UIRenderer::DrawPos(const VECTOR2& pos)
	{
		m_draw_pos = pos;
	}
	void UIRenderer::DrawPos(const float x, const float y)
	{
		m_draw_pos.x = x;
		m_draw_pos.y = y;
	}
	void UIRenderer::DrawPosX(const float x)
	{
		m_draw_pos.x = x;
	}
	void UIRenderer::DrawPosY(const float y)
	{
		m_draw_pos.y = y;
	}
	VECTOR2 UIRenderer::DrawPos() const
	{
		return m_draw_pos;
	}

	void UIRenderer::DrawSize(const VECTOR2& size)
	{
		m_draw_size = size;
	}
	void UIRenderer::DrawSize(const float x, const float y)
	{
		m_draw_size.x = x;
		m_draw_size.y = y;
	}
	void UIRenderer::DrawSizeX(const float x)
	{
		m_draw_size.x = x;
	}
	void UIRenderer::DrawSizeY(const float y)
	{
		m_draw_size.y = y;
	}
	VECTOR2 UIRenderer::DrawSize() const
	{
		return m_draw_size;
	}

	void UIRenderer::SamplePos(const VECTOR2& pos)
	{
		m_sample_pos = pos;
	}
	void UIRenderer::SamplePos(const float x, const float y)
	{
		m_sample_pos.x = x;
		m_sample_pos.y = y;
	}
	void UIRenderer::SamplePosX(const float x)
	{
		m_sample_pos.x = x;
	}
	void UIRenderer::SamplePosY(const float y)
	{
		m_sample_pos.y = y;
	}
	VECTOR2 UIRenderer::SamplePos() const
	{
		return m_sample_pos;
	}

	void UIRenderer::SampleSize(const VECTOR2& size)
	{
		m_sample_size = size;
	}
	void UIRenderer::SampleSize(const float x, const float y)
	{
		m_sample_size.x = x;
		m_sample_size.y = y;
	}
	void UIRenderer::SampleSizeX(const float x)
	{
		m_sample_size.x = x;
	}
	void UIRenderer::SampleSizeY(const float y)
	{
		m_sample_size.y = y;
	}
	VECTOR2 UIRenderer::SampleSize() const
	{
		return m_sample_size;
	}

	void UIRenderer::Rotation(const Quaternion& quat)
	{
		m_rotation = quat;
		m_euler_angles = m_rotation.ToEulerAngles();
	}
	Quaternion UIRenderer::Rotation() const
	{
		return m_rotation;
	}

	void UIRenderer::EulerAngles(const VECTOR3& euler)
	{
		m_euler_angles = euler;
		m_rotation = m_euler_angles.ToQuaternion();
	}
	void UIRenderer::EulerAngles(const float degree_x, const float degree_y, const float degree_z)
	{
		m_euler_angles = VECTOR3(degree_x, degree_y, degree_z);
		m_rotation = m_euler_angles.ToQuaternion();
	}
	void UIRenderer::EulerAngleX(const float degree_x)
	{
		m_euler_angles.x = degree_x;
		m_rotation = m_euler_angles.ToQuaternion();
	}
	void UIRenderer::EulerAngleY(const float degree_y)
	{
		m_euler_angles.y = degree_y;
		m_rotation = m_euler_angles.ToQuaternion();
	}
	void UIRenderer::EulerAngleZ(const float degree_z)
	{
		m_euler_angles.z = degree_z;
		m_rotation = m_euler_angles.ToQuaternion();
	}
	VECTOR3 UIRenderer::EulerAngles() const
	{
		return m_euler_angles;
	}
#pragma endregion

	void UIRenderer::OnCreated()
	{
		// ShadowCastingModeはOFF
		SetShadowCastMode(ShadowCastingMode::OFF);

		// マテリアル数は常に1つ
		ResizeMaterial(1);
		// デシリアライズ時にテクスチャデータがあればロードする
		if (m_texture_filedata.HasData())
		{
			m_texture.Load(m_texture_filedata.filepath, m_force_srgb);
		}
	}

	void UIRenderer::Render(
		ID3D11DeviceContext* p_context,
		int index, int start_index, int instance_count,
		VertexBuffer& instance_buffer,
		const std::shared_ptr<ICamera>& camera,
		bool write_depth
	)
	{
		assert(p_context != nullptr);

		// Material
		assert(index == 0);
		const auto* mat = &m_materials.at(index);
		if (!mat->HasMaterial())
		{
			mat = &Material::GetDefaultMaterial();
		}
		// マテリアルの有効化が描画に十分でないならドローコールをしない
		if (!mat->Activate(p_context, write_depth, write_depth))
		{
			return;
		}

		// UI用設定、テクスチャセット
		p_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		StateManager::SetState(p_context, StateManager::RS::FillNone);
		if (m_texture.HasTexture())
		{
			m_texture.SetSRV(p_context, TEXSLOT_SPRITE, ShaderVisibility::PS);
		}
		else
		{
			Texture::SetDummyWhiteSRV(p_context, TEXSLOT_SPRITE, ShaderVisibility::PS);
		}
		// Set VBs and IB
		for (int i = 0; i <= static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD7); ++i)
		{
			const int slot = mat->GetInputSlotFromType(i);
			if (slot < 0) continue;

			GetQuad().ActivateVB(p_context, slot, i);
		}
		GetQuad().ActivateIB(p_context);

		// Set InstanceBuffer
		{
			const int slot = mat->GetInputSlotFromType(static_cast<int>(VERTEX_ELEMENT_TYPE::UI_INSTANCE));
			if (slot > 0)
				instance_buffer.Set(p_context, slot);
		}

		p_context->DrawIndexedInstanced(4, instance_count, 0, 0, start_index);
	}

	void UIRenderer::SetInstance(UIInstance* p_instance)
	{
		p_instance->transform_matrix = m_current_transform_matrix;
		p_instance->instance_color = m_instance_color;
		p_instance->texture_transform = m_texcoord_transform;
	}

	int UIRenderer::GetSubsetCount() const
	{
		return 1;
	}

	unsigned UIRenderer::GetHashMesh() const
	{
		return 0;
	}

	unsigned UIRenderer::GetHashMaterial(int index) const
	{
		assert(index == 0);

		return m_materials.at(index).GetHash();
	}

	int UIRenderer::GetRenderQueue(int index) const
	{
		// UIはDepth依存
		return 0;
	}

	ShadowCastingMode UIRenderer::GetShadowCastMode() const
	{
		return ShadowCastingMode::OFF;
	}

	void UIRenderer::CalculateRenderParameter(const std::shared_ptr<ICamera>& camera)
	{
		const float vp_width = camera->GetViewportWidth();
		const float vp_height = camera->GetViewportHeight();

		// 親子関係はPositionのみ
		// Scale
		VECTOR3 scaling;
		{
			scaling = VECTOR3(m_draw_size.x * vp_width, m_draw_size.y * vp_height, 1.0f);
			scaling *= 0.5f;
		}
		// Rotate center
		VECTOR3 origin;
		VECTOR2 offset;
		{
			CalculateOriginFromPivot(origin, offset);
		}
		// Translate
		VECTOR3 translation = VECTOR3(m_draw_pos.x * vp_width, m_draw_pos.y * vp_height, 0.0f);
		translation.y *= -1.0f;
		translation.z = 0.0f;
		translation.x += (offset.x * scaling.x);
		translation.y += (offset.y * scaling.y);

		// Calculate transform matrix.
		{
			// Create Affine transform matrix.
			const MATRIX affine =
				DirectX::XMMatrixAffineTransformation(scaling, origin * scaling, m_rotation, translation);
			// Create matrix from Screen to NDC.
			const MATRIX StoN = {
				2.0f / vp_width	,0.0f				,0.0f	,0.0f,
				0.0f			,2.0f / vp_height	,0.0f	,0.0f,
				0.0f			,0.0f				,1.0f	,0.0f,
				-1.0f			,1.0f				,1.0f	,1.0f
			};

			m_current_transform_matrix = affine * StoN;
		}

		// Calculate texcoord transform matrix.
		{
			const auto tex_data = m_texture.HasTexture() ? m_texture.GetData() : Texture::GetDummyTextureData();
			
			const float tw = static_cast<float>(tex_data->width);
			const float th = static_cast<float>(tex_data->height);

			m_texcoord_transform =
				VECTOR4(
					m_sample_pos.x / tw,
					m_sample_pos.y / th,
					m_sample_size.x / tw,
					m_sample_size.y / th
				);
		}
	}

	unsigned UIRenderer::GetHashTexture() const
	{
		return m_texture.GetHash();
	}

	void UIRenderer::CalculateOriginFromPivot(VECTOR3& origin, VECTOR2& offset) const
	{
		switch (m_pivot)
		{
			case Pivot::Center:
				origin = VECTOR3(0.0f, 0.0f, 0.0f);
				offset = VECTOR2(0.0f, 0.0f);
				break;
			case Pivot::LeftTop:
				origin = VECTOR3(-1.0f, 1.0f, 0.0f);
				offset = VECTOR2(1.0f, -1.0f);
				break;
			case Pivot::RightTop:
				origin = VECTOR3(1.0f, 1.0f, 0.0f);
				offset = VECTOR2(-1.0f, -1.0f);
				break;
			case Pivot::LeftBottom:
				origin = VECTOR3(-1.0f, -1.0f, 0.0f);
				offset = VECTOR2(1.0f, 1.0f);
				break;
			case Pivot::RightBottom:
				origin = VECTOR3(1.0f, -1.0f, 0.0f);
				offset = VECTOR2(-1.0f, 1.0f);
				break;
			case Pivot::Top:
				origin = VECTOR3(0.0f, 1.0f, 0.0f);
				offset = VECTOR2(0.0f, -1.0f);
				break;
			case Pivot::Bottom:
				origin = VECTOR3(0.0f, -1.0f, 0.0f);
				offset = VECTOR2(0.0f, 1.0f);
				break;
			case Pivot::Left:
				origin = VECTOR3(-1.0f, 0.0f, 0.0f);
				offset = VECTOR2(-1.0f, 0.0f);
				break;
			case Pivot::Right:
				origin = VECTOR3(1.0f, 0.0f, 0.0f);
				offset = VECTOR2(1.0f, 0.0f);
				break;
		}
	}


}// namespace TKGEngine