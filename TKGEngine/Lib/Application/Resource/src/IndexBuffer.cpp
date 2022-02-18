
#include "../inc/IndexBuffer.h"

#include "Systems/inc/IGraphics.h"

#include <cassert>

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	bool IndexBuffer::Create(void* p_data, int row_count, BUFFER_HEAP_TYPE heap_type)
	{
		D3D11_BUFFER_DESC desc = {};
		desc.ByteWidth = sizeof(unsigned) * row_count;
		desc.Usage =
			(heap_type == BUFFER_HEAP_TYPE::BUFFER_HEAP_IMMUTABLE) ? D3D11_USAGE_IMMUTABLE :
			(heap_type == BUFFER_HEAP_TYPE::BUFFER_HEAP_DEFAULT) ? D3D11_USAGE_DEFAULT :
			D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA init_data = {};
		init_data.pSysMem = p_data;
		init_data.SysMemPitch = 0;
		init_data.SysMemSlicePitch = 0;

		auto hr = IGraphics::Get().Device()->CreateBuffer(
			&desc, &init_data, m_buffer.GetAddressOf());
		if (FAILED(hr))
		{
			assert(0 && "failed ID3D11Device::CreateBuffer() IndexBuffer::Create()");
			return false;
		}

		m_heap_type = heap_type;
		m_has_created = true;
		return true;
	}

	bool IndexBuffer::Resize(int row_count)
	{

		assert(0);

		return false;
	}

	void IndexBuffer::Release()
	{
		m_buffer.Reset();
		m_has_created = false;
	}

	void IndexBuffer::UpdateSubreasource(void* p_data)
	{
		if (m_heap_type != BUFFER_HEAP_TYPE::BUFFER_HEAP_DEFAULT)
		{
			return;
		}

		IGraphics::Get().DC(0, Graphics::DC_COPY_PATH::DC_CP_MAIN)->UpdateSubresource(
			m_buffer.Get(), 0, nullptr, p_data, 0, 0);
	}

	bool IndexBuffer::Map(void* p_dst)
	{
		if (m_heap_type != BUFFER_HEAP_TYPE::BUFFER_HEAP_DYNAMIC)
		{
			return false;
		}

		D3D11_MAPPED_SUBRESOURCE mapped_buffer = {};
		auto hr = IGraphics::Get().DC(0, Graphics::DC_COPY_PATH::DC_CP_MAIN)->Map(
			m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_buffer);
		if (FAILED(hr))
		{
			assert(0 && "failed ID3D11DeviceContext::Map() IndexBuffer::Map()");
			p_dst = nullptr;
			return false;
		}

		p_dst = mapped_buffer.pData;
		return true;
	}

	void IndexBuffer::Unmap()
	{
		if (m_heap_type != BUFFER_HEAP_TYPE::BUFFER_HEAP_DYNAMIC)
		{
			return;
		}

		IGraphics::Get().DC(0, Graphics::DC_COPY_PATH::DC_CP_MAIN)->Unmap(m_buffer.Get(), 0);
	}

	void IndexBuffer::Set(ID3D11DeviceContext* p_context)
	{
		p_context->IASetIndexBuffer(m_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

}	// namespace TKGEngine