#include "PlayerCoverSubState.h"
#include "PlayerActionState.h"
#include "PlayerController.h"
#include "../CharacterMoveController.h"
#include "../CharacterWeaponController.h"
#include "../Camera/CameraController.h"
#include "Components/inc/CRigidBody.h"
#include "Components/inc/CAnimator.h"
#include "Components/inc/CUIRenderer.h"

REGISTERCOMPONENT(TKGEngine::PlayerCoverSubState);

REGISTER_EVENT_FUNCTION_VOID(TKGEngine::PlayerCoverSubState, OnEndCoverBegin);
REGISTER_EVENT_FUNCTION_VOID(TKGEngine::PlayerCoverSubState, OnEndCoverEnd);

namespace TKGEngine
{
	void PlayerCoverSubState::Awake()
	{
		// 毎フレーム更新は行わない
		m_can_call_every_frame_update = false;
	}

	void PlayerCoverSubState::Start()
	{
		// 親ステートに登録する
		const auto parent_state = GetComponent<PlayerActionState>();
		m_parent_state = parent_state;
		if (!parent_state)
		{
			LOG_ASSERT("failed get PlayerActionState component. (PlayerCoverSubState)");
			return;
		}
		parent_state->RegisterSubState(GetComponent<PlayerCoverSubState>(), static_cast<int>(PlayerActionState::ActionState::Cover));

		// コンポーネントの取得
		// PlayerController
		m_player = GetComponent<PlayerController>();
		// CharacterMoveController
		m_mover = GetComponent<CharacterMoveController>();
		// CharacterWeaponController
		m_weapon_controller = GetComponent<CharacterWeaponController>();
		// CameraController
		const auto camera_target = IGameObject::Find("CameraTarget");
		if (camera_target == nullptr)
		{
			LOG_ASSERT("failed find \"CameraTarget\"");
			return;
		}
		m_camera = camera_target->GetComponent<CameraController>();
		// RigidBody
		m_rigidbody = GetComponent<RigidBody>();
		// Animator
		m_animator = GetComponent<Animator>();
		// UIRenderer
		{
			// CoverUI
			const auto cover_ui_obj = IGameObject::Find("CanCoverUI");
			m_cover_ui = cover_ui_obj->GetComponent<UIRenderer>();
			// ClimbOverUI
			const auto climb_over_obj = IGameObject::Find("CanClimbOverUI");
			m_climb_over_ui = climb_over_obj->GetComponent<UIRenderer>();
		}
	}

	void PlayerCoverSubState::OnTriggerEnter(std::shared_ptr<Collider>& collider)
	{
		const auto gameobject = collider->GetGameObject();
		// 移動上限
		if (gameobject->GetLayer() == Layer::CoverLimitField)
		{
			const auto tag = gameobject->GetTag();
			// 右
			if (tag == Tag::RightLimit)
			{
				m_hit_limit_right = true;
			}
			// 左
			else if (tag == Tag::LeftLimit)
			{
				m_hit_limit_left = true;
			}
		}
	}

	void PlayerCoverSubState::OnTriggerExit(std::shared_ptr<Collider>& collider)
	{
		const auto gameobject = collider->GetGameObject();
		// 移動上限
		if (gameobject->GetLayer() == Layer::CoverLimitField)
		{
			const auto tag = gameobject->GetTag();
			// 右
			if (tag == Tag::RightLimit)
			{
				m_hit_limit_right = false;
			}
			// 左
			else if (tag == Tag::LeftLimit)
			{
				m_hit_limit_left = false;
			}
		}
	}

	void PlayerCoverSubState::Enter()
	{
		// プレイヤー速度を0にする
		if (const auto mover = m_mover.GetWeak().lock())
		{
			mover->SetSpeed(0.0f);
		}

		// 状態をリセット
		m_state = State::Ready;
	}

	void PlayerCoverSubState::BeforeExecute()
	{
		// コンポーネントを取得
		m_player.Lock();
		m_mover.Lock();
		m_weapon_controller.Lock();
		m_camera.Lock();
		m_rigidbody.Lock();
		m_animator.Lock();
		m_climb_over_ui.Lock();
		m_cover_ui.Lock();
	}

	void PlayerCoverSubState::AfterExecute()
	{
		// コンポーネントの破棄
		m_player.Unlock();
		m_mover.Unlock();
		m_weapon_controller.Unlock();
		m_camera.Unlock();
		m_rigidbody.Unlock();
		m_animator.Unlock();
		m_climb_over_ui.Unlock();
		m_cover_ui.Unlock();
	}

	void PlayerCoverSubState::Execute()
	{
		const auto transform = GetTransform();
		const float delta_time = ITime::Get().DeltaTime();
		// タイマー加算
		m_timer += delta_time;

		switch (m_state)
		{
			case State::Ready:
			{
				// 開始時の位置と壁の法線をセット
				m_begin_position = m_player->GetCoverPosition();
				m_wall_normal = m_player->GetCoverNormal();

				// カバー開始
				m_animator->SetBool("OnCovering", true);
				m_mover->SetSpeed(0.0f);

				// 体の向き
				DirectionUpdate();

				// エイム時を解除
				m_weapon_controller->SetAimState(CharacterWeaponController::AimState::Low);

				// 壁基準右+X方向
				m_wall_right_axis = VECTOR3::Cross(-m_wall_normal, VECTOR3::Up).Normalized();
				// 速度軸を壁基準右+X方向にする
				m_mover->SetVelocityDirection(VECTOR2(m_wall_right_axis.x, m_wall_right_axis.z).Normalized());

				// 状態のリセット
				m_next_is_climb_over = false;
				m_on_end_cover_begin = false;
				m_on_end_cover_end = false;

				// 状態を進める
				m_timer = 0.0f;
				m_state = State::BeginCover;
			}
			break;
			case State::BeginCover:
			{
				// 始点までの移動方向
				const VECTOR3 current_pos = transform->Position();

				VECTOR3 dir = m_begin_position - current_pos;
				dir.y = 0.0f;
				const float distance = dir.Length();
				dir = dir.Normalized();

				//// 始点まで移動する
				// このフレームの移動量
				float move_distance = m_move_velocity_on_begin * delta_time;
				if (distance < move_distance)
				{
					move_distance = distance;
				}
				// 座標をセット
				{
					VECTOR3 next_pos = current_pos + move_distance * dir;
					next_pos.y = m_begin_position.y;
					m_rigidbody->SetPosition(next_pos);
				}

				// 目的方向に回転する
				const auto forward_vec2 = m_mover->GetForward();
				const VECTOR3 forward = VECTOR3(forward_vec2.x, 0.0f, forward_vec2.y);
				// このフレームの回転量
				const float rotate_velocity = 360.0f / m_begin_time;	// 回転速度は準備する時間によって決まる
				float rotate_angle = rotate_velocity * delta_time;
				// 目的方向との差分のほうが回転量より小さいなら代入する
				const float angle_difference = VECTOR3::Angle(forward, -m_wall_normal);
				if (angle_difference < rotate_angle)
				{
					rotate_angle = angle_difference;
				}
				// 回転方向を計算する
				rotate_angle *= VECTOR3::Cross(forward, -m_wall_normal).y < 0.0f ? -1.0f : 1.0f;
				// 回転をセット
				const auto next_forward = VECTOR3::TransformRotate(forward, Quaternion::AxisAngle(VECTOR3::Up, rotate_angle));
				m_mover->SetForward(VECTOR2(next_forward.x, next_forward.z).Normalized());
				// タイマーチェックかつモーション終了の待機
				if (m_timer >= m_begin_time && m_on_end_cover_begin)
				{
					m_timer = 0.0f;
					m_state = State::Covering;
				}
			}
			break;
			case State::Covering:
			{
				// UI非表示
				m_climb_over_ui->Enabled(false);
				m_cover_ui->Enabled(false);

				// エイム状態の更新
				m_weapon_controller->ApplyInputAimUpdate();
				// 射撃の更新
				if (m_weapon_controller->GetAimUpperWeight() >= 1.0f)
				{
					m_weapon_controller->ApplyInputShotUpdate();
				}
				// リロードの更新
				else
				{
					m_weapon_controller->ApplyInputReloadUpdate();
				}

				// 体の向きをカメラ方向から計算する
				DirectionUpdate();

				// 速度の更新
				VelocityUpdate();

				// ステート遷移のチェック
				TransitionCheck();
			}
			break;
			case State::EndCover:
			{
				// カバー終了
				m_animator->SetBool("OnCovering", false);

				// ステートを遷移する
				if (m_next_is_climb_over)
				{
					// 乗り越えは即遷移
					m_parent_state.Lock();
					m_parent_state->ChangeSubState(static_cast<int>(PlayerActionState::ActionState::ClimbOver));
					m_parent_state.Unlock();
				}
				else if (m_on_end_cover_end)
				{
					// 移動に遷移はカバー終了を待つ
					m_parent_state.Lock();
					m_parent_state->ChangeSubState(static_cast<int>(PlayerActionState::ActionState::Explore));
					m_parent_state.Unlock();

					// 左スティックの入力を受け付ける
					m_player->SetAllowLStickInput(true);
				}
			}
			break;
		}

		// 正規化速度をセットする
		m_animator->SetFloat("NormVelocity", m_mover->GetSpeed() / m_max_velocity);
	}

	void PlayerCoverSubState::Exit()
	{
		// UIを非アクティブにする
		if (const auto cover_ui = m_cover_ui.GetWeak().lock())
		{
			cover_ui->Enabled(false);
		}
		if (const auto climb_over_ui = m_climb_over_ui.GetWeak().lock())
		{
			climb_over_ui->Enabled(false);
		}
	}

	void PlayerCoverSubState::TransitionCheck()
	{
		const bool on_aiming = m_weapon_controller->GetAimState() == CharacterWeaponController::AimState::Hold;

		//// カバー中にカバーボタンでカバー終了(エイム時は遷移できない)
		if (!on_aiming)
		{
			// UI表示
			m_cover_ui->Enabled(true);

			// 入力時は遷移
			if (IInput::Get().GetPadDown(0, Input::GamePad::B))
			{
				// 速度を0にする
				m_mover->SetSpeed(0.0f);
				// 左スティックの入力を止める
				m_player->SetAllowLStickInput(false);
				// 立ち状態にする
				m_player->SetPostureState(PlayerController::PostureState::Standing);

				// ステート遷移
				m_state = State::EndCover;

				return;
			}
		}

		//// 乗り越え
		// UI表示
		m_climb_over_ui->Enabled(true);
		// 入力時遷移
		if (IInput::Get().GetPadDown(0, Input::GamePad::A))
		{
			// 乗り越え情報のセット
			m_player->SetClimbOverData(GetTransform()->Position(), -m_wall_normal);

			// ステート遷移
			m_state = State::EndCover;
			m_next_is_climb_over = true;
		}
	}

	void PlayerCoverSubState::DirectionUpdate()
	{
		// カメラ方向で体の向きを計算する
		const auto camera_dir = m_camera->GetCameraDirection();
		const float cover_angle = VECTOR3::Angle(camera_dir, -m_wall_normal) * MyMath::Sign(VECTOR3::Cross(-m_wall_normal, camera_dir).Dot(VECTOR3::Up));
		m_animator->SetFloat("CoverAngle", cover_angle);

		//// エイム時の上半身だけ注視点を向く角度を求める
		// 現在の正面ベクトルとの角度差を求める
		const auto current_pos = GetTransform()->Position();
		const auto focus_pos = m_camera->GetCameraPosition() + camera_dir * m_aim_focus_distance;
		// 目的方向
		VECTOR3 toward_dir = focus_pos - current_pos;
		toward_dir.y = 0.0f;
		toward_dir = toward_dir.Normalized();
		const auto forward_vec2 = m_mover->GetForward();
		const VECTOR3 forward = VECTOR3(forward_vec2.x, 0.0f, forward_vec2.y);
		// 水平
		{
			const float aim_horizontal_angle = VECTOR3::Angle(forward, toward_dir) * (VECTOR3::Cross(toward_dir, forward).y < 0.0f ? -1.0f : 1.0f);
			m_animator->SetFloat("AimHorizontal", aim_horizontal_angle);
		}
		// 垂直
		{
			// プレイヤーの高さを加算した位置から注視点へのベクトル
			VECTOR3 aim_dir = focus_pos - (current_pos + VECTOR3(0.0f, m_player->GetPlayerHeight(), 0.0f));
			aim_dir = aim_dir.Normalized();
			// 水平ベクトル
			const VECTOR3 hor_vec = VECTOR3(aim_dir.x, 0.0f, aim_dir.z).Normalized();
			const float aim_vertical_angle = VECTOR3::Angle(hor_vec, aim_dir) * (aim_dir.y < 0.0f ? -1.0f : 1.0f);
			m_animator->SetFloat("AimVertical", aim_vertical_angle);
		}
	}

	void PlayerCoverSubState::VelocityUpdate()
	{
		// エイム中は停止する
		if (m_weapon_controller->GetAimWeight() > 0.5f)
		{
			m_mover->SetSpeed(0.0f);
			return;
		}

		const float delta_time = ITime::Get().DeltaTime();
		float current_speed = m_mover->GetSpeed();

		// ワールド空間入力値と移動方向のなす角度が移動可能な範囲かチェック
		const auto world_input_vec2 = m_camera->GetWorldSpaceInputLStick();
		const VECTOR3 world_input = VECTOR3(world_input_vec2.x, 0.0f, world_input_vec2.y);
		const float angle_difference = MyMath::Abs(VECTOR3::Angle(world_input, m_wall_normal) - 90.0f);
		const float brake_speed = m_brake * delta_time;
		if (world_input_vec2.Length() > 0.1f && angle_difference < m_allow_input_different_angle)
		{
			const float added_speed = m_acceleration * delta_time;

			// 入力方向を右か左に二極化して速度を加算
			if (m_wall_right_axis.Dot(world_input) > 0.0f)
			{
				// 右
				if (!m_hit_limit_right)
				{
					current_speed += added_speed;
				}
				else
				{
					current_speed -= brake_speed;
					current_speed = MyMath::Max(0.0f, current_speed);
				}
			}
			else
			{
				// 左
				if (!m_hit_limit_left)
				{
					current_speed -= added_speed;
				}
				else
				{
					current_speed += brake_speed;
					current_speed = MyMath::Min(0.0f, current_speed);
				}
			}
			current_speed = MyMath::Clamp(current_speed, -m_max_velocity, m_max_velocity);
		}
		else
		{
			// 右に速度を持っている
			if (current_speed > 0.0f)
			{
				current_speed -= brake_speed;
				current_speed = MyMath::Max(0.0f, current_speed);
			}
			// 左に速度を持っている
			else if (current_speed < 0.0f)
			{
				current_speed += brake_speed;
				current_speed = MyMath::Min(0.0f, current_speed);
			}
		}
		m_mover->SetSpeed(current_speed);
	}

	void PlayerCoverSubState::OnEndCoverBegin()
	{
		m_on_end_cover_begin = true;
	}

	void PlayerCoverSubState::OnEndCoverEnd()
	{
		m_on_end_cover_end = true;
	}

	/////////////////////////////////////////////////////////////////////////
	// GUI設定
	/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	void PlayerCoverSubState::OnGUI()
	{
	}
#endif
}
