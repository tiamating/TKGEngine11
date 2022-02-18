
#include "../inc/IApp.h"

#include "Systems/inc/IWindow.h"
#include "Systems/inc/ITime.h"
#include "Systems/inc/IWindowSystem.h"
#include "Systems/inc/ITimeSystem.h"
#include "Systems/inc/LogSystem.h"

#include "Utility/inc/myfunc_math.h"

#include "Application/inc/SystemSetting.h"

#include <sstream>
#include <cassert>

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Internal linkage Global variable
	////////////////////////////////////////////////////////
	namespace /* anonymous */
	{
		IApp* gp_app = nullptr;

		bool g_wnd_size_changing = false;
		unsigned g_wnd_prev_size_width = 0;
		unsigned g_wnd_prev_size_height = 0;
		unsigned g_wnd_size_width = 0;
		unsigned g_wnd_size_height = 0;
	}	// /* anonymous */

	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	IApp::IApp(
		LPCWSTR title,
		HICON hIcon,
		HMENU hMenu
	)
		: m_window_setup_data(title, SystemSetting::WINDOW_WIDTH, SystemSetting::WINDOW_HEIGHT, hIcon, hMenu)
	{
		g_wnd_prev_size_width = g_wnd_size_width = SystemSetting::WINDOW_WIDTH;
		g_wnd_prev_size_height = g_wnd_size_height = SystemSetting::WINDOW_HEIGHT;
	}

	IApp::~IApp()
	{
		/* nothing */
	}

	int IApp::Run()
	{
		if (InitApp())
		{
			MainLoop();
		}
		TermApp();
#ifdef USE_DEBUGGER
		OnCheckLeak();
#endif// USE_DEBUGGER
		return 0;
	}

	void IApp::MainLoop()
	{
		MSG msg = {};
		FrameEventArgs args;

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				////////////////////////// Frame Update /////////////////////////
				// Check WM_SIZE message
				OnWndSizeChanging();
				// Reset window resize flag
				OnWndFrameEnd();

				// Update Time System
				{
					OnFrameTimeUpdate(args);
					const float fps = static_cast<float>(time_system->GetFPS());
					const float mspf = 1000.0f / fps;
					std::ostringstream outs;
					outs.precision(6);
					outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
					SetWindowTextA(IWindow::Get().GetHwnd(), outs.str().c_str());

				}
				args.is_stop_draw = m_is_stop_draw;
				/////////////////////////////////////////////////////////////////

				OnFrameBegin(args);
				OnFrameRender(args);
				OnFrameEnd(args);
				
				args.frame_idx = 1 - args.frame_idx;
			}
		}
	}

	bool IApp::InitApp()
	{
		// COMライブラリの初期化
		HRESULT hr = CoInitialize(nullptr);
		if (FAILED(hr))
		{
			assert(0 && "failed Com Library initialize IApp::InitApp()");
			return false;
		}

		// COMライブラリのセキュリティレベルを設定する
		hr = CoInitializeSecurity(
			nullptr,
			-1,
			nullptr,
			nullptr,
			RPC_C_AUTHN_LEVEL_DEFAULT,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			nullptr,
			EOAC_NONE,
			nullptr
		);
		if (FAILED(hr))
		{
			assert(0 && "failed Com Library initialize security level IApp::InitApp()");
			return false;
		}

		// Set Global pointer
		gp_app = this;

		// Initialize Window System
		window_system = Window::IWindowSystem::CreateInterface();
		window_system->SetWndParam(
			m_window_setup_data.title,
			m_window_setup_data.width, m_window_setup_data.height,
			m_window_setup_data.hIcon, m_window_setup_data.hMenu,
			WinProc);
		if (!InitWnd())
		{
			assert(0 && "failed initialize Window System IApp::InitApp()");
			return false;
		}

		// Initialize Time System
		time_system = Time::ITimeSystem::CreateInterface();
		if (!time_system->OnInit())
		{
			assert(0 && "failed initialize ITimeSystem::OnInit() IApp::InitApp()");
			return false;
		}
		time_system->Start();

		// Initialize Application Systems
		if (!OnInit())
		{
			assert(0 && "failed initialize Application Systems IApp::InitApp()");
			return false;
		}

		return true;
	}

	void IApp::TermApp()
	{
		// Terminate Application Systems
		OnTerm();

		if (time_system)
		{
			time_system->Stop();
			// Terminate Time System
			time_system->OnTerm();
		}

		// Terminate Window System
		TermWnd();

		// Remove Global pointer
		gp_app = nullptr;

		// COMライブラリの終了
		CoUninitialize();
	}

	bool IApp::InitWnd()
	{
		if (!window_system->OnInit())
		{
			assert(0 && "failed WindowSystem::OnInit()");
			return false;
		}
		return true;
	}

	void IApp::TermWnd()
	{
		if (window_system)
		{
			window_system->OnTerm();
		}
	}

	void IApp::OnWndSizeChanging()
	{
		static bool s_has_changed = false;
		// Windowのサイズ変更を変更し終えたフレームを取る
		if (!g_wnd_size_changing && s_has_changed)
		{
			// ウィンドウサイズが変更されていたらリサイズイベントを実行する
			if (g_wnd_prev_size_width != g_wnd_size_width || g_wnd_prev_size_height != g_wnd_size_height)
			{
				ResizeEventArgs args;
				args.width = g_wnd_size_width;
				args.height = g_wnd_size_height;
				g_wnd_prev_size_width = g_wnd_size_width;
				g_wnd_prev_size_height = g_wnd_size_height;
				window_system->OnResize(args);
				gp_app->DoResize(args);
			}
			g_wnd_size_changing = false;
		}
		s_has_changed = g_wnd_size_changing;
	}

	void IApp::OnWndFrameEnd()
	{
		g_wnd_size_changing = false;
	}

	void IApp::OnFrameTimeUpdate(FrameEventArgs& args)
	{
		// FPSロック時はループで待機
		while (!time_system->Tick()) {};

		time_system->CalcFramePerSec();

		args.delta_time = ITime::Get().DeltaTime();
		args.unscaled_delta_time = ITime::Get().UnscaledDeltaTime();
		args.total_time = ITime::Get().TotalTime();
		args.unscaled_total_time = ITime::Get().UnscaledTotalTime();
	}

	void IApp::StopDraw(const bool is_stop_draw)
	{
		m_is_stop_draw = is_stop_draw;
	}

	bool IApp::IsStopDraw() const
	{
		return m_is_stop_draw;
	}

	LRESULT CALLBACK IApp::WinProc(const HWND hwnd, const UINT msg, const WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{
		case WM_CREATE:
		{
			DragAcceptFiles(hwnd, TRUE);	//!< accept drag and drop
			break;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		case WM_SIZE:
		{
			const unsigned w = static_cast<unsigned>(LOWORD(lparam));
			const unsigned h = static_cast<unsigned>(HIWORD(lparam));
			g_wnd_size_width = MyMath::Max(w, 64u);
			g_wnd_size_height = MyMath::Max(h, 64u);
			g_wnd_size_changing = true;
			break;
		}
		case WM_ENTERSIZEMOVE:
		{
			// WM_ENTERSIZEMOVE is sent when the user grabs the resize bars
			gp_app->time_system->Stop();
			break;
		}
		case WM_EXITSIZEMOVE:
		{
			// WM_EXITSIZEMOVE is sent when the user releases the resize bars
			gp_app->time_system->Start();
			break;
		}
		case WM_DROPFILES:
		{
			// Get droped file count
			const UINT file_count = DragQueryFileW((HDROP)wparam, 0xFFFFFFFF, nullptr, 0);
			if (file_count <= 0)
			{
				break;
			}

			// Set args parameter
			DropEventArgs args;
			args.files = new wchar_t* [file_count];
			args.file_count = file_count;
			for (unsigned i = 0; i < file_count; ++i)
			{
				wchar_t* file = new wchar_t[MAX_PATH];
				DragQueryFileW((HDROP)wparam, i, file, MAX_PATH);
				args.files[i] = file;
			}

			// Do drop event
			gp_app->DoDrop(args);

			for (unsigned i = 0; i < file_count; ++i)
			{
				delete[] args.files[i];
				args.files[i] = nullptr;
			}
			delete[] args.files;
			args.files = nullptr;

			DragFinish((HDROP)wparam);
			break;
		}
		}

		gp_app->OnMsgProc(hwnd, msg, wparam, lparam);

		return DefWindowProc(hwnd, msg, wparam, lparam);
	}



}	// namespace TKGEngine