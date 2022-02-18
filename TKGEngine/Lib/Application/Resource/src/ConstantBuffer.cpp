
#include "../inc/ConstantBuffer.h"

#include "Systems/inc/IGraphics.h"

#include <cassert>

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
#pragma region ConstantBuffer
	bool ConstantBuffer::Create(int size, bool mappable)
	{
		return Create(size, nullptr, mappable);
	}

	bool ConstantBuffer::Create(int size, const void* p_data, bool mappable)
	{
		HRESULT hr = S_OK;

		if (size == 0)
		{
			assert(0 && "Invalid argument ConstantBuffer::Create()");
			return false;
		}

		// If already create buffer
		if (m_CB != nullptr)
		{
			Release();
		}

		m_mappable = mappable;
		m_buffer_size = size + (size % 16 == 0 ? 0 : 16 - size % 16);

		// Create Buffer
		{
			D3D11_BUFFER_DESC desc = {};
			desc.Usage = (mappable == true) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = static_cast<UINT>(m_buffer_size);
			desc.CPUAccessFlags = (mappable == true) ? D3D11_CPU_ACCESS_WRITE : 0;

			D3D11_SUBRESOURCE_DATA init_data = {};
			init_data.pSysMem = p_data;
			init_data.SysMemPitch = 0;
			init_data.SysMemSlicePitch = 0;

			hr = IGraphics::Get().Device()->CreateBuffer(
				&desc,
				(p_data == nullptr) ? nullptr : &init_data,
				m_CB.GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateBuffer() ConstantBuffer::Create()");
				return false;
			}
		}

		if (m_mappable == false)
		{
			// Store CPU memory
			for (auto&& memory : m_memories)
			{
				memory.resize(m_buffer_size);
			}
			if (p_data != nullptr)
			{
				memcpy(m_memories[m_current_idx].data(), p_data, size);
			}
		}

		return true;
	}

	void ConstantBuffer::Release()
	{
		m_CB.Reset();
		for (auto&& memory : m_memories)
		{
			memory.clear();
			memory.shrink_to_fit();
		}
		m_memory_has_updated = false;
		m_current_idx = 0;
		m_buffer_size = 0;
	}

	ID3D11Buffer* ConstantBuffer::GetBuffer() const
	{
		return m_CB.Get();
	}

	void ConstantBuffer::SetParam(const void* p_src, size_t size, size_t offset)
	{
		assert(m_mappable == false);

		m_memory_has_updated = true;
		memcpy(m_memories[m_current_idx].data() + offset, p_src, size);
	}

	void* ConstantBuffer::Map(ID3D11DeviceContext* p_context) const
	{
		assert(m_mappable);

		const D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
		D3D11_MAPPED_SUBRESOURCE mapped_buffer = {};
		const auto hr = p_context->Map(m_CB.Get(), 0, map, 0, &mapped_buffer);
		if (FAILED(hr))
		{
			assert(0 && "failed ID3D11DeviceContext::Map() ConstantBuffer::Map()");
			return nullptr;
		}
		return mapped_buffer.pData;
	}

	void ConstantBuffer::Unmap(ID3D11DeviceContext* p_context) const
	{
		assert(m_mappable);

		p_context->Unmap(m_CB.Get(), 0);
	}

	void ConstantBuffer::SetVS(ID3D11DeviceContext* p_dc, int slot)
	{
		// If memory has update, update subresource
		m_mutex.lock();
		if (m_memory_has_updated)
		{
			UpdateSubresource();
		}
		m_mutex.unlock();

		p_dc->VSSetConstantBuffers(slot, 1, m_CB.GetAddressOf());
	}

	void ConstantBuffer::SetPS(ID3D11DeviceContext* p_dc, int slot)
	{
		// If memory has update, update subresource
		m_mutex.lock();
		if (m_memory_has_updated)
		{
			UpdateSubresource();
		}
		m_mutex.unlock();

		p_dc->PSSetConstantBuffers(slot, 1, m_CB.GetAddressOf());
	}

	void ConstantBuffer::SetGS(ID3D11DeviceContext* p_dc, int slot)
	{
		// If memory has update, update subresource
		m_mutex.lock();
		if (m_memory_has_updated)
		{
			UpdateSubresource();
		}
		m_mutex.unlock();

		p_dc->GSSetConstantBuffers(slot, 1, m_CB.GetAddressOf());
	}

	void ConstantBuffer::SetDS(ID3D11DeviceContext* p_dc, int slot)
	{
		// If memory has update, update subresource
		m_mutex.lock();
		if (m_memory_has_updated)
		{
			UpdateSubresource();
		}
		m_mutex.unlock();

		p_dc->DSSetConstantBuffers(slot, 1, m_CB.GetAddressOf());
	}

	void ConstantBuffer::SetHS(ID3D11DeviceContext* p_dc, int slot)
	{
		// If memory has update, update subresource
		m_mutex.lock();
		if (m_memory_has_updated)
		{
			UpdateSubresource();
		}
		m_mutex.unlock();

		p_dc->HSSetConstantBuffers(slot, 1, m_CB.GetAddressOf());
	}

	void ConstantBuffer::SetCS(ID3D11DeviceContext* p_dc, int slot)
	{
		// If memory has update, update subresource
		m_mutex.lock();
		if (m_memory_has_updated)
		{
			UpdateSubresource();
		}
		m_mutex.unlock();

		p_dc->CSSetConstantBuffers(slot, 1, m_CB.GetAddressOf());
	}

	void ConstantBuffer::UpdateSubresource(ID3D11DeviceContext* context)
	{
		ID3D11DeviceContext* _context;
		if (context == nullptr)
			_context = IGraphics::Get().DC(0, Graphics::DC_COPY_PATH::DC_CP_MAIN);
		else
			_context = context;

		_context->UpdateSubresource(m_CB.Get(), 0, nullptr, m_memories[m_current_idx].data(), 0, 0);

		m_memory_has_updated = false;
		const size_t size = m_memories[m_current_idx].size() * sizeof(uint8_t);
		memcpy(m_memories[1 - m_current_idx].data(), m_memories[m_current_idx].data(), size);
		m_current_idx = 1 - m_current_idx;
	}


#pragma endregion

#pragma region ShaderConstantBuffer

#ifdef USE_IMGUI
	void ShaderConstantBuffer::OnGUI()
	{
		// パラメータ名順にGUI表示する
		for (auto& var_name : m_param_names)
		{
			auto& data = m_param_map.at(var_name);
			ImGui::PushID(&data);
			// GUI表示
			switch (data.var_class)
			{
				// Scalar
				case BufferParam::VARIABLE_CLASS::SCALAR:
				{
					OnGUIScalar(data);
				}
				break;
				// Vector
				case BufferParam::VARIABLE_CLASS::VECTOR:
				{
					OnGUIVector(data);
				}
				break;
				// Struct
				case BufferParam::VARIABLE_CLASS::STRUCT:
				{
					OnGUIStruct(data);
				}
				break;
			}
			ImGui::PopID();
		}
	}
	void ShaderConstantBuffer::OnGUIScalar(BufferParam& param, int offset)
	{
		const int total_offset = param.offset + offset;
		// 変数名表示
		ImGui::Text(param.var_name.c_str());
		// 変数名右クリックで表示フラグの変更
		OnRightClickPopup(param);

		// 変数のコントローラ表示
		ImGui::Indent(ImGui::INDENT_VALUE);
		switch (param.var_type)
		{
			// bool1
			case BufferParam::VARIABLE_TYPE::T_BOOL:
			{
				// intを疑似的にboolとする
				int buffer;
				const auto head = m_memories[m_current_idx].data();
				memcpy(&buffer, head + total_offset, param.size);
				bool check = buffer == 0 ? false : true;
				if (ImGui::Checkbox("##T_BOOL", &check))
				{
					buffer = check ? 1 : 0;
					memcpy(head + total_offset, &buffer, param.size);
					m_memory_has_updated = true;
				}
			}
			break;
			// int1
			case BufferParam::VARIABLE_TYPE::T_INT:
			{
				int buffer;
				const auto head = m_memories[m_current_idx].data();
				memcpy(&buffer, head + total_offset, param.size);
				switch (param.gui_type)
				{
					case BufferParam::GUI_TYPE::UNSIGNED_UNLIMIT:
						if (ImGui::DragInt("##T_INT", &buffer, 0.01f, 0, INT_MAX, "%d", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, &buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::UNLIMIT:
						if (ImGui::DragInt("##T_INT", &buffer, 0.01f, -INT_MAX, INT_MAX, "%d", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, &buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					default:
						param.gui_type = BufferParam::GUI_TYPE::UNSIGNED_UNLIMIT;
						break;
				}
			}
			break;
			// uint1
			case BufferParam::VARIABLE_TYPE::T_UINT:
			{
				int buffer;
				const auto head = m_memories[m_current_idx].data();
				memcpy(&buffer, head + total_offset, param.size);
				// UINTのため0 ~ INT_MAXの入力のみ
				if (ImGui::DragInt("##T_UINT", &buffer, 0.01f, 0, INT_MAX, " %d", ImGuiSliderFlags_AlwaysClamp))
				{
					memcpy(head + total_offset, &buffer, param.size);
					m_memory_has_updated = true;
				}
			}
			break;
			// float1
			case BufferParam::VARIABLE_TYPE::T_FLOAT:
			{
				float buffer;
				const auto head = m_memories[m_current_idx].data();
				memcpy(&buffer, head + total_offset, param.size);
				switch (param.gui_type)
				{
					case BufferParam::GUI_TYPE::UNSIGNED_NORM:
						if (ImGui::DragFloat("##T_FLOAT", &buffer, 0.002f, 0.0f, 1.0f, "(0~1) %.4f", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, &buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::UNSIGNED_NORM_NO_ZERO:
						if (ImGui::DragFloat("##T_FLOAT", &buffer, 0.002f, GUI_INPUT_EPSILON, 1.0f, "(eps~1) %.4f", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, &buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::NORM:
						if (ImGui::DragFloat("##T_FLOAT", &buffer, 0.002f, -1.0f, 1.0f, "(-1~1) %.4f", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, &buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::UNSIGNED_UNLIMIT:
						if (ImGui::DragFloat("##T_FLOAT", &buffer, 0.01f, 0.0f, FLT_MAX, "%.1f", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, &buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::UNLIMIT:
						if (ImGui::DragFloat("##T_FLOAT", &buffer, 0.01f, -FLT_MAX, FLT_MAX, "%.1f", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, &buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
				}
			}
			break;
		}
		ImGui::Unindent(ImGui::INDENT_VALUE);
	}
	void ShaderConstantBuffer::OnGUIVector(BufferParam& param, int offset)
	{
		const int total_offset = param.offset + offset;
		// 変数名表示
		ImGui::Text(param.var_name.c_str());
		// 変数名右クリックで表示フラグの変更
		OnRightClickPopup(param);

		// 変数のコントローラ表示
		ImGui::Indent(ImGui::INDENT_VALUE);
		switch (param.var_type)
		{
			// bool2~4
			case BufferParam::VARIABLE_TYPE::T_BOOL:
			{
				OnGUIVectorBool(param, offset);
			}
			break;
			// int2~4
			case BufferParam::VARIABLE_TYPE::T_INT:
			{
				OnGUIVectorInt(param, offset);
			}
			break;
			// uint2~4
			case BufferParam::VARIABLE_TYPE::T_UINT:
			{
				OnGUIVectorUint(param, offset);
			}
			break;
			// float2~4
			case BufferParam::VARIABLE_TYPE::T_FLOAT:
			{
				OnGUIVectorFloat(param, offset);
			}
			break;
		}
		ImGui::Unindent(ImGui::INDENT_VALUE);
	}
	void ShaderConstantBuffer::OnGUIVectorBool(BufferParam& param, int offset)
	{
		const int total_offset = param.offset + offset;
		// intを疑似的にboolとする
		std::vector<int> buffers(param.columns);
		const auto head = m_memories[m_current_idx].data();
		memcpy(buffers.data(), head + total_offset, param.size);
		bool is_change = false;
		// それぞれのチェックボックスを操作
		for (int i = 0; i < param.columns; ++i)
		{
			ImGui::PushID(i);
			bool check = buffers[i] == 0 ? false : true;
			if (ImGui::Checkbox("##T_BOOL", &check))
			{
				buffers[i] = check ? 1 : 0;
				is_change = true;
			}
			ImGui::PopID();
			ImGui::SameLine();
		}
		// SameLine打ち消し用dummyを挿入
		ImGui::Dummy(ImVec2(0.0f, 0.0f));
		// 変更があった場合
		if (is_change)
		{
			memcpy(head + total_offset, buffers.data(), param.size);
			m_memory_has_updated = true;
		}
	}
	void ShaderConstantBuffer::OnGUIVectorInt(BufferParam& param, int offset)
	{
		const int total_offset = param.offset + offset;
		switch (param.columns)
		{
			case 2:
			{
				int buffer[2];
				const auto head = m_memories[m_current_idx].data();
				memcpy(&buffer, head + total_offset, param.size);
				switch (param.gui_type)
				{
					case BufferParam::GUI_TYPE::UNSIGNED_UNLIMIT:
						if (ImGui::DragInt2("##T_INT", buffer, 0.01f, 0, INT_MAX, "%d", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::UNLIMIT:
						if (ImGui::DragInt2("##T_INT", buffer, 0.01f, -INT_MAX, INT_MAX, "%d", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					default:
						param.gui_type = BufferParam::GUI_TYPE::UNSIGNED_UNLIMIT;
						break;
				}
			}
			break;
			case 3:
			{
				int buffer[3];
				const auto head = m_memories[m_current_idx].data();
				memcpy(&buffer, head + total_offset, param.size);
				switch (param.gui_type)
				{
					case BufferParam::GUI_TYPE::UNSIGNED_UNLIMIT:
						if (ImGui::DragInt3("##T_INT", buffer, 0.01f, 0, INT_MAX, "%d", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::UNLIMIT:
						if (ImGui::DragInt3("##T_INT", buffer, 0.01f, -INT_MAX, INT_MAX, "%d", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					default:
						param.gui_type = BufferParam::GUI_TYPE::UNSIGNED_UNLIMIT;
						break;
				}
			}
			break;
			case 4:
			{
				int buffer[4];
				const auto head = m_memories[m_current_idx].data();
				memcpy(&buffer, head + total_offset, param.size);
				switch (param.gui_type)
				{
					case BufferParam::GUI_TYPE::UNSIGNED_UNLIMIT:
						if (ImGui::DragInt4("##T_INT", buffer, 0.01f, 0, INT_MAX, "%d", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::UNLIMIT:
						if (ImGui::DragInt4("##T_INT", buffer, 0.01f, -INT_MAX, INT_MAX, "%d", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					default:
						param.gui_type = BufferParam::GUI_TYPE::UNSIGNED_UNLIMIT;
						break;
				}
			}
			break;
		}
	}
	void ShaderConstantBuffer::OnGUIVectorUint(BufferParam& param, int offset)
	{
		const int total_offset = param.offset + offset;
		switch (param.columns)
		{
			case 2:
			{
				int buffer[2];
				const auto head = m_memories[m_current_idx].data();
				memcpy(buffer, head + total_offset, param.size);
				// UINTのため0 ~ INT_MAXの入力のみ
				if (ImGui::DragInt2("##T_UINT", buffer, 0.01f, 0, INT_MAX, " %d", ImGuiSliderFlags_AlwaysClamp))
				{
					memcpy(head + total_offset, buffer, param.size);
					m_memory_has_updated = true;
				}
			}
			break;
			case 3:
			{
				int buffer[3];
				const auto head = m_memories[m_current_idx].data();
				memcpy(buffer, head + total_offset, param.size);
				// UINTのため0 ~ INT_MAXの入力のみ
				if (ImGui::DragInt3("##T_UINT", buffer, 0.01f, 0, INT_MAX, " %d", ImGuiSliderFlags_AlwaysClamp))
				{
					memcpy(head + total_offset, buffer, param.size);
					m_memory_has_updated = true;
				}
			}
			break;
			case 4:
			{
				int buffer[4];
				const auto head = m_memories[m_current_idx].data();
				memcpy(buffer, head + total_offset, param.size);
				// UINTのため0 ~ INT_MAXの入力のみ
				if (ImGui::DragInt4("##T_UINT", buffer, 0.01f, 0, INT_MAX, " %d", ImGuiSliderFlags_AlwaysClamp))
				{
					memcpy(head + total_offset, buffer, param.size);
					m_memory_has_updated = true;
				}
			}
			break;
		}
	}
	void ShaderConstantBuffer::OnGUIVectorFloat(BufferParam& param, int offset)
	{
		const int total_offset = param.offset + offset;
		switch (param.columns)
		{
			case 2:
			{
				float buffer[2];
				const auto head = m_memories[m_current_idx].data();
				memcpy(buffer, head + total_offset, param.size);
				switch (param.gui_type)
				{
					case BufferParam::GUI_TYPE::UNSIGNED_NORM:
						if (ImGui::DragFloat2("##T_FLOAT", buffer, 0.002f, 0.0f, 1.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::UNSIGNED_NORM_NO_ZERO:
						if (ImGui::DragFloat2("##T_FLOAT", buffer, 0.002f, GUI_INPUT_EPSILON, 1.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::NORM:
						if (ImGui::DragFloat2("##T_FLOAT", buffer, 0.002f, -1.0f, 1.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::UNSIGNED_UNLIMIT:
						if (ImGui::DragFloat2("##T_FLOAT", buffer, 0.01f, 0.0f, FLT_MAX, "%.1f", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::UNLIMIT:
						if (ImGui::DragFloat2("##T_FLOAT", buffer, 0.01f, -FLT_MAX, FLT_MAX, "%.1f", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::COLOR:
						ImGui::Text("This variable is not color.");
						break;
				}
			}
			break;
			case 3:
			{
				float buffer[3];
				const auto head = m_memories[m_current_idx].data();
				memcpy(buffer, head + total_offset, param.size);
				switch (param.gui_type)
				{
					case BufferParam::GUI_TYPE::UNSIGNED_NORM:
						if (ImGui::DragFloat3("##T_FLOAT", buffer, 0.002f, 0.0f, 1.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::UNSIGNED_NORM_NO_ZERO:
						if (ImGui::DragFloat3("##T_FLOAT", buffer, 0.002f, GUI_INPUT_EPSILON, 1.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::NORM:
						if (ImGui::DragFloat3("##T_FLOAT", buffer, 0.002f, -1.0f, 1.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::UNSIGNED_UNLIMIT:
						if (ImGui::DragFloat3("##T_FLOAT", buffer, 0.01f, 0.0f, FLT_MAX, "%.1f", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::UNLIMIT:
						if (ImGui::DragFloat3("##T_FLOAT", buffer, 0.01f, -FLT_MAX, FLT_MAX, "%.1f", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::COLOR:
						if (ImGui::ColorEdit3("##T_FLOAT", buffer))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
				}
			}
			break;
			case 4:
			{
				float buffer[4];
				const auto head = m_memories[m_current_idx].data();
				memcpy(buffer, head + total_offset, param.size);
				switch (param.gui_type)
				{
					case BufferParam::GUI_TYPE::UNSIGNED_NORM:
						if (ImGui::DragFloat4("##T_FLOAT", buffer, 0.002f, 0.0f, 1.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::UNSIGNED_NORM_NO_ZERO:
						if (ImGui::DragFloat4("##T_FLOAT", buffer, 0.002f, GUI_INPUT_EPSILON, 1.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::NORM:
						if (ImGui::DragFloat4("##T_FLOAT", buffer, 0.002f, -1.0f, 1.0f, "%.4f", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::UNSIGNED_UNLIMIT:
						if (ImGui::DragFloat4("##T_FLOAT", buffer, 0.01f, 0.0f, FLT_MAX, "%.1f", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::UNLIMIT:
						if (ImGui::DragFloat4("##T_FLOAT", buffer, 0.01f, -FLT_MAX, FLT_MAX, "%.1f", ImGuiSliderFlags_AlwaysClamp))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
					case BufferParam::GUI_TYPE::COLOR:
						if (ImGui::ColorEdit4("##T_FLOAT", buffer, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf))
						{
							memcpy(head + total_offset, buffer, param.size);
							m_memory_has_updated = true;
						}
						break;
				}
			}
			break;
		}
	}
	void ShaderConstantBuffer::OnGUIStruct(BufferParam& param)
	{
		// 変数名でツリーノード作成
		constexpr ImGuiTreeNodeFlags tree_flags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
		if (ImGui::TreeNodeEx(param.var_name.c_str(), tree_flags))
		{
			// 変数名からメンバリストにアクセスする
			ImGui::Indent(ImGui::INDENT_VALUE);
			auto& member_params = m_struct_members_map.at(param.var_name);
			const size_t member_num = member_params.size();
			// メンバリスト全てのGUIを表示する
			for (size_t i = 0; i < member_num; ++i)
			{
				auto& member_param = member_params.at(i);
				ImGui::PushID(i);
				// メンバ変数の表示
			// GUI表示
				switch (member_param.var_class)
				{
					// Scalar
					case BufferParam::VARIABLE_CLASS::SCALAR:
					{
						OnGUIScalar(member_param, param.offset);
					}
					break;
					// Vector
					case BufferParam::VARIABLE_CLASS::VECTOR:
					{
						OnGUIVector(member_param, param.offset);
					}
					break;
				}
				ImGui::PopID();
			}
			ImGui::Unindent(ImGui::INDENT_VALUE);
			ImGui::TreePop();
		}
	}
	void ShaderConstantBuffer::OnRightClickPopup(BufferParam& param)
	{
		// 表示するアイテムを型によって制限する
		const bool is_float = (param.var_type == BufferParam::VARIABLE_TYPE::T_FLOAT);
		const bool is_signed = (param.var_type == BufferParam::VARIABLE_TYPE::T_FLOAT || param.var_type == BufferParam::VARIABLE_TYPE::T_INT);
		const bool is_bool = (param.var_type == BufferParam::VARIABLE_TYPE::T_BOOL);

		if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup(param.var_name.c_str());
		}
		if (ImGui::BeginPopup(param.var_name.c_str()))
		{
			// boolは選択する必要がない
			if (!is_bool)
			{
				if (is_float)
				{
					// 0 ~ 1
					if (ImGui::Selectable("Zero to One"))
					{
						param.gui_type = BufferParam::GUI_TYPE::UNSIGNED_NORM;
					}
					// 1.0e-4 ~ 1
					if (ImGui::Selectable("Epsilon to One"))
					{
						param.gui_type = BufferParam::GUI_TYPE::UNSIGNED_NORM_NO_ZERO;
					}
					// -1 ~ 1
					if (ImGui::Selectable("negative One to One"))
					{
						param.gui_type = BufferParam::GUI_TYPE::NORM;
					}
				}
				// 0 ~ +inf
				if (ImGui::Selectable("Zero to Inf"))
				{
					param.gui_type = BufferParam::GUI_TYPE::UNSIGNED_UNLIMIT;
				}
				// -inf ~ +inf
				if (is_signed)
				{
					if (ImGui::Selectable("Unlimited"))
					{
						param.gui_type = BufferParam::GUI_TYPE::UNLIMIT;
					}
				}
				if (is_float)
				{
					if (ImGui::Selectable("Color"))
					{
						param.gui_type = BufferParam::GUI_TYPE::COLOR;
					}
				}
			}
			ImGui::EndPopup();
		}
	}
#endif // USE_IMGUI

	bool ShaderConstantBuffer::Create(ID3D11ShaderReflectionConstantBuffer* p_reflection, bool force_clear)
	{
		if (!p_reflection)
		{
			assert(0 && "invalid argument ShaderConstantBuffer::Create()");
			return false;
		}

		Release(force_clear);
		HRESULT hr = S_OK;

		// CBuffer reflection
		D3D11_SHADER_BUFFER_DESC buf_desc = {};
		hr = p_reflection->GetDesc(&buf_desc);
		if (FAILED(hr))
		{
			// シェーダ内でCBの変数を使用していない場合と存在しないスロット番号の場合はfalse
			Release();
			return false;
		}

		// 16byteにアラインメントする
		m_buffer_size = static_cast<int>(buf_desc.Size);
		m_buffer_size = m_buffer_size + (m_buffer_size % 16 == 0 ? 0 : 16 - m_buffer_size % 16);

		// バッファがクリアされているならresizeするだけ
		if (force_clear)
		{
			for (auto&& memory : m_memories)
			{
				memory.resize(m_buffer_size);
			}
		}
		// バッファが残っているなら
		else
		{
			const int prev_size = static_cast<int>(m_memories[0].size());
			// 新しいほうが大きいときはリサイズのみ
			if (m_buffer_size >= prev_size)
			{
				for (auto&& memory : m_memories)
				{
					memory.resize(m_buffer_size);
				}
			}
			// 小さいときは超えた分をeraseする
			else
			{
				for (auto&& memory : m_memories)
				{
					memory.erase(memory.begin() + m_buffer_size, memory.end());
				}
			}
		}
		// パラメータ名リストのリサイズ
		const UINT buf_var_cnt = buf_desc.Variables;
		m_param_names.resize(buf_var_cnt);
		// 変数名の取得を先に行う
		for (UINT i = 0; i < buf_var_cnt; ++i)
		{
			auto p_var = p_reflection->GetVariableByIndex(i);
			if (!p_var)
			{
				continue;
			}
			auto p_type = p_var->GetType();
			// 変数情報
			D3D11_SHADER_VARIABLE_DESC var_desc = {};
			p_var->GetDesc(&var_desc);

			// 変数名を配列に入れる
			m_param_names.at(i) = var_desc.Name;
		}
		// 削除を強制しないなら既存の変数情報を保持する
		if (!force_clear)
		{
			// 変数名から既存のBufferParamは残す
			const auto itr_map_end = m_param_map.end();
			const auto itr_names_begin = m_param_names.begin();
			const auto itr_names_end = m_param_names.end();
			auto itr = m_param_map.begin();
			for (; itr != itr_map_end;)
			{
				// 既存の変数名が新しい名前リストに存在するならそのまま
				auto itr_find = std::find(itr_names_begin, itr_names_end, itr->first);
				if (itr_find != itr_names_end)
				{
					++itr;
					continue;
				}
				// 存在しないならmapから削除
				itr = m_param_map.erase(itr);
			}
		}
		// パラメータの情報を取得
		for (UINT i = 0; i < buf_var_cnt; ++i)
		{
			auto p_var = p_reflection->GetVariableByIndex(i);
			if (!p_var)
			{
				continue;
			}
			auto p_type = p_var->GetType();

			// 変数情報
			D3D11_SHADER_VARIABLE_DESC var_desc = {};
			p_var->GetDesc(&var_desc);
			// 型情報
			D3D11_SHADER_TYPE_DESC type_desc = {};
			p_type->GetDesc(&type_desc);

			// パラメータの情報を名前と結びつける
			BufferParam& param = m_param_map[var_desc.Name];
			param.var_name = var_desc.Name;
			param.offset = var_desc.StartOffset;
			param.size = var_desc.Size;
			param.columns = type_desc.Columns;
			param.array_elements = type_desc.Elements;
			param.var_type = GetVariableType(type_desc.Type);
			param.var_class = GetVariableClass(type_desc.Class);
			// columnsが1でもint1,float1がVectorになるため回避
			if (param.var_class == BufferParam::VARIABLE_CLASS::VECTOR)
			{
				if (param.columns == 1)
				{
					param.var_class = BufferParam::VARIABLE_CLASS::SCALAR;
				}
			}

			// 構造体だったら構造体情報を別で取得する
			if (type_desc.Class == D3D_SHADER_VARIABLE_CLASS::D3D_SVC_STRUCT && type_desc.Members > 0)
			{
				for (UINT mem_idx = 0; mem_idx < type_desc.Members; ++mem_idx)
				{
					// メンバ情報の取得
					D3D11_SHADER_TYPE_DESC member_desc = {};
					auto member_type = p_type->GetMemberTypeByIndex(mem_idx);
					member_type->GetDesc(&member_desc);
					// メンバ情報の登録
					m_struct_members_map[var_desc.Name].emplace_back();
					auto& member_param = m_struct_members_map[var_desc.Name].at(mem_idx);
					// メンバのオフセットは構造体変数の先頭位置が基準
					member_param.offset = member_desc.Offset;
					member_param.columns = member_desc.Columns;
					member_param.size = member_desc.Columns * 4/*byte*/;
					member_param.array_elements = member_desc.Elements;
					member_param.var_type = GetVariableType(member_desc.Type);
					member_param.var_class = GetVariableClass(member_desc.Class);
					// columnsが1でもint1,float1がVectorになるため回避
					if (member_param.var_class == BufferParam::VARIABLE_CLASS::VECTOR)
					{
						if (member_param.columns == 1)
						{
							member_param.var_class = BufferParam::VARIABLE_CLASS::SCALAR;
						}
					}
					member_param.var_name = p_type->GetMemberTypeName(mem_idx);
				}
			}
		}
		// CBufferの作成
		return Create();
	}
	bool ShaderConstantBuffer::Create()
	{
		// バッファサイズが0なら早期リターン
		if(m_buffer_size <= 0)
		{
			return true;
		}

		D3D11_BUFFER_DESC desc = {};
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = static_cast<UINT>(m_buffer_size);
		desc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA init_data = {};
		init_data.pSysMem = m_memories[m_current_idx].data();
		init_data.SysMemPitch = 0;
		init_data.SysMemSlicePitch = 0;

		const auto hr = IGraphics::Get().Device()->CreateBuffer(
			&desc,
			&init_data,
			m_CB.GetAddressOf());
		if (FAILED(hr))
		{
			assert(0 && "failed ID3D11Device::CreateBuffer() ShaderConstantBuffer::Create()");
			return false;
		}

		return true;
	}
	void ShaderConstantBuffer::Release(bool force_clear)
	{
		m_memory_has_updated = false;
		m_current_idx = 0;
		m_buffer_size = 0;
		m_CB.Reset();
		// 現在の中身を完全に消去するなら
		if (force_clear)
		{
			// バッファデータのクリア
			for (auto&& memory : m_memories)
			{
				memory.clear();
				memory.shrink_to_fit();
			}
			// 変数情報のクリア
			m_param_map.clear();
		}
		m_param_names.clear();
		m_param_names.shrink_to_fit();
		m_struct_members_map.clear();
	}

	ID3D11Buffer* ShaderConstantBuffer::GetBuffer() const
	{
		return m_CB.Get();
	}

	//ID3D11Buffer* const* ShaderConstantBuffer::GetAddressOfBuffer() const
	//{
	//	return m_CB.GetAddressOf();
	//}

	bool ShaderConstantBuffer::Contain(const std::string& name) const
	{
		return m_param_map.find(name) != m_param_map.end();
	}

	bool ShaderConstantBuffer::SetParam(const std::string& name, const void* p_src, size_t size)
	{
		// 登録されていなければ早期リターン
		const auto itr_find = m_param_map.find(name);
		if (itr_find == m_param_map.end())
		{
			return false;
		}

		const auto& param = itr_find->second;
		if (param.size != size)
		{
			return false;
		}

		const auto head = m_memories[m_current_idx].data();
		memcpy(head + param.offset, p_src, param.size);
		m_memory_has_updated = true;

		return true;
	}

	bool ShaderConstantBuffer::GetParam(const std::string& name, void* p_dst, size_t size) const
	{
		// 登録されていなければ早期リターン
		const auto itr_find = m_param_map.find(name);
		if (itr_find == m_param_map.end())
		{
			return false;
		}

		const auto& param = m_param_map.at(name);
		if (static_cast<size_t>(param.size) != size)
		{
			return false;
		}

		const auto head = m_memories[m_current_idx].data();
		memcpy(p_dst, head + param.offset, param.size);

		return true;
	}

	void ShaderConstantBuffer::SetVS(ID3D11DeviceContext* p_dc, const int slot)
	{
		// If memory has update, update subresource
		m_mutex.lock();
		if (m_memory_has_updated)
		{
			UpdateSubresource();
		}
		m_mutex.unlock();

		p_dc->VSSetConstantBuffers(slot, 1, m_CB.GetAddressOf());
	}

	void ShaderConstantBuffer::SetPS(ID3D11DeviceContext* p_dc, const int slot)
	{
		// If memory has update, update subresource
		m_mutex.lock();
		if (m_memory_has_updated)
		{
			UpdateSubresource();
		}
		m_mutex.unlock();

		p_dc->PSSetConstantBuffers(slot, 1, m_CB.GetAddressOf());
	}

	void ShaderConstantBuffer::SetGS(ID3D11DeviceContext* p_dc, const int slot)
	{
		// If memory has update, update subresource
		m_mutex.lock();
		if (m_memory_has_updated)
		{
			UpdateSubresource();
		}
		m_mutex.unlock();

		p_dc->GSSetConstantBuffers(slot, 1, m_CB.GetAddressOf());
	}

	void ShaderConstantBuffer::SetDS(ID3D11DeviceContext* p_dc, const int slot)
	{
		// If memory has update, update subresource
		m_mutex.lock();
		if (m_memory_has_updated)
		{
			UpdateSubresource();
		}
		m_mutex.unlock();

		p_dc->DSSetConstantBuffers(slot, 1, m_CB.GetAddressOf());
	}

	void ShaderConstantBuffer::SetHS(ID3D11DeviceContext* p_dc, int slot)
	{
		// If memory has update, update subresource
		m_mutex.lock();
		if (m_memory_has_updated)
		{
			UpdateSubresource();
		}
		m_mutex.unlock();

		p_dc->HSSetConstantBuffers(slot, 1, m_CB.GetAddressOf());
	}

	void ShaderConstantBuffer::SetCS(ID3D11DeviceContext* p_dc, const int slot)
	{
		// If memory has update, update subresource
		m_mutex.lock();
		if (m_memory_has_updated)
		{
			UpdateSubresource();
		}
		m_mutex.unlock();

		p_dc->CSSetConstantBuffers(slot, 1, m_CB.GetAddressOf());
	}

	ShaderConstantBuffer::BufferParam::VARIABLE_TYPE ShaderConstantBuffer::GetVariableType(const D3D_SHADER_VARIABLE_TYPE var_type)
	{
		BufferParam::VARIABLE_TYPE ret_var_type = BufferParam::VARIABLE_TYPE::T_BOOL;

		switch (var_type)
		{
			case D3D_SVT_BOOL:
				ret_var_type = BufferParam::VARIABLE_TYPE::T_BOOL;
				break;
			case D3D_SVT_INT:
				ret_var_type = BufferParam::VARIABLE_TYPE::T_INT;
				break;
			case D3D_SVT_UINT:
				ret_var_type = BufferParam::VARIABLE_TYPE::T_UINT;
				break;
			case D3D_SVT_FLOAT:
				ret_var_type = BufferParam::VARIABLE_TYPE::T_FLOAT;
				break;
			case D3D_SVT_VOID:
				ret_var_type = BufferParam::VARIABLE_TYPE::T_VOID;
				break;
			default:
				assert(0 && "invalid argument variable type");
				break;
		}

		return ret_var_type;
	}

	ShaderConstantBuffer::BufferParam::VARIABLE_CLASS ShaderConstantBuffer::GetVariableClass(const D3D_SHADER_VARIABLE_CLASS class_type)
	{
		BufferParam::VARIABLE_CLASS ret_var_class = BufferParam::VARIABLE_CLASS::SCALAR;

		switch (class_type)
		{
			case D3D_SVC_SCALAR:
				ret_var_class = BufferParam::VARIABLE_CLASS::SCALAR;
				break;
			case D3D_SVC_VECTOR:
				ret_var_class = BufferParam::VARIABLE_CLASS::VECTOR;
				break;
			case D3D_SVC_MATRIX_COLUMNS:
				ret_var_class = BufferParam::VARIABLE_CLASS::MATRIX;
				break;
			case D3D_SVC_STRUCT:
				ret_var_class = BufferParam::VARIABLE_CLASS::STRUCT;
				break;
			default:
				assert(0 && "invalid argument class type");
				break;
		}

		return ret_var_class;
	}

	void ShaderConstantBuffer::UpdateSubresource(ID3D11DeviceContext* context)
	{
		ID3D11DeviceContext* _context;
		if (context == nullptr)
			_context = IGraphics::Get().DC(0, Graphics::DC_COPY_PATH::DC_CP_MAIN);
		else
			_context = context;

		_context->UpdateSubresource(m_CB.Get(), 0, nullptr, m_memories[m_current_idx].data(), 0, 0);

		m_memory_has_updated = false;
		const size_t size = m_memories[m_current_idx].size() * sizeof(uint8_t);
		memcpy(m_memories[1 - m_current_idx].data(), m_memories[m_current_idx].data(), size);
		m_current_idx = 1 - m_current_idx;
	}

#pragma endregion

}	// namespace TKGEngine