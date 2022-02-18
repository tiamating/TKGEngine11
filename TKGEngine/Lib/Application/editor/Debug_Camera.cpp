#include "Debug_Camera.h"
#ifdef USE_IMGUI
#include "Systems/inc/IWindow.h"
#include "Systems/inc/IGraphics.h"
#include "Systems/inc/IInput.h"
#include "Systems/inc/ITime.h"
#include "Systems/inc/IGUI.h"
#include "Systems/inc/LogSystem.h"
#include "Systems/inc/Graphics_Defined.h"
#include "Systems/inc/StateManager.h"

#include "Utility/inc/myfunc_math.h"

#include "../Resource/inc/Shader_Defined.h"

namespace /* anonymous */
{
	constexpr float CAMERA_SPEED_WHEEL = 0.003f * 120.0f;
	constexpr float CAMERA_SPEED_ROTATE_X = 0.04f;
	constexpr float CAMERA_SPEED_ROTATE_Y = 0.08f;
	constexpr float CAMERA_SPEED_SLIDE = 0.005f;
	constexpr float CAMERA_SPEED_MOVE = 20.0f;
	constexpr float CAMERA_MOVE_OFFSET = 10.0f;
	constexpr float CAMERA_MOVE_BRAKE = 5.0f;

	// デバッグカメラの生成時に代入するTransform
	TKGEngine::VECTOR3 g_camera_pos = TKGEngine::VECTOR3(0.0f, 5.0f, 8.0f);
	TKGEngine::Quaternion g_camera_rot = TKGEngine::Quaternion::EulerAngles(TKGEngine::VECTOR3(-30.0f, 0.0f, 0.0f));
}// namespace /* anonymous */

namespace TKGEngine
{
	DebugCamera::DebugCamera()
	{
		m_position = g_camera_pos;
		m_rotation = g_camera_rot;
	}

	DebugCamera::~DebugCamera()
	{
		g_camera_pos = m_position;
		g_camera_rot = m_rotation;
	}

	void DebugCamera::OnGUIRayPick()
	{
		// TODO : GUI対応　レイピック用





	}

	bool DebugCamera::Create()
	{
		// Create Target
		if (!CreateTargetsAndTexture())
		{
			assert(0 && "failed CreateTargets() DebugCamera::OnCreate()");
			return false;
		}
		// Create CBuffer
		{
			if (!m_cb_camera.Create(sizeof(CB_Camera), false))
			{
				assert(0 && "failed ConstantBuffer::Create(CB_Camera) DebugCamera::OnCreate()");
				return false;
			}

			if (!m_cb_VP.Create(sizeof(MATRIX), false))
			{
				assert(0 && "failed ConstantBuffer::Create(CB_VP) DebugCamera::OnCreate()");
				return false;
			}
		}
		// Create VBuffer
		{
			if (!m_vb.Create(nullptr, static_cast<int>(sizeof(VECTOR3)), ARRAYSIZE(m_points), VERTEX_ELEMENT_TYPE::POSITION, BUFFER_HEAP_TYPE::BUFFER_HEAP_DYNAMIC))
			{
				assert(0 && "failed VertexBuffer::Create() DebugCamera::OnCreate()");
				return false;
			}
		}
		// Create Shader
		{
			m_shader.VS().Load("./Shader/DebugGrid_VS.cso");
			m_shader.PS().Load("./Shader/DebugGrid_PS.cso");
		}

		return true;
	}

	void DebugCamera::RenderGrid()
	{
		const VECTOR3& forward = m_rotation.Forward();

		// Get Grid scale
		int height_rank = -1;
		float height = MyMath::Abs(m_position.y);
		while (height > 0.0f)
		{
			height = static_cast<float>(static_cast<int>(height / 10));
			++height_rank;
		}
		// pos.y == 0.0f
		if (height_rank < 0)
			return;

		if (height_rank > 4)
			return;

		static constexpr size_t array_size = ARRAYSIZE(m_points);
		if (height_rank > 0)
			--height_rank;
		const int rank_end = height_rank + 1;


		VECTOR3 center = VECTOR3(m_position.x, 0.0f, m_position.z);
		int idx = 0;
		for (; height_rank <= rank_end; ++height_rank)
		{
			const int interval = MyMath::Pow(10, height_rank);
			// camera位置の最近傍のグリッド本数が何本目かを調べる
			// x
			{
				// 原点から何本目のグリッドがX中央か
				const int division = (static_cast<int>(center.x)) / interval;
				center.x = static_cast<float>(division * interval);
			}
			// y
			{
				center.y = 0.0f;
			}
			// z
			{
				// 原点から何本目のグリッドがZ中央か
				const int division = (static_cast<int>(center.z)) / interval;
				center.z = static_cast<float>(division * interval);
			}

			// Set Vertices
			{
				const float edge_length = static_cast<float>(interval) * (m_axis_line_count / 2);
				const float length = static_cast<float>(interval * (m_axis_line_count - 1));
				const VECTOR3 left_top = center + VECTOR3(-edge_length, 0.0f, edge_length);

				// Vertical
				for (int i = 0; i < m_axis_line_count; ++i)
				{
					// top
					m_points[idx++] = left_top + VECTOR3(static_cast<float>(i * interval), 0.0f, 0.0f);
					// bottom
					m_points[idx] = m_points[idx - 1] - VECTOR3(0.0f, 0.0f, length);
					++idx;
				}
				// Horizontal
				for (int i = 0; i < m_axis_line_count; ++i)
				{
					// left
					m_points[idx++] = left_top - VECTOR3(0.0f, 0.0f, static_cast<float>(i * interval));
					// right
					m_points[idx] = m_points[idx - 1] + VECTOR3(length, 0.0f, 0.0f);
					++idx;
				}
			}
		}

		// TODO : シーンビューのグリッド線はメイン描画の最後にコマンドをプッシュしている.他の描画をするなら順番を考える必要ある
		const auto context = IGraphics::Get().DC(0, Graphics::DC_RENDER_PATH::DC_RP_MAIN);

		// Mapped Buffer
		{
			VECTOR3* p_map = nullptr;
			m_vb.Map(context, reinterpret_cast<void**>(&p_map));
			{
				memcpy(p_map, m_points, sizeof(VECTOR3) * array_size);
			}
			m_vb.Unmap(context);
		}


		m_vb.Set(context, 0);
		m_shader.Activate(context, false);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		StateManager::SetState(context, StateManager::BS::Transparent);
		StateManager::SetState(context, StateManager::RS::FillNone);
		StateManager::SetState(context, StateManager::DS::BackGround, Stencil::Mask::None, false);

		context->Draw(array_size, 0);
	}


	void DebugCamera::SetWorldToViewMatrix(const MATRIX& v)
	{
		v.Invert().Decompose(nullptr, &m_rotation, &m_position);
	}

	void DebugCamera::SetWorldPosition(const VECTOR3& pos)
	{
		m_position = pos;
	}

	bool DebugCamera::ResizeBuffers()
	{
		ReleaseTargets();
		return CreateTargetsAndTexture();
	}
	
	void DebugCamera::SetCBCamera(ID3D11DeviceContext* p_context)
	{
		m_cb_camera.SetVS(p_context, CBS_CAMERA);
		m_cb_camera.SetGS(p_context, CBS_CAMERA);
		m_cb_camera.SetPS(p_context, CBS_CAMERA);
	}

	void DebugCamera::SetCBViewProjection(ID3D11DeviceContext* p_context)
	{
		m_cb_VP.SetVS(p_context, CBS_VP);
		m_cb_VP.SetGS(p_context, CBS_VP);
		m_cb_VP.SetPS(p_context, CBS_VP);
	}

	void DebugCamera::SetSRVMain(ID3D11DeviceContext* p_context, int slot, bool use_compute)
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

	void DebugCamera::SetSRVNormal(ID3D11DeviceContext* p_context, int slot, bool use_compute)
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

	void DebugCamera::SetSRVDiffuseSpecular(ID3D11DeviceContext* p_context, int slot, bool use_compute)
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

	void DebugCamera::SetSRVMainRoughness(ID3D11DeviceContext* p_context, int slot, bool use_compute)
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

	void DebugCamera::SetSRVMainDepth(ID3D11DeviceContext* p_context, int slot, bool use_compute)
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

	void DebugCamera::SetRTVs(ID3D11DeviceContext* p_context, bool set_color, bool set_depth)
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

	void DebugCamera::RemoveRTVs(ID3D11DeviceContext* p_context)
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

	void DebugCamera::ClearRTVs(ID3D11DeviceContext* p_context, const VECTOR4& color)
	{
		const FLOAT colors[4] = { color.x,color.y,color.z,color.w };
		m_main_target->Clear(p_context, colors);
		m_normal_target->Clear(p_context, colors);
		m_diffuse_target->Clear(p_context, colors);
		m_specular_target->Clear(p_context, colors);
		m_depth_target->Clear(p_context);
	}

	void DebugCamera::CopyAndBindTargets(ID3D11DeviceContext* p_context)
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

	void DebugCamera::CopyColorTarget(ID3D11DeviceContext* p_context, Texture& texture)
	{
		// Resourceバインドを外す
		RemoveRTVs(p_context);
		// Copy resource.
		p_context->CopyResource(texture.GetResource(), m_main_target->GetResource());
		// Set RTVs
		SetRTVs(p_context, true, true);
	}

	void DebugCamera::SetViewport(ID3D11DeviceContext* p_context)
	{
		IGraphics::Get().SetViewPort(
			p_context,
			m_vp_width, m_vp_height,
			m_top_left_x * m_vp_width, m_top_left_y * m_vp_height
		);
	}

	float DebugCamera::GetViewportWidth() const
	{
		return m_vp_width;
	}

	float DebugCamera::GetViewportHeight() const
	{
		return m_vp_height;
	}

	VECTOR3 DebugCamera::GetWorldPosition() const
	{
		return m_position;
	}

	Quaternion DebugCamera::GetWorldRotation() const
	{
		return m_rotation;
	}

	float DebugCamera::Depth() const
	{
		return 0.0f;
	}

	int DebugCamera::GetCullingLayer() const
	{
		return 0;
	}

	void DebugCamera::Update(ID3D11DeviceContext* p_context)
	{
		// Update parameter.
		{
			OnUpdate();

			m_previous_view_projection_matrix = m_view_matrix * m_reversed_projection_matrix;

			m_view_matrix = MATRIX::CreateLookTo(m_position, m_rotation.Forward(), m_rotation.Up());

			m_projection_matrix = MATRIX::CreatePerspectiveFieldOfView(
				MyMath::AngleToRadian(m_fov_y), m_vp_width / m_vp_height, m_near_clip_plane, m_far_clip_plane
			);
			// 現在のプロジェクション行列からフラスタムの作成
			m_frustum.Create(
				m_position,
				m_rotation,
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

				const VECTOR3 forward = m_rotation.Forward();

				cb.direction = forward;
				cb.pos = m_position;
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
				cb.target_width = IWindow::Get().GetScreenWidthf();
				cb.target_height = IWindow::Get().GetScreenHeightf();

				m_cb_camera.SetParam(&cb, sizeof(cb));
			}

			// CB_VP
			{
				const MATRIX VP = m_view_matrix * m_reversed_projection_matrix;
				m_cb_VP.SetParam(&VP, sizeof(VP));
			}
		}
	}

	bool DebugCamera::CreateTargetsAndTexture()
	{
		// Set viewport
		m_vp_width = IWindow::Get().GetScreenWidthf();
		m_vp_height = IWindow::Get().GetScreenHeightf();

		// https://zerogram.info/?p=1070
		// https://www.4gamer.net/games/110/G011050/20110401056/
		{
			// TODO : HDR非対応版
			TargetDesc desc(
				static_cast<UINT>(m_vp_width),
				static_cast<UINT>(m_vp_height),
				Graphics::g_color_format,
				m_msaa_count,
				m_msaa_quarity
			);
			const auto device = IGraphics::Get().Device();

			m_main_target = IColorTarget::CreateInterface();
			if (!m_main_target->Create(device, desc, true, true))
			{
				assert(0 && "failed IColorTarget::Create(Main) DebugCamera::CreateTargets()");
				return false;
			}

			desc.format = DXGI_FORMAT_R10G10B10A2_UNORM;
			m_normal_target = IColorTarget::CreateInterface();
			if (!m_normal_target->Create(device, desc, true, true))
			{
				assert(0 && "failed IColorTarget::Create(Normal) DebugCamera::CreateTargets()");
				return false;
			}

			desc.format = DXGI_FORMAT_R10G10B10A2_UNORM;
			m_diffuse_target = IColorTarget::CreateInterface();
			if (!m_diffuse_target->Create(device, desc, true, true))
			{
				assert(0 && "failed IColorTarget::Create(Diffuse) DebugCamera::CreateTargets()");
				return false;
			}

			desc.format = DXGI_FORMAT_R10G10B10A2_UNORM;
			m_specular_target = IColorTarget::CreateInterface();
			if (!m_specular_target->Create(device, desc, true, true))
			{
				assert(0 && "failed IColorTarget::Create(Specular) DebugCamera::CreateTargets()");
				return false;
			}

			desc.format = DXGI_FORMAT_R8_UINT;
			m_roughness_target = IColorTarget::CreateInterface();
			if (!m_roughness_target->Create(device, desc, true, true))
			{
				assert(0 && "failed IColorTarget::Create(Roughness) DebugCamera::CreateTargets()");
				return false;
			}

			desc.format = Graphics::g_depth_format;
			m_depth_target = IDepthTarget::CreateInterface();
			if (!m_depth_target->Create(device, desc, true))
			{
				assert(0 && "failed IColorTarget::Create(Depth) DebugCamera::CreateTargets()");
				return false;
			}
		}

		// Create Copy texture
		{
			TEX_DESC desc;
			desc.width = static_cast<int>(m_vp_width);
			desc.height = static_cast<int>(m_vp_height);
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

		return true;
	}

	void DebugCamera::ReleaseTargets()
	{
		m_main_target.reset();
		m_normal_target.reset();
		m_diffuse_target.reset();
		m_specular_target.reset();
		m_roughness_target.reset();
		m_depth_target.reset();

		m_copy_main_target_texture.Release();
		m_copy_depth_target_texture.Release();
	}

	void DebugCamera::OnUpdate()
	{
		const VECTOR3& forward = m_rotation.Forward();
		const VECTOR3& right = m_rotation.Right();
		const VECTOR3& up = m_rotation.Up();

		// Mouse wheel
		if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_::ImGuiHoveredFlags_AnyWindow))
		{
			const float wheel_val = ImGui::GetIO().MouseWheel;
			if(!MyMath::Approximately(wheel_val, 0.0f))
			{
				m_position += ((wheel_val * CAMERA_SPEED_WHEEL) * forward);
			}
		}

		// クリックを使う処理
		static bool mouse_clicked = false;
		static VECTOR2 push_pos;
		float delta_time = ITime::Get().UnscaledDeltaTime();

		// ImGuiのWindowに乗っている状態かつ、右クリックか中クリックされたら操作されている状態にする
		if (
			IGUI::Get().IsActiveInputScene() &&
			(ImGui::IsMouseClicked(ImGuiMouseButton_Middle) || ImGui::IsMouseClicked(ImGuiMouseButton_Right)) &&
			!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)
			)
		{
			IInput::Get().SetMouseVisible(false);
			mouse_clicked = true;
		}

		// 操作されている状態でないとカメラは操作できない
		if (!mouse_clicked)
			return;

		// Mouse middle button
		{
			if (IInput::Get().GetMouseButtonDown(Input::Mouse::MBUTTON))
			{
				push_pos = IInput::Get().GetMousePos();
			}
			else if (IInput::Get().GetMouseButton(Input::Mouse::MBUTTON))
			{
				VECTOR2 move_value = IInput::Get().GetMousePos() - push_pos;
				move_value *= (CAMERA_SPEED_SLIDE);
				m_position += ((move_value.x * -right) + (move_value.y * up));
				IInput::Get().SetMousePos(push_pos);
			}
		}
		// Mouse right button
		{
			if (IInput::Get().GetMouseButtonDown(Input::Mouse::RBUTTON))
			{
				push_pos = IInput::Get().GetMousePos();
			}
			else if (IInput::Get().GetMouseButton(Input::Mouse::RBUTTON))
			{
				// Rotation
				{
					const VECTOR2 move_value = IInput::Get().GetMousePos() - push_pos;
					VECTOR3 euler = m_rotation.ToEulerAngles();
#ifdef AXIS_RH
					euler.x += -move_value.y * CAMERA_SPEED_ROTATE_X;
#else
					euler.x += move_value.y * CAMERA_SPEED_ROTATE_X;
#endif// #ifdef AXIS_RH
					if (euler.x >= 89.0f)
						euler.x = 89.0f;
					else if (euler.x <= -89.0f)
						euler.x = -89.0f;
#ifdef AXIS_RH
					euler.y += -move_value.x * CAMERA_SPEED_ROTATE_Y;
#else
					euler.y += move_value.x * CAMERA_SPEED_ROTATE_Y;
#endif// #ifdef AXIS_RH
					euler.z = 0.0f;
					m_rotation = Quaternion::EulerAngles(euler);
				}

				// Move
				{
					static float weight_da = 0.f;
					static float weight_ws = 0.f;
					static float weight_eq = 0.f;
					static float weight_space_l_shift = 0.f;

					const float delta_time = ITime::Get().UnscaledDeltaTime();

					if (IInput::Get().GetKey(Input::Key::D))
					{
						weight_da += delta_time;
					}
					if (IInput::Get().GetKey(Input::Key::A))
					{
						weight_da -= delta_time;
					}
					if (IInput::Get().GetKey(Input::Key::W))
					{
						weight_ws += delta_time;
					}
					if (IInput::Get().GetKey(Input::Key::S))
					{
						weight_ws -= delta_time;
					}
					if (IInput::Get().GetKey(Input::Key::E))
					{
						weight_eq += delta_time;
					}
					if (IInput::Get().GetKey(Input::Key::Q))
					{
						weight_eq -= delta_time;
					}
					if (IInput::Get().GetKey(Input::Key::Space))
					{
						weight_space_l_shift += delta_time;
					}
					if (IInput::Get().GetKey(Input::Key::LeftShift))
					{
						weight_space_l_shift -= delta_time;
					}

					// D or A
					if (!MyMath::Approximately(weight_da, 0.0f))
					{
						if (weight_da > 0.0f)
						{
							// カメラ-X方向
							if (!IInput::Get().GetKey(Input::Key::D) || IInput::Get().GetKey(Input::Key::A))
							{
								weight_da -= delta_time * CAMERA_MOVE_BRAKE;
								if (weight_da < 0.0f)
									weight_da = 0.0f;
							}
						}
						else
						{
							// カメラ+X方向
							if (!IInput::Get().GetKey(Input::Key::A) || IInput::Get().GetKey(Input::Key::D))
							{
								weight_da += delta_time * CAMERA_MOVE_BRAKE;
								if (weight_da > 0.0f)
									weight_da = 0.0f;
							}
						}
						weight_da = MyMath::Clamp(weight_da, -1.0f, 1.0f);
						m_position += right * CAMERA_SPEED_MOVE * delta_time * weight_da;
					}
					// W or S
					if (!MyMath::Approximately(weight_ws, 0.0f))
					{
						if (weight_ws > 0.0f)
						{
							// カメラ-Z方向
							if (!IInput::Get().GetKey(Input::Key::W) || IInput::Get().GetKey(Input::Key::S))
							{
								weight_ws -= delta_time * CAMERA_MOVE_BRAKE;
								if (weight_ws < 0.0f)
									weight_ws = 0.0f;
							}
						}
						else
						{
							// カメラ+Z方向
							if (!IInput::Get().GetKey(Input::Key::S) || IInput::Get().GetKey(Input::Key::W))
							{
								weight_ws += delta_time * CAMERA_MOVE_BRAKE;
								if (weight_ws > 0.0f)
									weight_ws = 0.0f;
							}
						}
						weight_ws = MyMath::Clamp(weight_ws, -1.0f, 1.0f);
						m_position += forward * CAMERA_SPEED_MOVE * delta_time * weight_ws;
					}
					// E or Q
					if (!MyMath::Approximately(weight_eq, 0.0f))
					{
						if (weight_eq > 0.0f)
						{
							// カメラ-Y方向
							if (!IInput::Get().GetKey(Input::Key::E) || IInput::Get().GetKey(Input::Key::Q))
							{
								weight_eq -= delta_time * CAMERA_MOVE_BRAKE;
								if (weight_eq < 0.0f)
									weight_eq = 0.0f;
							}
						}
						else
						{
							// カメラ+Y方向
							if (!IInput::Get().GetKey(Input::Key::Q) || IInput::Get().GetKey(Input::Key::E))
							{
								weight_eq += delta_time * CAMERA_MOVE_BRAKE;
								if (weight_eq > 0.0f)
									weight_eq = 0.0f;
							}
						}
						weight_eq = MyMath::Clamp(weight_eq, -1.0f, 1.0f);
						m_position += up * CAMERA_SPEED_MOVE * delta_time * weight_eq;
					}
					// Space or LeftShift
					if (!MyMath::Approximately(weight_space_l_shift, 0.0f))
					{
						if (weight_space_l_shift > 0.0f)
						{
							// ワールド上方向
							if (!IInput::Get().GetKey(Input::Key::Space) || IInput::Get().GetKey(Input::Key::LeftShift))
							{
								weight_space_l_shift -= delta_time * CAMERA_MOVE_BRAKE;
								if (weight_space_l_shift < 0.0f)
									weight_space_l_shift = 0.0f;
							}
						}
						else
						{
							// ワールド下方向
							if (!IInput::Get().GetKey(Input::Key::LeftShift) || IInput::Get().GetKey(Input::Key::Space))
							{
								weight_space_l_shift += delta_time * CAMERA_MOVE_BRAKE;
								if (weight_space_l_shift > 0.0f)
									weight_space_l_shift = 0.0f;
							}
						}
						weight_space_l_shift = MyMath::Clamp(weight_space_l_shift, -1.0f, 1.0f);
						m_position += VECTOR3::Up * CAMERA_SPEED_MOVE * delta_time * weight_space_l_shift;
					}
				}
				IInput::Get().SetMousePos(push_pos);
			}
		}

		// 右クリックも中クリックもされていなければ操作されている状態を解除する
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Middle) || ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		{
			IInput::Get().SetMouseVisible(true);
			mouse_clicked = false;
		}
	}

	const Frustum& DebugCamera::GetFrustum() const
	{
		return m_frustum;
	}

	Frustum DebugCamera::GetFrustum(const float near_dist, const float far_dist) const
	{
		const MATRIX P = MATRIX::CreatePerspectiveFieldOfView(
			MyMath::AngleToRadian(m_fov_y), m_vp_width / m_vp_height, near_dist, far_dist
		);

		Frustum ret;
		ret.Create(m_position, m_rotation, P);
		return ret;
	}

	bool DebugCamera::IsActive()
	{
		return true;
	}

}// namespace TKGEngine
#endif // USE_IMGUI