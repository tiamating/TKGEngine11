#pragma once

#include "bounds.h"
#include "Frustum.h"

namespace TKGEngine::MyFunc
{
	// =============================================================
	// スイープテスト
	// =============================================================
	// AABBをSweepDirに平行移動したときのFrustumのAABBとの交差判定
	bool SweepIntersectionTest(const Bounds& object_aabb, const Bounds& frustum_aabb, const VECTOR3& sweep_dir);
	// AABBをSweepDirに平行移動したときのFrustumとの交差判定
	bool SweepIntersectionTest(const Bounds& object_aabb, const Frustum& frustum, const VECTOR3& sweep_dir);
	bool SweepIntersectionTest(const Bounds& object_aabb, const VECTOR3(&frustum_points)[8], const VECTOR3& sweep_dir);


}