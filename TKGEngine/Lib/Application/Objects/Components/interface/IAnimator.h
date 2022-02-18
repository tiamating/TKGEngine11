#pragma once

#include "Application/Resource/inc/VertexElement.h"
#include "Utility/inc/myfunc_vector.h"

namespace TKGEngine
{
	/// <summary>
	/// アニメーションの実行をしない状態の選択
	/// </summary>
	enum class CullingMode
	{
		AlwaysAnimation,		// No culling animation.
		CullUpdateTransform,	// When out of screen, no update transform.
		CullCompletely,			// When out of screen, cull animation.
	};

	/// <summary>
	/// アニメーションの更新に使用するタイムスケール
	/// </summary>
	enum class UpdateMode
	{
		Normal,			// use time scale.
		UnscaledTime,	// use unscaled time.
	};


	/// <summary>
	/// Animator interface.
	/// </summary>
	class IAnimator
	{
	public:
		virtual CullingMode GetCullingMode() const = 0;

		virtual void ApplyAnimationTransform() = 0;
		virtual void UpdateAnimationMatrix() = 0;

		virtual bool IsActiveAnimator() = 0;
	};
}// namespace TKGEngine