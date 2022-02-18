#pragma once

#include "Graphics_IDevice.h"

#include <wrl.h>
#include <memory>

namespace TKGEngine::Graphics
{
	class IQueue;
	class IProfiler;

	class Device
		: public IDevice
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Device() = default;
		virtual ~Device() = default;
		Device(const Device&) = delete;
		Device& operator=(const Device&) = delete;

		bool Init(int msaa_count, int msaa_quality, DXGI_FORMAT bb_format) override;
		void Release() override;

		void FlushGPU() override;
		bool OnResize(unsigned width, unsigned height) override;
		bool OnScreenModeChange(bool& is_windowed, int width, int height) override;

		void Present(int syncinterval) override;

		ID3D11Device* GetDevice() const override;
		IDXGISwapChain* GetSwapChain() const override;
		ID3D11DeviceContext* GetIC() const override;
		IQueue* GetQueueGraphics() const override;
		IQueue* GetQueueCompute() const override;
		IQueue* GetQueueCopy() const override;

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================
		void BeginFrame() override;
		void EndFrame() override;
#ifdef USE_IMGUI
		void TimeStamp(TS_TYPE type, int thread_idx) override;
		// プロファイラのデバッグ表示
		void ProfilerOnGUI() override;
#endif// #ifdef USE_IMGUI

		bool CreateDeviceAndIC();
		bool CreateSwapChain();


		// ==============================================
		// private variables
		// ==============================================
		bool m_is_init = false;

		Microsoft::WRL::ComPtr<ID3D11Device5> m_device = nullptr;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext4> m_ic = nullptr;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_swapchain = nullptr;
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_factory = nullptr;
#ifdef USE_DEBUGGER
		Microsoft::WRL::ComPtr<ID3D11Debug> m_debug = nullptr;
#endif// USE_DEBUGGER

		std::unique_ptr<IProfiler> m_profiler = nullptr;

		std::unique_ptr<IQueue> m_graphics_queue = nullptr;
		std::unique_ptr<IQueue> m_compute_queue = nullptr;
		std::unique_ptr<IQueue> m_copy_queue = nullptr;

		bool m_is_windowed = true;
		unsigned m_buffer_count = g_buffer_count;	//!< back buffer count
		unsigned m_msaa_count = 1;	//!< count of multi sample
		unsigned m_msaa_quality = 0;	//!< quality of multi sample
		DXGI_FORMAT m_backbuffer_format = DXGI_FORMAT_UNKNOWN;	//!< back buffer format
	};

	// --------------------------------------------------------
	// inline
	// --------------------------------------------------------
	// ---------------
	// Device class
	// ---------------
	inline ID3D11Device* Device::GetDevice() const
	{
		return m_device.Get();
	}

	inline IDXGISwapChain* Device::GetSwapChain() const
	{
		return m_swapchain.Get();
	}

	inline ID3D11DeviceContext* Device::GetIC() const
	{
		return m_ic.Get();
	}

	inline IQueue* Device::GetQueueGraphics() const
	{
		return m_graphics_queue.get();
	}

	inline IQueue* Device::GetQueueCompute() const
	{
		return m_compute_queue.get();
	}

	inline IQueue* Device::GetQueueCopy() const
	{
		return m_copy_queue.get();
	}

}	// namespace TKGEngine::Graphics