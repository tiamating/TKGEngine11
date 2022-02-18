#pragma once

#include "Systems/inc/TKGEngine_Defined.h"
#include "Utility/inc/myfunc_vector.h"

#include <DirectXCollision.h>

namespace TKGEngine
{
	/// <summary>
	/// AABB (axis-aligned bounding box) module
	/// </summary>
	class Bounds
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Bounds();
		Bounds(const VECTOR3& center, const VECTOR3& extents);
		virtual ~Bounds() = default;
		Bounds(const Bounds&);
		Bounds(const DirectX::BoundingBox&);
		Bounds(Bounds&&);
		Bounds& operator=(const Bounds&);
		Bounds& operator=(const DirectX::BoundingBox&);

		operator DirectX::BoundingBox() const;

		void Reset();

		// AABBÇÃçáê¨
		void Union(const Bounds& bounds);

		VECTOR3 GetCenter() const;
		VECTOR3 GetExtents() const;
		void SetCenter(const VECTOR3& center);
		void SetExtents(const VECTOR3& extents);

		void CreateMerged(const DirectX::BoundingBox& b1, const DirectX::BoundingBox& b2);
		void CreateFromSphere(const DirectX::BoundingSphere& sp);
		void CreateFromPoints(const VECTOR3& p1, const VECTOR3& p2);

		Bounds Transform(const MATRIX& mat) const;
		void Transform(const VECTOR3& translation, const Quaternion& quat, const VECTOR3& scale);

		bool Contains(const VECTOR3& point) const;
		bool Contains(const VECTOR3& p0, const VECTOR3& p1, const VECTOR3& p2) const;	// triangle
		bool Contains(const DirectX::BoundingSphere& sp) const;
		bool Contains(const DirectX::BoundingBox& box) const;
		bool Contains(const DirectX::BoundingOrientedBox& box) const;

		bool Intersects(const VECTOR3& origin, const VECTOR3& direction, float dist) const;	// Ray

		bool ContainsBy(const DirectX::BoundingFrustum& fr) const;
		bool ContainsBy(const VECTOR3 (&planes)[6]) const;
		bool ContainsBy(
			const VECTOR3& plane0, const VECTOR3& plane1, const VECTOR3& plane2,
			const VECTOR3& plane3, const VECTOR3& plane4, const VECTOR3& plane5
		) const;

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(m_aabb.Center.x),
					CEREAL_NVP(m_aabb.Center.y),
					CEREAL_NVP(m_aabb.Center.z),
					CEREAL_NVP(m_aabb.Extents.x),
					CEREAL_NVP(m_aabb.Extents.y),
					CEREAL_NVP(m_aabb.Extents.z)
				);
			}
		}
		// ==============================================
		// private methods
		// ==============================================
		/* nothing */


		// ==============================================
		// private variables
		// ==============================================
		DirectX::BoundingBox m_aabb;

	};
}

CEREAL_CLASS_VERSION(TKGEngine::Bounds, 1)

#include "../src/bounds.inl"