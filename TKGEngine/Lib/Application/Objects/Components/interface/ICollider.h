#pragma once

namespace TKGEngine
{
	/// <summary>
	/// コライダー用インターフェース
	/// </summary>
	class ICollider
	{
	public:
		/// <summary>
		/// 衝突状態のチェック時に呼ばれる
		/// </summary>
		/// <param name="other">衝突相手のID</param>
		virtual void AddTriggerHitID(int other) = 0;
		virtual void AddCollisionHitID(int other) = 0;

		// 衝突状態の更新
		virtual void UpdateHitState() = 0;

		// 衝突時の関数を呼ぶかどうか
		virtual bool IsCallOnFunction() const = 0;

		// Triggerかどうか
		virtual bool IsTrigger() const = 0;

		// Kinematicかどうか
		virtual bool IsKinematic() const = 0;

		// GameObjectIDの取得
		virtual int GetGameObjectID() const = 0;

		// ComponentIDの取得
		virtual int GetColliderID() const = 0;

		// Colliderが有効かどうか
		virtual bool IsActiveCollider() = 0;

		// Collisionのアクティブを変化させるときに呼ばれる
		virtual void OnSetCollisionActive(bool is_active) = 0;
	};
}