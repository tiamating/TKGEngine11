#pragma once

#include "ISystem.h"

#include "TKGEngine_Defined.h"

#include <memory>

namespace TKGEngine::Audio
{
	class IAudioSystem
		: public ISystem
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static std::unique_ptr<IAudioSystem> CreateInterface();

		virtual ~IAudioSystem() = default;

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