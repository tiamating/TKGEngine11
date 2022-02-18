#pragma once

#include "ISystem.h"

#include <memory>

namespace TKGEngine::Input
{
	class IInputSystem
		: public ISystem
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static std::unique_ptr<IInputSystem> CreateInterface();

		virtual ~IInputSystem() = default;

		// ISystem
		virtual bool OnInit() = 0;
		virtual void OnTerm() = 0;
		virtual void OnFrameBegin(const FrameEventArgs& args) = 0;
		virtual void OnFrameRender(const FrameEventArgs& args) = 0;
		virtual void OnFrameEnd(const FrameEventArgs& args) = 0;
		virtual void OnResize(const ResizeEventArgs& args) = 0;
		// ~ISystem
	};


}// namespace TKGEngine::Input