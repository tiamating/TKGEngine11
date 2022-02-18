#pragma once

#include "../inc/TKGEngine_Defined.h"
#ifdef USE_IMGUI
#include "ISystem.h"

#include <Windows.h>

namespace TKGEngine
{
	struct FrameEventArgs;
	struct ResizeEventArgs;
}// namespace TKGEngine

namespace TKGEngine::GUI
{
	/// ========================================================
	/// @class	IGUISystem
	/// @brief	GUI system interface
	/// 
	/// ========================================================
	class IGUISystem
		: public ISystem
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static IGUISystem* CreateInterface();

		virtual ~IGUISystem() = default;
		// ISystem
		virtual bool OnInit() = 0;
		virtual void OnTerm() = 0;
		virtual void OnFrameBegin(const FrameEventArgs& args) = 0;
		virtual void OnFrameRender(const FrameEventArgs& args) = 0;
		virtual void OnFrameEnd(const FrameEventArgs& args) = 0;
		virtual void OnResize(const ResizeEventArgs& args) = 0;
		// ~ISystem

		// IGUISystem
		virtual void OnMsgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;
		virtual bool HasChangedScreenMode() = 0;
		// ~IGUISystem
	};
}	// namespace TKGEngine::GUI
#endif// #ifdef USE_IMGUI