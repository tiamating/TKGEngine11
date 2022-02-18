#include "../inc/CCamera.h"

#include "../inc/CTransform.h"
#include "Managers/CameraManager.h"
#include "Managers/SceneManager.h"
#include "Application/Resource/inc/Shader_Defined.h"
#include "Utility/inc/myfunc_math.h"

#include "Systems/inc/Graphics_Defined.h"
#include "Systems/inc/IGraphics.h"
#include "Systems/inc/IWindow.h"

#include <DirectXMath.h>

REGISTERCOMPONENT(TKGEngine::Camera);

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	Camera::Camera()
		: Behaviour(COMPONENT_TYPE::COMPONENT_GENERAL)
	{
		/* nothing */
	}

	Camera::~Camera()
	{
		/* nothing */
	}

#ifdef USE_IMGUI
	void Camera::OnGUI()
	{
		ImGui::PushID(this);
		// Behaviour::OnGUIの実行
		Behaviour::OnGUI();

		// カリングレイヤーの選択
		OnGUICullingLayer();
		// 投影
		ImGui::Text("Is Perspective");
		ImGui::AlignedSameLine(0.5f);
		ImGui::Checkbox("##Is Perspective", &m_is_perspective);
		if (m_is_perspective)
		{
			// FOV
			ImGui::Text("FOV");
			ImGui::AlignedSameLine(0.5f);
			ImGui::SliderFloat("##FOV", &m_fov_y, 1.0f, 179.0f, "(1 = 179) %.0f(deg)", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SameLine();
			ImGui::HelpMarker("FOV axis is Vertical(Y axis).");
		}
		else
		{
			// 平衡投影の範囲
			ImGui::Text("Orthographic");
			ImGui::Indent(ImGui::INDENT_VALUE);
			ImGui::Text("Width");
			ImGui::AlignedSameLine(0.5f);
			ImGui::DragFloat("##Ortho Width", &m_projection_width, 0.4f, 1.0f, FLT_MAX, "%.0f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::Text("Height");
			ImGui::AlignedSameLine(0.5f);
			ImGui::DragFloat("##Ortho Height", &m_projection_height, 0.4f, 1.0f, FLT_MAX, "%.0f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::Unindent(ImGui::INDENT_VALUE);
		}
		// クリッピング範囲
		ImGui::Text("Clipping Plane");
		ImGui::Indent(ImGui::INDENT_VALUE);
		ImGui::Text("Near");
		ImGui::AlignedSameLine(0.5f);
		ImGui::DragFloat("##Near", &m_near_clip_plane, 0.05f, 0.001f, m_far_clip_plane - 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::Text("Far");
		ImGui::AlignedSameLine(0.5f);
		ImGui::DragFloat("##Far", &m_far_clip_plane, 0.5f, m_near_clip_plane + 1.0f, 50000.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::Unindent(ImGui::INDENT_VALUE);
		// 縦横の倍率
		ImGui::Text("View scale");
		ImGui::Indent(ImGui::INDENT_VALUE);
		ImGui::Text("X");
		ImGui::AlignedSameLine(0.5f);
		ImGui::DragFloat("##X scale", &m_vp_width_ratio, 0.005f, 0.01f, 1.0f, "(0.01 ~ 1) %.3f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::Text("Y");
		ImGui::AlignedSameLine(0.5f);
		ImGui::DragFloat("##Y scale", &m_vp_height_ratio, 0.005f, 0.01f, 1.0f, "(0.01 ~ 1) %.3f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::Unindent(ImGui::INDENT_VALUE);
		// ビューポートの左上座標
		ImGui::Text("View Port LeftTop");
		ImGui::Indent(ImGui::INDENT_VALUE);
		ImGui::Text("X");
		ImGui::AlignedSameLine(0.5f);
		ImGui::DragFloat("##X LeftTop", &m_top_left_x, 0.005f, 0.0f, 1.0f, "(0 ~ 1) %.2f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::Text("Y");
		ImGui::AlignedSameLine(0.5f);
		ImGui::DragFloat("##Y LeftTop", &m_top_left_y, 0.005f, 0.0f, 1.0f, "(0 ~ 1) %.2f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::Unindent(ImGui::INDENT_VALUE);
		// ソートに使用するデプス値
		ImGui::Text("Depth");
		ImGui::AlignedSameLine(0.5f);
		ImGui::DragFloat("##Depth", &m_depth, 0.01f, 0.0f, 0.0f, "%0.1f");

		ImGui::PopID();
	}

	void Camera::OnGUICullingLayer()
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
			if (ImGui::Begin("Select Camera culling Layer##Camera", &m_is_opened_culling_layer_window, flags))
			{
				constexpr ImGuiTreeNodeFlags tree_flags =
					ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
				constexpr ImGuiWindowFlags child_flags = ImGuiWindowFlags_HorizontalScrollbar;
				ImGui::Indent(window_size.x * 0.05f);
				ImGui::BeginChild("Child Window", ImVec2(window_size.x * 0.85f, window_size.y - ImGui::GetCursorPosY() - ImGui::ENABLE_SIZE_FROM_BOTTOM), true, child_flags);
				{
					constexpr int layer_num = static_cast<int>(Layer::MAX_USER_LAYER_NUM);
					// 全てのレイヤーを表示
					for (int i = 0; i < layer_num; ++i)
					{
						const int current_mask = (1 << i);

						// チェックと名前のクリックに対応
						ImGui::PushID(i);
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
						ImGui::PopID();
					}
				}
				ImGui::EndChild();
				ImGui::Unindent(window_size.x * 0.05f);
			}
			ImGui::End();
		}
	}
#endif // USE_IMGUI

	void Camera::OnCreate(const std::shared_ptr<Object>& p_self)
	{
		// Set CameraManager
		{
			const std::shared_ptr<Camera> camera = std::static_pointer_cast<Camera>(p_self);
			m_camera_id = CameraManager::RegisterManager(camera);
		}

		if (!CreateTargetsAndTexture())
		{
			assert(0 && "failed Camera::CreateTargets() Camera::OnCreate()");
			return;
		}
	}

	void Camera::OnDestroy()
	{
		CameraManager::UnregisterManager(m_camera_id);
	}

	bool Camera::CreateTargetsAndTexture()
	{
		// Set viewport
		m_window_width = IWindow::Get().GetScreenWidthf();
		m_window_height = IWindow::Get().GetScreenHeightf();

		// Create Target
		// https://zerogram.info/blog/?p=1070
		// https://www.4gamer.net/games/110/G011050/20110401056/
		{
			const auto device = IGraphics::Get().Device();

			// TODO : HDR非対応版
			TargetDesc desc(
				static_cast<UINT>(m_window_width),
				static_cast<UINT>(m_window_height),
				Graphics::g_color_format,
				m_msaa_count,
				m_msaa_quarity);
			m_main_target = IColorTarget::CreateInterface();
			if (!m_main_target->Create(device, desc, true, true))
			{
				assert(0 && "failed IColorTarget::Create(Main) Camera::OnCreate()");
				return false;
			}

			desc.format = DXGI_FORMAT_R10G10B10A2_UNORM;
			m_normal_target = IColorTarget::CreateInterface();
			if (!m_normal_target->Create(device, desc, true, true))
			{
				assert(0 && "failed IColorTarget::Create(Normal) Camera::OnCreate()");
				return false;
			}

			desc.format = DXGI_FORMAT_R10G10B10A2_UNORM;
			m_diffuse_target = IColorTarget::CreateInterface();
			if (!m_diffuse_target->Create(device, desc, true, true))
			{
				assert(0 && "failed IColorTarget::Create(Diffuse) Camera::OnCreate()");
				return false;
			}

			desc.format = DXGI_FORMAT_R10G10B10A2_UNORM;
			m_specular_target = IColorTarget::CreateInterface();
			if (!m_specular_target->Create(device, desc, true, true))
			{
				assert(0 && "failed IColorTarget::Create(Specular) Camera::OnCreate()");
				return false;
			}

			desc.format = DXGI_FORMAT_R8_UINT;
			m_roughness_target = IColorTarget::CreateInterface();
			if (!m_roughness_target->Create(device, desc, true, true))
			{
				assert(0 && "failed IColorTarget::Create(Roughness) Camera::OnCreate()");
				return false;
			}

			desc.format = Graphics::g_depth_format;
			m_depth_target = IDepthTarget::CreateInterface();
			if (!m_depth_target->Create(device, desc, true))
			{
				assert(0 && "failed IColorTarget::Create(Depth) Camera::OnCreate()");
				return false;
			}
		}

		// Create Copy Texture
		{
			TEX_DESC desc;
			desc.width = static_cast<int>(m_window_width);
			desc.height = static_cast<int>(m_window_height);
			desc.depth = 1;
			desc.array_size = 1;
			desc.mip_levels = 1;
			desc.format = Graphics::g_color_format;
			desc.option = TEXTURE_OPTION::TEXTURE_OPTION_NONE;
			desc.dimension = TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE2D;
			m_copy_main_target_texture.Create(desc, true, false, nullptr);

			desc.format = Graphics::g_depth_format;
			m_copy_depth_target_texture.Create(desc, true, false, nullptr);
		}

		// Create CBuffer
		{
			if (!m_cb_camera.Create(sizeof(CB_Camera), false))
			{
				assert(0 && "failed ConstantBuffer::Create(CB_Camera) Camera::OnCreate()");
				return false;
			}

			if (!m_cb_VP.Create(sizeof(MATRIX), false))
			{
				assert(0 && "failed ConstantBuffer::Create(CB_VP) Camera::OnCreate()");
				return false;
			}
		}

		return true;
	}

	void Camera::ReleaseTargets()
	{
		m_main_target.reset();
		m_normal_target.reset();
		m_diffuse_target.reset();
		m_specular_target.reset();
		m_roughness_target.reset();
		m_depth_target.reset();
	}

	bool Camera::ResizeBuffers()
	{
		ReleaseTargets();
		return CreateTargetsAndTexture();
	}

	void Camera::SetCBCamera(ID3D11DeviceContext* p_context)
	{
		m_cb_camera.SetVS(p_context, CBS_CAMERA);
		m_cb_camera.SetGS(p_context, CBS_CAMERA);
		m_cb_camera.SetPS(p_context, CBS_CAMERA);
	}

	void Camera::SetCBViewProjection(ID3D11DeviceContext* p_context)
	{
		m_cb_VP.SetVS(p_context, CBS_VP);
		m_cb_VP.SetGS(p_context, CBS_VP);
		m_cb_VP.SetPS(p_context, CBS_VP);
	}

	VECTOR2 Camera::WorldToViewport(const VECTOR3& point) const
	{
		const VECTOR4 vec(point, 1.0f);

		// World --> NDC
		VECTOR4 vp_vec = vec * m_view_matrix * m_reversed_projection_matrix;
		vp_vec /= vp_vec.w;

		return VECTOR2(vp_vec.x, vp_vec.y);
	}

	VECTOR3 Camera::ProjectionToWorld(const VECTOR3& point) const
	{
		VECTOR4 vec(point, 1.0f);
		// Reversed Z対策
		vec.z = 1.0f - vec.z;

		// NDC --> World
		const MATRIX IVP = (m_view_matrix * m_reversed_projection_matrix).Invert();
		const VECTOR4 world_vec = vec * IVP;

		return VECTOR3(world_vec);
	}

	void Camera::SetViewport(ID3D11DeviceContext* p_context)
	{
		IGraphics::Get().SetViewPort(
			p_context,
			m_vp_width_ratio * m_window_width,
			m_vp_height_ratio * m_window_height,
			m_top_left_x * m_window_width,
			m_top_left_y * m_window_height
		);
	}

	VECTOR3 Camera::GetWorldPosition() const
	{
		return GetTransform()->Position();
	}

	Quaternion Camera::GetWorldRotation() const
	{
		return GetTransform()->Rotation();
	}

	void Camera::SetSRVMain(ID3D11DeviceContext* p_context, const int slot, const bool use_compute)
	{
		if (use_compute)
		{
			p_context->CSSetShaderResources(slot, 1, m_main_target->GetAddressOfSRV());
		}
		else
		{
			p_context->PSSetShaderResources(slot, 1, m_main_target->GetAddressOfSRV());
		}
	}

	void Camera::SetSRVNormal(ID3D11DeviceContext* p_context, const int slot, const bool use_compute)
	{
		if (use_compute)
		{
			p_context->CSSetShaderResources(slot, 1, m_normal_target->GetAddressOfSRV());
		}
		else
		{
			p_context->PSSetShaderResources(slot, 1, m_normal_target->GetAddressOfSRV());
		}
	}

	void Camera::SetSRVDiffuseSpecular(ID3D11DeviceContext* p_context, const int slot, const bool use_compute)
	{
		if (use_compute)
		{
			p_context->CSSetShaderResources(slot, 1, m_diffuse_target->GetAddressOfSRV());
		}
		else
		{
			p_context->PSSetShaderResources(slot, 1, m_diffuse_target->GetAddressOfSRV());
		}
	}

	void Camera::SetSRVMainRoughness(ID3D11DeviceContext* p_context, const int slot, const bool use_compute)
	{
		if (use_compute)
		{
			p_context->CSSetShaderResources(slot, 1, m_specular_target->GetAddressOfSRV());
		}
		else
		{
			p_context->PSSetShaderResources(slot, 1, m_specular_target->GetAddressOfSRV());
		}
	}

	void Camera::SetSRVMainDepth(ID3D11DeviceContext* p_context, const int slot, const bool use_compute)
	{
		if (use_compute)
		{
			p_context->CSSetShaderResources(slot, 1, m_depth_target->GetAddressOfSRV());
		}
		else
		{
			p_context->PSSetShaderResources(slot, 1, m_depth_target->GetAddressOfSRV());
		}
	}

	void Camera::SetRTVs(ID3D11DeviceContext* p_context, const bool set_color, const bool set_depth)
	{
		ID3D11RenderTargetView* rtvs[] =
		{
			m_main_target->GetRTV(),
			m_normal_target->GetRTV(),
			m_diffuse_target->GetRTV(),
			m_specular_target->GetRTV()
		};

		if (set_depth && set_color)
		{
			p_context->OMSetRenderTargets(std::size(rtvs), rtvs, m_depth_target->GetDSV());
		}
		else if (!set_depth && set_color)
		{
			p_context->OMSetRenderTargets(std::size(rtvs), rtvs, nullptr);
		}
		else if (!set_color && set_depth)
		{
			ID3D11RenderTargetView* reset_rtvs[] =
			{
				nullptr,
				nullptr,
				nullptr,
				nullptr
			};
			p_context->OMSetRenderTargets(std::size(reset_rtvs), reset_rtvs, m_depth_target->GetDSV());
		}
		else
		{
			assert(0 && "Invalid argument.");
		}
	}

	void Camera::RemoveRTVs(ID3D11DeviceContext* p_context)
	{
		ID3D11RenderTargetView* reset_rtvs[] =
		{
			nullptr,
			nullptr,
			nullptr,
			nullptr
		};
		p_context->OMSetRenderTargets(std::size(reset_rtvs), reset_rtvs, nullptr);
	}

	void Camera::ClearRTVs(ID3D11DeviceContext* p_context, const VECTOR4& color)
	{
		const FLOAT colors[4] = { color.x,color.y,color.z,color.w };
		m_main_target->Clear(p_context, colors);
		m_normal_target->Clear(p_context, colors);
		m_diffuse_target->Clear(p_context, colors);
		m_specular_target->Clear(p_context, colors);
		m_depth_target->Clear(p_context);
	}

	void Camera::CopyAndBindTargets(ID3D11DeviceContext* p_context)
	{
		// Resourceバインドを外す
		RemoveRTVs(p_context);
		ID3D11ShaderResourceView* srv[1] = { nullptr };
		p_context->VSSetShaderResources(TEXSLOT_COLOR, 1, srv);
		p_context->PSSetShaderResources(TEXSLOT_COLOR, 1, srv);
		p_context->GSSetShaderResources(TEXSLOT_COLOR, 1, srv);
		p_context->DSSetShaderResources(TEXSLOT_COLOR, 1, srv);
		p_context->HSSetShaderResources(TEXSLOT_COLOR, 1, srv);

		p_context->VSSetShaderResources(TEXSLOT_DEPTH, 1, srv);
		p_context->PSSetShaderResources(TEXSLOT_DEPTH, 1, srv);
		p_context->GSSetShaderResources(TEXSLOT_DEPTH, 1, srv);
		p_context->DSSetShaderResources(TEXSLOT_DEPTH, 1, srv);
		p_context->HSSetShaderResources(TEXSLOT_DEPTH, 1, srv);

		// Copy resource.
		p_context->CopyResource(m_copy_main_target_texture.GetResource(), m_main_target->GetResource());
		p_context->CopyResource(m_copy_depth_target_texture.GetResource(), m_depth_target->GetResource());

		// Set RTVs
		SetRTVs(p_context, true, true);

		// Set copy texture.
		m_copy_main_target_texture.SetSRV(p_context, TEXSLOT_COLOR, ShaderVisibility::ALL);
		m_copy_depth_target_texture.SetSRV(p_context, TEXSLOT_DEPTH, ShaderVisibility::ALL);
	}

	void Camera::CopyColorTarget(ID3D11DeviceContext* p_context, Texture& texture)
	{
		// Resourceバインドを外す
		RemoveRTVs(p_context);
		// Copy resource.
		p_context->CopyResource(texture.GetResource(), m_main_target->GetResource());
		// Set RTVs
		SetRTVs(p_context, true, true);
	}

	void Camera::Update(ID3D11DeviceContext* p_context)
	{
		// Update parameter.
		{
			const std::shared_ptr<Transform>& transform = GetTransform();
			const VECTOR3 forward = transform->Forward();

			m_previous_view_projection_matrix = m_view_matrix * m_reversed_projection_matrix;

			m_view_matrix = MATRIX::CreateLookTo(transform->Position(), forward, transform->Up());
			m_projection_matrix = m_is_perspective ?
				MATRIX::CreatePerspectiveFieldOfView(
					MyMath::AngleToRadian(m_fov_y), (m_vp_width_ratio * m_window_width) / (m_vp_height_ratio * m_window_height), m_near_clip_plane, m_far_clip_plane) :
				MATRIX::CreateOrthographic(m_projection_width, m_projection_height, m_near_clip_plane, m_far_clip_plane);
			// 現在のプロジェクション行列からフラスタムの作成
			m_frustum.Create(
				transform->Position(),
				transform->Rotation(),
				m_projection_matrix
			);
			// プロジェクション行列からReversed-z行列を作成
			m_reversed_projection_matrix = m_projection_matrix * MATRIX::Reversed_Z;
		}

		// Update CBuffer parameter.
		{
			// CB_Camera
			{
				CB_Camera cb = {};

				const std::shared_ptr<Transform>& transform = GetTransform();
				const VECTOR3 forward = transform->Forward();

				cb.direction = forward;
				cb.pos = transform->Position();
				cb.V = m_view_matrix;
				cb.IV = cb.V.Invert();
				cb.P = m_reversed_projection_matrix;
				cb.IP = cb.P.Invert();
				cb.IVP = (cb.V * cb.P).Invert();
				cb.prev_VP = m_previous_view_projection_matrix;
				cb.near_dist = m_near_clip_plane;
				cb.far_dist = m_far_clip_plane;
				cb.far_subst_near = m_far_clip_plane - m_near_clip_plane;
				cb.inv_far_subst_near = 1.0f / cb.far_subst_near;
				cb.target_width = m_window_width;
				cb.target_height = m_window_height;

				m_cb_camera.SetParam(&cb, sizeof(cb));
			}
			// CB_VP
			{
				const MATRIX vp = m_view_matrix * m_reversed_projection_matrix;
				m_cb_VP.SetParam(&vp, sizeof(vp));
			}
		}
	}

	const Frustum& Camera::GetFrustum() const
	{
		return m_frustum;
	}

	Frustum Camera::GetFrustum(const float near_dist, const float far_dist) const
	{
		const std::shared_ptr<Transform>& transform = GetTransform();
		const MATRIX P = m_is_perspective ?
			MATRIX::CreatePerspectiveFieldOfView(
				MyMath::AngleToRadian(m_fov_y), (m_vp_width_ratio * m_window_width) / (m_vp_height_ratio * m_window_height), near_dist, far_dist) :
			MATRIX::CreateOrthographic(m_projection_width, m_projection_height, near_dist, far_dist);

		Frustum ret;
		ret.Create(transform->Position(), transform->Rotation(), P);
		return ret;
	}

	bool Camera::IsActive()
	{
		return IsActiveBehaviour();
	}

	bool Camera::IsOutputTarget() const
	{
		return m_is_output_target;
	}

}// namespace TKGEngine