#pragma once

#include "ISystem.h"

#include <memory>

namespace TKGEngine
{
	struct FrameEventArgs;
	struct ResizeEventArgs;
}// namespace TKGEngine

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace TKGEngine::Graphics
{
	class IQueue;

	/// ========================================================
	/// @class	IGraphicsSystem
	/// @brief	Graphics system interface
	/// 
	/// ========================================================
	class IGraphicsSystem
		: public ISystem
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static std::unique_ptr<IGraphicsSystem> CreateInterface();

		virtual ~IGraphicsSystem() = default;

		// ISystem
		virtual bool OnInit() = 0;
		virtual void OnTerm() = 0;
		virtual void OnFrameBegin(const FrameEventArgs& args) = 0;
		virtual void OnFrameRender(const FrameEventArgs& args) = 0;
		virtual void OnFrameEnd(const FrameEventArgs& args) = 0;
		virtual void OnResize(const ResizeEventArgs& args) = 0;
		// ~ISystem

		// IGraphicsSystem
		virtual void ChangeScreenModeWindowed(bool is_window) = 0;
		virtual bool IsWindowed() = 0;

		virtual void OnFrameSetUp() = 0;
		//virtual ID3D11Device* GetDevice() const = 0;
		//virtual ID3D11DeviceContext* GetIC() const = 0;
		virtual IQueue* GetQueueGraphics() const = 0;
		//virtual IQueue* GetQueueCompute() const = 0;
		virtual IQueue* GetQueueCopy() const = 0;
		// ~IGraphicsSystem

	};
}	// namespace TKGEngine::Graphics