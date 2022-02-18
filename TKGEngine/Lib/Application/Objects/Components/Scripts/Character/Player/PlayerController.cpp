#include "PlayerController.h"

#include "Components/inc/CAnimator.h"
#include "../CharacterMoveController.h"
#include "../CharacterWeaponController.h"


REGISTERCOMPONENT(TKGEngine::PlayerController);

REGISTER_EVENT_FUNCTION_VOID(TKGEngine::PlayerController, OnEndClimbOver);

namespace TKGEngine
{
	void PlayerController::Awake()
	{
		// 入力の更新はカメラよりも早く行う
		Priority(-11);
	}
	
	void PlayerController::Start()
	{
		// 毎フレーム更新関数を呼び出すかどうか
		m_can_call_every_frame_update = true;
		
		// Animator
		m_animator = GetComponent<Animator>();
		// CharacterMoveController
		m_mover = GetComponent<CharacterMoveController>();
		// CharacterWeaponController
		m_weapon_controller = GetComponent<CharacterWeaponController>();
	}

	void PlayerController::Update()
	{
		const float delta_time = ITime::Get().DeltaTime();

		// パッド入力データの取得
		if (m_allow_left_stick_input)
		{
			m_left_stick_input.SetInput(VECTOR2(IInput::Get().GetPadStickLX(0), IInput::Get().GetPadStickLY(0)));
		}

		// スライディングタイマーの更新
		if (m_sliding_timer > 0.0f)
		{
			m_sliding_timer -= delta_time;
		}
	}

	void PlayerController::LateUpdate()
	{
		// 参照の所有権を取得
		OnUpdateBegin();

		// 姿勢の更新
		UpdatePosture();
		// スライディングタイマーのセット
		if(m_animator->GetBool("OnSliding"))
		{
			m_sliding_timer = m_sliding_recast_time;
		}
		// アニメーションの更新
		MotionUpdate();

		// 参照の所有権を破棄
		OnUpdateEnd();
	}
	
	void PlayerController::SetPostureState(const PostureState state)
	{
		m_posture_state = state;
	}
	PlayerController::PostureState PlayerController::GetPostureState() const
	{
		return m_posture_state;
	}

	void PlayerController::SetClimbOverData(const VECTOR3& begin, const VECTOR3& direction)
	{
		m_climb_over_begin_position = begin;
		m_climb_over_direction = direction;
		m_can_climb_over = false;
	}

	void PlayerController::SetCoverData(const VECTOR3& position, const VECTOR3& normal)
	{
		m_cover_position = position;
		m_cover_wall_normal = normal;
	}

	void PlayerController::UpdatePosture()
	{
		// 姿勢の高さを更新する
		switch (m_posture_state)
		{
			case PostureState::Standing:
			{
				m_crouch_ratio -= ITime::Get().DeltaTime() / m_stand_time;
			}
			break;
			case PostureState::Crouching:
			{
				m_crouch_ratio += ITime::Get().DeltaTime() / m_crouch_time;
			}
			break;
		}
		// 姿勢の範囲にクランプ
		m_crouch_ratio = MyMath::Clamp(m_crouch_ratio, 0.0f, 1.0f);
		// イージングを加える
		m_crouch_weight = MyFunc::EaseIn::cubic(m_crouch_ratio);
	}

	void PlayerController::OnUpdateBegin()
	{
		// 参照の所有権を取得
		m_animator.Lock();
		m_mover.Lock();
		m_weapon_controller.Lock();
	}

	void PlayerController::OnUpdateEnd()
	{
		// 参照の所有権を破棄
		m_animator.Unlock();
		m_mover.Unlock();
		m_weapon_controller.Unlock();
	}

	void PlayerController::MotionUpdate()
	{
		// 入力量
		m_animator->SetFloat("LStickMagnitude", m_allow_left_stick_input ? m_left_stick_input.GetLength() : 0.0f);
		// 姿勢の高さ
		m_animator->SetFloat("PostureHeight", m_crouch_weight);
		// 速度
		m_animator->SetFloat("CurrentSpeed", m_mover->GetSpeed());
		// 走り状態
		m_animator->SetBool("OnRunning", m_on_running);
		// エイムの重み
		m_animator->SetFloat("AimWeight", m_weapon_controller->GetAimWeight());
		m_animator->SetFloat("AimUpperWeight", m_weapon_controller->GetAimUpperWeight());
	}

	void PlayerController::OnEndClimbOver()
	{
		m_can_climb_over = true;
	}

	/////////////////////////////////////////////////////////////////////////
	// GUI設定
	/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	void PlayerController::OnGUI()
	{
		// Crouch Time
		ImGui::Text("Crouch Time");
		ImGui::AlignedSameLine(0.5f);
		ImGui::SliderFloat("##Crouch Time", &m_crouch_time, 0.01f, 1.0f);
		ImGui::Text("Stand Time");
		ImGui::AlignedSameLine(0.5f);
		ImGui::SliderFloat("##Stand Time", &m_stand_time, 0.01f, 1.0f);
		ImGui::Separator();

		// Slide
		ImGui::Text("Sliding");
		ImGui::AlignedSameLine(0.5f);
		ImGui::SliderFloat("##Slide Recast Time", &m_sliding_recast_time, 0.1f, 2.0f);

		// Player Radius
		ImGui::Text("Player Radius");
		ImGui::AlignedSameLine(0.5f);
		ImGui::SliderFloat("##Player Radius", &m_player_radius, 0.05f, 1.0f);

	}
#endif
}
