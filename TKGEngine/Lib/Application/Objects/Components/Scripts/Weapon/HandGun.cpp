#include "HandGun.h"

#include "Components/inc/CParticleSystem.h"
#include "../Character/CharacterWeaponController.h"

REGISTERCOMPONENT(TKGEngine::HandGun);

namespace TKGEngine
{
	void HandGun::Shot(const VECTOR3& origin, const VECTOR3& direction)
	{
		// マズルフラッシュを発生させる
		if (const auto muzzle_flush = m_muzzle_flush.GetWeak().lock())
		{
			muzzle_flush->Play();
		}

		// レイを飛ばして衝突した場所にパーティクルを発生する
		Physics::RaycastHit ray_data;
		constexpr int hit_layer = 1 << static_cast<int>(Layer::EnemyHit) | 1 << static_cast<int>(Layer::Stage);
		if (Physics::Raycast(origin, direction, ray_data, m_range_distance, true, hit_layer))
		{
			if (const auto effect = m_hit_effect.GetWeak().lock())
			{
				// 衝突位置と法線を軸としてパーティクルを発生させる
				const auto transform = effect->GetTransform();
				transform->Position(ray_data.position + ray_data.normal * 0.1f);
				transform->Rotation(Quaternion::FromToRotation(VECTOR3::Forward, ray_data.normal));
				effect->Play();
			}
		}
		
		// 弾を減らす
		--m_remain_bullet_num;
		// リキャストタイマーを発生させる
		m_recast_timer = 1.0f / m_rate_per_seconds;
	}

	bool HandGun::CheckCanShot()
	{
		// リロード中は撃てない
		if (m_on_reloading)
			return false;
		// リキャスト済みで残弾があるときはショット可能
		if (m_recast_timer <= 0.0f && m_remain_bullet_num > 0)
			return true;
		return false;
	}

	void HandGun::BeginReload()
	{
		m_on_reloading = true;
	}

	void HandGun::CompleteReload()
	{
		// リロード中に呼ばれた場合のみ残弾を追加
		if (m_on_reloading)
		{
			m_remain_bullet_num = m_magazine_size;
		}
	}

	void HandGun::EndReload()
	{
		m_on_reloading = false;
	}

	void HandGun::CancelReload()
	{
		m_on_reloading = false;
	}

	bool HandGun::CheckCanReload()
	{
		// リロード中は早期リターン
		if (m_on_reloading)
			return false;

		// マガジン内残弾数がマガジンサイズより少ないときはリロード可能
		if (m_remain_bullet_num < m_magazine_size)
		{
			return true;
		}
		return false;
	}

	void HandGun::Start()
	{
		const auto transform = GetTransform();

		// 武器管理クラスに自身を登録する
		auto parent = transform->GetParent();
		while (true)
		{
			const auto grand_parent = parent->GetParent();
			// 親が見つからなくなるまで探索する
			if (grand_parent == nullptr)
				break;
			// さらに探索
			parent = grand_parent;
		}
		if (const auto weapon_manager = parent->GetComponent<CharacterWeaponController>())
		{
			weapon_manager->RegisterWeapon(CharacterWeaponController::WeaponType::HandGun, GetComponent<HandGun>());
		}


		// 子オブジェクトからパーティクルを取得する
		const int child_count = transform->GetChildCount();
		for (int i = 0; i < child_count; ++i)
		{
			const auto child = transform->GetChild(i);
			// パーティクルシステムを持つかチェック
			if (const auto ps = child->GetComponent<ParticleSystem>())
			{
				// マズルフラッシュ
				if (child->GetName() == std::string("MuzzleFlush"))
				{
					m_muzzle_flush = ps;
				}
				// ヒットエフェクト
				if (child->GetName() == std::string("HitEffect"))
				{
					m_hit_effect = ps;
				}
			}
		}
	}

	void HandGun::Update()
	{
		const float delta_time = ITime::Get().DeltaTime();

		// ショットのリキャストタイマーを進める
		m_recast_timer -= delta_time;
		if (m_recast_timer < 0.0f)
		{
			m_recast_timer = 0.0f;
		}
	}



	/////////////////////////////////////////////////////////////////////////
	// GUI設定
	/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	void HandGun::OnGUI()
	{
	}
#endif
}
