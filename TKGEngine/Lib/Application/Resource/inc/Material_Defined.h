#pragma once

namespace TKGEngine
{
	/// <summary>
	/// Render priority
	/// </summary>
	enum RenderQueue
	{
		Background = 0,		// 最初に描画するもの
		Geometry = 1000,	// 不透明なジオメトリ
		AlphaTest = 2000,	// 透明と不透明を持つ
		Transparent = 3000,	// 半透明を持つ
		Overlay = 4000,		// 最後に描画するもの
	};

	enum class PRIMITIVE_TOPOLOGY
	{
		TriangleList = 0,
		TriangleStrip,
		PointList,
		LineList,
		LineStrip,
		ControlPoint3,

		MAX_PRIMITIVE_TOPOLOGY
	};

}// namespace TKGEngine