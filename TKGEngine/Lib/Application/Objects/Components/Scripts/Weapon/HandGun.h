#pragma once
#include "IWeaponBase.h"

namespace TKGEngine
{
	class HandGun
		: public IWeaponBase
	{
	public:
		HandGun() = default;
		virtual ~HandGun() override = default;
		HandGun(const HandGun&) = delete;
		HandGun& operator=(const HandGun&) = delete;

		//////////////////////////////////
		// functions
		//////////////////////////////////

		// 発射
		virtual bool Shot(const VECTOR3& origin, const VECTOR3& direction) override;
		// 発射可能かチェック
		virtual bool CheckCanShot() override;

		// リロード開始
		virtual void BeginReload() override;
		// リロード完了
		virtual void CompleteReload() override;
		// リロード終了
		virtual void EndReload() override;
		// リロードキャンセル
		virtual void CancelReload() override;
		// リロード可能かチェック
		virtual bool CheckCanReload() override;
		
		// マガジン内残弾数を取得
		virtual int GetRemainBulletNum() const override { return m_remain_bullet_num; }

	private:
		/////////////////////////////////////////////////////////////////////////
		// 使用するものをコメント解除して使用
		/////////////////////////////////////////////////////////////////////////
		//virtual void Awake() override;
		virtual void Start() override;
		virtual void Update() override;
		//virtual void LateUpdate() override;
		//virtual void OnEnable() override;
		//virtual void OnDisable() override;
		//virtual void OnDestroyed() override;


		//////////////////////////////////
		// functions
		//////////////////////////////////



		//////////////////////////////////
		// variables
		//////////////////////////////////

		// 1秒間に撃てる弾数
		float m_rate_per_seconds = 2.5f;
		// リキャストタイマー
		float m_recast_timer = 0.0f;

		// マガジンサイズ
		int m_magazine_size = 6;
		// マガジン内残弾数
		int m_remain_bullet_num = 6;

		// 射程距離
		float m_range_distance = 20.0f;

		// ダメージ量
		int m_damage = 5;


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
					cereal::base_class<TKGEngine::IWeaponBase>(this)
				);
			}
		}

		/////////////////////////////////////////////////////////////////////////
		// GUI設定
		/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
		void OnGUI() override;
#endif // USE_IMGUI
	};
}

/////////////////////////////////////////////////////////////////////////
// Serialize登録
/////////////////////////////////////////////////////////////////////////
CEREAL_CLASS_VERSION(TKGEngine::HandGun, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::HandGun, "TKGEngine::HandGun")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::IWeaponBase, TKGEngine::HandGun)
