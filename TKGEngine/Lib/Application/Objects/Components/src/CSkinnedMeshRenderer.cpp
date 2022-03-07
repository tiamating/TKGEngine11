
#include "../inc/CSkinnedMeshRenderer.h"

#include "../../inc/IGameObject.h"
#include "../inc/CAnimator.h"

#include "Systems/inc/IGUI.h"
#include "Systems/inc/IGraphics.h"
#include "Application/Resource/inc/Mesh.h"
#include "Application/Resource/inc/VertexBuffer.h"

#include "Shader/Skinning/Skinning_Defined.h"

#include <functional>
#include <d3d11.h>

REGISTERCOMPONENT(TKGEngine::SkinnedMeshRenderer);

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Static member definition
	////////////////////////////////////////////////////////
	ComputeShader SkinnedMeshRenderer::m_skinning_shader;
	std::mutex SkinnedMeshRenderer::m_skinninged_mutex;


	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	SkinnedMeshRenderer::SkinnedMeshRenderer()
		: Renderer(RendererType::Skin)
	{
		/* nothing */
	}

	SkinnedMeshRenderer::~SkinnedMeshRenderer()
	{
		/* nothing */
	}

#ifdef USE_IMGUI
	void SkinnedMeshRenderer::OnGUI()
	{
		ImGui::PushID(this);

		// Renderer::OnGUI
		Renderer::OnGUI();
		ImGui::Separator();
		// Animator
		ImGui::Text("Animator Ref");
		ImGui::Indent(ImGui::INDENT_VALUE);
		{
			// Name
			ImGui::Text("Ref Name:");
			ImGui::SameLine();
			if (m_animator)
				ImGui::Text("%s", m_animator->GetName());
			else
				ImGui::Text("No Reference");
			// Button
			if (ImGui::Button("Set Animator"))
			{
				ImGui::OpenPopup("Select Animator Popup");
			}
			// Select window
			if (ImGui::BeginPopup("Select Animator Popup"))
			{
				// ���g���܂߂āA�e��T������Animator�������̂�\������
				std::function<void(const std::shared_ptr<Transform>&)> recurse = [&](const std::shared_ptr<Transform>& tr)
				{
					// parent�����݂���Ȃ�T�����p������
					const auto parent = tr->GetParent();
					if (parent)
					{
						recurse(parent);
					}
					// Animator�����Ȃ�\������
					const bool has_anim = tr->HasComponent<Animator>();
					if (has_anim)
					{
						const auto ref_anim = tr->GetComponent<Animator>();
						if (ImGui::Selectable(tr->GetName()))
						{
							m_animator = ref_anim;
						}
					}
				};

				recurse(GetTransform());

				ImGui::EndPopup();
			}
		}
		ImGui::Unindent(ImGui::INDENT_VALUE);
		ImGui::Separator();
		// Root Transform
		ImGui::Text("Root Transform Ref");
		ImGui::SameLine();
		ImGui::HelpMarker("Root Transform is used that calculate culling AABB.");
		ImGui::Indent(ImGui::INDENT_VALUE);
		{
			// Name
			ImGui::Text("Ref Name:");
			ImGui::SameLine();
			if (m_root_transform)
				ImGui::Text("%s", m_root_transform->GetName());
			else
				ImGui::Text("No Reference");
			// Animator�̎Q�Ƃ������Ȃ��Ȃ�Button�\�������Ȃ�
			if (m_animator)
			{
				// Button
				if (ImGui::Button("Set Root Transform"))
				{
					ImGui::OpenPopup("Select Transform Popup");
				}
				// Select window
				if (ImGui::BeginPopup("Select Transform Popup"))
				{
					// Animator��Avatar������Transform����I������
					const auto& bone_list = m_animator->GetBoneReferences();
					const int bone_cnt = bone_list.size();
					for (const auto& bone : bone_list)
					{
						const char* name = bone->GetName();
						if (ImGui::Selectable(name))
						{
							m_root_transform = bone;
						}
					}

					ImGui::EndPopup();
				}
			}
		}
		ImGui::Unindent(ImGui::INDENT_VALUE);
		ImGui::Separator();
		// Bounds
		ImGui::Text("Mesh Bounds");
		ImGui::SameLine();
		ImGui::HelpMarker("Set the AABB to be used when culling.");
		ImGui::Indent(ImGui::INDENT_VALUE);
		{
			// �ݒ肵��AABB���g�p���邩�A���true��Ԃ���
			ImGui::Text("Culling Always");
			ImGui::AlignedSameLine(0.5f);
			ImGui::Checkbox("##Culling Always through", &m_culling_always_through);
			if (!m_culling_always_through)
			{
				ImGui::Text("Extent");
				ImGui::AlignedSameLine(0.5f);
				{
					VECTOR3 extents = m_local_bounds.GetExtents();
					if (ImGui::DragFloat3("##bounds extent", &extents.x, 0.002f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp))
					{
						m_local_bounds.SetExtents(extents);
					}
				}
				ImGui::Text("Center");
				ImGui::AlignedSameLine(0.5f);
				{
					VECTOR3 center = m_local_bounds.GetCenter();
					if (ImGui::DragFloat3("##bounds center", &center.x, 0.002f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
					{
						m_local_bounds.SetCenter(center);
					}
				}
				// �͈͕\��
				const auto transform = GetTransform();
				if (transform)
				{
					const auto world_bounds = m_local_bounds.Transform(transform->GetLocalToWorldMatrix());
					IGUI::Get().DrawBox(world_bounds.GetCenter(), Quaternion::Identity, world_bounds.GetExtents(), VECTOR3::One);
				}
			}
		}
		ImGui::Unindent(ImGui::INDENT_VALUE);
		ImGui::Separator();
		// Mesh
		OnGUIMesh();
		// �}�e���A��
		OnGUIMaterial();

		ImGui::PopID();
	}
#endif // USE_IMGUI

	void SkinnedMeshRenderer::OnCreated()
	{
		// �X�L�j���O�p�R���s���[�g�V�F�[�_�̃��[�h
		if (!m_skinning_shader.HasResource())
		{
			m_skinning_shader.Load("./Shader/Skinning/Skinning_CS.cso");
			if (!m_skinning_shader.HasResource())
			{
				assert(0 && "failed load skinning shader. (SkinnedMeshRenderer::OnCreated())");
				return;
			}
		}
	}

	void SkinnedMeshRenderer::AddMesh(const std::string& filepath, bool set_global)
	{
		// Mesh��ǉ�
		Renderer::AddMesh(filepath, set_global);
		if (m_mesh.HasMesh())
		{
			// ���_�o�b�t�@���쐬
			CreateSkinningedVertexBuffer();
		}
	}

	void SkinnedMeshRenderer::AddMesh(const Mesh& mesh, bool set_global)
	{
		// Mesh��ǉ�
		Renderer::AddMesh(mesh, set_global);
		if (m_mesh.HasMesh())
		{
			// ���_�o�b�t�@���쐬
			CreateSkinningedVertexBuffer();
		}
	}

	void SkinnedMeshRenderer::AddAnimator(const std::shared_ptr<Animator>& animator)
	{
		m_animator = animator;
	}

	void SkinnedMeshRenderer::AddAnimator(const std::shared_ptr<IGameObject>& gameobject)
	{
		const auto s_ptr = gameobject->GetComponent<Animator>();
		if (!s_ptr)
			return;
		m_animator = s_ptr;
	}

	void SkinnedMeshRenderer::SetRootTransform(const std::shared_ptr<Transform>& transform)
	{
		m_root_transform = transform;
	}

	void SkinnedMeshRenderer::SetRootTransform(const std::shared_ptr<IGameObject>& gameobject)
	{
		m_root_transform = gameobject->GetTransform();
	}

	void SkinnedMeshRenderer::RemoveRootTransform()
	{
		m_root_transform.reset();
	}

	void SkinnedMeshRenderer::CalculateRenderParameter(const std::shared_ptr<ICamera>& camera)
	{
		const auto transform = GetTransform();

		// WorldPosition, WorldMatrix�̎擾
		m_current_world_position = transform->Position();
		m_current_transform_matrix = transform->GetLocalToWorldMatrix();

		// �A�j���[�V�����ɂ��ψʂ����邽�߃����_���[���ƂɓƎ���Bounds�������A���culling���Ȃ����I���ł���
		if (m_culling_always_through)
		{
			m_bounds = m_mesh.GetBounds()->Transform(m_current_transform_matrix);
		}
		else
		{
			m_bounds = m_local_bounds.Transform(m_current_transform_matrix);
		}

		// �t���[�����Ƃ�1�񂾂��t���O���Z�b�g
		// TODO : �X�L�j���O���J�����P�ʂŃ��Z�b�g���Ă���̂ŁA�t���[���P�ʂōs���悤�ɂ���
		m_is_skinninged_on_frame = false;
	}

	void SkinnedMeshRenderer::Render(
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
		auto& mat = m_materials.at(index);
		if (!mat.HasMaterial())
		{
			return;
		}
		// �}�e���A����񂪕`��ɏ\���łȂ��Ȃ�h���[�R�[�������Ȃ�
		if (!mat.Activate(p_context, write_depth, write_depth))
		{
			return;
		}

		const auto subset = m_mesh.GetSubset(index);

		// Set VBs and IB
		for (int i = 0; i <= static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD7); ++i)
		{
			const int slot = mat.GetInputSlotFromType(i);
			if (slot < 0) continue;

			// �ꕔ�̓X�L�j���O��̃o�b�t�@���Z�b�g����
			switch (static_cast<VERTEX_ELEMENT_TYPE>(i))
			{
				case VERTEX_ELEMENT_TYPE::POSITION:
				{
					m_skinninged_vs_position.Set(p_context, slot);
				}
				break;
				case VERTEX_ELEMENT_TYPE::NORMAL:
				{
					m_skinninged_vs_normal.Set(p_context, slot);
				}
				break;
				case VERTEX_ELEMENT_TYPE::TANGENT:
				{
					m_skinninged_vs_tangent.Set(p_context, slot);
				}
				break;

				// �c��̓��b�V���̎��o�b�t�@���Z�b�g
				default:
				{
					m_mesh.ActivateVB(p_context, slot, i);
				}
				break;
			}
		}
		m_mesh.ActivateIB(p_context);

		// Set InstanceBuffer
		{
			const int slot = mat.GetInputSlotFromType(static_cast<int>(VERTEX_ELEMENT_TYPE::MAIN_INSTANCE));
			if (slot > 0)
				instance_buffer.Set(p_context, slot);
		}

		// �X�L�j���O���s
		m_skinninged_mutex.lock();
		if (!m_is_skinninged_on_frame)
		{
			m_is_skinninged_on_frame = true;
			auto* compute_context = IGraphics::Get().DC(0, Graphics::DC_COMPUTE_PATH::DC_CP_MAIN);

			// �萔�o�b�t�@�F�A�j���[�V�����s��Z�b�g
			if (m_animator)
			{
				m_animator->SetCB(compute_context, CBS_MODEL, true);
			}
			else
			{
				// Animator�̎Q�Ƃ��Ȃ���΃f�t�H���g�|�[�Y
				Animator::SetDefaultCB(compute_context, CBS_MODEL, true);
			}
			// ���_�o�b�t�@�F�Z�b�g
			{
				// Input
				m_mesh.GetVertexBuffer(VERTEX_ELEMENT_TYPE::POSITION)->SetComputeInput(compute_context, SKINNINGSLOT_IN_VERTEX_POS);
				m_mesh.GetVertexBuffer(VERTEX_ELEMENT_TYPE::NORMAL)->SetComputeInput(compute_context, SKINNINGSLOT_IN_VERTEX_NOR);
				m_mesh.GetVertexBuffer(VERTEX_ELEMENT_TYPE::TANGENT)->SetComputeInput(compute_context, SKINNINGSLOT_IN_VERTEX_TAN);
				m_mesh.GetVertexBuffer(VERTEX_ELEMENT_TYPE::BONES)->SetComputeInput(compute_context, SKINNINGSLOT_IN_VERTEX_BON);
				m_mesh.GetVertexBuffer(VERTEX_ELEMENT_TYPE::WEIGHTS)->SetComputeInput(compute_context, SKINNINGSLOT_IN_VERTEX_WEI);
				// Output
				m_skinninged_vs_position.SetComputeOutput(compute_context, SKINNINGSLOT_OUT_VERTEX_POS);
				m_skinninged_vs_normal.SetComputeOutput(compute_context, SKINNINGSLOT_OUT_VERTEX_NOR);
				m_skinninged_vs_tangent.SetComputeOutput(compute_context, SKINNINGSLOT_OUT_VERTEX_TAN);
			}
			// �V�F�[�_�[�Z�b�g
			m_skinning_shader.Activate(compute_context);
			// �X�L�j���O���s
			const unsigned group_num = (m_mesh.GetVertexCount() + SKINNING_COMPUTE_THREADCOUNT - 1) / SKINNING_COMPUTE_THREADCOUNT;
			compute_context->Dispatch(group_num > 0 ? group_num : 1, 1, 1);
		}
		m_skinninged_mutex.unlock();

		p_context->DrawIndexedInstanced(subset.second, instance_count, subset.first, 0, start_index);
	}

	void SkinnedMeshRenderer::RenderShadow(ID3D11DeviceContext* p_context, int index, int start_index, int instance_count, VertexBuffer& instance_buffer)
	{
		assert(p_context != nullptr);

		// Mesh
		bool use_shadow_mesh = true;
		// Shadow Mesh���Ȃ����Main Mesh���g�p����
		if (!m_shadow_mesh.HasMesh())
		{
			if (!m_mesh.HasMesh())
			{
				// �ǂ����Mesh���Ȃ��Ȃ瑁�����^�[��
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
		// �}�e���A����񂪕`��ɏ\���łȂ��Ȃ�h���[�R�[�������Ȃ�
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

			// �ꕔ�̓X�L�j���O��̃o�b�t�@���Z�b�g����
			switch (static_cast<VERTEX_ELEMENT_TYPE>(i))
			{
				case VERTEX_ELEMENT_TYPE::POSITION:
				{
					m_skinninged_vs_position.Set(p_context, slot);
				}
				break;
				case VERTEX_ELEMENT_TYPE::NORMAL:
				{
					m_skinninged_vs_normal.Set(p_context, slot);
				}
				break;
				case VERTEX_ELEMENT_TYPE::TANGENT:
				{
					m_skinninged_vs_tangent.Set(p_context, slot);
				}
				break;

				// �c��̓��b�V���̎��o�b�t�@���Z�b�g
				default:
				{
					mesh.ActivateVB(p_context, slot, i);
				}
				break;
			}
		}
		mesh.ActivateIB(p_context);

		// Set InstanceBuffer
		{
			const int slot = mat->GetInputSlotFromType(static_cast<int>(VERTEX_ELEMENT_TYPE::MAIN_INSTANCE));
			if (slot > 0)
				instance_buffer.Set(p_context, slot);
		}

		// �X�L�j���O���s
		m_skinninged_mutex.lock();
		if (!m_is_skinninged_on_frame)
		{
			m_is_skinninged_on_frame = true;
			auto* compute_context = IGraphics::Get().DC(0, Graphics::DC_COMPUTE_PATH::DC_CP_MAIN);

			// �萔�o�b�t�@�F�A�j���[�V�����s��Z�b�g
			if (m_animator)
			{
				m_animator->SetCB(compute_context, CBS_MODEL, true);
			}
			else
			{
				// Animator�̎Q�Ƃ��Ȃ���΃f�t�H���g�|�[�Y
				Animator::SetDefaultCB(compute_context, CBS_MODEL, true);
			}
			// ���_�o�b�t�@�F�Z�b�g
			{
				// Input
				mesh.GetVertexBuffer(VERTEX_ELEMENT_TYPE::POSITION)->SetComputeInput(compute_context, SKINNINGSLOT_IN_VERTEX_POS);
				mesh.GetVertexBuffer(VERTEX_ELEMENT_TYPE::NORMAL)->SetComputeInput(compute_context, SKINNINGSLOT_IN_VERTEX_NOR);
				mesh.GetVertexBuffer(VERTEX_ELEMENT_TYPE::TANGENT)->SetComputeInput(compute_context, SKINNINGSLOT_IN_VERTEX_TAN);
				mesh.GetVertexBuffer(VERTEX_ELEMENT_TYPE::BONES)->SetComputeInput(compute_context, SKINNINGSLOT_IN_VERTEX_BON);
				mesh.GetVertexBuffer(VERTEX_ELEMENT_TYPE::WEIGHTS)->SetComputeInput(compute_context, SKINNINGSLOT_IN_VERTEX_WEI);
				// Output
				m_skinninged_vs_position.SetComputeOutput(compute_context, SKINNINGSLOT_OUT_VERTEX_POS);
				m_skinninged_vs_normal.SetComputeOutput(compute_context, SKINNINGSLOT_OUT_VERTEX_NOR);
				m_skinninged_vs_tangent.SetComputeOutput(compute_context, SKINNINGSLOT_OUT_VERTEX_TAN);
			}
			// �V�F�[�_�[�Z�b�g
			m_skinning_shader.Activate(compute_context);
			// �X�L�j���O���s
			const unsigned group_num = (mesh.GetVertexCount() + SKINNING_COMPUTE_THREADCOUNT - 1) / SKINNING_COMPUTE_THREADCOUNT;
			compute_context->Dispatch(group_num > 0 ? group_num : 1, 1, 1);
		}
		m_skinninged_mutex.unlock();

		p_context->DrawIndexedInstanced(subset.second, instance_count, subset.first, 0, start_index);
	}

	bool SkinnedMeshRenderer::CreateSkinningedVertexBuffer()
	{
		const int vertex_num = m_mesh.GetVertexCount();

		// Position
		if (m_skinninged_vs_position.Create(
			nullptr,
			VERTEX_ELEMENT::ELEMENT_SIZE[static_cast<int>(VERTEX_ELEMENT_TYPE::POSITION)],
			vertex_num,
			VERTEX_ELEMENT_TYPE::POSITION,
			BUFFER_HEAP_TYPE::BUFFER_HEAP_DEFAULT,
			true, true) == false)
		{
			assert(0 && "failed create skinninged_VB::POSITION SkinnedMeshRenderer::CreateSkinningedVertexBuffer()");
			return false;
		}
		// Normal
		if (m_skinninged_vs_normal.Create(
			nullptr,
			VERTEX_ELEMENT::ELEMENT_SIZE[static_cast<int>(VERTEX_ELEMENT_TYPE::NORMAL)],
			vertex_num,
			VERTEX_ELEMENT_TYPE::NORMAL,
			BUFFER_HEAP_TYPE::BUFFER_HEAP_DEFAULT,
			true, true) == false)
		{
			assert(0 && "failed create skinninged_VB::NORMAL SkinnedMeshRenderer::CreateSkinningedVertexBuffer()");
			return false;
		}
		// Tangent
		if (m_skinninged_vs_tangent.Create(
			nullptr,
			VERTEX_ELEMENT::ELEMENT_SIZE[static_cast<int>(VERTEX_ELEMENT_TYPE::TANGENT)],
			vertex_num,
			VERTEX_ELEMENT_TYPE::TANGENT,
			BUFFER_HEAP_TYPE::BUFFER_HEAP_DEFAULT,
			true, true) == false)
		{
			assert(0 && "failed create skinninged_VB::TANGENT SkinnedMeshRenderer::CreateSkinningedVertexBuffer()");
			return false;
		}

		return true;
	}


}// namespace TKGEngine