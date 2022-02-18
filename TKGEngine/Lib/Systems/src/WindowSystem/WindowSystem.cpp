
#include "Systems/inc/IWindowSystem.h"
#include "Systems/inc/IWindow.h"
#include "Systems/inc/EventArgsList.h"

#include "Application/inc/SystemSetting.h"

#include <Windows.h>

namespace TKGEngine::Window
{
	////////////////////////////////////////////////////////
	// Internal linkage Global variable
	////////////////////////////////////////////////////////
	namespace /* anonymous */
	{
		const wchar_t* g_wnd_class_name = L"AppWindowClass";
	}	// /* anonymous */

	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	/// ========================================================
	/// @class	WindowSystem
	/// @brief	Window system
	/// 
	/// ========================================================
	class WindowSystem
		: public IWindowSystem,
		public IWindow
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		WindowSystem() = default;
		virtual ~WindowSystem() = default;
		WindowSystem(WindowSystem&&) = default;
		WindowSystem(const WindowSystem&) = delete;
		WindowSystem& operator=(const WindowSystem&) = delete;

		bool OnInit() override;
		void OnTerm() override;
		void OnFrameBegin(const FrameEventArgs& args) override;
		void OnFrameRender(const FrameEventArgs& args) override;
		void OnFrameEnd(const FrameEventArgs& args) override;
		void OnResize(const ResizeEventArgs& args) override;

		///-----------------------------------------------------
		/// @brief	Check to see if it has a focus
		///-----------------------------------------------------
		[[nodiscard]] bool HasFocus() const override;

		///-----------------------------------------------------
		/// @brief	Set window parameter
		///-----------------------------------------------------
		void SetWndParam(
			const wchar_t* title,
			unsigned width,
			unsigned height,
			HICON hIcon,
			HMENU hMenu,
			WNDPROC wndProc
		) override;

		// ==============================================
		// public variables
		// ==============================================



	private:
		// ==============================================
		// private methods
		// ==============================================
		bool GetDisplayFeature();

		[[nodiscard]] DWORD GetScreenResolutionWidth() const override;
		[[nodiscard]] DWORD GetScreenResolutionHeight() const override;
		[[nodiscard]] DWORD GetDisplayFrequency() const override;

		// IWindow
		[[nodiscard]] RECT GetScreenRect() const override;
		[[nodiscard]] int GetScreenWidth() const override;
		[[nodiscard]] float GetScreenWidthf() const override;
		[[nodiscard]] int GetScreenHeight() const override;
		[[nodiscard]] float GetScreenHeightf() const override;
		[[nodiscard]] int GetScreenFrequency() const override;
		[[nodiscard]] HWND GetHwnd() const override;
		// ~IWindow


		// ==============================================
		// private variables
		// ==============================================
		const wchar_t* m_title = nullptr;	//!< Window title name
		HICON m_hicon = nullptr;			//!< Icon handle
		HMENU m_hmenu = nullptr;			//!< Menu handle
		WNDPROC m_wnd_proc = nullptr;		//!< window procedure
		HINSTANCE m_hinst = nullptr;		//!< Instance handle
		HWND m_hwnd = nullptr;				//!< window handle

		int m_window_width = 0;			//!< Window width
		int m_window_height = 0;		//!< Window height

		// main display param
		DWORD m_resol_width = 0;		//!< resolution width
		DWORD m_resol_height = 0;		//!< resolution height
		DWORD m_disp_frequency = 0;		//!< display frequency

	};

	std::unique_ptr<IWindowSystem> IWindowSystem::CreateInterface()
	{
		return std::make_unique<WindowSystem>();
	}

	bool WindowSystem::OnInit()
	{
		// Get instance handle
		m_hinst = GetModuleHandle(nullptr);
		if (!m_hinst)
		{
			assert(0 && "failed GetModuleHandle() WindowSystem::OnInit()");
			return false;
		}

		// Use default icon when do not have icon
		if (!m_hicon)
		{
			m_hicon = LoadIcon(m_hinst, IDI_APPLICATION);//LoadIcon(instance, MAKEINTRESOURCE(IDI_ICON2))
		}

		// Configurate window class ex
		WNDCLASSEX wc = {};
		{
			wc.cbSize = sizeof(wc);
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = m_wnd_proc;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hInstance = m_hinst;
			wc.hIcon = m_hicon;
			wc.hCursor = LoadCursor(nullptr, IDC_ARROW);//LoadCursor(h_inst, MAKEINTERSOURCE(IDC_CURSOR1));
			wc.hbrBackground = reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1));
			wc.lpszMenuName = nullptr;
			wc.lpszClassName = g_wnd_class_name;
			wc.hIconSm = nullptr;// m_hicon;
		}
		// Register window class
		if (!RegisterClassEx(&wc))
		{
			assert(0 && "failed RegisterClassEx() WindowSystem::OnInit()");
			return false;
		}

		// Configurate window rect
		RECT rc = { 0,0,static_cast<LONG>(m_window_width),static_cast<LONG>(m_window_height) };
		
		DWORD style = WS_OVERLAPPEDWINDOW;
		AdjustWindowRect(&rc, style, FALSE);
		style |= WS_VISIBLE;
		// Create window
		m_hwnd = CreateWindow(
			g_wnd_class_name,
			m_title,
			style,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			rc.right - rc.left,
			rc.bottom - rc.top,
			nullptr,
			m_hmenu,
			m_hinst,
			nullptr
		);
		if (!m_hwnd)
		{
			assert(0 && "failed CreateWindow() WindowSystem::OnInit()");
			return false;
		}

		// Display window
		ShowWindow(m_hwnd, SW_SHOWNORMAL);
		UpdateWindow(m_hwnd);
		// フルスクリーンでなければ最大化チェック
		if constexpr (!SystemSetting::WINDOW_FULLSCREEN_MODE && SystemSetting::WINDOW_SHOW_MAXIMIZE)
		{
			ShowWindow(m_hwnd, SW_MAXIMIZE);
		}

		// Set focus
		SetFocus(m_hwnd);

		// Get display feature
		GetDisplayFeature();

		IWindow::Init(this);

		return true;
	}

	void WindowSystem::OnTerm()
	{
		IWindow::Release();

		// Unregister window class
		if (m_hinst)
		{
			UnregisterClass(g_wnd_class_name, m_hinst);
		}
		// Unregister Icon
		if (m_hicon)
		{
			DestroyIcon(m_hicon);
		}
		// Clear pointer
		m_title = nullptr;
		m_hinst = nullptr;
		m_hicon = nullptr;
		m_hwnd = nullptr;
		m_hmenu = nullptr;
	}

	void WindowSystem::OnFrameBegin(const FrameEventArgs& args)
	{
		/* nothing */
	}

	void WindowSystem::OnFrameRender(const FrameEventArgs& args)
	{
		/* nothing */
	}

	void WindowSystem::OnFrameEnd(const FrameEventArgs& args)
	{
		/* nothing */
	}

	void WindowSystem::OnResize(const ResizeEventArgs& args)
	{
		m_window_width = args.width;
		m_window_height = args.height;
	}

	DWORD WindowSystem::GetScreenResolutionWidth() const
	{
		return m_resol_width;
	}

	DWORD WindowSystem::GetScreenResolutionHeight() const
	{
		return m_resol_height;
	}

	DWORD WindowSystem::GetDisplayFrequency() const
	{
		return m_disp_frequency;
	}

	bool WindowSystem::HasFocus() const
	{
		return GetActiveWindow() == m_hwnd;
	}

	void WindowSystem::SetWndParam(const wchar_t* _title, unsigned width, unsigned height, HICON hIcon, HMENU hMenu, WNDPROC wndProc)
	{
		m_title = _title;
		m_window_width = static_cast<int>(width);
		m_window_height = static_cast<int>(height);
		m_hicon = hIcon;
		m_hmenu = hMenu;
		m_wnd_proc = wndProc;
	}


	// ---------------------------------------------
	// Get display feature
	// ---------------------------------------------
	bool WindowSystem::GetDisplayFeature()
	{
		// http://jag5.dreamlog.jp/archives/7949249.html
		// https://nompor.com/2018/06/29/post-4002/
		// http://wisdom.sakura.ne.jp/system/winapi/win32/win37.html
		DEVMODE dev = {};
		if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dev) == 0)
		{
			assert(0 && "failed get display settings WindowSystem::GetDisplaySetting()");
			return false;
		}
		m_resol_width = dev.dmPelsWidth;
		m_resol_height = dev.dmPelsHeight;
		m_disp_frequency = dev.dmDisplayFrequency;
		return true;
	}


	RECT WindowSystem::GetScreenRect() const
	{
		RECT r = {};
		GetWindowRect(m_hwnd, &r);
		return r;
	}

	// IWindow
	inline int WindowSystem::GetScreenWidth() const
	{
		return m_window_width;
	}

	inline float WindowSystem::GetScreenWidthf() const
	{
		return static_cast<float>(m_window_width);
	}

	inline int WindowSystem::GetScreenHeight() const
	{
		return m_window_height;
	}
	inline float WindowSystem::GetScreenHeightf() const
	{
		return static_cast<float>(m_window_height);
	}

	inline int WindowSystem::GetScreenFrequency() const
	{
		return static_cast<int>(m_disp_frequency);
	}

	inline HWND WindowSystem::GetHwnd() const
	{
		return m_hwnd;
	}

	// ~IWindow


}	// namespace TKGEngine::Window