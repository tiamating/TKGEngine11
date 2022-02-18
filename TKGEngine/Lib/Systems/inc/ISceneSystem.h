#pragma once

#include "ISystem.h"

#include "TKGEngine_Defined.h"

#include <memory>

namespace TKGEngine
{
	struct FrameEventArgs;
	struct ResizeEventArgs;

}// namespace TKGEngine


namespace TKGEngine
{
	/// ========================================================
	/// @class	ISceneSystem
	/// @brief	Scene system interface
	/// 
	/// ========================================================
	class ISceneSystem
		: public ISystem
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static std::unique_ptr<ISceneSystem> CreateInterface();

		virtual ~ISceneSystem() = default;

		// ISystem
		virtual bool OnInit() = 0;
		virtual void OnTerm() = 0;
		virtual void OnFrameBegin(const FrameEventArgs& args) = 0;
		virtual void OnFrameRender(const FrameEventArgs& args) = 0;
		virtual void OnFrameEnd(const FrameEventArgs& args) = 0;
		virtual void OnResize(const ResizeEventArgs& args) = 0;
		// ~ISystem


	};
}	// namespace TKGEngine