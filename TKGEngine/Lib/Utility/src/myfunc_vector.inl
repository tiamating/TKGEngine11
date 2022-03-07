#pragma once
#include "../inc/myfunc_vector.h"

namespace TKGEngine
{
	// *****************************************************************************
	//
	//      VECTOR2
	//
	//******************************************************************************
#pragma region VECTOR2
// Comparison operators
	inline bool VECTOR2::operator==(const VECTOR2& v) const noexcept
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(this);
		const XMVECTOR v2 = XMLoadFloat2(&v);
		const XMVECTOR epsilon = XMLoadFloat(&MyMath::Epsilon);
		return XMVector2NearEqual(v1, v2, epsilon);
	}
	inline bool VECTOR2::operator!=(const VECTOR2& v) const noexcept
	{
		return !(*this == v);
	}

	// Assignment operators
	inline VECTOR2& VECTOR2::operator+=(const VECTOR2& v) noexcept
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(this);
		const XMVECTOR v2 = XMLoadFloat2(&v);
		const XMVECTOR X = XMVectorAdd(v1, v2);
		XMStoreFloat2(this, X);
		return *this;
	}
	inline VECTOR2& VECTOR2::operator-=(const VECTOR2& v) noexcept
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(this);
		const XMVECTOR v2 = XMLoadFloat2(&v);
		const XMVECTOR X = XMVectorSubtract(v1, v2);
		XMStoreFloat2(this, X);
		return *this;
	}
	inline VECTOR2& VECTOR2::operator*=(const VECTOR2& v) noexcept
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(this);
		const XMVECTOR v2 = XMLoadFloat2(&v);
		const XMVECTOR X = XMVectorMultiply(v1, v2);
		XMStoreFloat2(this, X);
		return *this;
	}
	inline VECTOR2& VECTOR2::operator*=(float s) noexcept
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(this);
		const XMVECTOR X = XMVectorScale(v1, s);
		XMStoreFloat2(this, X);
		return *this;
	}
	inline VECTOR2& VECTOR2::operator/=(float s)
	{
		using namespace DirectX;
		assert(s != 0.0f);
		const XMVECTOR v1 = XMLoadFloat2(this);
		const XMVECTOR X = XMVectorScale(v1, 1.f / s);
		XMStoreFloat2(this, X);
		return *this;
	}

	// Vector operations
	// ==================================================
	// 2Dベクトルのコンポーネントが設定された境界内にあるかどうかをテストする
	// 同値 : return (V.x <= Bounds.x && V.x >= -Bounds.x) && (V.y <= Bounds.y && V.y >= -Bounds.y);
	// ==================================================
	inline bool VECTOR2::InBounds(const VECTOR2& bounds) const
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(this);
		const XMVECTOR v2 = XMLoadFloat2(&bounds);
		return XMVector2InBounds(v1, v2);
	}

	// ==================================================
	// 2Dベクトルの長さ
	// ==================================================
	inline float VECTOR2::Length() const
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(this);
		const XMVECTOR X = XMVector2Length(v1);
		return XMVectorGetX(X);
	}
	// ==================================================
	// 2Dベクトルの長さの２乗
	// ==================================================
	inline constexpr float VECTOR2::LengthSq() const noexcept
	{
		return Dot(*this);
	}

	// ==================================================
	// 2Dベクトルの内積
	// ==================================================
	inline constexpr float VECTOR2::Dot(const VECTOR2& v) const noexcept
	{
		return x * v.x + y * v.y;
	}

	// ==================================================
	// 2Dベクトルの外積
	// ==================================================
	inline VECTOR3 VECTOR2::Cross(const VECTOR2& v) const
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(this);
		const XMVECTOR v2 = XMLoadFloat2(&v);
		const XMVECTOR R = XMVector3Cross(v1, v2);

		VECTOR3 result;
		XMStoreFloat3(&result, R);
		return result;
	}

	// ==================================================
	// ベクトルを正規化
	// ==================================================
	inline VECTOR2 VECTOR2::Normalized() const
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(this);
		const XMVECTOR X = XMVector2Normalize(v1);
		VECTOR2 result;
		XMStoreFloat2(&result, X);
		return result;
	}
	inline void VECTOR2::Normalized(VECTOR2& result) const
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(this);
		const XMVECTOR X = XMVector2Normalize(v1);
		XMStoreFloat2(&result, X);
	}

	// ==================================================
	// [ vmin, vmax ]の範囲に収める
	// ==================================================
	inline void VECTOR2::Clamp(const VECTOR2& vmin, const VECTOR2& vmax)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(this);
		const XMVECTOR v2 = XMLoadFloat2(&vmin);
		const XMVECTOR v3 = XMLoadFloat2(&vmax);
		const XMVECTOR X = XMVectorClamp(v1, v2, v3);
		XMStoreFloat2(this, X);
	}
	inline void VECTOR2::Clamp(const VECTOR2& vmin, const VECTOR2& vmax, VECTOR2& result) const
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(this);
		const XMVECTOR v2 = XMLoadFloat2(&vmin);
		const XMVECTOR v3 = XMLoadFloat2(&vmax);
		const XMVECTOR X = XMVectorClamp(v1, v2, v3);
		XMStoreFloat2(&result, X);
	}

	// Static functions
	// ==================================================
	// ベクトルのなす角
	// ==================================================
	inline float VECTOR2::Angle(const VECTOR2& v1, const VECTOR2& v2)
	{
		using namespace DirectX;
		const XMVECTOR V1 = XMVector3Normalize(XMLoadFloat2(&v1));
		const XMVECTOR V2 = XMVector3Normalize(XMLoadFloat2(&v2));
		float param = XMVectorGetX(XMVector2Dot(V1, V2));
		param = std::max(-1.0f, std::min(1.0f, param));
		return XMConvertToDegrees(acosf(param));
	}

	// ==================================================
	// 3Dベクトルの内積
	// ==================================================
	inline float VECTOR2::Dot(const VECTOR2& v1, const VECTOR2& v2)
	{
		using namespace DirectX;
		const XMVECTOR V1 = XMLoadFloat2(&v1);
		const XMVECTOR V2 = XMLoadFloat2(&v2);
		return XMVectorGetX(XMVector2Dot(V1, V2));
	}

	// ==================================================
	// 3Dベクトルの外積
	// ==================================================
	inline void VECTOR2::Cross(const VECTOR2& v1, const VECTOR2& v2, VECTOR3& result)
	{
		using namespace DirectX;
		const XMVECTOR V1 = XMLoadFloat2(&v1);
		const XMVECTOR V2 = XMLoadFloat2(&v2);
		const XMVECTOR R = XMVector3Cross(V1, V2);
		XMStoreFloat3(&result, R);
	}
	inline VECTOR3 VECTOR2::Cross(const VECTOR2& v1, const VECTOR2& v2)
	{
		using namespace DirectX;
		const XMVECTOR V1 = XMLoadFloat2(&v1);
		const XMVECTOR V2 = XMLoadFloat2(&v2);
		const XMVECTOR R = XMVector3Cross(V1, V2);
		VECTOR3 result;
		XMStoreFloat3(&result, R);
		return result;
	}

	// ==================================================
	// 2点間の距離
	// ==================================================
	inline float VECTOR2::Distance(const VECTOR2& v1, const VECTOR2& v2)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat2(&v1);
		const XMVECTOR x2 = XMLoadFloat2(&v2);
		const XMVECTOR V = XMVectorSubtract(x2, x1);
		const XMVECTOR X = XMVector2Length(V);
		return XMVectorGetX(X);
	}
	inline float VECTOR2::DistanceSq(const VECTOR2& v1, const VECTOR2& v2)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat2(&v1);
		const XMVECTOR x2 = XMLoadFloat2(&v2);
		const XMVECTOR V = XMVectorSubtract(x2, x1);
		const XMVECTOR X = XMVector2LengthSq(V);
		return XMVectorGetX(X);
	}

	// ==================================================
	// 2つのベクトルで各成分の一番小さな値を使用してベクトルを作成
	// ==================================================
	inline void VECTOR2::Min(const VECTOR2& v1, const VECTOR2& v2, VECTOR2& result)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat2(&v1);
		const XMVECTOR x2 = XMLoadFloat2(&v2);
		const XMVECTOR X = XMVectorMin(x1, x2);
		XMStoreFloat2(&result, X);
	}
	inline VECTOR2 VECTOR2::Min(const VECTOR2& v1, const VECTOR2& v2)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat2(&v1);
		const XMVECTOR x2 = XMLoadFloat2(&v2);
		const XMVECTOR X = XMVectorMin(x1, x2);

		VECTOR2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	// ==================================================
	// 2つのベクトルで各成分の一番大きな値を使用してベクトルを作成
	// ==================================================
	inline void VECTOR2::Max(const VECTOR2& v1, const VECTOR2& v2, VECTOR2& result)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat2(&v1);
		const XMVECTOR x2 = XMLoadFloat2(&v2);
		const XMVECTOR X = XMVectorMax(x1, x2);
		XMStoreFloat2(&result, X);
	}
	inline VECTOR2 VECTOR2::Max(const VECTOR2& v1, const VECTOR2& v2)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat2(&v1);
		const XMVECTOR x2 = XMLoadFloat2(&v2);
		const XMVECTOR X = XMVectorMax(x1, x2);

		VECTOR2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	// ==================================================
	// 2つのベクトル間の線形補間
	// ==================================================
	inline void VECTOR2::Lerp(const VECTOR2& v1, const VECTOR2& v2, float t, VECTOR2& result)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat2(&v1);
		const XMVECTOR x2 = XMLoadFloat2(&v2);
		const XMVECTOR X = XMVectorLerp(x1, x2, t);
		XMStoreFloat2(&result, X);
	}
	inline VECTOR2 VECTOR2::Lerp(const VECTOR2& v1, const VECTOR2& v2, float t)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat2(&v1);
		const XMVECTOR x2 = XMLoadFloat2(&v2);
		const XMVECTOR X = XMVectorLerp(x1, x2, t);

		VECTOR2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	// ==================================================
	// 2つのベクトル間のなめらかな遷移
	// ==================================================
	inline void VECTOR2::SmoothStep(const VECTOR2& v1, const VECTOR2& v2, float t, VECTOR2& result)
	{
		using namespace DirectX;
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t * (3.f - 2.f * t);
		const XMVECTOR x1 = XMLoadFloat2(&v1);
		const XMVECTOR x2 = XMLoadFloat2(&v2);
		const XMVECTOR X = XMVectorLerp(x1, x2, t);
		XMStoreFloat2(&result, X);
	}
	inline VECTOR2 VECTOR2::SmoothStep(const VECTOR2& v1, const VECTOR2& v2, float t)
	{
		using namespace DirectX;
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t * (3.f - 2.f * t);
		const XMVECTOR x1 = XMLoadFloat2(&v1);
		const XMVECTOR x2 = XMLoadFloat2(&v2);
		const XMVECTOR X = XMVectorLerp(x1, x2, t);

		VECTOR2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	// ==================================================
	// キャットムル-ロム スプライン曲線上の点
	// ==================================================
	inline void VECTOR2::CatmullRom(const VECTOR2& v1, const VECTOR2& v2, const VECTOR2& v3, const VECTOR2& v4, float t, VECTOR2& result)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat2(&v1);
		const XMVECTOR x2 = XMLoadFloat2(&v2);
		const XMVECTOR x3 = XMLoadFloat2(&v3);
		const XMVECTOR x4 = XMLoadFloat2(&v4);
		const XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);
		XMStoreFloat2(&result, X);
	}
	inline VECTOR2 VECTOR2::CatmullRom(const VECTOR2& v1, const VECTOR2& v2, const VECTOR2& v3, const VECTOR2& v4, float t)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat2(&v1);
		const XMVECTOR x2 = XMLoadFloat2(&v2);
		const XMVECTOR x3 = XMLoadFloat2(&v3);
		const XMVECTOR x4 = XMLoadFloat2(&v4);
		const XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);

		VECTOR2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	// ==================================================
	// エルミート スプライン補間(始点から終点まで、速度をt1->t2に変化して移動する)
	// ==================================================
	inline void VECTOR2::Hermite(const VECTOR2& v1, const VECTOR2& t1, const VECTOR2& v2, const VECTOR2& t2, float t, VECTOR2& result)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat2(&v1);
		const XMVECTOR x2 = XMLoadFloat2(&t1);
		const XMVECTOR x3 = XMLoadFloat2(&v2);
		const XMVECTOR x4 = XMLoadFloat2(&t2);
		const XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);
		XMStoreFloat2(&result, X);
	}
	inline VECTOR2 VECTOR2::Hermite(const VECTOR2& v1, const VECTOR2& t1, const VECTOR2& v2, const VECTOR2& t2, float t)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat2(&v1);
		const XMVECTOR x2 = XMLoadFloat2(&t1);
		const XMVECTOR x3 = XMLoadFloat2(&v2);
		const XMVECTOR x4 = XMLoadFloat2(&t2);
		const XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);

		VECTOR2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	// ==================================================
	// 入射、法線ベクトルから反射ベクトルを求める
	// ==================================================
	inline void VECTOR2::Reflect(const VECTOR2& incident, const VECTOR2& normal, VECTOR2& result)
	{
		using namespace DirectX;
		const XMVECTOR i = XMLoadFloat2(&incident);
		const XMVECTOR n = XMLoadFloat2(&normal);
		const XMVECTOR X = XMVector2Reflect(i, n);
		XMStoreFloat2(&result, X);
	}
	inline VECTOR2 VECTOR2::Reflect(const VECTOR2& incident, const VECTOR2& normal)
	{
		using namespace DirectX;
		const XMVECTOR i = XMLoadFloat2(&incident);
		const XMVECTOR n = XMLoadFloat2(&normal);
		const XMVECTOR X = XMVector2Reflect(i, n);

		VECTOR2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	// ==================================================
	// 入射、法線ベクトルと屈折率から屈折ベクトルを求める
	// ==================================================
	inline void VECTOR2::Refract(const VECTOR2& incident, const VECTOR2& normal, float refractionIndex, VECTOR2& result)
	{
		using namespace DirectX;
		const XMVECTOR i = XMLoadFloat2(&incident);
		const XMVECTOR n = XMLoadFloat2(&normal);
		const XMVECTOR X = XMVector2Refract(i, n, refractionIndex);
		XMStoreFloat2(&result, X);
	}
	inline VECTOR2 VECTOR2::Refract(const VECTOR2& incident, const VECTOR2& normal, float refractionIndex)
	{
		using namespace DirectX;
		const XMVECTOR i = XMLoadFloat2(&incident);
		const XMVECTOR n = XMLoadFloat2(&normal);
		const XMVECTOR X = XMVector2Refract(i, n, refractionIndex);

		VECTOR2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	// ==================================================
	// クォータニオン回転、位置ベクトルと方向ベクトルの行列変換
	// ==================================================
	inline void VECTOR2::TransformRotate(const VECTOR2& v, const Quaternion& q, VECTOR2& result)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(&v);
		const XMVECTOR Q = XMLoadFloat4(&q);
		const XMVECTOR X = XMVector3Rotate(v1, Q);
		XMStoreFloat2(&result, X);
	}
	inline VECTOR2 VECTOR2::TransformRotate(const VECTOR2& v, const Quaternion& q)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(&v);
		const XMVECTOR Q = XMLoadFloat4(&q);
		const XMVECTOR X = XMVector3Rotate(v1, Q);

		VECTOR2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	inline void VECTOR2::TransformMatrix(const VECTOR2& v, const MATRIX& m, VECTOR2& result)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(&v);
		const XMMATRIX M = XMLoadFloat4x4(&m);
		const XMVECTOR X = XMVector2TransformCoord(v1, M);
		XMStoreFloat2(&result, X);
	}
	inline VECTOR2 VECTOR2::TransformMatrix(const VECTOR2& v, const MATRIX& m)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(&v);
		const XMMATRIX M = XMLoadFloat4x4(&m);
		const XMVECTOR X = XMVector2TransformCoord(v1, M);

		VECTOR2 result;
		XMStoreFloat2(&result, X);
		return result;
	}

	inline void VECTOR2::TransformNormal(const VECTOR2& v, const MATRIX& m, VECTOR2& result)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(&v);
		const XMMATRIX M = XMLoadFloat4x4(&m);
		const XMVECTOR X = XMVector2TransformNormal(v1, M);
		XMStoreFloat2(&result, X);
	}
	inline VECTOR2 VECTOR2::TransformNormal(const VECTOR2& v, const MATRIX& m)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(&v);
		const XMMATRIX M = XMLoadFloat4x4(&m);
		const XMVECTOR X = XMVector2TransformNormal(v1, M);

		VECTOR2 result;
		XMStoreFloat2(&result, X);
		return result;
	}


	// Binary operators
	inline constexpr VECTOR2 operator+ (const VECTOR2& v1, const VECTOR2& v2) noexcept
	{
		return VECTOR2(v1.x + v2.x, v1.y + v2.y);
	}
	inline constexpr VECTOR2 operator- (const VECTOR2& v1, const VECTOR2& v2) noexcept
	{
		return VECTOR2(v1.x - v2.x, v1.y - v2.y);
	}
	inline constexpr VECTOR2 operator* (const VECTOR2& v1, const VECTOR2& v2) noexcept
	{
		return VECTOR2(v1.x * v2.x, v1.y * v2.y);
	}
	inline constexpr VECTOR2 operator* (float s, const VECTOR2& v) noexcept
	{
		return VECTOR2(v.x * s, v.y * s);
	}
	inline constexpr VECTOR2 operator* (const VECTOR2& v, float s) noexcept
	{
		return VECTOR2(v.x * s, v.y * s);
	}
	inline constexpr VECTOR2 operator/ (const VECTOR2& v1, const VECTOR2& v2)
	{
		return VECTOR2(v1.x * MyMath::InvertDivisionValue(v2.x), v1.y * MyMath::InvertDivisionValue(v2.y));
	}
	inline constexpr VECTOR2 operator/ (const VECTOR2& v, float s)
	{
		return v * MyMath::InvertDivisionValue(s);
	}
#pragma endregion


	// *****************************************************************************
	//
	//      VECTOR3
	//
	//******************************************************************************
#pragma region VECTOR3
// Comparison operators
	inline bool VECTOR3::operator==(const VECTOR3& v) const noexcept
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat3(this);
		const XMVECTOR v2 = XMLoadFloat3(&v);
		const XMVECTOR epsilon = XMLoadFloat(&MyMath::Epsilon);
		return XMVector3NearEqual(v1, v2, epsilon);
	}
	inline bool VECTOR3::operator!=(const VECTOR3& v) const noexcept
	{
		return !(*this == v);
	}

	// Assignment operators
	inline VECTOR3& VECTOR3::operator+=(const VECTOR3& v) noexcept
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat3(this);
		const XMVECTOR v2 = XMLoadFloat3(&v);
		const XMVECTOR X = XMVectorAdd(v1, v2);
		XMStoreFloat3(this, X);
		return *this;
	}
	inline VECTOR3& VECTOR3::operator-=(const VECTOR3& v) noexcept
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat3(this);
		const XMVECTOR v2 = XMLoadFloat3(&v);
		const XMVECTOR X = XMVectorSubtract(v1, v2);
		XMStoreFloat3(this, X);
		return *this;
	}
	inline VECTOR3& VECTOR3::operator*=(const VECTOR3& v) noexcept
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat3(this);
		const XMVECTOR v2 = XMLoadFloat3(&v);
		const XMVECTOR X = XMVectorMultiply(v1, v2);
		XMStoreFloat3(this, X);
		return *this;
	}
	inline VECTOR3& VECTOR3::operator*=(float s) noexcept
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat3(this);
		const XMVECTOR X = XMVectorScale(v1, s);
		XMStoreFloat3(this, X);
		return *this;
	}
	inline VECTOR3& VECTOR3::operator/=(float s)
	{
		using namespace DirectX;
		assert(s != 0.0f);
		const XMVECTOR v1 = XMLoadFloat3(this);
		const XMVECTOR X = XMVectorScale(v1, 1.f / s);
		XMStoreFloat3(this, X);
		return *this;
	}

	// Vector operations
	// ==================================================
	// 3Dベクトルのコンポーネントが設定された境界内にあるかどうかをテストする
	// 同値 : return (V.x <= Bounds.x && V.x >= -Bounds.x) && (V.y <= Bounds.y && V.y >= -Bounds.y) && (V.z <= Bounds.z && V.z >= -Bounds.z);
	// ==================================================
	inline bool VECTOR3::InBounds(const VECTOR3& bounds) const
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat3(this);
		const XMVECTOR v2 = XMLoadFloat3(&bounds);
		return XMVector3InBounds(v1, v2);
	}

	// ==================================================
	// 3Dベクトルの長さ
	// ==================================================
	inline float VECTOR3::Length() const
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat3(this);
		const XMVECTOR X = XMVector3Length(v1);
		return XMVectorGetX(X);
	}
	// ==================================================
	// 3Dベクトルの長さの２乗
	// ==================================================
	inline constexpr float VECTOR3::LengthSq() const noexcept
	{
		return Dot(*this);
	}

	// ==================================================
	// 3Dベクトルの内積
	// ==================================================
	inline constexpr float VECTOR3::Dot(const VECTOR3& v) const noexcept
	{
		return x * v.x + y * v.y + z * v.z;
	}

	// ==================================================
	// 3Dベクトルの外積
	// ==================================================
	inline VECTOR3 VECTOR3::Cross(const VECTOR3& v) const
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat3(this);
		const XMVECTOR v2 = XMLoadFloat3(&v);
		const XMVECTOR R = XMVector3Cross(v1, v2);

		VECTOR3 result;
		XMStoreFloat3(&result, R);
		return result;
	}

	// ==================================================
	// ベクトルを正規化
	// ==================================================
	inline VECTOR3 VECTOR3::Normalized() const
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat3(this);
		const XMVECTOR X = XMVector3Normalize(v1);
		VECTOR3 result;
		XMStoreFloat3(&result, X);
		return result;
	}
	inline void VECTOR3::Normalized(VECTOR3& result) const
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat3(this);
		const XMVECTOR X = XMVector3Normalize(v1);
		XMStoreFloat3(&result, X);
	}

	// ==================================================
	// [ vmin, vmax ]の範囲に収める
	// ==================================================
	inline void VECTOR3::Clamp(const VECTOR3& vmin, const VECTOR3& vmax)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat3(this);
		const XMVECTOR v2 = XMLoadFloat3(&vmin);
		const XMVECTOR v3 = XMLoadFloat3(&vmax);
		const XMVECTOR X = XMVectorClamp(v1, v2, v3);
		XMStoreFloat3(this, X);
	}
	inline void VECTOR3::Clamp(const VECTOR3& vmin, const VECTOR3& vmax, VECTOR3& result) const
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat3(this);
		const XMVECTOR v2 = XMLoadFloat3(&vmin);
		const XMVECTOR v3 = XMLoadFloat3(&vmax);
		const XMVECTOR X = XMVectorClamp(v1, v2, v3);
		XMStoreFloat3(&result, X);
	}

	// ==================================================
	// オイラー角からクォータニオンを生成
	// ==================================================
	inline Quaternion VECTOR3::ToQuaternion() const
	{
		using namespace DirectX;
		Quaternion result;
		XMStoreFloat4(&result, XMQuaternionRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z)));
		return result.Normalize();
	}

	// Static functions
	// ==================================================
	// ベクトルのなす角
	// ==================================================
	inline float VECTOR3::Angle(const VECTOR3& v1, const VECTOR3& v2)
	{
		using namespace DirectX;
		const XMVECTOR V1 = XMVector3Normalize(XMLoadFloat3(&v1));
		const XMVECTOR V2 = XMVector3Normalize(XMLoadFloat3(&v2));
		float param = XMVectorGetX(XMVector3Dot(V1, V2));
		param = std::max(-1.0f, std::min(1.0f, param));
		return XMConvertToDegrees(acosf(param));
	}

	// ==================================================
	// 3Dベクトルの内積
	// ==================================================
	inline float VECTOR3::Dot(const VECTOR3& v1, const VECTOR3& v2)
	{
		using namespace DirectX;
		const XMVECTOR V1 = XMLoadFloat3(&v1);
		const XMVECTOR V2 = XMLoadFloat3(&v2);
		return XMVectorGetX(XMVector3Dot(V1, V2));
	}

	// ==================================================
	// 3Dベクトルの外積
	// ==================================================
	inline void VECTOR3::Cross(const VECTOR3& v1, const VECTOR3& v2, VECTOR3& result)
	{
		using namespace DirectX;
		const XMVECTOR V1 = XMLoadFloat3(&v1);
		const XMVECTOR V2 = XMLoadFloat3(&v2);
		const XMVECTOR R = XMVector3Cross(V1, V2);
		XMStoreFloat3(&result, R);
	}
	inline VECTOR3 VECTOR3::Cross(const VECTOR3& v1, const VECTOR3& v2)
	{
		using namespace DirectX;
		const XMVECTOR V1 = XMLoadFloat3(&v1);
		const XMVECTOR V2 = XMLoadFloat3(&v2);
		const XMVECTOR R = XMVector3Cross(V1, V2);
		VECTOR3 result;
		XMStoreFloat3(&result, R);
		return result;
	}

	// ==================================================
	// 2点間の距離
	// ==================================================
	inline float VECTOR3::Distance(const VECTOR3& v1, const VECTOR3& v2)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat3(&v1);
		const XMVECTOR x2 = XMLoadFloat3(&v2);
		const XMVECTOR V = XMVectorSubtract(x2, x1);
		const XMVECTOR X = XMVector3Length(V);
		return XMVectorGetX(X);
	}
	inline float VECTOR3::DistanceSq(const VECTOR3& v1, const VECTOR3& v2)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat3(&v1);
		const XMVECTOR x2 = XMLoadFloat3(&v2);
		const XMVECTOR V = XMVectorSubtract(x2, x1);
		const XMVECTOR X = XMVector3LengthSq(V);
		return XMVectorGetX(X);
	}

	// ==================================================
	// 2つのベクトルで各成分の一番小さな値を使用してベクトルを作成
	// ==================================================
	inline void VECTOR3::Min(const VECTOR3& v1, const VECTOR3& v2, VECTOR3& result)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat3(&v1);
		const XMVECTOR x2 = XMLoadFloat3(&v2);
		const XMVECTOR X = XMVectorMin(x1, x2);
		XMStoreFloat3(&result, X);
	}
	inline VECTOR3 VECTOR3::Min(const VECTOR3& v1, const VECTOR3& v2)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat3(&v1);
		const XMVECTOR x2 = XMLoadFloat3(&v2);
		const XMVECTOR X = XMVectorMin(x1, x2);

		VECTOR3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	// ==================================================
	// 2つのベクトルで各成分の一番大きな値を使用してベクトルを作成
	// ==================================================
	inline void VECTOR3::Max(const VECTOR3& v1, const VECTOR3& v2, VECTOR3& result)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat3(&v1);
		const XMVECTOR x2 = XMLoadFloat3(&v2);
		const XMVECTOR X = XMVectorMax(x1, x2);
		XMStoreFloat3(&result, X);
	}
	inline VECTOR3 VECTOR3::Max(const VECTOR3& v1, const VECTOR3& v2)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat3(&v1);
		const XMVECTOR x2 = XMLoadFloat3(&v2);
		const XMVECTOR X = XMVectorMax(x1, x2);

		VECTOR3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	// ==================================================
	// 2つのベクトル間の線形補間
	// ==================================================
	inline void VECTOR3::Lerp(const VECTOR3& v1, const VECTOR3& v2, const float t, VECTOR3& result)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat3(&v1);
		const XMVECTOR x2 = XMLoadFloat3(&v2);
		const XMVECTOR X = XMVectorLerp(x1, x2, t);
		XMStoreFloat3(&result, X);
	}
	inline VECTOR3 VECTOR3::Lerp(const VECTOR3& v1, const VECTOR3& v2, const float t)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat3(&v1);
		const XMVECTOR x2 = XMLoadFloat3(&v2);
		const XMVECTOR X = XMVectorLerp(x1, x2, t);

		VECTOR3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	// ==================================================
	// 2つのベクトル間の球面補間
	// ==================================================
	inline void VECTOR3::Slerp(const VECTOR3& v1, const VECTOR3& v2, const float t, VECTOR3& result)
	{
		// 差分クォータニオンを求める
		const Quaternion diff = Quaternion::FromToRotation(v1, v2);
		// 補間量を求める
		const Quaternion interpolation = Quaternion::Slerp(Quaternion::Identity, diff, t);
		// 補間量を加える
		result = VECTOR3::TransformRotate(v1, interpolation);
	}
	inline VECTOR3 VECTOR3::Slerp(const VECTOR3& v1, const VECTOR3& v2, const float t)
	{
		// 差分クォータニオンを求める
		const Quaternion diff = Quaternion::FromToRotation(v1, v2);
		// 補間量を求める
		const Quaternion interpolation = Quaternion::Slerp(Quaternion::Identity, diff, t);
		// 補間量を加える
		return VECTOR3::TransformRotate(v1, interpolation);
	}

	// ==================================================
	// 2つのベクトル間のなめらかな遷移
	// ==================================================
	inline void VECTOR3::SmoothStep(const VECTOR3& v1, const VECTOR3& v2, float t, VECTOR3& result)
	{
		using namespace DirectX;
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t * (3.f - 2.f * t);
		const XMVECTOR x1 = XMLoadFloat3(&v1);
		const XMVECTOR x2 = XMLoadFloat3(&v2);
		const XMVECTOR X = XMVectorLerp(x1, x2, t);
		XMStoreFloat3(&result, X);
	}
	inline VECTOR3 VECTOR3::SmoothStep(const VECTOR3& v1, const VECTOR3& v2, float t)
	{
		using namespace DirectX;
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t * (3.f - 2.f * t);
		const XMVECTOR x1 = XMLoadFloat3(&v1);
		const XMVECTOR x2 = XMLoadFloat3(&v2);
		const XMVECTOR X = XMVectorLerp(x1, x2, t);

		VECTOR3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	// ==================================================
	// キャットムル-ロム スプライン曲線上の点
	// ==================================================
	inline void VECTOR3::CatmullRom(const VECTOR3& v1, const VECTOR3& v2, const VECTOR3& v3, const VECTOR3& v4, float t, VECTOR3& result)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat3(&v1);
		const XMVECTOR x2 = XMLoadFloat3(&v2);
		const XMVECTOR x3 = XMLoadFloat3(&v3);
		const XMVECTOR x4 = XMLoadFloat3(&v4);
		const XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);
		XMStoreFloat3(&result, X);
	}
	inline VECTOR3 VECTOR3::CatmullRom(const VECTOR3& v1, const VECTOR3& v2, const VECTOR3& v3, const VECTOR3& v4, float t)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat3(&v1);
		const XMVECTOR x2 = XMLoadFloat3(&v2);
		const XMVECTOR x3 = XMLoadFloat3(&v3);
		const XMVECTOR x4 = XMLoadFloat3(&v4);
		const XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);

		VECTOR3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	// ==================================================
	// エルミート スプライン補間(始点から終点まで、速度をt1->t2に変化して移動する)
	// ==================================================
	inline void VECTOR3::Hermite(const VECTOR3& v1, const VECTOR3& t1, const VECTOR3& v2, const VECTOR3& t2, float t, VECTOR3& result)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat3(&v1);
		const XMVECTOR x2 = XMLoadFloat3(&t1);
		const XMVECTOR x3 = XMLoadFloat3(&v2);
		const XMVECTOR x4 = XMLoadFloat3(&t2);
		const XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);
		XMStoreFloat3(&result, X);
	}
	inline VECTOR3 VECTOR3::Hermite(const VECTOR3& v1, const VECTOR3& t1, const VECTOR3& v2, const VECTOR3& t2, float t)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat3(&v1);
		const XMVECTOR x2 = XMLoadFloat3(&t1);
		const XMVECTOR x3 = XMLoadFloat3(&v2);
		const XMVECTOR x4 = XMLoadFloat3(&t2);
		const XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);

		VECTOR3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	// ==================================================
	// 入射、法線ベクトルから反射ベクトルを求める
	// ==================================================
	inline void VECTOR3::Reflect(const VECTOR3& incident, const VECTOR3& normal, VECTOR3& result)
	{
		using namespace DirectX;
		const XMVECTOR i = XMLoadFloat3(&incident);
		const XMVECTOR n = XMLoadFloat3(&normal);
		const XMVECTOR X = XMVector3Reflect(i, n);
		XMStoreFloat3(&result, X);
	}
	inline VECTOR3 VECTOR3::Reflect(const VECTOR3& incident, const VECTOR3& normal)
	{
		using namespace DirectX;
		const XMVECTOR i = XMLoadFloat3(&incident);
		const XMVECTOR n = XMLoadFloat3(&normal);
		const XMVECTOR X = XMVector3Reflect(i, n);

		VECTOR3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	// ==================================================
	// 入射、法線ベクトルと屈折率から屈折ベクトルを求める
	// ==================================================
	inline void VECTOR3::Refract(const VECTOR3& incident, const VECTOR3& normal, float refractionIndex, VECTOR3& result)
	{
		using namespace DirectX;
		const XMVECTOR i = XMLoadFloat3(&incident);
		const XMVECTOR n = XMLoadFloat3(&normal);
		const XMVECTOR X = XMVector3Refract(i, n, refractionIndex);
		XMStoreFloat3(&result, X);
	}
	inline VECTOR3 VECTOR3::Refract(const VECTOR3& incident, const VECTOR3& normal, float refractionIndex)
	{
		using namespace DirectX;
		const XMVECTOR i = XMLoadFloat3(&incident);
		const XMVECTOR n = XMLoadFloat3(&normal);
		const XMVECTOR X = XMVector3Refract(i, n, refractionIndex);

		VECTOR3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	// ==================================================
	// クォータニオン回転、位置ベクトルと方向ベクトルの行列変換
	// ==================================================
	inline void VECTOR3::TransformRotate(const VECTOR2& v, const Quaternion& q, VECTOR3& result)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(&v);
		const XMVECTOR Q = XMLoadFloat4(&q);
		const XMVECTOR X = XMVector3Rotate(v1, Q);
		XMStoreFloat3(&result, X);
	}
	inline VECTOR3 VECTOR3::TransformRotate(const VECTOR2& v, const Quaternion& q)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(&v);
		const XMVECTOR Q = XMLoadFloat4(&q);
		const XMVECTOR X = XMVector3Rotate(v1, Q);

		VECTOR3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	inline void VECTOR3::TransformRotate(const VECTOR3& v, const Quaternion& q, VECTOR3& result)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat3(&v);
		const XMVECTOR Q = XMLoadFloat4(&q);
		const XMVECTOR X = XMVector3Rotate(v1, Q);
		XMStoreFloat3(&result, X);
	}
	inline VECTOR3 VECTOR3::TransformRotate(const VECTOR3& v, const Quaternion& q)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat3(&v);
		const XMVECTOR Q = XMLoadFloat4(&q);
		const XMVECTOR X = XMVector3Rotate(v1, Q);

		VECTOR3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	inline void VECTOR3::TransformMatrix(const VECTOR3& v, const MATRIX& m, VECTOR3& result)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat3(&v);
		const XMMATRIX M = XMLoadFloat4x4(&m);
		const XMVECTOR X = XMVector3TransformCoord(v1, M);
		XMStoreFloat3(&result, X);
	}
	inline VECTOR3 VECTOR3::TransformMatrix(const VECTOR3& v, const MATRIX& m)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat3(&v);
		const XMMATRIX M = XMLoadFloat4x4(&m);
		const XMVECTOR X = XMVector3TransformCoord(v1, M);

		VECTOR3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	inline void VECTOR3::TransformNormal(const VECTOR3& v, const MATRIX& m, VECTOR3& result)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat3(&v);
		const XMMATRIX M = XMLoadFloat4x4(&m);
		const XMVECTOR X = XMVector3TransformNormal(v1, M);
		XMStoreFloat3(&result, X);
	}
	inline VECTOR3 VECTOR3::TransformNormal(const VECTOR3& v, const MATRIX& m)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat3(&v);
		const XMMATRIX M = XMLoadFloat4x4(&m);
		const XMVECTOR X = XMVector3TransformNormal(v1, M);

		VECTOR3 result;
		XMStoreFloat3(&result, X);
		return result;
	}

	// 回転行列からオイラー角を生成
	inline VECTOR3 VECTOR3::CreateEulerAngles(const MATRIX& m)
	{
		using namespace DirectX;
		VECTOR3 result;

		// _32が1を超えることがある
		const float _32_clamped = MyMath::Clamp(m._32, -1.0f, 1.0f);
		// m._32 == 1.fの時
		if (fabsf(_32_clamped - 1.0f) < FLT_EPSILON)
		{
			result.x = -XM_PIDIV2;
			result.y = 0.0f;
			result.z = atan2(m._21, m._11);
		}
		// m._32 == -1.fの時
		else if (fabsf(_32_clamped + 1.0f) < FLT_EPSILON)
		{
			result.x = XM_PIDIV2;
			result.y = 0.0f;
			result.z = atan2(m._21, m._11);
		}
		else
		{
			result.x = asin(-_32_clamped);
			result.y = atan2(m._31, m._33);
			result.z = atan2(m._12, m._22);
		}
		result.x = XMConvertToDegrees(result.x);
		result.y = XMConvertToDegrees(result.y);
		result.z = XMConvertToDegrees(result.z);

		return result;
	}

	// Binary operators
	inline constexpr VECTOR3 operator+ (const VECTOR3& v1, const VECTOR3& v2) noexcept
	{
		return VECTOR3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
	}
	inline constexpr VECTOR3 operator- (const VECTOR3& v1, const VECTOR3& v2) noexcept
	{
		return VECTOR3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
	}
	inline constexpr VECTOR3 operator* (const VECTOR3& v1, const VECTOR3& v2) noexcept
	{
		return VECTOR3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
	}
	inline constexpr VECTOR3 operator* (float s, const VECTOR3& v) noexcept
	{
		return VECTOR3(v.x * s, v.y * s, v.z * s);
	}
	inline constexpr VECTOR3 operator*(const VECTOR3& v, const MATRIX& mat) noexcept
	{
		return VECTOR3(
			v.x * mat._11 + v.y * mat._21 + v.z * mat._31,
			v.x * mat._12 + v.y * mat._22 + v.z * mat._32,
			v.x * mat._13 + v.y * mat._23 + v.z * mat._33
		);
	}
	inline constexpr VECTOR3 operator* (const VECTOR3& v, float s) noexcept
	{
		return VECTOR3(v.x * s, v.y * s, v.z * s);
	}
	inline constexpr VECTOR3 operator/ (const VECTOR3& v1, const VECTOR3& v2)
	{
		return VECTOR3(
			v1.x * MyMath::InvertDivisionValue(v2.x),
			v1.y * MyMath::InvertDivisionValue(v2.y),
			v1.z * MyMath::InvertDivisionValue(v2.z)
		);
	}
	inline constexpr VECTOR3 operator/ (const VECTOR3& v, float s)
	{
		return v * MyMath::InvertDivisionValue(s);
	}
#pragma endregion


	// *****************************************************************************
	//
	//      VECTOR4
	//
	//******************************************************************************
#pragma region VECTOR4
	// Comparison operators
	inline bool VECTOR4::operator==(const VECTOR4& v) const noexcept
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat4(this);
		const XMVECTOR v2 = XMLoadFloat4(&v);
		const XMVECTOR epsilon = XMLoadFloat(&MyMath::Epsilon);
		return XMVector4NearEqual(v1, v2, epsilon);
	}
	inline bool VECTOR4::operator!=(const VECTOR4& v) const noexcept
	{
		return !(*this == v);
	}

	// Assignment operators
	inline VECTOR4& VECTOR4::operator+=(const VECTOR4& v) noexcept
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat4(this);
		const XMVECTOR v2 = XMLoadFloat4(&v);
		const XMVECTOR X = XMVectorAdd(v1, v2);
		XMStoreFloat4(this, X);
		return *this;
	}
	inline VECTOR4& VECTOR4::operator-=(const VECTOR4& v) noexcept
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat4(this);
		const XMVECTOR v2 = XMLoadFloat4(&v);
		const XMVECTOR X = XMVectorSubtract(v1, v2);
		XMStoreFloat4(this, X);
		return *this;
	}
	inline VECTOR4& VECTOR4::operator*=(const VECTOR4& v) noexcept
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat4(this);
		const XMVECTOR v2 = XMLoadFloat4(&v);
		const XMVECTOR X = XMVectorMultiply(v1, v2);
		XMStoreFloat4(this, X);
		return *this;
	}
	inline VECTOR4& VECTOR4::operator*=(float s) noexcept
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat4(this);
		const XMVECTOR X = XMVectorScale(v1, s);
		XMStoreFloat4(this, X);
		return *this;
	}
	inline VECTOR4& VECTOR4::operator/=(float s)
	{
		using namespace DirectX;
		assert(s != 0.0f);
		const XMVECTOR v1 = XMLoadFloat4(this);
		const XMVECTOR X = XMVectorScale(v1, 1.f / s);
		XMStoreFloat4(this, X);
		return *this;
	}

	// Vector operations
	// ==================================================
	// 4Dベクトルのコンポーネントが設定された境界内にあるかどうかをテストする
	// 同値 : return (V.x <= Bounds.x && V.x >= -Bounds.x) && (V.y <= Bounds.y && V.y >= -Bounds.y) && (V.z <= Bounds.z && V.z >= -Bounds.z);
	// ==================================================
	inline bool VECTOR4::InBounds(const VECTOR4& bounds) const
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat4(this);
		const XMVECTOR v2 = XMLoadFloat4(&bounds);
		return XMVector4InBounds(v1, v2);
	}

	// ==================================================
	// 4Dベクトルの長さ
	// ==================================================
	inline float VECTOR4::Length() const
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat4(this);
		const XMVECTOR X = XMVector4Length(v1);
		return XMVectorGetX(X);
	}
	// ==================================================
	// 4Dベクトルの長さの２乗
	// ==================================================
	inline constexpr float VECTOR4::LengthSq() const noexcept
	{
		return Dot(*this);
	}

	// ==================================================
	// 4Dベクトルの内積
	// ==================================================
	inline constexpr float VECTOR4::Dot(const VECTOR4& v) const noexcept
	{
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}

	// ==================================================
	// ベクトルを正規化
	// ==================================================
	inline VECTOR4 VECTOR4::Normalized() const
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat4(this);
		const XMVECTOR X = XMVector4Normalize(v1);
		VECTOR4 result;
		XMStoreFloat4(&result, X);
		return result;
	}
	//inline void VECTOR4::Normalized()
	//{
	//	using namespace DirectX;
	//	XMVECTOR v1 = XMLoadFloat4(this);
	//	XMVECTOR X = XMVector4Normalize(v1);
	//	XMStoreFloat4(this, X);
	//}
	inline void VECTOR4::Normalized(VECTOR4& result) const
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat4(this);
		const XMVECTOR X = XMVector4Normalize(v1);
		XMStoreFloat4(&result, X);
	}

	// ==================================================
	// [ vmin, vmax ]の範囲に収める
	// ==================================================
	inline void VECTOR4::Clamp(const VECTOR4& vmin, const VECTOR4& vmax)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat4(this);
		const XMVECTOR v2 = XMLoadFloat4(&vmin);
		const XMVECTOR v4 = XMLoadFloat4(&vmax);
		const XMVECTOR X = XMVectorClamp(v1, v2, v4);
		XMStoreFloat4(this, X);
	}
	inline void VECTOR4::Clamp(const VECTOR4& vmin, const VECTOR4& vmax, VECTOR4& result) const
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat4(this);
		const XMVECTOR v2 = XMLoadFloat4(&vmin);
		const XMVECTOR v4 = XMLoadFloat4(&vmax);
		const XMVECTOR X = XMVectorClamp(v1, v2, v4);
		XMStoreFloat4(&result, X);
	}

	// Static functions
	// ==================================================
	// 4Dベクトルの内積
	// ==================================================
	inline float VECTOR4::Dot(const VECTOR4& v1, const VECTOR4& v2)
	{
		using namespace DirectX;
		const XMVECTOR V1 = XMLoadFloat4(&v1);
		const XMVECTOR V2 = XMLoadFloat4(&v2);
		return XMVectorGetX(XMVector4Dot(V1, V2));
	}

	// ==================================================
	// 4Dベクトルの外積
	// ==================================================
	inline void VECTOR4::Cross(const VECTOR4& v1, const VECTOR4& v2, const VECTOR4& v3, VECTOR4& result)
	{
		using namespace DirectX;
		const XMVECTOR V1 = XMLoadFloat4(&v1);
		const XMVECTOR V2 = XMLoadFloat4(&v2);
		const XMVECTOR V3 = XMLoadFloat4(&v3);
		const XMVECTOR R = XMVector4Cross(V1, V2, V3);
		XMStoreFloat4(&result, R);
	}
	inline VECTOR4 VECTOR4::Cross(const VECTOR4& v1, const VECTOR4& v2, const VECTOR4& v3)
	{
		using namespace DirectX;
		const XMVECTOR V1 = XMLoadFloat4(&v1);
		const XMVECTOR V2 = XMLoadFloat4(&v2);
		const XMVECTOR V3 = XMLoadFloat4(&v3);
		const XMVECTOR R = XMVector4Cross(V1, V2, V3);
		VECTOR4 result;
		XMStoreFloat4(&result, R);
		return result;
	}

	// ==================================================
	// 2点間の距離
	// ==================================================
	inline float VECTOR4::Distance(const VECTOR4& v1, const VECTOR4& v2)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat4(&v1);
		const XMVECTOR x2 = XMLoadFloat4(&v2);
		const XMVECTOR V = XMVectorSubtract(x2, x1);
		const XMVECTOR X = XMVector4Length(V);
		return XMVectorGetX(X);
	}
	inline float VECTOR4::DistanceSq(const VECTOR4& v1, const VECTOR4& v2)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat4(&v1);
		const XMVECTOR x2 = XMLoadFloat4(&v2);
		const XMVECTOR V = XMVectorSubtract(x2, x1);
		const XMVECTOR X = XMVector4LengthSq(V);
		return XMVectorGetX(X);
	}

	// ==================================================
	// 2つのベクトルで各成分の一番小さな値を使用してベクトルを作成
	// ==================================================
	inline void VECTOR4::Min(const VECTOR4& v1, const VECTOR4& v2, VECTOR4& result)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat4(&v1);
		const XMVECTOR x2 = XMLoadFloat4(&v2);
		const XMVECTOR X = XMVectorMin(x1, x2);
		XMStoreFloat4(&result, X);
	}
	inline VECTOR4 VECTOR4::Min(const VECTOR4& v1, const VECTOR4& v2)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat4(&v1);
		const XMVECTOR x2 = XMLoadFloat4(&v2);
		const XMVECTOR X = XMVectorMin(x1, x2);

		VECTOR4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	// ==================================================
	// 2つのベクトルで各成分の一番大きな値を使用してベクトルを作成
	// ==================================================
	inline void VECTOR4::Max(const VECTOR4& v1, const VECTOR4& v2, VECTOR4& result)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat4(&v1);
		const XMVECTOR x2 = XMLoadFloat4(&v2);
		const XMVECTOR X = XMVectorMax(x1, x2);
		XMStoreFloat4(&result, X);
	}
	inline VECTOR4 VECTOR4::Max(const VECTOR4& v1, const VECTOR4& v2)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat4(&v1);
		const XMVECTOR x2 = XMLoadFloat4(&v2);
		const XMVECTOR X = XMVectorMax(x1, x2);

		VECTOR4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	// ==================================================
	// 2つのベクトル間の線形補間
	// ==================================================
	inline void VECTOR4::Lerp(const VECTOR4& v1, const VECTOR4& v2, float t, VECTOR4& result)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat4(&v1);
		const XMVECTOR x2 = XMLoadFloat4(&v2);
		const XMVECTOR X = XMVectorLerp(x1, x2, t);
		XMStoreFloat4(&result, X);
	}
	inline VECTOR4 VECTOR4::Lerp(const VECTOR4& v1, const VECTOR4& v2, float t)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat4(&v1);
		const XMVECTOR x2 = XMLoadFloat4(&v2);
		const XMVECTOR X = XMVectorLerp(x1, x2, t);

		VECTOR4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	// ==================================================
	// 2つのベクトル間のなめらかな遷移
	// ==================================================
	inline void VECTOR4::SmoothStep(const VECTOR4& v1, const VECTOR4& v2, float t, VECTOR4& result)
	{
		using namespace DirectX;
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t * (4.f - 2.f * t);
		const XMVECTOR x1 = XMLoadFloat4(&v1);
		const XMVECTOR x2 = XMLoadFloat4(&v2);
		const XMVECTOR X = XMVectorLerp(x1, x2, t);
		XMStoreFloat4(&result, X);
	}
	inline VECTOR4 VECTOR4::SmoothStep(const VECTOR4& v1, const VECTOR4& v2, float t)
	{
		using namespace DirectX;
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t * (4.f - 2.f * t);
		const XMVECTOR x1 = XMLoadFloat4(&v1);
		const XMVECTOR x2 = XMLoadFloat4(&v2);
		const XMVECTOR X = XMVectorLerp(x1, x2, t);

		VECTOR4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	// ==================================================
	// キャットムル-ロム スプライン曲線上の点
	// ==================================================
	inline void VECTOR4::CatmullRom(const VECTOR4& v1, const VECTOR4& v2, const VECTOR4& v3, const VECTOR4& v4, float t, VECTOR4& result)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat4(&v1);
		const XMVECTOR x2 = XMLoadFloat4(&v2);
		const XMVECTOR x3 = XMLoadFloat4(&v3);
		const XMVECTOR x4 = XMLoadFloat4(&v4);
		const XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);
		XMStoreFloat4(&result, X);
	}
	inline VECTOR4 VECTOR4::CatmullRom(const VECTOR4& v1, const VECTOR4& v2, const VECTOR4& v3, const VECTOR4& v4, float t)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat4(&v1);
		const XMVECTOR x2 = XMLoadFloat4(&v2);
		const XMVECTOR x3 = XMLoadFloat4(&v3);
		const XMVECTOR x4 = XMLoadFloat4(&v4);
		const XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);

		VECTOR4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	// ==================================================
	// エルミート スプライン補間(始点から終点まで、速度をt1->t2に変化して移動する)
	// ==================================================
	inline void VECTOR4::Hermite(const VECTOR4& v1, const VECTOR4& t1, const VECTOR4& v2, const VECTOR4& t2, float t, VECTOR4& result)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat4(&v1);
		const XMVECTOR x2 = XMLoadFloat4(&t1);
		const XMVECTOR x3 = XMLoadFloat4(&v2);
		const XMVECTOR x4 = XMLoadFloat4(&t2);
		const XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);
		XMStoreFloat4(&result, X);
	}
	inline VECTOR4 VECTOR4::Hermite(const VECTOR4& v1, const VECTOR4& t1, const VECTOR4& v2, const VECTOR4& t2, float t)
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat4(&v1);
		const XMVECTOR x2 = XMLoadFloat4(&t1);
		const XMVECTOR x3 = XMLoadFloat4(&v2);
		const XMVECTOR x4 = XMLoadFloat4(&t2);
		const XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);

		VECTOR4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	// ==================================================
	// 入射、法線ベクトルから反射ベクトルを求める
	// ==================================================
	inline void VECTOR4::Reflect(const VECTOR4& incident, const VECTOR4& normal, VECTOR4& result)
	{
		using namespace DirectX;
		const XMVECTOR i = XMLoadFloat4(&incident);
		const XMVECTOR n = XMLoadFloat4(&normal);
		const XMVECTOR X = XMVector4Reflect(i, n);
		XMStoreFloat4(&result, X);
	}
	inline VECTOR4 VECTOR4::Reflect(const VECTOR4& incident, const VECTOR4& normal)
	{
		using namespace DirectX;
		const XMVECTOR i = XMLoadFloat4(&incident);
		const XMVECTOR n = XMLoadFloat4(&normal);
		const XMVECTOR X = XMVector4Reflect(i, n);

		VECTOR4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	// ==================================================
	// 入射、法線ベクトルと屈折率から屈折ベクトルを求める
	// ==================================================
	inline void VECTOR4::Refract(const VECTOR4& incident, const VECTOR4& normal, float refractionIndex, VECTOR4& result)
	{
		using namespace DirectX;
		const XMVECTOR i = XMLoadFloat4(&incident);
		const XMVECTOR n = XMLoadFloat4(&normal);
		const XMVECTOR X = XMVector4Refract(i, n, refractionIndex);
		XMStoreFloat4(&result, X);
	}
	inline VECTOR4 VECTOR4::Refract(const VECTOR4& incident, const VECTOR4& normal, float refractionIndex)
	{
		using namespace DirectX;
		const XMVECTOR i = XMLoadFloat4(&incident);
		const XMVECTOR n = XMLoadFloat4(&normal);
		const XMVECTOR X = XMVector4Refract(i, n, refractionIndex);

		VECTOR4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	// ==================================================
	// クォータニオン回転、位置ベクトルと方向ベクトルの行列変換
	// ==================================================
	inline void VECTOR4::TransformRotate(const VECTOR2& v, const Quaternion& q, VECTOR4& result)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(&v);
		const XMVECTOR Q = XMLoadFloat4(&q);
		XMVECTOR X = XMVector3Rotate(v1, Q);
		X = XMVectorSelect(g_XMIdentityR3, X, g_XMSelect1110);	// result.w = 1.f
		XMStoreFloat4(&result, X);
	}
	inline VECTOR4 VECTOR4::TransformRotate(const VECTOR2& v, const Quaternion& q)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat2(&v);
		const XMVECTOR Q = XMLoadFloat4(&q);
		XMVECTOR X = XMVector3Rotate(v1, Q);
		X = XMVectorSelect(g_XMIdentityR3, X, g_XMSelect1110);	// result.w = 1.f

		VECTOR4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	inline void VECTOR4::TransformRotate(const VECTOR3& v, const Quaternion& q, VECTOR4& result)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat3(&v);
		const XMVECTOR Q = XMLoadFloat4(&q);
		XMVECTOR X = XMVector3Rotate(v1, Q);
		X = XMVectorSelect(g_XMIdentityR3, X, g_XMSelect1110);	// result.w = 1.f
		XMStoreFloat4(&result, X);
	}
	inline VECTOR4 VECTOR4::TransformRotate(const VECTOR3& v, const Quaternion& q)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat3(&v);
		const XMVECTOR Q = XMLoadFloat4(&q);
		XMVECTOR X = XMVector3Rotate(v1, Q);
		X = XMVectorSelect(g_XMIdentityR3, X, g_XMSelect1110);	// result.w = 1.f

		VECTOR4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	inline void VECTOR4::TransformRotate(const VECTOR4& v, const Quaternion& q, VECTOR4& result)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat4(&v);
		const XMVECTOR Q = XMLoadFloat4(&q);
		XMVECTOR X = XMVector3Rotate(v1, Q);
		X = XMVectorSelect(v1, X, g_XMSelect1110);	// result.w = v.w
		XMStoreFloat4(&result, X);
	}
	inline VECTOR4 VECTOR4::TransformRotate(const VECTOR4& v, const Quaternion& q)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat4(&v);
		const XMVECTOR Q = XMLoadFloat4(&q);
		XMVECTOR X = XMVector3Rotate(v1, Q);
		X = XMVectorSelect(v1, X, g_XMSelect1110);	// result.w = v.w

		VECTOR4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	inline void VECTOR4::TransformMatrix(const VECTOR4& v, const MATRIX& m, VECTOR4& result)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat4(&v);
		const XMMATRIX M = XMLoadFloat4x4(&m);
		const XMVECTOR X = XMVector4Transform(v1, M);
		XMStoreFloat4(&result, X);
	}
	inline VECTOR4 VECTOR4::TransformMatrix(const VECTOR4& v, const MATRIX& m)
	{
		using namespace DirectX;
		const XMVECTOR v1 = XMLoadFloat4(&v);
		const XMMATRIX M = XMLoadFloat4x4(&m);
		const XMVECTOR X = XMVector4Transform(v1, M);

		VECTOR4 result;
		XMStoreFloat4(&result, X);
		return result;
	}

	// Binary operators
	inline constexpr VECTOR4 operator+ (const VECTOR4& v1, const VECTOR4& v2) noexcept
	{
		return VECTOR4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
	}
	inline constexpr VECTOR4 operator- (const VECTOR4& v1, const VECTOR4& v2) noexcept
	{
		return VECTOR4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
	}
	inline constexpr VECTOR4 operator* (const VECTOR4& v1, const VECTOR4& v2) noexcept
	{
		return VECTOR4(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
	}
	inline constexpr VECTOR4 operator* (float s, const VECTOR4& v) noexcept
	{
		return VECTOR4(v.x * s, v.y * s, v.z * s, v.w * s);
	}
	inline constexpr VECTOR4 operator* (const VECTOR4& v, float s) noexcept
	{
		return VECTOR4(v.x * s, v.y * s, v.z * s, v.w * s);
	}
	inline constexpr VECTOR4 operator*(const VECTOR4& v, const MATRIX& m) noexcept
	{
		return VECTOR4(
			v.x * m._11 + v.y * m._21 + v.z * m._31 + v.w * m._41,
			v.x * m._12 + v.y * m._22 + v.z * m._32 + v.w * m._42,
			v.x * m._13 + v.y * m._23 + v.z * m._33 + v.w * m._43,
			v.x * m._14 + v.y * m._24 + v.z * m._34 + v.w * m._44
		);
	}
	inline constexpr VECTOR4 operator/ (const VECTOR4& v1, const VECTOR4& v2)
	{
		return VECTOR4(
			v1.x * MyMath::InvertDivisionValue(v2.x),
			v1.y * MyMath::InvertDivisionValue(v2.y),
			v1.z * MyMath::InvertDivisionValue(v2.z),
			v1.w * MyMath::InvertDivisionValue(v2.w)
		);
	}
	inline constexpr VECTOR4 operator/ (const VECTOR4& v, float s)
	{
		return v * MyMath::InvertDivisionValue(s);
	}
#pragma endregion


	// *****************************************************************************
	//
	//      MATRIX
	//
	//******************************************************************************
#pragma region MATRIX
	inline MATRIX::MATRIX(const DirectX::XMFLOAT3X3& M) noexcept
	{
		_11 = M._11; _12 = M._12; _13 = M._13; _14 = 0.f;
		_21 = M._21; _22 = M._22; _23 = M._23; _24 = 0.f;
		_31 = M._31; _32 = M._32; _33 = M._33; _34 = 0.f;
		_41 = 0.f;   _42 = 0.f;   _43 = 0.f;   _44 = 1.f;
	}
	inline MATRIX::MATRIX(const DirectX::XMFLOAT4X3& M) noexcept
	{
		_11 = M._11; _12 = M._12; _13 = M._13; _14 = 0.f;
		_21 = M._21; _22 = M._22; _23 = M._23; _24 = 0.f;
		_31 = M._31; _32 = M._32; _33 = M._33; _34 = 0.f;
		_41 = M._41; _42 = M._42; _43 = M._43; _44 = 1.f;
	}

	// Comparison operators
	inline bool MATRIX::operator == (const MATRIX& M) const noexcept
	{
		using namespace DirectX;
		const XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		const XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		const XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		const XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		const XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
		const XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
		const XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
		const XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

		const XMVECTOR epsilon = XMLoadFloat(&MyMath::Epsilon);
		return XMVector4NearEqual(x1, y1, epsilon)
			&& XMVector4NearEqual(x2, y2, epsilon)
			&& XMVector4NearEqual(x3, y3, epsilon)
			&& XMVector4NearEqual(x4, y4, epsilon);
	}
	inline bool MATRIX::operator != (const MATRIX& M) const noexcept
	{
		return !(*this == M);
	}

	// Assignment operators
	inline MATRIX& MATRIX::operator= (const DirectX::XMFLOAT3X3& M) noexcept
	{
		_11 = M._11; _12 = M._12; _13 = M._13; _14 = 0.f;
		_21 = M._21; _22 = M._22; _23 = M._23; _24 = 0.f;
		_31 = M._31; _32 = M._32; _33 = M._33; _34 = 0.f;
		_41 = 0.f;   _42 = 0.f;   _43 = 0.f;   _44 = 1.f;
		return *this;
	}
	inline MATRIX& MATRIX::operator= (const DirectX::XMFLOAT4X3& M) noexcept
	{
		_11 = M._11; _12 = M._12; _13 = M._13; _14 = 0.f;
		_21 = M._21; _22 = M._22; _23 = M._23; _24 = 0.f;
		_31 = M._31; _32 = M._32; _33 = M._33; _34 = 0.f;
		_41 = M._41; _42 = M._42; _43 = M._43; _44 = 1.f;
		return *this;
	}
	inline MATRIX& MATRIX::operator= (const DirectX::XMFLOAT4X4& M) noexcept
	{
		_11 = M._11; _12 = M._12; _13 = M._13; _14 = M._14;
		_21 = M._21; _22 = M._22; _23 = M._23; _24 = M._24;
		_31 = M._31; _32 = M._32; _33 = M._33; _34 = M._34;
		_41 = M._41; _42 = M._42; _43 = M._43; _44 = M._44;
		return *this;
	}
	inline MATRIX& MATRIX::operator+= (const MATRIX& M) noexcept
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

		x1 = XMVectorAdd(x1, y1);
		x2 = XMVectorAdd(x2, y2);
		x3 = XMVectorAdd(x3, y3);
		x4 = XMVectorAdd(x4, y4);

		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
		return *this;
	}
	inline MATRIX& MATRIX::operator-= (const MATRIX& M) noexcept
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

		x1 = XMVectorSubtract(x1, y1);
		x2 = XMVectorSubtract(x2, y2);
		x3 = XMVectorSubtract(x3, y3);
		x4 = XMVectorSubtract(x4, y4);

		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
		return *this;
	}
	inline MATRIX& MATRIX::operator*= (const MATRIX& M) noexcept
	{
		using namespace DirectX;
		const XMMATRIX M1 = XMLoadFloat4x4(this);
		const XMMATRIX M2 = XMLoadFloat4x4(&M);
		const XMMATRIX X = XMMatrixMultiply(M1, M2);
		XMStoreFloat4x4(this, X);
		return *this;
	}
	inline MATRIX& MATRIX::operator*= (float S) noexcept
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		x1 = XMVectorScale(x1, S);
		x2 = XMVectorScale(x2, S);
		x3 = XMVectorScale(x3, S);
		x4 = XMVectorScale(x4, S);

		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
		return *this;
	}
	inline MATRIX& MATRIX::operator/= (float S)
	{
		using namespace DirectX;
		assert(S != 0.f);
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		const float rs = 1.f / S;

		x1 = XMVectorScale(x1, rs);
		x2 = XMVectorScale(x2, rs);
		x3 = XMVectorScale(x3, rs);
		x4 = XMVectorScale(x4, rs);

		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
		return *this;
	}

	inline MATRIX& MATRIX::operator/= (const MATRIX& M) // Element-wise divide
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

		x1 = XMVectorDivide(x1, y1);
		x2 = XMVectorDivide(x2, y2);
		x3 = XMVectorDivide(x3, y3);
		x4 = XMVectorDivide(x4, y4);

		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
		return *this;
	}

	// Unary operators
	inline MATRIX MATRIX::operator- () const noexcept
	{
		using namespace DirectX;
		XMVECTOR v1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
		XMVECTOR v2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
		XMVECTOR v3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
		XMVECTOR v4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

		v1 = XMVectorNegate(v1);
		v2 = XMVectorNegate(v2);
		v3 = XMVectorNegate(v3);
		v4 = XMVectorNegate(v4);

		MATRIX R;
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), v1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), v2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), v3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), v4);
		return R;
	}

	// MATRIX operations
	inline bool MATRIX::Decompose(VECTOR3& scale, Quaternion& rotation, VECTOR3& translation) const
	{
		using namespace DirectX;

		XMVECTOR s, r, t;

		if (!XMMatrixDecompose(&s, &r, &t, *this))
			return false;

		XMStoreFloat3(&scale, s);
		XMStoreFloat4(&rotation, r);
		XMStoreFloat3(&translation, t);

		return true;
	}

	inline bool MATRIX::Decompose(_In_opt_ VECTOR3* scale, _In_opt_ Quaternion* rotation, _In_opt_ VECTOR3* translation) const
	{
		using namespace DirectX;

		XMVECTOR s, r, t;

		if (!XMMatrixDecompose(&s, &r, &t, *this))
			return false;

		if (scale)
			XMStoreFloat3(scale, s);
		if (rotation)
			XMStoreFloat4(rotation, r);
		if (translation)
			XMStoreFloat3(translation, t);

		return true;
	}

	inline MATRIX MATRIX::Transpose() const
	{
		using namespace DirectX;
		const XMMATRIX M = XMLoadFloat4x4(this);
		MATRIX R;
		XMStoreFloat4x4(&R, XMMatrixTranspose(M));
		return R;
	}
	inline void MATRIX::Transpose(MATRIX& result) const
	{
		using namespace DirectX;
		const XMMATRIX M = XMLoadFloat4x4(this);
		XMStoreFloat4x4(&result, XMMatrixTranspose(M));
	}

	inline MATRIX MATRIX::Invert() const
	{
		using namespace DirectX;
		const XMMATRIX M = XMLoadFloat4x4(this);
		MATRIX R;
		XMVECTOR det;
		XMStoreFloat4x4(&R, XMMatrixInverse(&det, M));
		return R;
	}
	inline void MATRIX::Invert(MATRIX& result) const
	{
		using namespace DirectX;
		const XMMATRIX M = XMLoadFloat4x4(this);
		XMVECTOR det;
		XMStoreFloat4x4(&result, XMMatrixInverse(&det, M));
	}

	inline VECTOR3 MATRIX::CreateEulerAngles() const
	{
		using namespace DirectX;
		VECTOR3 result;
		// _32が1を超えることがある
		const float _32_clamped = MyMath::Clamp(_32, -1.0f, 1.0f);
		// m._32 == 1.fの時
		if (fabsf(_32_clamped - 1.0f) < FLT_EPSILON)
		{
			result.x = -XM_PIDIV2;
			result.y = 0.0f;
			result.z = atan2(_21, _11);
		}
		// m._32 == -1.fの時
		else if (fabsf(_32_clamped + 1.0f) < FLT_EPSILON)
		{
			result.x = XM_PIDIV2;
			result.y = 0.0f;
			result.z = atan2(_21, _11);
		}
		else
		{
			result.x = asin(-_32_clamped);
			result.y = atan2(_31, _33);
			result.z = atan2(_12, _22);
		}
		result.x = XMConvertToDegrees(result.x);
		result.y = XMConvertToDegrees(result.y);
		result.z = XMConvertToDegrees(result.z);

		return result;
	}

	inline float MATRIX::Determinant() const
	{
		using namespace DirectX;
		const XMMATRIX M = XMLoadFloat4x4(this);
		return XMVectorGetX(XMMatrixDeterminant(M));
	}

	inline Quaternion MATRIX::ToQuaternion() const
	{
		using namespace DirectX;
		const XMMATRIX M = XMLoadFloat4x4(this);

		Quaternion result;
		XMStoreFloat4(&result, XMQuaternionRotationMatrix(M));
		result.Normalize();
		return result;
	}

	// Static functions
	inline MATRIX MATRIX::CreateBillboard(const VECTOR3& object, const VECTOR3& cameraPosition, const VECTOR3& cameraUp, _In_opt_ const VECTOR3* cameraForward)
	{
		using namespace DirectX;
		XMVECTOR O = XMLoadFloat3(&object);
		XMVECTOR C = XMLoadFloat3(&cameraPosition);
		XMVECTOR Z = XMVectorSubtract(O, C);

		XMVECTOR N = XMVector3LengthSq(Z);
		if (XMVector3Less(N, g_XMEpsilon))
		{
			if (cameraForward)
			{
				XMVECTOR F = XMLoadFloat3(cameraForward);
				Z = XMVectorNegate(F);
			}
			else
			{
				Z = g_XMNegIdentityR2;
			}
		}
		else
		{
			Z = XMVector3Normalize(Z);
		}

		XMVECTOR up = XMLoadFloat3(&cameraUp);
		XMVECTOR X = XMVector3Cross(up, Z);
		X = XMVector3Normalize(X);

		XMVECTOR Y = XMVector3Cross(Z, X);

		XMMATRIX M{};
		M.r[0] = X;
		M.r[1] = Y;
		M.r[2] = Z;
		M.r[3] = XMVectorSetW(O, 1.f);

		MATRIX R;
		XMStoreFloat4x4(&R, M);
		return R;
		}
	inline MATRIX MATRIX::CreateBillboard(const MATRIX& view)
	{
		using namespace DirectX;
		MATRIX result(view);
		result._41 = result._42 = result._43 = 0.f;
		const auto quat = CreateQuaternion(result);
		return quat.Inverse().ToRotationMatrix();
	}
	inline MATRIX MATRIX::CreateBillboardHorizontal(const MATRIX& view)
	{
		using namespace DirectX;
		MATRIX result(view);
		result._41 = result._42 = result._43 = 0.f;
		auto quat = CreateQuaternion(result);
		quat.y = 0.0f;
		quat.Normalize();
		return quat.Inverse().ToRotationMatrix();
	}
	inline MATRIX MATRIX::CreateBillboardVertical(const MATRIX& view)
	{
		using namespace DirectX;
		MATRIX result(view);
		result._41 = result._42 = result._43 = 0.f;
		auto quat = CreateQuaternion(result);
		quat.x = 0.0f;
		quat.z = 0.0f;
		quat.Normalize();
		return quat.Inverse().ToRotationMatrix();
	}

	inline MATRIX MATRIX::CreateTranslation(const VECTOR3& position)
	{
		using namespace DirectX;
		MATRIX R;
		XMStoreFloat4x4(&R, XMMatrixTranslation(position.x, position.y, position.z));
		return R;
	}
	inline MATRIX MATRIX::CreateTranslation(float x, float y, float z)
	{
		using namespace DirectX;
		MATRIX R;
		XMStoreFloat4x4(&R, XMMatrixTranslation(x, y, z));
		return R;
	}

	inline MATRIX MATRIX::CreateScale(const VECTOR3& scales)
	{
		using namespace DirectX;
		MATRIX R;
		XMStoreFloat4x4(&R, XMMatrixScaling(scales.x, scales.y, scales.z));
		return R;
	}
	inline MATRIX MATRIX::CreateScale(float xs, float ys, float zs)
	{
		using namespace DirectX;
		MATRIX R;
		XMStoreFloat4x4(&R, XMMatrixScaling(xs, ys, zs));
		return R;
	}
	inline MATRIX MATRIX::CreateScale(float scale)
	{
		using namespace DirectX;
		MATRIX R;
		XMStoreFloat4x4(&R, XMMatrixScaling(scale, scale, scale));
		return R;
	}

	inline MATRIX MATRIX::CreateRotationX(float angles)
	{
		using namespace DirectX;
		MATRIX R;
		XMStoreFloat4x4(&R, XMMatrixRotationX(XMConvertToRadians(angles)));
		return R;
	}
	inline MATRIX MATRIX::CreateRotationY(float angles)
	{
		using namespace DirectX;
		MATRIX R;
		XMStoreFloat4x4(&R, XMMatrixRotationY(XMConvertToRadians(angles)));
		return R;
	}
	inline MATRIX MATRIX::CreateRotationZ(float angles)
	{
		using namespace DirectX;
		MATRIX R;
		XMStoreFloat4x4(&R, XMMatrixRotationZ(XMConvertToRadians(angles)));
		return R;
	}

	inline MATRIX MATRIX::CreateFromAxisAngle(const VECTOR3& axis, float angle)
	{
		using namespace DirectX;
		MATRIX R;
		const XMVECTOR a = XMLoadFloat3(&axis);
		XMStoreFloat4x4(&R, XMMatrixRotationAxis(a, XMConvertToRadians(angle)));
		return R;
	}

	inline MATRIX MATRIX::CreatePerspectiveFieldOfView(float fov, float aspectRatio, float nearPlane, float farPlane)
	{
		using namespace DirectX;
		MATRIX R;
#ifdef AXIS_RH
		XMStoreFloat4x4(&R, XMMatrixPerspectiveFovRH(fov, aspectRatio, nearPlane, farPlane));
#else
		XMStoreFloat4x4(&R, XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane));
#endif// #ifdef AXIS_RH
		return R;
	}
	inline MATRIX MATRIX::CreatePerspective(float width, float height, float nearPlane, float farPlane)
	{
		using namespace DirectX;
		MATRIX R;
#ifdef AXIS_RH
		XMStoreFloat4x4(&R, XMMatrixPerspectiveRH(width, height, nearPlane, farPlane));
#else
		XMStoreFloat4x4(&R, XMMatrixPerspectiveLH(width, height, nearPlane, farPlane));
#endif// #ifdef AXIS_RH
		return R;
	}
	inline MATRIX MATRIX::CreatePerspectiveOffCenter(float left, float right, float bottom, float top, float nearPlane, float farPlane)
	{
		using namespace DirectX;
		MATRIX R;
#ifdef AXIS_RH
		XMStoreFloat4x4(&R, XMMatrixPerspectiveOffCenterRH(left, right, bottom, top, nearPlane, farPlane));
#else
		XMStoreFloat4x4(&R, XMMatrixPerspectiveOffCenterLH(left, right, bottom, top, nearPlane, farPlane));
#endif// #ifdef AXIS_RH
		return R;
	}
	inline MATRIX MATRIX::CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane)
	{
		using namespace DirectX;
		MATRIX R;
#ifdef AXIS_RH
		XMStoreFloat4x4(&R, XMMatrixOrthographicRH(width, height, zNearPlane, zFarPlane));
#else
		XMStoreFloat4x4(&R, XMMatrixOrthographicLH(width, height, zNearPlane, zFarPlane));
#endif// #ifdef AXIS_RH
		return R;
	}
	inline MATRIX MATRIX::CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane)
	{
		using namespace DirectX;
		MATRIX R;
#ifdef AXIS_RH
		XMStoreFloat4x4(&R, XMMatrixOrthographicOffCenterRH(left, right, bottom, top, zNearPlane, zFarPlane));
#else
		XMStoreFloat4x4(&R, XMMatrixOrthographicOffCenterLH(left, right, bottom, top, zNearPlane, zFarPlane));
#endif// #ifdef AXIS_RH
		return R;
	}

	inline MATRIX MATRIX::CreateLookAt(const VECTOR3& eye, const VECTOR3& target, const VECTOR3& up)
	{
		using namespace DirectX;
		MATRIX R;
		const XMVECTOR eyev = XMLoadFloat3(&eye);
		const XMVECTOR targetv = XMLoadFloat3(&target);
		const XMVECTOR upv = XMLoadFloat3(&up);
#ifdef AXIS_RH
		XMStoreFloat4x4(&R, XMMatrixLookAtRH(eyev, targetv, upv));
#else
		XMStoreFloat4x4(&R, XMMatrixLookAtLH(eyev, targetv, upv));
#endif// #ifdef AXIS_RH
		return R;
	}
	inline MATRIX MATRIX::CreateLookTo(const VECTOR3& eye, const VECTOR3& eye_dir, const VECTOR3& up_dir)
	{
		using namespace DirectX;
		MATRIX R;
		const XMVECTOR eyev = XMLoadFloat3(&eye);
		const XMVECTOR z_dirv = XMLoadFloat3(&eye_dir);
		const XMVECTOR y_dirv = XMLoadFloat3(&up_dir);
#ifdef AXIS_RH
		XMStoreFloat4x4(&R, XMMatrixLookToRH(eyev, z_dirv, y_dirv));
#else
		XMStoreFloat4x4(&R, XMMatrixLookToLH(eyev, z_dirv, y_dirv));
#endif// #ifdef AXIS_RH
		return R;
	}
	inline MATRIX MATRIX::CreateWorld(const VECTOR3& position, const Quaternion& quat, const VECTOR3& scale)
	{
		using namespace DirectX;
		MATRIX R;
		const XMVECTOR posv = XMLoadFloat3(&position);
		const XMVECTOR quatv = XMLoadFloat4(&quat);
		const XMVECTOR scalev = XMLoadFloat3(&scale);
		const XMVECTOR zerov = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&R, XMMatrixAffineTransformation(scalev, zerov, quatv, posv));
		return R;
	}
	inline MATRIX MATRIX::CreateWorld(const VECTOR3& position, const VECTOR3& forward, const VECTOR3& up)
	{
		using namespace DirectX;

		const XMVECTOR zaxis = XMVector3Normalize(XMLoadFloat3(&forward));

		XMVECTOR yaxis = XMLoadFloat3(&up);
		const XMVECTOR xaxis = XMVector3Normalize(XMVector3Cross(yaxis, zaxis));
		yaxis = XMVector3Cross(zaxis, xaxis);

		MATRIX R;
		XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._11), xaxis);
		XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._21), yaxis);
		XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._31), zaxis);
		R._14 = R._24 = R._34 = 0.f;
		R._41 = position.x; R._42 = position.y; R._43 = position.z;
		R._44 = 1.f;
		return R;
	}

	inline MATRIX MATRIX::CreateFromQuaternion(const Quaternion& quat)
	{
		using namespace DirectX;
		MATRIX R;
		const XMVECTOR quatv = XMLoadFloat4(&quat);
		XMStoreFloat4x4(&R, XMMatrixRotationQuaternion(quatv));
		return R;
	}

	inline Quaternion MATRIX::CreateQuaternion(const MATRIX& mat)
	{
		using namespace DirectX;
		const XMMATRIX M = XMLoadFloat4x4(&mat);

		Quaternion result;
		XMStoreFloat4(&result, XMQuaternionRotationMatrix(M));
		return result;
	}

	inline MATRIX MATRIX::CreateFromYawPitchRoll(float x, float y, float z)
	{
		using namespace DirectX;
		MATRIX R;
		XMStoreFloat4x4(&R, XMMatrixRotationRollPitchYaw(x, y, z));
		return R;
	}

	inline void MATRIX::Lerp(const MATRIX& M1, const MATRIX& M2, float t, MATRIX& result)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

		x1 = XMVectorLerp(x1, y1, t);
		x2 = XMVectorLerp(x2, y2, t);
		x3 = XMVectorLerp(x3, y3, t);
		x4 = XMVectorLerp(x4, y4, t);

		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._41), x4);
	}
	inline MATRIX MATRIX::Lerp(const MATRIX& M1, const MATRIX& M2, float t)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

		x1 = XMVectorLerp(x1, y1, t);
		x2 = XMVectorLerp(x2, y2, t);
		x3 = XMVectorLerp(x3, y3, t);
		x4 = XMVectorLerp(x4, y4, t);

		MATRIX result;
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._41), x4);
		return result;
	}

	inline void MATRIX::Transform(const MATRIX& M, const Quaternion& rotation, MATRIX& result)
	{
		using namespace DirectX;
		const XMVECTOR quatv = XMLoadFloat4(&rotation);

		const XMMATRIX M0 = XMLoadFloat4x4(&M);
		const XMMATRIX M1 = XMMatrixRotationQuaternion(quatv);

		XMStoreFloat4x4(&result, XMMatrixMultiply(M0, M1));
	}
	inline MATRIX MATRIX::Transform(const MATRIX& M, const Quaternion& rotation)
	{
		using namespace DirectX;
		const XMVECTOR quatv = XMLoadFloat4(&rotation);

		const XMMATRIX M0 = XMLoadFloat4x4(&M);
		const XMMATRIX M1 = XMMatrixRotationQuaternion(quatv);

		MATRIX result;
		XMStoreFloat4x4(&result, XMMatrixMultiply(M0, M1));
		return result;
	}

	// Binary operators
	inline MATRIX operator+ (const MATRIX& M1, const MATRIX& M2) noexcept
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

		x1 = XMVectorAdd(x1, y1);
		x2 = XMVectorAdd(x2, y2);
		x3 = XMVectorAdd(x3, y3);
		x4 = XMVectorAdd(x4, y4);

		MATRIX R;
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
		return R;
	}
	inline MATRIX operator- (const MATRIX& M1, const MATRIX& M2) noexcept
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

		x1 = XMVectorSubtract(x1, y1);
		x2 = XMVectorSubtract(x2, y2);
		x3 = XMVectorSubtract(x3, y3);
		x4 = XMVectorSubtract(x4, y4);

		MATRIX R;
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
		return R;
	}
	inline MATRIX operator* (const MATRIX& M1, const MATRIX& M2) noexcept
	{
		using namespace DirectX;
		const XMMATRIX m1 = XMLoadFloat4x4(&M1);
		const XMMATRIX m2 = XMLoadFloat4x4(&M2);
		const XMMATRIX X = XMMatrixMultiply(m1, m2);

		MATRIX R;
		XMStoreFloat4x4(&R, X);
		return R;
	}
	inline MATRIX operator* (const MATRIX& M, float S) noexcept
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

		x1 = XMVectorScale(x1, S);
		x2 = XMVectorScale(x2, S);
		x3 = XMVectorScale(x3, S);
		x4 = XMVectorScale(x4, S);

		MATRIX R;
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
		return R;
	}
	inline MATRIX operator/ (const MATRIX& M, float S)
	{
		using namespace DirectX;
		assert(S != 0.f);

		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

		const float rs = 1.f / S;

		x1 = XMVectorScale(x1, rs);
		x2 = XMVectorScale(x2, rs);
		x3 = XMVectorScale(x3, rs);
		x4 = XMVectorScale(x4, rs);

		MATRIX R;
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
		return R;
	}
	inline MATRIX operator/ (const MATRIX& M1, const MATRIX& M2)
	{
		using namespace DirectX;
		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

		x1 = XMVectorDivide(x1, y1);
		x2 = XMVectorDivide(x2, y2);
		x3 = XMVectorDivide(x3, y3);
		x4 = XMVectorDivide(x4, y4);

		MATRIX R;
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
		return R;
	}
	// Element-wise divide
	inline MATRIX operator* (float S, const MATRIX& M) noexcept
	{
		return M * S;
	}
#pragma endregion


	// *****************************************************************************
	//
	//      Quaternion
	//
	//******************************************************************************
#pragma region Quaternion
// Comparison operators
	inline bool Quaternion::operator == (const Quaternion& q) const noexcept
	{
		using namespace DirectX;
		const XMVECTOR q1 = XMLoadFloat4(this);
		const XMVECTOR q2 = XMLoadFloat4(&q);
		return XMQuaternionEqual(q1, q2);
	}
	inline bool Quaternion::operator != (const Quaternion& q) const noexcept
	{
		return !(*this == q);
	}

	// Assignment operators
	inline Quaternion& Quaternion::operator*= (const Quaternion& q) noexcept
	{
		using namespace DirectX;
		const XMVECTOR q1 = XMLoadFloat4(this);
		const XMVECTOR q2 = XMLoadFloat4(&q);
		XMStoreFloat4(this, XMQuaternionNormalize(XMQuaternionMultiply(q1, q2)));
		return *this;
	}
	inline Quaternion& Quaternion::operator*= (float s) noexcept
	{
		using namespace DirectX;
		const XMVECTOR q = XMLoadFloat4(this);
		XMStoreFloat4(this, XMVectorScale(q, s));
		return *this;
	}
	inline Quaternion& Quaternion::operator/= (const Quaternion& q) noexcept
	{
		using namespace DirectX;
		const XMVECTOR q1 = XMLoadFloat4(this);
		XMVECTOR q2 = XMLoadFloat4(&q);
		q2 = XMQuaternionInverse(q2);
		XMStoreFloat4(this, XMQuaternionNormalize(XMQuaternionMultiply(q1, q2)));
		return *this;
	}
	inline Quaternion& Quaternion::operator/= (float s) noexcept
	{
		using namespace DirectX;
		assert(s != 0.0f);
		const XMVECTOR q = XMLoadFloat4(this);
		s = 1.0f / s;
		XMStoreFloat4(this, XMVectorScale(q, s));
		return *this;
	}

	// Quaternion operations
	inline float Quaternion::Length() const
	{
		using namespace DirectX;
		const XMVECTOR q = XMLoadFloat4(this);
		return XMVectorGetX(XMQuaternionLength(q));
	}
	inline float Quaternion::LengthSq() const noexcept
	{
		using namespace DirectX;
		const XMVECTOR q = XMLoadFloat4(this);
		return XMVectorGetX(XMQuaternionLengthSq(q));
	}

	inline VECTOR3 Quaternion::ToEulerAngles() const
	{
		const MATRIX M = CreateRotationMatrix(*this);
		return M.CreateEulerAngles();
	}

	inline MATRIX Quaternion::ToRotationMatrix() const noexcept
	{
		using namespace DirectX;
		MATRIX result;
		const XMVECTOR quat = XMLoadFloat4(this);
		XMStoreFloat4x4(&result, XMMatrixRotationQuaternion(quat));
		return result;
	}

	inline void Quaternion::ToAxisAngle(VECTOR3& axis, float& angle) const
	{
		// https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/index.htm
		using namespace DirectX;
		Quaternion q;
		const XMVECTOR v_q = XMLoadFloat4(this);
		XMStoreFloat4(&q, XMQuaternionNormalize(v_q));
		angle = 2 * acosf(std::max(-1.0f, std::min(1.0f, q.w)));
		if (MyMath::Approximately(angle, 0.0f))
		{
			angle = 0.0f;
			axis = VECTOR3::Up;
		}
		else
		{
			const double inv_sqrt = 1.0 / sqrt(1.0 - static_cast<double>(q.w) * q.w);
			axis.x = static_cast<float>(q.x * inv_sqrt);
			axis.y = static_cast<float>(q.y * inv_sqrt);
			axis.z = static_cast<float>(q.z * inv_sqrt);
			axis = axis.Normalized();
		}
		angle = XMConvertToDegrees(angle);
	}

	inline Quaternion Quaternion::Normalize() const
	{
		using namespace DirectX;
		Quaternion ret;
		const XMVECTOR q = XMLoadFloat4(this);
		XMStoreFloat4(&ret, XMQuaternionNormalize(q));
		return ret;
	}
	inline void Quaternion::Normalize(Quaternion& result) const
	{
		using namespace DirectX;
		const XMVECTOR q = XMLoadFloat4(this);
		XMStoreFloat4(&result, XMQuaternionNormalize(q));
	}

	// 共役を求める
	inline Quaternion Quaternion::Conjugate() const
	{
		using namespace DirectX;
		Quaternion ret;
		XMStoreFloat4(&ret, XMQuaternionConjugate(XMLoadFloat4(this)));
		return ret;
	}
	inline void Quaternion::Conjugate(Quaternion& result) const
	{
		using namespace DirectX;
		XMStoreFloat4(&result, XMQuaternionConjugate(XMLoadFloat4(this)));
	}

	inline void Quaternion::Inverse(Quaternion& result) const
	{
		using namespace DirectX;
		XMStoreFloat4(&result, XMQuaternionInverse(XMLoadFloat4(this)));
	}
	inline Quaternion Quaternion::Inverse() const
	{
		using namespace DirectX;
		Quaternion result;
		XMStoreFloat4(&result, XMQuaternionInverse(XMLoadFloat4(this)));
		return result;
	}

	inline float Quaternion::Dot(const Quaternion& q) const
	{
		using namespace DirectX;
		const XMVECTOR q1 = XMLoadFloat4(this);
		const XMVECTOR q2 = XMLoadFloat4(&q);
		return XMVectorGetX(XMQuaternionDot(q1, q2));
	}

	inline bool Quaternion::Equals(const Quaternion& q) const noexcept
	{
		return (*this == q);
	}

	inline VECTOR3 Quaternion::Right() const
	{
		return ToRotationMatrix().Right();
	}

	inline VECTOR3 Quaternion::Up() const
	{
		return ToRotationMatrix().Up();
	}

	inline VECTOR3 Quaternion::Forward() const
	{
		return ToRotationMatrix().Forward();
	}

	inline void Quaternion::SetFromToRotation(const VECTOR3& from_direction, const VECTOR3& to_direction)
	{
		using namespace DirectX;
		// 2つのベクトルが等しいとき
		if (from_direction == to_direction)
		{
			*this = Quaternion::Identity;
			return;
		}
		const XMVECTOR from = XMVector3Normalize(XMLoadFloat3(&from_direction));
		const XMVECTOR to = XMVector3Normalize(XMLoadFloat3(&to_direction));
		// 方向ベクトルがゼロまたは平行な時回転しない
		const XMVECTOR epsilon = XMLoadFloat(&MyMath::Epsilon);
		if ((XMVector3NearEqual(from, XMVectorZero(), epsilon)) || (XMVector3NearEqual(to, XMVectorZero(), epsilon)))
		{
			*this = Quaternion::Identity;
			return;
		}
		// 回転軸を求める
		VECTOR3 f_cross_t = VECTOR3::Cross(from_direction, to_direction);
		// 外積が0
		if (f_cross_t == VECTOR3::Zero)
		{
			// 回転軸がないため垂直なものを見つける
			f_cross_t = VECTOR3::Cross(VECTOR3::Forward, from_direction);
			if (f_cross_t == VECTOR3::Zero)
			{
				f_cross_t = VECTOR3::Cross(VECTOR3::Right, from_direction);
			}
		}
		XMVECTOR axis = XMLoadFloat3(&f_cross_t);

		// 軸に沿って回転する量を求める
		const float angle = VECTOR3::Angle(from_direction, to_direction);

		*this = AxisAngle(axis, angle);
	}
	inline void Quaternion::SetLookRotation(const VECTOR3& view, const VECTOR3& up)
	{
		using namespace DirectX;

		const XMVECTOR F = XMVector3Normalize(XMLoadFloat3(&view));	// forward

		XMVECTOR U = XMVector3Normalize(XMLoadFloat3(&up));
		const XMVECTOR epsilon = XMLoadFloat(&MyMath::Epsilon);
		// forwardがupwardsと一致するとき
		if (XMVector3NearEqual(F, U, epsilon))
		{
			// upwardがUpのとき
			if (XMVector3NearEqual(U, XMLoadFloat3(&VECTOR3::Up), epsilon))
			{
				U = XMVector3Normalize(XMLoadFloat3(&VECTOR3::Backward));
			}
			// upwardがUpでないならUpをいれる
			else
			{
				U = XMVector3Normalize(XMLoadFloat3(&VECTOR3::Up));
			}
		}
		const XMVECTOR R = XMVector3Normalize(XMVector3Cross(U, F));		// right
		U = XMVector3Normalize(XMVector3Cross(F, R));				// up

		const MATRIX M(R, U, F);
		*this = CreateFromRotationMatrix(M);
	}

	// Static functions
	// angles[°]
	inline float Quaternion::Angle(const Quaternion& q1, const Quaternion& q2)
	{
		using namespace DirectX;
		const XMVECTOR Q1 = XMQuaternionNormalize(XMLoadFloat4(&q1));
		const XMVECTOR Q2 = XMQuaternionNormalize(XMLoadFloat4(&q2));
		float param = XMVectorGetX(XMQuaternionDot(Q1, Q2));
		param = std::max(-1.0f, std::min(1.0f, param));
		return XMConvertToDegrees(acosf(param) * 2.f);
	}

	// euler-->Quaternion
	inline Quaternion Quaternion::EulerAngles(const VECTOR3& euler)
	{
		using namespace DirectX;
		Quaternion result;
		XMStoreFloat4(&result, XMQuaternionRotationRollPitchYaw(XMConvertToRadians(euler.x), XMConvertToRadians(euler.y), XMConvertToRadians(euler.z)));
		return result;
	}
	inline Quaternion Quaternion::EulerAngles(float x, float y, float z)
	{
		using namespace DirectX;
		Quaternion result;
		XMStoreFloat4(&result, XMQuaternionRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z)));
		return result;
	}

	// 内積(-1から1の範囲)
	inline float Quaternion::Dot(const Quaternion& q1, const Quaternion& q2)
	{
		using namespace DirectX;
		const XMVECTOR Q1 = XMLoadFloat4(&q1);
		const XMVECTOR Q2 = XMLoadFloat4(&q2);
		return XMVectorGetX(XMQuaternionDot(Q1, Q2));
	}

	// 軸に対して回転
	inline Quaternion Quaternion::AxisAngle(const VECTOR3& axis, float angle)
	{
		using namespace DirectX;
		if (MyMath::Approximately(angle, 0.0f))
		{
			return Quaternion::Identity;
		}

		const XMVECTOR a = XMLoadFloat3(&axis);

		Quaternion result;
		XMStoreFloat4(&result, XMQuaternionNormalize(XMQuaternionRotationAxis(a, XMConvertToRadians(angle))));
		return result;
	}

	// X、Y、Z軸それぞれの角度からクォータニオンを生成(z->x->y)
	inline Quaternion Quaternion::EulerToQuaternion(const VECTOR3& angles)
	{
		using namespace DirectX;
		Quaternion result;
		XMStoreFloat4(&result, XMQuaternionNormalize(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(angles.x), XMConvertToRadians(angles.y), XMConvertToRadians(angles.z))));
		return result;
	}
	inline Quaternion Quaternion::EulerToQuaternion(float x, float y, float z)
	{
		using namespace DirectX;
		Quaternion result;
		XMStoreFloat4(&result, XMQuaternionNormalize(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z))));
		return result;
	}

	// 回転行列からクォータニオンを生成
	inline Quaternion Quaternion::CreateFromRotationMatrix(const MATRIX& m)
	{
		using namespace DirectX;
		const XMMATRIX M = XMLoadFloat4x4(&m);
		Quaternion result;
		XMStoreFloat4(&result, XMQuaternionNormalize(XMQuaternionRotationMatrix(M)));
		return result;
	}

	// クォータニオンから回転行列を生成
	inline MATRIX Quaternion::CreateRotationMatrix(const Quaternion& q)
	{
		using namespace DirectX;
		MATRIX result;
		const XMVECTOR quat = XMLoadFloat4(&q);
		XMStoreFloat4x4(&result, XMMatrixRotationQuaternion(quat));
		//MATRIX result = {
		//	1.f - 2.f * q.y * q.y - 2.f * q.z * q.z	,2.f * q.x * q.y + 2.f * q.w * q.z			,2.f * q.x * q.z - 2.f * q.w * q.y			,0.f,
		//	2.f * q.x * q.y - 2.f * q.w * q.z		,1.f - 2.f * q.x * q.x - 2.f * q.z * q.z	,2.f * q.y * q.z + 2.f * q.w * q.x			,0.f,
		//	2.f * q.x * q.z + 2.f * q.w * q.y		,2.f * q.y * q.z - 2.f * q.w * q.x			,1.f - 2.f * q.x * q.x - 2.f * q.y * q.y	,0.f,
		//	0.f										,0.f										,0.f										,1.0f
		//};
		return result;
	}

	// From-->Toに回転するクォータニオンを生成
	inline Quaternion Quaternion::FromToRotation(const VECTOR3& from_direction, const VECTOR3& to_direction)
	{
		using namespace DirectX;
		// 2つのベクトルが等しいとき
		if (from_direction == to_direction)
		{
			return Quaternion::Identity;
		}
		const XMVECTOR from = XMVector3Normalize(XMLoadFloat3(&from_direction));
		const XMVECTOR to = XMVector3Normalize(XMLoadFloat3(&to_direction));
		const XMVECTOR epsilon = XMLoadFloat(&MyMath::Epsilon);
		// 方向ベクトルがゼロまたは平行な時回転しない
		if ((XMVector3NearEqual(from, XMVectorZero(), epsilon)) || (XMVector3NearEqual(to, XMVectorZero(), epsilon)))
		{
			return Quaternion::Identity;
		}
		// 回転軸を求める
		VECTOR3 f_cross_t = VECTOR3::Cross(from_direction, to_direction);
		// 外積が0
		if (f_cross_t == VECTOR3::Zero)
		{
			// 回転軸がないため垂直なものを見つける
			f_cross_t = VECTOR3::Cross(VECTOR3::Forward, from_direction);
			if (f_cross_t == VECTOR3::Zero)
			{
				f_cross_t = VECTOR3::Cross(VECTOR3::Right, from_direction);
			}
		}
		XMVECTOR axis = XMLoadFloat3(&f_cross_t);

		// 軸に沿って回転する量を求める
		const float angle = VECTOR3::Angle(from_direction, to_direction);

		return AxisAngle(axis, angle);
	}

	// forwardを正面に、upwards寄りの上向きをするクォータニオンを生成
	inline Quaternion Quaternion::LookRotation(const VECTOR3& forward, const VECTOR3& upward)
	{
		using namespace DirectX;

		const XMVECTOR F = XMVector3Normalize(XMLoadFloat3(&forward));	// forward
		
		XMVECTOR U = XMVector3Normalize(XMLoadFloat3(&upward));
		const XMVECTOR epsilon = XMLoadFloat(&MyMath::Epsilon);
		// forwardがupwardsと一致するとき
		if (XMVector3NearEqual(F, U, epsilon))
		{
			// upwardがUpのとき
			if (XMVector3NearEqual(U, XMLoadFloat3(&VECTOR3::Up), epsilon))
			{
				U = XMVector3Normalize(XMLoadFloat3(&VECTOR3::Backward));
			}
			// upwardがUpでないならUpをいれる
			else
			{
				U = XMVector3Normalize(XMLoadFloat3(&VECTOR3::Up));
			}
		}
		const XMVECTOR R = XMVector3Normalize(XMVector3Cross(U, F));		// right
		U = XMVector3Normalize(XMVector3Cross(F, R));				// up

		const MATRIX M(R, U, F);
		return CreateFromRotationMatrix(M);
	}

	// 線形補間
	inline void Quaternion::Lerp(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& result)
	{
		using namespace DirectX;
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);
		const XMVECTOR Q0 = XMLoadFloat4(&q1);
		const XMVECTOR Q1 = XMLoadFloat4(&q2);

		const XMVECTOR dot = XMVector4Dot(Q0, Q1);

		XMVECTOR R;
		if (XMVector4GreaterOrEqual(dot, XMVectorZero()))
		{
			R = XMVectorLerp(Q0, Q1, t);
		}
		else
		{
			const XMVECTOR tv = XMVectorReplicate(t);
			const XMVECTOR t1v = XMVectorReplicate(1.f - t);
			const XMVECTOR X0 = XMVectorMultiply(Q0, t1v);
			const XMVECTOR X1 = XMVectorMultiply(Q1, tv);
			R = XMVectorSubtract(X0, X1);
		}

		XMStoreFloat4(&result, XMQuaternionNormalize(R));
	}
	inline Quaternion Quaternion::Lerp(const Quaternion& q1, const Quaternion& q2, float t)
	{
		using namespace DirectX;
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);
		XMVECTOR Q0 = XMLoadFloat4(&q1);
		XMVECTOR Q1 = XMLoadFloat4(&q2);

		XMVECTOR dot = XMVector4Dot(Q0, Q1);

		XMVECTOR R;
		if (XMVector4GreaterOrEqual(dot, XMVectorZero()))
		{
			R = XMVectorLerp(Q0, Q1, t);
		}
		else
		{
			XMVECTOR tv = XMVectorReplicate(t);
			XMVECTOR t1v = XMVectorReplicate(1.f - t);
			XMVECTOR X0 = XMVectorMultiply(Q0, t1v);
			XMVECTOR X1 = XMVectorMultiply(Q1, tv);
			R = XMVectorSubtract(X0, X1);
		}

		Quaternion result;
		XMStoreFloat4(&result, XMQuaternionNormalize(R));
		return result;
	}
	inline void Quaternion::LerpUnclamped(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& result)
	{
		using namespace DirectX;
		const XMVECTOR Q0 = XMLoadFloat4(&q1);
		const XMVECTOR Q1 = XMLoadFloat4(&q2);

		const XMVECTOR dot = XMVector4Dot(Q0, Q1);

		XMVECTOR R;
		if (XMVector4GreaterOrEqual(dot, XMVectorZero()))
		{
			R = XMVectorLerp(Q0, Q1, t);
		}
		else
		{
			const XMVECTOR tv = XMVectorReplicate(t);
			const XMVECTOR t1v = XMVectorReplicate(1.f - t);
			const XMVECTOR X0 = XMVectorMultiply(Q0, t1v);
			const XMVECTOR X1 = XMVectorMultiply(Q1, tv);
			R = XMVectorSubtract(X0, X1);
		}

		XMStoreFloat4(&result, XMQuaternionNormalize(R));
	}
	inline Quaternion Quaternion::LerpUnclamped(const Quaternion& q1, const Quaternion& q2, float t)
	{
		using namespace DirectX;
		XMVECTOR Q0 = XMLoadFloat4(&q1);
		XMVECTOR Q1 = XMLoadFloat4(&q2);

		XMVECTOR dot = XMVector4Dot(Q0, Q1);

		XMVECTOR R;
		if (XMVector4GreaterOrEqual(dot, XMVectorZero()))
		{
			R = XMVectorLerp(Q0, Q1, t);
		}
		else
		{
			XMVECTOR tv = XMVectorReplicate(t);
			XMVECTOR t1v = XMVectorReplicate(1.f - t);
			XMVECTOR X0 = XMVectorMultiply(Q0, t1v);
			XMVECTOR X1 = XMVectorMultiply(Q1, tv);
			R = XMVectorSubtract(X0, X1);
		}

		Quaternion result;
		XMStoreFloat4(&result, XMQuaternionNormalize(R));
		return result;
	}

	// 球面補間
	inline void Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& result)
	{
		using namespace DirectX;
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);
		const XMVECTOR Q0 = XMLoadFloat4(&q1);
		const XMVECTOR Q1 = XMLoadFloat4(&q2);
		XMStoreFloat4(&result, XMQuaternionNormalize(XMQuaternionSlerp(Q0, Q1, t)));
	}
	inline Quaternion Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2, float t)
	{
		using namespace DirectX;
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);
		const XMVECTOR Q0 = XMLoadFloat4(&q1);
		const XMVECTOR Q1 = XMLoadFloat4(&q2);

		Quaternion result;
		XMStoreFloat4(&result, XMQuaternionNormalize(XMQuaternionSlerp(Q0, Q1, t)));
		return result;
	}
	inline void Quaternion::SlerpUnclamped(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& result)
	{
		using namespace DirectX;
		const XMVECTOR Q0 = XMLoadFloat4(&q1);
		const XMVECTOR Q1 = XMLoadFloat4(&q2);
		XMStoreFloat4(&result, XMQuaternionNormalize(XMQuaternionSlerp(Q0, Q1, t)));
	}
	inline Quaternion Quaternion::SlerpUnclamped(const Quaternion& q1, const Quaternion& q2, float t)
	{
		using namespace DirectX;
		const XMVECTOR Q0 = XMLoadFloat4(&q1);
		const XMVECTOR Q1 = XMLoadFloat4(&q2);

		Quaternion result;
		XMStoreFloat4(&result, XMQuaternionNormalize(XMQuaternionSlerp(Q0, Q1, t)));
		return result;
	}

	// 3次補間
	inline void Quaternion::SmoothStep(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& result)
	{
		using namespace DirectX;
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);
		t = t * t * (3.f - 2.f * t);
		const XMVECTOR Q0 = XMLoadFloat4(&q1);
		const XMVECTOR Q1 = XMLoadFloat4(&q2);
		XMStoreFloat4(&result, XMQuaternionNormalize(XMQuaternionSlerp(Q0, Q1, t)));
	}
	inline Quaternion Quaternion::SmoothStep(const Quaternion& q1, const Quaternion& q2, float t)
	{
		using namespace DirectX;
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);
		t = t * t * (3.f - 2.f * t);
		const XMVECTOR Q0 = XMLoadFloat4(&q1);
		const XMVECTOR Q1 = XMLoadFloat4(&q2);

		Quaternion result;
		XMStoreFloat4(&result, XMQuaternionNormalize(XMQuaternionSlerp(Q0, Q1, t)));
		return result;
	}

	// 連結する
	inline void Quaternion::Concatenate(const Quaternion& q1, const Quaternion& q2, Quaternion& result)
	{
		using namespace DirectX;
		const XMVECTOR Q0 = XMLoadFloat4(&q1);
		const XMVECTOR Q1 = XMLoadFloat4(&q2);
		XMStoreFloat4(&result, XMQuaternionNormalize(XMQuaternionMultiply(Q0, Q1)));
	}
	inline Quaternion Quaternion::Concatenate(const Quaternion& q1, const Quaternion& q2)
	{
		using namespace DirectX;
		const XMVECTOR Q0 = XMLoadFloat4(&q1);
		const XMVECTOR Q1 = XMLoadFloat4(&q2);

		Quaternion result;
		XMStoreFloat4(&result, XMQuaternionNormalize(XMQuaternionMultiply(Q0, Q1)));
		return result;
	}

	// Binary operators
	inline Quaternion operator* (const Quaternion& q1, const Quaternion& q2)
	{
		using namespace DirectX;
		const XMVECTOR Q1 = XMLoadFloat4(&q1);
		const XMVECTOR Q2 = XMLoadFloat4(&q2);

		Quaternion result;
		XMStoreFloat4(&result, XMQuaternionNormalize(XMQuaternionMultiply(Q1, Q2)));
		return result;
	}
	inline VECTOR3 operator* (const VECTOR3& point, const Quaternion& rot)
	{
		using namespace DirectX;
		const XMVECTOR P = XMLoadFloat3(&point);
		const XMVECTOR Q = XMLoadFloat4(&rot);

		VECTOR3 result;
		XMStoreFloat3(&result, XMVector3Rotate(P, Q));
		return result;
	}
	inline Quaternion operator* (const Quaternion& q, float s)
	{
		using namespace DirectX;
		const XMVECTOR Q = XMLoadFloat4(&q);

		Quaternion result;
		XMStoreFloat4(&result, XMVectorScale(Q, s));
		return result;
	}
	inline Quaternion operator* (float s, const Quaternion& q)
	{
		using namespace DirectX;
		const XMVECTOR Q = XMLoadFloat4(&q);

		Quaternion result;
		XMStoreFloat4(&result, XMVectorScale(Q, s));
		return result;
	}
	inline Quaternion operator/ (const Quaternion& q1, const Quaternion& q2)
	{
		using namespace DirectX;
		const XMVECTOR Q1 = XMLoadFloat4(&q1);
		const XMVECTOR Q2 = XMLoadFloat4(&q2);
		Quaternion result;
		XMStoreFloat4(&result, XMQuaternionNormalize(XMQuaternionMultiply(Q1, XMQuaternionInverse(Q2))));
		return result;
	}
	inline Quaternion operator/ (const Quaternion& q, float s)
	{
		using namespace DirectX;
		assert(s != 0.0f);
		const XMVECTOR Q = XMLoadFloat4(&q);

		Quaternion result;
		s = 1.f / s;
		XMStoreFloat4(&result, XMVectorScale(Q, s));
		return result;
	}
	}// namespace TKGEngine

		// C++ Library containers
namespace std
{
	template<> struct less<TKGEngine::VECTOR2>
	{
		constexpr bool operator()(const TKGEngine::VECTOR2& V1, const TKGEngine::VECTOR2& V2) const
		{
			return ((V1.x < V2.x) || ((V1.x == V2.x) && (V1.y < V2.y)));
		}
	};

	template<> struct less<TKGEngine::VECTOR3>
	{
		constexpr bool operator()(const TKGEngine::VECTOR3& V1, const TKGEngine::VECTOR3& V2) const
		{
			return ((V1.x < V2.x)
				|| ((V1.x == V2.x) && (V1.y < V2.y))
				|| ((V1.x == V2.x) && (V1.y == V2.y) && (V1.z < V2.z)));
		}
	};

	template<> struct less<TKGEngine::VECTOR4>
	{
		constexpr bool operator()(const TKGEngine::VECTOR4& V1, const TKGEngine::VECTOR4& V2) const
		{
			return ((V1.x < V2.x)
				|| ((V1.x == V2.x) && (V1.y < V2.y))
				|| ((V1.x == V2.x) && (V1.y == V2.y) && (V1.z < V2.z))
				|| ((V1.x == V2.x) && (V1.y == V2.y) && (V1.z == V2.z) && (V1.w < V2.w)));
		}
	};

	template<> struct less<TKGEngine::MATRIX>
	{
		constexpr bool operator()(const TKGEngine::MATRIX& M1, const TKGEngine::MATRIX& M2) const
		{
			if (M1._11 != M2._11) return M1._11 < M2._11;
			if (M1._12 != M2._12) return M1._12 < M2._12;
			if (M1._13 != M2._13) return M1._13 < M2._13;
			if (M1._14 != M2._14) return M1._14 < M2._14;
			if (M1._21 != M2._21) return M1._21 < M2._21;
			if (M1._22 != M2._22) return M1._22 < M2._22;
			if (M1._23 != M2._23) return M1._23 < M2._23;
			if (M1._24 != M2._24) return M1._24 < M2._24;
			if (M1._31 != M2._31) return M1._31 < M2._31;
			if (M1._32 != M2._32) return M1._32 < M2._32;
			if (M1._33 != M2._33) return M1._33 < M2._33;
			if (M1._34 != M2._34) return M1._34 < M2._34;
			if (M1._41 != M2._41) return M1._41 < M2._41;
			if (M1._42 != M2._42) return M1._42 < M2._42;
			if (M1._43 != M2._43) return M1._43 < M2._43;
			if (M1._44 != M2._44) return M1._44 < M2._44;

			return false;
		}
	};

	template<> struct less<TKGEngine::Quaternion>
	{
		bool operator()(const TKGEngine::Quaternion& Q1, const TKGEngine::Quaternion& Q2) const
		{
			return ((Q1.x < Q2.x)
				|| ((Q1.x == Q2.x) && (Q1.y < Q2.y))
				|| ((Q1.x == Q2.x) && (Q1.y == Q2.y) && (Q1.z < Q2.z))
				|| ((Q1.x == Q2.x) && (Q1.y == Q2.y) && (Q1.z == Q2.z) && (Q1.w < Q2.w)));
		}
	};

}
#pragma endregion