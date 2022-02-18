
#include "../inc/StructuredBuffer.h"

#include "Systems/inc/IGraphics.h"

#include <cassert>

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	bool StructuredBuffer::Create(int stride_size, int row_count, bool create_uav, const void* p_data)
	{
		HRESULT hr = S_OK;

		assert(stride_size > 0);
		assert(row_count > 0);

		m_stride_size = stride_size;
		m_row_count = row_count;

		m_has_uav = create_uav;

		// If already create buffer
		if (m_SB)
		{
			Release();
		}
		// https://tositeru.github.io/ImasaraDX11/part/type-of-buffer
		// Create Buffer
		{
			D3D11_BUFFER_DESC desc = {};
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = 0;
			desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
			if (create_uav)
				desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
			desc.ByteWidth = static_cast<UINT>(stride_size * row_count);
			desc.StructureByteStride = stride_size;
			desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			desc.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA init_data = {};
			init_data.pSysMem = p_data;
			init_data.SysMemPitch = 0;
			init_data.SysMemSlicePitch = 0;

			hr = IGraphics::Get().Device()->CreateBuffer(
				&desc,
				(p_data == nullptr) ? nullptr : &init_data,
				m_SB.GetAddressOf());

			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateBuffer() StructuredBuffer::Create()");
				return false;
			}
		}

		// Create SRV
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
			srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
			// D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS‚É‚Í‘Î‰ž‚µ‚Ä‚¢‚È‚¢
			srvd.Format = DXGI_FORMAT_UNKNOWN;
			srvd.BufferEx.Flags = 0;
			srvd.BufferEx.FirstElement = 0;
			srvd.BufferEx.NumElements = row_count;

			hr = IGraphics::Get().Device()->CreateShaderResourceView(m_SB.Get(), &srvd, m_SRV.GetAddressOf());
			if (FAILED(hr))
			{
				m_SB.Reset();

				assert(0 && "failed ID3D11Device->CreateShaderResourceView() StructuredBuffer::Create()");
				return false;
			}
		}
		// Create UAV
		if (create_uav)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavd = {};
			uavd.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			// D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS‚É‚Í‘Î‰ž‚µ‚Ä‚¢‚È‚¢
			uavd.Format = DXGI_FORMAT_UNKNOWN;
			uavd.Buffer.FirstElement = 0;
			uavd.Buffer.NumElements = row_count;

			hr = IGraphics::Get().Device()->CreateUnorderedAccessView(m_SB.Get(), &uavd, m_UAV.GetAddressOf());
			if (FAILED(hr))
			{
				m_SRV.Reset();
				m_SB.Reset();

				assert(0 && "failed ID3D11Device->CreateUnorderedAccessView() StructuredBuffer::Create()");
				return false;
			}
		}

		return true;
	}

	void StructuredBuffer::Release()
	{
		m_SB.Reset();
		m_SRV.Reset();
		m_UAV.Reset();
		m_has_uav = false;
		m_stride_size = 0;
		m_row_count = 0;
	}

	void StructuredBuffer::SetSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility)
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

	void StructuredBuffer::SetUAV(ID3D11DeviceContext* p_context, int slot)
	{
		if (!m_has_uav)
			return;

		p_context->CSSetUnorderedAccessViews(slot, 1, m_UAV.GetAddressOf(), nullptr);
	}

	ID3D11Buffer* StructuredBuffer::GetBuffer() const
	{
		return m_SB.Get();
	}

	ID3D11UnorderedAccessView* StructuredBuffer::GetUAV() const
	{
		return m_UAV.Get();
	}

	ID3D11UnorderedAccessView* const* StructuredBuffer::GetAddressOfUAV() const
	{
		return m_UAV.GetAddressOf();
	}

	ID3D11ShaderResourceView* StructuredBuffer::GetSRV() const
	{
		return m_SRV.Get();
	}

	ID3D11ShaderResourceView* const* StructuredBuffer::GetAddressOfSRV() const
	{
		return m_SRV.GetAddressOf();
	}

}	// namespace TKGEngine