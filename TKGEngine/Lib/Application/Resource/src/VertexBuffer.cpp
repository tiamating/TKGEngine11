
#include "../inc/VertexBuffer.h"

#include "Systems/inc/IGraphics.h"

#include <cassert>

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	bool VertexBuffer::Create(
		void* p_data,
		int stride_size,
		int row_count,
		VERTEX_ELEMENT_TYPE v_type,
		BUFFER_HEAP_TYPE heap_type,
		bool create_srv,
		bool create_uav
	)
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = stride_size * row_count;
		desc.Usage =
			(heap_type == BUFFER_HEAP_TYPE::BUFFER_HEAP_IMMUTABLE) ? D3D11_USAGE_IMMUTABLE :
			(heap_type == BUFFER_HEAP_TYPE::BUFFER_HEAP_DEFAULT) ? D3D11_USAGE_DEFAULT :
			D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags =
			(heap_type == BUFFER_HEAP_TYPE::BUFFER_HEAP_DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		// DirectX11.0‚Å‚ÍStructuredBuffer‚ÆVertexBuffer‚Í“¯Žž‚Éo—ˆ‚È‚¢‚½‚ßByteAddressBuffer‚Æ‚µ‚Äì¬
		// https://tositeru.github.io/ImasaraDX11/part/type-of-buffer
		if (create_srv)
		{
			desc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
			desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
		}
		if (create_uav)
		{
			desc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
			desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
		}

		// VertexBuffer
		if (!p_data)
		{
			auto hr = IGraphics::Get().Device()->CreateBuffer(
				&desc, nullptr, m_buffer.GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateBuffer() VertexBuffer::Create()");
				return false;
			}
		}
		else
		{
			D3D11_SUBRESOURCE_DATA init_data = {};
			init_data.pSysMem = p_data;
			init_data.SysMemPitch = 0;
			init_data.SysMemSlicePitch = 0;

			auto hr = IGraphics::Get().Device()->CreateBuffer(
				&desc, &init_data, m_buffer.GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateBuffer() VertexBuffer::Create()");
				return false;
			}
		}
		// http://www.project-asura.com/program/d3d11/d3d11_007.html
		// SRV
		if (create_srv)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
			srvd.Format = DXGI_FORMAT_R32_TYPELESS;
			srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
			srvd.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
			srvd.BufferEx.NumElements = desc.ByteWidth / 4/*sizeof(float)*/;

			auto hr = IGraphics::Get().Device()->CreateShaderResourceView(
				m_buffer.Get(), &srvd, m_SRV.GetAddressOf()
			);
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateShaderResourceView() VertexBuffer::Create()");
				return false;
			}
		}
		// UAV
		if (create_uav)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavd = {};
			uavd.Format = DXGI_FORMAT_R32_TYPELESS;
			uavd.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			uavd.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
			uavd.Buffer.NumElements = desc.ByteWidth / 4/*sizeof(float)*/;

			auto hr = IGraphics::Get().Device()->CreateUnorderedAccessView(
				m_buffer.Get(), &uavd, m_UAV.GetAddressOf()
			);
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateUnorderedAccessView() VertexBuffer::Create()");
				return false;
			}
		}

		m_element_type = v_type;
		m_heap_type = heap_type;
		m_has_created = true;
		return true;
	}

	bool VertexBuffer::Resize(int stride_size, int row_count)
	{
		// ‰Šú’l‚ð‚à‚ç‚í‚È‚¢‚Ì‚ÅDefault‚©Dynamic‚µ‚©o—ˆ‚È‚¢
		assert(m_heap_type != BUFFER_HEAP_TYPE::BUFFER_HEAP_IMMUTABLE);

		// Clear process
		bool created_srv = false;
		bool created_uav = false;
		{
			m_has_created = false;
			m_buffer.Reset();
			if (m_SRV)
			{
				created_srv = true;
				m_SRV.Reset();
			}
			if (m_UAV)
			{
				created_uav = true;
				m_UAV.Reset();
			}
		}

		// ReCreate process
		return Create(nullptr, stride_size, row_count, m_element_type, m_heap_type, created_srv, created_uav);
	}

	void VertexBuffer::Release()
	{
		m_has_created = false;
		m_buffer.Reset();
		m_SRV.Reset();
		m_UAV.Reset();
	}

	void VertexBuffer::UpdateSubresource(const void* p_data) const
	{
		if (m_heap_type != BUFFER_HEAP_TYPE::BUFFER_HEAP_DEFAULT)
		{
			return;
		}
		IGraphics::Get().DC(0, Graphics::DC_COPY_PATH::DC_CP_MAIN)->UpdateSubresource(
			m_buffer.Get(), 0, nullptr, p_data, 0, 0);
	}

	void VertexBuffer::UpdateSubresource(ID3D11DeviceContext* p_context, const void* p_data) const
	{
		if (m_heap_type != BUFFER_HEAP_TYPE::BUFFER_HEAP_DEFAULT)
		{
			return;
		}
		p_context->UpdateSubresource(m_buffer.Get(), 0, nullptr, p_data, 0, 0);
	}

	bool VertexBuffer::Map(void** pp_dst) const
	{
		if (m_heap_type != BUFFER_HEAP_TYPE::BUFFER_HEAP_DYNAMIC)
		{
			return false;
		}
		D3D11_MAPPED_SUBRESOURCE mapped_buffer = {};
		const auto hr = IGraphics::Get().DC(0, Graphics::DC_COPY_PATH::DC_CP_MAIN)->Map(
			m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_buffer);
		if (FAILED(hr))
		{
			assert(0 && "failed ID3D11DeviceContext::Map() VertexBuffer::Map()");
			*pp_dst = nullptr;
			return false;
		}

		*pp_dst = mapped_buffer.pData;
		return true;
	}
	bool VertexBuffer::Map(ID3D11DeviceContext* p_context, void** pp_dst) const
	{
		if (m_heap_type != BUFFER_HEAP_TYPE::BUFFER_HEAP_DYNAMIC)
		{
			return false;
		}
		D3D11_MAPPED_SUBRESOURCE mapped_buffer = {};
		const auto hr = p_context->Map(
			m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_buffer);
		if (FAILED(hr))
		{
			assert(0 && "failed ID3D11DeviceContext::Map() VertexBuffer::Map()");
			*pp_dst = nullptr;
			return false;
		}

		*pp_dst = mapped_buffer.pData;
		return true;
	}

	void VertexBuffer::Unmap() const
	{
		if (m_heap_type != BUFFER_HEAP_TYPE::BUFFER_HEAP_DYNAMIC)
		{
			return;
		}
		IGraphics::Get().DC(0, Graphics::DC_COPY_PATH::DC_CP_MAIN)->Unmap(m_buffer.Get(), 0);
	}
	void VertexBuffer::Unmap(ID3D11DeviceContext* p_context) const
	{
		if (m_heap_type != BUFFER_HEAP_TYPE::BUFFER_HEAP_DYNAMIC)
		{
			return;
		}
		p_context->Unmap(m_buffer.Get(), 0);
	}

	void VertexBuffer::Set(ID3D11DeviceContext* p_context, int slot)
	{
		const UINT stride = VERTEX_ELEMENT::ELEMENT_SIZE[static_cast<int>(m_element_type)];
		constexpr UINT offset = 0;

		p_context->IASetVertexBuffers(slot, 1, m_buffer.GetAddressOf(), &stride, &offset);
	}

	void VertexBuffer::SetComputeInput(ID3D11DeviceContext* p_context, int slot)
	{
		if (!m_SRV)
			return;
		p_context->CSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
	}

	void VertexBuffer::SetComputeOutput(ID3D11DeviceContext* p_context, int slot)
	{
		if (!m_UAV)
			return;
		p_context->CSSetUnorderedAccessViews(slot, 1, m_UAV.GetAddressOf(), nullptr);
	}

}	// namespace TKGEngine