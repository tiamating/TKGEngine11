#pragma once

#include "Buffer_Defined.h"

#include <d3d11.h>
#include <wrl.h>

#include <mutex>


struct ID3D11Device;

namespace TKGEngine
{
	/// <summary>
	/// Index Buffer module.
	/// Index element format is R32.
	/// </summary>
	class IndexBuffer
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		IndexBuffer() = default;
		virtual ~IndexBuffer() = default;
		IndexBuffer(const IndexBuffer&) = delete;
		IndexBuffer& operator=(const IndexBuffer&) = delete;

		bool Create(void* p_data, int row_count/*[Number of element]*/, BUFFER_HEAP_TYPE heap_type);
		bool Resize(int row_count/*[Number of element]*/);
		void Release();

		void UpdateSubreasource(void* p_data);	// TODO : サイズの決まった要素の配列をコピーするときは第5,6引数が必要かもしれない？
		bool Map(void* p_dst);
		void Unmap();

		void Set(ID3D11DeviceContext* p_context);

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

		Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer = nullptr;

		std::mutex m_mutex;

	};

	// -------------------------------------------------------
	// inline
	// -------------------------------------------------------
	inline bool IndexBuffer::IsCreated() const
	{
		return m_has_created;
	}


}	// namespace TKGEngine