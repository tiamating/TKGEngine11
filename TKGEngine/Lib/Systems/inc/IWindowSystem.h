#pragma once

#include "ISystem.h"

#include "TKGEngine_Defined.h"

#include <Windows.h>
#include <memory>

namespace TKGEngine
{
	struct FrameEventArgs;
	struct ResizeEventArgs;
}// namespace TKGEngine

namespace TKGEngine::Window
{
	/// ========================================================
	/// @class	IWindowSystem
	/// @brief	Window system interface
	/// 
	/// ========================================================
	class IWindowSystem
		: public ISystem
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static std::unique_ptr<IWindowSystem> CreateInterface();

		virtual ~IWindowSystem() = default;

		// ISystem
		virtual bool OnInit() = 0;
		virtual void OnTerm() = 0;
		virtual void OnFrameBegin(const FrameEventArgs& args) = 0;
		virtual void OnFrameRender(const FrameEventArgs& args) = 0;
		virtual void OnFrameEnd(const FrameEventArgs& args) = 0;
		virtual void OnResize(const ResizeEventArgs& args) = 0;
		// ~ISystem

		// IWindowSystem

		///-----------------------------------------------------
		/// @brief	Check to see if it has a focus
		///-----------------------------------------------------
		virtual bool HasFocus() const = 0;

		///-----------------------------------------------------
		/// @brief	Set window parameter
		///-----------------------------------------------------
		virtual void SetWndParam(
			const wchar_t* title,
			unsigned width,
			unsigned height,
			HICON hIcon,
			HMENU hMenu,
			WNDPROC wndProc
		) = 0;
		// ~IWindowSystem

	};
}	// namespace TKGEngine::Window