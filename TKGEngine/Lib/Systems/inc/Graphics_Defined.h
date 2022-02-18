#pragma once

#include <d3d11.h>

namespace TKGEngine::Graphics
{
	// ======================================
	// Variables
	// ======================================
	constexpr int g_buffer_count = 2;
	constexpr int g_dx11_alignment = 16;

	constexpr int g_max_num_render_threads = 1;
	constexpr int g_max_num_compute_threads = 1;
	constexpr int g_max_num_copy_threads = 1;

	constexpr DXGI_FORMAT g_color_format = DXGI_FORMAT_R8G8B8A8_UNORM;
	constexpr DXGI_FORMAT g_depth_format = DXGI_FORMAT_D24_UNORM_S8_UINT;


	/// ========================================================
	/// @enum	TS_TYPE
	/// @brief	GPU time stamp type
	/// 
	/// ========================================================
	enum class TS_TYPE
	{
		TS_BEGIN = 0,
		TS_COPY,
		TS_COMPUTE,
		TS_RENDER,
		TS_END,

		TS_MAX_NUM
	};

	/// ========================================================
	/// @enum	DC_RENDER_PATH
	/// @brief	deferred constext rendering pathes
	/// 
	/// ========================================================
	enum class DC_RENDER_PATH
	{
		DC_RP_SHADOW = 0,		// Render Shadow path
		DC_RP_Z_PREPATH,		// Render early Z
		DC_RP_MAIN,				// Render Main path
		DC_RP_POST_AFTER_MAIN,	// Post Effect without UI
		DC_RP_UI,				// Render UI path
		DC_RP_POST_AFTER_ALL,	// Post Effect for all

		DC_RP_MAX_NUM
	};
	/// ========================================================
	/// @enum	DC_COMPUTE_PATH
	/// @brief	deferred constext computing pathes
	/// 
	/// ========================================================
	enum class DC_COMPUTE_PATH
	{
		DC_CP_MAIN = 0,

		DC_CP_MAX_NUM
	};
	/// ========================================================
	/// @enum	DC_COPY_PATH
	/// @brief	deferred constext copy pathes
	/// 
	/// ========================================================
	enum class DC_COPY_PATH
	{
		DC_CP_MAIN = 0,

		DC_CP_MAX_NUM
	};


	// ======================================
	// Functions
	// ======================================

	/// ------------------------------------------------------------
	/// @brief	Convert to the proper depth format
	/// 
	/// @param[in] format		input format
	/// @param[in] tex_format	texture format
	/// @param[in] res_format	resource format
	/// ------------------------------------------------------------
	void ConvertDepthFormat
	(
		const DXGI_FORMAT& format,
		DXGI_FORMAT& tex_format,
		DXGI_FORMAT& res_format
	);

	/// ------------------------------------------------------------
	/// @brief	Convert to the proper color format
	/// 
	/// @param[in] format		input format
	/// @param[in] tex_format	texture format
	/// ------------------------------------------------------------
	void ConvertColorFormat
	(
		const DXGI_FORMAT& format,
		DXGI_FORMAT& tex_format
	);

}// namespace TKGEngine::Graphics