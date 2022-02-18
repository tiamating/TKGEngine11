#include "../inc/CCollider.h"

#include "../inc/CTransform.h"

#include "Systems/inc/PhysicsSystem.h"
#include "Managers/MonoBehaviourManager.h"

#include <unordered_map>
#include <utility>



namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	Collider::Collider()
		: Behaviour(COMPONENT_TYPE::COMPONENT_COLLIDER)
	{
		/* nothing */
	}

	Collider::~Collider()
	{
		/* nothing */
	}

#ifdef USE_IMGUI
	void Collider::OnGUI()
	{
		// 派生先でPushIDして使用

		// Behaviour::OnGUI
		Behaviour::OnGUI();

		// Call Function
		ImGui::Checkbox("Call collision function", &m_do_call_function);
		ImGui::SameLine();
		ImGui::HelpMarker("If it is true, call OnCollision and OnTrigger function.");
	}

	void Collider::ShapeGUI()
	{
		constexpr ImGuiTreeNodeFlags tree_flags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
		if (ImGui::TreeNodeEx("Shapes", tree_flags))
		{
			constexpr ImGuiTabBarFlags tab_bar_flags =
				ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_FittingPolicyResizeDown;
			if (ImGui::BeginTabBar("ShapeTagBar", tab_bar_flags))
			{
				// 追加用のボタン
				{
					if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
					{
						ImGui::OpenPopup("Add Shape");
					}
					if (ImGui::BeginPopup("Add Shape"))
					{
						// Box
						if (ImGui::Selectable("Box"))
						{
							AddShape(ShapeType::Box);
						}
						// Sphere
						if (ImGui::Selectable("Sphere"))
						{
							AddShape(ShapeType::Sphere);
						}
						// Capsule
						if (ImGui::Selectable("Capsule"))
						{
							AddShape(ShapeType::Capsule);
						}
						// Cylinder
						if (ImGui::Selectable("Cylinder"))
						{
							AddShape(ShapeType::Cylinder);
						}
						// Cone
						if (ImGui::Selectable("Cone"))
						{
							AddShape(ShapeType::Cone);
						}
						ImGui::EndPopup();
					}
				}
				// タブ表示
				constexpr ImGuiTabItemFlags tab_item_flags = ImGuiTabItemFlags_None;
				for (size_t i = 0; i < m_shapes.size(); ++i)
				{
					ImGui::PushID(i);
					bool is_open = true;
					if (ImGui::BeginTabItem(m_shapes.at(i)->GetShapeTypeName(), &is_open, tab_item_flags))
					{
						if (m_shapes.at(i)->OnGUI())
						{
							// 子形状にTransformの更新があった場合
							m_compound_shape->updateChildTransform(i, m_shapes.at(i)->GetbtTransform(), true);
							RecalculateBody();
						}
						ImGui::EndTabItem();
					}
					// タブを閉じたときは、Shapeを削除する
					if (!is_open)
					{
						RemoveShape(m_shapes.at(i)->GetShapeID());
					}
					ImGui::PopID();
				}
				ImGui::EndTabBar();
			}
			ImGui::TreePop();
		}
	}
#endif // USE_IMGUI

	bool Collider::IsStatic() const
	{
		return m_is_static;
	}

	bool Collider::IsKinematic() const
	{
		return m_is_kinematic;
	}

	void Collider::OnCreate(const std::shared_ptr<Object>& p_self)
	{
		// Compound Shapeの作成
		m_compound_shape = std::make_unique<btCompoundShape>();

		// 派生先ごとの生成処理
		OnCreated(p_self);
	}

	void Collider::OnDestroy()
	{
		// 派生先ごとの破棄処理
		OnDestroyed();

		// Compound Shapeの破棄
		m_compound_shape.reset();
	}

	void Collider::UpdateChildShape(ShapeID id)
	{
		// 複合形状からID探索
		const int child_index = FindShapeFromCompound(id);
		if (child_index < 0)
			return;

		// Shape DataからID探索
		const int data_index = FindShapeDataIndex(id);
		if (data_index < 0)
			return;

		// Shapeの更新
		m_compound_shape->updateChildTransform(child_index, m_shapes.at(data_index)->GetbtTransform(), true);
	}

	void Collider::AddTriggerHitID(int other)
	{
		m_current_trigger_list.emplace(other);
	}

	void Collider::AddCollisionHitID(int other)
	{
		m_current_collision_list.emplace(other);
	}

	void Collider::UpdateHitState()
	{
		// Trigger
		UpdateTriggerList();
		// 自身がTriggerならCollisionは呼ばれない
		if (IsTrigger())
		{
			return;
		}

		// Collision
		UpdateCollisionList();
	}

	void Collider::UpdateTriggerList()
	{
		// Colliderマップを取得しておく
		const auto* collider_map = PhysicsSystem::GetInstance()->GetColliderMap();

		// prevリストの先頭から探索する
		{
			auto itr = m_prev_trigger_list.begin();
			const auto itr_prev_end = m_prev_trigger_list.end();
			const auto itr_current_end = m_current_trigger_list.end();
			for (; itr != itr_prev_end;)
			{
				// prevに存在したものがcurrentに存在するかチェック
				const auto itr_find = m_current_trigger_list.find(*itr);
				// ColliderIDからColliderコンポーネントを取得する
				auto other = std::static_pointer_cast<Collider>(collider_map->find(*itr)->second);
				// prevリストに存在し、currentリストに存在するなら
				if (itr_find != itr_current_end)
				{
					// OnTriggerStayを呼ぶ
					MonoBehaviourManager::OnTriggerStay(GetOwnerID(), other);

					// currentリストから消去する
					m_current_trigger_list.erase(itr_find);

					++itr;
				}
				// prevに存在したものがcurrentに存在しないなら
				else
				{
					// OnTriggerExitを呼ぶ
					MonoBehaviourManager::OnTriggerExit(GetOwnerID(), other);

					// prevリストから消去する
					itr = m_prev_trigger_list.erase(itr);
				}
			}
		}

		// currentリストに残っているIDのOnTriggerEnterを呼ぶ
		{
			for (const auto& id : m_current_trigger_list)
			{
				// ColliderIDからColliderコンポーネントを取得する
				auto other = std::static_pointer_cast<Collider>(collider_map->find(id)->second);

				// OnTriggerEnterを呼ぶ
				MonoBehaviourManager::OnTriggerEnter(GetOwnerID(), other);

				// currentリストのIDをprevリストに入れる
				m_prev_trigger_list.emplace(id);
			}

			// currentリストをクリアする
			m_current_trigger_list.clear();
		}
	}

	void Collider::UpdateCollisionList()
	{
		// Colliderマップを取得しておく
		const auto* collider_map = PhysicsSystem::GetInstance()->GetColliderMap();

		// prevリストの先頭から探索する
		{
			auto itr = m_prev_collision_list.begin();
			const auto itr_prev_end = m_prev_collision_list.end();
			const auto itr_current_end = m_current_collision_list.end();
			for (; itr != itr_prev_end;)
			{
				// prevに存在したものがcurrentに存在するかチェック
				const auto itr_find = m_current_collision_list.find(*itr);
				// ColliderIDからColliderコンポーネントを取得する
				auto other = std::static_pointer_cast<Collider>(collider_map->find(*itr)->second);
				// prevリストに存在し、currentリストに存在するなら
				if (itr_find != itr_current_end)
				{
					// OnCollisionStayを呼ぶ
					MonoBehaviourManager::OnCollisionStay(GetOwnerID(), other);

					// currentリストから消去する
					m_current_collision_list.erase(itr_find);

					++itr;
				}
				// prevに存在したものがcurrentに存在しないなら
				else
				{
					// OnCollisionExitを呼ぶ
					MonoBehaviourManager::OnCollisionExit(GetOwnerID(), other);

					// prevリストから消去する
					itr = m_prev_collision_list.erase(itr);
				}
			}
		}

		// currentリストに残っているIDのOnCollisionEnterを呼ぶ
		{
			for (const auto& id : m_current_collision_list)
			{
				// ColliderIDからColliderコンポーネントを取得する
				auto other = std::static_pointer_cast<Collider>(collider_map->find(id)->second);

				// OnCollisionEnterを呼ぶ
				MonoBehaviourManager::OnCollisionEnter(GetOwnerID(), other);

				// currentリストのIDをprevリストに入れる
				m_prev_collision_list.emplace(id);
			}

			// currentリストをクリアする
			m_current_collision_list.clear();
		}
	}

	int Collider::GetGameObjectID() const
	{
		return GetOwnerID();
	}

	int Collider::GetColliderID() const
	{
		return GetInstanceID();
	}

	int Collider::FindShapeDataIndex(const ShapeID id) const
	{
		// IDの存在チェック
		int idx = -1;
		const int data_size = m_shapes.size();
		for (int i = 0; i < data_size; ++i)
		{
			// IDが一致したらbreak
			if (m_shapes.at(i)->GetShapeID() == id)
			{
				idx = i;
				break;
			}
		}

		return idx;
	}

	int Collider::FindShapeFromCompound(const ShapeID id) const
	{
		// IDの存在チェック
		int idx = -1;
		const int data_size = m_compound_shape->getNumChildShapes();
		for (int i = 0; i < data_size; ++i)
		{
			// IDが一致したらbreak
			if (m_compound_shape->getChildShape(i)->getUserIndex() == id)
			{
				idx = i;
				break;
			}
		}

		return idx;
	}

	void Collider::RecalculateShapeAABB()
	{
		// 子形状のつけなおし
		{
			// 取り外す
			const int child_cnt = m_compound_shape->getNumChildShapes();
			for (int i = child_cnt - 1; i >= 0; --i)
			{
				m_compound_shape->removeChildShapeByIndex(i);
			}

			// つける
			const size_t shape_cnt = m_shapes.size();
			for (const auto& shape : m_shapes)
			{
				m_compound_shape->addChildShape(shape->GetbtTransform(), shape->GetPtr());
			}
		}

		// 複合形状のAABBを再計算
		m_compound_shape->recalculateLocalAabb();
	}


	ShapeID Collider::AddShape(ShapeType shape_type, const VECTOR3& translate, const Quaternion& quat)
	{
		// Shapeを作成
		std::unique_ptr<ShapeBase> new_shape;
		switch (shape_type)
		{
			case ShapeType::Box:
				new_shape = std::make_unique<BoxShape>();
				break;
			case ShapeType::Sphere:
				new_shape = std::make_unique<SphereShape>();
				break;
			case ShapeType::Capsule:
				new_shape = std::make_unique<CapsuleShape>();
				break;
			case ShapeType::Cylinder:
				new_shape = std::make_unique<CylinderShape>();
				break;
			case ShapeType::Cone:
				new_shape = std::make_unique<ConeShape>();
				break;
				// TODO : メッシュコライダー追加予定
				//case ShapeType::StaticMesh:
				//	break;
				//case ShapeType::ConvexPolytope:
				//	break;
		}
		const ShapeID ret_id = new_shape->CreateShape(translate, quat);

		// CompoundShapeに登録
		{
			const btTransform transform = new_shape->GetbtTransform();
			m_compound_shape->addChildShape(transform, new_shape->GetPtr());
		}

		// 形状データ配列に追加
		m_shapes.emplace_back(std::move(new_shape));

		// CompoundShapeのAABBを更新
		RecalculateShapeAABB();

		return ret_id;
	}

	void Collider::RemoveShape(ShapeID id)
	{
		// IDのチェック
		const int idx = FindShapeDataIndex(id);
		// IDが存在しなかったら早期リターン
		if (idx < 0)
		{
			return;
		}

		// CompoundShapeから削除
		{
			// 削除時に内部でreculculateLocallAabb()が呼ばれている
			m_compound_shape->removeChildShape(m_shapes.at(idx)->GetPtr());
		}

		// 末尾にスワップしてから削除する
		{
			// idxが末尾の要素でなければ
			const int shape_max_idx = static_cast<int>(m_shapes.size()) - 1;
			if (idx < shape_max_idx)
			{
				// 削除する要素と末尾の要素を入れ替える
				std::swap(m_shapes.at(idx), m_shapes.at(shape_max_idx));
			}

			// 末尾の削除
			m_shapes.pop_back();
		}
	}

	void Collider::SetShapeTranslation(ShapeID id, const VECTOR3& translate)
	{
		// IDのチェック
		const int idx = FindShapeDataIndex(id);
		// IDが存在しなかったら早期リターン
		if (idx < 0)
		{
			return;
		}

		m_shapes.at(idx)->SetTranslation(translate);

		// 子形状の更新
		UpdateChildShape(id);
	}

	void Collider::SetShapeRotation(ShapeID id, const Quaternion& quat)
	{
		// IDのチェック
		const int idx = FindShapeDataIndex(id);
		// IDが存在しなかったら早期リターン
		if (idx < 0)
		{
			return;
		}

		m_shapes.at(idx)->SetRotation(quat);

		// 子形状の更新
		UpdateChildShape(id);
	}

	void Collider::SetShapeRotation(ShapeID id, const VECTOR3& euler)
	{
		// IDのチェック
		const int idx = FindShapeDataIndex(id);
		// IDが存在しなかったら早期リターン
		if (idx < 0)
		{
			return;
		}

		m_shapes.at(idx)->SetRotation(euler.ToQuaternion());

		// 子形状の更新
		UpdateChildShape(id);
	}

	void Collider::SetShapeRadius(ShapeID id, float radius)
	{
		// IDのチェック
		const int idx = FindShapeDataIndex(id);
		// IDが存在しなかったら早期リターン
		if (idx < 0)
		{
			return;
		}

		m_shapes.at(idx)->SetRadius(radius);
	}

	void Collider::SetShapeHeight(ShapeID id, float height)
	{
		// IDのチェック
		const int idx = FindShapeDataIndex(id);
		// IDが存在しなかったら早期リターン
		if (idx < 0)
		{
			return;
		}

		m_shapes.at(idx)->SetHeight(height);
	}

	void Collider::SetShapeExtents(ShapeID id, const VECTOR3& extents)
	{
		// IDのチェック
		const int idx = FindShapeDataIndex(id);
		// IDが存在しなかったら早期リターン
		if (idx < 0)
		{
			return;
		}

		m_shapes.at(idx)->SetExtents(extents);
	}


}// namespace TKGEngine