#include "PSSM.h"

#include "Components/interface/ICamera.h"
#include "Systems/inc/IGraphics.h"

#include "Application/Resource/inc/VertexBuffer.h"
#include "Systems/inc/IGUI.h"

// �V���h�E�}�b�v�̉𑜓x
static constexpr unsigned SHADOW_TEXTURE_SIZE[MAX_CASCADE] = { 2048, 1024, 512, 512 };


namespace TKGEngine
{
	PSSM::PSSM(LightType type)
		: ShadowMapBase(type)
	{
		// �X���b�h�̃T�C�Y�m��
		m_thread_result.reserve(MAX_CASCADE);
	}

#ifdef USE_IMGUI
	void PSSM::OnGUI()
	{
		constexpr ImGuiTreeNodeFlags tree_flags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
		if (ImGui::TreeNodeEx("PSSM", tree_flags))
		{
			// Test Accurate
			ImGui::Text("Test Accurate");
			ImGui::SameLine();
			ImGui::HelpMarker("Select test type, AABB vs Frustum or AABB vs Frustum's AABB.\nOn SweepIntersection.");
			ImGui::AlignedSameLine(0.5f);
			ImGui::Checkbox("##Test Accurate", &m_test_accurate);

			// Sampling���
			ImGui::Text("Shadow Sampling");
			{
				ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
				// Sampling Radius
				ImGui::Text("Radius");
				ImGui::AlignedSameLine(0.5f);
				ImGui::SliderFloat("##Sampling Radius", &m_sampling_radius_uv, 0.01f, 3.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
				// Sampling Count
				ImGui::Text("Count");
				ImGui::AlignedSameLine(0.5f);
				ImGui::SliderInt("##Sampling Count", &m_sampling_count, 1, 9, "(1 ~ 9) %d", ImGuiSliderFlags_AlwaysClamp);
			}
			ImGui::Separator();

			// Cascade count
			ImGui::Text("Cascade Count");
			ImGui::AlignedSameLine(0.5f);
			if (!m_on_gui_is_init)
			{
				// GUI�p�����[�^�̏�����
				m_on_gui_prev_cascade_num = m_cascade_num;
				m_on_gui_is_init = true;
			}
			ImGui::SliderInt("##Cascade count", &m_on_gui_prev_cascade_num, 1, MAX_CASCADE, "%d", ImGuiSliderFlags_AlwaysClamp);
			if (ImGui::IsItemDeactivated())
			{
				// �V�������������Z�b�g
				SetCascadeNum(m_on_gui_prev_cascade_num);
				// GUI�p�ɕ\�����镪�������X�V
				m_on_gui_prev_cascade_num = m_cascade_num;
			}
			// Split lambda
			ImGui::Text("Split Lambda");
			ImGui::AlignedSameLine(0.5f);
			ImGui::SliderFloat("##Split lambda", &m_lambda, 0.0f, 1.0f, "(0 ~ 1)%.3f", ImGuiSliderFlags_AlwaysClamp);
			// Split position
			ImGui::Text("Split Data");
			ImGui::Separator();
			{
				ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
				for (int i = 0; i < m_cascade_num; ++i)
				{
					ImGui::Text("[%d] : (Count.%d) %fm", i, m_casters[i].size(), m_split_positions[i]);
				}
			}
			ImGui::Separator();
			// Shadow Texture
			for(int i = 0;i< m_cascade_num;++i)
			{
				ImGui::Text("Cascade : %d", i);
				ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
				m_depth_targets[i]->OnGUI();
			}
			ImGui::Separator();
			ImGui::TreePop();
		}
	}
#endif // USE_IMGUI

	void PSSM::Create()
	{
		// Create Target
		{
			const auto device = IGraphics::Get().Device();

			TargetDesc desc(0, 0, DXGI_FORMAT_UNKNOWN, 1, 0);

			for (int i = 0; i < m_cascade_num; ++i)
			{
				// �e�N�X�`���T�C�Y���Z�b�g
				desc.width = desc.height = SHADOW_TEXTURE_SIZE[i];
				//// Color�̃t�H�[�}�b�g���Z�b�g
				//{
				//	desc.format = DXGI_FORMAT_R16G16_FLOAT;
				//	// Color�^�[�Q�b�g�̍쐬
				//	m_color_targets[i] = IColorTarget::CreateInterface();
				//	if (!m_color_targets[i]->Create(device, desc, true, true))
				//	{
				//		assert(0 && "failed IColorTarget::Create. PSSM::Create()");
				//		return;
				//	}
				//}
				// Depth�̃t�H�[�}�b�g���Z�b�g
				{
					desc.format = DXGI_FORMAT_D24_UNORM_S8_UINT;
					// Color�^�[�Q�b�g�̍쐬
					m_depth_targets[i] = IDepthTarget::CreateInterface();
					if (!m_depth_targets[i]->Create(device, desc, true))
					{
						assert(0 && "failed IDepthTarget::Create. PSSM::Create()");
						return;
					}
				}
			}
		}
		// Create CBuffer
		for (int i = 0; i < MAX_CASCADE; ++i)
		{
			if (!m_cb_LVPs[i].Create(sizeof(MATRIX), false))
			{
				assert(0 && "failed ConstantBuffer::Create. PSSM::Create()");
				return;
			}
		}
	}

	int PSSM::CalculateShadowCaster(const std::vector<MainData>& scene_objects, const int object_num, const std::shared_ptr<ICamera>& camera, const LightShadowData& data)
	{
		// �e�v�Z�p�N���X���Ȃ���Α������^�[��
		if (m_light_shadow_calculator == nullptr)
		{
			// �V���h�E�}�b�v��`�悵�Ȃ�
			for (auto& is_draw : m_draw_shadow_map)
			{
				is_draw = false;
			}
			return 0;
		}

		// �X�V�O����
		for (int i = 0; i < m_cascade_num; ++i)
		{
			auto& caster = m_casters[i];

			// �O�t���[���̏���ێ�
			m_is_rendered_target[i] = caster.empty() ? false : true;
			// �I�u�W�F�N�g���X�g�̃N���A
			caster.clear();
			// ���X�g�T�C�Y���I�u�W�F�N�g�̍ő吔�ɍ��킹��
			caster.reserve(object_num);
		}

		// ���C�g�r���[�s��̍쐬
		const MATRIX light_view = CalculateLightView(data.light_direction, data.light_up, VECTOR3::Zero);
		// �����ʒu�̌v�Z (�J�����̋ߕ��ʂ���e��`�悷�鉓���ʂ܂ł𕪊�����)
		CalculateSplitPosition(camera->GetNear(), data.far_plane);
		// ������̃t���X�^�����Ƃɕ`�悳���I�u�W�F�N�g���X�g�̍쐬��SplitViewProjection�̌v�Z
		{
			const auto calculate_func =
				[&](int index, const MATRIX& LV, const std::vector<MainData>& objs, int num, const std::shared_ptr<ICamera>& cam, const LightShadowData& d)
			{
				CalculateSplitLVP(index, LV, objs, num, cam, d);
			};
			for (int i = 0; i < m_cascade_num; ++i)
			{
				m_thread_result.emplace_back(m_calculate_split_LVP_thread.Add(calculate_func, i, light_view, scene_objects, object_num, camera, data));
			}
			// �I����ҋ@
			for (const auto& result : m_thread_result)
			{
				if (!result.valid())
					continue;
				result.wait();
			}
			m_thread_result.clear();
		}
		// LVP��CBuffer���X�V
		for (int i = 0; i < MAX_CASCADE; ++i)
		{
			m_cb_LVPs[i].SetParam(&m_split_view_projection_matrices[i], sizeof(MATRIX));
		}

		// �`��C���X�^���X�����J�E���g
		int instance_count = 0;
		// �e��`�悷��I�u�W�F�N�g�����݂��邩�`�F�b�N
		for (int i = 0; i < m_cascade_num; ++i)
		{
			if (m_casters[i].empty())
			{
				m_draw_shadow_map[i] = false;
			}
			else
			{
				m_draw_shadow_map[i] = true;
				instance_count += static_cast<int>(m_casters[i].size());
			}
		}
		return instance_count;
	}

	void PSSM::RenderShadow(ID3D11DeviceContext* context, VertexBuffer& instance_buffer)
	{
		// RTV�����Z�b�g����
		{
			ID3D11RenderTargetView* rtv = nullptr;
			context->OMSetRenderTargets(1, &rtv, nullptr);
		}
		// �O�t���[���̏����폜
		ClearFrameData(context);
		// �C���X�^���X�o�b�t�@�ɒl���l�߂�
		{
			// Map
			MainInstance* instance = nullptr;
			if (!instance_buffer.Map(context, reinterpret_cast<void**>(&instance)))
			{
				assert(0 && "failed InstanceBuffer::Map(). PSSM::RenderShadow()");
				return;
			}
			// Caster�����Ɏ擾���ăC���X�^���X�����l�߂�
			for (const auto& casters : m_casters)
			{
				for (const auto& data : casters)
				{
					data.renderer->SetInstance(instance++);
				}
			}
			// Unmap
			instance_buffer.Unmap(context);
		}
		// �V���h�E�}�b�v�̕`��
		{
			// �eCascade���Ƃ�LVP��RTV���Z�b�g���ĕ`��
			int instance_count = 0;
			ID3D11RenderTargetView* rtv = nullptr;
			for (int i = 0; i < MAX_CASCADE; ++i)
			{
				const auto& casters = m_casters[i];
				// �`�悷�邩�`�F�b�N
				if (casters.size() == 0)
					continue;
				// ViewPort�̃Z�b�g
				const auto& target_desc = m_depth_targets[i]->GetDesc();
				IGraphics::Get().SetViewPort(context, static_cast<float>(target_desc.width), static_cast<float>(target_desc.height));
				// LVP�̃Z�b�g
				m_cb_LVPs[i].SetVS(context, CBS_VP);
				m_cb_LVPs[i].SetPS(context, CBS_VP);
				// RTV�̃Z�b�g
				context->OMSetRenderTargets(1, &rtv, m_depth_targets[i]->GetDSV());
				// ����Caster��`��
				for (const auto& data : casters)
				{
					data.renderer->RenderShadow(context, data.subset_idx, instance_count++, 1, instance_buffer);
				}
			}
		}
		// RTV�����Z�b�g����
		{
			ID3D11RenderTargetView* rtv = nullptr;
			context->OMSetRenderTargets(1, &rtv, nullptr);
		}
	}

	void PSSM::SetSRV(ID3D11DeviceContext* context, const int slot, const int index) const
	{
		if (index < 0 || index >= m_cascade_num)
			return;
		m_depth_targets[index]->SetSRV(context, slot, ShaderVisibility::PS);
	}

	const MATRIX& PSSM::GetLVP(const int index) const
	{
		if (index < 0 || index >= m_cascade_num)
			return MATRIX::Identity;
		return m_split_view_projection_matrices[index];
	}

	const MATRIX& PSSM::GetLV(const int index) const
	{
		if (index < 0 || index >= m_cascade_num)
			return MATRIX::Identity;
		return m_split_view_matrices[index];
	}

	const MATRIX& PSSM::GetLP(const int index) const
	{
		if (index < 0 || index >= m_cascade_num)
			return MATRIX::Identity;
		return m_split_projection_matrices[index];
	}

	float PSSM::GetShadowSize(const int index) const
	{
		if (index < 0 || index >= m_cascade_num)
			return 1.0f;
		return static_cast<float>(m_depth_targets[index]->GetDesc().width);
	}

	unsigned PSSM::GetCascadeNum() const
	{
		return m_cascade_num;
	}

	bool PSSM::IsDrawShadowMap() const
	{
		for (const auto is_draw : m_draw_shadow_map)
		{
			if (is_draw)
				return true;
		}
		return false;
	}

	bool PSSM::IsDrawShadowMap(const int index) const
	{
		return m_draw_shadow_map[index];
	}

	float PSSM::GetSamplingRadius() const
	{
		return m_sampling_radius_uv;
	}

	int PSSM::GetSamplingCount() const
	{
		return m_sampling_count;
	}

	void PSSM::SetCascadeNum(const int num)
	{
		// �l���ύX���ꂽ��V���h�E�}�b�v�̍쐬����ύX����K�v������
		if (m_cascade_num != num)
		{
			// ���݂̃V���h�E�}�b�v��j��
			for (int i = 0; i < m_cascade_num; ++i)
			{
				//m_color_targets[i]->Release();
				m_depth_targets[i]->Release();
			}
			// �쐬�����X�V
			m_cascade_num = num;
			// �͈͊O�̑O�t���[���̏�������
			for (int i = m_cascade_num; i < MAX_CASCADE; ++i)
			{
				m_casters[i].clear();
			}
			// ���Ȃ���
			Create();
		}
	}

	void PSSM::ClearFrameData(ID3D11DeviceContext* context)
	{
		for (int i = 0; i < m_cascade_num; ++i)
		{
			// �^�[�Q�b�g�ɕ`�悪�Ȃ���΃N���A���Ȃ�
			if (!m_is_rendered_target[i])
				continue;
			// �^�[�Q�b�g�̃N���A
			//m_color_targets[i]->Clear(context);
			m_depth_targets[i]->Clear(context);
		}
	}

	MATRIX PSSM::CalculateLightView(const VECTOR3& light_dir, const VECTOR3& light_up, const VECTOR3& light_pos)
	{
		return MATRIX::CreateLookTo(light_pos, light_dir, light_up);
	}

	void PSSM::CalculateSplitPosition(const float near_plane, const float far_plane)
	{
		for (int i = 0; i < m_cascade_num; ++i)
		{
			// CLi = n*(f/n)^(i/numsplits)
			// CUi = n + (f-n)*(i/numsplits)
			// Ci = CLi*(lambda) + CUi*(1-lambda)
			const float i_div_m = i / static_cast<float>(m_cascade_num);
			const float c_log = near_plane * MyMath::Pow(far_plane / near_plane, i_div_m);
			const float c_uni = near_plane + (far_plane - near_plane) * i_div_m;
			m_split_positions[i] = c_log * m_lambda + c_uni * (1.0f - m_lambda);
		}
		// �ߕ��ʂƉ����ʂ͌Œ�
		m_split_positions[0] = near_plane;
		m_split_positions[m_cascade_num] = far_plane;
	}

	Bounds PSSM::CalculateCasterAABBInLightViewSpace(const std::vector<MainData>& casters, const Frustum& frustum, const MATRIX& light_view)
	{
		Bounds casters_aabb;

		// �e�𗎂Ƃ��I�u�W�F�N�g�����݂��Ȃ��Ȃ�SplitFrustum������AABB���g�p����
		if (casters.empty())
		{
			const Bounds frustum_aabb = frustum.GetAABB();
			casters_aabb = frustum_aabb.Transform(light_view);
		}
		// ���C�g�r���[��Ԃɕϊ�����AABB����������
		else
		{
			// ���C�g�r���[��Ԃɂ�����Caster��AABB�̍���
			casters_aabb = casters.at(0).renderer->GetRendererBounds().Transform(light_view);
			const size_t caster_num = casters.size();
			for (size_t i = 1; i < caster_num; ++i)
			{
				casters_aabb.Union(casters.at(i).renderer->GetRendererBounds().Transform(light_view));
			}
		}

		return casters_aabb;
	}

	MATRIX PSSM::CalculateLightProjectionMatrix(const Bounds& caster_aabb, const LightShadowData& data)
	{
		const VECTOR3 center = caster_aabb.GetCenter();
		const VECTOR3 half_size = caster_aabb.GetExtents();

		// ���C�g�r���[��Ԃł̍ő�A�ŏ�
		const VECTOR3 min = center - half_size;
		const VECTOR3 max = center + half_size;

		// near,far(�̖@���ƃ��C�g��������v�����max - min��0�Ȃ̂ŕ␳����)
		const float near_plane = data.near_plane;
		float far_plane = half_size.z * 2.0f + near_plane;
		if (far_plane - near_plane < near_plane)
		{
			far_plane += near_plane;
		}

		// ���C�g�v���W�F�N�V�����s����쐬(�̖@���ƃ��C�g���������������max - min��0�Ȃ̂ŕ␳����)
		const float width = half_size.x * 2.0f + 1.0f;
		const float height = half_size.y * 2.0f + 1.0f;
		const MATRIX light_proj = MATRIX::CreateOrthographic(width, height, near_plane, far_plane);
		// ���C�g�r���[�v���W�F�N�V�����s���Ԃ�
		return light_proj;
	}

	void PSSM::FixLightViewProjectionMatrix(const Frustum& split_frustum, const std::vector<MainData>& casters, MATRIX& view, MATRIX& proj, MATRIX& view_proj)
	{
		const MATRIX LVP = view * proj;

		// SplitFrustum��LVP�ϊ�����AABB�����߂�
		Bounds split_aabb = split_frustum.GetAABB();
		split_aabb = split_aabb.Transform(LVP);

		// Caster���Ȃ��Ȃ�SplitFrustum���g�p����
		if (casters.size() == 0)
		{
			AdjustCropAABB(split_aabb, view, proj, view_proj);
			return;
		}
		// Caster��LVP�ϊ�����AABB�����߂�
		Bounds caster_aabb = casters.at(0).renderer->GetRendererBounds().Transform(LVP);
		const size_t caster_num = casters.size();
		for (size_t i = 1; i < caster_num; ++i)
		{
			caster_aabb.Union(casters.at(i).renderer->GetRendererBounds().Transform(LVP));
		}
		// CasterAABB��X,Y��0�̎��͗]�������
		{
			constexpr float EXTENT_EPSILON = 0.1f;
			if (caster_aabb.GetExtents().x < EXTENT_EPSILON)
			{
				caster_aabb.SetExtents(caster_aabb.GetExtents() + VECTOR3(EXTENT_EPSILON, 0.0f, 0.0f));
			}
			if (caster_aabb.GetExtents().y < EXTENT_EPSILON)
			{
				caster_aabb.SetExtents(caster_aabb.GetExtents() + VECTOR3(0.0f, EXTENT_EPSILON, 0.0f));
			}
		}
		// casterAABB��splitAABB����œK��AABB���v�Z����
		VECTOR3 crop_min, crop_max;
		VECTOR3 split_min, split_max;
		{
			const VECTOR3 center = split_aabb.GetCenter();
			const VECTOR3 half_size = split_aabb.GetExtents();
			split_min = center - half_size;
			split_max = center + half_size;
		}
		VECTOR3 caster_min, caster_max;
		{
			const VECTOR3 center = caster_aabb.GetCenter();
			const VECTOR3 half_size = caster_aabb.GetExtents();
			caster_min = center - half_size;
			caster_max = center + half_size;
		}
		crop_min.x = MyMath::Max(split_min.x, caster_min.x);
		crop_max.x = MyMath::Min(split_max.x, caster_max.x);
		crop_min.y = MyMath::Max(split_min.y, caster_min.y);
		crop_max.y = MyMath::Min(split_max.y, caster_max.y);
		crop_min.z = caster_min.z;
		crop_max.z = split_max.z;
		Bounds crop_aabb;
		crop_aabb.CreateFromPoints(crop_min, crop_max);

		AdjustCropAABB(crop_aabb, view, proj, view_proj);
	}

	void PSSM::AdjustCropAABB(const Bounds& crop_aabb, MATRIX& view, MATRIX& proj, MATRIX& view_proj)
	{
		float scale_x, scale_y, scale_z;
		float offset_x, offset_y, offset_z;

		const VECTOR3 center = crop_aabb.GetCenter();
		const VECTOR3 half_size = crop_aabb.GetExtents();
		// AABB�̍ő�A�ŏ�
		const VECTOR3 min = center - half_size;
		const VECTOR3 max = center + half_size;

		// X,Y��[-1, 1]�ɃX�P�[�����O����
		scale_x = 1.0f / (half_size.x);
		scale_y = 1.0f / (half_size.y);
		// (0, 0)�𒆐S�Ɉړ�����
		offset_x = -0.5f * (max.x + min.x) * scale_x;
		offset_y = -0.5f * (max.y + min.y) * scale_y;
		// Z��[0, 1]�ɃX�P�[�����O�A�[���ʒu��␳����
		scale_z = 0.5f / (half_size.z);
		offset_z = -min.z * scale_z;

		// Crop�s��
		const MATRIX crop_matrix(
			scale_x, 0.0f, 0.0f, 0.0f,
			0.0f, scale_y, 0.0f, 0.0f,
			0.0f, 0.0f, scale_z, 0.0f,
			offset_x, offset_y, offset_z, 1.0f
		);
		// �I�t�Z�b�g�s��
		const MATRIX offset_matrix(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			-offset_x, -offset_y, -offset_z, 1.0f
		);

		//view_proj = view * proj * crop_matrix;

		// M = P * Crop * Offset * Inv_Crop * Inv_P
		const MATRIX P_Crop = proj * crop_matrix;
		const MATRIX M = P_Crop * offset_matrix * crop_matrix.Invert() * proj.Invert();
		// V * P * Crop = V * Inv_M * M * P * Crop
		// V = V * Inv_M
		view *= M.Invert();
		// P = M * P * Crop
		proj = M * P_Crop;
		view_proj = view * proj;
	}

	void PSSM::CalculateCastersWithLVP(const std::vector<MainData>& scene_objects, const int object_num, std::vector<MainData>& casters, MATRIX& view, MATRIX& proj, MATRIX& LVP)
	{
		// ���݂̃L���X�^�[���N���A
		casters.clear();
		
		// �V�[�����̃I�u�W�F�N�g��S�ă`�F�b�N����
		bool need_fix = false;
		float crop_min_z = 0.0f;
		for (int i = 0; i < object_num; ++i)
		{
			const auto& obj = scene_objects.at(i);

			// nullptr�`�F�b�N
			if (!obj.renderer)
				continue;

			// �I�u�W�F�N�g��AABB��LVP�ŕϊ�����
			const Bounds obj_aabb = obj.renderer->GetRendererBounds().Transform(LVP);
			// �N���b�s���O��Ԃ͈͓̔��`�F�b�N
			const VECTOR3 center = obj_aabb.GetCenter();
			const VECTOR3 half_size = obj_aabb.GetExtents();
			const VECTOR3 min = center - half_size;
			const VECTOR3 max = center + half_size;
			// X : ( -1.0, +1.0)
			if (max.x < -1.0f || min.x > 1.0f)
				continue;
			// Y : ( -1.0, +1.0)
			if (max.y < -1.0f || min.y > 1.0f)
				continue;
			// Z : ( 0.0, 1.0)
			if (min.z > 1.0f)
				continue;
			// Z��0.0��菬�����ꍇ�͍s����C������K�v������
			if (min.z < 0.0f)
			{
				need_fix = true;
				crop_min_z = MyMath::Min(crop_min_z, min.z);
			}
			// �͈͓��Ȃ̂ŃL���X�^�[�ɒǉ�
			casters.emplace_back(obj);
		}
		// �s����C������K�v������Ȃ�C��
		if(need_fix)
		{
			Bounds crop_aabb;
			crop_aabb.CreateFromPoints(VECTOR3::One, VECTOR3(-1.0f, -1.0f, crop_min_z));

			AdjustCropAABB(crop_aabb, view, proj, LVP);
		}
	}

	void PSSM::CalculateSplitLVP(const int index, const MATRIX& light_view, const std::vector<MainData>& scene_objects, const int object_num, const std::shared_ptr<ICamera>& camera, const LightShadowData& data)
	{
		// ������̃t���X�^��
		const Frustum split_frustum = camera->GetFrustum(m_split_positions[index], m_split_positions[index + 1]);
		// ������̃t���X�^���ɉe�𗎂Ƃ��I�u�W�F�N�g���X�g��T��
		m_light_shadow_calculator->FindCasters(scene_objects, object_num, camera, data, split_frustum, m_test_accurate, m_casters[index]);
		// ���C�g�r���[��Ԃł̃L���X�^�[AABB���v�Z����
		const Bounds caster_aabb = CalculateCasterAABBInLightViewSpace(m_casters[index], split_frustum, light_view);
		// ���C�g�r���[�v���W�F�N�V�������v�Z����
		m_split_view_matrices[index] = light_view;
		m_split_projection_matrices[index] = CalculateLightProjectionMatrix(caster_aabb, data);
		// �N���b�v�s����g���ă��C�g�r���[�v���W�F�N�V�����s����C������
		FixLightViewProjectionMatrix(split_frustum, m_casters[index], m_split_view_matrices[index], m_split_projection_matrices[index], m_split_view_projection_matrices[index]);
		// �L���X�^�[����LVP���쐬���Ă���ꍇ
		if (!m_casters[index].empty())
		{
			// �ŏI�I��LVP�ŃL���X�^�[��T�����ĕK�v�������view,projection���C������
			CalculateCastersWithLVP(scene_objects, object_num, m_casters[index], m_split_view_matrices[index], m_split_projection_matrices[index], m_split_view_projection_matrices[index]);
		}
		// Reversed-Z��K�p����
		m_split_projection_matrices[index] *= MATRIX::Reversed_Z;
		m_split_view_projection_matrices[index] *= MATRIX::Reversed_Z;
	}

}
