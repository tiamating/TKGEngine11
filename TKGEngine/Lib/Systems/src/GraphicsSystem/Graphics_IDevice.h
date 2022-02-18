#pragma once

#include "../../inc/Graphics_Defined.h"
#include "Systems/inc/TKGEngine_Defined.h"

#include <d3d11_4.h>
#include <dxgi1_6.h>
#if defined(DEBUG) || defined(_DEBUG)
#include <dxgidebug.h>
#endif// defined(DEBUG) || defined(_DEBUG)


namespace TKGEngine::Graphics
{
	class IQueue;

	class IDevice
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static IDevice* CreateInterface();

		IDevice() = default;
		virtual ~IDevice() = default;
		IDevice(const IDevice&) = delete;
		IDevice& operator=(const IDevice&) = delete;

		virtual bool Init(int msaa_count, int msaa_quality, DXGI_FORMAT bb_format) = 0;
		virtual void Release() = 0;

		virtual void FlushGPU() = 0;
		virtual bool OnResize(unsigned width, unsigned height) = 0;
		virtual bool OnScreenModeChange(bool& is_windowed, int width, int height) = 0;

		virtual void Present(int syncinterval) = 0;

		virtual ID3D11Device* GetDevice() const = 0;
		virtual IDXGISwapChain* GetSwapChain() const = 0;
		virtual ID3D11DeviceContext* GetIC() const = 0;
		virtual IQueue* GetQueueGraphics() const = 0;
		virtual IQueue* GetQueueCompute() const = 0;
		virtual IQueue* GetQueueCopy() const = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
#ifdef USE_IMGUI
		virtual void TimeStamp(TS_TYPE type, int thread_idx) = 0;
		// プロファイラのデバッグ表示
		virtual void ProfilerOnGUI() = 0;
#endif// #ifdef USE_IMGUI
	};


}	// namespace TKGEngine::Graphics