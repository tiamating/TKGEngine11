#pragma once

#include "Systems/inc/TKGEngine_Defined.h"

#include "myfunc_math.h"

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include <cassert>
#include <memory>
#include <functional>

namespace TKGEngine
{
	class VECTOR2;
	class VECTOR3;
	class VECTOR4;
	class MATRIX;
	class Quaternion;

	//==============================================================================
	//
	//      VECTOR2クラス
	//
	//==============================================================================
#pragma region VECTOR2
	class VECTOR2 : public DirectX::XMFLOAT2
	{
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(x),
					CEREAL_NVP(y)
				);
			}
		}

	public:
		constexpr VECTOR2() noexcept : XMFLOAT2(0.f, 0.f) {}
		constexpr explicit VECTOR2(float _x) noexcept : XMFLOAT2(_x, _x) {}
		constexpr VECTOR2(float _x, float _y) noexcept : XMFLOAT2(_x, _y) {}
		VECTOR2(DirectX::FXMVECTOR v) noexcept { XMStoreFloat2(this, v); }
		VECTOR2(const XMFLOAT2& v) noexcept : XMFLOAT2(v.x, v.y) {}
		explicit VECTOR2(const DirectX::XMFLOAT3& v) noexcept : XMFLOAT2(v.x, v.y) {}
		explicit VECTOR2(const DirectX::XMFLOAT4& v) noexcept : XMFLOAT2(v.x, v.y) {}
		explicit VECTOR2(const DirectX::XMVECTORF32& f) { this->x = f.f[0]; this->y = f.f[1]; }

		VECTOR2(const VECTOR2&) noexcept = default;
		VECTOR2& operator=(const VECTOR2&) noexcept = default;

		VECTOR2(VECTOR2&&) noexcept = default;
		VECTOR2& operator=(VECTOR2&&) noexcept = default;

		operator DirectX::XMVECTOR() const noexcept { return DirectX::XMLoadFloat2(this); }

		~VECTOR2() = default;


		// Comparison operators
		bool operator==(const VECTOR2& v) const noexcept;
		bool operator!=(const VECTOR2& v) const noexcept;


		// Assignment operators
		VECTOR2& operator=(const DirectX::XMVECTORF32& f) noexcept { this->x = f.f[0]; this->y = f.f[1]; return *this; }
		VECTOR2& operator+=(const VECTOR2& v) noexcept;
		VECTOR2& operator-=(const VECTOR2& v) noexcept;
		VECTOR2& operator*=(const VECTOR2& v) noexcept;
		VECTOR2& operator*=(float s) noexcept;
		VECTOR2& operator/=(float s);


		// Unary operators
		constexpr VECTOR2 operator+() const noexcept { return *this; }
		constexpr VECTOR2 operator-() const noexcept { return VECTOR2(-x, -y); }


		// Vector operations
		// ==================================================
		// 2Dベクトルのコンポーネントが設定された境界内にあるかどうかをテストする
		// 同値 : return (V.x <= Bounds.x && V.x >= -Bounds.x) && (V.y <= Bounds.y && V.y >= -Bounds.y);
		// ==================================================
		bool InBounds(const VECTOR2& bounds) const;

		// ==================================================
		// 2Dベクトルの長さ
		// ==================================================
		float Length() const;
		// ==================================================
		// 2Dベクトルの長さの２乗
		// ==================================================
		constexpr float LengthSq() const noexcept;

		// ==================================================
		// 2Dベクトルの内積
		// ==================================================
		constexpr float Dot(const VECTOR2& v) const noexcept;

		// ==================================================
		// 2Dベクトルの外積
		// ==================================================
		VECTOR3 Cross(const VECTOR2& v) const;

		// ==================================================
		// ベクトルを正規化
		// ==================================================
		VECTOR2 Normalized() const;
		void Normalized(VECTOR2& result) const;

		// ==================================================
		// [ vmin, vmax ]の範囲に収める
		// ==================================================
		void Clamp(const VECTOR2& vmin, const VECTOR2& vmax);
		void Clamp(const VECTOR2& vmin, const VECTOR2& vmax, VECTOR2& result) const;


		// Static functions
		// ==================================================
		// ベクトルのなす角
		// ==================================================
		static float Angle(const VECTOR2& v1, const VECTOR2& v2);

		// ==================================================
		// 2Dベクトルの内積
		// ==================================================
		static float Dot(const VECTOR2& v1, const VECTOR2& v2);

		// ==================================================
		// 2Dベクトルの外積
		// ==================================================
		static void Cross(const VECTOR2& v1, const VECTOR2& v2, VECTOR3& result);
		static VECTOR3 Cross(const VECTOR2& v1, const VECTOR2& v);

		// ==================================================
		// 2点間の距離
		// ==================================================
		static float Distance(const VECTOR2& v1, const VECTOR2& v2);
		static float DistanceSq(const VECTOR2& v1, const VECTOR2& v2);

		// ==================================================
		// 2つのベクトルで各成分の一番小さな値を使用してベクトルを作成
		// ==================================================
		static void Min(const VECTOR2& v1, const VECTOR2& v2, VECTOR2& result);
		static VECTOR2 Min(const VECTOR2& v1, const VECTOR2& v2);

		// ==================================================
		// 2つのベクトルで各成分の一番大きな値を使用してベクトルを作成
		// ==================================================
		static void Max(const VECTOR2& v1, const VECTOR2& v2, VECTOR2& result);
		static VECTOR2 Max(const VECTOR2& v1, const VECTOR2& v2);

		// ==================================================
		// 2つのベクトル間の線形補間
		// ==================================================
		static void Lerp(const VECTOR2& v1, const VECTOR2& v2, float t, VECTOR2& result);
		static VECTOR2 Lerp(const VECTOR2& v1, const VECTOR2& v2, float t);

		// ==================================================
		// 2つのベクトル間のなめらかな遷移
		// ==================================================
		static void SmoothStep(const VECTOR2& v1, const VECTOR2& v2, float t, VECTOR2& result);
		static VECTOR2 SmoothStep(const VECTOR2& v1, const VECTOR2& v2, float t);

		// ==================================================
		// キャットムル-ロム スプライン曲線上の点
		// ==================================================
		static void CatmullRom(const VECTOR2& v1, const VECTOR2& v2, const VECTOR2& v3, const VECTOR2& v4, float t, VECTOR2& result);
		static VECTOR2 CatmullRom(const VECTOR2& v1, const VECTOR2& v2, const VECTOR2& v3, const VECTOR2& v4, float t);

		// ==================================================
		// エルミート スプライン補間(始点から終点まで、速度をt1->t2に変化して移動する)
		// ==================================================
		static void Hermite(const VECTOR2& v1, const VECTOR2& t1, const VECTOR2& v2, const VECTOR2& t2, float t, VECTOR2& result);
		static VECTOR2 Hermite(const VECTOR2& v1, const VECTOR2& t1, const VECTOR2& v2, const VECTOR2& t2, float t);

		// ==================================================
		// 入射、法線ベクトルから反射ベクトルを求める
		// ==================================================
		static void Reflect(const VECTOR2& incident, const VECTOR2& normal, VECTOR2& result);
		static VECTOR2 Reflect(const VECTOR2& incident, const VECTOR2& normal);

		// ==================================================
		// 入射、法線ベクトルと屈折率から屈折ベクトルを求める
		// ==================================================
		static void Refract(const VECTOR2& incident, const VECTOR2& normal, float refractionIndex, VECTOR2& result);
		static VECTOR2 Refract(const VECTOR2& incident, const VECTOR2& normal, float refractionIndex);

		// ==================================================
		// クォータニオン回転、位置ベクトルと方向ベクトルの行列変換
		// ==================================================
		static void TransformRotate(const VECTOR2& v, const Quaternion& q, VECTOR2& result);
		static VECTOR2 TransformRotate(const VECTOR2& v, const Quaternion& q);

		static void TransformMatrix(const VECTOR2& v, const MATRIX& m, VECTOR2& result);
		static VECTOR2 TransformMatrix(const VECTOR2& v, const MATRIX& m);

		static void TransformNormal(const VECTOR2& v, const MATRIX& m, VECTOR2& result);
		static VECTOR2 TransformNormal(const VECTOR2& v, const MATRIX& m);


		// Constants
		static const VECTOR2 Zero;
		static const VECTOR2 One;
		static const VECTOR2 UnitX;
		static const VECTOR2 UnitY;

	};
	// Binary operators(二項演算子)
	inline constexpr VECTOR2 operator+ (const VECTOR2& v1, const VECTOR2& v2) noexcept;
	inline constexpr VECTOR2 operator- (const VECTOR2& v1, const VECTOR2& v2) noexcept;
	inline constexpr VECTOR2 operator* (const VECTOR2& v1, const VECTOR2& v2) noexcept;
	inline constexpr VECTOR2 operator* (const VECTOR2& v, const float s) noexcept;
	inline constexpr VECTOR2 operator* (const float s, const VECTOR2& v) noexcept;
	inline constexpr VECTOR2 operator/ (const VECTOR2& v1, const VECTOR2& v2);
	inline constexpr VECTOR2 operator/ (const VECTOR2& v, const float s);
#pragma endregion



	//==============================================================================
	//
	//      VECTOR3クラス
	//
	//==============================================================================
#pragma region VECTOR3
	class VECTOR3 : public DirectX::XMFLOAT3
	{
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(x),
					CEREAL_NVP(y),
					CEREAL_NVP(z)
				);
			}
		}

	public:
		constexpr VECTOR3() noexcept : XMFLOAT3(0.f, 0.f, 0.f) {}
		constexpr explicit VECTOR3(float _x) noexcept : XMFLOAT3(_x, _x, _x) {}
		constexpr VECTOR3(float _x, float _y, float _z) noexcept : XMFLOAT3(_x, _y, _z) {}
		VECTOR3(DirectX::FXMVECTOR v) noexcept { DirectX::XMStoreFloat3(this, v); }
		VECTOR3(const DirectX::XMFLOAT2& v, float z) noexcept : XMFLOAT3(v.x, v.y, z) {}
		explicit VECTOR3(const DirectX::XMFLOAT2& v) noexcept : XMFLOAT3(v.x, v.y, 0.0f) {}
		VECTOR3(const XMFLOAT3& v) noexcept : XMFLOAT3(v.x, v.y, v.z) {}
		explicit VECTOR3(const DirectX::XMFLOAT4& v) noexcept : XMFLOAT3(v.x, v.y, v.z) {}
		explicit VECTOR3(const DirectX::XMVECTORF32& f) noexcept { this->x = f.f[0]; this->y = f.f[1]; this->z = f.f[2]; }

		VECTOR3(const VECTOR3&) noexcept = default;
		VECTOR3& operator=(const VECTOR3&) noexcept = default;

		VECTOR3(VECTOR3&&) noexcept = default;
		VECTOR3& operator=(VECTOR3&&) noexcept = default;

		// w = 0
		operator DirectX::XMVECTOR() const noexcept { return DirectX::XMLoadFloat3(this); }

		~VECTOR3() = default;

		// Comparison operators
		bool operator==(const VECTOR3& v) const noexcept;
		bool operator!=(const VECTOR3& v) const noexcept;


		// Assignment operators
		VECTOR3& operator=(const DirectX::XMVECTORF32& f) noexcept { this->x = f.f[0]; this->y = f.f[1]; this->z = f.f[2]; return *this; }
		//VECTOR3& operator=(const VECTOR4& v) { this->x = v.x; this->y = v.y; this->z = v.z; }
		VECTOR3& operator+=(const VECTOR3& v) noexcept;
		VECTOR3& operator-=(const VECTOR3& v) noexcept;
		VECTOR3& operator*=(const VECTOR3& v) noexcept;
		VECTOR3& operator*=(float s) noexcept;
		VECTOR3& operator/=(float s);


		// Unary operators
		constexpr VECTOR3 operator+() const noexcept { return *this; }
		constexpr VECTOR3 operator-() const noexcept { return VECTOR3(-x, -y, -z); }


		// Vector operations
		// ==================================================
		// 3Dベクトルのコンポーネントが設定された境界内にあるかどうかをテストする
		// 同値 : return (V.x <= Bounds.x && V.x >= -Bounds.x) && (V.y <= Bounds.y && V.y >= -Bounds.y) && (V.z <= Bounds.z && V.z >= -Bounds.z);
		// ==================================================
		bool InBounds(const VECTOR3& bounds) const;

		// ==================================================
		// 3Dベクトルの長さ
		// ==================================================
		float Length() const;
		// ==================================================
		// 3Dベクトルの長さの２乗
		// ==================================================
		constexpr float LengthSq() const noexcept;

		// ==================================================
		// 3Dベクトルの内積
		// ==================================================
		constexpr float Dot(const VECTOR3& v) const noexcept;

		// ==================================================
		// 3Dベクトルの外積
		// ==================================================
		VECTOR3 Cross(const VECTOR3& v) const;

		// ==================================================
		// ベクトルを正規化
		// ==================================================
		VECTOR3 Normalized() const;
		void Normalized(VECTOR3& result) const;

		// ==================================================
		// [ vmin, vmax ]の範囲に収める
		// ==================================================
		void Clamp(const VECTOR3& vmin, const VECTOR3& vmax);
		void Clamp(const VECTOR3& vmin, const VECTOR3& vmax, VECTOR3& result) const;

		// ==================================================
		// オイラー角からクォータニオンを生成
		// ==================================================
		Quaternion ToQuaternion() const;


		// Static functions
		// ==================================================
		// ベクトルのなす角
		// ==================================================
		static float Angle(const VECTOR3& v1, const VECTOR3& v2);

		// ==================================================
		// 3Dベクトルの内積
		// ==================================================
		static float Dot(const VECTOR3& v1, const VECTOR3& v2);

		// ==================================================
		// 3Dベクトルの外積
		// ==================================================
		static void Cross(const VECTOR3& v1, const VECTOR3& v2, VECTOR3& result);
		static VECTOR3 Cross(const VECTOR3& v1, const VECTOR3& v2);

		// ==================================================
		// 2点間の距離
		// ==================================================
		static float Distance(const VECTOR3& v1, const VECTOR3& v2);
		static float DistanceSq(const VECTOR3& v1, const VECTOR3& v2);

		// ==================================================
		// 2つのベクトルで各成分の一番小さな値を使用してベクトルを作成
		// ==================================================
		static void Min(const VECTOR3& v1, const VECTOR3& v2, VECTOR3& result);
		static VECTOR3 Min(const VECTOR3& v1, const VECTOR3& v2);

		// ==================================================
		// 2つのベクトルで各成分の一番大きな値を使用してベクトルを作成
		// ==================================================
		static void Max(const VECTOR3& v1, const VECTOR3& v2, VECTOR3& result);
		static VECTOR3 Max(const VECTOR3& v1, const VECTOR3& v2);

		// ==================================================
		// 2つのベクトル間の線形補間
		// ==================================================
		static void Lerp(const VECTOR3& v1, const VECTOR3& v2, float t, VECTOR3& result);
		static VECTOR3 Lerp(const VECTOR3& v1, const VECTOR3& v2, float t);

		// ==================================================
		// 2つのベクトル間の球面補間
		// ==================================================
		static void Slerp(const VECTOR3& v1, const VECTOR3& v2, float t, VECTOR3& result);
		static VECTOR3 Slerp(const VECTOR3& v1, const VECTOR3& v2, float t);

		// ==================================================
		// 2つのベクトル間のなめらかな遷移
		// ==================================================
		static void SmoothStep(const VECTOR3& v1, const VECTOR3& v2, float t, VECTOR3& result);
		static VECTOR3 SmoothStep(const VECTOR3& v1, const VECTOR3& v2, float t);

		// ==================================================
		// キャットムル-ロム スプライン曲線上の点
		// ==================================================
		static void CatmullRom(const VECTOR3& v1, const VECTOR3& v2, const VECTOR3& v3, const VECTOR3& v4, float t, VECTOR3& result);
		static VECTOR3 CatmullRom(const VECTOR3& v1, const VECTOR3& v2, const VECTOR3& v3, const VECTOR3& v4, float t);

		// ==================================================
		// エルミート スプライン補間(始点から終点まで、速度をt1->t2に変化して移動する)
		// ==================================================
		static void Hermite(const VECTOR3& v1, const VECTOR3& t1, const VECTOR3& v2, const VECTOR3& t2, float t, VECTOR3& result);
		static VECTOR3 Hermite(const VECTOR3& v1, const VECTOR3& t1, const VECTOR3& v2, const VECTOR3& t2, float t);

		// ==================================================
		// 入射、法線ベクトルから反射ベクトルを求める
		// ==================================================
		static void Reflect(const VECTOR3& incident, const VECTOR3& normal, VECTOR3& result);
		static VECTOR3 Reflect(const VECTOR3& incident, const VECTOR3& normal);

		// ==================================================
		// 入射、法線ベクトルと屈折率から屈折ベクトルを求める
		// ==================================================
		static void Refract(const VECTOR3& incident, const VECTOR3& normal, float refractionIndex, VECTOR3& result);
		static VECTOR3 Refract(const VECTOR3& incident, const VECTOR3& normal, float refractionIndex);

		// ==================================================
		// クォータニオン回転、位置ベクトルと方向ベクトルの行列変換
		// ==================================================
		static void TransformRotate(const VECTOR2& v, const Quaternion& q, VECTOR3& result);
		static VECTOR3 TransformRotate(const VECTOR2& v, const Quaternion& q);

		static void TransformRotate(const VECTOR3& v, const Quaternion& q, VECTOR3& result);
		static VECTOR3 TransformRotate(const VECTOR3& v, const Quaternion& q);

		// rotation, scaling, translation
		static void TransformMatrix(const VECTOR3& v, const MATRIX& m, VECTOR3& result);
		static VECTOR3 TransformMatrix(const VECTOR3& v, const MATRIX& m);

		// rotation, scaling
		static void TransformNormal(const VECTOR3& v, const MATRIX& m, VECTOR3& result);
		static VECTOR3 TransformNormal(const VECTOR3& v, const MATRIX& m);

		// 回転行列からオイラー角を生成
		static VECTOR3 CreateEulerAngles(const MATRIX& m);

		// Constants
		static const VECTOR3 Zero;
		static const VECTOR3 One;
		static const VECTOR3 UnitX;
		static const VECTOR3 UnitY;
		static const VECTOR3 UnitZ;
		static const VECTOR3 Up;
		static const VECTOR3 Down;
		static const VECTOR3 Right;
		static const VECTOR3 Left;
		static const VECTOR3 Forward;
		static const VECTOR3 Backward;
	};

	// Binary operators
	inline constexpr VECTOR3 operator+ (const VECTOR3& v1, const VECTOR3& v2) noexcept;
	inline constexpr VECTOR3 operator- (const VECTOR3& v1, const VECTOR3& v2) noexcept;
	inline constexpr VECTOR3 operator* (const VECTOR3& v1, const VECTOR3& v2) noexcept;
	inline constexpr VECTOR3 operator* (const VECTOR3& v, float s) noexcept;
	inline constexpr VECTOR3 operator* (float s, const VECTOR3& v) noexcept;
	inline constexpr VECTOR3 operator* (const VECTOR3& v, const MATRIX& mat) noexcept;
	inline constexpr VECTOR3 operator/ (const VECTOR3& v1, const VECTOR3& v2);
	inline constexpr VECTOR3 operator/ (const VECTOR3& v, float s);
#pragma endregion



	//==============================================================================
	//
	//      VECTOR4クラス
	//
	//==============================================================================
#pragma region VECTOR4
	class VECTOR4 : public DirectX::XMFLOAT4
	{
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(x),
					CEREAL_NVP(y),
					CEREAL_NVP(z),
					CEREAL_NVP(w)
				);
			}
		}

	public:
		constexpr VECTOR4() noexcept : XMFLOAT4(0.f, 0.f, 0.f, 0.f) {}
		constexpr explicit VECTOR4(float _x) noexcept : XMFLOAT4(_x, _x, _x, _x) {}
		constexpr VECTOR4(float _x, float _y, float _z, float _w) noexcept : XMFLOAT4(_x, _y, _z, _w) {}
		VECTOR4(DirectX::FXMVECTOR v) noexcept { XMStoreFloat4(this, v); }
		VECTOR4(const DirectX::XMFLOAT3& v, float w) noexcept : XMFLOAT4(v.x, v.y, v.z, w) {}
		explicit VECTOR4(const DirectX::XMFLOAT2& v) noexcept : XMFLOAT4(v.x, v.y, 0.0f, 0.0f) {}
		explicit VECTOR4(const DirectX::XMFLOAT3& v) noexcept : XMFLOAT4(v.x, v.y, v.z, 0.0f) {}
		VECTOR4(const XMFLOAT4& v) noexcept : XMFLOAT4(v.x, v.y, v.z, v.w) {}
		explicit VECTOR4(const DirectX::XMVECTORF32& f) noexcept { this->x = f.f[0]; this->y = f.f[1]; this->z = f.f[2]; this->w = f.f[3]; }

		VECTOR4(const VECTOR4&) noexcept = default;
		VECTOR4& operator=(const VECTOR4&) noexcept = default;

		VECTOR4(VECTOR4&&) noexcept = default;
		VECTOR4& operator=(VECTOR4&&) noexcept = default;
		
		operator DirectX::XMVECTOR() const noexcept { return DirectX::XMLoadFloat4(this); }

		~VECTOR4() = default;

		// Comparison operators
		bool operator==(const VECTOR4& v) const noexcept;
		bool operator!=(const VECTOR4& v) const noexcept;


		// Assignment operators
		VECTOR4& operator=(const DirectX::XMVECTORF32& f) noexcept { this->x = f.f[0]; this->y = f.f[1]; this->z = f.f[2]; this->w = f.f[3]; return *this; }
		VECTOR4& operator+=(const VECTOR4& v) noexcept;
		VECTOR4& operator-=(const VECTOR4& v) noexcept;
		VECTOR4& operator*=(const VECTOR4& v) noexcept;
		VECTOR4& operator*=(float s) noexcept;
		VECTOR4& operator/=(float s);


		// Unary operators
		constexpr VECTOR4 operator+() const noexcept { return *this; }
		constexpr VECTOR4 operator-() const noexcept { return VECTOR4(-x, -y, -z, -w); }


		// Vector operations
		// ==================================================
		// 4Dベクトルのコンポーネントが設定された境界内にあるかどうかをテストする
		// ==================================================
		bool InBounds(const VECTOR4& bounds) const;

		// ==================================================
		// 4Dベクトルの長さ
		// ==================================================
		float Length() const;
		// ==================================================
		// 4Dベクトルの長さの２乗
		// ==================================================
		constexpr float LengthSq() const noexcept;

		// ==================================================
		// 4Dベクトルの内積
		// ==================================================
		constexpr float Dot(const VECTOR4& v) const noexcept;

		// ==================================================
		// ベクトルを正規化
		// ==================================================
		VECTOR4 Normalized() const;
		//void Normalized();
		void Normalized(VECTOR4& result) const;

		// ==================================================
		// [ vmin, vmax ]の範囲に収める
		// ==================================================
		void Clamp(const VECTOR4& vmin, const VECTOR4& vmax);
		void Clamp(const VECTOR4& vmin, const VECTOR4& vmax, VECTOR4& result) const;


		// Static functions
		// ==================================================
		// 4Dベクトルの内積
		// ==================================================
		static float Dot(const VECTOR4& v1, const VECTOR4& v2);

		// ==================================================
		// 4Dベクトルの外積
		// ==================================================
		static void Cross(const VECTOR4& v1, const VECTOR4& v2, const VECTOR4& v3, VECTOR4& result);
		static VECTOR4 Cross(const VECTOR4& v1, const VECTOR4& v2, const VECTOR4& v3);

		// ==================================================
		// 2点間の距離
		// ==================================================
		static float Distance(const VECTOR4& v1, const VECTOR4& v2);
		static float DistanceSq(const VECTOR4& v1, const VECTOR4& v2);

		// ==================================================
		// 2つのベクトルで各成分の一番小さな値を使用してベクトルを作成
		// ==================================================
		static void Min(const VECTOR4& v1, const VECTOR4& v2, VECTOR4& result);
		static VECTOR4 Min(const VECTOR4& v1, const VECTOR4& v2);

		// ==================================================
		// 2つのベクトルで各成分の一番大きな値を使用してベクトルを作成
		// ==================================================
		static void Max(const VECTOR4& v1, const VECTOR4& v2, VECTOR4& result);
		static VECTOR4 Max(const VECTOR4& v1, const VECTOR4& v2);

		// ==================================================
		// 2つのベクトル間の線形補間
		// ==================================================
		static void Lerp(const VECTOR4& v1, const VECTOR4& v2, float t, VECTOR4& result);
		static VECTOR4 Lerp(const VECTOR4& v1, const VECTOR4& v2, float t);

		// ==================================================
		// 2つのベクトル間のなめらかな遷移
		// ==================================================
		static void SmoothStep(const VECTOR4& v1, const VECTOR4& v2, float t, VECTOR4& result);
		static VECTOR4 SmoothStep(const VECTOR4& v1, const VECTOR4& v2, float t);

		// ==================================================
		// キャットムル-ロム スプライン曲線上の点
		// ==================================================
		static void CatmullRom(const VECTOR4& v1, const VECTOR4& v2, const VECTOR4& v3, const VECTOR4& v4, float t, VECTOR4& result);
		static VECTOR4 CatmullRom(const VECTOR4& v1, const VECTOR4& v2, const VECTOR4& v3, const VECTOR4& v4, float t);

		// ==================================================
		// エルミート スプライン補間(始点から終点まで、速度をt1->t2に変化して移動する)
		// ==================================================
		static void Hermite(const VECTOR4& v1, const VECTOR4& t1, const VECTOR4& v2, const VECTOR4& t2, float t, VECTOR4& result);
		static VECTOR4 Hermite(const VECTOR4& v1, const VECTOR4& t1, const VECTOR4& v2, const VECTOR4& t2, float t);

		// ==================================================
		// 入射、法線ベクトルから反射ベクトルを求める
		// ==================================================
		static void Reflect(const VECTOR4& incident, const VECTOR4& normal, VECTOR4& result);
		static VECTOR4 Reflect(const VECTOR4& incident, const VECTOR4& normal);

		// ==================================================
		// 入射、法線ベクトルと屈折率から屈折ベクトルを求める
		// ==================================================
		static void Refract(const VECTOR4& incident, const VECTOR4& normal, float refractionIndex, VECTOR4& result);
		static VECTOR4 Refract(const VECTOR4& incident, const VECTOR4& normal, float refractionIndex);

		// ==================================================
		// クォータニオン回転、位置ベクトルと方向ベクトルの行列変換
		// ==================================================
		static void TransformRotate(const VECTOR2& v, const Quaternion& q, VECTOR4& result);
		static VECTOR4 TransformRotate(const VECTOR2& v, const Quaternion& q);

		static void TransformRotate(const VECTOR3& v, const Quaternion& q, VECTOR4& result);
		static VECTOR4 TransformRotate(const VECTOR3& v, const Quaternion& q);

		static void TransformRotate(const VECTOR4& v, const Quaternion& q, VECTOR4& result);
		static VECTOR4 TransformRotate(const VECTOR4& v, const Quaternion& q);


		static void TransformMatrix(const VECTOR4& v, const MATRIX& m, VECTOR4& result);
		static VECTOR4 TransformMatrix(const VECTOR4& v, const MATRIX& m);

		
		// Constants
		static const VECTOR4 Zero;
		static const VECTOR4 One;
		static const VECTOR4 UnitX;
		static const VECTOR4 UnitY;
		static const VECTOR4 UnitZ;
		static const VECTOR4 UnitW;
	};

	// Binary operators
	inline constexpr VECTOR4 operator+ (const VECTOR4& v1, const VECTOR4& v2) noexcept;
	inline constexpr VECTOR4 operator- (const VECTOR4& v1, const VECTOR4& v2) noexcept;
	inline constexpr VECTOR4 operator* (const VECTOR4& v1, const VECTOR4& v2) noexcept;
	inline constexpr VECTOR4 operator* (float s, const VECTOR4& v) noexcept;
	inline constexpr VECTOR4 operator* (const VECTOR4& v, float s) noexcept;
	inline constexpr VECTOR4 operator* (const VECTOR4& v, const MATRIX& m) noexcept;
	inline constexpr VECTOR4 operator/ (const VECTOR4& v1, const VECTOR4& v2);
	inline constexpr VECTOR4 operator/ (const VECTOR4& v, float s);
#pragma endregion



	// =============================================================
	// 
	//                     MATRIX
	// 
	// =============================================================
#pragma region MATRIX
	class MATRIX : public DirectX::XMFLOAT4X4
	{
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(_11), CEREAL_NVP(_12), CEREAL_NVP(_13), CEREAL_NVP(_14),
					CEREAL_NVP(_21), CEREAL_NVP(_22), CEREAL_NVP(_23), CEREAL_NVP(_24),
					CEREAL_NVP(_31), CEREAL_NVP(_32), CEREAL_NVP(_33), CEREAL_NVP(_34),
					CEREAL_NVP(_41), CEREAL_NVP(_42), CEREAL_NVP(_43), CEREAL_NVP(_44)
				);
			}
		}

	public:
		MATRIX() noexcept
			: XMFLOAT4X4(1.f, 0, 0, 0,
				0, 1.f, 0, 0,
				0, 0, 1.f, 0,
				0, 0, 0, 1.f) {}
		constexpr MATRIX(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33) noexcept
			: XMFLOAT4X4(m00, m01, m02, m03,
				m10, m11, m12, m13,
				m20, m21, m22, m23,
				m30, m31, m32, m33) {}
		explicit MATRIX(const VECTOR3& axis_x, const VECTOR3& axis_y, const VECTOR3& axis_z) noexcept
			: XMFLOAT4X4(axis_x.x, axis_x.y, axis_x.z, 0,
				axis_y.x, axis_y.y, axis_y.z, 0,
				axis_z.x, axis_z.y, axis_z.z, 0,
				0, 0, 0, 1.f) {}
		explicit MATRIX(const VECTOR4& r0, const VECTOR4& r1, const VECTOR4& r2, const VECTOR4& r3) noexcept
			: XMFLOAT4X4(r0.x, r0.y, r0.z, r0.w,
				r1.x, r1.y, r1.z, r1.w,
				r2.x, r2.y, r2.z, r2.w,
				r3.x, r3.y, r3.z, r3.w) {}
		MATRIX(const XMFLOAT4X4& M) noexcept { memcpy_s(this, sizeof(float) * 16, &M, sizeof(XMFLOAT4X4)); }
		MATRIX(const DirectX::XMFLOAT3X3& M) noexcept;
		MATRIX(const DirectX::XMFLOAT4X3& M) noexcept;

		MATRIX(DirectX::CXMMATRIX M) noexcept { XMStoreFloat4x4(this, M); }

		MATRIX(const MATRIX&) noexcept = default;
		MATRIX& operator=(const MATRIX&) noexcept = default;

		MATRIX(MATRIX&&) = default;
		MATRIX& operator=(MATRIX&&) = default;

		operator DirectX::XMMATRIX() const noexcept { return XMLoadFloat4x4(this); }

		~MATRIX() = default;

		// Comparison operators
		bool operator == (const MATRIX& M) const noexcept;
		bool operator != (const MATRIX& M) const noexcept;

		// Assignment operators
		MATRIX& operator= (const DirectX::XMFLOAT3X3& M) noexcept;
		MATRIX& operator= (const DirectX::XMFLOAT4X3& M) noexcept;
		MATRIX& operator= (const DirectX::XMFLOAT4X4& M) noexcept;
		MATRIX& operator+= (const MATRIX& M) noexcept;
		MATRIX& operator-= (const MATRIX& M) noexcept;
		MATRIX& operator*= (const MATRIX& M) noexcept;
		MATRIX& operator*= (float S) noexcept;
		MATRIX& operator/= (float S);

		MATRIX& operator/= (const MATRIX& M);
		// Element-wise divide

		// Unary operators
		constexpr MATRIX operator+ () const noexcept { return *this; }
		MATRIX operator- () const noexcept;

		// Properties
		constexpr const VECTOR3 Up() const noexcept { return VECTOR3(_21, _22, _23); }
		void Up(const VECTOR3& v) noexcept { _21 = v.x; _22 = v.y; _23 = v.z; }

		constexpr const VECTOR3 Down() const noexcept { return VECTOR3(-_21, -_22, -_23); }
		void Down(const VECTOR3& v) noexcept { _21 = -v.x; _22 = -v.y; _23 = -v.z; }

		constexpr const VECTOR3 Right() const noexcept { return VECTOR3(_11, _12, _13); }
		void Right(const VECTOR3& v) noexcept { _11 = v.x; _12 = v.y; _13 = v.z; }

		constexpr const VECTOR3 Left() const noexcept { return VECTOR3(-_11, -_12, -_13); }
		void Left(const VECTOR3& v) noexcept { _11 = -v.x; _12 = -v.y; _13 = -v.z; }

		constexpr const VECTOR3 Forward() const noexcept { return VECTOR3(_31, _32, _33); }
		void Forward(const VECTOR3& v) noexcept { _31 = v.x; _32 = v.y; _33 = v.z; }

		constexpr const VECTOR3 Backward() const noexcept { return VECTOR3(-_31, -_32, -_33); }
		void Backward(const VECTOR3& v) noexcept { _31 = -v.x; _32 = -v.y; _33 = -v.z; }

		constexpr const VECTOR3 Translation() const noexcept { return VECTOR3(_41, _42, _43); }
		void Translation(const VECTOR3& v) noexcept { _41 = v.x; _42 = v.y; _43 = v.z; }

		// MATRIX operations

		// Decompose the transformation matrix
		bool Decompose(VECTOR3& scale, Quaternion& rotation, VECTOR3& translation) const;
		bool Decompose(_In_opt_ VECTOR3* scale, _In_opt_ Quaternion* rotation, _In_opt_ VECTOR3* translation) const;

		MATRIX Transpose() const;
		void Transpose(MATRIX& result) const;

		MATRIX Invert() const;
		void Invert(MATRIX& result) const;

		// 回転行列からオイラー角を生成
		VECTOR3 CreateEulerAngles() const;

		// 行列式
		float Determinant() const;

		Quaternion ToQuaternion() const;

		// Static functions
		static MATRIX CreateBillboard(const VECTOR3& object, const VECTOR3& cameraPosition, const VECTOR3& cameraUp, _In_opt_ const VECTOR3* cameraForward = nullptr);
		// cancel view matrix rotation components
		static MATRIX CreateBillboard(const MATRIX& view);
		static MATRIX CreateBillboardHorizontal(const MATRIX& view);
		static MATRIX CreateBillboardVertical(const MATRIX& view);

		static MATRIX CreateTranslation(const VECTOR3& position);
		static MATRIX CreateTranslation(float x, float y, float z);

		static MATRIX CreateScale(const VECTOR3& scales);
		static MATRIX CreateScale(float xs, float ys, float zs);
		static MATRIX CreateScale(float scale);

		static MATRIX CreateRotationX(float angles);
		static MATRIX CreateRotationY(float angles);
		static MATRIX CreateRotationZ(float angles);

		static MATRIX CreateFromAxisAngle(const VECTOR3& axis, float angle);

		static MATRIX CreatePerspectiveFieldOfView(float fov, float aspectRatio, float nearPlane, float farPlane);
		static MATRIX CreatePerspective(float width, float height, float nearPlane, float farPlane);
		static MATRIX CreatePerspectiveOffCenter(float left, float right, float bottom, float top, float nearPlane, float farPlane);
		static MATRIX CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane);
		static MATRIX CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane);

		static MATRIX CreateLookAt(const VECTOR3& eye, const VECTOR3& target, const VECTOR3& up);
		static MATRIX CreateLookTo(const VECTOR3& eye, const VECTOR3& eye_dir, const VECTOR3& up_dir);
		static MATRIX CreateWorld(const VECTOR3& position, const Quaternion& quat, const VECTOR3& scale);
		static MATRIX CreateWorld(const VECTOR3& position, const VECTOR3& forward, const VECTOR3& up);

		static MATRIX CreateFromQuaternion(const Quaternion& quat);
		static Quaternion CreateQuaternion(const MATRIX& mat);

		static MATRIX CreateFromYawPitchRoll(float x, float y, float z);

		static void Lerp(const MATRIX& M1, const MATRIX& M2, float t, MATRIX& result);
		static MATRIX Lerp(const MATRIX& M1, const MATRIX& M2, float t);

		static void Transform(const MATRIX& M, const Quaternion& rotation, MATRIX& result);
		static MATRIX Transform(const MATRIX& M, const Quaternion& rotation);

		// Constants
		static const MATRIX Identity;
		static const MATRIX Reversed_Z;
	};

	// Binary operators
	inline MATRIX operator+ (const MATRIX& M1, const MATRIX& M2) noexcept;
	inline MATRIX operator- (const MATRIX& M1, const MATRIX& M2) noexcept;
	inline MATRIX operator* (const MATRIX& M1, const MATRIX& M2) noexcept;
	inline MATRIX operator* (const MATRIX& M, float S) noexcept;
	MATRIX operator/ (const MATRIX& M, float S);
	MATRIX operator/ (const MATRIX& M1, const MATRIX& M2);
	// Element-wise divide
	inline MATRIX operator* (float S, const MATRIX& M) noexcept;
#pragma endregion



	// =============================================================
	// 
	//                     Quaternion
	// 
	// =============================================================
#pragma region Quaternion
	class Quaternion : public DirectX::XMFLOAT4
	{
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(x),
					CEREAL_NVP(y),
					CEREAL_NVP(z),
					CEREAL_NVP(w)
				);
			}
		}

	public:
		Quaternion() noexcept : XMFLOAT4(0.f, 0.f, 0.f, 1.f) {}
		constexpr Quaternion(float _x, float _y, float _z, float _w) noexcept : XMFLOAT4(_x, _y, _z, _w) {}
		Quaternion(const VECTOR3& v, float scalar) noexcept : XMFLOAT4(v.x, v.y, v.z, scalar) {}
		explicit Quaternion(const VECTOR4& v) noexcept : XMFLOAT4(v.x, v.y, v.z, v.w) {}
		Quaternion(DirectX::FXMVECTOR v) noexcept { DirectX::XMStoreFloat4(this, v); }
		Quaternion(const XMFLOAT4& q) noexcept { this->x = q.x; this->y = q.y; this->z = q.z; this->w = q.w; }
		explicit Quaternion(const DirectX::XMVECTORF32& f) noexcept { this->x = f.f[0]; this->y = f.f[1]; this->z = f.f[2]; this->w = f.f[3]; }

		Quaternion(const Quaternion&) = default;
		Quaternion& operator=(const Quaternion&) = default;

		Quaternion(Quaternion&&) = default;
		Quaternion& operator=(Quaternion&&) = default;
		
		operator DirectX::XMVECTOR() const noexcept { return XMLoadFloat4(this); }

		~Quaternion() = default;

		// Comparison operators
		bool operator == (const Quaternion& q) const noexcept;
		bool operator != (const Quaternion& q) const noexcept;

		// Assignment operators
		constexpr Quaternion& operator= (const DirectX::XMVECTORF32& F) noexcept { x = F.f[0]; y = F.f[1]; z = F.f[2]; w = F.f[3]; return *this; }
		Quaternion& operator+=(const Quaternion& q) = delete;
		Quaternion& operator-=(const Quaternion& q) = delete;
		Quaternion& operator*= (const Quaternion& q) noexcept;
		Quaternion& operator*= (float s) noexcept;
		Quaternion& operator/= (const Quaternion& q) noexcept;
		Quaternion& operator/= (float s) noexcept;

		// Unary operators
		constexpr Quaternion operator+ () const noexcept { return *this; }

		// Quaternion operations
		float Length() const;
		float LengthSq() const noexcept;

		VECTOR3 ToEulerAngles() const;
		MATRIX ToRotationMatrix() const noexcept;
		void ToAxisAngle(VECTOR3& axis, float& angle) const;

		Quaternion Normalize() const;
		void Normalize(Quaternion& result) const;

		// 共役を求める
		Quaternion Conjugate() const;
		void Conjugate(Quaternion& result) const;

		// 正規化された共役 = 逆クォータニオン
		void Inverse(Quaternion& result) const;
		Quaternion Inverse() const;

		float Dot(const Quaternion& q) const;

		bool Equals(const Quaternion& q) const noexcept;

		VECTOR3 Right() const;
		VECTOR3 Up() const;
		VECTOR3 Forward() const;

		void SetFromToRotation(const VECTOR3& from_direction, const VECTOR3& to_direction);
		void SetLookRotation(const VECTOR3& view, const VECTOR3& up = VECTOR3::Up);

		// Static functions
		// Return an angles[°]
		static float Angle(const Quaternion& q1, const Quaternion& q2);

		// オイラー角-->クォータニオン
		static Quaternion EulerAngles(const VECTOR3& euler);
		static Quaternion EulerAngles(float x, float y, float z);

		// 内積(0から1の範囲が得られる)
		static float Dot(const Quaternion& q1, const Quaternion& q2);

		// 軸に対して回転
		static Quaternion AxisAngle(const VECTOR3& axis, float angle);

		// X、Y、Z軸それぞれの角度からクォータニオンを生成(z->x->y)
		static Quaternion EulerToQuaternion(const VECTOR3& angles);
		static Quaternion EulerToQuaternion(float x, float y, float z);

		// 回転行列からクォータニオンを生成
		static Quaternion CreateFromRotationMatrix(const MATRIX& m);

		// クォータニオンから回転行列を生成
		static MATRIX CreateRotationMatrix(const Quaternion& q);

		// From-->Toに回転するクォータニオンを生成
		static Quaternion FromToRotation(const VECTOR3& from_direction, const VECTOR3& to_direction);

		// forwardを正面に、upwards寄りの上向きをするクォータニオンを生成
		static Quaternion LookRotation(const VECTOR3& forward, const VECTOR3& upward = VECTOR3::Up);

		// 線形補間
		static void Lerp(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& result);
		static Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float t);
		static void LerpUnclamped(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& result);
		static Quaternion LerpUnclamped(const Quaternion& q1, const Quaternion& q2, float t);

		// 球面補間
		static void Slerp(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& result);
		static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t);
		static void SlerpUnclamped(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& result);
		static Quaternion SlerpUnclamped(const Quaternion& q1, const Quaternion& q2, float t);

		// 3次補間
		static void SmoothStep(const Quaternion& q1, const Quaternion& q2, float t, Quaternion& result);
		static Quaternion SmoothStep(const Quaternion& q1, const Quaternion& q2, float t);

		// 連結する
		static void Concatenate(const Quaternion& q1, const Quaternion& q2, Quaternion& result);
		static Quaternion Concatenate(const Quaternion& q1, const Quaternion& q2);


		// Constants
		static const Quaternion Identity;
		static const Quaternion ROTATE_AXIS_X_180;
		static const Quaternion ROTATE_AXIS_Y_180;
		static const Quaternion ROTATE_AXIS_Z_180;
	};

	// Binary operators
	Quaternion operator* (const Quaternion& q1, const Quaternion& q2);
	VECTOR3 operator* (const VECTOR3& point, const Quaternion& rot);
	Quaternion operator* (const Quaternion& q, float s);
	Quaternion operator* (float s, const Quaternion& q);
	Quaternion operator/ (const Quaternion& q1, const Quaternion& q2);
	Quaternion operator/ (const Quaternion& q, float s);
#pragma endregion

}// namespace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::VECTOR2, 1)
CEREAL_CLASS_VERSION(TKGEngine::VECTOR3, 1)
CEREAL_CLASS_VERSION(TKGEngine::VECTOR4, 1)
CEREAL_CLASS_VERSION(TKGEngine::MATRIX, 1)
CEREAL_CLASS_VERSION(TKGEngine::Quaternion, 1)

// -----------------------------------------------------------------------------------------------------
// 
//                inline
// 
// -----------------------------------------------------------------------------------------------------
#include "../src/myfunc_vector.inl"