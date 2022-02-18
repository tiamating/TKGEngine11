#pragma once

#include <d3d11.h>


namespace TKGEngine::Graphics
{

	class ICommandList
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static ICommandList* CreateInterface();

		ICommandList() = default;
		virtual ~ICommandList() = default;
		ICommandList(const ICommandList&) = delete;
		ICommandList& operator=(const ICommandList&) = delete;

		virtual bool Init(ID3D11Device* p_device) = 0;
		virtual void Release() = 0;

		//virtual ID3D11CommandList* GetCommandList() = 0;
		virtual ID3D11DeviceContext* GetDC() = 0;

		//virtual void Begin() = 0;
		//virtual void End(ID3D11DeviceContext* p_ic) = 0;
		//virtual void GetData(ID3D11DeviceContext* p_ic) = 0;	// refarence IQuery class
		//virtual UINT64 GetTimeStamp() const = 0;

		virtual void ExecuteCommandList(ID3D11DeviceContext* p_context) = 0;
		virtual HRESULT FinishCommandList() = 0;

		virtual void GenerateMips(ID3D11ShaderResourceView* p_SRV) = 0;

		virtual void ClearRenderTargetView(
			ID3D11RenderTargetView* p_RTV, const FLOAT color_RGBA[4]) = 0;
		virtual void ClearDepthStencilView(
			ID3D11DepthStencilView* p_DSV, UINT clear_flags, FLOAT depth, UINT8 stencil) = 0;
		virtual void ClearUnorderedAccessViewFloat(
			ID3D11UnorderedAccessView* p_UAV, const FLOAT values[4]) = 0;
		virtual void ClearUnorderedAccessViewUint(
			ID3D11UnorderedAccessView* p_UAV, const UINT values[4]) = 0;

		virtual void Draw(
			UINT vertex_count, UINT start_vertex_location) = 0;
		virtual void DrawIndexed(
			UINT index_count, UINT start_index_location, INT base_vertex_location) = 0;
		virtual void DrawInstanced(
			UINT vertex_count_per_instance,
			UINT instance_count,
			UINT start_vertex_location,
			UINT start_instance_location) = 0;
		virtual void DrawInstancedIndirect(
			ID3D11Buffer* p_buffer_for_args, UINT aligned_byte_offset_for_args) = 0;
		virtual void DrawIndexedInstanced(
			UINT index_count_per_instance,
			UINT instance_count,
			UINT start_index_location,
			INT base_vertex_location,
			UINT start_instance_location) = 0;
		virtual void DrawIndexedInstancedIndirect(
			ID3D11Buffer* p_buffer_for_args, UINT aligned_byte_offset_for_args) = 0;

		virtual void IASetIndexBuffer(
			ID3D11Buffer* p_index_buffer, DXGI_FORMAT format, UINT offset) = 0;
		virtual void IASetInputLayout(
			ID3D11InputLayout* p_input_layout) = 0;
		virtual void IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY topology) = 0;
		virtual void IASetVertexBuffers(
			UINT start_slot,
			UINT num_buffers,
			ID3D11Buffer* const* pp_vertex_buffers,
			const UINT* p_strides,
			const UINT* p_offsets) = 0;

		virtual void OMSetBlendState(
			ID3D11BlendState* p_blend_state,
			const FLOAT blend_factor[4],
			UINT sample_mask) = 0;
		virtual void OMSetDepthStencilState(
			ID3D11DepthStencilState* p_depth_stencil_state,
			UINT stencil_ref) = 0;
		virtual void OMSetRenderTargets(
			UINT num_views,
			ID3D11RenderTargetView* const* pp_RTVs,
			ID3D11DepthStencilView* p_DSV) = 0;
		virtual void OMSetRenderTargetsAndUnorderedAccessViews(
			UINT num_RTVs,
			ID3D11RenderTargetView* const* pp_RTVs,
			ID3D11DepthStencilView* p_DSV,
			UINT UAV_start_slot,
			UINT num_UAVs,
			ID3D11UnorderedAccessView* const* pp_UAVs,
			const UINT* p_UAV_initial_counts) = 0;

		virtual void RSSetScissorRects(
			UINT num_rects,
			const D3D11_RECT* p_rects) = 0;
		virtual void RSSetViewports(
			UINT num_viewports,
			const D3D11_VIEWPORT* p_viewports) = 0;
		virtual void RSSetState(
			ID3D11RasterizerState* p_RS) = 0;

		virtual void VSSetConstantBuffers(
			UINT start_slot, UINT num_buffers, ID3D11Buffer* const* pp_constant_buffers) = 0;
		virtual void VSSetSamplers(
			UINT start_slot, UINT num_samplers, ID3D11SamplerState* const* pp_samplers) = 0;
		virtual void VSSetShader(
			ID3D11VertexShader* p_VS, ID3D11ClassInstance* const* pp_class_instances, UINT num_class_instances) = 0;
		virtual void VSSetShaderResources(
			UINT start_slot, UINT num_views, ID3D11ShaderResourceView* const* pp_SRVs) = 0;

		virtual void PSSetConstantBuffers(
			UINT start_slot, UINT num_buffers, ID3D11Buffer* const* pp_constant_buffers) = 0;
		virtual void PSSetSamplers(
			UINT start_slot, UINT num_samplers, ID3D11SamplerState* const* pp_samplers) = 0;
		virtual void PSSetShader(
			ID3D11PixelShader* p_PS, ID3D11ClassInstance* const* pp_class_instances, UINT num_class_instances) = 0;
		virtual void PSSetShaderResources(
			UINT start_slot, UINT num_views, ID3D11ShaderResourceView* const* pp_SRVs) = 0;

		virtual void GSSetConstantBuffers(
			UINT start_slot, UINT num_buffers, ID3D11Buffer* const* pp_constant_buffers) = 0;
		virtual void GSSetSamplers(
			UINT start_slot, UINT num_samplers, ID3D11SamplerState* const* pp_samplers) = 0;
		virtual void GSSetShader(
			ID3D11GeometryShader* p_GS, ID3D11ClassInstance* const* pp_class_instances, UINT num_class_instances) = 0;
		virtual void GSSetShaderResources(
			UINT start_slot, UINT num_views, ID3D11ShaderResourceView* const* pp_SRVs) = 0;

		virtual void DSSetConstantBuffers(
			UINT start_slot, UINT num_buffers, ID3D11Buffer* const* pp_constant_buffers) = 0;
		virtual void DSSetSamplers(
			UINT start_slot, UINT num_samplers, ID3D11SamplerState* const* pp_samplers) = 0;
		virtual void DSSetShader(
			ID3D11DomainShader* p_DS, ID3D11ClassInstance* const* pp_class_instances, UINT num_class_instances) = 0;
		virtual void DSSetShaderResources(
			UINT start_slot, UINT num_views, ID3D11ShaderResourceView* const* pp_SRVs) = 0;

		virtual void HSSetConstantBuffers(
			UINT start_slot, UINT num_buffers, ID3D11Buffer* const* pp_constant_buffers) = 0;
		virtual void HSSetSamplers(
			UINT start_slot, UINT num_samplers, ID3D11SamplerState* const* pp_samplers) = 0;
		virtual void HSSetShader(
			ID3D11HullShader* p_HS, ID3D11ClassInstance* const* pp_class_instances, UINT num_class_instances) = 0;
		virtual void HSSetShaderResources(
			UINT start_slot, UINT num_views, ID3D11ShaderResourceView* const* pp_SRVs) = 0;


		virtual void CSSetConstantBuffers(
			UINT start_slot, UINT num_buffers, ID3D11Buffer* const * pp_constant_buffers) = 0;
		virtual void CSSetSamplers(
			UINT start_slot, UINT num_samplers, ID3D11SamplerState* const * pp_samplers) = 0;
		virtual void CSSetShader(
			ID3D11ComputeShader* p_CS, ID3D11ClassInstance* const* pp_class_instances, UINT num_class_instances) = 0;
		virtual void CSSetShaderResources(
			UINT start_slot, UINT num_views, ID3D11ShaderResourceView* const* pp_SRVs) = 0;
		virtual void CSSetUnorderedAccessViews(
			UINT start_slot, UINT num_UAVs, ID3D11UnorderedAccessView* const* pp_UAVs, const UINT* p_UAV_initial_counts) = 0;
		virtual void Dispatch(
			UINT thread_group_count_x, UINT thread_group_count_y, UINT thread_group_count_z) = 0;
		virtual void DispatchIndirect(
			ID3D11Buffer* p_buffer_for_args, UINT aligned_byte_offset_for_args) = 0;


		// DCをMappingすると一時メモリ確保されそこにコピーするため、TypeはDiscardするようにする
		virtual void Map(
			ID3D11Resource* p_resource,
			UINT subresource,
			D3D11_MAP map_type,
			UINT map_flags,
			D3D11_MAPPED_SUBRESOURCE* p_mapped_resource) = 0;
		virtual void Unmap(
			ID3D11Resource* p_resource,
			UINT subresource) = 0;
		virtual void UpdateSubresource(
			ID3D11Resource* p_dst_resource,
			UINT dst_subresource,
			const D3D11_BOX* p_dst_box,
			const void* p_src_data,
			UINT src_row_pitch,
			UINT src_depth_pitch) = 0;
		virtual void CopyResource(
			ID3D11Resource* p_dst, ID3D11Resource* p_src) = 0;
		virtual void CopyStructureCount(
			ID3D11Buffer* p_dst_buffer, UINT dst_aligned_byte_offset, ID3D11UnorderedAccessView* p_src_UAV) = 0;
		virtual void CopySubresourceRegion(
			ID3D11Resource* p_dst_resource, UINT dst_subresource, UINT dst_x, UINT dst_y, UINT dst_z,
			ID3D11Resource* p_src_resource, UINT src_subresource, const D3D11_BOX* p_src_box) = 0;


	};

}	// namespace TKGEngine::Graphics