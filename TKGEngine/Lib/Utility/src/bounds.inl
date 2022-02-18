#pragma once
#include "../inc/bounds.h"

namespace TKGEngine
{
	// ----------------------------------------------------------------
	// 
	// Bounds class declaration
	// 
	// ----------------------------------------------------------------
	inline Bounds::Bounds()
		: m_aabb()
	{
		/* nothing */
	}

	inline Bounds::Bounds(const VECTOR3& center, const VECTOR3& extents)
		: m_aabb(center, extents)
	{
		/* nothing */
	}

	inline Bounds::Bounds(const Bounds& b)
		: m_aabb(b.m_aabb)
	{
		/* nothing */
	}

	inline Bounds::Bounds(const DirectX::BoundingBox& b)
		: m_aabb(b)
	{
		/* nothing */
	}

	inline Bounds::Bounds(Bounds&& b)
		: m_aabb(b.m_aabb)
	{
		/* nothing */
	}

	inline Bounds& Bounds::operator=(const Bounds& b)
	{
		m_aabb = b.m_aabb;
		return *this;
	}

	inline Bounds& Bounds::operator=(const DirectX::BoundingBox& b)
	{
		m_aabb = b;
		return *this;
	}

	inline Bounds::operator DirectX::BoundingBox() const
	{
		return m_aabb;
	}

	inline void Bounds::Reset()
	{
		m_aabb = DirectX::BoundingBox();
	}

	inline void Bounds::Union(const Bounds& bounds)
	{
		const VECTOR3 min_1 = m_aabb.Center - m_aabb.Extents;
		const VECTOR3 max_1 = m_aabb.Center + m_aabb.Extents;
		const VECTOR3 min_2 = bounds.m_aabb.Center - bounds.m_aabb.Extents;
		const VECTOR3 max_2 = bounds.m_aabb.Center + bounds.m_aabb.Extents;
		const VECTOR3 min = MyMath::Min(min_1, min_2);
		const VECTOR3 max = MyMath::Max(max_1, max_2);
		CreateFromPoints(min, max);
	}

	inline VECTOR3 Bounds::GetCenter() const
	{
		return m_aabb.Center;
	}

	inline VECTOR3 Bounds::GetExtents() const
	{
		return m_aabb.Extents;
	}

	inline void Bounds::SetCenter(const VECTOR3& center)
	{
		m_aabb.Center = center;
	}

	inline void Bounds::SetExtents(const VECTOR3& extents)
	{
		m_aabb.Extents = extents;
	}

	inline void Bounds::CreateMerged(const DirectX::BoundingBox& b1, const DirectX::BoundingBox& b2)
	{
		DirectX::BoundingBox::CreateMerged(m_aabb, b1, b2);
	}

	inline void Bounds::CreateFromSphere(const DirectX::BoundingSphere& sp)
	{
		DirectX::BoundingBox::CreateFromSphere(m_aabb, sp);
	}

	inline void Bounds::CreateFromPoints(const VECTOR3& p1, const VECTOR3& p2)
	{
		DirectX::BoundingBox::CreateFromPoints(m_aabb, p1, p2);
	}

	inline Bounds Bounds::Transform(const MATRIX& mat) const
	{
		Bounds ret_val;
		m_aabb.Transform(ret_val.m_aabb, mat);
		return ret_val;
	}

	inline void Bounds::Transform(const VECTOR3& translation, const Quaternion& quat, const VECTOR3& scale)
	{
		m_aabb.Transform(m_aabb, MATRIX::CreateWorld(translation, quat, scale));
	}

	inline bool Bounds::Contains(const VECTOR3& point) const
	{
		return m_aabb.Contains(point) != DirectX::ContainmentType::DISJOINT;
	}

	inline bool Bounds::Contains(const VECTOR3& p0, const VECTOR3& p1, const VECTOR3& p2) const
	{
		return m_aabb.Contains(p0, p1, p2) != DirectX::ContainmentType::DISJOINT;
	}

	inline bool Bounds::Contains(const DirectX::BoundingSphere& sp) const
	{
		return m_aabb.Contains(sp) != DirectX::ContainmentType::DISJOINT;
	}

	inline bool Bounds::Contains(const DirectX::BoundingBox& box) const
	{
		return m_aabb.Contains(box) != DirectX::ContainmentType::DISJOINT;
	}

	inline bool Bounds::Contains(const DirectX::BoundingOrientedBox& box) const
	{
		return m_aabb.Contains(box) != DirectX::ContainmentType::DISJOINT;
	}

	inline bool Bounds::Intersects(const VECTOR3& origin, const VECTOR3& direction, float dist) const
	{
		return m_aabb.Intersects(origin, direction, dist);
	}

	inline bool Bounds::ContainsBy(const DirectX::BoundingFrustum& fr) const
	{
		DirectX::XMVECTOR plane0, plane1, plane2, plane3, plane4, plane5;
		fr.GetPlanes(&plane0, &plane1, &plane2, &plane3, &plane4, &plane5);
		return m_aabb.ContainedBy(plane0, plane1, plane2, plane3, plane4, plane5) != DirectX::ContainmentType::DISJOINT;
	}

	inline bool Bounds::ContainsBy(const VECTOR3(&planes)[6]) const
	{
		return ContainsBy(planes[0], planes[1], planes[2], planes[3], planes[4], planes[5]);
	}

	inline bool Bounds::ContainsBy(const VECTOR3& plane0, const VECTOR3& plane1, const VECTOR3& plane2, const VECTOR3& plane3, const VECTOR3& plane4, const VECTOR3& plane5) const
	{
		return m_aabb.ContainedBy(plane0, plane1, plane2, plane3, plane4, plane5) != DirectX::ContainmentType::DISJOINT;
	}
}