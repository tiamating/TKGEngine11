#include "CharacterWeaponController.h"

#include "Camera/CameraController.h"
#include "Components/inc/CAnimator.h"
#include "Components/Scripts/Weapon/IWeaponBase.h"

REGISTERCOMPONENT(TKGEngine::CharacterWeaponController);

REGISTER_EVENT_FUNCTION_VOID(TKGEngine::CharacterWeaponController, OnBeginReload);
REGISTER_EVENT_FUNCTION_VOID(TKGEngine::CharacterWeaponController, OnCompleteReload);
REGISTER_EVENT_FUNCTION_VOID(TKGEngine::CharacterWeaponController, OnEndReload);

namespace TKGEngine
{
	/////////////////////////////////////////////////////////////////////////
	// Updateが呼ばれる前に一度だけ呼び出される
	/////////////////////////////////////////////////////////////////////////
	void CharacterWeaponController::Start()
	{
		// コンポーネントの取得
		// CameraController
		const auto camera_target = IGameObject::Find("CameraTarget");
		if (camera_target == nullptr)
		{
			LOG_ASSERT("failed find \"CameraTarget\"");
			return;
		}
		m_camera = camera_target->GetComponent<CameraController>();
		// Animator
		m_animator = GetComponent<Animator>();
	}

	/////////////////////////////////////////////////////////////////////////
	// 毎フレーム呼び出される
	/////////////////////////////////////////////////////////////////////////
	void CharacterWeaponController::Update()
	{
		// 参照の取得
		m_camera.Lock();
		m_animator.Lock();
		const auto current_weapon = m_current_weapon_type;
		m_weapons.at(static_cast<int>(current_weapon)).Lock();

		// エイム状態の更新
		AimUpdate();

		// 参照の破棄
		m_camera.Unlock();
		m_animator.Unlock();
		m_weapons.at(static_cast<int>(current_weapon)).Unlock();
	}

	void CharacterWeaponController::RegisterWeapon(WeaponType type, const std::weak_ptr<IWeaponBase>& weapon)
	{
		m_weapons.at(static_cast<int>(type)) = weapon;
	}

	void CharacterWeaponController::ApplyInputAimUpdate()
	{
		// エイム状態の更新
		if (IInput::Get().GetPadLTrigger(0) > m_left_trigger_threshold)
		{
			SetAimState(AimState::Hold);
		}
		else
		{
			SetAimState(AimState::Low);
		}
	}

	void CharacterWeaponController::ApplyInputShotUpdate()
	{
		// 入力がなければ入力状態をリセットして早期リターン
		if (IInput::Get().GetPadRTrigger(0) < m_right_trigger_threshold)
		{
			m_right_trigger_entered = false;
			return;
		}
		// 入力が解除されていなければ早期リターン
		if (m_right_trigger_entered)
			return;
		// 入力フラグを立てる
		m_right_trigger_entered = true;

		const auto weapon = m_weapons.at(static_cast<int>(m_current_weapon_type)).GetWeak().lock();
		if (weapon == nullptr)
			return;
		// 残弾数が0ならリロード
		if (weapon->CheckCanReload() && weapon->GetRemainBulletNum() == 0)
		{
			if (const auto animator = m_animator.GetWeak().lock())
			{
				animator->SetTrigger("OnReload");
			}
		}
		// 射撃可能なら射撃処理
		else if (weapon->CheckCanShot())
		{
			// カメラ座標とカメラ方向から弾を発射
			if (const auto camera = m_camera.GetWeak().lock())
			{
				weapon->Shot(camera->GetCameraPosition(), camera->GetCameraDirection());
				if (const auto animator = m_animator.GetWeak().lock())
				{
					animator->SetTrigger("OnShot");
				}
			}
		}
	}

	void CharacterWeaponController::ApplyInputReloadUpdate()
	{
		// 入力がなければ入力状態をリセットして早期リターン
		if (IInput::Get().GetPadRTrigger(0) < m_right_trigger_threshold)
		{
			m_right_trigger_entered = false;
			return;
		}
		// 入力が解除されていなければ早期リターン
		if (m_right_trigger_entered)
			return;
		// 入力フラグを立てる
		m_right_trigger_entered = true;

		const auto weapon = m_weapons.at(static_cast<int>(m_current_weapon_type)).GetWeak().lock();
		if (weapon == nullptr)
			return;
		if(weapon->CheckCanReload())
		{
			if (const auto animator = m_animator.GetWeak().lock())
			{
				animator->SetTrigger("OnReload");
			}
		}
	}

	void CharacterWeaponController::CancelReload()
	{
		if(const auto weapon = m_weapons.at(static_cast<int>(m_current_weapon_type)).GetWeak().lock())
		{
			weapon->CancelReload();
		}
	}

	void CharacterWeaponController::AimUpdate()
	{
		const float delta_time = ITime::Get().DeltaTime();

		// 構え状態によって更新する
		switch (m_aim_state)
		{
			case AimState::Hold:
			{
				// 重みを加算
				m_aim_weight += delta_time * MyMath::InvertDivisionValue(m_aim_transition_time);
				m_aim_upper_weight += delta_time * MyMath::InvertDivisionValue(m_aim_transition_time);
			}
			break;
			case AimState::Low:
			{
				// 重みを減算
				m_aim_weight -= delta_time * MyMath::InvertDivisionValue(m_aim_transition_time);
				m_aim_upper_weight -= delta_time * MyMath::InvertDivisionValue(m_aim_transition_time);
			}
			break;
		}

		m_aim_weight = MyMath::Clamp(m_aim_weight, 0.0f, 1.0f);
		m_aim_upper_weight = MyMath::Clamp(m_aim_upper_weight, 0.0f, 1.0f);

		// リロード中は構えを維持する
		const auto weapon = m_weapons.at(static_cast<int>(m_current_weapon_type)).GetShared();
		if (weapon->OnReloading())
		{
			m_aim_upper_weight = 1.0f;
		}
	}

	void CharacterWeaponController::OnBeginReload()
	{
		// 武器にリロード開始を教える
		if (const auto weapon = m_weapons.at(static_cast<int>(m_current_weapon_type)).GetWeak().lock())
		{
			weapon->BeginReload();
		}
	}

	void CharacterWeaponController::OnCompleteReload()
	{
		// 武器に弾込め完了を教える
		if (const auto weapon = m_weapons.at(static_cast<int>(m_current_weapon_type)).GetWeak().lock())
		{
			weapon->CompleteReload();
		}
	}

	void CharacterWeaponController::OnEndReload()
	{
		// 武器にリロード終了を教える
		if (const auto weapon = m_weapons.at(static_cast<int>(m_current_weapon_type)).GetWeak().lock())
		{
			weapon->EndReload();
		}
	}

	/////////////////////////////////////////////////////////////////////////
	// GUI設定
	/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	void CharacterWeaponController::OnGUI()
	{
	}
#endif
}
