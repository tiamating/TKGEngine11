
#include "../../inc/StateManager.h"

#include <d3d11.h>

#include <cassert>

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Static member definition
	////////////////////////////////////////////////////////
	StateManager::DepthManager StateManager::m_depth_manager;
	StateManager::RasterizerManager StateManager::m_rasterizer_manager;
	StateManager::BlendManager StateManager::m_blend_manager;
	StateManager::SamplerManager StateManager::m_sampler_manager;


	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	bool StateManager::InitManager(ID3D11Device* p_device, ID3D11DeviceContext* p_context)
	{
		if (!m_depth_manager.CreateState(p_device))
			return false;
		if (!m_rasterizer_manager.CreateState(p_device))
			return false;
		if (!m_blend_manager.CreateState(p_device))
			return false;
		if (!m_sampler_manager.CreateState(p_device))
			return false;

		return true;
	}

	void StateManager::SetState(ID3D11DeviceContext* p_context, const DS type, const Stencil::Mask mask, const bool write_depth)
	{
		m_depth_manager.SetState(p_context, type, mask, write_depth);
	}

	void StateManager::SetState(ID3D11DeviceContext* p_context, RS type)
	{
		m_rasterizer_manager.SetState(p_context, type);
	}

	void StateManager::SetState(ID3D11DeviceContext* p_context, BS type)
	{
		m_blend_manager.SetState(p_context, type);
	}

	void StateManager::SetAllSampler(ID3D11DeviceContext* p_context)
	{
		for (int i = SAMPLSLOT_ONDEMAND0; i <= SAMPLSLOT_CMP_DEPTH; ++i)
		{
			m_sampler_manager.SetState(p_context, i);
		}
	}

#pragma region Depth
	bool StateManager::DepthManager::CreateState(ID3D11Device* p_device)
	{
		assert(p_device != nullptr);

		// BackGround
		{
			// Write Depth
			D3D11_DEPTH_STENCIL_DESC dsd = {};
			dsd.DepthEnable = true;
			dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			dsd.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
			dsd.StencilEnable = false;
			dsd.StencilReadMask = 0;
			dsd.StencilWriteMask = 0xFF;
			dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			auto hr = p_device->CreateDepthStencilState(
				&dsd, depth_types[static_cast<int>(DS::BackGround)][static_cast<int>(DSType::WriteDepth)].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateDepthStencilState() (BackGround WriteDepth)");
				return false;
			}

			// No Write
			hr = p_device->CreateDepthStencilState(
				&dsd, depth_types[static_cast<int>(DS::BackGround)][static_cast<int>(DSType::NoWriteDepth)].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateDepthStencilState() (BackGround NoWriteDepth)");
				return false;
			}
		}
		// Geometry
		{
			// Write Depth
			D3D11_DEPTH_STENCIL_DESC dsd = {};
			dsd.DepthEnable = true;
			dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsd.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
			dsd.StencilEnable = true;
			dsd.StencilReadMask = 0xff;
			dsd.StencilWriteMask = 0xff;
			dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			auto hr = p_device->CreateDepthStencilState(
				&dsd, depth_types[static_cast<int>(DS::Geometry)][static_cast<int>(DSType::WriteDepth)].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateDepthStencilState() (Geometry WriteDepth)");
				return false;
			}

			// No Write
			dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			hr = p_device->CreateDepthStencilState(
				&dsd, depth_types[static_cast<int>(DS::Geometry)][static_cast<int>(DSType::NoWriteDepth)].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateDepthStencilState() (Geometry NoWriteDepth)");
				return false;
			}
		}
		// NoTest
		{
			// Write Depth
			D3D11_DEPTH_STENCIL_DESC dsd = {};
			dsd.DepthEnable = false;
			dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsd.DepthFunc = D3D11_COMPARISON_ALWAYS;
			dsd.StencilEnable = false;
			dsd.StencilReadMask = 0;
			dsd.StencilWriteMask = 0;
			dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			auto hr = p_device->CreateDepthStencilState(
				&dsd, depth_types[static_cast<int>(DS::NoTest)][static_cast<int>(DSType::WriteDepth)].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateDepthStencilState() (NoTest WriteDepth)");
				return false;
			}

			// No Write
			hr = p_device->CreateDepthStencilState(
				&dsd, depth_types[static_cast<int>(DS::NoTest)][static_cast<int>(DSType::NoWriteDepth)].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateDepthStencilState() (NoTest NoWriteDepth)");
				return false;
			}
		}
		// WriteOnly
		{
			D3D11_DEPTH_STENCIL_DESC dsd = {};
			dsd.DepthEnable = true;
			dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsd.DepthFunc = D3D11_COMPARISON_ALWAYS;
			dsd.StencilEnable = false;
			dsd.StencilReadMask = 0;
			dsd.StencilWriteMask = 0;
			dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			auto hr = p_device->CreateDepthStencilState(
				&dsd, depth_types[static_cast<int>(DS::WriteOnly)][static_cast<int>(DSType::WriteDepth)].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateDepthStencilState() (WriteOnly WriteDepth)");
				return false;
			}

			// No Write
			dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			hr = p_device->CreateDepthStencilState(
				&dsd, depth_types[static_cast<int>(DS::WriteOnly)][static_cast<int>(DSType::NoWriteDepth)].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateDepthStencilState() (WriteOnly NoWriteDepth)");
				return false;
			}
		}

		return true;
	}

	void StateManager::DepthManager::SetState(ID3D11DeviceContext* p_context, DS type, Stencil::Mask mask, bool write_depth)
	{
		assert(p_context != nullptr);
		assert(static_cast<int>(type) >= 0 && static_cast<int>(type) < static_cast<int>(DS::MAX_DEPTHSTENCIL_STATE));
		const DSType ds_type = write_depth ? DSType::WriteDepth : DSType::NoWriteDepth;
		p_context->OMSetDepthStencilState(depth_types[static_cast<int>(type)][static_cast<int>(ds_type)].Get(), static_cast<int>(mask));
	}
#pragma endregion


#pragma region Rasterizer
	bool StateManager::RasterizerManager::CreateState(ID3D11Device* p_device)
	{
		assert(p_device != nullptr);

		// FILL_FRONT
		{
			D3D11_RASTERIZER_DESC rd = {};
			rd.FillMode = D3D11_FILL_SOLID;
			rd.CullMode = D3D11_CULL_FRONT;
			rd.FrontCounterClockwise = true;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = true;
			rd.ScissorEnable = false;
			rd.MultisampleEnable = false;
			rd.AntialiasedLineEnable = false;
			auto hr = p_device->CreateRasterizerState(
				&rd, rasterizer_types[(static_cast<int>(RS::FillFront))].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateRasterizerState() (FillFront)");
				return false;
			}
		}
		// FILL_BACK
		{
			D3D11_RASTERIZER_DESC rd = {};
			rd.FillMode = D3D11_FILL_SOLID;
			rd.CullMode = D3D11_CULL_BACK;
			rd.FrontCounterClockwise = true;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = true;
			rd.ScissorEnable = false;
			rd.MultisampleEnable = false;
			rd.AntialiasedLineEnable = false;
			auto hr = p_device->CreateRasterizerState(
				&rd, rasterizer_types[(static_cast<int>(RS::FillBack))].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateRasterizerState() (FillBack)");
				return false;
			}
		}
		// FillNone
		{
			D3D11_RASTERIZER_DESC rd = {};
			rd.FillMode = D3D11_FILL_SOLID;
			rd.CullMode = D3D11_CULL_NONE;
			rd.FrontCounterClockwise = true;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = true;
			rd.ScissorEnable = false;
			rd.MultisampleEnable = false;
			rd.AntialiasedLineEnable = false;
			auto hr = p_device->CreateRasterizerState(
				&rd, rasterizer_types[(static_cast<int>(RS::FillNone))].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateRasterizerState() (FillNone)");
				return false;
			}
		}
		// WireFront
		{
			D3D11_RASTERIZER_DESC rd = {};
			rd.FillMode = D3D11_FILL_WIREFRAME;
			rd.CullMode = D3D11_CULL_FRONT;
			rd.FrontCounterClockwise = true;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = true;
			rd.ScissorEnable = false;
			rd.MultisampleEnable = false;
			rd.AntialiasedLineEnable = false;
			auto hr = p_device->CreateRasterizerState(
				&rd, rasterizer_types[(static_cast<int>(RS::WireFront))].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateRasterizerState() (WireFront)");
				return false;
			}
		}
		// WireBack
		{
			D3D11_RASTERIZER_DESC rd = {};
			rd.FillMode = D3D11_FILL_WIREFRAME;
			rd.CullMode = D3D11_CULL_BACK;
			rd.FrontCounterClockwise = true;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = true;
			rd.ScissorEnable = false;
			rd.MultisampleEnable = false;
			rd.AntialiasedLineEnable = false;
			auto hr = p_device->CreateRasterizerState(
				&rd, rasterizer_types[(static_cast<int>(RS::WireBack))].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateRasterizerState() (WireBack)");
				return false;
			}
		}
		// WireNone
		{
			D3D11_RASTERIZER_DESC rd = {};
			rd.FillMode = D3D11_FILL_WIREFRAME;
			rd.CullMode = D3D11_CULL_NONE;
			rd.FrontCounterClockwise = true;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = true;
			rd.ScissorEnable = false;
			rd.MultisampleEnable = false;
			rd.AntialiasedLineEnable = false;
			auto hr = p_device->CreateRasterizerState(
				&rd, rasterizer_types[(static_cast<int>(RS::WireNone))].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateRasterizerState() (WireNone)");
				return false;
			}
		}
		// WireFrontSmooth
		{
			D3D11_RASTERIZER_DESC rd = {};
			rd.FillMode = D3D11_FILL_WIREFRAME;
			rd.CullMode = D3D11_CULL_FRONT;
			rd.FrontCounterClockwise = true;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = true;
			rd.ScissorEnable = false;
			rd.MultisampleEnable = false;
			rd.AntialiasedLineEnable = true;
			auto hr = p_device->CreateRasterizerState(
				&rd, rasterizer_types[(static_cast<int>(RS::WireFrontSmooth))].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateRasterizerState() (WireFrontSmooth)");
				return false;
			}
		}
		// WireBackSmooth
		{
			D3D11_RASTERIZER_DESC rd = {};
			rd.FillMode = D3D11_FILL_WIREFRAME;
			rd.CullMode = D3D11_CULL_BACK;
			rd.FrontCounterClockwise = true;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = true;
			rd.ScissorEnable = false;
			rd.MultisampleEnable = false;
			rd.AntialiasedLineEnable = true;
			auto hr = p_device->CreateRasterizerState(
				&rd, rasterizer_types[(static_cast<int>(RS::WireBackSmooth))].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateRasterizerState() (WireBackSmooth)");
				return false;
			}
		}
		// WireNoneSmooth
		{
			D3D11_RASTERIZER_DESC rd = {};
			rd.FillMode = D3D11_FILL_WIREFRAME;
			rd.CullMode = D3D11_CULL_NONE;
			rd.FrontCounterClockwise = true;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = true;
			rd.ScissorEnable = false;
			rd.MultisampleEnable = false;
			rd.AntialiasedLineEnable = true;
			auto hr = p_device->CreateRasterizerState(
				&rd, rasterizer_types[(static_cast<int>(RS::WireNoneSmooth))].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateRasterizerState() (WireNoneSmooth)");
				return false;
			}
		}
		// FillFrontMS
		{
			D3D11_RASTERIZER_DESC rd = {};
			rd.FillMode = D3D11_FILL_SOLID;
			rd.CullMode = D3D11_CULL_FRONT;
			rd.FrontCounterClockwise = true;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = true;
			rd.ScissorEnable = false;
			rd.MultisampleEnable = true;
			rd.AntialiasedLineEnable = false;
			auto hr = p_device->CreateRasterizerState(
				&rd, rasterizer_types[(static_cast<int>(RS::FillFrontMS))].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateRasterizerState() (FillFrontMS)");
				return false;
			}
		}
		// FillBackMS
		{
			D3D11_RASTERIZER_DESC rd = {};
			rd.FillMode = D3D11_FILL_SOLID;
			rd.CullMode = D3D11_CULL_BACK;
			rd.FrontCounterClockwise = true;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = true;
			rd.ScissorEnable = false;
			rd.MultisampleEnable = true;
			rd.AntialiasedLineEnable = false;
			auto hr = p_device->CreateRasterizerState(
				&rd, rasterizer_types[(static_cast<int>(RS::FillBackMS))].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateRasterizerState() (FillBackMS)");
				return false;
			}
		}
		// FillNoneMS
		{
			D3D11_RASTERIZER_DESC rd = {};
			rd.FillMode = D3D11_FILL_SOLID;
			rd.CullMode = D3D11_CULL_NONE;
			rd.FrontCounterClockwise = true;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = true;
			rd.ScissorEnable = false;
			rd.MultisampleEnable = true;
			rd.AntialiasedLineEnable = false;
			auto hr = p_device->CreateRasterizerState(
				&rd, rasterizer_types[(static_cast<int>(RS::FillNoneMS))].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateRasterizerState() (FillNoneMS)");
				return false;
			}
		}
		// FillFrontNoClip
		{
			D3D11_RASTERIZER_DESC rd = {};
			rd.FillMode = D3D11_FILL_SOLID;
			rd.CullMode = D3D11_CULL_FRONT;
			rd.FrontCounterClockwise = true;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = false;
			rd.ScissorEnable = false;
			rd.MultisampleEnable = false;
			rd.AntialiasedLineEnable = false;
			auto hr = p_device->CreateRasterizerState(
				&rd, rasterizer_types[(static_cast<int>(RS::FillFrontNoClip))].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateRasterizerState() (FillFrontNoClip)");
				return false;
			}
		}
		// FillBackNoClip
		{
			D3D11_RASTERIZER_DESC rd = {};
			rd.FillMode = D3D11_FILL_SOLID;
			rd.CullMode = D3D11_CULL_BACK;
			rd.FrontCounterClockwise = true;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = false;
			rd.ScissorEnable = false;
			rd.MultisampleEnable = false;
			rd.AntialiasedLineEnable = false;
			auto hr = p_device->CreateRasterizerState(
				&rd, rasterizer_types[(static_cast<int>(RS::FillBackNoClip))].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateRasterizerState() (FillBackNoClip)");
				return false;
			}
		}
		// FillNoneNoClip
		{
			D3D11_RASTERIZER_DESC rd = {};
			rd.FillMode = D3D11_FILL_SOLID;
			rd.CullMode = D3D11_CULL_NONE;
			rd.FrontCounterClockwise = true;
			rd.DepthBias = 0;
			rd.DepthBiasClamp = 0;
			rd.SlopeScaledDepthBias = 0;
			rd.DepthClipEnable = false;
			rd.ScissorEnable = false;
			rd.MultisampleEnable = false;
			rd.AntialiasedLineEnable = false;
			auto hr = p_device->CreateRasterizerState(
				&rd, rasterizer_types[(static_cast<int>(RS::FillNoneNoClip))].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateRasterizerState() (FillNoneNoClip)");
				return false;
			}
		}

		return true;
	}

	void StateManager::RasterizerManager::SetState(ID3D11DeviceContext* p_context, RS type)
	{
		assert(p_context != nullptr);
		assert(static_cast<int>(type) >= 0 && static_cast<int>(type) < static_cast<int>(RS::MAX_RATERIZER_STATE));

		p_context->RSSetState(rasterizer_types[static_cast<int>(type)].Get());
	}
#pragma endregion


#pragma region Blend
	bool StateManager::BlendManager::CreateState(ID3D11Device* p_device)
	{
		assert(p_device != nullptr);

		// Opaque
		{
			D3D11_BLEND_DESC bd = {};
			bd.RenderTarget[0].BlendEnable = false;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			auto hr = p_device->CreateBlendState(
				&bd, blend_types[static_cast<int>(BS::Opaque)].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateBlendState() (Opaque)");
				return false;
			}
		}
		// Transparent
		{
			D3D11_BLEND_DESC bd = {};
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;//D3D11_BLEND_ZERO;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			auto hr = p_device->CreateBlendState(
				&bd, blend_types[static_cast<int>(BS::Transparent)].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateBlendState() (Transparent)");
				return false;
			}
		}
		// Addition
		{
			D3D11_BLEND_DESC bd = {};
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			auto hr = p_device->CreateBlendState(
				&bd, blend_types[static_cast<int>(BS::Addition)].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateBlendState() (Addition)");
				return false;
			}
		}
		// Multiply
		{
			D3D11_BLEND_DESC bd = {};
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			auto hr = p_device->CreateBlendState(
				&bd, blend_types[static_cast<int>(BS::Multiply)].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateBlendState() (Multiply)");
				return false;
			}
		}
		// Subtract
		{
			D3D11_BLEND_DESC bd = {};
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			auto hr = p_device->CreateBlendState(
				&bd, blend_types[static_cast<int>(BS::Subtract)].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateBlendState() (Subtract)");
				return false;
			}
		}
		// Lighten
		{
			D3D11_BLEND_DESC bd = {};
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;
			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			auto hr = p_device->CreateBlendState(
				&bd, blend_types[static_cast<int>(BS::Lighten)].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateBlendState() (Lighten)");
				return false;
			}
		}
		// Darken
		{
			D3D11_BLEND_DESC bd = {};
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;
			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MIN;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			auto hr = p_device->CreateBlendState(
				&bd, blend_types[static_cast<int>(BS::Darken)].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateBlendState() (Darken)");
				return false;
			}
		}
		// Inverse
		{
			D3D11_BLEND_DESC bd = {};
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_INV_SRC_COLOR;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_DEST_COLOR;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			auto hr = p_device->CreateBlendState(
				&bd, blend_types[static_cast<int>(BS::Inverse)].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateBlendState() (Inverse)");
				return false;
			}
		}
		// Decal
		{
			D3D11_BLEND_DESC bd = {};
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask =
				D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE;
			bd.IndependentBlendEnable = true;
			bd.RenderTarget[1] = bd.RenderTarget[0];
			bd.RenderTarget[1].RenderTargetWriteMask =
				D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN;
			bd.AlphaToCoverageEnable = false;
			auto hr = p_device->CreateBlendState(
				&bd, blend_types[static_cast<int>(BS::Decal)].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateBlendState() (Decal)");
				return false;
			}
		}
		// Disable
		{
			D3D11_BLEND_DESC bd = {};
			bd.RenderTarget[0].BlendEnable = false;
			bd.RenderTarget[0].RenderTargetWriteMask = 0;
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			auto hr = p_device->CreateBlendState(
				&bd, blend_types[static_cast<int>(BS::Disable)].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateBlendState() (Disable)");
				return false;
			}
		}
		// AlphaToCoverage
		{
			D3D11_BLEND_DESC bd = {};
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = true;
			auto hr = p_device->CreateBlendState(
				&bd, blend_types[static_cast<int>(BS::AlphaToCoverage)].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateBlendState() (AlphaToCoverage)");
				return false;
			}
		}
		// DeferredLight
		{
			D3D11_BLEND_DESC bd = {};
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask =
				D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE;
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			auto hr = p_device->CreateBlendState(
				&bd, blend_types[static_cast<int>(BS::DeferredLight)].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateBlendState() (DeferredLight)");
				return false;
			}
		}
		// EnvironmentalLight
		{
			D3D11_BLEND_DESC bd = {};
			bd.RenderTarget[0].BlendEnable = true;
			bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
			bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd.RenderTarget[0].RenderTargetWriteMask =
				D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE;
			bd.IndependentBlendEnable = false;
			bd.AlphaToCoverageEnable = false;
			auto hr = p_device->CreateBlendState(
				&bd, blend_types[static_cast<int>(BS::EnvironmentalLight)].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateBlendState() (EnvironmentalLight)");
				return false;
			}
		}

		return true;
	}

	void StateManager::BlendManager::SetState(ID3D11DeviceContext* p_context, BS type)
	{
		assert(p_context != nullptr);
		assert(static_cast<int>(type) >= 0 && static_cast<int>(type) < static_cast<int>(BS::MAX_BLEND_STATE));

		p_context->OMSetBlendState(blend_types[static_cast<int>(type)].Get(), nullptr, 0xffffffff);
	}
#pragma endregion


#pragma region Sampler
	bool StateManager::SamplerManager::CreateState(ID3D11Device* p_device)
	{
		assert(p_device != nullptr);

		// LINEAR_MIRROR
		{
			D3D11_SAMPLER_DESC sd = {};
			sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
			sd.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
			sd.MipLODBias = 0;
			sd.MaxAnisotropy = 0;
			sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
			FLOAT borderClr[4] = { 0.0f,0.0f,0.0f,0.0f };
			for (int i = 0; i < 4; ++i)
			{
				sd.BorderColor[i] = borderClr[i];
			}
			sd.MinLOD = 0;
			sd.MaxLOD = D3D11_FLOAT32_MAX;
			auto hr = p_device->CreateSamplerState(
				&sd, sampler_types[SAMPLSLOT_LINEAR_MIRROR].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateSamplerState() (SAMPLSLOT_LINEAR_MIRROR)");
				return false;
			}
		}
		// LINEAR_CLAMP
		{
			D3D11_SAMPLER_DESC sd = {};
			sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			sd.MipLODBias = 0;
			sd.MaxAnisotropy = 0;
			sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
			FLOAT borderClr[4] = { 0.0f,0.0f,0.0f,0.0f };
			for (int i = 0; i < 4; ++i)
			{
				sd.BorderColor[i] = borderClr[i];
			}
			sd.MinLOD = 0;
			sd.MaxLOD = D3D11_FLOAT32_MAX;
			auto hr = p_device->CreateSamplerState(
				&sd, sampler_types[SAMPLSLOT_LINEAR_CLAMP].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateSamplerState() (SAMPLSLOT_LINEAR_CLAMP)");
				return false;
			}
		}
		// LINEAR_WRAP
		{
			D3D11_SAMPLER_DESC sd = {};
			sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			sd.MipLODBias = 0;
			sd.MaxAnisotropy = 0;
			sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
			FLOAT borderClr[4] = { 0.0f,0.0f,0.0f,0.0f };
			for (int i = 0; i < 4; ++i)
			{
				sd.BorderColor[i] = borderClr[i];
			}
			sd.MinLOD = 0;
			sd.MaxLOD = D3D11_FLOAT32_MAX;
			auto hr = p_device->CreateSamplerState(
				&sd, sampler_types[SAMPLSLOT_LINEAR_WRAP].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateSamplerState() (SAMPLSLOT_LINEAR_WRAP)");
				return false;
			}
		}
		// POINT_MIRROR
		{
			D3D11_SAMPLER_DESC sd = {};
			sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
			sd.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
			sd.MipLODBias = 0;
			sd.MaxAnisotropy = 0;
			sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
			FLOAT borderClr[4] = { 0.0f,0.0f,0.0f,0.0f };
			for (int i = 0; i < 4; ++i)
			{
				sd.BorderColor[i] = borderClr[i];
			}
			sd.MinLOD = 0;
			sd.MaxLOD = D3D11_FLOAT32_MAX;
			auto hr = p_device->CreateSamplerState(
				&sd, sampler_types[SAMPLSLOT_POINT_MIRROR].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateSamplerState() (SAMPLSLOT_POINT_MIRROR)");
				return false;
			}
		}
		// POINT_CLAMP
		{
			D3D11_SAMPLER_DESC sd = {};
			sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			sd.MipLODBias = 0;
			sd.MaxAnisotropy = 0;
			sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
			FLOAT borderClr[4] = { 0.0f,0.0f,0.0f,0.0f };
			for (int i = 0; i < 4; ++i)
			{
				sd.BorderColor[i] = borderClr[i];
			}
			sd.MinLOD = 0;
			sd.MaxLOD = D3D11_FLOAT32_MAX;
			auto hr = p_device->CreateSamplerState(
				&sd, sampler_types[SAMPLSLOT_POINT_CLAMP].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateSamplerState() (SAMPLSLOT_POINT_CLAMP)");
				return false;
			}
		}
		// POINT_WRAP
		{
			D3D11_SAMPLER_DESC sd = {};
			sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			sd.MipLODBias = 0;
			sd.MaxAnisotropy = 0;
			sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
			FLOAT borderClr[4] = { 0.0f,0.0f,0.0f,0.0f };
			for (int i = 0; i < 4; ++i)
			{
				sd.BorderColor[i] = borderClr[i];
			}
			sd.MinLOD = 0;
			sd.MaxLOD = D3D11_FLOAT32_MAX;
			auto hr = p_device->CreateSamplerState(
				&sd, sampler_types[SAMPLSLOT_POINT_WRAP].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateSamplerState() (SAMPLSLOT_POINT_WRAP)");
				return false;
			}
		}
		// ANISO_MIRROR
		{
			D3D11_SAMPLER_DESC sd = {};
			sd.Filter = D3D11_FILTER_ANISOTROPIC;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
			sd.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
			sd.MipLODBias = 0;
			sd.MaxAnisotropy = 16;
			sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
			FLOAT borderClr[4] = { 0.0f,0.0f,0.0f,0.0f };
			for (int i = 0; i < 4; ++i)
			{
				sd.BorderColor[i] = borderClr[i];
			}
			sd.MinLOD = 0;
			sd.MaxLOD = D3D11_FLOAT32_MAX;
			auto hr = p_device->CreateSamplerState(
				&sd, sampler_types[SAMPLSLOT_ANISO_MIRROR].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateSamplerState() (SAMPLSLOT_ANISO_MIRROR)");
				return false;
			}
		}
		// ANISO_CLAMP
		{
			D3D11_SAMPLER_DESC sd = {};
			sd.Filter = D3D11_FILTER_ANISOTROPIC;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			sd.MipLODBias = 0;
			sd.MaxAnisotropy = 16;
			sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
			FLOAT borderClr[4] = { 0.0f,0.0f,0.0f,0.0f };
			for (int i = 0; i < 4; ++i)
			{
				sd.BorderColor[i] = borderClr[i];
			}
			sd.MinLOD = 0;
			sd.MaxLOD = D3D11_FLOAT32_MAX;
			auto hr = p_device->CreateSamplerState(
				&sd, sampler_types[SAMPLSLOT_ANISO_CLAMP].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateSamplerState() (SAMPLSLOT_ANISO_CLAMP)");
				return false;
			}
		}
		// ANISO_WRAP
		{
			D3D11_SAMPLER_DESC sd = {};
			sd.Filter = D3D11_FILTER_ANISOTROPIC;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			sd.MipLODBias = 0;
			sd.MaxAnisotropy = 16;
			sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
			FLOAT borderClr[4] = { 0.0f,0.0f,0.0f,0.0f };
			for (int i = 0; i < 4; ++i)
			{
				sd.BorderColor[i] = borderClr[i];
			}
			sd.MinLOD = 0;
			sd.MaxLOD = D3D11_FLOAT32_MAX;
			auto hr = p_device->CreateSamplerState(
				&sd, sampler_types[SAMPLSLOT_ANISO_WRAP].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateSamplerState() (SAMPLSLOT_ANISO_WRAP)");
				return false;
			}
		}
		// CMP_DEPTH
		{
			D3D11_SAMPLER_DESC sd = {};
			sd.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
			sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
			sd.MipLODBias = 0;
			sd.MaxAnisotropy = 0;
			sd.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
			FLOAT borderClr[4] = { 0.0f,0.0f,0.0f,0.0f };
			for (int i = 0; i < 4; ++i)
			{
				sd.BorderColor[i] = borderClr[i];
			}
			sd.MinLOD = 0;
			sd.MaxLOD = D3D11_FLOAT32_MAX;
			auto hr = p_device->CreateSamplerState(
				&sd, sampler_types[SAMPLSLOT_CMP_DEPTH].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateSamplerState() (SAMPLSLOT_CMP_DEPTH)");
				return false;
			}
		}

		return true;
	}

	void StateManager::SamplerManager::SetState(ID3D11DeviceContext* p_context, int slot)
	{
		assert(p_context != nullptr);

		p_context->VSSetSamplers(slot, 1, sampler_types[slot].GetAddressOf());
		p_context->PSSetSamplers(slot, 1, sampler_types[slot].GetAddressOf());
		p_context->GSSetSamplers(slot, 1, sampler_types[slot].GetAddressOf());
	}
#pragma endregion

}// namespace TKGEngine