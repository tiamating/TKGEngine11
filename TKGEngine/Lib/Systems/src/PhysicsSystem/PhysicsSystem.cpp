

#include "../../inc/PhysicsSystem.h"
#include "IBulletDebugDraw.h"

#include "Systems/inc/IGUI.h"

#include "Application/Objects/Components/inc/CCollider.h"
#include "Application/Objects/Components/interface/ICollider.h"

#include <cassert>

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Static variable declaration
	////////////////////////////////////////////////////////
	PhysicsSystem* PhysicsSystem::m_instance = nullptr;


	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	bool PhysicsSystem::Create()
	{
		if (m_instance == nullptr)
		{
			m_instance = new PhysicsSystem;
			return m_instance->Initialize();
		}

		assert(0 && "PhysicsSystem::Create() has called twice.");
		return false;
	}

	void PhysicsSystem::Destroy()
	{
		if (m_instance)
		{
			m_instance->Finalize();
			delete m_instance;
			m_instance = nullptr;
		}
	}

	void PhysicsSystem::FrameUpdate(const float elapsed_time)
	{
		if (m_instance)
		{
			m_instance->Update(elapsed_time);
		}
	}

	PhysicsSystem* PhysicsSystem::GetInstance()
	{
		return m_instance;
	}

#ifdef USE_IMGUI
	void PhysicsSystem::DebugDraw()
	{
		if (m_instance)
		{
			m_instance->OnDebugDraw();
		}
	}

	void PhysicsSystem::OpenLayer()
	{
		m_is_gui_opened = true;
	}

	void PhysicsSystem::OnGUILayer()
	{
		// ウィンドウが開かれていない
		if (!m_is_gui_opened)
			return;

		// レイヤー設定ウィンドウ表示
		ImGui::IDWrapped id("PhysicsSystem");
		const ImVec2 window_size(300.0f, 450.0f);
		ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);
		constexpr ImGuiWindowFlags flags =
			ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize ^ ImGuiWindowFlags_NoCollapse;
		if (ImGui::Begin("Collision Layer Mask", &m_is_gui_opened, flags))
		{
			constexpr int layer_num = static_cast<int>(Layer::MAX_USER_LAYER_NUM);
			static int selection_layer = 0;

			// 設定するレイヤーの選択
			if(ImGui::BeginCombo("Layer", NAMEOF_ENUM(static_cast<Layer>(selection_layer)).data()))
			{
				for (int i = 0; i < static_cast<int>(Layer::MAX_USER_LAYER_NUM); ++i)
				{
					const bool is_selected = (selection_layer == i);
					if (ImGui::Selectable(NAMEOF_ENUM(static_cast<Layer>(i)).data(),is_selected))
					{
						selection_layer = i;
					}
				}
				ImGui::EndCombo();
			}
			ImGui::Separator();

			constexpr ImGuiTreeNodeFlags tree_flags =
				ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
			constexpr ImGuiWindowFlags child_flags = ImGuiWindowFlags_HorizontalScrollbar;
			ImGui::Indent(window_size.x * 0.05f);
			// 子ウィンドウサイズ
			ImVec2 child_window_size;
			child_window_size.x = window_size.x * 0.85f;
			child_window_size.y =
				window_size.y - (ImGui::GetCursorPosY() + ImGui::DEFAULT_SPACE_SIZE * 2.0f + ImGui::ENABLE_SIZE_FROM_BOTTOM);
			// 子ウィンドウ
			ImGui::BeginChild("Child Window", child_window_size, true, child_flags);
			{
				for (int i = 0; i < layer_num; ++i)
				{
					// 選択結果[i][j]を[j][i]にも代入する
					ImGui::IDWrapped select_id(i);
					if (ImGui::Checkbox("##is collision", &m_layer_mask[selection_layer][i]))
					{
						m_layer_mask[i][selection_layer] = m_layer_mask[selection_layer][i];
					}
					ImGui::SameLine();
					ImGui::TreeNodeEx(NAMEOF_ENUM(static_cast<Layer>(i)).data(), tree_flags);
					{
						if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
						{
							m_layer_mask[selection_layer][i] = !m_layer_mask[selection_layer][i];
							m_layer_mask[i][selection_layer] = m_layer_mask[selection_layer][i];
						}
					}
				}
			}
			ImGui::EndChild();
			ImGui::Unindent(window_size.x * 0.05f);

			// レイヤーマスク再計算のボタン
			ImVec2 button_size;
			{
				const float width = ImGui::GetWindowWidth();
				constexpr float button_space = 0.2f;
				button_size.x = width * (1.0f - button_space * 2.0f);
				button_size.y = ImGui::DEFAULT_SPACE_SIZE * 1.5f;
				// ボタンを中央に置く
				ImGui::SetCursorPosX(width * button_space);
			}
			if (ImGui::Button("Create Layer Mask", button_size))
			{
				CreateLayerMaskParam();
			}
		}
		ImGui::End();
	}
#endif // USE_IMGUI

	void PhysicsSystem::RegisterRigidBody(std::shared_ptr<Collider>& collider, btRigidBody* rigid_body)
	{
		const int collider_id = collider->GetInstanceID();
		// コリジョンオブジェクトに固有のIDを割り振る
		rigid_body->setUserIndex(collider_id);

		// 管理用配列に登録
		// Colliderの登録
		m_collider_map.emplace(collider_id, collider);
		// ColliderIDの登録
		m_goid_collision_map[collider->GetOwnerID()].emplace(collider_id);
	}

	void PhysicsSystem::UnregisterRigidBody(btRigidBody* rigid_body)
	{
		// Worldから消去
		m_dynamics_world->removeRigidBody(rigid_body);

		// 管理用配列から消去
		const auto itr_find = m_collider_map.find(rigid_body->getUserIndex());
		// Colliderが見つかったら
		if (itr_find != m_collider_map.end())
		{
			const auto goid_itr_find = m_goid_collision_map.find(itr_find->second->GetGameObjectID());
			// OwnerIDが見つかったら
			if (goid_itr_find != m_goid_collision_map.end())
			{
				// リストからColliderIDを削除
				const int collider_id = itr_find->second->GetColliderID();
				const auto col_itr = goid_itr_find->second.find(collider_id);
				if (col_itr != goid_itr_find->second.end())
				{
					goid_itr_find->second.erase(col_itr);
				}

				// 要素がなくなったものは削除する
				if (goid_itr_find->second.empty() == true)
				{
					m_goid_collision_map.erase(goid_itr_find);
				}
			}
			// Colliderの削除
			m_collider_map.erase(itr_find);
		}
	}

	void PhysicsSystem::RegisterGhostObject(std::shared_ptr<Collider>& collider, btGhostObject* ghost_obj)
	{
		const int collider_id = collider->GetInstanceID();
		// コリジョンオブジェクトに固有のIDを割り振る
		ghost_obj->setUserIndex(collider->GetInstanceID());

		// 管理用配列に登録
		// Colliderの登録
		m_collider_map.emplace(collider_id, collider);
		// ColliderIDの登録
		m_goid_collision_map[collider->GetOwnerID()].emplace(collider_id);
	}

	void PhysicsSystem::UnregisterGhostObject(btGhostObject* ghost_obj)
	{
		// Worldから消去
		m_dynamics_world->removeCollisionObject(ghost_obj);

		// 管理用配列から消去
		const auto itr_find = m_collider_map.find(ghost_obj->getUserIndex());
		// Colliderが見つかったら
		if (itr_find != m_collider_map.end())
		{
			const auto goid_itr_find = m_goid_collision_map.find(itr_find->second->GetGameObjectID());
			// OwnerIDが見つかったら
			if (goid_itr_find != m_goid_collision_map.end())
			{
				// リストからColliderIDを削除
				const int collider_id = itr_find->second->GetColliderID();
				const auto col_itr = goid_itr_find->second.find(collider_id);
				if (col_itr != goid_itr_find->second.end())
				{
					goid_itr_find->second.erase(col_itr);
				}

				// 要素がなくなったものは削除する
				if (goid_itr_find->second.empty())
				{
					m_goid_collision_map.erase(goid_itr_find);
				}
			}
			// Colliderの削除
			m_collider_map.erase(itr_find);
		}
	}

	void PhysicsSystem::AddAction(btActionInterface* action) const
	{
		m_dynamics_world->addAction(action);
	}

	void PhysicsSystem::RemoveAction(btActionInterface* action) const
	{
		m_dynamics_world->removeAction(action);
	}

	void PhysicsSystem::SetCollisionActive(const GameObjectID goid, const bool is_active)
	{
		// GameObjectIDで探索
		const auto itr_find = m_goid_collision_map.find(goid);
		// 早期リターン
		if (itr_find == m_goid_collision_map.end())
		{
			return;
		}

		// ColliderIDからColliderの関数を呼ぶ
		const auto col_itr_end = m_collider_map.end();
		for (const auto& id : itr_find->second)
		{
			const auto col_itr = m_collider_map.find(id);
			// 終端確認
			if (col_itr == col_itr_end)
				continue;
			// アクティブをTrueにする時は全ての条件がTrueでないといけない
			if (is_active)
			{
				// アクティブチェック
				if (!col_itr->second->IsActiveCollider())
					continue;
			}
			// Colliderの有効、無効変更時の関数を呼ぶ
			col_itr->second->OnSetCollisionActive(is_active);
		}
	}

	void PhysicsSystem::SetWorldGravity(const VECTOR3& gravity)
	{
		m_gravity = gravity;
		m_dynamics_world->setGravity(ConvertVectorTobtVector(gravity));
	}

	void PhysicsSystem::CreateLayerMaskParam()
	{
		constexpr int MAX_USER_LAYER_NUM = static_cast<int>(Layer::MAX_USER_LAYER_NUM);
		constexpr int initialize_layer_mask_param = static_cast<int>(Layer::AllFilter);
		// レイヤーごとに計算する
		for (int i = 0; i < MAX_USER_LAYER_NUM; ++i)
		{
			// 初期はすべて衝突(0~31ビットを立てる)
			m_layer_mask_param[i] = initialize_layer_mask_param;

			// 32bitのビットシフトを使用してレイヤーマスクの値を計算する
			for (int j = 0; j < MAX_USER_LAYER_NUM; ++j)
			{
				// 判定を行わない組み合わせのフラグはおろす
				if (!m_layer_mask[i][j])
				{
					m_layer_mask_param[i] &= ~(1 << j);
				}
			}
		}
	}

	int PhysicsSystem::GetLayerMask(Layer layer)
	{
		return m_layer_mask_param[static_cast<int>(layer)];
	}

	std::shared_ptr<Collider> PhysicsSystem::GetColliderForID(const int col_id)
	{
		const auto itr_find = m_collider_map.find(col_id);
		if (itr_find == m_collider_map.end())
		{
			return std::shared_ptr<Collider>();
		}
		else
		{
			return std::static_pointer_cast<Collider>(itr_find->second);
		}
	}

	void PhysicsSystem::DrawLine(const VECTOR3& from, const VECTOR3& to, const VECTOR3& from_color, const VECTOR3& to_color) const
	{
#ifdef USE_IMGUI
		if (!m_bullet_debug_draw)
			return;
		m_bullet_debug_draw->DrawLine(from, to, from_color, to_color);
#endif // USE_IMGUI
	}

	void PhysicsSystem::DrawSphere(const VECTOR3& pos, const float radius, const VECTOR3& color) const
	{
#ifdef USE_IMGUI
		if (!m_bullet_debug_draw)
			return;
		m_bullet_debug_draw->DrawSphere(pos, radius, color);
#endif // USE_IMGUI
	}

	void PhysicsSystem::DrawBox(const VECTOR3& pos, const Quaternion& rot, const VECTOR3& half_extents, const VECTOR3& color) const
	{
#ifdef USE_IMGUI
		if (!m_bullet_debug_draw)
			return;
		m_bullet_debug_draw->DrawBox(pos, rot, half_extents, color);
#endif // USE_IMGUI
	}

	void PhysicsSystem::DrawCapsule(const VECTOR3& pos, const Quaternion& rot, const float radius, const float height, const VECTOR3& color) const
	{
#ifdef USE_IMGUI
		if (!m_bullet_debug_draw)
			return;
		m_bullet_debug_draw->DrawCapsule(pos, rot, radius, height, color);
#endif // USE_IMGUI
	}

	void PhysicsSystem::DrawCone(const VECTOR3& pos, const Quaternion& rot, const float radius, const float height, const VECTOR3& color) const
	{
#ifdef USE_IMGUI
		if (!m_bullet_debug_draw)
			return;
		m_bullet_debug_draw->DrawCone(pos, rot, radius, height, color);
#endif // USE_IMGUI
	}

	void PhysicsSystem::DrawContactPoint(const VECTOR3& pos, const VECTOR3& normal, const float distance, const VECTOR3& color) const
	{
#ifdef USE_IMGUI
		if (!m_bullet_debug_draw)
			return;
		m_bullet_debug_draw->DrawContactPoint(pos, normal, distance, color);
#endif // USE_IMGUI
	}

	PhysicsSystem::PhysicsSystem()
	{
		constexpr int MAX_USER_LAYER_NUM = static_cast<int>(Layer::MAX_USER_LAYER_NUM);
		// ビットマスクの初期化
		for (int i = 0; i < MAX_USER_LAYER_NUM; ++i)
		{
			for (int j = 0; j < MAX_USER_LAYER_NUM; ++j)
			{
				m_layer_mask[i][j] = true;
			}
		}
	}

	bool PhysicsSystem::Initialize()
	{
		// 衝突検出方法の選択
		{
			// コリジョン計算の設定
			btDefaultCollisionConstructionInfo cci;
			m_collision_configuration = std::make_unique<btDefaultCollisionConfiguration>(cci);

			// Collision dispatcher
			m_collision_dispatcher =
				std::make_unique<btCollisionDispatcher>(m_collision_configuration.get());
		}
		// ブロードフェーズ法の設定
		m_broadphase = std::make_unique<btDbvtBroadphase>();

		// 拘束(剛体間リンク)のソルバ設定
		m_solver = std::make_unique<btSequentialImpulseConstraintSolver>();

		// Bulletのワールド作成
		m_dynamics_world = std::make_unique<btDiscreteDynamicsWorld>
			(
				m_collision_dispatcher.get(),
				m_broadphase.get(),
				m_solver.get(),
				m_collision_configuration.get()
				);
		
		m_dynamics_world->setGravity(ConvertVectorTobtVector(m_gravity));

		//ゴーストペアコールバック
		m_ghost_pair_call = std::make_unique<btGhostPairCallback>();
		m_dynamics_world->getPairCache()->setInternalGhostPairCallback(m_ghost_pair_call.get());

		// ブロードフェーズフィルターコールバック
		m_broadphase_filter_callback = std::make_unique<BroadphaseFilterCallback>();
		m_dynamics_world->getPairCache()->setOverlapFilterCallback(m_broadphase_filter_callback.get());

#ifdef USE_IMGUI
		// Bulletデバッグ表示
		m_bullet_debug_draw = std::make_unique<IBulletDebugDraw>();
		m_bullet_debug_draw->Initialize();
		m_bullet_debug_draw->SetDebugMode
		(
			btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawContactPoints
		);
		m_dynamics_world->setDebugDrawer(m_bullet_debug_draw.get());
#endif // USE_IMGUI

		// レイヤーマスクの値を計算する
		CreateLayerMaskParam();

		return true;
	}

	void PhysicsSystem::Finalize()
	{
		//Bulletのオブジェクトは終端から消していくと
		//イテレータ(index)をそのまま利用できる
		// Bulletは配列で管理
		//  Removeの処理は最後の要素とswapしてサイズを1つ減らすため、終端から削除するほうが高速
		{
			const int constraint_num = m_dynamics_world->getNumConstraints();
			for (int i = constraint_num - 1; i >= 0; --i)
			{
				auto* constraint = m_dynamics_world->getConstraint(i);
				m_dynamics_world->removeConstraint(constraint);
				delete constraint;
			}

			const int col_num = m_dynamics_world->getNumCollisionObjects();
			for (int i = col_num - 1; i >= 0; --i)
			{
				btCollisionObject* obj = m_dynamics_world->getCollisionObjectArray()[i];
				m_dynamics_world->removeCollisionObject(obj);
				btRigidBody* rigid = btRigidBody::upcast(obj);
				if (rigid != nullptr)
				{
					delete rigid->getMotionState();
				}
				delete obj;
			}
		}

		// btBroadphaseInterfaceなどワールド作成時に指定したものは
		// btDiscreteDynamicsWorldクラスのデストラクタで使用するため後で削除
		m_dynamics_world.reset();

#ifdef USE_IMGUI
		m_bullet_debug_draw.reset();
#endif // USE_IMGUI
		m_broadphase_filter_callback.reset();
		m_ghost_pair_call.reset();
		m_solver.reset();
		m_broadphase.reset();
		m_collision_dispatcher.reset();
		m_collision_configuration.reset();
	}

	void PhysicsSystem::Update(const float elapsed_time)
	{
		// Physics Worldの更新
		m_dynamics_world->stepSimulation(elapsed_time, 1, elapsed_time);

#ifdef USE_IMGUI
		// 実行中以外は呼ばれない
		if (!IGUI::Get().IsPlaying())
		{
			return;
		}
#endif // USE_IMGUI

		// 衝突状態を取得して更新する
		UpdateCollisionState();

		// 衝突状態に応じた関数を呼ぶ
		CallCollisionFunction();
	}

#ifdef USE_IMGUI
	void PhysicsSystem::OnDebugDraw() const
	{
		// TODO : これを呼ぶ前にカメラ情報をCBufferに送る
		m_dynamics_world->debugDrawWorld();
	}
#endif // USE_IMGUI

	void PhysicsSystem::UpdateCollisionState()
	{
		auto* dispatcher = m_dynamics_world->getDispatcher();
		const int num_manifolds = dispatcher->getNumManifolds();
		for (int i = 0; i < num_manifolds; ++i)
		{
			const btPersistentManifold* contact_manifold = dispatcher->getManifoldByIndexInternal(i);
			// 衝突点がなければ早期リターン
			const int contact_count = contact_manifold->getNumContacts();
			if (contact_count == 0)
			{
				continue;
			}

			const btCollisionObject* obj_a = contact_manifold->getBody0();
			const btCollisionObject* obj_b = contact_manifold->getBody1();

			// キャラクターコントローラと衝突した場合は関数は呼ばない
			if ((obj_a->getCollisionFlags() & btCollisionObject::CollisionFlags::CF_CHARACTER_OBJECT) != 0 ||
				(obj_b->getCollisionFlags() & btCollisionObject::CollisionFlags::CF_CHARACTER_OBJECT) != 0)
			{
				continue;
			}

			// CollisionIDからColliderコンポーネントを取得
			const auto& col_a = m_collider_map.at(obj_a->getUserIndex());
			const auto& col_b = m_collider_map.at(obj_b->getUserIndex());

			// 関数を呼ぶかどうか
			const bool do_call_func_a = col_a->IsCallOnFunction();
			const bool do_call_func_b = col_b->IsCallOnFunction();
			// 衝突相手にTriggerが含まれているか、キネマティック同士ならTriggerリストに追加
			if ((col_a->IsKinematic() && col_b->IsKinematic()) || col_a->IsTrigger() || col_b->IsTrigger())
			{
				if (do_call_func_a)
					col_a->AddTriggerHitID(obj_b->getUserIndex());

				if (do_call_func_b)
					col_b->AddTriggerHitID(obj_a->getUserIndex());
			}
			// どちらもCollisionならCollisionリストに追加
			else
			{
				if (do_call_func_a)
					col_a->AddCollisionHitID(obj_b->getUserIndex());

				if (do_call_func_b)
					col_b->AddCollisionHitID(obj_a->getUserIndex());
			}
		}
	}

	void PhysicsSystem::CallCollisionFunction()
	{
		// Colliderリストから衝突相手の更新をする
		for (const auto& col : m_collider_map)
		{
			// オブジェクトのアクティブチェック
			if (!col.second->IsActiveCollider())
				continue;

			// 関数を呼ばないなら早期リターン
			if (!col.second->IsCallOnFunction())
				continue;

			// それぞれの更新をする
			col.second->UpdateHitState();
		}
	}
}