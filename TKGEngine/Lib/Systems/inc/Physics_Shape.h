#pragma once

#include "Systems/inc/TKGEngine_Defined.h"
#include "Systems/inc/Physics_Defined.h"

#include <btBulletCollisionCommon.h>

namespace TKGEngine
{
	/// <summary>
	/// 扱う形状のデータのベース
	/// </summary>
#pragma region ShapeBase
	class ShapeBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		ShapeBase() = default;
		virtual ~ShapeBase();

		ShapeBase(const ShapeBase&) = delete;
		ShapeBase(ShapeBase&&) = default;

#ifdef USE_IMGUI
		virtual bool OnGUI();
#endif// #ifdef USE_IMGUI

		/// <summary>
		/// TransformデータからbtTransformを作成して返す
		/// </summary>
		btTransform GetbtTransform() const
		{
			return CreatebtTransform(m_translation, m_rotation);
		}

		// 形状データの作成
		virtual ShapeID CreateShape() = 0;
		ShapeID CreateShape(
			const VECTOR3& translate,
			const Quaternion& quat
		);

		// 形状を返す
		virtual ShapeType GetShapeType() const = 0;
		virtual const char* GetShapeTypeName() const = 0;

		// 範囲
		virtual void SetExtents(const VECTOR3& extents) = 0;

		// 半径
		virtual void SetRadius(float radius) = 0;

		// 高さ
		virtual void SetHeight(float height) = 0;

		// 位置
		void SetTranslation(const VECTOR3& tr);
		VECTOR3 GetTranslation() const
		{
			return m_translation;
		}

		// 回転
		void SetRotation(const Quaternion& quat);
		Quaternion GetRotation() const
		{
			return m_rotation;
		}

		// 質量
		void SetMass(float mass)
		{
			if (mass <= MIN_MASS)
			{
				mass = MIN_MASS;
			}
			
			m_mass = mass;
		}
		float GetMass() const
		{
			return m_mass;
		}

		// 形状データのIDを返す
		ShapeID GetShapeID() const
		{
			return m_shape_id;
		}

		// btCollisionShape*を返す
		btCollisionShape* GetPtr() const
		{
			return m_shape_ptr;
		}


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	protected:
		// ==============================================
		// protected methods
		// ==============================================

		// ShapeごとのIDを新しく取得する
		ShapeID UniqueID();


		// ==============================================
		// protected variables
		// ==============================================
		// 質量の最小値は 1g
		static constexpr float MIN_MASS = 0.001f;
		
		// Shape毎に区別するための独自のID
		static ShapeID m_current_shape_id;

		// 自身のID
		ShapeID m_shape_id = -1;

		// 自身のポインタ
		btCollisionShape* m_shape_ptr = nullptr;

		// Shapeの設定 (Transform)
		VECTOR3 m_translation = VECTOR3::Zero;
		Quaternion m_rotation = Quaternion::Identity;


	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(m_translation),
					CEREAL_NVP(m_rotation),
					CEREAL_NVP(m_mass)
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

		// 質量
		float m_mass = 1.0f;
	};
#pragma endregion
	// ~ShapeBase class


	/// <summary>
	/// ボックス形状
	/// </summary>
#pragma region BoxShape
	class BoxShape
		: public ShapeBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		BoxShape() = default;
		virtual ~BoxShape() = default;

		BoxShape(const BoxShape&) = delete;
		BoxShape(BoxShape&&) = default;

		// 形状データの作成
		virtual ShapeID CreateShape() override;

		// 形状を返す
		virtual ShapeType GetShapeType() const override
		{
			return ShapeType::Box;
		}
		virtual const char* GetShapeTypeName() const override
		{
			return "Box";
		}

		// 範囲
		virtual void SetExtents(const VECTOR3& extents) override;

		// 半径
		virtual void SetRadius(float radius) override {};

		// 高さ
		virtual void SetHeight(float height) override {};

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
					cereal::base_class<ShapeBase>(this),
					CEREAL_NVP(m_half_extents)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		virtual bool OnGUI() override;
#endif// #ifdef USE_IMGUI


		// ==============================================
		// private methods
		// ==============================================

		// 形状データ
		std::unique_ptr<btBoxShape> m_shape = nullptr;

		// 各辺の長さ
		VECTOR3 m_half_extents = VECTOR3(0.5f, 0.5f, 0.5f);

	};
#pragma endregion
	// ~BoxShape class

	/// <summary>
	/// 球形状
	/// </summary>
#pragma region SphereShape
	class SphereShape
		: public ShapeBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		SphereShape() = default;
		virtual ~SphereShape() = default;

		SphereShape(const SphereShape&) = delete;
		SphereShape(SphereShape&&) = default;

		// 形状データの作成
		virtual ShapeID CreateShape() override;

		// 形状を返す
		virtual ShapeType GetShapeType() const override
		{
			return ShapeType::Sphere;
		}
		virtual const char* GetShapeTypeName() const override
		{
			return "Sphere";
		}

		// 範囲
		virtual void SetExtents(const VECTOR3& extents) override {};

		// 半径
		virtual void SetRadius(float radius) override;

		// 高さ
		virtual void SetHeight(float height) override {};


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
					cereal::base_class<ShapeBase>(this),
					CEREAL_NVP(m_radius)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		virtual bool OnGUI() override;
#endif// #ifdef USE_IMGUI


		// ==============================================
		// private methods
		// ==============================================

		// 形状データ
		std::unique_ptr<btSphereShape> m_shape = nullptr;

		// 半径
		float m_radius = 0.5f;

	};
#pragma endregion
	// ~SphereShape class


	/// <summary>
	/// カプセル形状
	/// </summary>
#pragma region CapsuleShape
	class CapsuleShape
		: public ShapeBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		CapsuleShape() = default;
		virtual ~CapsuleShape() = default;

		CapsuleShape(const CapsuleShape&) = delete;
		CapsuleShape(CapsuleShape&&) = default;

		// 形状データの作成
		virtual ShapeID CreateShape() override;

		// 形状を返す
		virtual ShapeType GetShapeType() const override
		{
			return ShapeType::Capsule;
		}
		virtual const char* GetShapeTypeName() const override
		{
			return "Capsule";
		}

		// 範囲
		virtual void SetExtents(const VECTOR3& extents) override {};

		// 半径
		virtual void SetRadius(float radius) override;

		// 高さ
		virtual void SetHeight(float height) override;


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
					cereal::base_class<ShapeBase>(this),
					CEREAL_NVP(m_radius),
					CEREAL_NVP(m_height)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		virtual bool OnGUI() override;
#endif// #ifdef USE_IMGUI


		// ==============================================
		// private methods
		// ==============================================

		// 形状データ
		std::unique_ptr<btCapsuleShape> m_shape = nullptr;

		// 半径
		float m_radius = 0.5f;

		// 高さ
		float m_height = 1.0f;

	};
#pragma endregion
	// ~CapsuleShape class


	/// <summary>
	/// 円柱形状
	/// </summary>
#pragma region CylinderShape
	class CylinderShape
		: public ShapeBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		CylinderShape() = default;
		virtual ~CylinderShape() = default;

		CylinderShape(const CylinderShape&) = delete;
		CylinderShape(CylinderShape&&) = default;

		// 形状データの作成
		virtual ShapeID CreateShape() override;

		// 形状を返す
		virtual ShapeType GetShapeType() const override
		{
			return ShapeType::Cylinder;
		}
		virtual const char* GetShapeTypeName() const override
		{
			return "Cylinder";
		}

		// 範囲
		virtual void SetExtents(const VECTOR3& extents) override {};

		// 半径
		virtual void SetRadius(float radius) override;

		// 高さ
		virtual void SetHeight(float height) override;


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
					cereal::base_class<ShapeBase>(this),
					CEREAL_NVP(m_radius),
					CEREAL_NVP(m_height)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		virtual bool OnGUI() override;
#endif// #ifdef USE_IMGUI


		// ==============================================
		// private methods
		// ==============================================

		// 形状データ
		std::unique_ptr<btCylinderShape> m_shape = nullptr;

		// 半径
		float m_radius = 1.0f;

		// 高さ
		float m_height = 1.0f;

	};
#pragma endregion
	// ~CylinderShape class

	/// <summary>
	/// 円錐形状
	/// </summary>
#pragma region ConeShape
	class ConeShape
		: public ShapeBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		ConeShape() = default;
		virtual ~ConeShape() = default;

		ConeShape(const ConeShape&) = delete;
		ConeShape(ConeShape&&) = default;

		// 形状データの作成
		virtual ShapeID CreateShape() override;

		// 形状を返す
		virtual ShapeType GetShapeType() const override
		{
			return ShapeType::Cone;
		}
		virtual const char* GetShapeTypeName() const override
		{
			return "Cone";
		}

		// 範囲
		virtual void SetExtents(const VECTOR3& extents) override {};

		// 半径
		virtual void SetRadius(float radius) override;

		// 高さ
		virtual void SetHeight(float height) override;


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
					cereal::base_class<ShapeBase>(this),
					CEREAL_NVP(m_radius),
					CEREAL_NVP(m_height)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		virtual bool OnGUI() override;
#endif// #ifdef USE_IMGUI


		// ==============================================
		// private methods
		// ==============================================

		// 形状データ
		std::unique_ptr<btConeShape> m_shape = nullptr;

		// 半径
		float m_radius = 0.5f;

		// 高さ
		float m_height = 1.0f;

	};
#pragma endregion
	// ~ConeShape class

}// namespace TKGEngine

// ShapeBase
CEREAL_CLASS_VERSION(TKGEngine::ShapeBase, 1)
// BoxShape
CEREAL_CLASS_VERSION(TKGEngine::BoxShape, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::BoxShape, "TKGEngine::BoxShape")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::ShapeBase, TKGEngine::BoxShape)
// SphereShape
CEREAL_CLASS_VERSION(TKGEngine::SphereShape, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::SphereShape, "TKGEngine::SphereShape")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::ShapeBase, TKGEngine::SphereShape)
// CapsuleShape
CEREAL_CLASS_VERSION(TKGEngine::CapsuleShape, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::CapsuleShape, "TKGEngine::CapsuleShape")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::ShapeBase, TKGEngine::CapsuleShape)
// CylinderShape
CEREAL_CLASS_VERSION(TKGEngine::CylinderShape, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::CylinderShape, "TKGEngine::CylinderShape")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::ShapeBase, TKGEngine::CylinderShape)
// ConeShape
CEREAL_CLASS_VERSION(TKGEngine::ConeShape, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::ConeShape, "TKGEngine::ConeShape")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::ShapeBase, TKGEngine::ConeShape)