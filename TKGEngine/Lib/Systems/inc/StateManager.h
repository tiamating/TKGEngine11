#pragma once

#include "Application/Resource/inc/Shader_Defined.h"
#include "Application/inc/StencilMaskList.h"

#include <wrl.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;
struct ID3D11BlendState;
struct ID3D11SamplerState;


namespace TKGEngine
{
	class StateManager
	{
	public:
		/// <summary>
		/// Depth Stencil state type
		/// </summary>
		enum class DS
		{
			BackGround = 0,
			Geometry,
			NoTest,		// DepthEnable = false
			WriteOnly,	// DepthFunc = COMPARISON_ALWAYS


			MAX_DEPTHSTENCIL_STATE
		};

		/// <summary>
		/// Depth Type. Write or NoWrite Depth
		/// </summary>
		enum class DSType
		{
			WriteDepth = 0,
			NoWriteDepth,

			MAX_DS_TYPE
		};

		/// <summary>
		/// Rasterizer state type.
		/// Select FillMode and CullMode.
		/// </summary>
		enum class RS
		{
			// https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ns-d3d11-d3d11_rasterizer_desc
			FillFront = 0,
			FillBack,
			FillNone,

			WireFront,
			WireBack,
			WireNone,

			// AntialiasedLineEnable = true
			WireFrontSmooth,
			WireBackSmooth,
			WireNoneSmooth,

			// MultisampleEnable = true
			FillFrontMS,
			FillBackMS,
			FillNoneMS,

			// DepthClipEnable = false
			FillFrontNoClip,
			FillBackNoClip,
			FillNoneNoClip,

			MAX_RATERIZER_STATE
		};

		/// <summary>
		/// Blend state type
		/// </summary>
		enum class BS
		{
			Opaque = 0,
			Transparent,
			Addition,
			Multiply,
			Subtract,
			Lighten,
			Darken,
			Inverse,
			Decal,	// ステッカー
			Disable,
			AlphaToCoverage,

			DeferredLight,
			EnvironmentalLight,

			MAX_BLEND_STATE
		};

		// ==============================================
		// public methods
		// ==============================================
		static bool InitManager(ID3D11Device* p_device, ID3D11DeviceContext* p_context);

		static void SetState(ID3D11DeviceContext* p_context, DS type, Stencil::Mask stencil_mask, bool write_depth);
		static void SetState(ID3D11DeviceContext* p_context, RS type);
		static void SetState(ID3D11DeviceContext* p_context, BS type);
		static void SetAllSampler(ID3D11DeviceContext* p_context);


	private:
		// -----------------------------------------
		// Each State Manager
		// -----------------------------------------
		// -----------------
		// Depth
		// -----------------
		class DepthManager
		{
		public:
			bool CreateState(ID3D11Device* p_device);
			void SetState(ID3D11DeviceContext* p_context, DS type, Stencil::Mask mask, bool write_depth);

		private:
			Microsoft::WRL::ComPtr<ID3D11DepthStencilState>
				depth_types[static_cast<int>(DS::MAX_DEPTHSTENCIL_STATE)][static_cast<int>(DSType::MAX_DS_TYPE)];
		};

		// -----------------
		// Rasterizer
		// -----------------
		class RasterizerManager
		{
		public:
			bool CreateState(ID3D11Device* p_device);
			void SetState(ID3D11DeviceContext* p_context, RS type);

		private:
			Microsoft::WRL::ComPtr<ID3D11RasterizerState>
				rasterizer_types[static_cast<int>(RS::MAX_RATERIZER_STATE)];
		};
		// -----------------
		// Blend
		// -----------------
		class BlendManager
		{
		public:
			bool CreateState(ID3D11Device* p_device);
			void SetState(ID3D11DeviceContext* p_context, BS type);

		private:
			Microsoft::WRL::ComPtr<ID3D11BlendState>
				blend_types[static_cast<int>(BS::MAX_BLEND_STATE)];
		};
		// -----------------
		// Sampler
		// -----------------
		class SamplerManager
		{
		public:
			bool CreateState(ID3D11Device* p_device);
			void SetState(ID3D11DeviceContext* p_context, int slot);

		private:
			Microsoft::WRL::ComPtr<ID3D11SamplerState>
				sampler_types[SAMPLSLOT_CMP_DEPTH + 1];
		};


		// ==============================================
		// private variables
		// ==============================================
		static DepthManager m_depth_manager;
		static RasterizerManager m_rasterizer_manager;
		static BlendManager m_blend_manager;
		static SamplerManager m_sampler_manager;

	};

}// namespace TKGEngine