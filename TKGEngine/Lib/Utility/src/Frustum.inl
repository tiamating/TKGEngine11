#pragma once
#include "../inc/Frustum.h"

namespace TKGEngine
{
	// ----------------------------------------------------------------
	// 
	// Frustum class declaration
	// 
	// ----------------------------------------------------------------
	inline Frustum::Frustum()
		: m_frustum()
	{
		/* nothing */
	}

	inline Frustum::Frustum(const MATRIX& projection)
		: m_frustum(
			projection
#ifdef AXIS_RH
			, true
#endif
		)
	{
		/* nothing */
	}

	inline Frustum::Frustum(const Frustum& f)
	{
		this->m_frustum = f.m_frustum;
	}

	inline Frustum::Frustum(Frustum&& f)
	{
		this->m_frustum = f.m_frustum;
	}

	inline Frustum& Frustum::operator=(const Frustum& f)
	{
		this->m_frustum = f.m_frustum;
		return *this;
	}

	inline Frustum& Frustum::operator=(Frustum&& f)
	{
		this->m_frustum = f.m_frustum;
		return *this;
	}

	inline void Frustum::GetCorners(VECTOR3(&corners)[8]) const
	{
		m_frustum.GetCorners(corners);
	}

	inline DirectX::BoundingBox Frustum::GetAABB() const
	{
		VECTOR3 corners[8];
		m_frustum.GetCorners(corners);

		VECTOR3 min(FLT_MAX);
		VECTOR3 max(-FLT_MAX);

		for (int i = 0; i < 8; ++i)
		{
			const auto& c = corners[i];
			min = MyMath::Min(min, c);
			max = MyMath::Max(max, c);
		}

		DirectX::BoundingBox ret;
		DirectX::BoundingBox::CreateFromPoints(ret, min, max);
		return ret;
	}

	inline void Frustum::GetPlanes(VECTOR3(&planes)[6]) const
	{
		GetPlanes(planes[0], planes[1], planes[2], planes[3], planes[4], planes[5]);
	}

	inline void Frustum::GetPlanes(VECTOR3& near_plane, VECTOR3& far_plane, VECTOR3& right_plane, VECTOR3& left_plane, VECTOR3& top_plane, VECTOR3& bottom_plane) const
	{
		DirectX::XMVECTOR _plane0, _plane1, _plane2, _plane3, _plane4, _plane5;
		m_frustum.GetPlanes(&_plane0, &_plane1, &_plane2, &_plane3, &_plane4, &_plane5);
		near_plane = _plane0;
		far_plane = _plane1;
		right_plane = _plane2;
		left_plane = _plane3;
		top_plane = _plane4;
		bottom_plane = _plane5;
	}

	inline void Frustum::Create(const VECTOR3& pos, const Quaternion& quat, const MATRIX& projection)
	{
		CreateFromMatrix(projection);
		m_frustum.Origin = pos;
		m_frustum.Orientation = quat;
	}

	inline void Frustum::CreateFromMatrix(const MATRIX& projection)
	{
		DirectX::BoundingFrustum::CreateFromMatrix(
			m_frustum,
			projection
#ifdef AXIS_RH
			, true
#endif
		);
	}

	inline void Frustum::Transform(const MATRIX& matrix)
	{
		m_frustum.Transform(m_frustum, matrix);
	}

	inline void Frustum::Transform(const VECTOR3& translation, const Quaternion& quat, const VECTOR3& scale)
	{
		m_frustum.Transform(m_frustum, MATRIX::CreateWorld(translation, quat, scale));
	}

	inline 	bool Frustum::Contains(const VECTOR3& point) const
	{
		return m_frustum.Contains(point) != DirectX::ContainmentType::DISJOINT;
	}

	inline bool Frustum::Contains(const VECTOR3& p0, const VECTOR3& p1, const VECTOR3& p2) const
	{
		return m_frustum.Contains(p0, p1, p2) != DirectX::ContainmentType::DISJOINT;
	}

	inline bool Frustum::Contains(const DirectX::BoundingSphere& sp) const
	{
		return m_frustum.Contains(sp) != DirectX::ContainmentType::DISJOINT;
	}

	inline bool Frustum::Contains(const DirectX::BoundingBox& box) const
	{
		return m_frustum.Contains(box) != DirectX::ContainmentType::DISJOINT;
	}

	inline bool Frustum::Contains(const DirectX::BoundingOrientedBox& box) const
	{
		return m_frustum.Contains(box) != DirectX::ContainmentType::DISJOINT;
	}

	inline bool Frustum::Intersects(const VECTOR3& origin, const VECTOR3& direction, float dist) const
	{
		return m_frustum.Intersects(origin, direction, dist);
	}

}// namespace TKGEngine