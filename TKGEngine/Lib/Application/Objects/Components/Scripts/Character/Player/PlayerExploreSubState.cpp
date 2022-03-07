#include "PlayerExploreSubState.h"
#include "PlayerActionState.h"
#include "PlayerController.h"
#include "../CharacterMoveController.h"
#include "../CharacterWeaponController.h"
#include "../Camera/CameraController.h"
#include "Components/inc/CRigidBody.h"
#include "Components/inc/CAnimator.h"
#include "Components/inc/CUIRenderer.h"

REGISTERCOMPONENT(TKGEngine::PlayerExploreSubState);

REGISTER_EVENT_FUNCTION_VOID(TKGEngine::PlayerExploreSubState, OnMoveBegin);

namespace TKGEngine
{
	void PlayerExploreSubState::Awake()
	{
		// 毎フレーム更新は行わない
		m_can_call_every_frame_update = false;
	}

	void PlayerExploreSubState::Start()
	{
		// 親ステートに登録する
		const auto parent_state = GetComponent<PlayerActionState>();
		m_parent_state = parent_state;
		if (!parent_state)
		{
			LOG_ASSERT("failed get PlayerActionState component. (PlayerExploreSubState)");
			return;
		}
		parent_state->RegisterSubState(GetComponent<PlayerExploreSubState>(), static_cast<int>(PlayerActionState::ActionState::Explore));

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

	void PlayerExploreSubState::Enter()
	{
		// 走り状態の取得
		if (const auto player = m_player.GetWeak().lock())
		{
			if (IInput::Get().GetPad(0, Input::GamePad::LSHOULDER))
			{
				player->SetRunState(true);
				player->SetPostureState(PlayerController::PostureState::Standing);
			}
			else
			{
				player->SetRunState(false);
			}
		}

		// 状態の初期化
		m_on_move_begin = false;

		// カメラ状態セット
		if(const auto camera = m_camera.GetWeak().lock())
		{
			camera->SetState(CameraController::CameraState::Explore);
		}
	}

	void PlayerExploreSubState::BeforeExecute()
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

	void PlayerExploreSubState::AfterExecute()
	{
		// コンポーネントの破棄
		m_player.Unlock();
		m_mover.Unlock();
		m_weapon_controller.Lock();
		m_camera.Unlock();
		m_rigidbody.Unlock();
		m_animator.Unlock();
		m_climb_over_ui.Unlock();
		m_cover_ui.Unlock();
	}

	void PlayerExploreSubState::Execute()
	{
		// UI非表示
		m_climb_over_ui->Enabled(false);
		m_cover_ui->Enabled(false);

		// エイム状態の更新
		m_weapon_controller->ApplyInputAimUpdate();
		// エイム中かどうか
		const bool on_aiming = (m_weapon_controller->GetAimState() == CharacterWeaponController::AimState::Hold);
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

		// カメラから見た入力値の取得
		m_left_stick_input_world_space = m_camera->GetWorldSpaceInputLStick();

		// 走り入力判定
		if (IInput::Get().GetPadDown(0, Input::GamePad::LSHOULDER))
		{
			m_player->SetRunState(true);
			// 走りになるときは立ち状態にする
			m_player->SetPostureState(PlayerController::PostureState::Standing);
		}
		else if (IInput::Get().GetPadUp(0, Input::GamePad::LSHOULDER))
		{
			m_player->SetRunState(false);
		}

		// カバー遷移
		if(m_can_cover)
		{
			// カメラ方向にレイを飛ばす
			const VECTOR3 camera_dir = m_camera->GetCameraDirection();
			const VECTOR3 ray_dir = VECTOR3(camera_dir.x, 0.0f, camera_dir.z).Normalized();

			Physics::RaycastHit ray_data;
			// レイでカバーオブジェクトをチェックする
			if (CheckCover(ray_data, ray_dir))
			{
				// UI表示
				m_cover_ui->Enabled(true);

				// 入力時は遷移
				if (IInput::Get().GetPadDown(0, Input::GamePad::B))
				{
					// 遷移とカバー時の情報をセット
					TransitionCover(ray_data, ray_dir);
					return;
				}
			}
		}

		// スライディング遷移もしくはしゃがみ姿勢の変更
		if (IInput::Get().GetPadDown(0, Input::GamePad::B))
		{
			//// カバー遷移
			//if (m_can_cover)
			//{
			//	// ボタン入力時はレイで判定を実行
			//	if (OnPushCover())
			//		return;
			//}

			// スライディングに移行
			if (m_player->CanSliding() && m_player->IsRunning() && m_mover->GetSpeed() > m_sliding_threshold)
			{
				m_parent_state.Lock();
				m_parent_state->ChangeSubState(static_cast<int>(PlayerActionState::ActionState::Slide));
				m_parent_state.Unlock();
				return;
			}
			// しゃがみと立ちの状態を変更
			else
			{
				// 姿勢の変更
				ChangePostureState();
				// 走り状態の解除
				m_player->SetRunState(false);
			}
		}
		//// 壁乗り越え遷移(ボタンとスティック入力がある)
		//else if (m_can_climb_over && m_player->CanClimbOver() && !on_aiming &&
		//	IInput::Get().GetPadDown(0, Input::GamePad::A) &&
		//	m_player->GetLStickData().GetLength() > 0.0f
		//	)
		//{
		//	// ボタン入力時はレイで判定を実行
		//	if (OnPushClimbOver())
		//		return;
		//}
		// 壁乗り越え遷移(ボタンとスティック入力がある)
		else if (m_can_climb_over && m_player->CanClimbOver() && !on_aiming &&
			m_player->GetLStickData().GetLength() > 0.0f
			)
		{
			Physics::RaycastHit ray_data;
			if (CheckClimbOver(ray_data))
			{
				// UIを表示
				m_climb_over_ui->Enabled(true);

				// ボタン入力時は遷移実行
				if (IInput::Get().GetPadDown(0, Input::GamePad::A))
				{
					// 遷移と乗り越え時の情報をセット
					TransitionClimbOver(ray_data);
					return;
				}
			}
		}



		// エイム時
		if (on_aiming)
		{
			// 移動方向の固定を解除
			m_on_move_begin = false;

			// 移動更新
			DirectionUpdateOnAim();
			VelocityUpdateOnAim();
			// Animatorにセット
			ApplyToParameterOnAim();
		}
		// 非エイム時
		else
		{
			// 移動更新
			DirectionUpdate();
			VelocityUpdate();
			// Animatorにセット
			ApplyToParameter();
		}
	}

	void PlayerExploreSubState::Exit()
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

	void PlayerExploreSubState::OnTriggerEnter(std::shared_ptr<Collider>& collider)
	{
		const Layer layer = collider->GetGameObject()->GetLayer();
		// 乗り越え入力可能状態切り替え
		if (layer == Layer::ClimbField)
		{
			m_can_climb_over = true;
		}
		// カバー入力可能状態切り替え
		else if (layer == Layer::CoverField)
		{
			m_can_cover = true;
		}
	}

	void PlayerExploreSubState::OnTriggerExit(std::shared_ptr<Collider>& collider)
	{
		const Layer layer = collider->GetGameObject()->GetLayer();
		// 乗り越え入力可能状態切り替え
		if (layer == Layer::ClimbField)
		{
			m_can_climb_over = false;
		}
		// カバー入力可能状態切り替え
		else if (layer == Layer::CoverField)
		{
			m_can_cover = false;
		}
	}

	void PlayerExploreSubState::ChangePostureState()
	{
		// 立ちならしゃがみ、しゃがみなら立ちに変化する
		const auto posture_state = m_player->GetPostureState();
		if (posture_state == PlayerController::PostureState::Standing)
		{
			m_player->SetPostureState(PlayerController::PostureState::Crouching);
		}
		else
		{
			m_player->SetPostureState(PlayerController::PostureState::Standing);
		}
	}

	void PlayerExploreSubState::DirectionUpdate()
	{
		// 経過時間
		const float delta_time = ITime::Get().DeltaTime();
		// 現在の正面方向
		const VECTOR2 current_forward = m_mover->GetForward();

		// 初期化
		m_different_angle_calc_next_dir = 0.0f;

		// 入力がなければ速度のみ変化するため早期リターン
		if (MyMath::Approximately(m_player->GetLStickData().GetLength(), 0.0f))
		{
			m_on_move_begin = false;
			return;
		}

		// 目的方向
		VECTOR2 toward_dir = m_left_stick_input_world_space;

		// Move Begin中は目標方向に到達するまで方向は変わらない
		if (m_on_move_begin)
		{
			const float different_angle = VECTOR2::Angle(m_turn_to_direction_on_begin, current_forward);
			if (different_angle < m_max_different_angle_end_turn)
			{
				// 目標方向に到達
				m_on_move_begin = false;
			}
			else
			{
				// 移動開始時の目的方向を維持する
				toward_dir = m_turn_to_direction_on_begin;
			}
		}

		// 現在の方向を目的方向に回転する
		{
			// 現在の方向と目的方向の角度差
			m_different_angle_calc_input = VECTOR2::Angle(current_forward, toward_dir);
			const float abs_different_angle = m_different_angle_calc_input;
			// 角度差の方向を求める
			{
				const auto forward_cross_toward = VECTOR3::Cross(
					VECTOR3(current_forward.x, 0.0f, current_forward.y),
					VECTOR3(toward_dir.x, 0.0f, toward_dir.y)
				);
				m_different_angle_calc_input *= forward_cross_toward.y < 0.0f ? -1.0f : 1.0f;
			}

			// 回転速度
			const float angular_speed = m_on_move_begin ? (m_angular_speed_on_move_begin * m_turn_weight_on_move_begin) : m_angular_speed;
			// 回転量
			float rotate_angle = angular_speed * delta_time;
			if (rotate_angle > abs_different_angle)
				rotate_angle = abs_different_angle;
			m_different_angle_calc_next_dir = rotate_angle * MyMath::Sign(m_different_angle_calc_input);
			// 現在の正面方向を回転する
			const VECTOR3 current_dir = VECTOR3(current_forward.x, 0.0f, current_forward.y);
			const auto different_rotation = Quaternion::AxisAngle(VECTOR3::Up, m_different_angle_calc_next_dir);
			const VECTOR3 next_dir = VECTOR3::TransformRotate(current_dir, different_rotation);
			const VECTOR2 next_forward = VECTOR2(next_dir.x, next_dir.z).Normalized();

			// 方向を更新
			m_mover->SetForward(next_forward);
		}
	}

	void PlayerExploreSubState::VelocityUpdate()
	{
		// 経過時間
		const float delta_time = ITime::Get().DeltaTime();
		// 現在の速度情報を取得
		const VECTOR2 current_velocity_direction = m_mover->GetVelocityDirection();
		const VECTOR2 current_forward = m_mover->GetForward();
		VECTOR2 next_velocity_direction = VECTOR2::Zero;
		const float current_speed = m_mover->GetSpeed();
		float next_speed = 0.0f;

		// 入力が閾値以下なら停止処理
		if (MyMath::Approximately(m_player->GetLStickData().GetLength(), 0.0f))
		{
			const float brake_speed = m_linear_brake * delta_time;
			next_speed = current_speed - brake_speed;
			// 速度が0以下なら停止
			if (next_speed <= 0.0f)
			{
				next_speed = 0.0f;
				next_velocity_direction = VECTOR2::Zero;
			}
			// ゆっくり減速
			else
			{
				next_velocity_direction = current_velocity_direction;
			}
		}
		// 入力があれば移動処理
		else
		{
			const bool is_crouching = m_player->GetPostureState() == PlayerController::PostureState::Crouching;
			const bool on_running = m_on_move_begin ? m_is_running_on_begin : m_player->IsRunning();
			const float max_speed = is_crouching ? m_max_crouch_speed : (on_running ? m_max_run_speed : m_max_walk_speed);

			// 現在の最大速度を超えているならゆっくり減速
			if (current_speed > max_speed)
			{
				const float brake_speed = m_linear_brake * delta_time;
				next_speed = current_speed - brake_speed;
				// 速度が0以下なら停止
				if (next_speed <= 0.0f)
				{
					next_speed = 0.0f;
					next_velocity_direction = VECTOR2::Zero;
				}
				// ゆっくり減速
				else
				{
					next_velocity_direction = current_velocity_direction;
				}
			}
			// 加速、速度維持処理
			else
			{
				// 入力方向を考慮して速度を加算する
				{
					const float acceleration = is_crouching ? m_linear_crouch_acceleration : (on_running ? m_linear_run_acceleration : m_linear_walk_acceleration);
					const float added_speed = acceleration * delta_time;
					// 目的方向
					const VECTOR2 toward_dir = m_left_stick_input_world_space;

					// 移動開始時は
					if (m_on_move_begin)
					{
						// 加速する
						next_speed = current_speed + added_speed;
						// 速度上限に丸める
						next_speed = MyMath::Min(next_speed, max_speed);
						// 速度方向をセットする
						next_velocity_direction = m_turn_to_direction_on_begin;
					}
					// 加速分は目的方向に加算する
					else
					{
						const float vel_dir_dot_input = VECTOR2::Dot(current_velocity_direction, current_forward);
						VECTOR2 next_velocity;
						if (vel_dir_dot_input > 0.0f)
							next_velocity = m_left_stick_input_world_space * (MyMath::Max(0.0f, vel_dir_dot_input) * current_speed + added_speed);
						else
							next_velocity = m_left_stick_input_world_space * added_speed;
						// 加算後の速度
						const float next_velocity_length = next_velocity.Length();
						if (next_velocity_length > max_speed)
							next_speed = max_speed;
						else
							next_speed = next_velocity_length;
						// 加算後の速度方向
						next_velocity_direction = current_forward;
					}
				}
			}
		}

		// 速度情報をセット
		m_mover->SetSpeed(next_speed);
		m_mover->SetVelocityDirection(next_velocity_direction);
	}

	void PlayerExploreSubState::ApplyToParameter()
	{
		// 正規化角度差
		m_animator->SetFloat("NormDifferentAngle", m_different_angle_calc_input / 180.0f);
		// 立ち状態における正規化速度
		m_animator->SetFloat("NormVelocity", m_mover->GetSpeed() / m_max_run_speed);
	}

	void PlayerExploreSubState::DirectionUpdateOnAim()
	{
		// 経過時間
		const float delta_time = ITime::Get().DeltaTime();

		//// 現在のForwardをゆっくりカメラ方向に向ける
		const auto current_forward_vec2 = m_mover->GetForward();
		const auto current_forward = VECTOR3(current_forward_vec2.x, 0.0f, current_forward_vec2.y).Normalized();
		const auto current_pos = GetTransform()->Position();
		const auto camera_dir = m_camera->GetCameraDirection();
		const auto focus_pos = m_camera->GetCameraPosition() + camera_dir * m_aim_focus_distance;
		// 目的方向
		VECTOR3 toward_dir = focus_pos - current_pos;
		toward_dir.y = 0.0f;
		toward_dir = toward_dir.Normalized();
		// 現在の方向と目的方向の角度差
		float diff_angle = VECTOR3::Angle(current_forward, toward_dir);
		{
			// 角度差の方向を求める
			const auto forward_cross_toward = VECTOR3::Cross(current_forward, toward_dir);
			diff_angle *= forward_cross_toward.y < 0.0f ? -1.0f : 1.0f;
		}
		// 回転量
		const float rotate_angle = m_aim_angular_speed * delta_time;
		diff_angle = MyMath::Clamp(diff_angle, -rotate_angle, rotate_angle);
		// 現在の正面方向を回転する
		const auto different_rotation = Quaternion::AxisAngle(VECTOR3::Up, diff_angle);
		const VECTOR3 next_dir = VECTOR3::TransformRotate(current_forward, different_rotation);
		const VECTOR2 next_forward = VECTOR2(next_dir.x, next_dir.z).Normalized();
		// 回転後の正面ベクトルをセット
		m_mover->SetForward(next_forward);

		//// エイム時の上半身だけ注視点を向く角度を求める
		// 現在の正面ベクトルとの角度差を求める
		// 水平
		{
			const VECTOR3 forward_vec3 = VECTOR3(next_forward.x, 0.0f, next_forward.y);
			m_aim_horizontal_angle = VECTOR3::Angle(forward_vec3, toward_dir) * (VECTOR3::Cross(forward_vec3, toward_dir).y < 0.0f ? -1.0f : 1.0f);
		}
		// 垂直
		{
			// プレイヤーの高さを加算した位置から注視点へのベクトル
			VECTOR3 aim_dir = focus_pos - (current_pos + VECTOR3(0.0f, m_player->GetPlayerHeight(), 0.0f));
			aim_dir = aim_dir.Normalized();
			// 水平ベクトル
			const VECTOR3 hor_vec = VECTOR3(aim_dir.x, 0.0f, aim_dir.z).Normalized();
			m_aim_vertical_angle = VECTOR3::Angle(hor_vec, aim_dir) * (aim_dir.y < 0.0f ? -1.0f : 1.0f);
		}

	}

	void PlayerExploreSubState::VelocityUpdateOnAim()
	{
		// 経過時間
		const float delta_time = ITime::Get().DeltaTime();

		// 現在の速度情報を取得
		const VECTOR2 current_velocity_direction = m_mover->GetVelocityDirection();
		const VECTOR2 current_forward = m_mover->GetForward();
		VECTOR2 next_velocity_direction = VECTOR2::Zero;
		const float brake_speed = m_aim_brake * delta_time;
		const float current_speed = m_mover->GetSpeed();
		float next_speed = 0.0f;

		// 入力が閾値以下なら停止処理
		if (MyMath::Approximately(m_player->GetLStickData().GetLength(), 0.0f))
		{
			next_speed = current_speed - brake_speed;
			// 速度が0以下なら停止
			if (next_speed <= 0.0f)
			{
				next_speed = 0.0f;
				next_velocity_direction = VECTOR2::Zero;
			}
			// ゆっくり減速
			else
			{
				next_velocity_direction = current_velocity_direction;
			}
		}
		// 入力があれば移動処理
		else
		{
			// 現在の最大速度を超えているならゆっくり減速
			if (current_speed > m_max_aim_speed)
			{
				next_speed = current_speed - brake_speed;
				// 速度が0以下なら停止
				if (next_speed <= 0.0f)
				{
					next_speed = 0.0f;
					next_velocity_direction = VECTOR2::Zero;
				}
				// ゆっくり減速
				else
				{
					next_velocity_direction = current_velocity_direction;
				}
			}
			// 加速、速度維持処理
			else
			{
				// 加算された速度量
				const float added_speed = m_aim_acceleration * delta_time;
				// 加算された後の速度ベクトル
				const float vel_dir_dot_input = VECTOR2::Dot(current_velocity_direction, m_left_stick_input_world_space);
				VECTOR2 next_velocity;
				if (vel_dir_dot_input >= 0.0f)
					next_velocity = m_left_stick_input_world_space * (MyMath::Max(0.0f, vel_dir_dot_input) * current_speed + added_speed);
				else
					next_velocity = m_left_stick_input_world_space * added_speed;
				// 加算後の速度量
				const float next_velocity_length = next_velocity.Length();
				if (next_velocity_length > m_max_aim_speed)
					next_speed = m_max_aim_speed;
				else
					next_speed = next_velocity_length;
				// 加算後の速度方向
				next_velocity_direction = m_left_stick_input_world_space;
			}
		}
		// 入力との角度差
		const VECTOR3 next_velocity_direction_vec3 = VECTOR3(next_velocity_direction.x, 0.0f, next_velocity_direction.y);
		const VECTOR3 current_forward_vec3 = VECTOR3(current_forward.x, 0.0f, current_forward.y);
		if (next_velocity_direction == VECTOR2::Zero)
		{
			m_different_angle_calc_input = 0.0f;
		}
		else
		{
			m_different_angle_calc_input = VECTOR3::Angle(next_velocity_direction_vec3, current_forward_vec3) *
				(VECTOR3::Cross(current_forward_vec3, next_velocity_direction_vec3).y < 0.0f ? -1.0f : 1.0f);
		}

		// 速度情報をセット
		m_mover->SetSpeed(next_speed);
		m_mover->SetVelocityDirection(next_velocity_direction);
	}

	void PlayerExploreSubState::ApplyToParameterOnAim()
	{
		//// エイム角度
		// 水平方向
		m_animator->SetFloat("AimHorizontal", m_aim_horizontal_angle);
		// 垂直方向
		m_animator->SetFloat("AimVertical", m_aim_vertical_angle);
		// エイム時正規化移動速度
		m_animator->SetFloat("NormVelocity", m_mover->GetSpeed() / m_max_aim_speed);
		// エイム時移動方向
		if (m_mover->GetVelocityDirection() != VECTOR2::Zero)
		{
			// 速度方向が存在するなら更新する
			const float cos_theta = MyMath::Cos(MyMath::AngleToRadian(m_different_angle_calc_input));
			const float sin_theta = MyMath::Sin(MyMath::AngleToRadian(m_different_angle_calc_input));
			const float horizontal_vel_dir = -sin_theta;
			const float vertical_vel_dir = cos_theta;
			m_animator->SetFloat("HorVelocity", horizontal_vel_dir);
			m_animator->SetFloat("VerVelocity", vertical_vel_dir);
		}
		// エイム時正面と移動方向の角度差
		m_animator->SetFloat("NormDifferentAngle", m_different_angle_calc_input / 180.0f);
	}

	bool PlayerExploreSubState::OnPushClimbOver()
	{
		Physics::RaycastHit ray_data;
		// レイで乗り越えるオブジェクトをチェックする
		if (CheckClimbOver(ray_data))
		{
			// 遷移と乗り越え時の情報をセット
			TransitionClimbOver(ray_data);
			return true;
		}
		return false;
	}

	bool PlayerExploreSubState::CheckClimbOver(Physics::RaycastHit& ray_data)
	{
		const auto transform = GetTransform();

		// 入力方向にレイを飛ばして、乗り越えられるオブジェクトに衝突したらステート遷移
		const VECTOR3 pos = transform->Position();
		const VECTOR3 ray_origin = pos + VECTOR3(0.0f, 0.5f, 0.0f);
		const VECTOR3 ray_dir = VECTOR3(m_left_stick_input_world_space.x, 0.0f, m_left_stick_input_world_space.y);
		// 方向ベクトルが0にならないようにする
		if (ray_dir.Length() > 0.1f)
		{
			if (Physics::Raycast(ray_origin, ray_dir, ray_data, m_climb_over_check_ray_length, false, 1 << static_cast<int>(Layer::Stage)))
			{
				// レイ方向と法線がなす角度が範囲内に収まっているか
				if (VECTOR3::Angle(ray_data.normal, -ray_dir) < m_climb_over_max_different_angle)
				{
					// 衝突したオブジェクトが乗り越えられる物ならtrue
					if (ray_data.collider->GetGameObject()->GetTag() == Tag::Wall1m)
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	void PlayerExploreSubState::TransitionClimbOver(const Physics::RaycastHit& ray_data)
	{
		const auto transform = GetTransform();

		//// 乗り越え始点、方向をセット
		// 法線
		VECTOR3 hit_normal = ray_data.normal;
		hit_normal.y = 0.0f;
		hit_normal = hit_normal.Normalized();
		VECTOR3 begin = ray_data.position;
		// 現在の高さと合わせる
		const VECTOR3 pos = transform->Position();
		begin.y = pos.y;
		// キャラクターのサイズ分手前にする
		const float radius = m_player->GetPlayerRadius();
		begin += radius * hit_normal;
		// セットする
		m_player->SetClimbOverData(begin, -hit_normal);

		// ステートの遷移
		m_parent_state.Lock();
		m_parent_state->ChangeSubState(static_cast<int>(PlayerActionState::ActionState::ClimbOver));
		m_parent_state.Unlock();
	}

	bool PlayerExploreSubState::OnPushCover()
	{
		// カメラ方向にレイを飛ばす
		const VECTOR3 camera_dir = m_camera->GetCameraDirection();
		const VECTOR3 ray_dir = VECTOR3(camera_dir.x, 0.0f, camera_dir.z).Normalized();

		Physics::RaycastHit ray_data;
		// レイでカバーオブジェクトをチェックする
		if (CheckCover(ray_data, ray_dir))
		{
			// 遷移とカバー時の情報をセット
			TransitionCover(ray_data, ray_dir);
			return true;
		}
		return false;
	}

	bool PlayerExploreSubState::CheckCover(Physics::RaycastHit& ray_data, const VECTOR3& ray_direction)
	{
		// カメラ方向にレイを飛ばして遮蔽があればカバー遷移可能
		const auto transform = GetTransform();
		const VECTOR3 pos = transform->Position();
		const VECTOR3 ray_origin = pos + VECTOR3(0.0f, 0.5f, 0.0f);
		if (Physics::Raycast(ray_origin, ray_direction, ray_data, m_cover_check_ray_length, false, 1 << static_cast<int>(Layer::Stage)))
		{
			return true;
		}
		return false;
	}

	void PlayerExploreSubState::TransitionCover(const Physics::RaycastHit& ray_data, const VECTOR3& ray_direction)
	{
		const auto transform = GetTransform();

		//// カバー目標位置、壁の法線をセット
		// 法線
		VECTOR3 hit_normal = ray_data.normal;
		hit_normal.y = 0.0f;
		hit_normal = hit_normal.Normalized();
		// 目標位置
		const float norm_dot_dir = hit_normal.Dot(-ray_direction);
		const float min_distance = norm_dot_dir * ray_data.distance;	// プレイヤーから壁までの最短距離
		const VECTOR3 pos = transform->Position();
		VECTOR3 cover_pos = pos - hit_normal * min_distance;
		// 現在の高さと合わせる
		cover_pos.y = pos.y;
		// キャラクターのサイズ分手前にする
		const float radius = m_player->GetPlayerRadius();
		cover_pos += radius * hit_normal;
		// セットする
		m_player->SetCoverData(cover_pos, hit_normal);

		// ステートの遷移
		m_parent_state.Lock();
		m_parent_state->ChangeSubState(static_cast<int>(PlayerActionState::ActionState::Cover));
		m_parent_state.Unlock();
	}

	void PlayerExploreSubState::OnMoveBegin()
	{
		// 旋回目標までの角度を保持
		if (const auto animator = m_animator.GetWeak().lock())
		{
			animator->SetFloat("MoveBeginAngle", m_different_angle_calc_input);
			m_on_move_begin = true;
			m_turn_to_direction_on_begin = m_left_stick_input_world_space;
		}
		// 移動開始時の最大速度を決める
		if (const auto player = m_player.GetWeak().lock())
		{
			m_is_running_on_begin = player->IsRunning();
		}
		// 回転目標をセット、速度を0にする
		if (const auto mover = m_mover.GetWeak().lock())
		{
			m_turn_weight_on_move_begin = MyMath::Max(30.0f / 180.0f, VECTOR2::Angle(m_turn_to_direction_on_begin, mover->GetForward()) / 180.0f);
			mover->SetSpeed(0.0f);
		}
	}

	/////////////////////////////////////////////////////////////////////////
	// GUI設定
	/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	void PlayerExploreSubState::OnGUI()
	{
		ImGui::SliderFloat("AngularSpeed", &m_angular_speed, 1.0f, 500.0f);
	}
#endif
}
