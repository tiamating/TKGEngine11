
#include "Graphics_Device.h"

#include "Graphics_IProfiler.h"
#include "Graphics_IQueue.h"
#include "Systems/inc/IWindow.h"
#include "Systems/inc/ITime.h"
#include "Systems/inc/TKGEngine_Defined.h"
#include "Systems/inc/StateManager.h"

#include <cassert>

namespace TKGEngine::Graphics
{
	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	IDevice* IDevice::CreateInterface()
	{
		return new Device();
	}

	bool Device::Init(int msaa_count, int msaa_quality, DXGI_FORMAT bb_format)
	{
		m_is_init = true;
		
		m_msaa_count = msaa_count;
		m_msaa_quality = msaa_quality;
		m_backbuffer_format = bb_format;

		// Create Device and Immediate Context
		if (!CreateDeviceAndIC())
		{
			Release();
			return false;
		}
		// Create SwapChain
		if (!CreateSwapChain())
		{
			Release();
			return false;
		}
		// Create Queues
		{
			m_graphics_queue.reset(IQueue::CreateInterface());
			if (m_graphics_queue->Init(m_device.Get(), static_cast<int>(DC_RENDER_PATH::DC_RP_MAX_NUM), g_max_num_render_threads) == false)
			{
				Release();
				return false;
			}
			m_compute_queue.reset(IQueue::CreateInterface());
			if (m_compute_queue->Init(m_device.Get(), static_cast<int>(DC_COMPUTE_PATH::DC_CP_MAX_NUM), g_max_num_compute_threads) == false)
			{
				Release();
				return false;
			}
			m_copy_queue.reset(IQueue::CreateInterface());
			if (m_copy_queue->Init(m_device.Get(), static_cast<int>(DC_COPY_PATH::DC_CP_MAX_NUM), g_max_num_copy_threads) == false)
			{
				Release();
				return false;
			}
		}
		// Create Queries
		{
			m_profiler.reset(IProfiler::CreateInterface());
			if (!m_profiler->Init(m_device.Get()))
			{
				Release();
				return false;
			}
		}
		// Create State depthstencil, blend, rasterizer and sampler.
		{
			if (!StateManager::InitManager(m_device.Get(), m_ic.Get()))
			{
				Release();
				return false;
			}
		}

		return true;
	}

	void Device::Release()
	{
		if (m_is_init)
		{
			m_profiler.reset();

			m_copy_queue.reset();
			m_compute_queue.reset();
			m_graphics_queue.reset();

			m_swapchain.Reset();
			m_factory.Reset();
			m_ic.Reset();
			m_device.Reset();

			m_msaa_count = 0;
			m_msaa_quality = 0;
			m_backbuffer_format = DXGI_FORMAT_UNKNOWN;
			m_is_init = false;
		}
	}

	void Device::FlushGPU()
	{
		m_profiler->WaitForIdle(m_ic.Get());
	}

	bool Device::OnResize(unsigned width, unsigned height)
	{
		HRESULT hr = S_OK;

		DXGI_MODE_DESC mode_desc = {};
		mode_desc.Width = width;
		mode_desc.Height = height;
		mode_desc.RefreshRate.Denominator = IWindow::Get().GetDisplayFrequency();
		mode_desc.RefreshRate.Numerator = 1;
		mode_desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		mode_desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;// DXGI_MODE_SCALING_STRETCHED;
		hr = m_swapchain->ResizeTarget(&mode_desc);
		if (FAILED(hr))
		{
			assert(0 && "failed IDXGISwapChain->ResizeTarget() Device::OnResize()");
			return false;
		}

		UINT flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#ifndef USE_IMGUI
		if (m_msaa_count <= 1)
		{
			flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		}
#endif
		hr = m_swapchain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, flags);
		if (FAILED(hr))
		{
			assert(0 && "failed IDXGISwapChain->ResizeBuffers() Device::OnResize()");
			return false;
		}

		return true;
	}

	bool Device::OnScreenModeChange(bool& is_windowed, int width, int height)
	{
		BOOL is_fullscreen;
		auto hr = m_swapchain->GetFullscreenState(&is_fullscreen, nullptr);
		if (FAILED(hr))
		{
			assert(0 && "failed IDXGISwapChain::GetFullscreenState() Device::OnScreenModeChange()");
			return false;
		}

		hr = m_swapchain->SetFullscreenState((is_fullscreen == FALSE) ? TRUE : FALSE, nullptr);
		if (FAILED(hr))
		{
			assert(0 && "failed IDXGISwapChain::SetFullscreenState() Device::OnScreenModeChange()");
			return false;
		}

		OnResize(width, height);

		m_is_windowed = (is_fullscreen == FALSE) ? false : true;
		is_windowed = m_is_windowed;

		return true;
	}

	void Device::Present(int syncinterval)
	{
#ifndef USE_IMGUI
		if (syncinterval == 0 && m_msaa_count <= 1)
		{
			DXGI_PRESENT_PARAMETERS prm = {};
			prm.DirtyRectsCount = 0;
			prm.pDirtyRects = nullptr;
			prm.pScrollOffset = nullptr;
			prm.pScrollRect = nullptr;
			if (m_is_windowed)
			{
				m_swapchain->Present1(syncinterval, DXGI_PRESENT_ALLOW_TEARING, &prm);
				//m_swapchain->Present(syncinterval, DXGI_PRESENT_ALLOW_TEARING);
			}
			else
			{
				m_swapchain->Present1(syncinterval, 0, &prm);
			}
		}
		else
#endif
		{
			m_swapchain->Present(syncinterval, 0);
		}
	}

	bool Device::CreateDeviceAndIC()
	{
		HRESULT hr = S_OK;

		// TODO : 複数のデバイスから最も性能の良いものを選択する
		// https://gist.github.com/rdeioris/384c572f6e2663a0fd97ad55a0d072a6
		// https://stackoverflow.com/questions/42354369/idxgifactory-versions
//		// Create Factory
//		{
//			T_UINT flag = 0;
//#if defined (DEBUG) || (_DEBUG)
//			flag |= DXGI_CREATE_FACTORY_DEBUG;
//#endif
//			auto hr = CreateDXGIFactory2(flag, IID_PPV_ARGS(m_factory.GetAddressOf()));
//			if (FAILED(hr))
//			{
//				assert(0 && "failed CreateDXGIFactory2() Device::CreateDeviceAndIC()");
//				return false;
//			}
//		}

		// Create Device
		{
			UINT device_flags = 0;
			device_flags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG) || defined(_DEBUG)
			device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif// defined(DEBUG) || defined(_DEBUG)

			// Driver Type
			D3D_DRIVER_TYPE driver_types[] = {
				D3D_DRIVER_TYPE_HARDWARE,
				D3D_DRIVER_TYPE_WARP,
				D3D_DRIVER_TYPE_REFERENCE,
			};
			UINT num_driver_types = sizeof(driver_types) / sizeof(driver_types[0]);

			// Feature Level
			D3D_FEATURE_LEVEL feature_levels[] = {
				//D3D_FEATURE_LEVEL_12_1,
				//D3D_FEATURE_LEVEL_12_0,
				D3D_FEATURE_LEVEL_11_1,
				D3D_FEATURE_LEVEL_11_0,
				//D3D_FEATURE_LEVEL_10_1,
				//D3D_FEATURE_LEVEL_10_0,
				//D3D_FEATURE_LEVEL_9_3,
				//D3D_FEATURE_LEVEL_9_2,
				//D3D_FEATURE_LEVEL_9_1,
			};
			UINT num_feature_levels = sizeof(feature_levels) / sizeof(feature_levels[0]);

			for (UINT idx = 0; idx < num_driver_types; idx++)
			{
				D3D_DRIVER_TYPE driverType = driver_types[idx];
				D3D_FEATURE_LEVEL featureLevel;
				hr = D3D11CreateDevice(
					nullptr, driverType, nullptr, device_flags, feature_levels,
					num_feature_levels, D3D11_SDK_VERSION, (ID3D11Device**)m_device.GetAddressOf(),
					&featureLevel, (ID3D11DeviceContext**)m_ic.GetAddressOf());
				if (SUCCEEDED(hr))break;
			}
			if (FAILED(hr))
			{
				assert(0 && "failed create device Device::CreateDeviceAndIC()");
				return false;
			}
		}

		// Create Factory
		{
			// Get Interface
			Microsoft::WRL::ComPtr<IDXGIDevice> dxgi_device = nullptr;
			hr = m_device->QueryInterface(IID_PPV_ARGS(dxgi_device.GetAddressOf()));
			if (FAILED(hr))
			{
				assert(0 && "failed QueryInterface IDXGIDevice Device::CreateDeviceAndIC()");
				return false;
			}

			// Get Adapter
			Microsoft::WRL::ComPtr<IDXGIAdapter> adapter = nullptr;
			hr = dxgi_device->GetAdapter(adapter.GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed GetAdapter Device::CreateDeviceAndIC()");
				return false;
			}

			// Get Factory
			hr = adapter->GetParent(IID_PPV_ARGS(m_factory.GetAddressOf()));
			if (FAILED(hr))
			{
				assert(0 && "failed GetParent Device::CreateDeviceAndIC()");
				return false;
			}
		}

		return true;
	}

	bool Device::CreateSwapChain()
	{
		HRESULT hr = S_OK;
		
		// Configurate SwapChain desc
		DXGI_SWAP_CHAIN_DESC1 sc_desc = {};
		{
			sc_desc.Width = IWindow::Get().GetScreenWidth();
			sc_desc.Height = IWindow::Get().GetScreenHeight();
			sc_desc.Format = m_backbuffer_format;
			sc_desc.Stereo = false;
			sc_desc.SampleDesc.Count = m_msaa_count;
			sc_desc.SampleDesc.Quality = m_msaa_quality;
			sc_desc.BufferUsage =
				DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
			sc_desc.BufferCount = m_buffer_count;
			sc_desc.Scaling = 
#ifndef USE_IMGUI
				m_msaa_count <= 1 ? DXGI_SCALING_NONE : 
#endif
				DXGI_SCALING_STRETCH;
			sc_desc.SwapEffect =
#ifndef USE_IMGUI
				m_msaa_count <= 1 ? DXGI_SWAP_EFFECT_FLIP_DISCARD : 
#endif
				DXGI_SWAP_EFFECT_DISCARD;
			sc_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			sc_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#ifndef USE_IMGUI
			if (m_msaa_count <= 1)
			{
				sc_desc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
			}
#endif
		}
		// Configurate SwapChain FullScreen desc
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fs_desc = {};
		{
			fs_desc.RefreshRate.Denominator = IWindow::Get().GetDisplayFrequency();
			fs_desc.RefreshRate.Numerator = 1;
			fs_desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			fs_desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;// DXGI_MODE_SCALING_STRETCHED;
			fs_desc.Windowed = m_is_windowed;
		}
		hr = m_factory->CreateSwapChainForHwnd(
			m_device.Get(),
			IWindow::Get().GetHwnd(),
			&sc_desc,
			&fs_desc,
			nullptr,
			reinterpret_cast<IDXGISwapChain1**>(m_swapchain.GetAddressOf())
		);
		if (FAILED(hr))
		{
			assert(0 && "failed create swapchain GraphicsSystem::CreateSwapChain()");
			return false;
		}

		// Window mode can not be changed FullScreen, when push Alt+Enter
		hr = m_factory->MakeWindowAssociation(
			IWindow::Get().GetHwnd(),
			DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
		if (FAILED(hr))
		{
			assert(0 && "failed MakeWindowAssociation() Device::CreateDeviceAndIC()");
			return false;
		}
		
		return true;
	}

	void Device::BeginFrame()
	{
		m_profiler->BeginFrame(m_ic.Get());
	}

	void Device::EndFrame()
	{
		m_profiler->EndFrame(m_ic.Get());
		m_profiler->WaitAndUpdate(m_ic.Get());
	}

#ifdef USE_IMGUI
	void Device::TimeStamp(TS_TYPE type, int thread_idx)
	{
		m_profiler->TimeStamp(m_ic.Get(), type, thread_idx);
	}

	void Device::ProfilerOnGUI()
	{
		if (m_profiler)
			m_profiler->OnGUI();
	}
#endif// #ifdef USE_IMGUI


}	// namespace TKGEngine::Graphics