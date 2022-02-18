
#include "../inc/Application.h"

#ifdef USE_IMGUI
#include "Systems/inc/IGUISystem.h"
#include "../../imgui/imgui.h"
#endif// USE_IMGUI

#include "Systems/inc/AssetSystem.h"
#include "Systems/inc/IGraphicsSystem.h"

#include "Systems/inc/LogSystem.h"
#include "Systems/inc/IInputSystem.h"
#include "Systems/inc/IAudioSystem.h"
#include "Systems/inc/ISceneSystem.h"
#include "Systems/inc/IWindow.h"
#include "Systems/inc/IInput.h"

#include "Application/inc/SystemSetting.h"

#include "Utility/inc/random.h"
#include "Utility/inc/myfunc_math.h"

#include "../../DirectXTK/Inc/Keyboard.h"
#include "../../DirectXTK/Inc/GamePad.h"
#include "../../DirectXTK/Inc/Mouse.h"
#pragma comment(lib, "DirectXTK.lib")


namespace TKGEngine
{
	Application::Application(
		LPCWSTR title,
		HICON hIcon,
		HMENU hMenu)
		: IApp(title, hIcon, hMenu)
	{
		/* nothing */
	}
	Application::~Application() = default;

	void System::ApplicationQuit()
	{
		PostMessage(IWindow::Get().GetHwnd(), WM_CLOSE, 0, 0);
	}

	bool Application::OnInit()
	{
		// Initialize Graphics System
		graphics_system = Graphics::IGraphicsSystem::CreateInterface();
		if (!graphics_system->OnInit())
		{
			assert(0 && "failed IGraphicsSystem::OnInit() Application::OnInit()");
			return false;
		}
		// 起動時にフルスクリーン対応するか
		if(SystemSetting::WINDOW_FULLSCREEN_MODE)
		{
			graphics_system->ChangeScreenModeWindowed(false);
		}

		// Initialize Asset System
		AssetSystem::Create();

#ifdef USE_IMGUI
		// Initialize GUI System
		gui_system.reset(GUI::IGUISystem::CreateInterface());
		if (!gui_system->OnInit())
		{
			assert(0 && "failed IGUISystem::OnInit() Application::OnInit()");
			return false;
		}
#endif// USE_IMGUI

		// Initialize Log System
		Log::LogSystem::Create(time_system.get());

		// Initialize Input System
		input_system = Input::IInputSystem::CreateInterface();
		if (!input_system->OnInit())
		{
			assert(0 && "failed IInputSystem::OnInit() Application::OnInit()");
			return false;
		}

		// Initialize Audio System
		audio_system = Audio::IAudioSystem::CreateInterface();
		if (!audio_system->OnInit())
		{
			assert(0 && "failed IAudioSystem::OnInit() Application::OnInit()");
			return false;
		}

		// Initialize Scene System
		scene_system = ISceneSystem::CreateInterface();
		if (!scene_system->OnInit())
		{
			assert(0 && "failed ISceneSystem::OnInit() Application::OnInit()");
			return false;
		}

		// Initialize Random seed.
		Random::Initialize();
		
		return true;
	}

	void Application::OnTerm()
	{
		if (scene_system)
		{
			scene_system->OnTerm();
		}

		if (audio_system)
		{
			audio_system->OnTerm();
		}

		if (input_system)
		{
			input_system->OnTerm();
		}

		Log::LogSystem::Destroy();

#ifdef USE_IMGUI
		if (gui_system)
		{
			gui_system->OnTerm();
		}
#endif// USE_IMGUI

		AssetSystem::Destroy();

		if (graphics_system)
		{
			graphics_system->OnTerm();
		}
	}

	void Application::OnMsgProc(const HWND hwnd, const UINT msg, const WPARAM wparam, const LPARAM lparam)
	{
#ifdef  USE_IMGUI
		if (gui_system)
		{
			gui_system->OnMsgProc(hwnd, msg, wparam, lparam);
		}
#endif// USE_IMGUI

		switch (msg)
		{
		case WM_ACTIVATEAPP:
			DirectX::Keyboard::ProcessMessage(msg, wparam, lparam);
			DirectX::Mouse::ProcessMessage(msg, wparam, lparam);
			break;

		case WM_KEYDOWN:
		{
			if (wparam == VK_ESCAPE)
			{
				if (graphics_system->IsWindowed())
				{
					int id = MessageBox(nullptr, TEXT("Finish Application?"), TEXT("Exit Application."), MB_OKCANCEL | MB_ICONQUESTION);
					if (id == IDOK)
					{
						//PostMessage(hwnd, WM_CLOSE, 0, 0);
						System::ApplicationQuit();
					}
				}
				else
				{
					graphics_system->ChangeScreenModeWindowed(true);
					//PostMessage(hwnd, WM_CLOSE, 0, 0);
					int id = MessageBox(nullptr, TEXT("Finish Application?"), TEXT("Exit Application."), MB_OKCANCEL | MB_ICONQUESTION);
					if (id == IDOK)
					{
						//PostMessage(hwnd, WM_CLOSE, 0, 0);
						System::ApplicationQuit();
					}
					else
					{
						graphics_system->ChangeScreenModeWindowed(false);
					}
				}
				break;
			}

		}
		[[fallthrough]];
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			DirectX::Keyboard::ProcessMessage(msg, wparam, lparam);
			break;

		case WM_INPUT:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_MOUSEHOVER:
			DirectX::Mouse::ProcessMessage(msg, wparam, lparam);
			break;
		}
	}

	void Application::DoDrop(const DropEventArgs& args)
	{
		/* nothing */
	}

	void Application::DoResize(const ResizeEventArgs& args)
	{
		graphics_system->OnResize(args);

		scene_system->OnResize(args);
	}

	void Application::OnFrameBegin(const FrameEventArgs& args)
	{
		input_system->OnFrameBegin(args);
		audio_system->OnFrameBegin(args);

		// ウィンドウモード変更
#ifdef USE_IMGUI
		if(ImGui::IsKeyDown(VK_F5) || gui_system->HasChangedScreenMode())
		{
			Log::LogSystem::Destroy();
			gui_system->OnTerm();
			graphics_system->ChangeScreenModeWindowed(graphics_system->IsWindowed() ? false : true);
			gui_system->OnInit();
			Log::LogSystem::Create(time_system.get());
		}
//#else
//		if (IInput::Get().GetKeyDown(Input::Key::F5))
//		{
//			graphics_system->ChangeScreenModeWindowed();
//		}
#endif// #ifdef USE_IMGUI

#ifdef USE_IMGUI
		gui_system->OnFrameBegin(args);
#endif// USE_IMGUI
		graphics_system->OnFrameBegin(args);

		scene_system->OnFrameBegin(args);
	}

	void Application::OnFrameRender(const FrameEventArgs& args)
	{
		scene_system->OnFrameRender(args);

		graphics_system->OnFrameRender(args);

#ifdef USE_IMGUI
		Log::LogSystem::GetInstance()->Draw();
		gui_system->OnFrameRender(args);
#endif
	}

	void Application::OnFrameEnd(const FrameEventArgs& args)
	{
#ifdef USE_IMGUI
		gui_system->OnFrameEnd(args);
#endif
		scene_system->OnFrameEnd(args);
		//graphics_system->OnFrameEnd(args);
	}

}	// namespace TKGEngine