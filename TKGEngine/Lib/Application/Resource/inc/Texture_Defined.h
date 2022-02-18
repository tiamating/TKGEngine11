#pragma once

#include <d3d11.h>

namespace TKGEngine
{
	enum class TEXTURE_OPTION
	{
		TEXTURE_OPTION_NONE = 0,
		TEXTURE_OPTION_CUBEMAP = 0x1 << 0,
		TEXTURE_OPTION_VOLUME = 0x1 << 1,
	};
	enum class TEXTURE_DIMENSION
	{
		TEXTURE_DIMENSION_TEXTURE1D = 2,
		TEXTURE_DIMENSION_TEXTURE2D = 3,
		TEXTURE_DIMENSION_TEXTURE3D = 4,
	};
	struct TEX_DESC
	{
		int width = 0;
		int height = 0;	// Should be 1 for 1D textures
		int depth = 0;	// Should be 1 for 1D or 2D textures
		int array_size = 0;	// For cubemap, this is a multiple of 6
		int mip_levels = 0;	// If 0, create max mip level.
		int row_pitch = 0;	// Number of bytes in the line of data
		int slice_pitch = 0;	// Number of bytes in the 2D
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		TEXTURE_OPTION option = TEXTURE_OPTION::TEXTURE_OPTION_NONE;
		TEXTURE_DIMENSION dimension = TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE2D;

		void Clear()
		{
			width = 0;
			height = 0;
			depth = 0;
			array_size = 0;
			mip_levels = 0;
			row_pitch = 0;
			slice_pitch = 0;
			format = DXGI_FORMAT_UNKNOWN;
			option = TEXTURE_OPTION::TEXTURE_OPTION_NONE;
			dimension = TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE2D;
		}
	};


}// namespace TKGEngine