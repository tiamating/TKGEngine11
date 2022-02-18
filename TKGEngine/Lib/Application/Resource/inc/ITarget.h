#pragma once

#include "Systems/inc/TKGEngine_Defined.h"
#include "../../Resource/inc/Shader_Defined.h"

#include <d3d11.h>
#include <wrl.h>
#include <memory>

namespace TKGEngine
{
#pragma region Target 2D
	/////////////////////////////////////
	/// @struct	TargetDesc
	/// @brief	Color and Depth Target Description
	///
	/////////////////////////////////////
	struct TargetDesc
	{
		UINT width = 0;
		UINT height = 0;
		UINT array_size = 0;
		UINT mip_levels = 1;
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		DXGI_SAMPLE_DESC sample_desc = { 1,0 };
		UINT CPU_access_flags = 0;

		TargetDesc() = default;
		TargetDesc(UINT w, UINT h, DXGI_FORMAT format_, UINT msaa_count, UINT msaa_quality)
			: width(w), height(h), format(format_)
			, array_size(1), mip_levels(1)
		{
			sample_desc.Count = msaa_count;
			sample_desc.Quality = msaa_quality;
		}
		void Reset()
		{
			width = 0; height = 0;
			array_size = 0; mip_levels = 1;
			format = DXGI_FORMAT_UNKNOWN;
			sample_desc.Count = 1; sample_desc.Quality = 0;
			CPU_access_flags = 0;
		}
	};

	/////////////////////////////////////
	/// @class	IColorTarget
	/// @brief	Color Target interface
	///
	/////////////////////////////////////
	class IColorTarget
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static std::unique_ptr<IColorTarget> CreateInterface();

		IColorTarget() = default;
		virtual ~IColorTarget() = default;
		IColorTarget(const IColorTarget&) = delete;
		IColorTarget(IColorTarget&&) = default;
		IColorTarget& operator=(const IColorTarget&) = delete;

#ifdef USE_IMGUI
		virtual void OnGUI() = 0;
#endif // USE_IMGUI

		/// ------------------------------------------
		/// @brief	Create color target from back buffer
		/// @param[in]	p_device	Device pointer
		/// @param[in]	p_swapchain	SwapChain pointer
		///
		/// @retval true	successful create color target
		/// @retval false	failed create color target
		/// ------------------------------------------
		virtual bool Create(ID3D11Device* p_device, IDXGISwapChain* p_swapchain) = 0;

		/// ------------------------------------------
		/// @brief	Create color target
		/// @param[in]	p_device	Device pointer
		/// @param[in]	desc		Target description
		/// @param[in]	create_rtv	Create RTV
		/// @param[in]	create_uav	Create UAV
		/// @param[in]	desc		Target description
		///
		/// @retval true	successful create color target
		/// @retval false	failed create color target
		/// ------------------------------------------
		virtual bool Create(ID3D11Device* p_device, const TargetDesc& desc, bool create_rtv, bool create_uav) = 0;

		/// ------------------------------------------
		/// @brief	Resize color target
		/// @param[in]	p_device	Device pointer
		/// @param[in]	p_swapchain	SwapChain pointer
		///
		/// @retval true	successful resize color target
		/// @retval false	failed resize color target
		/// ------------------------------------------
		virtual bool Resize(ID3D11Device* p_device, IDXGISwapChain* p_swapchain) = 0;

		/// ------------------------------------------
		/// @brief	Resize color target
		/// @param[in]	p_device	Device pointer
		/// @param[in]	width		new target width
		/// @param[in]	height		new target height
		///
		/// @retval true	successful resize color target
		/// @retval false	failed resize color target
		/// ------------------------------------------
		virtual bool Resize(ID3D11Device* p_device, UINT width, UINT height) = 0;

		/// ------------------------------------------
		/// @brief	Release color target
		/// ------------------------------------------
		virtual void Release() = 0;

		/// ------------------------------------------
		/// @brief	OMSet() color target
		/// ------------------------------------------
		virtual void SetRTV(ID3D11DeviceContext* p_context) = 0;
		virtual void RemoveRTV(ID3D11DeviceContext* p_context) = 0;

		virtual void SetSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility) = 0;

		virtual void SetUAV(ID3D11DeviceContext* p_context, int slot = 0) = 0;
		virtual void RemoveUAV(ID3D11DeviceContext* p_context, int slot = 0) = 0;

		/// ------------------------------------------
		/// @brief	clear target
		/// ------------------------------------------
		virtual void Clear(ID3D11DeviceContext* p_context) = 0;
		virtual void Clear(ID3D11DeviceContext* p_context, const float(&color)[4]) = 0;

		/// ------------------------------------------
		/// @brief	Get Texture resource
		/// 
		/// @return	return Texture resource
		/// ------------------------------------------
		virtual ID3D11Texture2D* const GetResource() const = 0;

		/// ------------------------------------------
		/// @brief	Get Shader Resource View
		/// 
		/// @return return SRV
		/// ------------------------------------------
		virtual ID3D11ShaderResourceView* const GetSRV() const = 0;
		virtual ID3D11ShaderResourceView* const* GetAddressOfSRV() const = 0;

		/// ------------------------------------------
		/// @brief	Get Render Target View
		/// 
		/// @return return RTV
		/// ------------------------------------------
		virtual ID3D11RenderTargetView* const GetRTV() const = 0;

		/// ------------------------------------------
		/// @brief	Get Unordered Access View
		/// 
		/// @return return UAV
		/// ------------------------------------------
		virtual ID3D11UnorderedAccessView* const GetUAV() const = 0;
		virtual ID3D11UnorderedAccessView* const* GetAddressOfUAV() const = 0;

		/// ------------------------------------------
		/// @brief	Get Target description
		/// 
		/// @return return Target description
		/// ------------------------------------------
		virtual TargetDesc GetDesc() const = 0;
	};

	/////////////////////////////////////
	/// @class	IDepthTarget
	/// @brief	Depth Target interface
	///
	/////////////////////////////////////
	class IDepthTarget
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static std::unique_ptr<IDepthTarget> CreateInterface();

		IDepthTarget() = default;
		virtual ~IDepthTarget() = default;
		IDepthTarget(const IDepthTarget&) = delete;
		IDepthTarget(IDepthTarget&&) = default;
		IDepthTarget& operator=(const IDepthTarget&) = delete;

#ifdef USE_IMGUI
		virtual void OnGUI() = 0;
#endif // USE_IMGUI

		/// ------------------------------------------
		/// @brief	Create depth target
		/// @param[in]	p_device	Device pointer
		/// @param[in]	desc		Target description
		/// @param[in]	create_uav	Create UAV
		///
		/// @retval true	successful create depth target
		/// @retval false	failed create depth target
		/// ------------------------------------------
		virtual bool Create(ID3D11Device* p_device, const TargetDesc& desc, bool create_uav) = 0;

		/// ------------------------------------------
		/// @brief	Resize depth target
		/// @param[in]	p_device	Device pointer
		/// @param[in]	width		new target width
		/// @param[in]	height		new target height
		///
		/// @retval true	successful resize depth target
		/// @retval false	failed resize depth target
		/// ------------------------------------------
		virtual bool Resize(ID3D11Device* p_device, UINT width, UINT height) = 0;

		/// ------------------------------------------
		/// @brief	Release depth target
		/// ------------------------------------------
		virtual void Release() = 0;

		/// ------------------------------------------
		/// @brief	OMSet() depth target
		/// ------------------------------------------
		virtual void SetRTV(ID3D11DeviceContext* p_context) = 0;
		virtual void RemoveRTV(ID3D11DeviceContext* p_context) = 0;

		virtual void SetSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility) = 0;

		/// ------------------------------------------
		/// @brief	clear target
		/// ------------------------------------------
		virtual void Clear(ID3D11DeviceContext* p_context) = 0;

		/// ------------------------------------------
		/// @brief	Get Texture resource
		/// 
		/// @return	return Texture resource
		/// ------------------------------------------
		virtual ID3D11Texture2D* const GetResource() const = 0;

		/// ------------------------------------------
		/// @brief	Get Shader Resource View
		/// 
		/// @return return SRV
		/// ------------------------------------------
		virtual ID3D11ShaderResourceView* const GetSRV() const = 0;
		virtual ID3D11ShaderResourceView* const* GetAddressOfSRV() const = 0;

		/// ------------------------------------------
		/// @brief	Get Depth Stencil View
		/// 
		/// @return return DSV
		/// ------------------------------------------
		virtual ID3D11DepthStencilView* const GetDSV() const = 0;

		/// ------------------------------------------
		/// @brief	Get Target description
		/// 
		/// @return return Target description
		/// ------------------------------------------
		virtual TargetDesc GetDesc() const = 0;
	};

#pragma endregion

#pragma region Target Cube
	/////////////////////////////////////
	/// @struct	CubeDesc
	/// @brief	Cube Target Description
	///
	/////////////////////////////////////
	struct CubeDesc
	{
		UINT cube_size = 0;
		UINT mip_levels = 1;
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		UINT CPU_access_flags = 0;

		CubeDesc() = default;
		CubeDesc(UINT size, DXGI_FORMAT format_, UINT msaa_count, UINT msaa_quality)
			: cube_size(size), format(format_), mip_levels(1)
		{/* nothing */}
		void Reset()
		{
			cube_size = 0;
			mip_levels = 1;
			format = DXGI_FORMAT_UNKNOWN;
			CPU_access_flags = 0;
		}
	};

	/////////////////////////////////////
	/// @enum	CUBEMAP_FACE
	/// @brief	CubeMap face type
	///
	/////////////////////////////////////
	//enum class CUBEMAP_FACE
	//{
	//	POSITIVE_X = 0,
	//	NEGATIVE_X,
	//	POSITIVE_Y,
	//	NEGATIVE_Y,
	//	POSITIVE_Z,
	//	NEGATIVE_Z,
	//};

	/////////////////////////////////////
	/// @class	IColorTargetCube
	/// @brief	Cube Color Target interface
	///
	/////////////////////////////////////
	class IColorTargetCube
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static std::unique_ptr<IColorTargetCube> CreateInterface();

		IColorTargetCube() = default;
		virtual ~IColorTargetCube() = default;
		IColorTargetCube(const IColorTargetCube&) = delete;
		IColorTargetCube(IColorTargetCube&&) = default;
		IColorTargetCube& operator=(const IColorTargetCube&) = delete;


		/// ------------------------------------------
		/// @brief	Create color target
		/// @param[in]	p_device	Device pointer
		/// @param[in]	desc		Cube Target description
		///
		/// @retval true	successful create color target
		/// @retval false	failed create color target
		/// ------------------------------------------
		virtual bool Create(ID3D11Device* p_device, const CubeDesc& desc) = 0;

		/// ------------------------------------------
		/// @brief	Resize color target
		/// @param[in]	p_device	Device pointer
		/// @param[in]	size		new target size
		///
		/// @retval true	successful resize color target
		/// @retval false	failed resize color target
		/// ------------------------------------------
		virtual bool Resize(ID3D11Device* p_device, UINT size) = 0;

		/// ------------------------------------------
		/// @brief	Release color target
		/// ------------------------------------------
		virtual void Release() = 0;

		/// ------------------------------------------
		/// @brief	OMSet() color target
		/// ------------------------------------------
		virtual void SetRTV(ID3D11DeviceContext* p_context) = 0;
		virtual void RemoveRTV(ID3D11DeviceContext* p_context) = 0;

		virtual void SetSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility) = 0;

		/// ------------------------------------------
		/// @brief	clear target
		/// ------------------------------------------
		virtual void Clear(ID3D11DeviceContext* p_context, const float(&color)[4]) = 0;

		/// ------------------------------------------
		/// @brief	Get Texture resource
		/// 
		/// @return	return Texture resource
		/// ------------------------------------------
		virtual ID3D11Texture2D* const GetResource() const = 0;

		/// ------------------------------------------
		/// @brief	Get Shader Resource View
		/// 
		/// @return return SRV
		/// ------------------------------------------
		virtual ID3D11ShaderResourceView* const GetSRV() const = 0;

		/// ------------------------------------------
		/// @brief	Get Render Target View
		/// @param[in]	face_type	render target face type
		/// 
		/// @return return RTV
		/// ------------------------------------------
		virtual ID3D11RenderTargetView* const* GetAddressOfRTV() const = 0;

		/// ------------------------------------------
		/// @brief	Get Cube target description
		/// 
		/// @return return Cube target description
		/// ------------------------------------------
		virtual CubeDesc GetDesc() const = 0;
	};

	/////////////////////////////////////
	/// @class	IDepthTargetCube
	/// @brief	Cube Depth Target interface
	///
	/////////////////////////////////////
	class IDepthTargetCube
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static std::unique_ptr<IDepthTargetCube> CreateInterface();

		IDepthTargetCube() = default;
		virtual ~IDepthTargetCube() = default;
		IDepthTargetCube(const IDepthTargetCube&) = delete;
		IDepthTargetCube(IDepthTargetCube&&) = default;
		IDepthTargetCube& operator=(const IDepthTargetCube&) = delete;

		/// ------------------------------------------
		/// @brief	Create depth target
		/// @param[in]	p_device	Device pointer
		/// @param[in]	desc		Cube Target description
		///
		/// @retval true	successful create depth target
		/// @retval false	failed create depth target
		/// ------------------------------------------
		virtual bool Create(ID3D11Device* p_device, const CubeDesc& desc) = 0;

		/// ------------------------------------------
		/// @brief	Resize depth target
		/// @param[in]	p_device	Device pointer
		/// @param[in]	size		Texture size
		///
		/// @retval true	successful resize depth target
		/// @retval false	failed resize depth target
		/// ------------------------------------------
		virtual bool Resize(ID3D11Device* p_device, UINT size) = 0;

		/// ------------------------------------------
		/// @brief	Release depth target
		/// ------------------------------------------
		virtual void Release() = 0;

		/// ------------------------------------------
		/// @brief	OMSet() depth target
		/// ------------------------------------------
		virtual void SetRTV(ID3D11DeviceContext* p_context) = 0;
		virtual void RemoveRTV(ID3D11DeviceContext* p_context) = 0;

		virtual void SetSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility) = 0;

		/// ------------------------------------------
		/// @brief	clear target
		/// ------------------------------------------
		virtual void Clear(ID3D11DeviceContext* p_context) = 0;

		/// ------------------------------------------
		/// @brief	Get Texture resource
		/// 
		/// @return	return Texture resource
		/// ------------------------------------------
		virtual ID3D11Texture2D* const GetResource() const = 0;

		/// ------------------------------------------
		/// @brief	Get Shader Resource View
		/// 
		/// @return return SRV
		/// ------------------------------------------
		virtual ID3D11ShaderResourceView* const GetSRV() const = 0;

		/// ------------------------------------------
		/// @brief	Get Depth Stencil View
		/// @param[in]	face_type	render target face type
		/// 
		/// @return return DSV
		/// ------------------------------------------
		virtual ID3D11DepthStencilView* const* GetDSV() const = 0;

		/// ------------------------------------------
		/// @brief	Get Cube Target description
		/// 
		/// @return return Cube Target description
		/// ------------------------------------------
		virtual CubeDesc GetDesc() const = 0;
	};

#pragma endregion
}