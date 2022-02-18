
#include "Systems/inc/IGUISystem.h"
#ifdef USE_IMGUI

#include "Components/inc/CTransform.h"
#include "Components/inc/CLight.h"
#include "Components/inc/CCamera.h"

#include "Systems/inc/IGUI.h"
#include "Systems/inc/IWindow.h"
#include "Systems/inc/IGraphics.h"
#include "Systems/inc/IInput.h"
#include "Systems/inc/PhysicsSystem.h"
#include "Managers/SceneManager.h"
#include "Managers/GameObjectManager.h"
#include "Managers/CameraManager.h"

#include "GUI_Gizmo.h"

#include "Application/Resource/inc/FBXLoader.h"

#include "Utility/inc/myfunc_file.h"
#include "Utility/inc/myfunc_imgui.h"

#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <Windows.h>
#include <d3d11.h>

#include <vector>


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace TKGEngine::GUI
{
	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////

	/// ========================================================
	/// @class	GUISystem
	/// @brief	GUI system
	/// 
	/// ========================================================
	class GUISystem
		: public IGUISystem
		, public IGUI
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		GUISystem();
		virtual ~GUISystem() = default;
		GUISystem(GUISystem&&) = default;
		GUISystem(const GUISystem&) = delete;
		GUISystem& operator=(const GUISystem&) = delete;

		bool OnInit() override;
		void OnTerm() override;
		void OnFrameBegin(const FrameEventArgs& args) override;
		void OnFrameRender(const FrameEventArgs& args) override;
		void OnFrameEnd(const FrameEventArgs& args) override;
		void OnResize(const ResizeEventArgs& args) override;

		void OnMsgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;
		bool HasChangedScreenMode() override;


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================
		// GUI
		// ヒエラルキーでの選択時に使用する関数群
		// 左クリック
		void SetSelectionObject(GameObjectID goid) override;
		void AddSelectionObject(GameObjectID goid) override;
		void RemoveSelectionObject(GameObjectID goid) override;
		void ClearSelectionObject() override;
		const std::vector<GameObjectID>& GetSelectionObjects() const override;
		bool IsSelecting(GameObjectID goid) const override;
		// 右クリック
		void SetRightClickObject(GameObjectID goid) override;
		void ClearRightClickObject() override;
		GameObjectID GetRightClickObject() const override;

		// Draw Shape
		void DrawLine(const VECTOR3& from, const VECTOR3& to, const VECTOR3& from_color, const VECTOR3& to_color) override;
		void DrawSphere(const VECTOR3& pos, const float radius, const VECTOR3& color) override;
		void DrawBox(const VECTOR3& pos, const Quaternion& rot, const VECTOR3& half_extents, const VECTOR3& color) override;
		void DrawCapsule(const VECTOR3& pos, const Quaternion& rot, const float radius, const float height, const VECTOR3& color) override;
		void DrawCone(const VECTOR3& pos, const Quaternion& rot, const float radius, const float height, const VECTOR3& color) override;
		void DrawContactPoint(const VECTOR3& pos, const VECTOR3& normal, const float distance, const VECTOR3& color) override;
		// ~Draw Shape

		// Gizmo
		virtual void EnterControlTransform() override;
		virtual void ExitControlTransform() override;
		// ~Gizmo

		// Scene
		// ゲームシーンを実行中か
		bool IsPlaying() const override;
		// ゲームシーンをちょうど実行された時か
		bool IsPlayedOnFrame() const override;
		// ゲームシーンをちょうど停止した時か
		bool IsStoppedOnFrame() const override;
		bool IsSceneView() const override;
		bool DoDrawDebugExtent() const override;
		bool DoDrawUI() const override;
		void SetDrawController(const GameObjectID goid, const AnimatorController& controller, const Avatar& avatar);
		// IInputの入力が有効であるか
		bool IsActiveInputScene() const override;
		// IInputの入力の有効、無効をセットする
		void SetActiveInputScene(bool is_active) override;
		bool IsPushSaveGlobalSetting() const override;
		// ~Scene
		// ~GUI

		// ImGuiの描画
		void RenderImGui();

		// メインメニューバー関連
		void UpdateMainMenuBar();
		// 常に表示されているGUI関連
		void UpdateDefaultGUI();
		// ショートカットによる操作
		void OnInputShortcut();

		void EditorUpdateOnFrameEnd();
		// フレーム単位で変化を取るためのフラグの更新
		void UpdateOnFrameFlags();

		// ショートカット
		// Ctrl + F
		void SetFocusOnCtrlFKey(GameObjectID goid);

		// Main menu bar item
		void InMainMenuBarGamePlay();
		void InMainMenuBarFile();
		void InMainMenuBarSetting();
		void InMainMenuBarInfo();
		void InMainMenuBarWindow();

		void InMenuItemExport();
		void InMenuItemSetting();
		void InMainMenuBarFlags();
		// ~Main menu bar item

		// On Right Click
		void OnGUIHierarchyRightClick();
		void OnRightClickCreateFunction(bool is_root, const size_t str_size, char* str);

		// On Delete
		void OnDeleteSelectingObject();


		// ==============================================
		// private variables
		// ==============================================
		static constexpr float ONCREATE_DISTANCE = 10.0f;

		D3D11_VIEWPORT m_viewport = {};

		std::vector<GameObjectID> m_selecting_objects;
		GameObjectID m_right_click_object = INVALID_ID;

		Gizmo::GUIGizmo m_gizmo;

		// シーンの再生中か
		bool m_playing_scene = false;
		// ゲームシーンをちょうど実行したフレームか
		bool m_played_scene_on_frame = false;
		// ゲームシーンをちょうど停止したフレームか
		bool m_stopped_scene_on_frame = false;
		// シーンビューを表示中か
		bool m_use_scene_view = true;
		// 範囲のデバッグ描画が有効か
		bool m_do_render_debug_extent = true;
		// UIの描画が有効か
		bool m_do_render_UI = true;
		// シーンへの入力が有効か
		bool m_is_active_input_scene = true;
		// カーソルが消されていた場合、座標を戻す時に使う
		bool m_is_invisible_cursor = false;
		VECTOR2 m_bind_mouse_pos;
		// 右クリック時のウィンドウ表示状態
		bool m_is_active_right_click_option_window = false;

		// Animator Controllerを表示するか
		bool m_do_render_controller_window = false;
		// 表示選択中のAnimator用データ
		GameObjectID m_has_controller_goid_ref = 0;
		AnimatorController m_controller_ref;
		Avatar m_avatar_ref;

		// FBXを変換中か
		bool m_active_export_fbx = false;
		// Windowモードが変更されたか
		bool m_has_changed_window_mode = false;

		// Global SettingのSaveボタンが押されたか
		bool m_is_push_save_global_setting = false;
	};

	IGUISystem* IGUISystem::CreateInterface()
	{
		return new GUISystem();
	}

	GUISystem::GUISystem()
	{
		/* nothing */
	}

	bool GUISystem::OnInit()
	{
		// ImGui初期化
		ImGui_ImplWin32_EnableDpiAwareness();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;
		//io.ConfigViewportsNoDefaultParent = true;
		//io.ConfigDockingAlwaysTabBar = true;
		io.ConfigDockingTransparentPayload = true;
		io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;
		io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.Alpha = 1.0f;
			style.WindowRounding = 10.0f;
			style.WindowBorderSize = 3.0f;
			style.ChildRounding = 4.0f;
			style.FrameBorderSize = 1.0f;
			style.FrameRounding = 6.0f;
			style.GrabMinSize = 10.0f;
			style.PopupRounding = 2.0f;
			style.ScrollbarRounding = 12.0f;
			style.ScrollbarSize = 13.0f;
			style.TabBorderSize = 1.0f;
			style.TabRounding = 0.0f;
			style.IndentSpacing = 5.0f;

			ImGui::StyleColorsDark();
			const auto colors = style.Colors;
			//colors[ImGuiCol_Text] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
			//colors[ImGuiCol_TextDisabled] = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
			colors[ImGuiCol_WindowBg] = ImVec4(0.074f, 0.074f, 0.074f, 0.800f);
			//colors[ImGuiCol_ChildBg] = ImVec4(0.280f, 0.280f, 0.280f, 0.300f);
			//colors[ImGuiCol_PopupBg] = ImVec4(0.263f, 0.263f, 0.263f, 1.000f);
			//colors[ImGuiCol_Border] = ImVec4(0.050f, 0.050f, 0.050f, 1.000f);
			//colors[ImGuiCol_BorderShadow] = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
			//colors[ImGuiCol_FrameBg] = ImVec4(0.120f, 0.120f, 0.120f, 1.000f);
			//colors[ImGuiCol_FrameBgHovered] = ImVec4(0.280f, 0.280f, 0.450f, 1.000f);
			//colors[ImGuiCol_FrameBgActive] = ImVec4(0.210f, 0.210f, 0.720f, 1.000f);
			colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
			colors[ImGuiCol_TitleBgActive] = ImVec4(0.28f, 0.35f, 0.63f, 1.00f);
			colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.41f);
			colors[ImGuiCol_MenuBarBg] = ImVec4(0.055f, 0.055f, 0.055f, 0.900f);
			//colors[ImGuiCol_ScrollbarBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
			//colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
			//colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
			//colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.391f, 0.391f, 1.000f, 1.000f);
			//colors[ImGuiCol_CheckMark] = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
			//colors[ImGuiCol_SliderGrab] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
			//colors[ImGuiCol_SliderGrabActive] = ImVec4(0.000f, 0.391f, 0.800f, 1.000f);
			colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
			colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
			colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
			colors[ImGuiCol_Header] = ImVec4(0.56f, 0.79f, 0.98f, 0.41f);
			colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
			colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			//colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
			//colors[ImGuiCol_SeparatorHovered] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
			//colors[ImGuiCol_SeparatorActive] = ImVec4(0.391f, 0.391f, 1.000f, 1.000f);
			//colors[ImGuiCol_ResizeGrip] = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
			//colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.600f, 0.600f, 1.000f, 0.670f);
			//colors[ImGuiCol_ResizeGripActive] = ImVec4(0.391f, 0.391f, 1.000f, 1.000f);
			colors[ImGuiCol_Tab] = ImVec4(0.233f, 0.233f, 0.233f, 1.000f);
			colors[ImGuiCol_TabHovered] = ImVec4(0.250f, 0.550f, 0.437f, 1.000f);
			colors[ImGuiCol_TabActive] = ImVec4(0.179f, 0.378f, 0.829f, 1.000f);
			colors[ImGuiCol_TabUnfocused] = ImVec4(0.183f, 0.183f, 0.183f, 1.000f);
			colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.333f, 0.333f, 0.483f, 1.000f);
			//colors[ImGuiCol_DockingPreview] = ImVec4(0.280f, 0.280f, 0.750f, 0.681f);
			//colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
			//colors[ImGuiCol_PlotLines] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
			//colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
			//colors[ImGuiCol_PlotHistogram] = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
			//colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
			//colors[ImGuiCol_TextSelectedBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
			//colors[ImGuiCol_DragDropTarget] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
			//colors[ImGuiCol_NavHighlight] = ImVec4(1.000f, 0.391f, 0.000f, 1.000f);
			//colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.000f, 0.391f, 0.000f, 1.000f);
			//colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
			//colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
		}

		// カラーピッカーの見た目
		ImGui::SetColorEditOptions(ImGuiColorEditFlags_Float | ImGuiColorEditFlags_PickerHueWheel);

		// ImGuiで使用するフォントの設定
		ImFont* font = io.Fonts->AddFontFromFileTTF(
			"./Asset/Fonts/corbelz.ttf", 17.0f, nullptr, io.Fonts->GetGlyphRangesJapanese()
		);
		assert(font != nullptr);

		ImGui_ImplWin32_Init(IWindow::Get().GetHwnd());
		ImGui_ImplDX11_Init(IGraphics::Get().Device(), IGraphics::Get().IC());

		// 実体への参照を登録
		IGUI::Init(this);

		return true;
	}

	void GUISystem::OnTerm()
	{
		// 実体への参照を消去
		IGUI::Release();

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void GUISystem::OnFrameBegin(const FrameEventArgs& args)
	{
		// Frame Initialize
		{
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			// ギズモ
			m_gizmo.OnFrameBegin();
		}

		// DockSpaceの設定
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			//ImGui::SetNextWindowPos(viewport->GetWorkPos());
			//ImGui::SetNextWindowSize(viewport->GetWorkSize());
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			bool p_open = true;
			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_NoDocking;
			window_flags |= ImGuiWindowFlags_NoTitleBar;
			window_flags |= ImGuiWindowFlags_NoCollapse;
			window_flags |= ImGuiWindowFlags_NoResize;
			window_flags |= ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
			window_flags |= ImGuiWindowFlags_NoNavFocus;
			window_flags |= ImGuiWindowFlags_NoBackground;
			ImGui::Begin("DockSpace", &p_open, window_flags);
			ImGuiDockNodeFlags dockspace_flags = 0;
			dockspace_flags |= ImGuiDockNodeFlags_NoDockingInCentralNode;
			dockspace_flags |= ImGuiDockNodeFlags_PassthruCentralNode;
			const ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			ImGui::End();
		}

		// ImGui::Demoの表示
		//bool show_demo = false;
		//ImGui::ShowDemoWindow(&show_demo);

		// ショートカット入力による操作
		OnInputShortcut();

		// インゲームとエディタで入力制限を設ける
		{
			if (ImGui::IsAnyMouseReleased())
			{
				// ImGuiのいずれかのWindowが選択中ならシーンに入力されない
				if (
					(ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow) && ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) ||
					(ImGui::IsAnyItemHovered() && ImGui::IsAnyItemActive())
					)
				{
					SetActiveInputScene(false);
				}
				// ImGuiを触っていなければシーン入力をアクティブにする
				else if (
					!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) &&
					!ImGui::IsAnyItemHovered()
					)
				{
					SetActiveInputScene(true);

					// マウスが消えているものは元の位置に戻す
					if (m_is_invisible_cursor)
					{
						IInput::Get().SetMouseVisible(false);
						IInput::Get().SetMousePos(m_bind_mouse_pos);
						m_is_invisible_cursor = false;
					}
				}
			}
			// エディタモードとゲームモードで透明度を変える
			if (IsActiveInputScene())
			{
				ImGuiStyle& style = ImGui::GetStyle();
				style.Colors[ImGuiCol_WindowBg] = ImVec4(0.074f, 0.074f, 0.074f, 0.500f);
			}
			else
			{
				ImGuiStyle& style = ImGui::GetStyle();
				style.Colors[ImGuiCol_WindowBg] = ImVec4(0.074f, 0.074f, 0.074f, 0.800f);
			}
		}
	}

	void GUISystem::OnFrameRender(const FrameEventArgs& args)
	{
		// GUI main update
		{
			// フラグの処理
			UpdateOnFrameFlags();

			// Gizmo
			if (m_use_scene_view)
			{
				m_gizmo.OnFrameRender();
			}

			// Main Menu Bar
			UpdateMainMenuBar();

			// Hierarchy Inspector
			UpdateDefaultGUI();
		}

		// ImGuiの描画
		RenderImGui();
	}

	void GUISystem::OnFrameEnd(const FrameEventArgs& args)
	{
		// Gizmoの変化量をTransformに適用
		m_gizmo.OnFrameEnd();

		// FrameEndで行う処理
		EditorUpdateOnFrameEnd();
	}

	void GUISystem::OnResize(const ResizeEventArgs& args)
	{
		/* nothing */
	}

	void GUISystem::OnMsgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);

		switch (msg)
		{
			case WM_DPICHANGED:
			{
				if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
				{
					const RECT* suggested_rect = (RECT*)lparam;
					SetWindowPos(hwnd, nullptr,
						suggested_rect->left, suggested_rect->top,
						suggested_rect->right - suggested_rect->left,
						suggested_rect->bottom - suggested_rect->top,
						SWP_NOZORDER | SWP_NOACTIVATE);
				}
				break;
			}
		}
	}

	bool GUISystem::HasChangedScreenMode()
	{
		const bool ret = m_has_changed_window_mode;
		m_has_changed_window_mode = false;
		return ret;
	}

	void GUISystem::SetSelectionObject(GameObjectID goid)
	{
		m_selecting_objects.clear();
		m_selecting_objects.emplace_back(goid);
	}

	void GUISystem::AddSelectionObject(GameObjectID goid)
	{
		m_selecting_objects.emplace_back(goid);
	}

	void GUISystem::RemoveSelectionObject(GameObjectID goid)
	{
		const size_t size = m_selecting_objects.size();
		if (size == 0)
			return;

		for (size_t i = 0; i < size; ++i)
		{
			if (m_selecting_objects.at(i) == goid)
			{
				m_selecting_objects.at(i) = m_selecting_objects.back();
				m_selecting_objects.pop_back();

				break;
			}
		}
	}

	void GUISystem::ClearSelectionObject()
	{
		m_selecting_objects.clear();
	}

	const std::vector<GameObjectID>& GUISystem::GetSelectionObjects() const
	{
		return m_selecting_objects;
	}

	bool GUISystem::IsSelecting(GameObjectID goid) const
	{
		const size_t size = m_selecting_objects.size();
		if (size == 0)
			return false;

		for (size_t i = 0; i < size; ++i)
		{
			if (m_selecting_objects.at(i) == goid)
				return true;
		}

		return false;
	}

	void GUISystem::SetRightClickObject(GameObjectID goid)
	{
		m_right_click_object = goid;
	}

	void GUISystem::ClearRightClickObject()
	{
		m_right_click_object = INVALID_ID;
	}

	GameObjectID GUISystem::GetRightClickObject() const
	{
		return m_right_click_object;
	}

	void GUISystem::DrawLine(const VECTOR3& from, const VECTOR3& to, const VECTOR3& from_color, const VECTOR3& to_color)
	{
		PhysicsSystem::GetInstance()->DrawLine(from, to, from_color, to_color);
	}

	void GUISystem::DrawSphere(const VECTOR3& pos, const float radius, const VECTOR3& color)
	{
		PhysicsSystem::GetInstance()->DrawSphere(pos, radius, color);
	}

	void GUISystem::DrawBox(const VECTOR3& pos, const Quaternion& rot, const VECTOR3& half_extents, const VECTOR3& color)
	{
		PhysicsSystem::GetInstance()->DrawBox(pos, rot, half_extents, color);
	}

	void GUISystem::DrawCapsule(const VECTOR3& pos, const Quaternion& rot, const float radius, const float height, const VECTOR3& color)
	{
		PhysicsSystem::GetInstance()->DrawCapsule(pos, rot, radius, height, color);
	}

	void GUISystem::DrawCone(const VECTOR3& pos, const Quaternion& rot, const float radius, const float height, const VECTOR3& color)
	{
		PhysicsSystem::GetInstance()->DrawCone(pos, rot, radius, height, color);
	}

	void GUISystem::DrawContactPoint(const VECTOR3& pos, const VECTOR3& normal, const float distance, const VECTOR3& color)
	{
		PhysicsSystem::GetInstance()->DrawContactPoint(pos, normal, distance, color);
	}

	void GUISystem::EnterControlTransform()
	{
		m_gizmo.EnterControlTransform();
	}

	void GUISystem::ExitControlTransform()
	{
		m_gizmo.ExitControlTransform();
	}

	bool GUISystem::IsPlaying() const
	{
		return m_playing_scene;
	}

	bool GUISystem::IsPlayedOnFrame() const
	{
		return m_played_scene_on_frame;
	}

	bool GUISystem::IsStoppedOnFrame() const
	{
		return m_stopped_scene_on_frame;
	}

	bool GUISystem::IsSceneView() const
	{
		return m_use_scene_view;
	}

	bool GUISystem::DoDrawDebugExtent() const
	{
		return m_do_render_debug_extent;
	}

	bool GUISystem::DoDrawUI() const
	{
		return m_do_render_UI;
	}

	void GUISystem::SetDrawController(const GameObjectID goid, const AnimatorController& controller, const Avatar& avatar)
	{
		m_has_controller_goid_ref = goid;
		m_controller_ref = controller;
		m_avatar_ref = avatar;
		m_do_render_controller_window = true;
	}

	bool GUISystem::IsActiveInputScene() const
	{
		return m_is_active_input_scene;
	}

	void GUISystem::SetActiveInputScene(const bool is_active)
	{
		m_is_active_input_scene = is_active;
	}

	bool GUISystem::IsPushSaveGlobalSetting() const
	{
		return m_is_push_save_global_setting;
	}

	void GUISystem::RenderImGui()
	{
		m_viewport.Width = static_cast<FLOAT>(IWindow::Get().GetScreenWidth());
		m_viewport.Height = static_cast<FLOAT>(IWindow::Get().GetScreenHeight());
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
		m_viewport.TopLeftX = 0.0f;//top left X
		m_viewport.TopLeftY = 0.0f;//top left Y
		IGraphics::Get().IC()->RSSetViewports(1, &m_viewport);
		IGraphics::Get().SetRTBackBuffer(IGraphics::Get().IC(), false);
		ImGui::Render();
		//g_systems.Graphics()->ClearBackBuffer();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void GUISystem::UpdateMainMenuBar()
	{
		// Main menu bar
		if (ImGui::BeginMainMenuBar())
		{
			// Play/Stop
			InMainMenuBarGamePlay();

			// ファイル関係
			InMainMenuBarFile();

			// 設定関係
			InMainMenuBarSetting();

			// 操作説明関係
			InMainMenuBarInfo();

			// ウィンドウ表示関係
			InMainMenuBarWindow();

			ImGui::Spacing();
			// シーンビュー選択
			InMainMenuBarFlags();

			ImGui::EndMainMenuBar();
		}

	}
	void GUISystem::UpdateDefaultGUI()
	{
		// Hierarchy
		ImGui::Begin("Hierarchy");
		{
			if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootWindow) && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
			{
				m_is_active_right_click_option_window = true;
			}
		}
		ImGui::End();

		// Inspector
		ImGui::Begin("Inspector");
		ImGui::End();

		// Animator Controller
		if (m_do_render_controller_window && GameObjectManager::GetGameObject(m_has_controller_goid_ref) == nullptr)
		{
			// アニメーションコントローラの表示時にコントローラの参照が存在しないならリセット
			m_has_controller_goid_ref = 0;
			m_do_render_controller_window = false;
		}
		// コントローラの参照があり、表示フラグがTrue
		if (m_do_render_controller_window)
		{
			if (ImGui::Begin("Animator Controller", &m_do_render_controller_window, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking))
			{
				if (m_controller_ref.HasController() && m_avatar_ref.HasAvatar())
				{
					const auto* bone_names = m_avatar_ref.GetAlignedBoneNames();
					if (bone_names)
						m_controller_ref.OnGUI(m_has_controller_goid_ref, bone_names);
				}
			}
			ImGui::End();
		}

		// 選択中のオブジェクトのGUI表示
		SceneManager::OnGUI();

		// 右クリック時のオプションウィンドウ表示
		if (m_is_active_right_click_option_window)
		{
			OnGUIHierarchyRightClick();
		}
		// 選択中のオブジェクトの状態をチェック
		if (!m_selecting_objects.empty())
		{
			// 削除されているオブジェクトならリストをクリアする
			const auto go = GameObjectManager::GetGameObject(m_selecting_objects.at(0));
			if (!go)
			{
				m_selecting_objects.clear();
			}
		}
		if (m_right_click_object != INVALID_ID)
		{
			// 削除されているオブジェクトならリストをクリアする
			const auto go = GameObjectManager::GetGameObject(m_right_click_object);
			if (!go)
			{
				m_right_click_object = INVALID_ID;
			}
		}

		// 初回のみ実行、ヒエラルキーウィンドウにフォーカス
		if (static bool set_focus_on_beginning = false; !set_focus_on_beginning)
		{
			set_focus_on_beginning = true;
			ImGui::SetWindowFocus("Hierarchy");
		}
	}

	void GUISystem::OnInputShortcut()
	{
		// インゲーム中にF1でエディタをさわれるようにする
		if (ImGui::IsKeyDown(VK_F1) && IsActiveInputScene())
		{
			// マウスが見えているならエディタモードにするだけ
			if (IInput::Get().GetMouseVisible())
			{
				m_is_invisible_cursor = false;
				SetActiveInputScene(false);
			}
			// マウスが見えないなら位置を戻せるようにする
			else
			{
				IInput::Get().SetMouseVisible(true);
				m_is_invisible_cursor = true;
				m_bind_mouse_pos = IInput::Get().GetMousePos();
				SetActiveInputScene(false);
			}
		}

		// 選択中のオブジェクトに対する操作

		// エディタにフォーカスしているときのみ有効な処理
		if (!IsActiveInputScene() && !m_selecting_objects.empty())
		{
			// エディタモードかつ選択状態のオブジェクトが存在するとき、ctrl+Dで複製する
			const auto& im_io = ImGui::GetIO();
			if (im_io.KeyCtrl && ImGui::IsKeyPressed('D', false))
			{
				const auto created_object = IGameObject::Duplicate(GameObjectManager::GetGameObject(m_selecting_objects.at(0)));
				m_selecting_objects.clear();
				m_selecting_objects.emplace_back(created_object->GetGameObjectID());
			}

			// Deleteキーで0番の選択中オブジェクトを削除する
			{
				static bool push_delete_key = false;
				if (ImGui::IsKeyPressed(VK_DELETE, false))
				{
					push_delete_key = true;
				}
				// 削除確認用ウィンドウの表示
				if (push_delete_key)
				{
					ImGui::PushID(this);
					ImGui::SetNextWindowPos(ImGui::GetMousePos(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
					ImGui::SetNextWindowSize(ImVec2());
					if (ImGui::Begin("##On push delete key", nullptr, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration))
					{
						ImGui::Text("Delete GameObject : ");
						ImGui::SameLine();
						ImGui::Text(GameObjectManager::GetGameObject(m_selecting_objects.at(0))->GetName());
						if (ImGui::Button("Delete"))
						{
							// 選択中のオブジェクトを削除
							OnDeleteSelectingObject();
							push_delete_key = false;
						}
						ImGui::SameLine();
						if(ImGui::Button("Cancel"))
						{
							push_delete_key = false;
						}
					}
					ImGui::End();
					ImGui::PopID();
				}
			}
		}
		// エディタにフォーカスしなくても実行される処理
		if (!m_selecting_objects.empty())
		{
			// 選択中のオブジェクトが存在するときctrl + Fでフォーカスする
			const auto& im_io = ImGui::GetIO();
			if (im_io.KeyCtrl && ImGui::IsKeyPressed('F', false))
			{
				SetFocusOnCtrlFKey(m_selecting_objects.at(0));
			}
		}
	}

	void GUISystem::EditorUpdateOnFrameEnd()
	{
		/* nothing */
	}

	void GUISystem::UpdateOnFrameFlags()
	{
		// FBX Exporter
		if (m_active_export_fbx)
		{
			if (FBXLoader::LoadFromFile())
			{
				m_active_export_fbx = false;
			}
		}

		// Global Settingをセーブするボタンが押されたフラグのリセット
		m_is_push_save_global_setting = false;

		// Play/Stopボタンを押した瞬間のフラグをリセット
		m_played_scene_on_frame = false;
		m_stopped_scene_on_frame = false;
	}

	void GUISystem::SetFocusOnCtrlFKey(GameObjectID goid)
	{
		// フォーカスされたオブジェクトの存在チェック
		const auto object = GameObjectManager::GetGameObject(goid);
		if (!object)
		{
			return;
		}
		// 現在のデバッグカメラのView行列から平行移動量だけを変化させる
		const auto debug_camera = CameraManager::GetDebugCamera();
		// カメラの正面方向10mの位置に来るようにする
		constexpr float focus_distance = 10.0f;
		const VECTOR3 forward = VECTOR3::Forward * debug_camera->GetWorldRotation();
		const VECTOR3 new_pos = object->GetTransform()->Position() - focus_distance * forward;
		CameraManager::SetSceneViewPosition(new_pos);
	}

	void GUISystem::InMainMenuBarGamePlay()
	{
		// Play / Stop
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.6f));
		if (!m_playing_scene)
		{
			if (ImGui::MenuItem("Play##Play Scene"))
			{
				m_playing_scene = true;
				m_played_scene_on_frame = true;
				// MenuBarの色を実行中の色にする
				auto* const colors = ImGui::GetStyle().Colors;
				colors[ImGuiCol_MenuBarBg] = ImVec4(0.3f, 0.08f, 0.08f, 0.9f);
			}
		}
		else
		{
			if (ImGui::MenuItem("Stop##Stop Scene"))
			{
				m_playing_scene = false;
				m_stopped_scene_on_frame = true;
				// MenuBarの色をエディット中の色にする
				auto* const colors = ImGui::GetStyle().Colors;
				colors[ImGuiCol_MenuBarBg] = ImVec4(0.055f, 0.055f, 0.055f, 0.900f);
			}
		}
		ImGui::PopStyleColor(2);
		ImGui::Dummy(ImVec2(20.0f, 0.0f));
	}

	void GUISystem::InMainMenuBarFile()
	{
		// Fileメニュー
		if (ImGui::BeginMenu("File"))
		{
			// FBXエクスポート
			InMenuItemExport();

			ImGui::EndMenu();
		}
		ImGui::Spacing();
	}

	void GUISystem::InMainMenuBarSetting()
	{
		// 設定メニュー
		if (ImGui::BeginMenu("Setting"))
		{
			// システムセッティング
			InMenuItemSetting();

			ImGui::EndMenu();
		}
		ImGui::Spacing();

		// ウィンドウ表示
		PhysicsSystem::GetInstance()->OnGUILayer();
	}

	void GUISystem::InMainMenuBarInfo()
	{
		// 操作説明メニュー
		if (ImGui::BeginMenu("Info"))
		{
			// インゲーム状態からマウスを切り離す
			{
				ImGui::MenuItem("Use mouse in editor", "F1");
				ImGui::HelpText("You can find and controll cursor,\nif cursor is hidden and binding.");
			}
			// フルスクリーン <--> ウィンドウモード
			{
				//m_has_changed_window_mode = false;
				if (ImGui::MenuItem("Change Screen mode", "F5"))
				{
					m_has_changed_window_mode = true;
				}
				ImGui::HelpText("Change screen mode.\nFullScreen <--> WindowMode");
			}
			// 複製
			{
				if (ImGui::MenuItem("Duplicate GameObject", "ctrl + D"))
				{
					if (!IsActiveInputScene() && !m_selecting_objects.empty())
					{
						const auto created_object = IGameObject::Duplicate(GameObjectManager::GetGameObject(m_selecting_objects.at(0)));
						m_selecting_objects.clear();
						m_selecting_objects.emplace_back(created_object->GetGameObjectID());
					}
				}
			}
			// フォーカスする
			{
				if (ImGui::MenuItem("Focus GameObject", "ctrl + F"))
				{
					if (!m_selecting_objects.empty())
					{
						SetFocusOnCtrlFKey(m_selecting_objects.at(0));
					}
				}
			}
			// Delete
			{
				if (ImGui::MenuItem("Delete GameObject", "Delete"))
				{
					if (!IsActiveInputScene() && !m_selecting_objects.empty())
					{
						// 選択中のオブジェクトを削除
						OnDeleteSelectingObject();
					}
				}
			}

			ImGui::EndMenu();
		}
	}

	void GUISystem::InMainMenuBarWindow()
	{
		// ウィンドウ表示メニュー
		if (ImGui::BeginMenu("Window"))
		{
			// コントローラ用ウィンドウを表示する
			{
				if (ImGui::MenuItem("Draw Animator Controller", "", m_do_render_controller_window))
				{
					m_do_render_controller_window = !m_do_render_controller_window;
				}
			}

			ImGui::EndMenu();
		}
	}

	void GUISystem::InMenuItemExport()
	{
		// FBXエクスポート
		if (ImGui::MenuItem("FBX / OBJ"))
		{
			m_active_export_fbx = true;
		}
	}

	void GUISystem::InMenuItemSetting()
	{
		// グローバルセッティング
		if (ImGui::BeginMenu("Global Setting"))
		{
			// Save
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
			if (ImGui::MenuItem("Save##Global Setting"))
			{
				m_is_push_save_global_setting = true;

			}
			ImGui::PopStyleColor();
			// Physics
			if (ImGui::BeginMenu("Physics"))
			{
				// Collision Layer mask
				if (ImGui::MenuItem("Collision Layer mask"))
				{
					PhysicsSystem::GetInstance()->OpenLayer();
				}
				// World Gravity
				if (ImGui::BeginMenu("Set World Gravity"))
				{
					VECTOR3 current_gravity = PhysicsSystem::GetInstance()->GetWorldGravity();
					if (ImGui::InputFloat3("##World Gravity", &current_gravity.x, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue))
					{
						PhysicsSystem::GetInstance()->SetWorldGravity(current_gravity);
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			// Light
			if (ImGui::MenuItem("Light Setting TODO : No Create!"))
			{
				// TODO : Shadow,Light setting
			}
			ImGui::EndMenu();
		}
	}

	void GUISystem::InMainMenuBarFlags()
	{
		// シーンビュー or ゲームビュー
		ImGui::Checkbox("Scene View", &m_use_scene_view);

		// ワイヤーフレームによるデバッグ描画
		ImGui::Checkbox("Draw Extent", &m_do_render_debug_extent);

		// デバッグカメラのUI描画
		ImGui::Checkbox("Draw UI", &m_do_render_UI);
	}

	void TKGEngine::GUI::GUISystem::OnGUIHierarchyRightClick()
	{
		ImGui::PushID("HierarchyRightClick");

		// 現在のマウス位置にウィンドウ表示
		const ImVec2 window_size(180.0f, 340.0f);
		const ImVec2 mouse_pos = ImGui::GetMousePos();
		const ImVec2 window_pos = ImVec2(mouse_pos.x - window_size.x * 0.5f, mouse_pos.y);
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(window_size, ImGuiCond_Appearing);
		const ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration ^ ImGuiWindowFlags_NoTitleBar;
		const ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_SpanAvailWidth;
		ImGui::Begin("Option##Hierarchy Option Window", &m_is_active_right_click_option_window, flags);
		{
			// 入力用バッファ
			constexpr size_t input_str_len = 256;
			static char input_str[input_str_len] = "\0";

			// 選択中のオブジェクトがあるか
			const bool is_selecting = !m_selecting_objects.empty();
			// 右クリックがオブジェクトを選択してされたか
			const bool is_right_click_selected = (m_right_click_object != INVALID_ID);
			// 右クリックオブジェクトと選択中のオブジェクトが同一かどうか
			const bool is_same_select_object = is_selecting ? (m_selecting_objects.at(0) == m_right_click_object) : false;
			// テキスト入力中はクリックで閉じないようにするため
			bool is_clicked_on_textbox = false;

			// 選択中のオブジェクト名表示
			std::string select_object_name;
			{
				// 左クリック
				ImGui::Text("L:");
				ImGui::SameLine();
				if (is_selecting)
				{
					select_object_name = GameObjectManager::GetGameObject(m_selecting_objects.at(0))->GetName();
					ImGui::Text(select_object_name.c_str());
				}
				else
				{
					ImGui::TextDisabled("No Select Object");
				}
				// 右クリック
				ImGui::Text("R:");
				if (is_right_click_selected)
				{
					ImGui::SameLine();
					ImGui::Text(GameObjectManager::GetGameObject(m_right_click_object)->GetName());
				}
			}

			// Hierarchy
			{
				ImGui::CollapsingHeader("Hierarchy", tree_flags);
				{
					// 選択中のオブジェクトをルートにする用メニュー
					if (ImGui::BeginMenu("Set Root", is_selecting))
					{
						if (ImGui::MenuItem("Root"))
						{
							GameObjectManager::GetGameObject(m_selecting_objects.at(0))->GetTransform()->SetParent(true);
							// ウィンドウを閉じる
							m_is_active_right_click_option_window = false;
						}
						ImGui::EndMenu();
					}
					ImGui::HelpText("Set selection object set to root object.");

					// 選択中のオブジェクトを右クリックした先のオブジェクトの子にする
					if (ImGui::BeginMenu("Set Parent", is_right_click_selected && is_selecting && !is_same_select_object))
					{
						// 右クリックオブジェクト
						const auto right_click_object = GameObjectManager::GetGameObject(m_right_click_object);
						const auto right_click_transform = right_click_object->GetTransform();
						// 選択中のオブジェクト
						const auto selecting_object = GameObjectManager::GetGameObject(m_selecting_objects.at(0));
						const auto selecting_transform = selecting_object->GetTransform();

						// 親と子の循環が発生しないように右クリックオブジェクトのヒエラルキーに、選択中のオブジェクトが存在しないことをチェック
						bool is_valid = false;
						{
							// 右クリックオブジェクトに親がなければ
							if (right_click_transform->GetParent() == nullptr)
							{
								is_valid = true;
							}
							// 再帰的に親が選択中のオブジェクトと一致するかチェックする関数オブジェクト
							static std::function<void(const std::shared_ptr<Transform>&)> recurse = [&](const std::shared_ptr<Transform>& parent)
							{
								const auto& grand_parent = parent->GetParent();
								// 親が選択中のオブジェクトかチェック
								if (grand_parent == selecting_transform)
								{
									is_valid = false;
									return;
								}
								// 親を持たないなら再帰終了
								if (grand_parent == nullptr)
								{
									is_valid = true;
									return;
								}
								// 再帰的に検索する
								recurse(grand_parent);
							};
							if (!is_valid)
							{
								recurse(right_click_transform);
							}
						}
						if (ImGui::MenuItem(right_click_object->GetName(), nullptr, false, is_valid))
						{
							if (selecting_object)
							{
								selecting_object->GetTransform()->SetParent(GameObjectManager::GetGameObject(m_right_click_object)->GetTransform(), true);
							}
							// ウィンドウを閉じる
							m_is_active_right_click_option_window = false;
						}

						ImGui::EndMenu();
					}
					ImGui::HelpText("Set selection object set to right click object child.");
				}
			}

			// Create
#pragma region create object
			{
				ImGui::CollapsingHeader("Create Object", tree_flags);
				{
					// Root用
					if (ImGui::BeginMenu("Root"))
					{
						// 呼び出されている間はウィンドウ上でクリックできるようにする
						is_clicked_on_textbox = true;
						OnRightClickCreateFunction(true, input_str_len, input_str);
						ImGui::EndMenu();
					}
					// Child用
					if (ImGui::BeginMenu("Child", is_right_click_selected))
					{
						// 呼び出されている間はウィンドウ上でクリックできるようにする
						is_clicked_on_textbox = true;
						OnRightClickCreateFunction(false, input_str_len, input_str);
						ImGui::EndMenu();
					}
				}
			}
#pragma endregion

			// Save Load
			{
				ImGui::CollapsingHeader("Save & Load", tree_flags);
				// オブジェクトセーブ用メニュー
				if (ImGui::BeginMenu("Save Object", is_selecting))
				{
					if (ImGui::MenuItem(select_object_name.c_str()))
					{
						GameObjectManager::SaveGameObject(m_selecting_objects.at(0));
						// ウィンドウを閉じる
						m_is_active_right_click_option_window = false;
					}
					ImGui::EndMenu();
				}
				// オブジェクトロード用メニュー
				if (ImGui::BeginMenu("Load Object"))
				{
					if (ImGui::MenuItem("Load"))
					{
						// ロードするオブジェクトのファイル名を取得
						std::string filepath;
						if (MyFunc::FetchOpenFileName(filepath, TEXT("GameObject(*.object)\0*.object\0"), TEXT("Load GameObject"), TEXT(".\\Asset")))
						{
							// ロードする
							const auto created_object = GameObjectManager::LoadGameObject(filepath);
							// DebugカメラからON_CREATE_DISTANCE離れた位置
							const auto debug_camera = CameraManager::GetDebugCamera();
							const VECTOR3 create_pos = VECTOR3::Forward * debug_camera->GetWorldRotation() * ONCREATE_DISTANCE + debug_camera->GetWorldPosition();
							created_object->GetTransform()->Position(create_pos);
							// 作成したオブジェクトを選択状態にする
							m_selecting_objects.clear();
							m_selecting_objects.emplace_back(created_object->GetGameObjectID());
						}
						// ウィンドウを閉じる
						m_is_active_right_click_option_window = false;
					}
					ImGui::EndMenu();
				}
			}

			// Delete
			{
				ImGui::CollapsingHeader("Delete", tree_flags);
				{
					// オブジェクトを削除用メニュー
					if (ImGui::BeginMenu("Destroy Object", is_selecting))
					{
						if (ImGui::BeginMenu(select_object_name.c_str()))
						{
							// Delete
							if (ImGui::MenuItem("Delete"))
							{
								// 選択中のオブジェクトを削除
								OnDeleteSelectingObject();
								// ウィンドウを閉じる
								m_is_active_right_click_option_window = false;
							}
							// Cancel
							if (ImGui::MenuItem("Cancel"))
							{
								// ウィンドウを閉じる
								m_is_active_right_click_option_window = false;
							}

							ImGui::EndMenu();
						}
						ImGui::EndMenu();
					}
					ImGui::HelpText("Destroy GameObject.");
				}
			}

			// ウィンドウ外で左クリックしたら消える、入力ボックス上は無視
			if (!ImGui::IsWindowAppearing() &&
				!ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows) &&
				ImGui::IsAnyMouseReleased() &&
				!is_clicked_on_textbox
				)
			{
				// 入力バッファの初期化
				input_str[0] = '\0';
				// ウィンドウを閉じる
				m_is_active_right_click_option_window = false;
			}
		}
		ImGui::End();

		ImGui::PopID();
	}

	void GUISystem::OnRightClickCreateFunction(bool is_root, const size_t str_size, char* str)
	{
		// 文字列入力用ラムダ式
		auto name_input_func = [&](const char* label)->bool
		{

			if (ImGui::InputText(label, str, str_size, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				return true;
			}
			return false;
		};

		// 作成されたオブジェクトを入れるポインタ
		std::shared_ptr<IGameObject> created_object = nullptr;

		// 空ゲームオブジェクト作成
		if (ImGui::BeginMenu("Empty Object"))
		{
			ImGui::Text("New Object Name");
			if (name_input_func(""))
			{
				// 空オブジェクトを作成
				if (is_root)
				{
					created_object = GameObjectManager::CreateGameObject(str);
				}
				else
				{
					created_object = GameObjectManager::CreateChild(GameObjectManager::GetGameObject(m_right_click_object), str);
				}
				// 入力バッファの初期化
				str[0] = '\0';
				// ウィンドウを閉じる
				m_is_active_right_click_option_window = false;
			}
			ImGui::EndMenu();
		}

		// プリミティブ作成
#pragma region Create Primitive
		if (ImGui::BeginMenu("Primitive"))
		{
			if (ImGui::BeginMenu("Cube"))
			{
				ImGui::Text("New Cube Name");
				if (name_input_func(""))
				{
					// 空オブジェクトを作成
					if (is_root)
					{
						created_object = GameObjectManager::CreateCube(str);
					}
					else
					{
						created_object = GameObjectManager::CreateCube(GameObjectManager::GetGameObject(m_right_click_object), str);
					}
					// 入力バッファの初期化
					str[0] = '\0';
					// ウィンドウを閉じる
					m_is_active_right_click_option_window = false;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Sphere (Smooth)"))
			{
				ImGui::Text("New Sphere(Smooth) Name");
				if (name_input_func(""))
				{
					// オブジェクトを作成
					if (is_root)
					{
						created_object = GameObjectManager::CreateSphereSmooth(str);
					}
					else
					{
						created_object = GameObjectManager::CreateSphereSmooth(GameObjectManager::GetGameObject(m_right_click_object), str);
					}
					// 入力バッファの初期化
					str[0] = '\0';
					// ウィンドウを閉じる
					m_is_active_right_click_option_window = false;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Sphere (Flat)"))
			{
				ImGui::Text("New Sphere(Flat) Name");
				if (name_input_func(""))
				{
					// オブジェクトを作成
					if (is_root)
					{
						created_object = GameObjectManager::CreateSphereFlat(str);
					}
					else
					{
						created_object = GameObjectManager::CreateSphereFlat(GameObjectManager::GetGameObject(m_right_click_object), str);
					}
					// 入力バッファの初期化
					str[0] = '\0';
					// ウィンドウを閉じる
					m_is_active_right_click_option_window = false;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Capsule"))
			{
				ImGui::Text("New Capsule Name");
				if (name_input_func(""))
				{
					// オブジェクトを作成
					if (is_root)
					{
						created_object = GameObjectManager::CreateCapsule(str);
					}
					else
					{
						created_object = GameObjectManager::CreateCapsule(GameObjectManager::GetGameObject(m_right_click_object), str);
					}
					// 入力バッファの初期化
					str[0] = '\0';
					// ウィンドウを閉じる
					m_is_active_right_click_option_window = false;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Cylinder"))
			{
				ImGui::Text("New Cylinder Name");
				if (name_input_func(""))
				{
					// オブジェクトを作成
					if (is_root)
					{
						created_object = GameObjectManager::CreateCylinder(str);
					}
					else
					{
						created_object = GameObjectManager::CreateCylinder(GameObjectManager::GetGameObject(m_right_click_object), str);
					}
					// 入力バッファの初期化
					str[0] = '\0';
					// ウィンドウを閉じる
					m_is_active_right_click_option_window = false;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Plane"))
			{
				ImGui::Text("New Plane Name");
				if (name_input_func(""))
				{
					// オブジェクトを作成
					if (is_root)
					{
						created_object = GameObjectManager::CreatePlane(str);
					}
					else
					{
						created_object = GameObjectManager::CreatePlane(GameObjectManager::GetGameObject(m_right_click_object), str);
					}
					// 入力バッファの初期化
					str[0] = '\0';
					// ウィンドウを閉じる
					m_is_active_right_click_option_window = false;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
#pragma endregion
		// ~Create Primitive

		// Light作成
#pragma region Create Light
		if (ImGui::BeginMenu("Light"))
		{
			LightType light_type = LightType::Point;
			// Light作成ラムダ式
			auto create_light_func = [&](const char* label) {
				if (ImGui::BeginMenu(label))
				{
					ImGui::Text("New Object Name");
					if (name_input_func(""))
					{
						// 空オブジェクトを作成
						if (is_root)
						{
							created_object = GameObjectManager::CreateGameObject(str);
						}
						else
						{
							created_object = GameObjectManager::CreateChild(GameObjectManager::GetGameObject(m_right_click_object), str);
						}
						// Lightの設定
						const std::string light_name = label;
						if (light_name == "Directional")
							light_type = LightType::Directional;
						else if (light_name == "Spot")
							light_type = LightType::Spot;
						else
							light_type = LightType::Point;

						// 入力バッファの初期化
						str[0] = '\0';
						// ウィンドウを閉じる
						m_is_active_right_click_option_window = false;
					}
					ImGui::EndMenu();
				}
			};
			// Directional
			create_light_func("Directional");
			// Spot
			create_light_func("Spot");
			// Point
			create_light_func("Point");

			// 作成されていたらコンポーネントをアタッチする
			if (created_object)
			{
				const auto light = created_object->AddComponent<Light>();
				light->SetType(light_type);
			}

			ImGui::EndMenu();
		}
#pragma endregion
		// ~Create Light

		// Camera作成
		if (ImGui::BeginMenu("Camera"))
		{
			ImGui::Text("New Object Name");
			if (name_input_func(""))
			{
				// 空オブジェクトを作成
				if (is_root)
				{
					created_object = GameObjectManager::CreateGameObject(str);
				}
				else
				{
					created_object = GameObjectManager::CreateChild(GameObjectManager::GetGameObject(m_right_click_object), str);
				}
				// コンポーネントをアタッチする
				auto camera = created_object->AddComponent<Camera>();
				// 入力バッファの初期化
				str[0] = '\0';
				// ウィンドウを閉じる
				m_is_active_right_click_option_window = false;
			}
			ImGui::EndMenu();
		}

		// 作成されていたら視界に映るように移動して選択状態にする
		if (created_object)
		{
			// DebugカメラからONCREATE_DISTANCE離れた位置
			const auto debug_camera = CameraManager::GetDebugCamera();
			const VECTOR3 create_pos = VECTOR3::Forward * debug_camera->GetWorldRotation() * ONCREATE_DISTANCE + debug_camera->GetWorldPosition();
			created_object->GetTransform()->Position(create_pos);
			// 作成したオブジェクトを選択状態にする
			m_selecting_objects.clear();
			m_selecting_objects.emplace_back(created_object->GetGameObjectID());
		}
	}

	void GUISystem::OnDeleteSelectingObject()
	{
		// 選択中のオブジェクトを削除
		GameObjectManager::GetGameObject(m_selecting_objects.at(0))->Destroy();
		// 選択中のオブジェクトリストを空にする
		ClearSelectionObject();
		ClearRightClickObject();
	}

}	// namespace TKGEngine::GUI
#endif// #ifdef USE_IMGUI