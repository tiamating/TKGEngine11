#pragma once

#include "Systems/inc/TKGEngine_Defined.h"

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
	/// Constant Buffer module
	/// </summary>
	class ConstantBuffer
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		ConstantBuffer() = default;
		virtual ~ConstantBuffer() = default;
		ConstantBuffer(const ConstantBuffer&) = delete;
		ConstantBuffer& operator=(const ConstantBuffer&) = delete;

		bool Create(int size, bool mappable);
		bool Create(int size, const void* p_data, bool mappable);
		void Release();

		ID3D11Buffer* GetBuffer() const;
		//ID3D11Buffer* const* GetAddressOfBuffer() const;

		// When calculate offset, use "offsetof" macro.
		void SetParam(const void* p_src, size_t size, size_t offset = 0);

		/// <summary>
		/// Get mapped address
		/// </summary>
		/// <param name="p_context"></param>
		/// <returns>nullptr : failed. not nullptr : succeed.</returns>
		void* Map(ID3D11DeviceContext* p_context) const;
		void Unmap(ID3D11DeviceContext* p_context) const;

		void SetVS(ID3D11DeviceContext* p_dc, int slot);
		void SetPS(ID3D11DeviceContext* p_dc, int slot);
		void SetGS(ID3D11DeviceContext* p_dc, int slot);
		void SetDS(ID3D11DeviceContext* p_dc, int slot);
		void SetHS(ID3D11DeviceContext* p_dc, int slot);
		void SetCS(ID3D11DeviceContext* p_dc, int slot);


		// ==============================================
		// public variables
		// ==============================================


	private:
		// ==============================================
		// private methods
		// ==============================================
		void UpdateSubresource(ID3D11DeviceContext* context = nullptr);


		// ==============================================
		// private variables
		// ==============================================
		bool m_mappable = false;	//!< If not mappable, update CB with UpdateSubresource
		bool m_memory_has_updated = false;
		int m_current_idx = 0;
		int m_buffer_size = 0;
		std::vector<uint8_t> m_memories[2];
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_CB;
		std::mutex m_mutex;
	};
	
	// ASSERT :  配列、同名のメンバと変数、構造体内構造体はサポートしていない
	/// <summary>
	/// Constant Buffer module created from reflection
	/// </summary>
	class ShaderConstantBuffer
	{
	private:
		struct BufferParam
		{
			enum class VARIABLE_TYPE
			{
				T_BOOL = 0,
				T_INT,
				T_UINT,
				T_FLOAT,
				T_VOID
			};
			enum class VARIABLE_CLASS
			{
				SCALAR = 0,
				VECTOR,
				MATRIX,
				STRUCT
			};
			enum class GUI_TYPE
			{
				UNSIGNED_NORM = 0,
				UNSIGNED_NORM_NO_ZERO,
				NORM,
				UNSIGNED_UNLIMIT,
				UNLIMIT,
				COLOR
			};

			int offset = 0;
			int size = 0;
			int columns = 0;
			int array_elements = 0;
			GUI_TYPE gui_type = GUI_TYPE::UNSIGNED_NORM;
			VARIABLE_TYPE var_type = VARIABLE_TYPE::T_BOOL;
			VARIABLE_CLASS var_class = VARIABLE_CLASS::SCALAR;
			std::string var_name;

		private:
			friend class cereal::access;
			template <class Archive>
			void serialize(Archive& archive, const std::uint32_t version)
			{
				//if (version > 0)
				{
					archive(
						CEREAL_NVP(offset),
						CEREAL_NVP(size),
						CEREAL_NVP(columns),
						CEREAL_NVP(array_elements),
						CEREAL_NVP(gui_type),
						CEREAL_NVP(var_type),
						CEREAL_NVP(var_class),
						CEREAL_NVP(var_name)
					);
				}
			}
		};

	public:
		// ==============================================
		// public methods
		// ==============================================
		ShaderConstantBuffer() = default;
		virtual ~ShaderConstantBuffer() = default;
		ShaderConstantBuffer(const ShaderConstantBuffer&) = delete;
		ShaderConstantBuffer& operator=(const ShaderConstantBuffer&) = delete;

#ifdef USE_IMGUI
		void OnGUI();
#endif // USE_IMGUI

		bool Create(ID3D11ShaderReflectionConstantBuffer* p_reflection, bool force_clear);
		bool Create();
		void Release(bool force_clear = true);

		ID3D11Buffer* GetBuffer() const;

		/// <summary>
		/// Checks if a parameter with a given name is included
		/// </summary>
		bool Contain(const std::string& name) const;
		bool SetParam(const std::string& name, const void* p_src, size_t size);
		template<class T>
		bool SetParam(const std::string& name, const T& value);
		bool GetParam(const std::string& name, void* p_dst, size_t size) const;
		template<class T>
		bool GetParam(const std::string& name, T& value) const;

		void SetVS(ID3D11DeviceContext* p_dc, int slot);
		void SetPS(ID3D11DeviceContext* p_dc, int slot);
		void SetGS(ID3D11DeviceContext* p_dc, int slot);
		void SetDS(ID3D11DeviceContext* p_dc, int slot);
		void SetHS(ID3D11DeviceContext* p_dc, int slot);
		void SetCS(ID3D11DeviceContext* p_dc, int slot);


		// ==============================================
		// public variables
		// ==============================================


	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(m_current_idx),
					CEREAL_NVP(m_buffer_size),
					CEREAL_NVP(m_param_map),
					CEREAL_NVP(m_param_names),
					CEREAL_NVP(m_struct_members_map),
					CEREAL_NVP(m_memories[0]),
					CEREAL_NVP(m_memories[1])
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		void OnGUIScalar(BufferParam& param, int offset = 0);
		void OnGUIVector(BufferParam& param, int offset = 0);
		void OnGUIVectorBool(BufferParam& param, int offset = 0);
		void OnGUIVectorInt(BufferParam& param, int offset = 0);
		void OnGUIVectorUint(BufferParam& param, int offset = 0);
		void OnGUIVectorFloat(BufferParam& param, int offset = 0);
		void OnGUIStruct(BufferParam& param);
		void OnRightClickPopup(BufferParam& param);
#endif // USE_IMGUI

		BufferParam::VARIABLE_TYPE GetVariableType(D3D_SHADER_VARIABLE_TYPE var_type);
		BufferParam::VARIABLE_CLASS GetVariableClass(D3D_SHADER_VARIABLE_CLASS class_type);

		void UpdateSubresource(ID3D11DeviceContext* context = nullptr);


		// ==============================================
		// private variables
		// ==============================================
#ifdef USE_IMGUI
		static constexpr float GUI_INPUT_EPSILON = 1.0e-4f;
#endif // USE_IMGUI

		bool m_memory_has_updated = false;
		int m_current_idx = 0;
		int m_buffer_size = 0;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_CB;
		// <変数名, 変数情報>
		std::unordered_map<std::string, BufferParam> m_param_map;
		// CBuffer変数名リスト
		std::vector<std::string> m_param_names;
		// <構造体インスタンス名, メンバリスト>
		std::unordered_map<std::string, std::vector<BufferParam>> m_struct_members_map;
		std::vector<uint8_t> m_memories[2];
		std::mutex m_mutex;
	};

	template<class T>
	inline bool ShaderConstantBuffer::SetParam(const std::string& name, const T& value)
	{
		return SetParam(name, &value, sizeof(T));
	}

	template<class T>
	inline bool ShaderConstantBuffer::GetParam(const std::string& name, T& value) const
	{
		return GetParam(name, &value, sizeof(T));
	}

}	// namespace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::ShaderConstantBuffer, 1)