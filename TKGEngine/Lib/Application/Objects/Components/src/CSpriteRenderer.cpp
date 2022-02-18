
#include "../inc/CSpriteRenderer.h"

#include "../inc/CTransform.h"
#include "../interface/ICamera.h"
#include "Application/Resource/inc/VertexBuffer.h"

#include "Utility/inc/myfunc_file.h"

#include <d3d11.h>

REGISTERCOMPONENT(TKGEngine::SpriteRenderer);

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	SpriteRenderer::SpriteRenderer()
		: Renderer(RendererType::Mesh)
	{
		/* nothing */
	}

	SpriteRenderer::~SpriteRenderer()
	{
		/* nothing */
	}

#ifdef USE_IMGUI
	void SpriteRenderer::OnGUI()
	{
		ImGui::IDWrapped id(this);

		// Renderer::OnGUI
		Renderer::OnGUI();

		//// SpriteRenderer
		// Billboard Type
		ImGui::Text("Billboard Type");
		ImGui::AlignedSameLine(0.5f);
		ImGui::ComboEnum<BillboardType, BillboardType::Max_BillboardType>("##Billboard Type", &m_billboard_type);
		// �e�N�X�`��
		constexpr ImGuiTreeNodeFlags tree_flags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
		if (ImGui::TreeNodeEx("Sprite Texture", tree_flags))
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

		// �}�e���A��
		ImGui::Separator();
		OnGUIMaterial();
	}
#endif // USE_IMGUI

	bool SpriteRenderer::SetTexture(const char* filepath, bool force_srgb)
	{
		m_texture.Load(filepath, force_srgb);
		if (!m_texture.HasTexture())
			return false;
		// �t�@�C�����[�h�\�ȃe�N�X�`���f�[�^������
		m_texture_filedata.Set(filepath);
		m_force_srgb = force_srgb;
		return true;
	}

	bool SpriteRenderer::SetTexture(const Texture& texture)
	{
		m_texture = texture;
		if (!m_texture.HasTexture())
			return false;
		// ���s���ɍ쐬���ꂽ�e�N�X�`���f�[�^�̂��߃f�V���A���C�Y���ɂ̓��[�h�s��
		m_texture_filedata.Clear();
		m_force_srgb = m_texture.GetForceSRGB();
		return true;
	}

	bool SpriteRenderer::HasTexture() const
	{
		return m_texture.HasTexture();
	}

	void SpriteRenderer::OnCreated()
	{
		// �}�e���A�����͏��1��
		ResizeMaterial(1);
		// �f�V���A���C�Y���Ƀe�N�X�`���f�[�^������΃��[�h����
		if (m_texture_filedata.HasData())
		{
			m_texture.Load(m_texture_filedata.filepath, m_force_srgb);
		}
	}

	void SpriteRenderer::Render(
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
		// �}�e���A���̗L�������`��ɏ\���łȂ��Ȃ�h���[�R�[�������Ȃ�
		if (!mat->Activate(p_context, write_depth, write_depth))
		{
			return;
		}

		// Quad�p�ݒ�A�e�N�X�`���Z�b�g
		p_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		m_texture.SetSRV(p_context, TEXSLOT_SPRITE, ShaderVisibility::PS);

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
			const int slot = mat->GetInputSlotFromType(static_cast<int>(VERTEX_ELEMENT_TYPE::MAIN_INSTANCE));
			if (slot > 0)
				instance_buffer.Set(p_context, slot);
		}

		p_context->DrawIndexedInstanced(4, instance_count, 0, 0, start_index);
	}

	void SpriteRenderer::RenderShadow(ID3D11DeviceContext* p_context, int index, int start_index, int instance_count, VertexBuffer& instance_buffer)
	{
		assert(p_context != nullptr);

		// Material
		assert(index == 0);
		const auto* mat = &m_shadow_materials.at(index);
		if (!mat->HasMaterial())
		{
			mat = &Material::GetDefaultMaterial();
		}
		// �}�e���A���̗L�������`��ɏ\���łȂ��Ȃ�h���[�R�[�������Ȃ�
		if (!mat->Activate(p_context, true, false))
		{
			return;
		}

		// Quad�p�ݒ�A�e�N�X�`���Z�b�g
		p_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		m_texture.SetSRV(p_context, TEXSLOT_SPRITE, ShaderVisibility::PS);

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
			const int slot = mat->GetInputSlotFromType(static_cast<int>(VERTEX_ELEMENT_TYPE::MAIN_INSTANCE));
			if (slot > 0)
				instance_buffer.Set(p_context, slot);
		}

		p_context->DrawIndexedInstanced(4, instance_count, 0, 0, start_index);
	}

	int SpriteRenderer::GetSubsetCount() const
	{
		return 1;
	}

	unsigned SpriteRenderer::GetHashMesh() const
	{
		return 0;
	}

	unsigned SpriteRenderer::GetHashMaterial(int index) const
	{
		assert(index == 0);

		return m_materials.at(index).GetHash();
	}

	void SpriteRenderer::CalculateRenderParameter(const std::shared_ptr<ICamera>& camera)
	{
		// Transform���Q��
		const auto s_transform = GetTransform();

		// TODO : �A�^�b�`���ꂽ�I�u�W�F�N�g�̉�]�ɂ���ăr���{�[�h������邱�Ƃ����邪�A��]�𗘗p�������ꍇ�����邽�߂�������e�F����

		// WorldPosition, WorldMatrix�̎擾
		m_current_world_position = s_transform->Position();
		m_current_transform_matrix = s_transform->GetLocalToWorldMatrix();

		const auto pos_mat = MATRIX::CreateTranslation(s_transform->Position());
		const auto sca_mat = MATRIX::CreateScale(s_transform->LossyScale());

		// Billboard�Ή�
		switch (m_billboard_type)
		{
		case BillboardType::None:
			/* nothing */
			break;
		case BillboardType::Vertical:
			m_current_transform_matrix = sca_mat * MATRIX::CreateBillboardVertical(camera->GetWorldToViewMatrix()) * pos_mat;
			break;
		case BillboardType::Horizontal:
			m_current_transform_matrix = sca_mat * MATRIX::CreateBillboardHorizontal(camera->GetWorldToViewMatrix()) * pos_mat;
			break;
		case BillboardType::View:
			m_current_transform_matrix = sca_mat * MATRIX::CreateBillboard(camera->GetWorldToViewMatrix()) * pos_mat;
			break;
		}

		// Bounds�̍X�V
		m_bounds = GetQuad().GetBounds()->Transform(m_current_transform_matrix);
	}

}// namespace TKGEngine