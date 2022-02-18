
#include "CameraManager.h"

#include "RendererManager.h"
#include "SceneManager.h"
#include "Systems/inc/StateManager.h"
#include "Systems/inc/IGraphics.h"
#include "Systems/inc/IWindow.h"
#include "Application/Resource/inc/Effect.h"

#ifdef USE_IMGUI
#include "../../editor/Debug_Camera.h"
#include "Systems/inc/IGUI.h"
#include "Systems/inc/PhysicsSystem.h"
#include <Windows.h>
#endif// #ifdef USE_IMGUI

#include <algorithm>
#include <iterator>
#include <cassert>


namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Static member definition
	////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	ImFont* CameraManager::m_assertion_font = nullptr;
	std::shared_ptr<DebugCamera> CameraManager::m_debug_camera = nullptr;
#endif// #ifdef USE_IMGUI
	std::unordered_map<CameraID, std::shared_ptr<ICamera>> CameraManager::m_camera_index;
	int CameraManager::m_management_camera_cnt = 0;
	int CameraManager::m_current_id = 0;

	static constexpr int START_LIST_SIZE = 5;
	static constexpr int ADD_SIZE = 5;
	std::vector<std::shared_ptr<ICamera>> CameraManager::m_sorted_list;
	int CameraManager::m_current_count = 0;
	int CameraManager::m_prev_count = 0;
	int CameraManager::m_current_size = 0;

	Shader CameraManager::m_fullscreen_quad_shader;

	std::mutex CameraManager::m_mutex;

	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	bool CameraManager::Initialize()
	{
		// カメラ登録用リストの初期処理
		m_current_size = START_LIST_SIZE;
		m_sorted_list.resize(START_LIST_SIZE);

#ifdef USE_IMGUI
		// カメラなし時のウィンドウ用フォント設定
		if (!m_assertion_font)
		{
			const ImGuiIO& io = ImGui::GetIO();
			m_assertion_font = io.Fonts->AddFontFromFileTTF(
				"./Asset/Fonts/corbelz.ttf", 40.0f);
			assert(m_assertion_font && "Failed Debug font.");
		}
		// デバッグカメラ
		m_debug_camera = std::make_shared<DebugCamera>();
		if (!m_debug_camera->Create())
		{
			assert(0 && "failed DebugCamear::Create() CameraManager::Initialize()");
			return false;
		}
#endif// #ifdef USE_IMGUI

		// FullscreenQuadシェーダー
		m_fullscreen_quad_shader.VS().Load("./Shader/FullscreenQuad/FullscreenQuad_VS.cso");
		m_fullscreen_quad_shader.PS().Load("./Shader/FullscreenQuad/FullscreenQuad_PS.cso");

		return true;
	}

	void CameraManager::Terminate()
	{
#ifdef USE_IMGUI
		m_debug_camera.reset();
#endif// #ifdef USE_IMGUI
	}

	void CameraManager::Run()
	{
#ifdef USE_IMGUI
		// シーンビュー
		if (IGUI::Get().IsSceneView())
		{
			PresentAndClear();
			RenderAndUpdateDebugCamera();
			return;
		}
#endif// #ifdef USE_IMGUI

		// Sort Part
		SetVector();
		// デバッグ用 有効なカメラがないなら早期リターン
		if (!m_current_count)
		{
#ifdef USE_IMGUI
			const ImVec2 wnd_size = ImVec2(480.0f, 50.f);
			ImVec2 wnd_center(0.f, 0.f);
			const RECT rect = IWindow::Get().GetScreenRect();
			{
				wnd_center.x = (rect.left - wnd_size.x * 0.5f) + (rect.right - rect.left) * 0.5f;
				wnd_center.y = (rect.top - wnd_size.y * 0.5f) + (rect.bottom - rect.top) * 0.5f;
			}
			ImGui::PushFont(m_assertion_font);
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.7f, 0.3f, 0.3f, 1.0f));
			ImGui::SetNextWindowPos(wnd_center, ImGuiCond_Always);
			ImGui::SetNextWindowSize(wnd_size, ImGuiCond_Always);
			ImGui::Begin("Assertion No Camera", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
			ImGui::Text("Scenes don't have Camera!");
			ImGui::NewLine();
			ImGui::End();
			ImGui::PopStyleColor();
			ImGui::PopFont();
#endif// #ifdef USE_IMGUI

			PresentAndClear();
			return;
		}
		SortCameras();

		// Render Part
		PresentAndClear();
		RenderAndUpdateCameras();

		// Clean Up Part
		CleanUpVector();
		Clear();
	}

	CameraID CameraManager::RegisterManager(const std::shared_ptr<ICamera>& p_camera)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		m_camera_index.emplace(++m_current_id, p_camera);
		++m_management_camera_cnt;
		return m_current_id;
	}

	void CameraManager::UnregisterManager(CameraID id)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		m_camera_index.erase(id);
		--m_management_camera_cnt;
	}

	void CameraManager::OnResizeWindow()
	{
		std::lock_guard<std::mutex> lock(m_mutex);

#ifdef USE_IMGUI
		const std::shared_ptr<ICamera> s_ptr = std::static_pointer_cast<ICamera>(m_debug_camera);
		s_ptr->ResizeBuffers();
#endif// #ifdef USE_IMGUI

		for (const auto& camera : m_camera_index)
		{
			camera.second->ResizeBuffers();
		}
	}

#ifdef USE_IMGUI
	MATRIX CameraManager::GetSceneView()
	{
		const std::shared_ptr<ICamera> s_ptr = std::static_pointer_cast<ICamera>(m_debug_camera);
		return s_ptr->GetWorldToViewMatrix();
	}
	void CameraManager::SetSceneView(const MATRIX& view)
	{
		m_debug_camera->SetWorldToViewMatrix(view);
	}
	void CameraManager::SetSceneViewPosition(const VECTOR3& pos)
	{
		m_debug_camera->SetWorldPosition(pos);
	}
	MATRIX CameraManager::GetSceneProjection()
	{
		const std::shared_ptr<ICamera> s_ptr = std::static_pointer_cast<ICamera>(m_debug_camera);
		return s_ptr->GetProjectionMatrix();
	}
	std::shared_ptr<ICamera> CameraManager::GetDebugCamera()
	{
		return m_debug_camera;
	}
#endif// #ifdef USE_IMGUI

	void CameraManager::SetVector()
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		for (auto&& camera : m_camera_index)
		{
			// カメラがアクティブかチェック
			if (!camera.second->IsActive())
				continue;
			// リストのサイズチェック
			if (m_current_count >= m_current_size)
			{
				m_current_size += ADD_SIZE;
				m_sorted_list.resize(ADD_SIZE);
			}
			// リストにセット
			m_sorted_list.at(m_current_count) = camera.second;
			++m_current_count;
		}
	}

	void CameraManager::SortCameras()
	{
		// 2つ以上の場合はソートする
		if (m_management_camera_cnt > 1)
		{
			const auto itr_begin = m_sorted_list.begin();
			auto itr_end = itr_begin;
			std::advance(itr_end, m_current_count);
			std::sort(itr_begin, itr_end, SortFunc_Depth);
		}
	}

	bool CameraManager::SortFunc_Depth(const std::shared_ptr<ICamera>& l, const std::shared_ptr<ICamera>& r)
	{
		return l->Depth() < r->Depth();
	}

	void CameraManager::PresentAndClear()
	{
		IGraphics::Get().Present(0);
		IGraphics::Get().ClearBackBuffer();
	}

	void CameraManager::RenderAndUpdateCameras()
	{
		for (int i = 0; i < m_current_count; ++i)
		{
			const auto& camera = m_sorted_list.at(i);

			// RTVとDSVのクリア
			camera->ClearRTVs(IGraphics::Get().IC(), VECTOR4(0.0f, 0.4f, 0.f, 1.0f));

			// 現在のTransformでCameraを更新する
			camera->Update(IGraphics::Get().IC());

			// Effectのカメラ行列を更新する
			Effect::SetCameraParam(camera->GetWorldToViewMatrix(), camera->GetProjectionMatrix());

			// Sceneの定数バッファをセット
			SceneManager::SetPipeline();

			// DC_RP_MAINにSkyBox描画を積む
			SceneManager::RenderSkyBox(camera);

			// 描画の実行
			RendererManager::Run(camera);

#ifdef USE_IMGUI
			// コリジョン範囲のデバッグ描画
			if (IGUI::Get().DoDrawDebugExtent())
			{
				PhysicsSystem::DebugDraw();
			}
#endif// #ifdef USE_IMGUI

			const auto post_process_context = IGraphics::Get().DC(0, Graphics::DC_RENDER_PATH::DC_RP_POST_AFTER_MAIN);

			// TODO : ポストエフェクトの処理を追加する

			// コンテキストの設定
			camera->SetViewport(post_process_context);
			camera->SetCBCamera(post_process_context);
			camera->SetCBViewProjection(post_process_context);

			// カメラのレンダーターゲットをテクスチャとしてセットする
			camera->RemoveRTVs(post_process_context);
			camera->SetSRVMain(post_process_context, TEXSLOT_COLOR);

			// BackBufferに出力するものを実行する
			if(camera->IsOutputTarget())
			{
				// Primitive Topology
				post_process_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
				// DepthStencil State
				StateManager::SetState(post_process_context, StateManager::DS::NoTest, Stencil::Mask::None, false);
				// Blend State
				StateManager::SetState(post_process_context, StateManager::BS::Opaque);
				// Rasterizer State
				StateManager::SetState(post_process_context, StateManager::RS::FillBack);

				// Viewport
				IGraphics::Get().SetViewPort(post_process_context, IWindow::Get().GetScreenWidthf(),IWindow::Get().GetScreenHeightf());
				// Shader
				m_fullscreen_quad_shader.Activate(post_process_context, false);
				// RTV
				IGraphics::Get().SetRTBackBuffer(post_process_context, false);
				// 描画
				post_process_context->Draw(4, 0);
			}
		}
	}

#ifdef USE_IMGUI
	void CameraManager::RenderAndUpdateDebugCamera()
	{
		const std::shared_ptr<ICamera> camera = std::static_pointer_cast<ICamera>(m_debug_camera);
		camera->ClearRTVs(IGraphics::Get().IC(), VECTOR4(0.0f, 0.0f, 0.3f, 1.0f));
		camera->Update(IGraphics::Get().IC());

		// Effectのカメラ行列を更新する
		Effect::SetCameraParam(camera->GetWorldToViewMatrix(), camera->GetProjectionMatrix());

		// Sceneの定数バッファをセット
		SceneManager::SetPipeline();

		// DC_RP_MAINにSkyBox描画を積む
		SceneManager::RenderSkyBox(camera);

		// 描画の実行
		RendererManager::Run(camera);

		// コリジョン範囲のデバッグ描画
		if (IGUI::Get().DoDrawDebugExtent())
		{
			PhysicsSystem::DebugDraw();
		}

		// DC_RP_MAINにGrid描画を積む
		m_debug_camera->RenderGrid();

		const auto post_process_context = IGraphics::Get().DC(0, Graphics::DC_RENDER_PATH::DC_RP_POST_AFTER_MAIN);

		// コンテキストの設定
		camera->SetViewport(post_process_context);
		camera->SetCBCamera(post_process_context);
		camera->SetCBViewProjection(post_process_context);

		// カメラのレンダーターゲットをテクスチャとしてセットする
		camera->RemoveRTVs(post_process_context);
		camera->SetSRVMain(post_process_context, TEXSLOT_COLOR);

		// BackBufferに出力する
		post_process_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		StateManager::SetState(post_process_context, StateManager::DS::NoTest, Stencil::Mask::None, false);
		StateManager::SetState(post_process_context, StateManager::BS::Opaque);
		StateManager::SetState(post_process_context, StateManager::RS::FillBack);
		IGraphics::Get().SetViewPort(post_process_context, IWindow::Get().GetScreenWidthf(), IWindow::Get().GetScreenHeightf());
		m_fullscreen_quad_shader.Activate(post_process_context, false);
		IGraphics::Get().SetRTBackBuffer(post_process_context, false);
		post_process_context->Draw(4, 0);
	}
#endif// #ifdef USE_IMGUI

	void CameraManager::CleanUpVector()
	{
		if (m_prev_count > m_current_count)
		{
			for (int i = m_current_count; i < m_prev_count; ++i)
			{
				m_sorted_list.at(i) = nullptr;
			}
		}
	}

	void CameraManager::Clear()
	{
		m_prev_count = m_current_count;
		m_current_count = 0;
	}

}// namespace TKGEngine