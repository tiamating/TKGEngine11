#pragma once

#include "Components/inc/CBehaviour.h"
#include "Components/interface/ICollider.h"

#include "Systems/inc/Physics_Shape.h"
#include "Systems/inc/TKGEngine_Defined.h"
#include "Systems/inc/Physics_Defined.h"

#include "Utility/inc/myfunc_vector.h"



namespace TKGEngine
{
	/// <summary>
	/// Management Collider parameter.
	/// </summary>
	class Collider
		: public Behaviour
		, public ICollider
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Collider(const Collider&) = delete;
		Collider& operator=(const Collider&) = delete;

		Collider();
		~Collider() override;

		// �ʒu��ݒ�
		virtual void SetPosition(const VECTOR3& pos) = 0;
		// ��]��ݒ�
		virtual void SetRotation(const Quaternion& rot) = 0;

		// ���x(RigidBody�p)
		[[nodiscard]] virtual VECTOR3 GetLinearVelocity() const = 0;


		/// �t���O�A�p�����[�^�̕ύX���ɁA��蒼���K�v�����邽�߁A���̊֐�
		virtual void RecreateCollisionObject() = 0;

		// �X�^�e�B�b�N
		virtual void SetStatic(const bool is_static) = 0;
		bool IsStatic() const;
		
		// �L�l�}�e�B�b�N
		virtual void SetKinematic(const bool is_kinematic) = 0;
		bool IsKinematic() const override;

		// �Փˌ��m���Ɋ֐����ĂԂ��ǂ���
		inline void SetCallOnFunction(bool do_call);

		// RigidBody���ǂ���
		virtual bool IsRigidBody() const = 0;


		// Shape�֘A
		
		/// <summary>
		/// �`��f�[�^��ǉ�����
		/// </summary>
		/// <returns>Shape���Ƃ�ID</returns>
		virtual ShapeID AddShape(
			ShapeType shape_type,
			const VECTOR3& translate = VECTOR3::Zero,
			const Quaternion& quat = Quaternion::Identity
		);
		// �`��f�[�^�̍폜
		virtual void RemoveShape(ShapeID id);

		// �ʒu
		void SetShapeTranslation(ShapeID id, const VECTOR3& translate);
		// ��]
		void SetShapeRotation(ShapeID id, const Quaternion& quat);
		void SetShapeRotation(ShapeID id, const VECTOR3& euler);
		// ���a
		void SetShapeRadius(ShapeID id, float radius);
		// ����
		void SetShapeHeight(ShapeID id, float height);
		// �͈�
		void SetShapeExtents(ShapeID id, const VECTOR3& extents);


		// ~Shape�֘A


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	protected:
		// ==============================================
		// protected methods
		// ==============================================
#ifdef USE_IMGUI
		void OnGUI() override;
		void ShapeGUI();
#endif // USE_IMGUI

	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					cereal::base_class<Behaviour>(this),
					CEREAL_NVP(m_do_call_function),
					CEREAL_NVP(m_shapes),
					CEREAL_NVP(m_is_static),
					CEREAL_NVP(m_is_kinematic)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
		void OnCreate(const std::shared_ptr<Object>& p_self) final;
		void OnDestroy() final;

		// �q�`��̌`����X�V����
		void UpdateChildShape(ShapeID id);

		// ICollider
		/// <summary>
		/// �Փˏ�Ԃ̃`�F�b�N���ɌĂ΂��
		/// </summary>
		/// <param name="other">�Փˑ����ID</param>
		void AddTriggerHitID(int other) final;
		void AddCollisionHitID(int other) final;

		// �Փˏ�Ԃ̍X�V
		void UpdateHitState() final;
		void UpdateTriggerList();
		void UpdateCollisionList();

		// �Փˎ��̊֐����ĂԂ��ǂ���
		bool IsCallOnFunction() const final;

		// GameObjectID�̎擾
		int GetGameObjectID() const final;

		// ComponentID�̎擾
		int GetColliderID() const final;
		// ~ICollider

		// ==============================================
		// private variables
		// ==============================================

		// OnCollision, OnTrigger���ĂԂ�
		bool m_do_call_function = true;

		// �Փˏ�Ԃ�ۑ�����ID���X�g
		std::unordered_set<int> m_prev_trigger_list;
		std::unordered_set<int> m_current_trigger_list;

		std::unordered_set<int> m_prev_collision_list;
		std::unordered_set<int> m_current_collision_list;


	protected:
		// ==============================================
		// protected methods
		// ==============================================
		virtual void OnCreated(const std::shared_ptr<Object>& p_self) = 0;
		virtual void OnDestroyed() = 0;

		// ShapeID����shape�z��̃C���f�b�N�X��T������
		int FindShapeDataIndex(const ShapeID id) const;
		// ShapeID����CompoundShape���̃C���f�b�N�X��T������
		int FindShapeFromCompound(const ShapeID id) const;

		// Compound Shape��AABB�̍Čv�Z
		void RecalculateShapeAABB();

		// ���C���[�O���[�v�ƃ��C���[�}�X�N�̐ݒ���쐬����
		virtual void CreateLayerGroupAndMask(int& group, int& mask) = 0;

		// ICollider
		bool IsTrigger() const override = 0;
		// Collision�̃A�N�e�B�u��ω�������Ƃ��ɌĂ΂��
		void OnSetCollisionActive(bool is_active) override = 0;
		// Collider���L�����ǂ���
		bool IsActiveCollider() final;
		// ~ICollider

		// Compound Shape�̊����A���ʁAAABB�̍Čv�Z
		virtual void RecalculateBody() = 0;

		// ==============================================
		// protected variables
		// ==============================================

		// �����`��
		std::unique_ptr<btCompoundShape> m_compound_shape = nullptr;
		// �`��f�[�^
		std::vector<std::unique_ptr<ShapeBase>> m_shapes;
		//// �`��X�P�[��
		//VECTOR3 m_scale = VECTOR3::One;

		// �X�^�e�B�b�N
		bool m_is_static = false;

		// �L�l�}�e�B�b�N
		bool m_is_kinematic = false;

	};

	// --------------------------------------------------------------
	// inline
	// --------------------------------------------------------------
	inline void Collider::SetCallOnFunction(bool do_call)
	{
		m_do_call_function = do_call;
	}

	inline bool Collider::IsCallOnFunction() const
	{
		return m_do_call_function;
	}
	inline bool Collider::IsActiveCollider()
	{
		return IsActiveBehaviour();
	}

}// namespace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::Collider, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::Collider, "TKGEngine::Collider")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Behaviour, TKGEngine::Collider)