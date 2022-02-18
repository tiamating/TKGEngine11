#pragma once

#include "Systems/inc/TKGEngine_Defined.h"
#include "Utility/inc/myfunc_vector.h"

#include <DirectXCollision.h>

namespace TKGEngine
{
	/// <summary>
	/// Camera frustum module
	/// </summary>
	class Frustum
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Frustum();
		Frustum(const MATRIX& projection);
		virtual ~Frustum() = default;
		Frustum(const Frustum&);
		Frustum(Frustum&&);
		Frustum& operator=(const Frustum&);
		Frustum& operator=(Frustum&&);

		operator DirectX::BoundingFrustum() const noexcept { return m_frustum; }

		void GetCorners(VECTOR3(&corners)[8]) const;
		DirectX::BoundingBox GetAABB() const;

		void GetPlanes(VECTOR3(&planes)[6]) const;
		void GetPlanes(
			VECTOR3& near_plane,
			VECTOR3& far_plane,
			VECTOR3& right_plane,
			VECTOR3& left_plane,
			VECTOR3& top_plane,
			VECTOR3& bottom_plane
		) const;

		void Create(const VECTOR3& pos, const Quaternion& quat, const MATRIX& projection);
		void CreateFromMatrix(const MATRIX& projection);

		void Transform(const MATRIX& matrix);
		void Transform(const VECTOR3& translation, const Quaternion& quat, const VECTOR3& scale);

		bool Contains(const VECTOR3& point) const;
		bool Contains(const VECTOR3& p0, const VECTOR3& p1, const VECTOR3& p2) const;	// triangle
		bool Contains(const DirectX::BoundingSphere& sp) const;
		bool Contains(const DirectX::BoundingBox& box) const;
		bool Contains(const DirectX::BoundingOrientedBox& box) const;

		bool Intersects(const VECTOR3& origin, const VECTOR3& direction, float dist) const;;	// Ray


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
					CEREAL_NVP(m_frustum.Origin.x),
					CEREAL_NVP(m_frustum.Origin.y),
					CEREAL_NVP(m_frustum.Origin.z),
					CEREAL_NVP(m_frustum.Orientation.x),
					CEREAL_NVP(m_frustum.Orientation.y),
					CEREAL_NVP(m_frustum.Orientation.z),
					CEREAL_NVP(m_frustum.Orientation.w),
					CEREAL_NVP(m_frustum.RightSlope),
					CEREAL_NVP(m_frustum.LeftSlope),
					CEREAL_NVP(m_frustum.TopSlope),
					CEREAL_NVP(m_frustum.BottomSlope),
					CEREAL_NVP(m_frustum.Near),
					CEREAL_NVP(m_frustum.Far)
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
		DirectX::BoundingFrustum m_frustum;

	};
}

CEREAL_CLASS_VERSION(TKGEngine::Frustum, 1)

#include "../src/Frustum.inl"
