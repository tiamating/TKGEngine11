#include "../inc/CTriggerBody.h"

#include "../inc/CTransform.h"
#include "../../inc/IGameObject.h"

#include "Systems/inc/PhysicsSystem.h"


REGISTERCOMPONENT(TKGEngine::TriggerBody)

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// TriggerBody class Methods
	////////////////////////////////////////////////////////
#pragma region TriggerBody
	TriggerBody::TriggerBody()
	{
		/* nothing */
	}

	TriggerBody::~TriggerBody()
	{
		/* nothing */
	}

#ifdef USE_IMGUI
	void TriggerBody::OnGUI()
	{
		ImGui::PushID(this);

		// Collider::OnGUI
		Collider::OnGUI();
		// Is Static
		if (ImGui::Checkbox("Is Static", &m_is_static))
		{
			SetStatic(m_is_static);
		}
		// Shapes
		ImGui::Separator();
		Collider::ShapeGUI();

		// Triggerを作り直す
		ImVec2 button_size;
		{
			const float width = ImGui::GetWindowWidth();
			constexpr float button_space = 0.2f;
			button_size.x = width * (1.0f - button_space * 2.0f);
			button_size.y = ImGui::DEFAULT_SPACE_SIZE * 1.5f;
			// ボタンを中央に置く
			ImGui::SetCursorPosX(width * button_space);
		}
		if (ImGui::Button("Recreate Trigger", button_size))
		{
			RecreateCollisionObject();
		}

		ImGui::PopID();
	}

	void TriggerBody::OnGizmoEnter()
	{

	}

	void TriggerBody::OnGizmoStay()
	{
		// Triggerの位置を現在のTransform位置に移動する
		const auto transform = GetTransform();
		const btTransform bt_transform(
			ConvertQuaternionTobtQuaternion(transform->Rotation()),
			ConvertVectorTobtVector(transform->Position())
		);
		m_ghost->setWorldTransform(bt_transform);
	}

	void TriggerBody::OnGizmoExit()
	{
		// Triggerの位置を現在のTransform位置に移動する
		const auto transform = GetTransform();
		const btTransform bt_transform(
			ConvertQuaternionTobtQuaternion(transform->Rotation()),
			ConvertVectorTobtVector(transform->Position())
		);
		m_ghost->setWorldTransform(bt_transform);
	}
#endif // USE_IMGUI

	void TriggerBody::SetPosition(const VECTOR3& pos)
	{
		// 早期リターン
		if (m_ghost == nullptr || IsStatic())
			return;

		// Triggerの位置を現在のTransform位置に移動する
		m_ghost->getWorldTransform().setOrigin(ConvertVectorTobtVector(pos));
	}

	void TriggerBody::SetRotation(const Quaternion& rot)
	{
		// 早期リターン
		if (!m_ghost || IsStatic())
			return;

		// Triggerの回転を現在のTransform位置に変更する
		m_ghost->getWorldTransform().setRotation(ConvertQuaternionTobtQuaternion(rot));
	}

	VECTOR3 TriggerBody::GetLinearVelocity() const
	{
		LOG_ASSERT("Call GetLinearVelocity() on TriggerBody component. This is RigidBody only.");
		return VECTOR3::Zero;
	}

	void TriggerBody::RecreateCollisionObject()
	{
		// GameObject、Behaviourが非アクティブなら早期リターン
		if (!IsActiveCollider())
		{
			return;
		}

		// TriggerをWorldから外す
		PhysicsSystem::GetInstance()->GetWorld()->removeCollisionObject(m_ghost.get());

		// 子形状の再設定、結合AABBの再計算
		RecalculateShapeAABB();

		// レイヤーを設定してワールドに登録
		{
			// レイヤーグループとマスクレイヤーの取得
			int group = 0;
			int mask = 0;
			CreateLayerGroupAndMask(group, mask);

			// Ghostはレイヤーだけが登録状態では変更できないので再登録する
			PhysicsSystem::GetInstance()->GetWorld()->addCollisionObject(m_ghost.get(), group, mask);
		}

		// Triggerの位置を現在のTransform位置に移動する
		{
			const auto transform = GetTransform();
			m_ghost->getWorldTransform().setOrigin(ConvertVectorTobtVector(transform->Position()));
			m_ghost->getWorldTransform().setRotation(ConvertQuaternionTobtQuaternion(transform->Rotation()));
		}
	}

	void TriggerBody::SetStatic(const bool is_static)
	{
		// 状態の代入
		m_is_static = is_static;

		// 早期リターン
		if (!m_ghost)
		{
			return;
		}

		// Collisionのフラグの設定
		if (m_is_static)
		{
			m_ghost->setCollisionFlags(m_ghost->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
		}
		else
		{
			m_ghost->setCollisionFlags(m_ghost->getCollisionFlags() & ~btCollisionObject::CF_STATIC_OBJECT);
		}

		// Staticでないならアクションインターフェースを登録する
		if (!IsStatic())
		{
			// アクションインターフェースの登録
			if (!m_set_action)
			{
				PhysicsSystem::GetInstance()->AddAction(m_action.get());
				m_set_action = true;
			}
		}
		else
		{
			// アクションインターフェースの解除
			if (m_set_action)
			{
				PhysicsSystem::GetInstance()->RemoveAction(m_action.get());
				m_set_action = false;
			}
		}
	}

	void TriggerBody::SetKinematic(const bool is_kinematic)
	{
		/* nothing */
		// Triggerは衝突しないのでキネマティックにはならない
	}

	ShapeID TriggerBody::AddShape(ShapeType shape_type, const VECTOR3& translate, const Quaternion& quat)
	{
		// Children Shapeの作成
		const ShapeID ret_id = Collider::AddShape(shape_type, translate, quat);

		return ret_id;
	}

	void TriggerBody::RemoveShape(ShapeID id)
	{
		// Children Shapeの削除
		Collider::RemoveShape(id);
	}

	void TriggerBody::OnCreated(const std::shared_ptr<Object>& p_self)
	{
		// ゴーストオブジェクトの作成
		m_ghost = std::make_unique<btGhostObject>();
		m_ghost->setCollisionFlags(m_ghost->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

		// ロード時にShapeがあれば作成
		if (!m_shapes.empty())
		{
			for (const auto& shape : m_shapes)
			{
				// 実体作成
				shape->CreateShape();
				// CompoundShapeに登録
				m_compound_shape->addChildShape(shape->GetbtTransform(), shape->GetPtr());
			}
			// AABB、慣性、質量の再計算
			RecalculateBody();
		}
		// ゴーストにシェイプを取付
		m_ghost->setCollisionShape(m_compound_shape.get());

		// マネージャーに登録
		{
			// ObjectをColliderにキャスト
			std::shared_ptr<Collider> collider = std::static_pointer_cast<Collider>(p_self);

			// Managerに登録
			PhysicsSystem::GetInstance()->RegisterGhostObject(collider, m_ghost.get());
		}

		// アクションインターフェースの作成
		m_action = std::make_unique<TriggerAction>(GetTransform(), m_ghost.get());

		// Staticでないならアクションインターフェースを登録する
		if (!IsStatic())
		{
			// アクションインターフェースの登録
			PhysicsSystem::GetInstance()->AddAction(m_action.get());
			m_set_action = true;
		}
	}

	void TriggerBody::OnDestroyed()
	{
		// アクションインターフェースを削除する
		PhysicsSystem::GetInstance()->RemoveAction(m_action.get());
		m_set_action = false;
		// Managerから削除
		PhysicsSystem::GetInstance()->UnregisterGhostObject(m_ghost.get());

		// アクションインターフェースの破棄
		m_action.reset();
		// ゴーストオブジェクトの破棄
		m_ghost.reset();
	}

	void TriggerBody::OnEnabled()
	{
		// 現在のTransformに更新する
		{
			m_ghost->getWorldTransform().setOrigin(ConvertVectorTobtVector(GetTransform()->Position()));
			m_ghost->getWorldTransform().setRotation(ConvertQuaternionTobtQuaternion(GetTransform()->Rotation()));
		}
		// TriggerBodyをWorldにセット
		{
			// レイヤーグループとマスクレイヤーの取得
			int group = 0;
			int mask = 0;
			CreateLayerGroupAndMask(group, mask);

			PhysicsSystem::GetInstance()->GetWorld()->addCollisionObject(m_ghost.get(), group, mask);
		}
		// アクションインターフェースをセット
		if (!IsStatic())
		{
			if (!m_set_action)
			{
				PhysicsSystem::GetInstance()->GetWorld()->addAction(m_action.get());
				m_set_action = true;
			}
		}
	}

	void TriggerBody::OnDisabled()
	{
		// TriggerBodyをWorldから外す
		PhysicsSystem::GetInstance()->GetWorld()->removeCollisionObject(m_ghost.get());

		// アクションインターフェースを外す
		if (!IsStatic())
		{
			if (m_set_action)
			{
				PhysicsSystem::GetInstance()->GetWorld()->removeAction(m_action.get());
				m_set_action = false;
			}
		}
	}

	void TriggerBody::OnSetCollisionActive(bool is_active)
	{
		if (is_active)
		{
			OnEnabled();
		}
		else
		{
			OnDisabled();
		}
	}

	void TriggerBody::RecalculateBody()
	{
		// 子形状の再設定、結合AABBの再計算
		RecalculateShapeAABB();
	}

	void TriggerBody::CreateLayerGroupAndMask(int& group, int& mask)
	{
		Layer layer = GetGameObject()->GetLayer();
		int current_layer = (1 << static_cast<int>(layer)) | (1 << static_cast<int>(Layer::Trigger));
		int current_layer_mask = PhysicsSystem::GetInstance()->GetLayerMask(layer);

		// 衝突しないレイヤーの設定
		int ignore_layer = 0;
		{
			// Static同士は衝突しない
			if (IsStatic())
			{
				constexpr int static_layer = static_cast<int>(Layer::Static);
				current_layer |= (1 << static_layer);
				ignore_layer |= (1 << static_layer);
			}
		}

		group = current_layer;
		mask = current_layer_mask & ~ignore_layer;
	}
#pragma endregion
	// ~TriggerBody class


	////////////////////////////////////////////////////////
	// TriggerBody::TriggerAction class Methods
	////////////////////////////////////////////////////////
	TriggerBody::TriggerAction::TriggerAction(const std::shared_ptr<Transform>& transform_, btGhostObject* ghost_)
		: transform(transform_), ghost(ghost_)
	{
		/* nothing */
	}

	TriggerBody::TriggerAction::~TriggerAction()
	{
		transform.reset();
		ghost = nullptr;
	}

	void TriggerBody::TriggerAction::updateAction(btCollisionWorld* collisionWorld, btScalar deltaTimeStep)
	{
		// Triggerの位置を現在のTransform位置に移動する
		const btTransform bt_transform(
			ConvertQuaternionTobtQuaternion(transform->Rotation()),
			ConvertVectorTobtVector(transform->Position())
		);
		ghost->setWorldTransform(bt_transform);
	}


}// ~namespace TKGEngine