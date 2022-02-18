#pragma once

#include "TKGEngine_Defined.h"
#include "EventArgsList.h"

namespace TKGEngine
{
	/// ========================================================
	/// @class	ISystem
	/// @brief	System interface
	/// 
	/// ========================================================
	class ISystem
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		virtual ~ISystem() = default;

		virtual bool OnInit() = 0;
		virtual void OnTerm() = 0;
		virtual void OnFrameBegin(const FrameEventArgs& args) = 0;
		virtual void OnFrameRender(const FrameEventArgs& args) = 0;
		virtual void OnFrameEnd(const FrameEventArgs& args) = 0;
		virtual void OnResize(const ResizeEventArgs& args) = 0;
	};
}	// namespace TKGEngine