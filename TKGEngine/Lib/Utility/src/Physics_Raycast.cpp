
#include "Utility/inc/Physics_Raycast.h"

#include "Systems/inc/TKGEngine_Defined.h"
#include "Systems/inc/PhysicsSystem.h"
#include "Components/inc/CCollider.h"
#include "Components/inc/CRigidBody.h"
#include "Components/inc/CTransform.h"

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Static variable declaration
	////////////////////////////////////////////////////////
	static constexpr int TRIGGER_LAYER = (1 << static_cast<int>(Layer::Trigger));
	static constexpr int RAYCAST_LAYER = (1 << static_cast<int>(Layer::Raycast));
	// 0~29bitを取り出すマスク
	static constexpr int SELECTABLE_LAYER_MASK = 0x3fffffff;

	constexpr VECTOR3 Physics::DEBUG_COLOR = VECTOR3(1.0f, 0.5f, 1.0f);
	constexpr VECTOR3 Physics::DEBUG_HIT_COLOR = VECTOR3(0.9f, 0.6f, 0.0f);
	constexpr VECTOR3 Physics::DEBUG_CONTACT_POINT_COLOR = VECTOR3(0.3f, 0.8f, 1.0f);


	////////////////////////////////////////////////////////
	// Local Class declaration
	// フィルター機能を改変したRaycastコールバック
	////////////////////////////////////////////////////////
	// レイ用最近傍位置用コールバック
	class TKGClosestRayResultCallback
		: public btCollisionWorld::ClosestRayResultCallback
	{
	public:
		TKGClosestRayResultCallback(const btVector3& ray_from_world, const btVector3& ray_to_world, const bool is_hit_trigger)
			: btCollisionWorld::ClosestRayResultCallback(ray_from_world, ray_to_world), m_is_hit_trigger(is_hit_trigger)
		{
			/* nothing */
		}
		virtual ~TKGClosestRayResultCallback() = default;

		virtual bool needsCollision(btBroadphaseProxy* proxy0) const override
		{
			// Triggerと衝突しない
			if (!m_is_hit_trigger)
			{
				// 相手がTriggerならfalse
				if (proxy0->m_collisionFilterGroup & TRIGGER_LAYER)
				{
					return false;
				}
			}
			// 衝突するレイヤーかチェック
			const bool collides =
				(proxy0->m_collisionFilterGroup & m_collisionFilterMask) > 0 &&
				(m_collisionFilterGroup & proxy0->m_collisionFilterMask) == m_collisionFilterGroup;
			return collides;
		}

	private:
		bool m_is_hit_trigger;
	};

	// レイ用衝突している全てのコリジョン用コールバック
	class TKGAllHitsRayResultCallback
		: public btCollisionWorld::AllHitsRayResultCallback
	{
	public:
		TKGAllHitsRayResultCallback(const btVector3& ray_from_world, const btVector3& ray_to_world, const bool is_hit_trigger)
			: btCollisionWorld::AllHitsRayResultCallback(ray_from_world, ray_to_world), m_is_hit_trigger(is_hit_trigger)
		{
			/* nothing */
		}
		virtual ~TKGAllHitsRayResultCallback() = default;

		virtual bool needsCollision(btBroadphaseProxy* proxy0) const override
		{
			// Triggerと衝突しない
			if (!m_is_hit_trigger)
			{
				// 相手がTriggerならfalse
				if (proxy0->m_collisionFilterGroup & TRIGGER_LAYER)
				{
					return false;
				}
			}
			// 衝突するレイヤーかチェック
			const bool collides =
				(proxy0->m_collisionFilterGroup & m_collisionFilterMask) > 0 &&
				(m_collisionFilterGroup & proxy0->m_collisionFilterMask) == m_collisionFilterGroup;
			return collides;
		}

	private:
		bool m_is_hit_trigger;
	};

	// 凸包用最近傍位置用コールバック
	class TKGClosestConvexResultCallback
		: public btCollisionWorld::ClosestConvexResultCallback
	{
	public:
		TKGClosestConvexResultCallback(const btVector3& convex_from_world, const btVector3& convex_to_world, const bool is_hit_trigger)
			: btCollisionWorld::ClosestConvexResultCallback(convex_from_world, convex_to_world), m_is_hit_trigger(is_hit_trigger)
		{
			/* nothing */
		}
		virtual ~TKGClosestConvexResultCallback() = default;

		virtual bool needsCollision(btBroadphaseProxy* proxy0) const override
		{
			// Triggerと衝突しない
			if (!m_is_hit_trigger)
			{
				// 相手がTriggerならfalse
				if (proxy0->m_collisionFilterGroup & TRIGGER_LAYER)
				{
					return false;
				}
			}
			// 衝突するレイヤーかチェック
			const bool collides =
				(proxy0->m_collisionFilterGroup & m_collisionFilterMask) > 0 &&
				(m_collisionFilterGroup & proxy0->m_collisionFilterMask) == m_collisionFilterGroup;
			return collides;
		}

	private:
		bool m_is_hit_trigger;
	};

	// 凸包用衝突している全てのコリジョン用コールバック
	class TKGAllHitsConvexResultCallback
		: public btCollisionWorld::ConvexResultCallback
	{
	public:
		TKGAllHitsConvexResultCallback(const btVector3& convex_from_world, const btVector3& convex_to_world, const bool is_hit_trigger)
			: m_convexFromWorld(convex_from_world), m_convexToWorld(convex_to_world), m_is_hit_trigger(is_hit_trigger)
		{
			/* nothing */
		}
		virtual ~TKGAllHitsConvexResultCallback() = default;

		btAlignedObjectArray<const btCollisionObject*> m_hitCollisionObjects;

		btVector3 m_convexFromWorld;
		btVector3 m_convexToWorld;

		btAlignedObjectArray<btVector3> m_hitNormalWorld;
		btAlignedObjectArray<btVector3> m_hitPointWorld;
		btAlignedObjectArray<btScalar> m_hitFractions;

		// 衝突しているか
		bool hasHit() const
		{
			return (m_hitCollisionObjects.size() > 0);
		}
		// 条件式
		virtual bool needsCollision(btBroadphaseProxy* proxy0) const override
		{
			// Triggerと衝突しない
			if (!m_is_hit_trigger)
			{
				// 相手がTriggerならfalse
				if (proxy0->m_collisionFilterGroup & TRIGGER_LAYER)
				{
					return false;
				}
			}
			// 衝突するレイヤーかチェック
			const bool collides =
				(proxy0->m_collisionFilterGroup & m_collisionFilterMask) > 0 &&
				(m_collisionFilterGroup & proxy0->m_collisionFilterMask) == m_collisionFilterGroup;
			return collides;
		}
		// 衝突時の処理
		virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convex_result, bool normal_in_world_space)
		{
			m_closestHitFraction = convex_result.m_hitFraction;
			m_hitCollisionObjects.push_back(convex_result.m_hitCollisionObject);
			btVector3 hit_normal_world;
			if (normal_in_world_space)
			{
				hit_normal_world = convex_result.m_hitNormalLocal;
			}
			else
			{
				///need to transform normal into worldspace
				hit_normal_world = convex_result.m_hitCollisionObject->getWorldTransform().getBasis() * convex_result.m_hitNormalLocal;
			}
			m_hitNormalWorld.push_back(hit_normal_world);
			btVector3 hit_point_world;
			hit_point_world.setInterpolate3(m_convexFromWorld, m_convexToWorld, convex_result.m_hitFraction);
			m_hitPointWorld.push_back(hit_point_world);
			m_hitFractions.push_back(convex_result.m_hitFraction);
			return m_closestHitFraction;
		}

	private:
		bool m_is_hit_trigger;
	};

	// コリジョン接触位置検出用コールバック
	class TKGContactResultCallback
		: public btCollisionWorld::ContactResultCallback
	{
	public:
		const btCollisionObject* m_collisionObject;

		btVector3 m_hitNormalWorld;
		btVector3 m_hitPointWorld;
		btScalar m_distance;

		TKGContactResultCallback(btScalar threshold, const bool is_hit_trigger)
			: m_collisionObject(nullptr), m_is_hit_trigger(is_hit_trigger)
		{
			// 衝突閾値の設定
			m_closestDistanceThreshold = threshold;
		}
		virtual ~TKGContactResultCallback() = default;

		// 衝突しているか
		bool hasHit() const
		{
			return (m_collisionObject != nullptr);
		}
		// 条件式
		virtual bool needsCollision(btBroadphaseProxy* proxy0) const override
		{
			// Triggerと衝突しない
			if (!m_is_hit_trigger)
			{
				// 相手がTriggerならfalse
				if (proxy0->m_collisionFilterGroup & TRIGGER_LAYER)
				{
					return false;
				}
			}
			// 衝突するレイヤーかチェック
			const bool collides =
				(proxy0->m_collisionFilterGroup & m_collisionFilterMask) > 0 &&
				(m_collisionFilterGroup & proxy0->m_collisionFilterMask) == m_collisionFilterGroup;
			return collides;
		}
		// 衝突時の処理
		virtual btScalar addSingleResult(
			btManifoldPoint& cp,
			const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0,
			const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1
		)
		{
			// Distance
			const btScalar distance = cp.getDistance();
			// 閾値を超えないなら衝突していない
			if (distance + m_closestDistanceThreshold > 0)
			{
				// 戻り値は使われないので意味はない
				return 0;
			}
			m_distance = distance;
			// Col Objの保存
			m_collisionObject = colObj1Wrap->getCollisionObject();
			// Normal
			m_hitNormalWorld = cp.m_normalWorldOnB;
			// Position
			m_hitPointWorld = cp.getPositionWorldOnB();

			// 戻り値は使われないので意味はない
			return 0;
		}

	private:
		bool m_is_hit_trigger;
	};

	// コリジョン接触している全てのコリジョン用コールバック
	class TKGAllContactResultCallback
		: public btCollisionWorld::ContactResultCallback
	{
	public:
		TKGAllContactResultCallback(btScalar threshold, const bool is_hit_trigger)
			: m_collisionObject(nullptr), m_is_hit_trigger(is_hit_trigger)
		{
			// 衝突閾値の設定
			m_closestDistanceThreshold = threshold;
		}
		virtual ~TKGAllContactResultCallback() = default;

		const btCollisionObject* m_collisionObject;
		btAlignedObjectArray<const btCollisionObject*> m_collisionObjects;

		btAlignedObjectArray<btVector3> m_hitNormalWorld;
		btAlignedObjectArray<btVector3> m_hitPointWorld;
		btAlignedObjectArray<btScalar> m_distances;

	private:
		int m_current_index = 0;
		btScalar m_current_distance = static_cast<btScalar>(0.0);
		bool m_is_hit_trigger;

	public:

		// 衝突しているか
		bool hasHit() const
		{
			return (m_collisionObject != nullptr);
		}
		// 条件式
		virtual bool needsCollision(btBroadphaseProxy* proxy0) const override
		{
			// Triggerと衝突しない
			if (!m_is_hit_trigger)
			{
				// 相手がTriggerならfalse
				if (proxy0->m_collisionFilterGroup & TRIGGER_LAYER)
				{
					return false;
				}
			}
			// 衝突するレイヤーかチェック
			const bool collides =
				(proxy0->m_collisionFilterGroup & m_collisionFilterMask) > 0 &&
				(m_collisionFilterGroup & proxy0->m_collisionFilterMask) == m_collisionFilterGroup;
			return collides;
		}
		// 衝突時の処理
		virtual btScalar addSingleResult(
			btManifoldPoint& cp,
			const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0,
			const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1
		)
		{
			// Distance
			const btScalar distance = cp.getDistance();
			// 閾値を超えないなら衝突していない
			if (distance + m_closestDistanceThreshold > 0)
			{
				// 戻り値は使われないので意味はない
				return 0;
			}

			// 同一オブジェクトが複数の衝突点を持つ場合はもっとも距離の近いものをセットする
			if (m_collisionObject == colObj0Wrap->getCollisionObject())
			{
				if (distance > m_current_distance)
				{
					return 0;
				}
				// 一番近い距離を保存する
				m_current_distance = distance;
				// 最後尾の要素を上書きする
				--m_current_index;

				// データをセットする
				m_distances[m_current_index] = distance;
				// Col Objの保存
				m_collisionObject = colObj0Wrap->getCollisionObject();
				m_collisionObjects[m_current_index] = colObj0Wrap->getCollisionObject();
				// Normal
				const btVector3 hitNormalWorld = -cp.m_normalWorldOnB;
				m_hitNormalWorld[m_current_index] = hitNormalWorld;
				// Position
				m_hitPointWorld[m_current_index] = cp.getPositionWorldOnB();
			}
			// 異なるオブジェクトなら進める
			else
			{
				// 距離の情報をクリア
				m_current_distance = 0.0f;

				// データをセットする
				m_distances.push_back(distance);
				// Col Objの保存
				m_collisionObject = colObj0Wrap->getCollisionObject();
				m_collisionObjects.push_back(colObj0Wrap->getCollisionObject());
				// Normal
				const btVector3 hit_normal_world = -cp.m_normalWorldOnB;
				m_hitNormalWorld.push_back(hit_normal_world);
				// Position
				m_hitPointWorld.push_back(cp.getPositionWorldOnB());
			}

			++m_current_index;
			// 戻り値は使われないので意味はない
			return 0;
		}
	};

	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
#pragma region Raycast
	bool Physics::Raycast(const VECTOR3& origin, const VECTOR3& direction, const float max_distance, const bool is_hit_trigger, const int layer_mask)
	{
		// レイの長さは無限を超えない
		if (MyMath::IsInfinity(max_distance))
		{
			LOG_ASSERT("In Raycast(). Ray distance is over infinity.");
			return false;
		}

		// Rayの設定
		TKGClosestRayResultCallback ray_callback(
			ConvertVectorTobtVector(origin),
			ConvertVectorTobtVector(origin + max_distance * direction),
			is_hit_trigger
		);
		{
			ray_callback.m_collisionFilterGroup = RAYCAST_LAYER;
			ray_callback.m_collisionFilterMask = layer_mask & SELECTABLE_LAYER_MASK;
		}

#ifdef USE_IMGUI
		// Rayのデバッグ表示
		PhysicsSystem::GetInstance()->DrawLine(origin, origin + max_distance * direction, DEBUG_COLOR, DEBUG_COLOR);
#endif // USE_IMGUI

		// Rayテストの実行
		PhysicsSystem::GetInstance()->GetWorld()->rayTest(
			ray_callback.m_rayFromWorld,
			ray_callback.m_rayToWorld,
			ray_callback
		);

		// 結果
		if (ray_callback.hasHit())
		{
#ifdef USE_IMGUI
			// Rayのデバッグ表示
			PhysicsSystem::GetInstance()->DrawBox(ConvertbtVectorToVector(ray_callback.m_hitPointWorld), Quaternion::Identity, VECTOR3(0.05f), DEBUG_HIT_COLOR);
#endif // USE_IMGUI
			return true;
		}
		else
		{
			return false;
		}
	}

	bool Physics::Raycast(const VECTOR3& origin, const VECTOR3& direction, RaycastHit& hit_info, const float max_distance, const bool is_hit_trigger, const int layer_mask)
	{
		// レイの長さは無限を超えない
		if (MyMath::IsInfinity(max_distance))
		{
			LOG_ASSERT("In Raycast(). Ray distance is over infinity.");
			return false;
		}

		// Rayの設定
		TKGClosestRayResultCallback ray_callback(
			ConvertVectorTobtVector(origin),
			ConvertVectorTobtVector(origin + max_distance * direction),
			is_hit_trigger
		);
		{
			ray_callback.m_collisionFilterGroup = RAYCAST_LAYER;
			ray_callback.m_collisionFilterMask = layer_mask & SELECTABLE_LAYER_MASK;
		}

#ifdef USE_IMGUI
		// Rayのデバッグ表示
		PhysicsSystem::GetInstance()->DrawLine(origin, origin + max_distance * direction, DEBUG_COLOR, DEBUG_COLOR);
#endif // USE_IMGUI

		// Rayテストの実行
		PhysicsSystem::GetInstance()->GetWorld()->rayTest(
			ray_callback.m_rayFromWorld,
			ray_callback.m_rayToWorld,
			ray_callback
		);

		// 結果
		if (ray_callback.hasHit())
		{
			hit_info.position = ConvertbtVectorToVector(ray_callback.m_hitPointWorld);
			hit_info.normal = ConvertbtVectorToVector(ray_callback.m_hitNormalWorld);
			hit_info.distance = (hit_info.position - origin).Length();
			hit_info.hit_fraction = ray_callback.m_closestHitFraction;
			hit_info.collider = PhysicsSystem::GetInstance()->
				GetColliderForID(ray_callback.m_collisionObject->getUserIndex());
			if (hit_info.collider && hit_info.collider->IsRigidBody())
			{
				hit_info.rigidbody = std::static_pointer_cast<RigidBody>(hit_info.collider);
			}
			else
			{
				hit_info.rigidbody = nullptr;
			}
#ifdef USE_IMGUI
			// Rayのデバッグ表示
			PhysicsSystem::GetInstance()->DrawBox(hit_info.position, Quaternion::Identity, VECTOR3(0.1f), DEBUG_HIT_COLOR);
#endif // USE_IMGUI

			return true;
		}
		else
		{
			hit_info.position = VECTOR3::Zero;
			hit_info.normal = VECTOR3::Zero;
			hit_info.distance = MyMath::Infinity;
			hit_info.hit_fraction = 0.0f;
			hit_info.collider = nullptr;
			hit_info.rigidbody = nullptr;

			return false;
		}
	}

	bool Physics::RaycastAll(const VECTOR3& origin, const VECTOR3& direction, std::vector<RaycastHit>& hit_infos, const float max_distance, const bool is_hit_trigger, const int layer_mask)
	{
		// 返す情報のクリア
		hit_infos.clear();

		// レイの長さは無限を超えない
		if (MyMath::IsInfinity(max_distance))
		{
			LOG_ASSERT("In Raycast(). Ray distance is over infinity.");
			return false;
		}

		// Rayの設定
		TKGAllHitsRayResultCallback ray_callback(
			ConvertVectorTobtVector(origin),
			ConvertVectorTobtVector(origin + max_distance * direction),
			is_hit_trigger
		);
		{
			ray_callback.m_collisionFilterGroup = RAYCAST_LAYER;
			ray_callback.m_collisionFilterMask = layer_mask & SELECTABLE_LAYER_MASK;
		}

#ifdef USE_IMGUI
		// Rayのデバッグ表示
		PhysicsSystem::GetInstance()->DrawLine(origin, origin + max_distance * direction, DEBUG_COLOR, DEBUG_COLOR);
#endif // USE_IMGUI

		// Rayテストの実行
		PhysicsSystem::GetInstance()->GetWorld()->rayTest(
			ray_callback.m_rayFromWorld,
			ray_callback.m_rayToWorld,
			ray_callback
		);

		// 結果
		const int callback_size = ray_callback.m_collisionObjects.size();
		// 衝突なしなら
		if (callback_size <= 0)
			return false;
		// 情報を詰め込む
		hit_infos.resize(callback_size);
		for (int i = 0; i < callback_size; ++i)
		{
			auto& info = hit_infos.at(i);

			info.position = ConvertbtVectorToVector(ray_callback.m_hitPointWorld[i]);
			info.normal = ConvertbtVectorToVector(ray_callback.m_hitNormalWorld[i]);
			info.distance = (info.position - origin).Length();
			info.hit_fraction = ray_callback.m_hitFractions[i];
			info.collider = PhysicsSystem::GetInstance()->
				GetColliderForID(ray_callback.m_collisionObjects[i]->getUserIndex());
			if (info.collider && info.collider->IsRigidBody())
			{
				info.rigidbody = std::static_pointer_cast<RigidBody>(info.collider);
			}
			else
			{
				info.rigidbody = nullptr;
			}
#ifdef USE_IMGUI
			// Rayのデバッグ表示
			PhysicsSystem::GetInstance()->DrawBox(info.position, Quaternion::Identity, VECTOR3(0.1f), DEBUG_HIT_COLOR);
#endif // USE_IMGUI
		}
		// 衝突情報の距離ソート
		auto distance_sort = [&](const RaycastHit& lhs, const RaycastHit& rhs)->bool
		{
			return lhs.distance < rhs.distance;
		};
		std::sort(hit_infos.begin(), hit_infos.end(), distance_sort);

		return true;
	}
#pragma endregion 
	// ~Raycast

#pragma region Line cast
	bool Physics::Linecast(const VECTOR3& start, const VECTOR3& end, const bool is_hit_trigger, const int layer_mask)
	{
		const VECTOR3 seg = end - start;
		return Raycast(start, seg.Normalized(), seg.Length(), is_hit_trigger, layer_mask);
	}

	bool Physics::Linecast(const VECTOR3& start, const VECTOR3& end, RaycastHit& hit_info, const bool is_hit_trigger, const int layer_mask)
	{
		const VECTOR3 seg = end - start;
		return Raycast(start, seg.Normalized(), hit_info, seg.Length(), is_hit_trigger, layer_mask);
	}
#pragma endregion
	// ~Line cast

#pragma region Sphere cast
	bool Physics::SphereCast(const VECTOR3& origin, const float radius, const VECTOR3& direction, const float max_distance, const bool is_hit_trigger, const float ccd_penetration, const int layer_mask)
	{
		// 長さは無限を超えない
		if (MyMath::IsInfinity(max_distance))
		{
			LOG_ASSERT("In SphereCast(). Sphere distance is over infinity.");
			return false;
		}

		// Shapeの設定
		const btSphereShape shape(radius);

#ifdef USE_IMGUI
		// Sphereのデバッグ表示
		PhysicsSystem::GetInstance()->DrawSphere(origin, radius, DEBUG_COLOR);
		PhysicsSystem::GetInstance()->DrawLine(origin, origin + max_distance * direction, DEBUG_COLOR, DEBUG_COLOR);
		PhysicsSystem::GetInstance()->DrawSphere(origin + max_distance * direction, radius, DEBUG_COLOR);
#endif // USE_IMGUI

		// テストの実行
		const bool ret_val = ClosestConvexCast(&shape, origin, Quaternion::Identity, origin + max_distance * direction, Quaternion::Identity, is_hit_trigger, ccd_penetration, layer_mask);

		// 結果
		return ret_val;
	}

	bool Physics::SphereCast(const VECTOR3& origin, const float radius, const VECTOR3& direction, const float max_distance, RaycastHit& hit_info, const bool is_hit_trigger, const float ccd_penetration, const int layer_mask)
	{
		// 長さは無限を超えない
		if (MyMath::IsInfinity(max_distance))
		{
			LOG_ASSERT("In SphereCast(). Sphere distance is over infinity.");
			return false;
		}

		// Sphereの設定
		const btSphereShape shape(radius);

#ifdef USE_IMGUI
		// Sphereのデバッグ表示
		PhysicsSystem::GetInstance()->DrawSphere(origin, radius, DEBUG_COLOR);
		PhysicsSystem::GetInstance()->DrawLine(origin, origin + max_distance * direction, DEBUG_COLOR, DEBUG_COLOR);
		PhysicsSystem::GetInstance()->DrawSphere(origin + max_distance * direction, radius, DEBUG_COLOR);
#endif // USE_IMGUI

		// テストの実行
		const bool ret_val = ClosestConvexCast(hit_info, &shape, origin, Quaternion::Identity, origin + max_distance * direction, Quaternion::Identity, is_hit_trigger, ccd_penetration, layer_mask);

		// 結果
		return ret_val;
	}

	bool Physics::CheckSphere(const VECTOR3& origin, const float radius, const bool is_hit_trigger, const float threshold, const int layer_mask)
	{
		// Shapeの設定
		btSphereShape shape(radius);

		// コリジョンオブジェクトの作成
		btCollisionObject col_obj;
		col_obj.setCollisionShape(&shape);
		col_obj.setWorldTransform(btTransform(btQuaternion::getIdentity(), ConvertVectorTobtVector(origin)));

		// 衝突テスト
		const bool ret_val = CheckConvex(&col_obj, is_hit_trigger, threshold, layer_mask);

#ifdef USE_IMGUI
		// Sphereのデバッグ表示
		PhysicsSystem::GetInstance()->DrawSphere(origin, radius, ret_val ? DEBUG_HIT_COLOR : DEBUG_COLOR);
#endif // USE_IMGUI

		return ret_val;
	}

	bool Physics::OverlapSphere(const VECTOR3& origin, const float radius, std::vector<RaycastHit>& hit_infos, const bool is_hit_trigger, const float threshold, const int layer_mask)
	{
		// Shapeの設定
		btSphereShape shape(radius);

		// コリジョンオブジェクトの作成
		btCollisionObject col_obj;
		col_obj.setCollisionShape(&shape);
		col_obj.setWorldTransform(btTransform(btQuaternion::getIdentity(), ConvertVectorTobtVector(origin)));

		// 衝突テスト
		const bool ret_val = OverlapConvex(hit_infos, &col_obj, is_hit_trigger, threshold, layer_mask);

#ifdef USE_IMGUI
		// Sphereのデバッグ表示
		PhysicsSystem::GetInstance()->DrawSphere(origin, radius, ret_val ? DEBUG_HIT_COLOR : DEBUG_COLOR);
#endif // USE_IMGUI

		return ret_val;
	}
#pragma endregion
	// ~Sphere cast

#pragma region Box cast
	bool Physics::BoxCast(const VECTOR3& start_center, const Quaternion& start_rot, const VECTOR3& end_center, const Quaternion& end_rot, const VECTOR3& half_extents, const bool is_hit_trigger, const float ccd_penetration, const int layer_mask)
	{
		const float length = (end_center - start_center).Length();
		// 長さは無限を超えない
		if (MyMath::IsInfinity(length))
		{
			LOG_ASSERT("In BoxCast(). Box distance is over infinity.");
			return false;
		}

		// Shapeの設定
		const btBoxShape shape(ConvertVectorTobtVector(half_extents));

#ifdef USE_IMGUI
		// Boxのデバッグ表示
		PhysicsSystem::GetInstance()->DrawBox(start_center, start_rot, half_extents, DEBUG_COLOR);
		PhysicsSystem::GetInstance()->DrawLine(start_center, end_center, DEBUG_COLOR, DEBUG_COLOR);
		PhysicsSystem::GetInstance()->DrawBox(end_center, end_rot, half_extents, DEBUG_COLOR);
#endif // USE_IMGUI

		// テストの実行
		const bool ret_val = ClosestConvexCast(&shape, start_center, start_rot, end_center, end_rot, is_hit_trigger, ccd_penetration, layer_mask);

		// 結果
		return ret_val;
	}

	bool Physics::BoxCast(const VECTOR3& start_center, const Quaternion& start_rot, const VECTOR3& end_center, const Quaternion& end_rot, const VECTOR3& half_extents, RaycastHit& hit_info, const bool is_hit_trigger, const float ccd_penetration, const int layer_mask)
	{
		const float length = (end_center - start_center).Length();
		// 長さは無限を超えない
		if (MyMath::IsInfinity(length))
		{
			LOG_ASSERT("In BoxCast(). Box distance is over infinity.");
			return false;
		}

		// Shapeの設定
		const btBoxShape shape(ConvertVectorTobtVector(half_extents));

#ifdef USE_IMGUI
		// Boxのデバッグ表示
		PhysicsSystem::GetInstance()->DrawBox(start_center, start_rot, half_extents, DEBUG_COLOR);
		PhysicsSystem::GetInstance()->DrawLine(start_center, end_center, DEBUG_COLOR, DEBUG_COLOR);
		PhysicsSystem::GetInstance()->DrawBox(end_center, end_rot, half_extents, DEBUG_COLOR);
#endif // USE_IMGUI

		// テストの実行
		const bool ret_val = ClosestConvexCast(hit_info, &shape, start_center, start_rot, end_center, end_rot, is_hit_trigger, ccd_penetration, layer_mask);

		// 結果
		return ret_val;
	}

	bool Physics::CheckBox(const VECTOR3& center, const Quaternion& rot, const VECTOR3& half_extents, const bool is_hit_trigger, const float threshold, int layer_mask)
	{
		// Shapeの設定
		btBoxShape shape(ConvertVectorTobtVector(half_extents));

		// コリジョンオブジェクトの作成
		btCollisionObject col_obj;
		col_obj.setCollisionShape(&shape);
		col_obj.setWorldTransform(btTransform(ConvertQuaternionTobtQuaternion(rot), ConvertVectorTobtVector(center)));

		// 衝突テスト
		const bool ret_val = CheckConvex(&col_obj, is_hit_trigger, threshold, layer_mask);

#ifdef USE_IMGUI
		// Boxのデバッグ表示
		PhysicsSystem::GetInstance()->DrawBox(center, rot, half_extents, ret_val ? DEBUG_HIT_COLOR : DEBUG_COLOR);
#endif // USE_IMGUI

		return ret_val;
	}

	bool Physics::OverlapBox(const VECTOR3& center, const Quaternion& rot, const VECTOR3& half_extents, std::vector<RaycastHit>& hit_infos, const bool is_hit_trigger, const float threshold, const int layer_mask)
	{
		// Shapeの設定
		btBoxShape shape(ConvertVectorTobtVector(half_extents));

		// コリジョンオブジェクトの作成
		btCollisionObject col_obj;
		col_obj.setCollisionShape(&shape);
		col_obj.setWorldTransform(btTransform(ConvertQuaternionTobtQuaternion(rot), ConvertVectorTobtVector(center)));

		// 衝突テスト
		const bool ret_val = OverlapConvex(hit_infos, &col_obj, is_hit_trigger, threshold, layer_mask);

#ifdef USE_IMGUI
		// Boxのデバッグ表示
		PhysicsSystem::GetInstance()->DrawBox(center, rot, half_extents, ret_val ? DEBUG_HIT_COLOR : DEBUG_COLOR);
#endif // USE_IMGUI

		return ret_val;
	}
#pragma endregion
	// ~Box cast

#pragma region Capsule cast
	bool Physics::CapsuleCast(const VECTOR3& point_1, const VECTOR3& point_2, const float radius, const VECTOR3& direction, const float max_distance, const bool is_hit_trigger, const float ccd_penetration, const int layer_mask)
	{
		// 長さは無限を超えない
		if (MyMath::IsInfinity(max_distance))
		{
			LOG_ASSERT("In CapsuleCast(). Capsule distance is over infinity.");
			return false;
		}

		// Shapeの設定
		VECTOR3 capsule_axis = point_2 - point_1;
		{
			// +Y Upにする
			capsule_axis = capsule_axis.y > 0.0f ? capsule_axis : -capsule_axis;
		}
		const float height = capsule_axis.Length();
		const btCapsuleShape shape = btCapsuleShape(static_cast<btScalar>(radius), static_cast<btScalar>(height));
		const VECTOR3 start_center = (point_2 + point_1) * 0.5f;
		const VECTOR3 end_center = start_center + direction * max_distance;
		const Quaternion capsule_orientation = Quaternion::FromToRotation(VECTOR3::Up, capsule_axis);
#ifdef USE_IMGUI
		// Boxのデバッグ表示
		PhysicsSystem::GetInstance()->DrawCapsule(start_center, capsule_orientation, radius, height, DEBUG_COLOR);
		PhysicsSystem::GetInstance()->DrawLine(start_center, end_center, DEBUG_COLOR, DEBUG_COLOR);
		PhysicsSystem::GetInstance()->DrawCapsule(end_center, capsule_orientation, radius, height, DEBUG_COLOR);
#endif // USE_IMGUI

		// テストの実行
		const bool ret_val = ClosestConvexCast(&shape, start_center, capsule_orientation, end_center, capsule_orientation, is_hit_trigger, ccd_penetration, layer_mask);

		// 結果
		return ret_val;
	}

	bool Physics::CapsuleCast(const VECTOR3& point_1, const VECTOR3& point_2, const float radius, const VECTOR3& direction, const float max_distance, RaycastHit& hit_info, const bool is_hit_trigger, const float ccd_penetration, const int layer_mask)
	{
		// 長さは無限を超えない
		if (MyMath::IsInfinity(max_distance))
		{
			LOG_ASSERT("In CapsuleCast(). Capsule distance is over infinity.");
			return false;
		}

		// Shapeの設定
		VECTOR3 capsule_axis = point_2 - point_1;
		{
			// +Y Upにする
			capsule_axis = capsule_axis.y > 0.0f ? capsule_axis : -capsule_axis;
		}
		const float height = capsule_axis.Length();
		const btCapsuleShape shape = btCapsuleShape(static_cast<btScalar>(radius), static_cast<btScalar>(height));
		const VECTOR3 start_center = (point_2 + point_1) * 0.5f;
		const VECTOR3 end_center = start_center + direction * max_distance;
		const Quaternion capsule_orientation = Quaternion::FromToRotation(VECTOR3::Up, capsule_axis);
#ifdef USE_IMGUI
		// Boxのデバッグ表示
		PhysicsSystem::GetInstance()->DrawCapsule(start_center, capsule_orientation, radius, height, DEBUG_COLOR);
		PhysicsSystem::GetInstance()->DrawLine(start_center, end_center, DEBUG_COLOR, DEBUG_COLOR);
		PhysicsSystem::GetInstance()->DrawCapsule(end_center, capsule_orientation, radius, height, DEBUG_COLOR);
#endif // USE_IMGUI

		// テストの実行
		const bool ret_val = ClosestConvexCast(hit_info, &shape, start_center, capsule_orientation, end_center, capsule_orientation, is_hit_trigger, ccd_penetration, layer_mask);

		// 結果
		return ret_val;
	}

	bool Physics::CheckCapsule(const VECTOR3& point_1, const VECTOR3& point_2, const float radius, const bool is_hit_trigger, const float threshold, const int layer_mask)
	{
		// Shapeの設定
		VECTOR3 capsule_axis = point_2 - point_1;
		{
			// +Y Upにする
			capsule_axis = capsule_axis.y > 0.0f ? capsule_axis : -capsule_axis;
		}
		const float height = capsule_axis.Length();
		btCapsuleShape shape = btCapsuleShape(static_cast<btScalar>(radius), static_cast<btScalar>(height));
		const VECTOR3 center = (point_2 + point_1) * 0.5f;
		const Quaternion orientation = Quaternion::FromToRotation(VECTOR3::Up, capsule_axis);

		// コリジョンオブジェクトの作成
		btCollisionObject col_obj;
		col_obj.setCollisionShape(&shape);
		col_obj.setWorldTransform(btTransform(ConvertQuaternionTobtQuaternion(orientation), ConvertVectorTobtVector(center)));

		// 衝突テスト
		const bool ret_val = CheckConvex(&col_obj, is_hit_trigger, threshold, layer_mask);

#ifdef USE_IMGUI
		// Capsuleのデバッグ表示
		PhysicsSystem::GetInstance()->DrawCapsule(center, orientation, radius, height, ret_val ? DEBUG_HIT_COLOR : DEBUG_COLOR);
#endif // USE_IMGUI

		return ret_val;
	}

	bool Physics::OverlapCapsule(const VECTOR3& point_1, const VECTOR3& point_2, const float radius, std::vector<RaycastHit>& hit_infos, const bool is_hit_trigger, const float threshold, const int layer_mask)
	{
		// Shapeの設定
		VECTOR3 capsule_axis = point_2 - point_1;
		{
			// +Y Upにする
			capsule_axis = capsule_axis.y > 0.0f ? capsule_axis : -capsule_axis;
		}
		const float height = capsule_axis.Length();
		btCapsuleShape shape = btCapsuleShape(static_cast<btScalar>(radius), static_cast<btScalar>(height));
		const VECTOR3 center = (point_2 + point_1) * 0.5f;
		const Quaternion orientation = Quaternion::FromToRotation(VECTOR3::Up, capsule_axis);

		// コリジョンオブジェクトの作成
		btCollisionObject col_obj;
		col_obj.setCollisionShape(&shape);
		col_obj.setWorldTransform(btTransform(ConvertQuaternionTobtQuaternion(orientation), ConvertVectorTobtVector(center)));

		// 衝突テスト
		const bool ret_val = OverlapConvex(hit_infos, &col_obj, is_hit_trigger, threshold, layer_mask);

#ifdef USE_IMGUI
		// Capsuleのデバッグ表示
		PhysicsSystem::GetInstance()->DrawCapsule(center, orientation, radius, height, ret_val ? DEBUG_HIT_COLOR : DEBUG_COLOR);
#endif // USE_IMGUI

		return ret_val;
	}
#pragma endregion
	// ~Capsule cast


	bool Physics::ClosestConvexCast(const btConvexShape* shape, const VECTOR3& start_pos, const Quaternion& start_rot, const VECTOR3& end_pos, const Quaternion& end_rot, const bool is_hit_trigger, const float ccd_penetration, const int layer_mask)
	{
		// Shapeがないときreturn
		if (!shape)
		{
			LOG_ASSERT("In ClosestConvexCast(). Shape is nullptr.");
			return false;
		}
		// 凸包でないときreturn
		if (!shape->isConvex())
		{
			LOG_ASSERT("In ClosestConvexCast(). Shape is not convex.");
			return false;
		}

		// コールバック作成
		TKGClosestConvexResultCallback convex_callback(
			ConvertVectorTobtVector(start_pos), ConvertVectorTobtVector(end_pos), is_hit_trigger);
		convex_callback.m_collisionFilterGroup = RAYCAST_LAYER;
		convex_callback.m_collisionFilterMask = layer_mask & SELECTABLE_LAYER_MASK;

		// convexテストの実行
		PhysicsSystem::GetInstance()->GetWorld()->convexSweepTest(
			shape,
			btTransform(ConvertQuaternionTobtQuaternion(start_rot), ConvertVectorTobtVector(start_pos)),
			btTransform(ConvertQuaternionTobtQuaternion(end_rot), ConvertVectorTobtVector(end_pos)),
			convex_callback,
			ccd_penetration
		);

		// 結果
		if (convex_callback.hasHit())
		{
#ifdef USE_IMGUI
			// 衝突位置の描画
			const auto seg = end_pos - start_pos;
			RenderHitShape(
				const_cast<btConvexShape*>(shape),
				convex_callback.m_closestHitFraction * seg.Length() * seg.Normalized() + start_pos,
				Quaternion::Lerp(start_rot, end_rot, static_cast<float>(convex_callback.m_closestHitFraction))
			);
#endif // USE_IMGUI
			return true;
		}
		else
		{
			return false;
		}
	}

	bool Physics::ClosestConvexCast(RaycastHit& hit_info, const btConvexShape* shape, const VECTOR3& start_pos, const Quaternion& start_rot, const VECTOR3& end_pos, const Quaternion& end_rot, const bool is_hit_trigger, const float ccd_penetration, const int layer_mask)
	{
		// Shapeがないときreturn
		if (!shape)
		{
			hit_info.position = VECTOR3::Zero;
			hit_info.normal = VECTOR3::Zero;
			hit_info.distance = MyMath::Infinity;
			hit_info.hit_fraction = 0.0f;
			hit_info.collider = nullptr;
			hit_info.rigidbody = nullptr;

			LOG_ASSERT("In ClosestConvexCast(). Shape is nullptr.");
			return false;
		}
		// 凸包でないときreturn
		if (!shape->isConvex())
		{
			hit_info.position = VECTOR3::Zero;
			hit_info.normal = VECTOR3::Zero;
			hit_info.distance = MyMath::Infinity;
			hit_info.hit_fraction = 0.0f;
			hit_info.collider = nullptr;
			hit_info.rigidbody = nullptr;

			LOG_ASSERT("In ClosestConvexCast(). Shape is not convex.");
			return false;
		}

		// コールバック作成
		TKGClosestConvexResultCallback convex_callback(
			ConvertVectorTobtVector(start_pos), ConvertVectorTobtVector(end_pos), is_hit_trigger);
		convex_callback.m_collisionFilterGroup = RAYCAST_LAYER;
		convex_callback.m_collisionFilterMask = layer_mask & SELECTABLE_LAYER_MASK;

		// convexテストの実行
		PhysicsSystem::GetInstance()->GetWorld()->convexSweepTest(
			shape,
			btTransform(ConvertQuaternionTobtQuaternion(start_rot), ConvertVectorTobtVector(start_pos)),
			btTransform(ConvertQuaternionTobtQuaternion(end_rot), ConvertVectorTobtVector(end_pos)),
			convex_callback,
			ccd_penetration
		);

		// 結果
		if (convex_callback.hasHit())
		{
			hit_info.position = ConvertbtVectorToVector(convex_callback.m_hitPointWorld);
			hit_info.normal = ConvertbtVectorToVector(convex_callback.m_hitNormalWorld);
			hit_info.distance = convex_callback.m_closestHitFraction * (end_pos - start_pos).Length();
			hit_info.hit_fraction = convex_callback.m_closestHitFraction;
			hit_info.collider = PhysicsSystem::GetInstance()->
				GetColliderForID(convex_callback.m_hitCollisionObject->getUserIndex());
			if (hit_info.collider && hit_info.collider->IsRigidBody())
			{
				hit_info.rigidbody = std::static_pointer_cast<RigidBody>(hit_info.collider);
			}
			else
			{
				hit_info.rigidbody = nullptr;
			}
#ifdef USE_IMGUI
			// 衝突位置の描画
			RenderHitShape(
				const_cast<btConvexShape*>(shape),
				hit_info.distance * (end_pos - start_pos).Normalized() + start_pos,
				Quaternion::Lerp(start_rot, end_rot, static_cast<float>(convex_callback.m_closestHitFraction))
			);
#endif // USE_IMGUI

			return true;
		}
		else
		{
			hit_info.position = VECTOR3::Zero;
			hit_info.normal = VECTOR3::Zero;
			hit_info.distance = MyMath::Infinity;
			hit_info.hit_fraction = 0.0f;
			hit_info.collider = nullptr;
			hit_info.rigidbody = nullptr;

			return false;
		}
	}

	bool Physics::CheckConvex(btCollisionObject* col_obj, const bool is_hit_trigger, const float threshold, const int layer_mask)
	{
		// コールバック作成
		TKGContactResultCallback callback(threshold, is_hit_trigger);
		callback.m_collisionFilterGroup = RAYCAST_LAYER;
		callback.m_collisionFilterMask = layer_mask & SELECTABLE_LAYER_MASK;

		// 衝突テスト
		PhysicsSystem::GetInstance()->GetWorld()->contactTest(col_obj, callback);

		// テスト結果
		if (callback.hasHit())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool Physics::OverlapConvex(std::vector<RaycastHit>& hit_infos, btCollisionObject* col_obj, const bool is_hit_trigger, const float threshold, const int layer_mask)
	{
		// 返す情報のクリア
		hit_infos.clear();

		// コールバック作成
		TKGAllContactResultCallback callback(threshold, is_hit_trigger);
		callback.m_collisionFilterGroup = RAYCAST_LAYER;
		callback.m_collisionFilterMask = layer_mask & SELECTABLE_LAYER_MASK;

		// 衝突テスト
		PhysicsSystem::GetInstance()->GetWorld()->contactTest(col_obj, callback);

		// 結果
		const int callback_size = callback.m_collisionObjects.size();
		// 衝突なしなら
		if (callback_size <= 0)
			return false;
		// 情報を詰め込む
		hit_infos.resize(callback_size);
		for (int i = 0; i < callback_size; ++i)
		{
			auto& info = hit_infos.at(i);

			info.position = ConvertbtVectorToVector(callback.m_hitPointWorld[i]);
			info.normal = ConvertbtVectorToVector(callback.m_hitNormalWorld[i]);
			info.distance = static_cast<float>(callback.m_distances[i]);
			info.hit_fraction = 0.0f;
			info.collider = PhysicsSystem::GetInstance()->
				GetColliderForID(callback.m_collisionObjects[i]->getUserIndex());
			if (info.collider && info.collider->IsRigidBody())
			{
				info.rigidbody = std::static_pointer_cast<RigidBody>(info.collider);
			}
			else
			{
				info.rigidbody = nullptr;
			}
#ifdef USE_IMGUI
			// 衝突位置の描画
			PhysicsSystem::GetInstance()->DrawContactPoint(info.position, info.normal, MyMath::Abs(info.distance), DEBUG_CONTACT_POINT_COLOR);
#endif // USE_IMGUI
		}

		return true;
	}

	void Physics::RenderHitShape(btConvexShape* shape, const VECTOR3& pos, const Quaternion& rot)
	{
		switch (shape->getShapeType())
		{
			case BroadphaseNativeTypes::BOX_SHAPE_PROXYTYPE:
			{
				const btBoxShape* box = static_cast<btBoxShape*>(shape);
				PhysicsSystem::GetInstance()->DrawBox(pos, rot, ConvertbtVectorToVector(box->getHalfExtentsWithoutMargin()), DEBUG_HIT_COLOR);
			}
			break;

			case BroadphaseNativeTypes::SPHERE_SHAPE_PROXYTYPE:
			{
				const btSphereShape* sphere = static_cast<btSphereShape*>(shape);
				PhysicsSystem::GetInstance()->DrawSphere(pos, static_cast<float>(sphere->getRadius()), DEBUG_HIT_COLOR);
			}
			break;

			case BroadphaseNativeTypes::CAPSULE_SHAPE_PROXYTYPE:
			{
				const btCapsuleShape* capsule = static_cast<btCapsuleShape*>(shape);
				PhysicsSystem::GetInstance()->DrawCapsule(pos, rot, static_cast<float>(capsule->getRadius()), static_cast<float>(capsule->getHalfHeight() * 2.0), DEBUG_HIT_COLOR);
			}
			break;
		}
	}

}// namespace TKGEngine