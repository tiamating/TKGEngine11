#pragma once

#include "bounds.h"
#include "Frustum.h"

namespace TKGEngine::MyFunc
{
	// =============================================================
	// �X�C�[�v�e�X�g
	// =============================================================
	// AABB��SweepDir�ɕ��s�ړ������Ƃ���Frustum��AABB�Ƃ̌�������
	bool SweepIntersectionTest(const Bounds& object_aabb, const Bounds& frustum_aabb, const VECTOR3& sweep_dir);
	// AABB��SweepDir�ɕ��s�ړ������Ƃ���Frustum�Ƃ̌�������
	bool SweepIntersectionTest(const Bounds& object_aabb, const Frustum& frustum, const VECTOR3& sweep_dir);
	bool SweepIntersectionTest(const Bounds& object_aabb, const VECTOR3(&frustum_points)[8], const VECTOR3& sweep_dir);


}