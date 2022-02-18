#pragma once

#include "Systems/inc/TKGEngine_Defined.h"
#include "Systems/inc/Physics_Defined.h"

#include <btBulletCollisionCommon.h>

namespace TKGEngine
{
	/// <summary>
	/// �����`��̃f�[�^�̃x�[�X
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
		/// Transform�f�[�^����btTransform���쐬���ĕԂ�
		/// </summary>
		btTransform GetbtTransform() const
		{
			return CreatebtTransform(m_translation, m_rotation);
		}

		// �`��f�[�^�̍쐬
		virtual ShapeID CreateShape() = 0;
		ShapeID CreateShape(
			const VECTOR3& translate,
			const Quaternion& quat
		);

		// �`���Ԃ�
		virtual ShapeType GetShapeType() const = 0;
		virtual const char* GetShapeTypeName() const = 0;

		// �͈�
		virtual void SetExtents(const VECTOR3& extents) = 0;

		// ���a
		virtual void SetRadius(float radius) = 0;

		// ����
		virtual void SetHeight(float height) = 0;

		// �ʒu
		void SetTranslation(const VECTOR3& tr);
		VECTOR3 GetTranslation() const
		{
			return m_translation;
		}

		// ��]
		void SetRotation(const Quaternion& quat);
		Quaternion GetRotation() const
		{
			return m_rotation;
		}

		// ����
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

		// �`��f�[�^��ID��Ԃ�
		ShapeID GetShapeID() const
		{
			return m_shape_id;
		}

		// btCollisionShape*��Ԃ�
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

		// Shape���Ƃ�ID��V�����擾����
		ShapeID UniqueID();


		// ==============================================
		// protected variables
		// ==============================================
		// ���ʂ̍ŏ��l�� 1g
		static constexpr float MIN_MASS = 0.001f;
		
		// Shape���ɋ�ʂ��邽�߂̓Ǝ���ID
		static ShapeID m_current_shape_id;

		// ���g��ID
		ShapeID m_shape_id = -1;

		// ���g�̃|�C���^
		btCollisionShape* m_shape_ptr = nullptr;

		// Shape�̐ݒ� (Transform)
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

		// ����
		float m_mass = 1.0f;
	};
#pragma endregion
	// ~ShapeBase class


	/// <summary>
	/// �{�b�N�X�`��
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

		// �`��f�[�^�̍쐬
		virtual ShapeID CreateShape() override;

		// �`���Ԃ�
		virtual ShapeType GetShapeType() const override
		{
			return ShapeType::Box;
		}
		virtual const char* GetShapeTypeName() const override
		{
			return "Box";
		}

		// �͈�
		virtual void SetExtents(const VECTOR3& extents) override;

		// ���a
		virtual void SetRadius(float radius) override {};

		// ����
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

		// �`��f�[�^
		std::unique_ptr<btBoxShape> m_shape = nullptr;

		// �e�ӂ̒���
		VECTOR3 m_half_extents = VECTOR3(0.5f, 0.5f, 0.5f);

	};
#pragma endregion
	// ~BoxShape class

	/// <summary>
	/// ���`��
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

		// �`��f�[�^�̍쐬
		virtual ShapeID CreateShape() override;

		// �`���Ԃ�
		virtual ShapeType GetShapeType() const override
		{
			return ShapeType::Sphere;
		}
		virtual const char* GetShapeTypeName() const override
		{
			return "Sphere";
		}

		// �͈�
		virtual void SetExtents(const VECTOR3& extents) override {};

		// ���a
		virtual void SetRadius(float radius) override;

		// ����
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

		// �`��f�[�^
		std::unique_ptr<btSphereShape> m_shape = nullptr;

		// ���a
		float m_radius = 0.5f;

	};
#pragma endregion
	// ~SphereShape class


	/// <summary>
	/// �J�v�Z���`��
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

		// �`��f�[�^�̍쐬
		virtual ShapeID CreateShape() override;

		// �`���Ԃ�
		virtual ShapeType GetShapeType() const override
		{
			return ShapeType::Capsule;
		}
		virtual const char* GetShapeTypeName() const override
		{
			return "Capsule";
		}

		// �͈�
		virtual void SetExtents(const VECTOR3& extents) override {};

		// ���a
		virtual void SetRadius(float radius) override;

		// ����
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

		// �`��f�[�^
		std::unique_ptr<btCapsuleShape> m_shape = nullptr;

		// ���a
		float m_radius = 0.5f;

		// ����
		float m_height = 1.0f;

	};
#pragma endregion
	// ~CapsuleShape class


	/// <summary>
	/// �~���`��
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

		// �`��f�[�^�̍쐬
		virtual ShapeID CreateShape() override;

		// �`���Ԃ�
		virtual ShapeType GetShapeType() const override
		{
			return ShapeType::Cylinder;
		}
		virtual const char* GetShapeTypeName() const override
		{
			return "Cylinder";
		}

		// �͈�
		virtual void SetExtents(const VECTOR3& extents) override {};

		// ���a
		virtual void SetRadius(float radius) override;

		// ����
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

		// �`��f�[�^
		std::unique_ptr<btCylinderShape> m_shape = nullptr;

		// ���a
		float m_radius = 1.0f;

		// ����
		float m_height = 1.0f;

	};
#pragma endregion
	// ~CylinderShape class

	/// <summary>
	/// �~���`��
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

		// �`��f�[�^�̍쐬
		virtual ShapeID CreateShape() override;

		// �`���Ԃ�
		virtual ShapeType GetShapeType() const override
		{
			return ShapeType::Cone;
		}
		virtual const char* GetShapeTypeName() const override
		{
			return "Cone";
		}

		// �͈�
		virtual void SetExtents(const VECTOR3& extents) override {};

		// ���a
		virtual void SetRadius(float radius) override;

		// ����
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

		// �`��f�[�^
		std::unique_ptr<btConeShape> m_shape = nullptr;

		// ���a
		float m_radius = 0.5f;

		// ����
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