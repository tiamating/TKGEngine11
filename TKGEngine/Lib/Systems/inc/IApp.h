#pragma once
#include "TKGEngine_Defined.h"
#include "EventArgsList.h"

#include <Windows.h>
#include <memory>
#include <mutex>

namespace TKGEngine
{
	///-----------------------------------------------------
	/// @struct WindowSetUpData
	/// 
	/// @param[in]	title	Application title name
	/// @param[in]	width	window width
	/// @param[in]	height	window height
	/// @param[in]	hIcon	Icon handle
	/// @param[in]	hMenu	Menu handle
	///-----------------------------------------------------
	struct WindowSetUpData
	{
		LPCWSTR title;
		unsigned width;
		unsigned height;
		HICON hIcon;
		HMENU hMenu;

		WindowSetUpData(LPCWSTR t, unsigned w, unsigned h, HICON icon, HMENU menu)
			: title(t), width(w), height(h), hIcon(icon), hMenu(menu)
		{
			/* nothing */
		}
	};

	namespace Window
	{
		class IWindowSystem;
	}
	namespace Time
	{
		class ITimeSystem;
	}

	/// ========================================================
	/// @class	IApp
	/// @brief	Application interface
	/// 
	/// ========================================================
	class IApp
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		///-----------------------------------------------------
		/// @brief	Constructor with arguments
		/// 
		/// @param[in]	title	Application title name
		/// @param[in]	hIcon	Icon handle
		/// @param[in]	hMenu	Menu handle
		///-----------------------------------------------------
		IApp(
			LPCWSTR title,
			HICON hIcon,
			HMENU hMenu
		);

		///-----------------------------------------------------
		/// @brief	Destructor
		///-----------------------------------------------------
		virtual ~IApp();

		///-----------------------------------------------------
		/// @brief	Run the applicating
		///-----------------------------------------------------
		int Run();


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	protected:
		// ==============================================
		// protected methods
		// ==============================================
		virtual bool OnInit() = 0;
		virtual void OnTerm() = 0;
		virtual void OnFrameBegin(const FrameEventArgs& args) = 0;
		virtual void OnFrameRender(const FrameEventArgs& args) = 0;
		virtual void OnFrameEnd(const FrameEventArgs& args) = 0;
		virtual void OnMsgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;
		virtual void DoDrop(const DropEventArgs& args) = 0;
		virtual void DoResize(const ResizeEventArgs& args) = 0;
#ifdef USE_DEBUGGER
		virtual void OnCheckLeak() = 0;
#endif// USE_DEBUGGER

		void StopDraw(bool is_stop_draw);
		bool IsStopDraw() const;

		static LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		// ==============================================
		// protected variables
		// ==============================================
		std::unique_ptr<Window::IWindowSystem> window_system = nullptr;
		std::unique_ptr<Time::ITimeSystem> time_system = nullptr;

	private:
		// ==============================================
		// private methods
		// ==============================================
		void MainLoop();

		bool InitApp();
		void TermApp();
		bool InitWnd();
		void TermWnd();

		void OnWndSizeChanging();
		void OnWndFrameEnd();
		void OnFrameTimeUpdate(FrameEventArgs& args);

		// ==============================================
		// private variables
		// ==============================================
		bool m_is_stop_draw = false;	//!< Load stop flag
		WindowSetUpData m_window_setup_data;


	};
}	// namespace TKGEngine