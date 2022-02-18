#pragma once
#include "Components/inc/CMonoBehaviour.h"

namespace TKGEngine
{
	class ParticleSystem;

	/// <summary>
	/// 銃のベースクラス
	/// </summary>
	class IWeaponBase
		: public MonoBehaviour
	{
	public:
		IWeaponBase() = default;
		virtual ~IWeaponBase() = default;
		IWeaponBase(const IWeaponBase&) = delete;
		IWeaponBase& operator=(const IWeaponBase&) = delete;
		IWeaponBase& operator=(IWeaponBase&&) = default;

		// 発射
		virtual void Shot(const VECTOR3& origin, const VECTOR3& direction) = 0;
		// 発射可能かチェック
		virtual bool CheckCanShot() = 0;

		// リロード開始
		virtual void BeginReload() = 0;
		// リロード完了
		virtual void CompleteReload() = 0;
		// リロード終了
		virtual void EndReload() = 0;
		// リロードキャンセル
		virtual void CancelReload() = 0;
		// リロード可能かチェック
		virtual bool CheckCanReload() = 0;
		// リロード中
		bool OnReloading() const { return m_on_reloading; }

		
		// マガジン内残弾数を取得
		virtual int GetRemainBulletNum() const = 0;

	protected:
		// マズルフラッシュ用パーティクル
		SWPtr<ParticleSystem> m_muzzle_flush;
		// ヒット用パーティクル
		SWPtr<ParticleSystem> m_hit_effect;
		
		// リロード中フラグ
		bool m_on_reloading = false;

	private:
		/////////////////////////////////////////////////////////////////////////
		// Serialize設定
		/////////////////////////////////////////////////////////////////////////
		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			if (version == 1)
			{
				archive(
					cereal::base_class<TKGEngine::MonoBehaviour>(this)
				);
			}
		}
	};


}
/////////////////////////////////////////////////////////////////////////
// Serialize登録
/////////////////////////////////////////////////////////////////////////
CEREAL_CLASS_VERSION(TKGEngine::IWeaponBase, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::IWeaponBase, "TKGEngine::IWeaponBase")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::IWeaponBase)
