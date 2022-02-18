#pragma once

#include "IApp.h"
#include "SystemFunction.h"
#include <memory>

namespace TKGEngine::Graphics
{
	class IGraphicsSystem;
}// namespace TKGEngine::Graphics

namespace TKGEngine::GUI
{
	class IGUISystem;
}// namespace TKGEngine::GUI

namespace TKGEngine
{
	class ISceneSystem;
}// namespace TKGEngine

namespace TKGEngine::Input
{
	class IInputSystem;
}// namespace TKGEngine::Input

namespace TKGEngine::Audio
{
	class IAudioSystem;
}// namespace TKGEngine::Audio

namespace TKGEngine
{
	/// ========================================================
	/// @class	Application
	/// @brief	main application class
	/// 
	/// ========================================================
	class Application
		: public IApp
		, public System
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
		Application(
			LPCWSTR title,
			HICON hIcon,
			HMENU hMenu
		);

		virtual ~Application();

		// ==============================================
		// public variables
		// ==============================================



	private:
		// ==============================================
		// private methods
		// ==============================================
		bool OnInit() override;
		void OnTerm() override;
		void OnMsgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) override;
		void DoDrop(const DropEventArgs& args) override;
		void DoResize(const ResizeEventArgs& args) override;
		void OnFrameBegin(const FrameEventArgs& args) override;
		void OnFrameRender(const FrameEventArgs& args) override;
		void OnFrameEnd(const FrameEventArgs& args) override;
//#ifdef USE_DEBUGGER
//		void OnCheckLeak() override;
//#endif// USE_DEBUGGER

		// ==============================================
		// private variables
		// ==============================================
		std::unique_ptr<Graphics::IGraphicsSystem> graphics_system = nullptr;
#ifdef USE_IMGUI
		std::unique_ptr<GUI::IGUISystem> gui_system = nullptr;
#endif// USE_IMGUI
		std::unique_ptr<Input::IInputSystem> input_system = nullptr;
		std::unique_ptr<Audio::IAudioSystem> audio_system = nullptr;
		std::unique_ptr<ISceneSystem> scene_system = nullptr;
	};
}	// namespace TKGEngine