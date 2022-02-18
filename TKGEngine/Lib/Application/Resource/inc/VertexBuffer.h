#pragma once

#include "../inc/VertexElement.h"
#include "Buffer_Defined.h"

#include <d3d11.h>
#include <wrl.h>

#include <mutex>


namespace TKGEngine
{
	/// <summary>
	/// Vertex Buffer module
	/// </summary>
	class VertexBuffer
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		VertexBuffer() = default;
		virtual ~VertexBuffer() = default;
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;

		bool Create(
			void* p_data, 
			int stride_size/*[Byte]*/, 
			int row_count/*[Number of element]*/, 
			VERTEX_ELEMENT_TYPE v_type, 
			BUFFER_HEAP_TYPE heap_type,
			bool create_srv = false,
			bool create_uav = false
		);
		bool Resize(int stride_size/*[Byte]*/, int row_count/*[Number of element]*/);
		void Release();

		// Single thread only
		void UpdateSubresource(const void* p_data) const;	// TODO : サイズの決まった要素の配列をコピーするときは第5,6引数が必要かもしれない？
		// Multi thread usable
		void UpdateSubresource(ID3D11DeviceContext* p_context, const void* p_data) const;

		// Single thread only
		bool Map(void** pp_dst) const;
		void Unmap() const;
		// ~Single thread only

		// Multi thread usable
		bool Map(ID3D11DeviceContext* p_context, void** pp_dst) const;
		void Unmap(ID3D11DeviceContext* p_context) const;
		// ~Multi thread usable

		// VertexBufferのセット
		void Set(ID3D11DeviceContext* p_context, int slot);
		// ComputeShaderにセット
		void SetComputeInput(ID3D11DeviceContext* p_context, int slot);
		void SetComputeOutput(ID3D11DeviceContext* p_context, int slot);

		bool IsCreated() const;

		// ==============================================
		// public variables
		// ==============================================



	private:
		// ==============================================
		// private methods
		// ==============================================
		/* nothing */


		// ==============================================
		// private variables
		// ==============================================
		bool m_has_created = false;
		BUFFER_HEAP_TYPE m_heap_type = BUFFER_HEAP_TYPE::BUFFER_HEAP_DEFAULT;

		VERTEX_ELEMENT_TYPE m_element_type = VERTEX_ELEMENT_TYPE::MAX_TYPE_NUM;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer = nullptr;
		// ByteAddressBuffer用view
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV = nullptr;
		// 出力用view
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_UAV = nullptr;

		std::mutex m_mutex;
	};

	// ------------------------------------------------------
	// inline
	// ------------------------------------------------------
	inline bool VertexBuffer::IsCreated() const
	{
		return m_has_created;
	}


}	// namespace TKGEngine