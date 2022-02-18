#pragma once


struct ID3D11DeviceContext;

namespace TKGEngine
{

	/// ===========================
	/// @struct	ResizeEventArgs
	/// ===========================
	struct ResizeEventArgs
	{
		int width = 0;
		int height = 0;
	};

	/// ===========================
	/// @struct	FrameEventArgs
	/// ===========================
	struct FrameEventArgs
	{
		int frame_idx = 0;
		float delta_time = 0.f;
		float unscaled_delta_time = 0.f;
		double total_time = 0.0;
		double unscaled_total_time = 0.0;
		bool is_stop_draw = false;
	};

	/// ===========================
	/// @struct	DropEventArgs
	/// ===========================
	struct DropEventArgs
	{
		wchar_t** files = nullptr;
		unsigned file_count = 0;
	};



}	// namespace TKGEngine