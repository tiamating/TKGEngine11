#pragma once

#include "Shader_Defined.h"

#include <string>
#include <Windows.h>

namespace TKGEngine
{
	enum class VERTEX_ELEMENT_TYPE
	{
		// Vertex Data
		POSITION = 0,
		NORMAL,
		TANGENT,
		BONES,
		WEIGHTS,
		COLOR,
		TEXCOORD0,
		TEXCOORD1,
		TEXCOORD2,
		TEXCOORD3,
		TEXCOORD4,
		TEXCOORD5,
		TEXCOORD6,
		TEXCOORD7,	// Vertex DataÇÃ ç≈ëÂ - 1 ÇåìÇÀÇƒÇ¢ÇÈ
		// ~Vertex Data

		// Instance Data
		TRANSFORM_MATRIX,
		TEXCOORD_TRANSFORM,
		INSTANCE_COLOR,

		MAIN_INSTANCE = TEXCOORD_TRANSFORM,
		UI_INSTANCE = INSTANCE_COLOR,
		// ~Instance Data

		MAX_TYPE_NUM
	};

	namespace VERTEX_ELEMENT
	{
		static constexpr LPCSTR SEMANTIC_NAME[static_cast<int>(VERTEX_ELEMENT_TYPE::MAX_TYPE_NUM)] =
		{ 
			"POSITION",
			"NORMAL",
			"TANGENT",
			"BONES",
			"WEIGHTS",
			"COLOR",
			"TEXCOORD0",
			"TEXCOORD1",
			"TEXCOORD2",
			"TEXCOORD3",
			"TEXCOORD4",
			"TEXCOORD5",
			"TEXCOORD6",
			"TEXCOORD7",

			"TRANSFORM_MATRIX",
			"TEXCOORD_TRANSFORM",
			"INSTANCE_COLOR"
		};

		static constexpr int ELEMENT_SIZE[static_cast<int>(VERTEX_ELEMENT_TYPE::MAX_TYPE_NUM)] =
		{
			sizeof(float) * 3,		// POSITION
			sizeof(float) * 3,		// NORMAL
			sizeof(float) * 3,		// TANGENT
			sizeof(unsigned) * 4,	// BONES
			sizeof(float) * 4,		// WEIGHTS
			sizeof(float) * 4,		// COLOR
			sizeof(float) * 2,		// TEXCOORD0
			sizeof(float) * 2,		// TEXCOORD1
			sizeof(float) * 2,		// TEXCOORD2
			sizeof(float) * 2,		// TEXCOORD3
			sizeof(float) * 2,		// TEXCOORD4
			sizeof(float) * 2,		// TEXCOORD5
			sizeof(float) * 2,		// TEXCOORD6
			sizeof(float) * 2,		// TEXCOORD7

			//sizeof(float) * 4 * 4,	// MATRIX
			//sizeof(float) * 4,		// VECTOR4
			//sizeof(float) * 4,		// VECTOR4
			0,
			sizeof(MainInstance),
			sizeof(UIInstance)
		};

	}// namespace VERTEX_ELEMENT
}// namespace TKGEngine