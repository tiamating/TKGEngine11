#pragma once

#include "../../Resource/inc/Shader_Defined.h"

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <memory>

#include <unordered_map>
#include <vector>
#include <string>

#include <mutex>


namespace TKGEngine
{
	/// <summary>
	/// Structured Buffer module
	/// </summary>
	class StructuredBuffer
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		StructuredBuffer() = default;
		virtual ~StructuredBuffer() = default;
		StructuredBuffer(const StructuredBuffer&) = delete;
		StructuredBuffer& operator=(const StructuredBuffer&) = delete;

		bool Create(int stride_size, int row_count, bool create_uav, const void* p_data);
		void Release();

		//bool Resize(int stride_size, int row_count, const void* p_data);

		void SetSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility);
		void SetUAV(ID3D11DeviceContext* p_context, int slot);

		ID3D11Buffer* GetBuffer() const;

		ID3D11UnorderedAccessView* GetUAV() const;
		ID3D11UnorderedAccessView* const* GetAddressOfUAV() const;

		ID3D11ShaderResourceView* GetSRV() const;
		ID3D11ShaderResourceView* const* GetAddressOfSRV() const;


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================
		//void UpdateSubresource();


		// ==============================================
		// private variables
		// ==============================================
		int m_stride_size = 0;
		int m_row_count = 0;
		bool m_has_uav = false;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_SB;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_UAV;
	};

}	// namespace TKGEngine