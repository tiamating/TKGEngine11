
#include "Graphics_CommandList.h"

#include <cassert>

namespace TKGEngine::Graphics
{
	ICommandList* ICommandList::CreateInterface()
	{
		return new CommandList();
	}

	CommandList::~CommandList()
	{
		Release();
	}

	bool CommandList::Init(ID3D11Device* p_device)
	{
		assert(p_device != nullptr);

		m_is_init = true;
		HRESULT hr = S_OK;

		// Create DC
		hr = p_device->CreateDeferredContext(0, m_dc.GetAddressOf());
		if (FAILED(hr))
		{
			assert(0 && "failed ID3D11Device->CreateDeferredContext() CommandList::Init()");
			Release();
			return false;
		}

		return true;
	}

	void CommandList::Release()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_is_init == true)
		{
			m_dc.Reset();
			m_cmd_list.Reset();

			m_is_init = false;
		}
	}


	ID3D11DeviceContext* CommandList::GetDC()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_dc.Get();
	}

	//void CommandList::End(ID3D11DeviceContext* p_ic)
	//{
	//	m_queries_ts->End(p_ic);
	//}

	//void CommandList::GetData(ID3D11DeviceContext* p_ic)
	//{
	//	m_queries_ts->GetData(p_ic);
	//}

	//UINT64 CommandList::GetTimeStamp() const
	//{
	//	return m_queries_ts->GetTimeStamp();
	//}

	void CommandList::ExecuteCommandList(ID3D11DeviceContext* p_context)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		p_context->ExecuteCommandList(m_cmd_list.Get(), FALSE);
		m_cmd_list.Reset();
	}

	HRESULT CommandList::FinishCommandList()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		return m_dc->FinishCommandList(FALSE, m_cmd_list.GetAddressOf());
	}

	void CommandList::GenerateMips(ID3D11ShaderResourceView* p_SRV)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->GenerateMips(p_SRV);
	}

	void CommandList::ClearRenderTargetView(ID3D11RenderTargetView* p_RTV, const FLOAT color_RGBA[4])
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->ClearRenderTargetView(p_RTV, color_RGBA);
	}

	void CommandList::ClearDepthStencilView(ID3D11DepthStencilView* p_DSV, UINT clear_flags, FLOAT depth, UINT8 stencil)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->ClearDepthStencilView(p_DSV, clear_flags, depth, stencil);
	}

	void CommandList::ClearUnorderedAccessViewFloat(ID3D11UnorderedAccessView* p_UAV, const FLOAT values[4])
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->ClearUnorderedAccessViewFloat(p_UAV, values);
	}

	void CommandList::ClearUnorderedAccessViewUint(ID3D11UnorderedAccessView* p_UAV, const UINT values[4])
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->ClearUnorderedAccessViewUint(p_UAV, values);
	}

	void CommandList::Draw(UINT vertex_count, UINT start_vertex_location)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->Draw(vertex_count, start_vertex_location);
	}

	void CommandList::DrawIndexed(UINT index_count, UINT start_index_location, INT base_vertex_location)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->DrawIndexed(index_count, start_index_location, base_vertex_location);
	}

	void CommandList::DrawInstanced(UINT vertex_count_per_instance, UINT instance_count, UINT start_vertex_location, UINT start_instance_location)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->DrawInstanced(vertex_count_per_instance, instance_count, start_vertex_location, start_instance_location);
	}

	void CommandList::DrawInstancedIndirect(ID3D11Buffer* p_buffer_for_args, UINT aligned_byte_offset_for_args)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->DrawInstancedIndirect(p_buffer_for_args, aligned_byte_offset_for_args);
	}

	void CommandList::DrawIndexedInstanced(UINT index_count_per_instance, UINT instance_count, UINT start_index_location, INT base_vertex_location, UINT start_instance_location)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->DrawIndexedInstanced(index_count_per_instance, instance_count, start_index_location, base_vertex_location, start_instance_location);
	}

	void CommandList::DrawIndexedInstancedIndirect(ID3D11Buffer* p_buffer_for_args, UINT aligned_byte_offset_for_args)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->DrawIndexedInstancedIndirect(p_buffer_for_args, aligned_byte_offset_for_args);
	}

	void CommandList::IASetIndexBuffer(ID3D11Buffer* p_index_buffer, DXGI_FORMAT format, UINT offset)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->IASetIndexBuffer(p_index_buffer, format, offset);
	}

	void CommandList::IASetInputLayout(ID3D11InputLayout* p_input_layout)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->IASetInputLayout(p_input_layout);
	}

	void CommandList::IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->IASetPrimitiveTopology(topology);
	}

	void CommandList::IASetVertexBuffers(UINT start_slot, UINT num_buffers, ID3D11Buffer* const* pp_vertex_buffers, const UINT* p_strides, const UINT* p_offsets)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->IASetVertexBuffers(start_slot, num_buffers, pp_vertex_buffers, p_strides, p_offsets);
	}

	void CommandList::OMSetBlendState(ID3D11BlendState* p_blend_state, const FLOAT blend_factor[4], UINT sample_mask)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->OMSetBlendState(p_blend_state, blend_factor, sample_mask);
	}

	void CommandList::OMSetDepthStencilState(ID3D11DepthStencilState* p_depth_stencil_state, UINT stencil_ref)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->OMSetDepthStencilState(p_depth_stencil_state, stencil_ref);
	}

	void CommandList::OMSetRenderTargets(UINT num_views, ID3D11RenderTargetView* const* pp_RTVs, ID3D11DepthStencilView* p_DSV)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->OMSetRenderTargets(num_views, pp_RTVs, p_DSV);
	}

	void CommandList::OMSetRenderTargetsAndUnorderedAccessViews(
		UINT num_RTVs, ID3D11RenderTargetView* const* pp_RTVs, ID3D11DepthStencilView* p_DSV, 
		UINT UAV_start_slot, UINT num_UAVs, ID3D11UnorderedAccessView* const* pp_UAVs, const UINT* p_UAV_initial_counts)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->OMSetRenderTargetsAndUnorderedAccessViews(num_RTVs, pp_RTVs, p_DSV, UAV_start_slot, num_UAVs, pp_UAVs, p_UAV_initial_counts);
	}

	void CommandList::RSSetScissorRects(UINT num_rects, const D3D11_RECT* p_rects)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->RSSetScissorRects(num_rects, p_rects);
	}

	void CommandList::RSSetViewports(UINT num_viewports, const D3D11_VIEWPORT* p_viewports)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->RSSetViewports(num_viewports, p_viewports);
	}

	void CommandList::RSSetState(ID3D11RasterizerState* p_RS)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->RSSetState(p_RS);
	}

	void CommandList::VSSetConstantBuffers(UINT start_slot, UINT num_buffers, ID3D11Buffer* const* pp_constant_buffers)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->VSSetConstantBuffers(start_slot, num_buffers, pp_constant_buffers);
	}

	void CommandList::VSSetSamplers(UINT start_slot, UINT num_samplers, ID3D11SamplerState* const* pp_samplers)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->VSSetSamplers(start_slot, num_samplers, pp_samplers);
	}

	void CommandList::VSSetShader(ID3D11VertexShader* p_VS, ID3D11ClassInstance* const* pp_class_instances, UINT num_class_instances)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->VSSetShader(p_VS, pp_class_instances, num_class_instances);
	}

	void CommandList::VSSetShaderResources(UINT start_slot, UINT num_views, ID3D11ShaderResourceView* const* pp_SRVs)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->VSSetShaderResources(start_slot, num_views, pp_SRVs);
	}

	void CommandList::PSSetConstantBuffers(UINT start_slot, UINT num_buffers, ID3D11Buffer* const* pp_constant_buffers)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->PSSetConstantBuffers(start_slot, num_buffers, pp_constant_buffers);
	}

	void CommandList::PSSetSamplers(UINT start_slot, UINT num_samplers, ID3D11SamplerState* const* pp_samplers)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->PSSetSamplers(start_slot, num_samplers, pp_samplers);
	}

	void CommandList::PSSetShader(ID3D11PixelShader* p_PS, ID3D11ClassInstance* const* pp_class_instances, UINT num_class_instances)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->PSSetShader(p_PS, pp_class_instances, num_class_instances);
	}

	void CommandList::PSSetShaderResources(UINT start_slot, UINT num_views, ID3D11ShaderResourceView* const* pp_SRVs)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->PSSetShaderResources(start_slot, num_views, pp_SRVs);
	}

	void CommandList::GSSetConstantBuffers(UINT start_slot, UINT num_buffers, ID3D11Buffer* const* pp_constant_buffers)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->GSSetConstantBuffers(start_slot, num_buffers, pp_constant_buffers);
	}

	void CommandList::GSSetSamplers(UINT start_slot, UINT num_samplers, ID3D11SamplerState* const* pp_samplers)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->GSSetSamplers(start_slot, num_samplers, pp_samplers);
	}

	void CommandList::GSSetShader(ID3D11GeometryShader* p_GS, ID3D11ClassInstance* const* pp_class_instances, UINT num_class_instances)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->GSSetShader(p_GS, pp_class_instances, num_class_instances);
	}

	void CommandList::GSSetShaderResources(UINT start_slot, UINT num_views, ID3D11ShaderResourceView* const* pp_SRVs)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->GSSetShaderResources(start_slot, num_views, pp_SRVs);
	}

	void CommandList::DSSetConstantBuffers(UINT start_slot, UINT num_buffers, ID3D11Buffer* const* pp_constant_buffers)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->DSSetConstantBuffers(start_slot, num_buffers, pp_constant_buffers);
	}

	void CommandList::DSSetSamplers(UINT start_slot, UINT num_samplers, ID3D11SamplerState* const* pp_samplers)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->DSSetSamplers(start_slot, num_samplers, pp_samplers);
	}

	void CommandList::DSSetShader(ID3D11DomainShader* p_DS, ID3D11ClassInstance* const* pp_class_instances, UINT num_class_instances)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->DSSetShader(p_DS, pp_class_instances, num_class_instances);
	}

	void CommandList::DSSetShaderResources(UINT start_slot, UINT num_views, ID3D11ShaderResourceView* const* pp_SRVs)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->DSSetShaderResources(start_slot, num_views, pp_SRVs);
	}

	void CommandList::HSSetConstantBuffers(UINT start_slot, UINT num_buffers, ID3D11Buffer* const* pp_constant_buffers)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->HSSetConstantBuffers(start_slot, num_buffers, pp_constant_buffers);
	}

	void CommandList::HSSetSamplers(UINT start_slot, UINT num_samplers, ID3D11SamplerState* const* pp_samplers)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->HSSetSamplers(start_slot, num_samplers, pp_samplers);
	}

	void CommandList::HSSetShader(ID3D11HullShader* p_HS, ID3D11ClassInstance* const* pp_class_instances, UINT num_class_instances)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->HSSetShader(p_HS, pp_class_instances, num_class_instances);
	}

	void CommandList::HSSetShaderResources(UINT start_slot, UINT num_views, ID3D11ShaderResourceView* const* pp_SRVs)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->HSSetShaderResources(start_slot, num_views, pp_SRVs);
	}

	void CommandList::CSSetConstantBuffers(UINT start_slot, UINT num_buffers, ID3D11Buffer* const* pp_constant_buffers)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->CSSetConstantBuffers(start_slot, num_buffers, pp_constant_buffers);
	}

	void CommandList::CSSetSamplers(UINT start_slot, UINT num_samplers, ID3D11SamplerState* const* pp_samplers)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->CSSetSamplers(start_slot, num_samplers, pp_samplers);
	}

	void CommandList::CSSetShader(ID3D11ComputeShader* p_CS, ID3D11ClassInstance* const* pp_class_instances, UINT num_class_instances)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->CSSetShader(p_CS, pp_class_instances, num_class_instances);
	}

	void CommandList::CSSetShaderResources(UINT start_slot, UINT num_views, ID3D11ShaderResourceView* const* pp_SRVs)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->CSSetShaderResources(start_slot, num_views, pp_SRVs);
	}

	void CommandList::CSSetUnorderedAccessViews(UINT start_slot, UINT num_UAVs, ID3D11UnorderedAccessView* const* pp_UAVs, const UINT* p_UAV_initial_counts)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->CSSetUnorderedAccessViews(start_slot, num_UAVs, pp_UAVs, p_UAV_initial_counts);
	}

	void CommandList::Dispatch(UINT thread_group_count_x, UINT thread_group_count_y, UINT thread_group_count_z)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->Dispatch(thread_group_count_x, thread_group_count_y, thread_group_count_z);
	}

	void CommandList::DispatchIndirect(ID3D11Buffer* p_buffer_for_args, UINT aligned_byte_offset_for_args)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->DispatchIndirect(p_buffer_for_args, aligned_byte_offset_for_args);
	}

	void CommandList::Map(ID3D11Resource* p_resource, UINT subresource, D3D11_MAP map_type, UINT map_flags, D3D11_MAPPED_SUBRESOURCE* p_mapped_resource)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->Map(p_resource, subresource, map_type, map_flags, p_mapped_resource);
	}

	void CommandList::Unmap(ID3D11Resource* p_resource, UINT subresource)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->Unmap(p_resource, subresource);
	}

	void CommandList::UpdateSubresource(
		ID3D11Resource* p_dst_resource, UINT dst_subresource, const D3D11_BOX* p_dst_box, 
		const void* p_src_data, UINT src_row_pitch, UINT src_depth_pitch)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->UpdateSubresource(
			p_dst_resource, dst_subresource, p_dst_box,
			p_src_data, src_row_pitch, src_depth_pitch);
	}

	void CommandList::CopyResource(ID3D11Resource* p_dst, ID3D11Resource* p_src)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->CopyResource(p_dst, p_src);
	}

	void CommandList::CopyStructureCount(ID3D11Buffer* p_dst_buffer, UINT dst_aligned_byte_offset, ID3D11UnorderedAccessView* p_src_UAV)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->CopyStructureCount(p_dst_buffer, dst_aligned_byte_offset, p_src_UAV);
	}

	void CommandList::CopySubresourceRegion(
		ID3D11Resource* p_dst_resource, UINT dst_subresource, UINT dst_x, UINT dst_y, UINT dst_z,
		ID3D11Resource* p_src_resource, UINT src_subresource, const D3D11_BOX* p_src_box)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_dc->CopySubresourceRegion(
			p_dst_resource, dst_subresource, dst_x, dst_y, dst_z,
			p_src_resource, src_subresource, p_src_box);
	}

}// namespace TKGEngine::Graphics