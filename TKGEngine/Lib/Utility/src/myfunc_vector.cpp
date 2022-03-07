#include "../inc/myfunc_vector.h"

namespace TKGEngine
{
	const VECTOR2 VECTOR2::Zero = VECTOR2(0.f, 0.f);
	const VECTOR2 VECTOR2::One = { 1.f, 1.f };
	const VECTOR2 VECTOR2::UnitX = { 1.f, 0.f };
	const VECTOR2 VECTOR2::UnitY = { 0.f, 1.f };

	const VECTOR3 VECTOR3::Zero = { 0.f, 0.f, 0.f };
	const VECTOR3 VECTOR3::One = { 1.f, 1.f, 1.f };
	const VECTOR3 VECTOR3::UnitX = { 1.f, 0.f, 0.f };
	const VECTOR3 VECTOR3::UnitY = { 0.f, 1.f, 0.f };
	const VECTOR3 VECTOR3::UnitZ = { 0.f, 0.f, 1.f };
	const VECTOR3 VECTOR3::Up = { 0.f, 1.f, 0.f };
	const VECTOR3 VECTOR3::Down = { 0.f, -1.f, 0.f };
	const VECTOR3 VECTOR3::Right = { 1.f, 0.f, 0.f };
	const VECTOR3 VECTOR3::Left = { -1.f, 0.f, 0.f };
	const VECTOR3 VECTOR3::Forward = { 0.f, 0.f, 1.f };
	const VECTOR3 VECTOR3::Backward = { 0.f, 0.f, -1.f };

	const VECTOR4 VECTOR4::Zero = { 0.f, 0.f, 0.f, 0.f };
	const VECTOR4 VECTOR4::One = { 1.f, 1.f, 1.f, 1.f };
	const VECTOR4 VECTOR4::UnitX = { 1.f, 0.f, 0.f, 0.f };
	const VECTOR4 VECTOR4::UnitY = { 0.f, 1.f, 0.f, 0.f };
	const VECTOR4 VECTOR4::UnitZ = { 0.f, 0.f, 1.f, 0.f };
	const VECTOR4 VECTOR4::UnitW = { 0.f, 0.f, 0.f, 1.f };

	const MATRIX MATRIX::Identity = { 1.f, 0.f, 0.f, 0.f,
									  0.f, 1.f, 0.f, 0.f,
									  0.f, 0.f, 1.f, 0.f,
									  0.f, 0.f, 0.f, 1.f };
	const MATRIX MATRIX::Reversed_Z = { 1.f, 0.f, 0.f, 0.f,
									  0.f, 1.f, 0.f, 0.f,
									  0.f, 0.f, -1.f, 0.f,
									  0.f, 0.f, 1.f, 1.f };

	const Quaternion Quaternion::Identity = { 0.f,0.f,0.f,1.f };
	const Quaternion Quaternion::ROTATE_AXIS_X_180 = { 1.f,0.f,0.f,0.f };
	const Quaternion Quaternion::ROTATE_AXIS_Y_180 = { 0.f,1.f,0.f,0.f };
	const Quaternion Quaternion::ROTATE_AXIS_Z_180 = { 0.f,0.f,1.f,0.f };
}// namespace TKGEngine