#pragma once

#include "Application/Objects/Managers/ComponentManager.h"
#include "Application/inc/ProjectSetting.h"
#include "Systems/inc/TKGEngine_Defined.h"
#include "Systems/inc/Physics_Defined.h"
#include "Systems/inc/LogSystem.h"
#include "Utility/inc/myfunc_vector.h"

#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include <unordered_set>
#include <unordered_map>

#include <memory>


namespace TKGEngine
{
	// ==============================================
	// �O���錾
	// ==============================================
	class IBulletDebugDraw;
	class ICollider;
	class Collider;

	/// <summary>
	/// Bullet�����̃}�l�[�W���[(�V���O���g��)
	/// �f�o�C�X�ƃR���e�L�X�g���g�p���邽�߁AGraphicsSystem�̐�����ɏ���������
	/// </summary>
	class PhysicsSystem
	{
		friend class cereal::access;
		template <class Archive>
		void load(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(m_layer_mask),
					CEREAL_NVP(m_gravity)
				);

			}
			// Gravity�̒l���X�V����
			SetWorldGravity(m_gravity);
			// ���C���[�}�X�N�l��\����v�Z����
			CreateLayerMaskParam();
		}
		template <class Archive>
		void save(Archive& archive, const std::uint32_t version) const
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(m_layer_mask),
					CEREAL_NVP(m_gravity)
				);
			}
		}

	public:
		// ==============================================
		// public methods
		// ==============================================
		static bool Create();
		static void Destroy();

		static void FrameUpdate(const float elapsed_time);

		static PhysicsSystem* GetInstance();

#ifdef USE_IMGUI
		static void DebugDraw();
#endif // USE_IMGUI


		// ==============================================
		// public methods
		// ==============================================

#ifdef USE_IMGUI
		void OpenLayer();
		// Layer mask�̐ݒ�
		void OnGUILayer();
#endif // USE_IMGUI

		/// <summary>
		/// Bullet World�̎Q�Ƃ�Ԃ�
		/// </summary>
		btDiscreteDynamicsWorld* GetWorld()
		{
			return m_instance->m_dynamics_world.get();
		}

		/// <summary>
		/// RigidBody��Bullet World�ɓo�^
		/// </summary>
		void RegisterRigidBody(std::shared_ptr<Collider>& collider, btRigidBody* rigid_body);
		/// <summary>
		/// RigidBody��Bullet World�������
		/// </summary>
		void UnregisterRigidBody(btRigidBody* rigid_body);

		/// <summary>
		/// GhostObject��Bullet World�ɓo�^
		/// </summary>
		void RegisterGhostObject(std::shared_ptr<Collider>& collider, btGhostObject* ghost_obj);
		/// <summary>
		/// GhostObject��Bullet World�������
		/// </summary>
		void UnregisterGhostObject(btGhostObject* ghost_obj);

		/// <summary>
		/// �A�N�V�����C���^�[�t�F�[�X��o�^
		/// </summary>
		void AddAction(btActionInterface* action) const;
		/// <summary>
		/// �A�N�V�����C���^�[�t�F�[�X���폜
		/// </summary>
		void RemoveAction(btActionInterface* action) const;

		/// <summary>
		/// GameObject��active���X�V�����Ƃ���Collider�̏�Ԃ��X�V���邽�߂ɌĂ�
		/// </summary>
		void SetCollisionActive(GameObjectID goid, bool is_active);


		void SetWorldGravity(const VECTOR3& gravity);

		[[nodiscard]] VECTOR3 GetWorldGravity() const
		{
			return m_gravity;
		}

		// ���C���[�}�X�N�l��\����v�Z����
		void CreateLayerMaskParam();

		// ���C���[�}�X�N���擾����
		int GetLayerMask(Layer layer);

		// Collider�}�b�v�̃|�C���^��Ԃ�
		[[nodiscard]] const std::unordered_map<int, std::shared_ptr<ICollider>>* GetColliderMap() const
		{
			return &m_collider_map;
		}

		// CollisionID����Collider�R���|�[�l���g���擾����
		std::shared_ptr<Collider> GetColliderForID(const int col_id);

		// �f�o�b�O�p�`��`��
		void DrawLine(const VECTOR3& from, const VECTOR3& to, const VECTOR3& from_color, const VECTOR3& to_color) const;
		void DrawSphere(const VECTOR3& pos, const float radius, const VECTOR3& color) const;
		void DrawBox(const VECTOR3& pos, const Quaternion& rot, const VECTOR3& half_extents, const VECTOR3& color) const;
		void DrawCapsule(const VECTOR3& pos, const Quaternion& rot, const float radius, const float height, const VECTOR3& color) const;
		void DrawCone(const VECTOR3& pos, const Quaternion& rot, const float radius, const float height, const VECTOR3& color) const;
		void DrawContactPoint(const VECTOR3& pos, const VECTOR3& normal, const float distance, const VECTOR3& color) const;


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================
		PhysicsSystem();
		virtual ~PhysicsSystem() = default;

		bool Initialize();
		void Finalize();
		void Update(const float elapsed_time);

#ifdef USE_IMGUI
		// �R���W�����̃f�o�b�O�`��
		void OnDebugDraw() const;
#endif // USE_IMGUI

		// �Փ˂��Ă���I�u�W�F�N�g�y�A�̏�Ԃ��X�V
		void UpdateCollisionState();

		// �o�^����Ă���I�u�W�F�N�g�̏Փˏ�Ԃɉ������֐����Ă�
		void CallCollisionFunction();


		// ==============================================
		// private variables
		// ==============================================
		// Singleton�C���X�^���X
		static PhysicsSystem* m_instance;

		// �Փˌ��o���@�̑I��(�f�t�H���g��I��)
		std::unique_ptr<btDefaultCollisionConfiguration> m_collision_configuration = nullptr;
		std::unique_ptr<btCollisionDispatcher> m_collision_dispatcher = nullptr;

		// �u���[�h�t�F�[�Y�@�̐ݒ�(Dynamic AABB tree method)
		std::unique_ptr<btBroadphaseInterface> m_broadphase = nullptr;

		// �S��(���̊ԃ����N)�̃\���o�ݒ�
		std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver = nullptr;

		// Bullet�̃��[���h�쐬
		std::unique_ptr<btDiscreteDynamicsWorld> m_dynamics_world = nullptr;

		//�S�[�X�g�y�A�R�[���o�b�N
		std::unique_ptr<btGhostPairCallback> m_ghost_pair_call = nullptr;

		// �u���[�h�t�F�[�Y�t�B���^�[�R�[���o�b�N
		struct BroadphaseFilterCallback
			: public btOverlapFilterCallback
		{
			virtual ~BroadphaseFilterCallback() = default;
			// �Փˏ����𖞂����Ƃ�true��Ԃ�
			virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const override
			{
				// static���m�͏Փ˂��Ȃ�
				// layer & mask == layer�ƂȂ�����̂ݒʂ�
				const bool collides =
					(proxy0->m_collisionFilterGroup & proxy1->m_collisionFilterMask) == proxy0->m_collisionFilterGroup &&
					(proxy1->m_collisionFilterGroup & proxy0->m_collisionFilterMask) == proxy1->m_collisionFilterGroup;
				if (!collides) return false;

				// AABB vs AABB
				if (
					proxy0->m_aabbMin.x() <= proxy1->m_aabbMax.x() && proxy0->m_aabbMax.x() >= proxy1->m_aabbMin.x() &&
					proxy0->m_aabbMin.y() <= proxy1->m_aabbMax.y() && proxy0->m_aabbMax.y() >= proxy1->m_aabbMin.y() &&
					proxy0->m_aabbMin.z() <= proxy1->m_aabbMax.z() && proxy0->m_aabbMax.z() >= proxy1->m_aabbMin.z()
					)
				{
					return true;
				}

				return false;
			}
		};
		std::unique_ptr<BroadphaseFilterCallback> m_broadphase_filter_callback = nullptr;

#ifdef USE_IMGUI
		// Open GUI window
		bool m_is_gui_opened = false;

		// Bullet Debug Draw
		std::unique_ptr<IBulletDebugDraw> m_bullet_debug_draw = nullptr;
#endif // USE_IMGUI

		// ���[���h�d�͉����x
		VECTOR3 m_gravity = VECTOR3(0.0f, -9.8f, 0.0f);

		// �R���W�����e�X�g�̃}�X�N�I�����ɕ\������\�p
		bool m_layer_mask[30][30] = {};
		// ���s�J�n���Ƀ��C���[���Ɍ��肳���}�X�N�l
		int m_layer_mask_param[30] = {};

		// CollisionObject��Component���֘A�t����
		std::unordered_map<int, std::shared_ptr<ICollider>> m_collider_map;

		// GameObjectID��CollisionObject���֘A�t����
		std::unordered_map<GameObjectID, std::unordered_set<int>> m_goid_collision_map;
	};

}// namespace TKGEngine


CEREAL_CLASS_VERSION(TKGEngine::PhysicsSystem, 1)
// archive����load save���g�p����B�������
CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(TKGEngine::PhysicsSystem, cereal::specialization::member_load_save)