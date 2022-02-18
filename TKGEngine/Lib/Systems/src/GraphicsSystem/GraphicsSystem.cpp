
#include "../../inc/IGraphicsSystem.h"
#include "../../inc/IGraphics.h"
#include "../../inc/Graphics_Defined.h"
#include "../../inc/StateManager.h"
#include "../../inc/EventArgsList.h"

#include "Graphics_IDevice.h"
#include "Graphics_IQueue.h"
#include "Graphics_ICommandList.h"

#include "Systems/inc/TKGEngine_Defined.h"
#include "Systems/inc/IWindow.h"
#include "Systems/inc/ITime.h"
#include "Systems/inc/LogSystem.h"
#include "Application/Resource/inc/ITarget.h"
#include "Application/inc/SystemSetting.h"

#include <mutex>
#include <wrl.h>
#include <memory>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

namespace TKGEngine::Graphics
{
	/// ========================================================
	/// @class	GraphicsSystem
	/// @brief	Graphics system
	/// 
	/// ========================================================
	class GraphicsSystem
		: public IGraphicsSystem
		, public IGraphics
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		GraphicsSystem();
		virtual ~GraphicsSystem();
		GraphicsSystem(GraphicsSystem&&) = default;
		GraphicsSystem(const GraphicsSystem&) = delete;
		GraphicsSystem& operator=(const GraphicsSystem&) = delete;

		bool OnInit() override;
		void OnTerm() override;
		void OnFrameBegin(const FrameEventArgs& args) override;
		void OnFrameRender(const FrameEventArgs& args) override;
		void OnFrameEnd(const FrameEventArgs& args) override;
		void OnResize(const ResizeEventArgs& args) override;

		void ChangeScreenModeWindowed(bool is_window) override;
		bool IsWindowed() override;

		void OnFrameSetUp() override;

		IQueue* GetQueueGraphics() const override;
		//IQueue* GetQueueCompute() const override;
		IQueue* GetQueueCopy() const override;


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================
		// Graphics
		inline ID3D11Device* Device() const override;
		inline ID3D11DeviceContext* IC() const override;
		inline ID3D11DeviceContext* DC(int thread_idx, Graphics::DC_RENDER_PATH path_idx) const override;
		inline ID3D11DeviceContext* DC(int thread_idx, Graphics::DC_COPY_PATH path_idx) const override;
		inline ID3D11DeviceContext* DC(int thread_idx, Graphics::DC_COMPUTE_PATH path_idx) const override;

		void Present(int syncinterval) override;
		void SetViewPort(ID3D11DeviceContext* p_context, float width, float height, float top_left_x = 0.0f, float top_left_y = 0.0f) override;

		void SetRTBackBuffer(ID3D11DeviceContext* p_context, bool set_depth) override;
		void ClearBackBuffer() override;
		// ~Graphics


		// ==============================================
		// private variables
		// ==============================================
		std::unique_ptr<IDevice> m_p_device;
		std::unique_ptr<IColorTarget> m_color_target;
		std::unique_ptr<IDepthTarget> m_depth_target;

		FLOAT m_clear_color[4];
		bool m_is_windowed = true;	//!< Window mode
		unsigned m_msaa_count = 1;	//!< count of multi sample
		unsigned m_msaa_quality = 0;	//!< quality of multi sample
		DXGI_FORMAT m_backbuffer_format = DXGI_FORMAT_UNKNOWN;	//!< back buffer format
		DXGI_FORMAT m_depth_stencil_format = DXGI_FORMAT_UNKNOWN;	//!< depth stencil format

		int m_windowed_width = 0;
		int m_windowed_height = 0;
	};

	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	std::unique_ptr<IGraphicsSystem> IGraphicsSystem::CreateInterface()
	{
		return std::make_unique<GraphicsSystem>();
	}

	GraphicsSystem::GraphicsSystem()
		: m_msaa_count(1)
		, m_msaa_quality(0)
		, m_backbuffer_format(g_color_format)
		, m_depth_stencil_format(g_depth_format)
	{
		m_clear_color[0] = 0.0f;
		m_clear_color[1] = 0.4f;
		m_clear_color[2] = 0.0f;
		m_clear_color[3] = 1.0f;
	}

	GraphicsSystem::~GraphicsSystem()
	{
		/* nothing */
	}

	bool GraphicsSystem::OnInit()
	{
		// Create Device
		m_p_device.reset(IDevice::CreateInterface());
		if (!m_p_device->Init(m_msaa_count, m_msaa_quality, m_backbuffer_format))
		{
			return false;
		}

		// Create RTV and DSV
		{
			m_color_target = IColorTarget::CreateInterface();
			m_depth_target = IDepthTarget::CreateInterface();
			if (!m_color_target->Create(m_p_device->GetDevice(), m_p_device->GetSwapChain()))
			{
				return false;
			}
			const TargetDesc target_desc(IWindow::Get().GetScreenWidth(), IWindow::Get().GetScreenHeight(), m_depth_stencil_format, m_msaa_count, m_msaa_quality);
			if (!m_depth_target->Create(m_p_device->GetDevice(), target_desc, false))
			{
				return false;
			}
		}

		// Set Accessor
		IGraphics::Init(this);

		return true;
	}

	void GraphicsSystem::OnTerm()
	{
		IGraphics::Release();

		if (!m_is_windowed)
		{
			ChangeScreenModeWindowed(true);
		}

		m_color_target->Release();
		m_depth_target->Release();

		m_p_device.reset();
	}

	void GraphicsSystem::OnFrameBegin(const FrameEventArgs& args)
	{
		/* nothing */
	}

	void GraphicsSystem::OnFrameRender(const FrameEventArgs& args)
	{
		ID3D11DeviceContext* context = m_p_device->GetIC();
		m_p_device->BeginFrame();

		// プロファイルをはさみこんでコマンドリストを実行する
		// Copy
		{
			m_p_device->GetQueueCopy()->Execute(context, 0, static_cast<int>(DC_COPY_PATH::DC_CP_MAIN));
#ifdef USE_IMGUI
			m_p_device->TimeStamp(TS_TYPE::TS_COPY, 0);
#endif// #ifdef USE_IMGUI
		}
		// Compute
		{
			m_p_device->GetQueueCompute()->Execute(context, 0, static_cast<int>(DC_COMPUTE_PATH::DC_CP_MAIN));
#ifdef USE_IMGUI
			m_p_device->TimeStamp(TS_TYPE::TS_COMPUTE, 0);
#endif// #ifdef USE_IMGUI
		}
		// Render
		{
			m_p_device->GetQueueGraphics()->Execute(context, 0, static_cast<int>(DC_RENDER_PATH::DC_RP_SHADOW));
#ifdef USE_IMGUI
			m_p_device->TimeStamp(TS_TYPE::TS_RENDER, static_cast<int>(DC_RENDER_PATH::DC_RP_SHADOW));
#endif// #ifdef USE_IMGUI

			m_p_device->GetQueueGraphics()->Execute(context, 0, static_cast<int>(DC_RENDER_PATH::DC_RP_Z_PREPATH));
#ifdef USE_IMGUI
			m_p_device->TimeStamp(TS_TYPE::TS_RENDER, static_cast<int>(DC_RENDER_PATH::DC_RP_Z_PREPATH));
#endif// #ifdef USE_IMGUI

			m_p_device->GetQueueGraphics()->Execute(context, 0, static_cast<int>(DC_RENDER_PATH::DC_RP_MAIN));
#ifdef USE_IMGUI
			m_p_device->TimeStamp(TS_TYPE::TS_RENDER, static_cast<int>(DC_RENDER_PATH::DC_RP_MAIN));
#endif// #ifdef USE_IMGUI

			m_p_device->GetQueueGraphics()->Execute(context, 0, static_cast<int>(DC_RENDER_PATH::DC_RP_POST_AFTER_MAIN));
#ifdef USE_IMGUI
			m_p_device->TimeStamp(TS_TYPE::TS_RENDER, static_cast<int>(DC_RENDER_PATH::DC_RP_POST_AFTER_MAIN));
#endif// #ifdef USE_IMGUI

			m_p_device->GetQueueGraphics()->Execute(context, 0, static_cast<int>(DC_RENDER_PATH::DC_RP_UI));
#ifdef USE_IMGUI
			m_p_device->TimeStamp(TS_TYPE::TS_RENDER, static_cast<int>(DC_RENDER_PATH::DC_RP_UI));
#endif// #ifdef USE_IMGUI

			m_p_device->GetQueueGraphics()->Execute(context, 0, static_cast<int>(DC_RENDER_PATH::DC_RP_POST_AFTER_ALL));
#ifdef USE_IMGUI
			m_p_device->TimeStamp(TS_TYPE::TS_RENDER, static_cast<int>(DC_RENDER_PATH::DC_RP_POST_AFTER_ALL));
#endif// #ifdef USE_IMGUI
		}

		m_p_device->EndFrame();
#ifdef USE_IMGUI
		// プロファイル結果のGUI
		m_p_device->ProfilerOnGUI();
#endif// #ifdef USE_IMGUI
	}

	void GraphicsSystem::OnFrameEnd(const FrameEventArgs& args)
	{
		/* nothing */
	}

	void GraphicsSystem::ChangeScreenModeWindowed(bool is_window)
	{
		// モード変更がなければ早期リターン
		if (m_is_windowed == is_window)
			return;

		// GPUの処理終了待機とターゲットの解放
		m_p_device->FlushGPU();
		m_color_target->Release();
		m_depth_target->Release();
		// ウィンドウサイズを復元できるようにする
		int width;
		int height;
		if (m_is_windowed)
		{
			width = IWindow::Get().GetScreenResolutionWidth();
			height = IWindow::Get().GetScreenResolutionHeight();
			m_windowed_width = IWindow::Get().GetScreenWidth();
			m_windowed_height = IWindow::Get().GetScreenHeight();
		}
		else
		{
			width = m_windowed_width;
			height = m_windowed_height;
		}

		// モードフラグの変更とウィンドウサイズの適用
		m_p_device->OnScreenModeChange(m_is_windowed, width, height);

		// ターゲットのリサイズ
		if (!m_color_target->Resize(m_p_device->GetDevice(), m_p_device->GetSwapChain()))
		{
			assert(0 && "failed ColorTarget::Resize() GraphicsSystem::ChangeScreenModeWindowed");
			return;
		}
		if (!m_depth_target->Resize(m_p_device->GetDevice(), width, height))
		{
			assert(0 && "failed DepthTarget::Resize() GraphicsSystem::ChangeScreenModeWindowed");
			return;
		}
	}

	bool GraphicsSystem::IsWindowed()
	{
		return m_is_windowed;
	}

	void GraphicsSystem::OnFrameSetUp()
	{
		/* nothing */
	}

	void GraphicsSystem::OnResize(const ResizeEventArgs& args)
	{
		// GPUのコマンド終了まで待機
		m_p_device->FlushGPU();
		// ターゲットテクスチャの解放
		m_color_target->Release();
		m_depth_target->Release();

		// リサイズ
		if (!m_p_device->OnResize(args.width, args.height))
		{
			assert(0 && "failed Device::OnResize()");
			return;
		}

		if (!m_color_target->Resize(m_p_device->GetDevice(), m_p_device->GetSwapChain()))
		{
			assert(0 && "failed ColorTarget::Resize() GraphicsSystem::OnResize");
			return;
		}
		if (!m_depth_target->Resize(m_p_device->GetDevice(), args.width, args.height))
		{
			assert(0 && "failed DepthTarget::Resize() GraphicsSystem::OnResize");
			return;
		}
	}

	inline ID3D11Device* GraphicsSystem::Device() const
	{
		return m_p_device->GetDevice();
	}

	inline ID3D11DeviceContext* GraphicsSystem::IC() const
	{
		return m_p_device->GetIC();
	}

	inline ID3D11DeviceContext* GraphicsSystem::DC(int thread_idx, Graphics::DC_RENDER_PATH path_idx) const
	{
		return m_p_device->GetQueueGraphics()->GetCommandList(thread_idx, static_cast<int>(path_idx))->GetDC();
	}

	inline ID3D11DeviceContext* GraphicsSystem::DC(int thread_idx, Graphics::DC_COPY_PATH path_idx) const
	{
		return m_p_device->GetQueueCopy()->GetCommandList(thread_idx, static_cast<int>(path_idx))->GetDC();
	}

	inline ID3D11DeviceContext* GraphicsSystem::DC(int thread_idx, Graphics::DC_COMPUTE_PATH path_idx) const
	{
		return m_p_device->GetQueueCompute()->GetCommandList(thread_idx, static_cast<int>(path_idx))->GetDC();
	}

	inline IQueue* GraphicsSystem::GetQueueGraphics() const
	{
		return m_p_device->GetQueueGraphics();
	}

	//inline IQueue* GraphicsSystem::GetQueueCompute() const
	//{
	//	return m_p_device->GetQueueCompute();
	//}

	inline IQueue* GraphicsSystem::GetQueueCopy() const
	{
		return m_p_device->GetQueueCopy();
	}

	inline void GraphicsSystem::Present(int syncinterval)
	{
		m_p_device->Present(syncinterval);
	}

	void GraphicsSystem::SetViewPort(ID3D11DeviceContext* p_context, float width, float height, float top_left_x, float top_left_y)
	{
		D3D11_VIEWPORT vp = {};
		vp.Width = width;
		vp.Height = height;
		vp.MaxDepth = 1.0f;
		vp.MinDepth = 0.0f;
		vp.TopLeftX = top_left_x;
		vp.TopLeftY = top_left_y;

		p_context->RSSetViewports(1, &vp);
	}

	// TODO : this method is into Queue class method
	void GraphicsSystem::SetRTBackBuffer(ID3D11DeviceContext* p_context, bool set_depth)
	{
		ID3D11RenderTargetView* rtvs[] = {
			m_color_target->GetRTV()
		};
		// TODO : DepthTargetはポストエフェクト時にはSRVとして使ってるためセットされない？
		if (set_depth)
		{
			p_context->OMSetRenderTargets(1, rtvs, m_depth_target->GetDSV());
		}
		else
		{
			p_context->OMSetRenderTargets(1, rtvs, nullptr);
		}
	}

	void GraphicsSystem::ClearBackBuffer()
	{
		const auto context = m_p_device->GetIC();
		m_color_target->Clear(context, m_clear_color);
		m_depth_target->Clear(context);
	}

	//#ifdef USE_DEBUGGER
	//	void GraphicsSystem::LeakCheck()
	//	{
	//		// メモリリークが出た場合は、下記をコメントアウトすれば詳細情報が表示される.
	//		// 確認するためには表示するより前に、インターフェースをすべて開放する必要がある
	//		if (m_debug)
	//		{
	//			m_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	//	}
	//		m_debug.Reset();
	//}
	//#endif//USE_DEBUGGER

}	// namespace TKGEngine::Graphics