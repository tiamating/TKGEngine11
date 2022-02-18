
#include "../inc/CMeshRenderer.h"

#include "Application/Resource/inc/VertexBuffer.h"

#include <d3d11.h>

REGISTERCOMPONENT(TKGEngine::MeshRenderer);

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	MeshRenderer::MeshRenderer()
		: Renderer(RendererType::Mesh)
	{
		/* nothing */
	}

	MeshRenderer::~MeshRenderer()
	{
		/* nothing */
	}

#ifdef USE_IMGUI
	void MeshRenderer::OnGUI()
	{
		const ImGui::IDWrapped gui_id(this);

		// Renderer::OnGUI
		Renderer::OnGUI();
		ImGui::Separator();
		// Mesh
		OnGUIMesh();
		// マテリアル
		OnGUIMaterial();
	}
#endif // USE_IMGUI

	void MeshRenderer::Render(
		ID3D11DeviceContext* p_context,
		int index, int start_index, int instance_count,
		VertexBuffer& instance_buffer,
		const std::shared_ptr<ICamera>& camera,
		bool write_depth
	)
	{
		assert(p_context != nullptr);

		// Mesh
		if (!m_mesh.HasMesh())
			return;

		// Material
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

		const auto subset = m_mesh.GetSubset(index);

		// Set VBs and IB
		for (int i = 0; i <= static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD7); ++i)
		{
			const int slot = mat->GetInputSlotFromType(i);
			if (slot < 0) continue;

			m_mesh.ActivateVB(p_context, slot, i);
		}
		m_mesh.ActivateIB(p_context);

		// Set InstanceBuffer
		{
			const int slot = mat->GetInputSlotFromType(static_cast<int>(VERTEX_ELEMENT_TYPE::MAIN_INSTANCE));
			if (slot > 0)
				instance_buffer.Set(p_context, slot);
		}

		p_context->DrawIndexedInstanced(subset.second, instance_count, subset.first, 0, start_index);
	}

	void MeshRenderer::RenderShadow(ID3D11DeviceContext* p_context, int index, int start_index, int instance_count, VertexBuffer& instance_buffer)
	{
		assert(p_context != nullptr);

		// Mesh
		bool use_shadow_mesh = true;
		// Shadow MeshがなければMain Meshを使用する
		if (!m_shadow_mesh.HasMesh())
		{
			if(!m_mesh.HasMesh())
			{
				// どちらのMeshもないなら早期リターン
				return;
			}
			use_shadow_mesh = false;
		}
		Mesh& mesh = use_shadow_mesh ? m_shadow_mesh : m_mesh;

		// Material
		const auto* mat = &m_shadow_materials.at(index);
		if (!mat->HasMaterial())
		{
			mat = &Material::GetDefaultMaterial();
		}
		// マテリアルの有効化が描画に十分でないならドローしない
		if (!mat->Activate(p_context, true, false))
		{
			return;
		}

		const auto subset = mesh.GetSubset(index);

		// Set VBs and IB
		for (int i = 0; i <= static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD7); ++i)
		{
			const int slot = mat->GetInputSlotFromType(i);
			if (slot < 0) continue;

			mesh.ActivateVB(p_context, slot, i);
		}
		mesh.ActivateIB(p_context);

		// Set InstanceBuffer
		{
			const int slot = mat->GetInputSlotFromType(static_cast<int>(VERTEX_ELEMENT_TYPE::MAIN_INSTANCE));
			if (slot > 0)
				instance_buffer.Set(p_context, slot);
		}

		p_context->DrawIndexedInstanced(subset.second, instance_count, subset.first, 0, start_index);
	}

}// namespace TKGEngine