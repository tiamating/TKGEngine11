#pragma once

#include "CCollider.h"

#include <btBulletDynamicsCommon.h>


class btGhostObject;

namespace TKGEngine
{
	/// <summary>
	/// Trigger Object module.
	/// </summary>
	class TriggerBody
		: public Collider
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		TriggerBody(const TriggerBody&) = delete;
		TriggerBody& operator=(const TriggerBody&) = delete;

		TriggerBody();
		virtual ~TriggerBody();


		// 位置を設定
		virtual void SetPosition(const VECTOR3& pos) override;
		// 回転を設定
		virtual void SetRotation(const Quaternion& rot) override;

		// 速度(RigidBody用)
		virtual VECTOR3 GetLinearVelocity() const override;

		/// フラグ、パラメータの変更時に、作り直す必要があるため、その関数
		virtual void RecreateCollisionObject() override;

		// スタティック
		virtual void SetStatic(const bool is_static) override;

		// キネマティック
		virtual void SetKinematic(const bool is_kinematic) override;

		// RigidBodyかどうか
		virtual bool IsRigidBody() const override
		{
			return false;
		}

		// Shape関連

		/// <summary>
		/// 形状データを追加する
		/// </summary>
		/// <returns>ShapeData配列のID</returns>
		virtual ShapeID AddShape(
			ShapeType shape_type,
			const VECTOR3& translate = VECTOR3::Zero,
			const Quaternion& quat = Quaternion::Identity
		);
		// 形状データの削除
		virtual void RemoveShape(ShapeID id);

		// ~Shape関連



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
					cereal::base_class<Collider>(this)
				);
			}
		}

		// ==============================================
		// private class
		// ==============================================
#ifdef USE_IMGUI
		void OnGUI() override;
		void OnGizmoEnter() override;
		void OnGizmoStay() override;
		void OnGizmoExit() override;
#endif // USE_IMGUI


		/// <summary>
		/// トリガーオブジェクト用アクションインターフェース
		/// </summary>
		class TriggerAction
			: public btActionInterface
		{
		public:
			TriggerAction(const std::shared_ptr<TKGEngine::Transform>& transform_, btGhostObject* ghost_);
			TriggerAction(const TriggerAction&) = delete;
			TriggerAction(TriggerAction&&) = default;
			virtual ~TriggerAction();

			virtual void updateAction(btCollisionWorld* collisionWorld, btScalar deltaTimeStep);

			virtual void debugDraw(btIDebugDraw* debugDrawer) {}

		private:
			std::shared_ptr<TKGEngine::Transform> transform;
			btGhostObject* ghost;
		};


		// ==============================================
		// private methods
		// ==============================================
		// Collider
		void OnCreated(const std::shared_ptr<Object>& p_self) override;
		void OnDestroyed() override;
		// ~Collider
		// 
		// Behaviour
		void OnEnabled() override;
		void OnDisabled() override;
		// ~Behaviour
		
		// ICollider
		// Triggerかどうか
		virtual bool IsTrigger() const
		{
			return true;
		}

		// Collisionのアクティブを変化させるときに呼ばれる
		virtual void OnSetCollisionActive(bool is_active) override;
		// ~ICollider

		// Compound ShapeのAABBの再計算
		void RecalculateBody() override;

		// レイヤーグループとレイヤーマスクの設定を作成する
		virtual void CreateLayerGroupAndMask(int& group, int& mask) override;


		// ==============================================
		// private variables
		// ==============================================

		// ゴーストオブジェクト
		std::unique_ptr<btGhostObject> m_ghost = nullptr;

		// アクションインターフェース
		std::unique_ptr<TriggerAction> m_action = nullptr;
		bool m_set_action = false;
	};

}// ~namespaace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::TriggerBody, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::TriggerBody, "TKGEngine::TriggerBody")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Collider, TKGEngine::TriggerBody)