#pragma once

namespace TKGEngine
{
	// ========================================
	//				Tag
	// ========================================
	enum class Tag
	{
		Default = 0,

		Wall1m,
		LeftLimit,
		RightLimit,

		MAX_TAG_NUM
	};

	// ========================================
	//				Layer
	// ========================================
	enum class Layer
	{
		Default = 0,
		Raycast = 1,

		Player,
		Enemy,

		Stage,
		ClimbField,
		CoverField,
		CoverLimitField,

		PlayerHit,
		EnemyHit,

		Goal,


		//---------------
		// “ÆŽ©Layer‚Í29‚Ü‚Å
		// --------------
		MAX_USER_LAYER_NUM,

		Trigger = 30,
		Static = 31,

		MAX_LAYER_NUM,
		AllFilter = ~0// &~((1 << static_cast<int>(Trigger)) | (1 << static_cast<int>(Static)))
	};

	constexpr int INT_BIT_COUNT = 32;


}// namespace TKGEngine