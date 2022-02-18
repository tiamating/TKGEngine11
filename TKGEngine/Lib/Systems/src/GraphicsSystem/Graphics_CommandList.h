#pragma once

#include "Graphics_ICommandList.h"

#include "../../inc/Graphics_Defined.h"

#include <wrl.h>
#include <memory>
#include <mutex>

namespace TKGEngine::Graphics
{
	class CommandList
		: public ICommandList
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		CommandList() = default;
		virtual ~CommandList();
		CommandList(const CommandList&) = delete;
		CommandList& operator=(const CommandList&) = delete;

		// ICommandList
		bool Init(ID3D11Device* p_device) override;
		void Release() override;
		//ID3D11CommandList* GetCommandList() override;
		ID3D11DeviceContext* GetDC() override;

		// void Begin() override;
		//void End(ID3D11DeviceContext* p_ic) override;
		//void GetData(ID3D11DeviceContext* p_ic) override;
		//UINT64 GetTimeStamp() const override;

		void ExecuteCommandList(ID3D11DeviceContext* p_context) override;
		HRESULT FinishCommandList() override;

		void GenerateMips(ID3D11ShaderResourceView* p_SRV) override;

		void ClearRenderTargetView(
			ID3D11RenderTargetView* p_RTV, const FLOAT color_RGBA[4]) override;
		void ClearDepthStencilView(
			ID3D11DepthStencilView* p_DSV, UINT clear_flags, FLOAT depth, UINT8 stencil) override;
		void ClearUnorderedAccessViewFloat(
			ID3D11UnorderedAccessView* p_UAV, const FLOAT values[4]) override;
		void ClearUnorderedAccessViewUint(
			ID3D11UnorderedAccessView* p_UAV, const UINT values[4]) override;

		void Draw(
			UINT vertex_count, UINT start_vertex_location) override;
		void DrawIndexed(
			UINT index_count, UINT start_index_location, INT base_vertex_location) override;
		void DrawInstanced(
			UINT vertex_count_per_instance,
			UINT instance_count,
			UINT start_vertex_location,
			UINT start_instance_location) override;
		void DrawInstancedIndirect(
			ID3D11Buffer* p_buffer_for_args, UINT aligned_byte_offset_for_args) override;
		void DrawIndexedInstanced(
			UINT index_count_per_instance,
			UINT instance_count,
			UINT start_index_location,
			INT base_vertex_location,
			UINT start_instance_location) override;
		void DrawIndexedInstancedIndirect(
			ID3D11Buffer* p_buffer_for_args, UINT aligned_byte_offset_for_args) override;

		void IASetIndexBuffer(
			ID3D11Buffer* p_index_buffer, DXGI_FORMAT format, UINT offset) override;
		void IASetInputLayout(
			ID3D11InputLayout* p_input_layout) override;
		void IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY topology) override;
		void IASetVertexBuffers(
			UINT start_slot,
			UINT num_buffers,
			ID3D11Buffer* const* pp_vertex_buffers,
			const UINT* p_strides,
			const UINT* p_offsets) override;

		void OMSetBlendState(
			ID3D11BlendState* p_blend_state,
			const FLOAT blend_factor[4],
			UINT sample_mask) override;
		void OMSetDepthStencilState(
			ID3D11DepthStencilState* p_depth_stencil_state,
			UINT stencil_ref) override;
		void OMSetRenderTargets(
			UINT num_views,
			ID3D11RenderTargetView* const* pp_RTVs,
			ID3D11DepthStencilView* p_DSV) override;
		void OMSetRenderTargetsAndUnorderedAccessViews(
			UINT num_RTVs,
			ID3D11RenderTargetView* const* pp_RTVs,
			ID3D11DepthStencilView* p_DSV,
			UINT UAV_start_slot,
			UINT num_UAVs,
			ID3D11UnorderedAccessView* const* pp_UAVs,
			const UINT* p_UAV_initial_counts) override;

		void RSSetScissorRects(
			UINT num_rects,
			const D3D11_RECT* p_rects) override;
		void RSSetViewports(
			UINT num_viewports,
			const D3D11_VIEWPORT* p_viewports) override;
		void RSSetState(
			ID3D11RasterizerState* p_RS) override;

		void VSSetConstantBuffers(
			UINT start_slot, UINT num_buffers, ID3D11Buffer* const* pp_constant_buffers) override;
		void VSSetSamplers(
			UINT start_slot, UINT num_samplers, ID3D11SamplerState* const* pp_samplers) override;
		void VSSetShader(
			ID3D11VertexShader* p_VS, ID3D11ClassInstance* const* pp_class_instances, UINT num_class_instances) override;
		void VSSetShaderResources(
			UINT start_slot, UINT num_views, ID3D11ShaderResourceView* const* pp_SRVs) override;

		void PSSetConstantBuffers(
			UINT start_slot, UINT num_buffers, ID3D11Buffer* const* pp_constant_buffers) override;
		void PSSetSamplers(
			UINT start_slot, UINT num_samplers, ID3D11SamplerState* const* pp_samplers) override;
		void PSSetShader(
			ID3D11PixelShader* p_PS, ID3D11ClassInstance* const* pp_class_instances, UINT num_class_instances) override;
		void PSSetShaderResources(
			UINT start_slot, UINT num_views, ID3D11ShaderResourceView* const* pp_SRVs) override;

		void GSSetConstantBuffers(
			UINT start_slot, UINT num_buffers, ID3D11Buffer* const* pp_constant_buffers) override;
		void GSSetSamplers(
			UINT start_slot, UINT num_samplers, ID3D11SamplerState* const* pp_samplers) override;
		void GSSetShader(
			ID3D11GeometryShader* p_GS, ID3D11ClassInstance* const* pp_class_instances, UINT num_class_instances) override;
		void GSSetShaderResources(
			UINT start_slot, UINT num_views, ID3D11ShaderResourceView* const* pp_SRVs) override;

		void DSSetConstantBuffers(
			UINT start_slot, UINT num_buffers, ID3D11Buffer* const* pp_constant_buffers) override;
		void DSSetSamplers(
			UINT start_slot, UINT num_samplers, ID3D11SamplerState* const* pp_samplers) override;
		void DSSetShader(
			ID3D11DomainShader* p_DS, ID3D11ClassInstance* const* pp_class_instances, UINT num_class_instances) override;
		void DSSetShaderResources(
			UINT start_slot, UINT num_views, ID3D11ShaderResourceView* const* pp_SRVs) override;

		void HSSetConstantBuffers(
			UINT start_slot, UINT num_buffers, ID3D11Buffer* const* pp_constant_buffers) override;
		void HSSetSamplers(
			UINT start_slot, UINT num_samplers, ID3D11SamplerState* const* pp_samplers) override;
		void HSSetShader(
			ID3D11HullShader* p_HS, ID3D11ClassInstance* const* pp_class_instances, UINT num_class_instances) override;
		void HSSetShaderResources(
			UINT start_slot, UINT num_views, ID3D11ShaderResourceView* const* pp_SRVs) override;


		void CSSetConstantBuffers(
			UINT start_slot, UINT num_buffers, ID3D11Buffer* const* pp_constant_buffers) override;
		void CSSetSamplers(
			UINT start_slot, UINT num_samplers, ID3D11SamplerState* const* pp_samplers) override;
		void CSSetShader(
			ID3D11ComputeShader* p_CS, ID3D11ClassInstance* const* pp_class_instances, UINT num_class_instances) override;
		void CSSetShaderResources(
			UINT start_slot, UINT num_views, ID3D11ShaderResourceView* const* pp_SRVs) override;
		void CSSetUnorderedAccessViews(
			UINT start_slot, UINT num_UAVs, ID3D11UnorderedAccessView* const* pp_UAVs, const UINT* p_UAV_initial_counts) override;
		void Dispatch(
			UINT thread_group_count_x, UINT thread_group_count_y, UINT thread_group_count_z) override;
		void DispatchIndirect(
			ID3D11Buffer* p_buffer_for_args, UINT aligned_byte_offset_for_args) override;

		void Map(
			ID3D11Resource* p_resource,
			UINT subresource,
			D3D11_MAP map_type,
			UINT map_flags,
			D3D11_MAPPED_SUBRESOURCE* p_mapped_resource) override;
		void Unmap(
			ID3D11Resource* p_resource,
			UINT subresource) override;
		void UpdateSubresource(
			ID3D11Resource* p_dst_resource,
			UINT dst_subresource,
			const D3D11_BOX* p_dst_box,
			const void* p_src_data,
			UINT src_row_pitch,
			UINT src_depth_pitch) override;
		void CopyResource(
			ID3D11Resource* p_dst, ID3D11Resource* p_src) override;
		void CopyStructureCount(
			ID3D11Buffer* p_dst_buffer, UINT dst_aligned_byte_offset, ID3D11UnorderedAccessView* p_src_UAV) override;
		void CopySubresourceRegion(
			ID3D11Resource* p_dst_resource, UINT dst_subresource, UINT dst_x, UINT dst_y, UINT dst_z,
			ID3D11Resource* p_src_resource, UINT src_subresource, const D3D11_BOX* p_src_box) override;
		// ~ICommandList

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
		bool m_is_init = false;

		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_dc;
		Microsoft::WRL::ComPtr<ID3D11CommandList> m_cmd_list;
		std::mutex m_mutex;
	};

}	// namespace TKGEngine::Graphics