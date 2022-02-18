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
	// 前方宣言
	// ==============================================
	class IBulletDebugDraw;
	class ICollider;
	class Collider;

	/// <summary>
	/// Bullet物理のマネージャー(シングルトン)
	/// デバイスとコンテキストを使用するため、GraphicsSystemの生成後に初期化する
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
			// Gravityの値を更新する
			SetWorldGravity(m_gravity);
			// レイヤーマスク値を表から計算する
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
		// Layer maskの設定
		void OnGUILayer();
#endif // USE_IMGUI

		/// <summary>
		/// Bullet Worldの参照を返す
		/// </summary>
		btDiscreteDynamicsWorld* GetWorld()
		{
			return m_instance->m_dynamics_world.get();
		}

		/// <summary>
		/// RigidBodyをBullet Worldに登録
		/// </summary>
		void RegisterRigidBody(std::shared_ptr<Collider>& collider, btRigidBody* rigid_body);
		/// <summary>
		/// RigidBodyをBullet Worldから消去
		/// </summary>
		void UnregisterRigidBody(btRigidBody* rigid_body);

		/// <summary>
		/// GhostObjectをBullet Worldに登録
		/// </summary>
		void RegisterGhostObject(std::shared_ptr<Collider>& collider, btGhostObject* ghost_obj);
		/// <summary>
		/// GhostObjectをBullet Worldから消去
		/// </summary>
		void UnregisterGhostObject(btGhostObject* ghost_obj);

		/// <summary>
		/// アクションインターフェースを登録
		/// </summary>
		void AddAction(btActionInterface* action) const;
		/// <summary>
		/// アクションインターフェースを削除
		/// </summary>
		void RemoveAction(btActionInterface* action) const;

		/// <summary>
		/// GameObjectのactiveを更新したときにColliderの状態を更新するために呼ぶ
		/// </summary>
		void SetCollisionActive(GameObjectID goid, bool is_active);


		void SetWorldGravity(const VECTOR3& gravity);

		[[nodiscard]] VECTOR3 GetWorldGravity() const
		{
			return m_gravity;
		}

		// レイヤーマスク値を表から計算する
		void CreateLayerMaskParam();

		// レイヤーマスクを取得する
		int GetLayerMask(Layer layer);

		// Colliderマップのポインタを返す
		[[nodiscard]] const std::unordered_map<int, std::shared_ptr<ICollider>>* GetColliderMap() const
		{
			return &m_collider_map;
		}

		// CollisionIDからColliderコンポーネントを取得する
		std::shared_ptr<Collider> GetColliderForID(const int col_id);

		// デバッグ用形状描画
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
		// コリジョンのデバッグ描画
		void OnDebugDraw() const;
#endif // USE_IMGUI

		// 衝突しているオブジェクトペアの状態を更新
		void UpdateCollisionState();

		// 登録されているオブジェクトの衝突状態に応じた関数を呼ぶ
		void CallCollisionFunction();


		// ==============================================
		// private variables
		// ==============================================
		// Singletonインスタンス
		static PhysicsSystem* m_instance;

		// 衝突検出方法の選択(デフォルトを選択)
		std::unique_ptr<btDefaultCollisionConfiguration> m_collision_configuration = nullptr;
		std::unique_ptr<btCollisionDispatcher> m_collision_dispatcher = nullptr;

		// ブロードフェーズ法の設定(Dynamic AABB tree method)
		std::unique_ptr<btBroadphaseInterface> m_broadphase = nullptr;

		// 拘束(剛体間リンク)のソルバ設定
		std::unique_ptr<btSequentialImpulseConstraintSolver> m_solver = nullptr;

		// Bulletのワールド作成
		std::unique_ptr<btDiscreteDynamicsWorld> m_dynamics_world = nullptr;

		//ゴーストペアコールバック
		std::unique_ptr<btGhostPairCallback> m_ghost_pair_call = nullptr;

		// ブロードフェーズフィルターコールバック
		struct BroadphaseFilterCallback
			: public btOverlapFilterCallback
		{
			virtual ~BroadphaseFilterCallback() = default;
			// 衝突条件を満たすときtrueを返す
			virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const override
			{
				// static同士は衝突しない
				// layer & mask == layerとなる条件のみ通す
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

		// ワールド重力加速度
		VECTOR3 m_gravity = VECTOR3(0.0f, -9.8f, 0.0f);

		// コリジョンテストのマスク選択時に表示する表用
		bool m_layer_mask[30][30] = {};
		// 実行開始時にレイヤー毎に決定されるマスク値
		int m_layer_mask_param[30] = {};

		// CollisionObjectとComponentを関連付ける
		std::unordered_map<int, std::shared_ptr<ICollider>> m_collider_map;

		// GameObjectIDとCollisionObjectを関連付ける
		std::unordered_map<GameObjectID, std::unordered_set<int>> m_goid_collision_map;
	};

}// namespace TKGEngine


CEREAL_CLASS_VERSION(TKGEngine::PhysicsSystem, 1)
// archive時にload saveを使用する曖昧さ回避
CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(TKGEngine::PhysicsSystem, cereal::specialization::member_load_save)