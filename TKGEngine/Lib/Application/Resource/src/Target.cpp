
#include "../inc/ITarget.h"

#include <cassert>

namespace TKGEngine
{
	namespace Graphics
	{
		/// ------------------------------------------------------------
		/// @brief	Convert to the proper depth format
		/// 
		/// @param[in] format		input format
		/// @param[in] tex_format	texture format
		/// @param[in] res_format	resource format
		/// ------------------------------------------------------------
		void ConvertDepthFormat
		(
			const DXGI_FORMAT& format,
			DXGI_FORMAT& tex_format,
			DXGI_FORMAT& res_format
		)
		{
			// Init by input format
			tex_format = format;
			res_format = format;

			// Get format
			switch (format)
			{
			case DXGI_FORMAT_D16_UNORM:
			{
				tex_format = DXGI_FORMAT_R16_TYPELESS;
				res_format = DXGI_FORMAT_R16_UNORM;
			}
			break;

			case DXGI_FORMAT_D24_UNORM_S8_UINT:
			{
				tex_format = DXGI_FORMAT_R24G8_TYPELESS;
				res_format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			}
			break;

			case DXGI_FORMAT_D32_FLOAT:
			{
				tex_format = DXGI_FORMAT_R32_TYPELESS;
				res_format = DXGI_FORMAT_R32_FLOAT;
			}
			break;

			case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
			{
				tex_format = DXGI_FORMAT_R32G8X24_TYPELESS;
				res_format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
			}
			break;
			}
		}

		/// ------------------------------------------------------------
		/// @brief	Convert to the proper color format
		/// 
		/// @param[in] format		input format
		/// @param[in] tex_format	texture format
		/// ------------------------------------------------------------
		void ConvertColorFormat
		(
			const DXGI_FORMAT& format,
			DXGI_FORMAT& tex_format
		)
		{
			tex_format = format;

			switch (format)
			{
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
			case DXGI_FORMAT_R32G32B32A32_UINT:
			case DXGI_FORMAT_R32G32B32A32_SINT:
			{
				tex_format = DXGI_FORMAT_R32G32B32A32_TYPELESS;
			}
			break;

			case DXGI_FORMAT_R32G32B32_FLOAT:
			case DXGI_FORMAT_R32G32B32_UINT:
			case DXGI_FORMAT_R32G32B32_SINT:
			{
				tex_format = DXGI_FORMAT_R32G32B32_TYPELESS;
			}
			break;

			case DXGI_FORMAT_R16G16B16A16_FLOAT:
			case DXGI_FORMAT_R16G16B16A16_UNORM:
			case DXGI_FORMAT_R16G16B16A16_UINT:
			case DXGI_FORMAT_R16G16B16A16_SNORM:
			case DXGI_FORMAT_R16G16B16A16_SINT:
			{
				tex_format = DXGI_FORMAT_R16G16B16A16_TYPELESS;
			}
			break;

			case DXGI_FORMAT_R32G32_FLOAT:
			case DXGI_FORMAT_R32G32_UINT:
			case DXGI_FORMAT_R32G32_SINT:
			{
				tex_format = DXGI_FORMAT_R32G32_TYPELESS;
			}
			break;

			case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
			{
				tex_format = DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
			}
			break;

			case DXGI_FORMAT_R10G10B10A2_UNORM:
			case DXGI_FORMAT_R10G10B10A2_UINT:
			{
				tex_format = DXGI_FORMAT_R10G10B10A2_TYPELESS;
			}
			break;

			case DXGI_FORMAT_R8G8B8A8_UNORM:
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			case DXGI_FORMAT_R8G8B8A8_UINT:
			case DXGI_FORMAT_R8G8B8A8_SNORM:
			case DXGI_FORMAT_R8G8B8A8_SINT:
			{
				tex_format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
			}
			break;

			case DXGI_FORMAT_R16G16_FLOAT:
			case DXGI_FORMAT_R16G16_UNORM:
			case DXGI_FORMAT_R16G16_UINT:
			case DXGI_FORMAT_R16G16_SNORM:
			case DXGI_FORMAT_R16G16_SINT:
			{
				tex_format = DXGI_FORMAT_R16G16_TYPELESS;
			}
			break;

			case DXGI_FORMAT_R32_FLOAT:
			case DXGI_FORMAT_R32_UINT:
			case DXGI_FORMAT_R32_SINT:
			{
				tex_format = DXGI_FORMAT_R32_TYPELESS;
			}
			break;

			case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			{
				tex_format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			}
			break;

			case DXGI_FORMAT_R8G8_UNORM:
			case DXGI_FORMAT_R8G8_UINT:
			case DXGI_FORMAT_R8G8_SNORM:
			case DXGI_FORMAT_R8G8_SINT:
			{
				tex_format = DXGI_FORMAT_R8G8_TYPELESS;
			}
			break;

			case DXGI_FORMAT_R16_FLOAT:
			case DXGI_FORMAT_R16_UNORM:
			case DXGI_FORMAT_R16_UINT:
			case DXGI_FORMAT_R16_SNORM:
			case DXGI_FORMAT_R16_SINT:
			{
				tex_format = DXGI_FORMAT_R16_TYPELESS;
			}
			break;

			case DXGI_FORMAT_R8_UNORM:
			case DXGI_FORMAT_R8_UINT:
			case DXGI_FORMAT_R8_SNORM:
			case DXGI_FORMAT_R8_SINT:
			{
				tex_format = DXGI_FORMAT_R8_TYPELESS;
			}
			break;

			case DXGI_FORMAT_BC1_UNORM:
			case DXGI_FORMAT_BC1_UNORM_SRGB:
			{
				tex_format = DXGI_FORMAT_BC1_TYPELESS;
			}
			break;

			case DXGI_FORMAT_BC2_UNORM:
			case DXGI_FORMAT_BC2_UNORM_SRGB:
			{
				tex_format = DXGI_FORMAT_BC2_TYPELESS;
			}
			break;

			case DXGI_FORMAT_BC3_UNORM:
			case DXGI_FORMAT_BC3_UNORM_SRGB:
			{
				tex_format = DXGI_FORMAT_BC3_TYPELESS;
			}
			break;

			case DXGI_FORMAT_BC4_UNORM:
			case DXGI_FORMAT_BC4_SNORM:
			{
				tex_format = DXGI_FORMAT_BC4_TYPELESS;
			}
			break;

			case DXGI_FORMAT_BC5_UNORM:
			case DXGI_FORMAT_BC5_SNORM:
			{
				tex_format = DXGI_FORMAT_BC5_TYPELESS;
			}
			break;

			case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			{
				tex_format = DXGI_FORMAT_B8G8R8A8_TYPELESS;
			}
			break;

			case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			{
				tex_format = DXGI_FORMAT_B8G8R8X8_TYPELESS;
			}
			break;

			case DXGI_FORMAT_BC6H_UF16:
			case DXGI_FORMAT_BC6H_SF16:
			{
				tex_format = DXGI_FORMAT_BC6H_TYPELESS;
			}
			break;

			case DXGI_FORMAT_BC7_UNORM:
			case DXGI_FORMAT_BC7_UNORM_SRGB:
			{
				tex_format = DXGI_FORMAT_BC7_TYPELESS;
			}
			break;
			}
		}
	}	// namespace Graphics


	/////////////////////////////////////////////////////////////
	//
	// ColorTarget
	//
	/////////////////////////////////////////////////////////////
#pragma region ColorTarget
	/////////////////////////////////////
	/// @class	ColorTarget
	/// @brief	have SRV and RTV
	///
	/////////////////////////////////////
	class ColorTarget
		: public IColorTarget
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		ColorTarget() = default;
		virtual ~ColorTarget();
		ColorTarget(const ColorTarget&) = delete;
		ColorTarget(ColorTarget&&) = default;
		ColorTarget& operator=(const ColorTarget&) = delete;

		bool Create(ID3D11Device* p_device, IDXGISwapChain* p_swapchain) override;
		bool Create(ID3D11Device* p_device, const TargetDesc& desc, bool create_rtv, bool create_uav) override;

		bool Resize(ID3D11Device* p_device, IDXGISwapChain* p_swapchain) override;
		bool Resize(ID3D11Device* p_device, UINT width, UINT height) override;

		void Release() override;

		void SetRTV(ID3D11DeviceContext* p_context) override;
		void RemoveRTV(ID3D11DeviceContext* p_context) override;

		void SetSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility) override;

		void SetUAV(ID3D11DeviceContext* p_context, int slot = 0) override;
		void RemoveUAV(ID3D11DeviceContext* p_context, int slot = 0) override;

		void Clear(ID3D11DeviceContext* p_context) override;
		void Clear(ID3D11DeviceContext* p_context, const float(&color)[4]) override;

		ID3D11Texture2D* const GetResource() const override;
		ID3D11ShaderResourceView* const GetSRV() const override;
		ID3D11ShaderResourceView* const* GetAddressOfSRV() const override;
		ID3D11RenderTargetView* const GetRTV() const override;
		ID3D11UnorderedAccessView* const GetUAV() const override;
		ID3D11UnorderedAccessView* const* GetAddressOfUAV() const override;
		TargetDesc GetDesc() const override;

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================

#ifdef USE_IMGUI
		void OnGUI() override;
#endif // USE_IMGUI


		// ==============================================
		// private variables
		// ==============================================
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture = nullptr;		//!< texture resource
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV = nullptr;	//!< SRV
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RTV = nullptr;		//!< RTV
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_UAV = nullptr;	//!< UAV
		bool m_has_rtv = false;
		bool m_has_uav = false;
		TargetDesc m_desc;	//!< target description
	};

	std::unique_ptr<IColorTarget> IColorTarget::CreateInterface()
	{
		return std::make_unique<ColorTarget>();
	}

	ColorTarget::~ColorTarget()
	{
		Release();
	}

	bool ColorTarget::Create(ID3D11Device* p_device, IDXGISwapChain* p_swapchain)
	{
		HRESULT hr = S_OK;

		// Get back buffer
		{
			hr = p_swapchain->GetBuffer(0, IID_PPV_ARGS(m_texture.GetAddressOf()));
			if (FAILED(hr))
			{
				assert(0 && "failed IDXGISwapChain->GetBuffer() ColorTarget::Create()");
				return false;
			}
		}

		// Create RTV
		{
			hr = p_device->CreateRenderTargetView(
				m_texture.Get(), nullptr, m_RTV.GetAddressOf());
			if (FAILED(hr))
			{
				m_texture.Reset();

				assert(0 && "failed ID3D11Device->CreateRenderTargetView() ColorTarget::Create()");
				return false;
			}
		}

		// Create SRV
		{
			hr = p_device->CreateShaderResourceView(
				m_texture.Get(), nullptr, m_SRV.GetAddressOf());
			if (FAILED(hr))
			{
				m_texture.Reset();
				m_RTV.Reset();

				assert(0 && "failed ID3D11Device->CreateShaderResourceView() ColorTarget::Create()");
				return false;
			}
		}

		// Get back buffer description
		D3D11_TEXTURE2D_DESC desc = {};
		m_texture->GetDesc(&desc);
		m_desc.width = desc.Width;
		m_desc.height = desc.Height;
		m_desc.array_size = desc.ArraySize;
		m_desc.CPU_access_flags = desc.CPUAccessFlags;
		m_desc.format = desc.Format;
		m_desc.mip_levels = desc.MipLevels;
		m_desc.sample_desc = desc.SampleDesc;

		return true;
	}

	bool ColorTarget::Create(ID3D11Device* p_device, const TargetDesc& desc, bool create_rtv, bool create_uav)
	{
		assert(desc.array_size >= 1);
		HRESULT hr = S_OK;

		// Get texture format
		DXGI_FORMAT tex_format;
		Graphics::ConvertColorFormat(desc.format, tex_format);
		m_has_rtv = create_rtv;
		m_has_uav = create_uav;

		// Configurate texture
		D3D11_TEXTURE2D_DESC tex_desc = {};
		tex_desc.Width = desc.width;
		tex_desc.Height = desc.height;
		tex_desc.MipLevels = desc.mip_levels;
		tex_desc.ArraySize = desc.array_size;
		tex_desc.Format = tex_format;
		tex_desc.SampleDesc.Count = desc.sample_desc.Count;
		tex_desc.SampleDesc.Quality = desc.sample_desc.Quality;
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		if (create_uav)
			tex_desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
		tex_desc.CPUAccessFlags = desc.CPU_access_flags;
		tex_desc.MiscFlags = 0;
		if (desc.mip_levels != 1)
			tex_desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

		// Create texture
		hr = p_device->CreateTexture2D(&tex_desc, nullptr, m_texture.GetAddressOf());
		if (FAILED(hr))
		{
			assert(0 && "failed ID3D11Device->CreateTexture2D() ColorTarget::Create()");
			return false;
		}

		// RTV
		if(create_rtv)
		{
			// Create RTV
			D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
			rtvd.Format = desc.format;

			// If the array size is 1
			if (desc.array_size == 1)
			{
				// no multi sampling
				if (desc.sample_desc.Count <= 1)
				{
					rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
					rtvd.Texture2D.MipSlice = 0;
				}
				// multi sampling
				else
				{
					rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
				}
			}
			// If the array size is greater than 1
			else
			{
				// no multi sampling
				if (desc.sample_desc.Count <= 1)
				{
					rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
					rtvd.Texture2DArray.ArraySize = desc.array_size;
					rtvd.Texture2DArray.FirstArraySlice = 0;
					rtvd.Texture2DArray.MipSlice = 0;
				}
				// multi sampling
				else
				{
					rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
					rtvd.Texture2DMSArray.ArraySize = desc.array_size;
					rtvd.Texture2DMSArray.FirstArraySlice = 0;
				}
			}

			// Create RTV
			hr = p_device->CreateRenderTargetView(m_texture.Get(), &rtvd, m_RTV.GetAddressOf());
			if (FAILED(hr))
			{
				m_texture.Reset();

				assert(0 && "failed ID3D11Device->CreateRenderTargetView ColorTarget::Create()");
				return false;
			}
		}

		// SRV
		{
			// Configurate SRV
			D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
			srvd.Format = desc.format;

			// If the array size is 1
			if (desc.array_size == 1)
			{
				// no multi sampling
				if (desc.sample_desc.Count <= 1)
				{
					srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					srvd.Texture2D.MipLevels = (desc.mip_levels == 0) ? -1 : desc.mip_levels;
					srvd.Texture2D.MostDetailedMip = 0;
				}
				// multi sampling
				else
				{
					srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
				}
			}
			// If the array size is greater than 1
			else
			{
				// no multi sampling
				if (desc.sample_desc.Count <= 1)
				{
					srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
					srvd.Texture2DArray.ArraySize = desc.array_size;
					srvd.Texture2DArray.FirstArraySlice = 0;
					srvd.Texture2DArray.MipLevels = (desc.mip_levels == 0) ? -1 : desc.mip_levels;
					srvd.Texture2DArray.MostDetailedMip = 0;
				}
				// multi sampling
				else
				{
					srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
					srvd.Texture2DMSArray.ArraySize = desc.array_size;
					srvd.Texture2DMSArray.FirstArraySlice = 0;
				}
			}

			// Create SRV
			hr = p_device->CreateShaderResourceView(m_texture.Get(), &srvd, m_SRV.GetAddressOf());
			if (FAILED(hr))
			{
				m_texture.Reset();
				m_RTV.Reset();

				assert(0 && "failed ID3D11Device->CreateShaderResourceView() ColorTarget::Create()");
				return false;
			}
		}
		// http://www.project-asura.com/program/d3d11/d3d11_007.html
		// UAV
		if(create_uav)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavd = {};
			uavd.Format = desc.format;
			if (desc.array_size == 1)
			{
				uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
				uavd.Texture2D.MipSlice = 0;
			}
			else
			{
				uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
				uavd.Texture2DArray.ArraySize = desc.array_size;
				uavd.Texture2DArray.FirstArraySlice = 0;
				uavd.Texture2DArray.MipSlice = 0;
			}

			hr = p_device->CreateUnorderedAccessView(m_texture.Get(), &uavd, m_UAV.ReleaseAndGetAddressOf());
			if (FAILED(hr))
			{
				m_texture.Reset();
				m_RTV.Reset();
				m_SRV.Reset();

				assert(0 && "failed ID3D11Device->CreateUnorderedAccessView() ColorTarget::Create()");
				return false;
			}
		}

		m_desc = desc;

		return true;
	}

	bool ColorTarget::Resize(ID3D11Device* p_device, IDXGISwapChain* p_swapchain)
	{
		m_RTV.Reset();
		m_SRV.Reset();
		m_UAV.Reset();
		m_texture.Reset();
		m_desc.Reset();

		return Create(p_device, p_swapchain);
	}

	bool ColorTarget::Resize(ID3D11Device* p_device, UINT width, UINT height)
	{
		m_RTV.Reset();
		m_SRV.Reset();
		m_UAV.Reset();
		m_texture.Reset();

		m_desc.width = width;
		m_desc.height = height;

		return Create(p_device, m_desc, m_has_rtv, m_has_uav);
	}

	void ColorTarget::Release()
	{
		m_SRV.Reset();
		m_RTV.Reset();
		m_texture.Reset();
	}

	void ColorTarget::SetRTV(ID3D11DeviceContext* p_context)
	{
		ID3D11RenderTargetView* rtvs[] = {m_RTV.Get()};
		p_context->OMSetRenderTargets(1, rtvs, nullptr);
	}

	void ColorTarget::RemoveRTV(ID3D11DeviceContext* p_context)
	{
		ID3D11RenderTargetView* rtvs[] = { nullptr };
		p_context->OMSetRenderTargets(1, rtvs, nullptr);
	}

	void ColorTarget::SetSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility)
	{
		ID3D11ShaderResourceView* srvs[] =
		{
			m_SRV.Get()
		};

		switch (visibility)
		{
		case ShaderVisibility::VS:
			p_context->VSSetShaderResources(slot, 1, srvs);
			break;

		case ShaderVisibility::PS:
			p_context->PSSetShaderResources(slot, 1, srvs);
			break;

		case ShaderVisibility::GS:
			p_context->GSSetShaderResources(slot, 1, srvs);
			break;

		case ShaderVisibility::DS:
			p_context->DSSetShaderResources(slot, 1, srvs);
			break;

		case ShaderVisibility::HS:
			p_context->HSSetShaderResources(slot, 1, srvs);
			break;

		case ShaderVisibility::ALL:
			p_context->VSSetShaderResources(slot, 1, srvs);
			p_context->PSSetShaderResources(slot, 1, srvs);
			p_context->GSSetShaderResources(slot, 1, srvs);
			p_context->DSSetShaderResources(slot, 1, srvs);
			p_context->HSSetShaderResources(slot, 1, srvs);
			break;

		case ShaderVisibility::CS:
			p_context->CSSetShaderResources(slot, 1, srvs);
			break;
		}
	}

	void ColorTarget::SetUAV(ID3D11DeviceContext* p_context, int slot)
	{
		ID3D11UnorderedAccessView* uavs[] =
		{
			m_UAV.Get()
		};

		p_context->CSSetUnorderedAccessViews(slot, 1, uavs, nullptr);
	}

	void ColorTarget::RemoveUAV(ID3D11DeviceContext* p_context, int slot)
	{
		ID3D11UnorderedAccessView* uavs[] =
		{
			nullptr
		};

		p_context->CSSetUnorderedAccessViews(slot, 1, uavs, nullptr);
	}

	void ColorTarget::Clear(ID3D11DeviceContext* p_context)
	{
		float color[4] = { 0.0f,0.0f,0.0f,0.0f };
		p_context->ClearRenderTargetView(m_RTV.Get(), color);
	}

	void ColorTarget::Clear(ID3D11DeviceContext* p_context, const float(&color)[4])
	{
		p_context->ClearRenderTargetView(m_RTV.Get(), color);
	}

	ID3D11Texture2D* const ColorTarget::GetResource() const
	{
		return m_texture.Get();
	}

	ID3D11ShaderResourceView* const ColorTarget::GetSRV() const
	{
		return m_SRV.Get();
	}

	ID3D11ShaderResourceView* const* ColorTarget::GetAddressOfSRV() const
	{
		return m_SRV.GetAddressOf();
	}

	ID3D11RenderTargetView* const ColorTarget::GetRTV() const
	{
		return m_RTV.Get();
	}

	ID3D11UnorderedAccessView* const ColorTarget::GetUAV() const
	{
		return m_UAV.Get();
	}

	ID3D11UnorderedAccessView* const* ColorTarget::GetAddressOfUAV() const
	{
		return m_UAV.GetAddressOf();
	}

	TargetDesc ColorTarget::GetDesc() const
	{
		return m_desc;
	}

#ifdef USE_IMGUI
	void ColorTarget::OnGUI()
	{
		// テクスチャを持っていたら表示する
		if (m_SRV)
		{
			// テクスチャデータ
			{
				// W and H
				ImGui::Text("Width:(%d) Height(%d)", m_desc.width, m_desc.height);
				// Format
				ImGui::Text(NAMEOF_ENUM(m_desc.format).data());
				// array size
				ImGui::Text("Array size(%d)", m_desc.array_size);
				// mip level
				ImGui::Text("Mip Level(%d)", m_desc.mip_levels);
			}
			// 画像
			{
				const float button_width = ImGui::GetWindowWidth() * 0.3f;
				ImGui::ImageWindowButton(
					m_SRV.Get(), this,
					ImVec2(static_cast<float>(m_desc.width), static_cast<float>(m_desc.height)),
					ImVec2(button_width, button_width)
				);
			}
		}
		// なければテキスト表示
		else
		{
			ImGui::TextColored(ImVec4(0.8f, 0.0f, 0.0f, 1.0f), "No Target.");
		}
	}
#endif // USE_IMGUI

#pragma endregion


	/////////////////////////////////////////////////////////////
	//
	// DepthTarget
	//
	/////////////////////////////////////////////////////////////
#pragma region DepthTarget
	/////////////////////////////////////
	/// @class	DepthTarget
	/// @brief	have DSV and SRV
	///
	/////////////////////////////////////
	class DepthTarget
		: public IDepthTarget
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		DepthTarget() = default;
		virtual ~DepthTarget();
		DepthTarget(const DepthTarget&) = delete;
		DepthTarget(DepthTarget&&) = default;
		DepthTarget& operator=(const DepthTarget&) = delete;

		bool Create(ID3D11Device* p_device, const TargetDesc& desc, bool create_uav) override;

		bool Resize(ID3D11Device* p_device, UINT width, UINT height) override;

		void Release() override;

		void SetRTV(ID3D11DeviceContext* p_context) override;
		void RemoveRTV(ID3D11DeviceContext* p_context) override;

		void SetSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility) override;

		void Clear(ID3D11DeviceContext* p_context) override;

		ID3D11Texture2D* const GetResource() const override;
		ID3D11ShaderResourceView* const GetSRV() const override;
		ID3D11ShaderResourceView* const* GetAddressOfSRV() const override;
		ID3D11DepthStencilView* const GetDSV() const override;
		TargetDesc GetDesc() const override;
		
		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================

#ifdef USE_IMGUI
		void OnGUI() override;
#endif // USE_IMGUI


		// ==============================================
		// private variables
		// ==============================================
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture = nullptr;	//!< texture resource
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV = nullptr;	//!< SRV
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DSV = nullptr;		//!< DSV
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_UAV = nullptr;	//!< UAV
		bool m_has_uav = false;
		TargetDesc m_desc;	//!< target description
	};

	std::unique_ptr<IDepthTarget> IDepthTarget::CreateInterface()
	{
		return std::make_unique<DepthTarget>();
	}

	DepthTarget::~DepthTarget()
	{
		Release();
	}

	bool DepthTarget::Create(ID3D11Device* p_device, const TargetDesc& desc, bool create_uav)
	{
		assert(desc.array_size >= 1);
		HRESULT hr = S_OK;

		DXGI_FORMAT tex_format;
		DXGI_FORMAT res_format;
		Graphics::ConvertDepthFormat(desc.format, tex_format, res_format);
		m_has_uav = create_uav;

		// Texture2D
		{
			// Configurate Texture
			D3D11_TEXTURE2D_DESC texd = {};
			texd.Width = desc.width;
			texd.Height = desc.height;
			texd.MipLevels = desc.mip_levels;
			texd.ArraySize = desc.array_size;
			texd.Format = tex_format;
			texd.SampleDesc.Count = desc.sample_desc.Count;
			texd.SampleDesc.Quality = desc.sample_desc.Quality;
			texd.Usage = D3D11_USAGE_DEFAULT;
			texd.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			texd.CPUAccessFlags = desc.CPU_access_flags;
			texd.MiscFlags = 0;
			if (m_desc.mip_levels != 1)
				texd.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

			// Create Texture
			hr = p_device->CreateTexture2D(&texd, nullptr, m_texture.GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device->CreateTexture2D() DepthTarget::Create()");
				return false;
			}
		}

		// DSV
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
			dsvd.Format = desc.format;

			// If the array size is 1
			if (desc.array_size == 1)
			{
				// no multi sampling
				if (desc.sample_desc.Count <= 1)
				{
					dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
					dsvd.Texture2D.MipSlice = 0;
				}
				// multi sampling
				else
				{
					dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
				}
			}
			// If the array size is greater than 1
			else
			{
				// no multi sampling
				if (desc.sample_desc.Count <= 1)
				{
					dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
					dsvd.Texture2DArray.ArraySize = desc.array_size;
					dsvd.Texture2DArray.FirstArraySlice = 0;
					dsvd.Texture2DArray.MipSlice = 0;
				}
				// multi sampling
				else
				{
					dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
					dsvd.Texture2DMSArray.ArraySize = desc.array_size;
					dsvd.Texture2DMSArray.FirstArraySlice = 0;
				}
			}

			// Create DSV
			hr = p_device->CreateDepthStencilView(m_texture.Get(), &dsvd, m_DSV.GetAddressOf());
			if (FAILED(hr))
			{
				m_texture.Reset();

				assert(0 && "failed ID3D11Device->CreateDepthStencilView() DepthTarget::Create()");
				return false;
			}
		}

		// SRV
		{
			// Configurate SRV
			D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
			srvd.Format = res_format;

			// If the array size is 1
			if (desc.array_size == 1)
			{
				// no multi sampling
				if (desc.sample_desc.Count <= 1)
				{
					srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					srvd.Texture2D.MipLevels = desc.mip_levels;
					srvd.Texture2D.MostDetailedMip = 0;
				}
				// multi sampling
				else
				{
					srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
				}
			}
			// If the array size is greater than 1
			else
			{
				// no multi sampling
				if (desc.sample_desc.Count <= 1)
				{
					srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
					srvd.Texture2DArray.ArraySize = desc.array_size;
					srvd.Texture2DArray.FirstArraySlice = 0;
					srvd.Texture2DArray.MipLevels = desc.mip_levels;
					srvd.Texture2DArray.MostDetailedMip = 0;
				}
				// multi sampling
				else
				{
					srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
					srvd.Texture2DMSArray.ArraySize = desc.array_size;
					srvd.Texture2DMSArray.FirstArraySlice = 0;
				}
			}

			// Create SRV
			hr = p_device->CreateShaderResourceView(m_texture.Get(), &srvd, m_SRV.GetAddressOf());
			if (FAILED(hr))
			{
				m_texture.Reset();
				m_DSV.Reset();

				assert(0 && "failed ID3D11Device->CreateShaderResourceView() DepthTarget::Create()");
				return false;
			}
		}
		//// UAV
		//if (create_uav == true)
		//{
		//	D3D11_UNORDERED_ACCESS_VIEW_DESC uavd = {};
		//	uavd.Format = desc.format;
		//	if (desc.array_size == 1)
		//	{
		//		uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		//		uavd.Texture2D.MipSlice = 0;
		//	}
		//	else
		//	{
		//		uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
		//		uavd.Texture2DArray.ArraySize = desc.array_size;
		//		uavd.Texture2DArray.FirstArraySlice = 0;
		//		uavd.Texture2DArray.MipSlice = 0;
		//	}

		//	hr = p_device->CreateUnorderedAccessView(m_texture.Get(), &uavd, m_UAV.ReleaseAndGetAddressOf());
		//	if (FAILED(hr))
		//	{
		//		m_texture.Reset();
		//		m_DSV.Reset();
		//		m_SRV.Reset();

		//		assert(0 && "failed ID3D11Device->CreateUnorderedAccessView() DepthTarget::Create()");
		//		return false;
		//	}
		//}

		m_desc = desc;

		return true;
	}

	bool DepthTarget::Resize(ID3D11Device* p_device, UINT width, UINT height)
	{
		m_SRV.Reset();
		m_DSV.Reset();
		m_texture.Reset();

		m_desc.width = width;
		m_desc.height = height;

		return Create(p_device, m_desc, m_has_uav);
	}

	void DepthTarget::Release()
	{
		m_SRV.Reset();
		m_DSV.Reset();
		m_texture.Reset();
		//m_desc.Reset();
	}

	void DepthTarget::SetRTV(ID3D11DeviceContext* p_context)
	{
		p_context->OMSetRenderTargets(0, nullptr, m_DSV.Get());
	}

	void DepthTarget::RemoveRTV(ID3D11DeviceContext* p_context)
	{
		p_context->OMSetRenderTargets(0, nullptr, nullptr);
	}

	void DepthTarget::SetSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility)
	{
		switch (visibility)
		{
		case ShaderVisibility::VS:
			p_context->VSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::PS:
			p_context->PSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::GS:
			p_context->GSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::DS:
			p_context->DSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::HS:
			p_context->HSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::ALL:
			p_context->VSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			p_context->PSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			p_context->GSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			p_context->DSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			p_context->HSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::CS:
			p_context->CSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;
		}
	}

	void DepthTarget::Clear(ID3D11DeviceContext* p_context)
	{
		p_context->ClearDepthStencilView(m_DSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);
	}

	ID3D11Texture2D* const DepthTarget::GetResource() const
	{
		return m_texture.Get();
	}

	ID3D11ShaderResourceView* const DepthTarget::GetSRV() const
	{
		return m_SRV.Get();
	}

	ID3D11ShaderResourceView* const* DepthTarget::GetAddressOfSRV() const
	{
		return m_SRV.GetAddressOf();
	}

	ID3D11DepthStencilView* const DepthTarget::GetDSV() const
	{
		return m_DSV.Get();
	}

	TargetDesc DepthTarget::GetDesc() const
	{
		return m_desc;
	}
	
#ifdef USE_IMGUI
	void DepthTarget::OnGUI()
	{
		// テクスチャを持っていたら表示する
		if (m_SRV)
		{
			// テクスチャデータ
			{
				// W and H
				ImGui::Text("Width:(%d) Height(%d)", m_desc.width, m_desc.height);
				// Format
				ImGui::Text(NAMEOF_ENUM(m_desc.format).data());
				// array size
				ImGui::Text("Array size(%d)", m_desc.array_size);
				// mip level
				ImGui::Text("Mip Level(%d)", m_desc.mip_levels);
			}
			// 画像
			{
				const float button_width = ImGui::GetWindowWidth() * 0.3f;
				ImGui::ImageWindowButton(
					m_SRV.Get(), this,
					ImVec2(static_cast<float>(m_desc.width), static_cast<float>(m_desc.height)),
					ImVec2(button_width, button_width)
				);
			}
		}
		// なければテキスト表示
		else
		{
			ImGui::TextColored(ImVec4(0.8f, 0.0f, 0.0f, 1.0f), "No Target.");
		}
	}
#endif // USE_IMGUI

#pragma endregion


	/////////////////////////////////////////////////////////////
	//
	// ColorTargetCube
	//
	/////////////////////////////////////////////////////////////
#pragma region ColorTargetCube
	/////////////////////////////////////
	/// @class	ColorTargetCube
	/// @brief	have SRV and RTVs as Cube
	///
	/////////////////////////////////////
	class ColorTargetCube
		: public IColorTargetCube
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		ColorTargetCube() = default;
		virtual ~ColorTargetCube();
		ColorTargetCube(const ColorTargetCube&) = delete;
		ColorTargetCube(ColorTargetCube&&) = default;
		ColorTargetCube& operator=(const ColorTargetCube&) = delete;

		bool Create(ID3D11Device* p_device, const CubeDesc& desc) override;

		bool Resize(ID3D11Device* p_device, UINT size) override;

		void Release() override;

		void SetRTV(ID3D11DeviceContext* p_context) override;
		void RemoveRTV(ID3D11DeviceContext* p_context) override;

		void SetSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility) override;

		void Clear(ID3D11DeviceContext* p_context, const float(&color)[4]) override;

		ID3D11Texture2D* const GetResource() const override;
		ID3D11ShaderResourceView* const GetSRV() const override;
		ID3D11RenderTargetView* const* GetAddressOfRTV() const override;
		CubeDesc GetDesc() const override;
		

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================
		/* nothing */


		// ==============================================
		// private variables
		// ==============================================
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture = nullptr;	//!< texture resource
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV = nullptr;	//!< SRV
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RTV = nullptr;	//!< RTV
		CubeDesc m_desc;	//!< cube target description
	};


	std::unique_ptr<IColorTargetCube> IColorTargetCube::CreateInterface()
	{
		return std::make_unique<ColorTargetCube>();
	}

	ColorTargetCube::~ColorTargetCube()
	{
		Release();
	}

	bool ColorTargetCube::Create(ID3D11Device* p_device, const CubeDesc& desc)
	{
		HRESULT hr = S_OK;

		// Get texture format
		DXGI_FORMAT tex_format;
		Graphics::ConvertColorFormat(desc.format, tex_format);

		// Configurate texture
		D3D11_TEXTURE2D_DESC tex_desc = {};
		tex_desc.Width = desc.cube_size;
		tex_desc.Height = desc.cube_size;
		tex_desc.MipLevels = desc.mip_levels;
		tex_desc.ArraySize = 6;
		tex_desc.Format = tex_format;
		tex_desc.SampleDesc.Count = 1;
		tex_desc.SampleDesc.Quality = 0;
		tex_desc.Usage = D3D11_USAGE_DEFAULT;
		tex_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		tex_desc.CPUAccessFlags = desc.CPU_access_flags;
		tex_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

		// Create texture
		hr = p_device->CreateTexture2D(&tex_desc, nullptr, m_texture.GetAddressOf());
		if (FAILED(hr))
		{
			assert(0 && "failed ID3D11Device->CreateTexture2D() ColorTargetCube::Create()");
			return false;
		}

		// RTV
		{
			// TODO : マルチサンプリングなしにのみ対応中
			// Create RTV
			D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
			rtvd.Format = desc.format;

			rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			//rtvd.Texture2DArray.ArraySize = 1;
			rtvd.Texture2DArray.ArraySize = 6;
			rtvd.Texture2DArray.MipSlice = 0;
			rtvd.Texture2DArray.FirstArraySlice = 0;

			// Create RTVs
			//for (int i = 0; i < 6; ++i)
			{
				hr = p_device->CreateRenderTargetView(m_texture.Get(), &rtvd, m_RTV.GetAddressOf());
				if (FAILED(hr))
				{
					m_texture.Reset();

					assert(0 && "failed ID3D11Device->CreateRenderTargetView ColorTargetCube::Create()");
					return false;
				}
			}
		}

		// SRV
		{
			// Configurate SRV
			D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
			srvd.Format = desc.format;

			srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			srvd.TextureCube.MipLevels = desc.mip_levels;
			srvd.TextureCube.MostDetailedMip = 0;

			// Create SRV
			hr = p_device->CreateShaderResourceView(m_texture.Get(), &srvd, m_SRV.GetAddressOf());
			if (FAILED(hr))
			{
				m_texture.Reset();
				//for (auto&& v : m_RTVs)
				//{
				//	v.Reset();
				//}
				m_RTV.Reset();

				assert(0 && "failed ID3D11Device->CreateShaderResourceView() ColorTargetCube::Create()");
				return false;
			}
		}
		m_desc = desc;

		return true;
	}

	bool ColorTargetCube::Resize(ID3D11Device* p_device, UINT size)
	{
		//for (auto&& v : m_RTVs)
		//{
		//	v.Reset();
		//}
		m_RTV.Reset();
		m_SRV.Reset();
		m_texture.Reset();

		m_desc.cube_size = size;

		return Create(p_device, m_desc);
	}

	void ColorTargetCube::Release()
	{
		m_SRV.Reset();
		//for (auto&& v : m_RTVs)
		//{
		//	v.Reset();
		//}
		m_RTV.Reset();
		m_texture.Reset();
		//m_desc.Reset();
	}

	void ColorTargetCube::SetRTV(ID3D11DeviceContext* p_context)
	{
		ID3D11RenderTargetView* rtvs[] = { m_RTV.Get() };
		p_context->OMSetRenderTargets(1, rtvs, nullptr);
	}

	void ColorTargetCube::RemoveRTV(ID3D11DeviceContext* p_context)
	{
		ID3D11RenderTargetView* rtvs[] = { nullptr };
		p_context->OMSetRenderTargets(1, rtvs, nullptr);
	}

	void ColorTargetCube::SetSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility)
	{
		switch (visibility)
		{
		case ShaderVisibility::VS:
			p_context->VSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::PS:
			p_context->PSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::GS:
			p_context->GSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::DS:
			p_context->DSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::HS:
			p_context->HSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::ALL:
			p_context->VSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			p_context->PSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			p_context->GSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			p_context->DSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			p_context->HSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::CS:
			p_context->CSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;
		}
	}

	void ColorTargetCube::Clear(ID3D11DeviceContext* p_context, const float(&color)[4])
	{
		p_context->ClearRenderTargetView(m_RTV.Get(), color);
	}

	ID3D11Texture2D* const ColorTargetCube::GetResource() const
	{
		return m_texture.Get();
	}

	ID3D11ShaderResourceView* const ColorTargetCube::GetSRV() const
	{
		return m_SRV.Get();
	}

	ID3D11RenderTargetView* const* ColorTargetCube::GetAddressOfRTV() const
	{
		return m_RTV.GetAddressOf();
	}

	CubeDesc ColorTargetCube::GetDesc() const
	{
		return m_desc;
	}
#pragma endregion


	/////////////////////////////////////////////////////////////
	//
	// DepthTargetCube
	//
	/////////////////////////////////////////////////////////////
#pragma region DepthTargetCube
	/////////////////////////////////////
	/// @class	DepthTargetCube
	/// @brief	have DSVs and SRV as Cube
	///
	/////////////////////////////////////
	class DepthTargetCube
		: public IDepthTargetCube
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		DepthTargetCube() = default;
		virtual ~DepthTargetCube();
		DepthTargetCube(const DepthTargetCube&) = delete;
		DepthTargetCube(DepthTargetCube&&) = default;
		DepthTargetCube& operator=(const DepthTargetCube&) = delete;

		bool Create(ID3D11Device* p_device, const CubeDesc& desc) override;

		bool Resize(ID3D11Device* p_device, UINT size) override;

		void Release() override;

		void SetRTV(ID3D11DeviceContext* p_context) override;
		void RemoveRTV(ID3D11DeviceContext* p_context) override;

		void SetSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility) override;

		void Clear(ID3D11DeviceContext* p_context) override;

		ID3D11Texture2D* const GetResource() const override;
		ID3D11ShaderResourceView* const GetSRV() const override;
		ID3D11DepthStencilView* const* GetDSV() const override;
		CubeDesc GetDesc() const override;


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================
		/* nothing */


		// ==============================================
		// private variables
		// ==============================================
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture = nullptr;	//!< texture resource
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV = nullptr;	//!< SRV
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DSV = nullptr;	//!< DSV
		CubeDesc m_desc;	//!< cube target description
	};

	std::unique_ptr<IDepthTargetCube> IDepthTargetCube::CreateInterface()
	{
		return std::make_unique<DepthTargetCube>();
	}

	DepthTargetCube::~DepthTargetCube()
	{
		Release();
	}

	bool DepthTargetCube::Create(ID3D11Device* p_device, const CubeDesc& desc)
	{
		HRESULT hr = S_OK;

		DXGI_FORMAT tex_format;
		DXGI_FORMAT res_format;
		Graphics::ConvertDepthFormat(desc.format, tex_format, res_format);

		// Texture2D
		{
			// Configurate Texture
			D3D11_TEXTURE2D_DESC texd = {};
			texd.Width = desc.cube_size;
			texd.Height = desc.cube_size;
			texd.MipLevels = desc.mip_levels;
			texd.ArraySize = 6;
			texd.Format = tex_format;
			texd.SampleDesc.Count = 1;
			texd.SampleDesc.Quality = 0;
			texd.Usage = D3D11_USAGE_DEFAULT;
			texd.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			texd.CPUAccessFlags = desc.CPU_access_flags;
			texd.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

			// Create Texture
			hr = p_device->CreateTexture2D(&texd, nullptr, m_texture.GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device->CreateTexture2D() DepthTargetCube::Create()");
				return false;
			}
		}

		// DSV
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvd = {};
			dsvd.Format = desc.format;

			dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			//dsvd.Texture2DArray.ArraySize = 1;
			dsvd.Texture2DArray.ArraySize = 6;
			dsvd.Texture2DArray.MipSlice = 0;
			dsvd.Texture2DArray.FirstArraySlice = 0;

			// Create DSV
			//for (int i = 0; i < 6; ++i)
			{
				//dsvd.Texture2DArray.FirstArraySlice = i;

				//hr = p_device->CreateDepthStencilView(m_texture.Get(), &dsvd, m_DSVs[i].GetAddressOf());
				hr = p_device->CreateDepthStencilView(m_texture.Get(), &dsvd, m_DSV.GetAddressOf());
				if (FAILED(hr))
				{
					m_texture.Reset();

					assert(0 && "failed ID3D11Device->CreateDepthStencilView() DepthTargetCube::Create()");
					return false;
				}
			}
		}

		// SRV
		{
			// Configurate SRV
			D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
			srvd.Format = res_format;

			srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			srvd.TextureCube.MipLevels = desc.mip_levels;
			srvd.TextureCube.MostDetailedMip = 0;

			// Create SRV
			hr = p_device->CreateShaderResourceView(m_texture.Get(), &srvd, m_SRV.GetAddressOf());
			if (FAILED(hr))
			{
				m_texture.Reset();
				//for (auto&& v : m_DSVs)
				//{
				//	v.Reset();
				//}
				m_DSV.Reset();
				assert(0 && "failed ID3D11Device->CreateShaderResourceView() DepthTargetCube::Create()");
				return false;
			}
		}

		m_desc = desc;

		return true;
	}

	bool DepthTargetCube::Resize(ID3D11Device* p_device, UINT size)
	{
		m_SRV.Reset();
		//for (auto&& v : m_DSVs)
		//{
		//	v.Reset();
		//}
		m_DSV.Reset();
		m_texture.Reset();

		m_desc.cube_size = size;

		return Create(p_device, m_desc);
	}

	void DepthTargetCube::Release()
	{
		m_SRV.Reset();
		//for (auto&& v : m_DSVs)
		//{
		//	v.Reset();
		//}
		m_DSV.Reset();
		m_texture.Reset();
		//m_desc.Reset();
	}

	void DepthTargetCube::SetRTV(ID3D11DeviceContext* p_context)
	{
		p_context->OMSetRenderTargets(0, nullptr, m_DSV.Get());
	}

	void DepthTargetCube::RemoveRTV(ID3D11DeviceContext* p_context)
	{
		p_context->OMSetRenderTargets(0, nullptr, nullptr);
	}

	void DepthTargetCube::SetSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility)
	{
		switch (visibility)
		{
		case ShaderVisibility::VS:
			p_context->VSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::PS:
			p_context->PSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::GS:
			p_context->GSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::DS:
			p_context->DSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::HS:
			p_context->HSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::ALL:
			p_context->VSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			p_context->PSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			p_context->GSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			p_context->DSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			p_context->HSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::CS:
			p_context->CSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;
		}
	}

	void DepthTargetCube::Clear(ID3D11DeviceContext* p_context)
	{
		p_context->ClearDepthStencilView(m_DSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	ID3D11Texture2D* const DepthTargetCube::GetResource() const
	{
		return m_texture.Get();
	}

	ID3D11ShaderResourceView* const DepthTargetCube::GetSRV() const
	{
		return m_SRV.Get();
	}
	
	ID3D11DepthStencilView* const* DepthTargetCube::GetDSV() const
	{
		return m_DSV.GetAddressOf();
	}

	CubeDesc DepthTargetCube::GetDesc() const
	{
		return m_desc;
	}
#pragma endregion


}	// namespace TKGEngine