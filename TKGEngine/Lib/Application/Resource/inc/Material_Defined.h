#pragma once

namespace TKGEngine
{
	/// <summary>
	/// Render priority
	/// </summary>
	enum RenderQueue
	{
		Background = 0,		// �ŏ��ɕ`�悷�����
		Geometry = 1000,	// �s�����ȃW�I���g��
		AlphaTest = 2000,	// �����ƕs����������
		Transparent = 3000,	// ������������
		Overlay = 4000,		// �Ō�ɕ`�悷�����
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