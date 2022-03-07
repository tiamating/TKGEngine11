#include "CameraController.h"

#include "../Player/PlayerController.h"
#include "../CharacterWeaponController.h"

REGISTERCOMPONENT(TKGEngine::CameraController);

namespace TKGEngine
{
	/////////////////////////////////////////////////////////////////////////
	// 実体生成時に呼び出される
	/////////////////////////////////////////////////////////////////////////
	void CameraController::Awake()
	{
		// 更新はプレイヤー入力の更新後
		Priority(-10);
	}

	/////////////////////////////////////////////////////////////////////////
	// Updateが呼ばれる前に一度だけ呼び出される
	/////////////////////////////////////////////////////////////////////////
	void CameraController::Start()
	{
		// 毎フレーム更新関数を呼び出すかどうか
		m_can_call_every_frame_update = true;

		// コンポーネントの取得
		// Transform
		m_camera_transform = IGameObject::Find("MainCamera")->GetTransform();
		const auto s_ptr_player = IGameObject::Find("unitychan")->GetTransform();
		m_target = s_ptr_player->GetTransform();
		// PlayerController
		m_player = s_ptr_player->GetComponent<PlayerController>();
		// CharacterWeaponController
		m_weapon_controller = s_ptr_player->GetComponent<CharacterWeaponController>();
	}

	/////////////////////////////////////////////////////////////////////////
	// 毎フレーム呼び出される
	/////////////////////////////////////////////////////////////////////////
	void CameraController::Update()
	{
		// 前フレームの情報をコピー
		m_prev_camera_dir = m_current_camera_dir;
		m_prev_camera_pos = m_current_camera_pos;

		// 入力のよる更新
		InputUpdate();
	}

	/////////////////////////////////////////////////////////////////////////
	// Update後に毎フレーム呼び出される
	/////////////////////////////////////////////////////////////////////////
	void CameraController::LateUpdate()
	{
		// カメラの位置と回転をTransformに適用する
		ApplyToCamera();
	}


	void CameraController::AddShake(float begin_amplitude, float time)
	{
		m_shake_begin_amplitude = begin_amplitude;
		m_shake_time = time;
	}

	void CameraController::SetState(const CameraState state)
	{
		// カメラ状態をセット
		m_current_camera_state = state;

		// カバー状態にするならカバーの中央値を取得
		if (state == CameraState::Cover)
		{
			if (const auto player = m_player.lock())
			{
				const auto normal = player->GetCoverNormal();
				m_cover_base_angle = MyMath::Wrap(VECTOR3::Angle(-normal, VECTOR3::Forward) * (VECTOR3::Cross(VECTOR3::Forward, -normal).y < 0.0f ? -1.0f : 1.0f), -180.0f, 180.0f);
			}
		}
	}

	void CameraController::InputUpdate()
	{
		// パッド入力データの取得
		m_right_stick_input.SetInput(VECTOR2(IInput::Get().GetPadStickRX(0), IInput::Get().GetPadStickRY(0)));
		m_left_stick_input.SetInput(VECTOR2(IInput::Get().GetPadStickLX(0), IInput::Get().GetPadStickLY(0)));
		// カメラの位置を計算
		{
			const float unscaled_delta_time = ITime::Get().UnscaledDeltaTime();
			// X
			m_camera_angle_horizontal -= m_right_stick_input.GetInput().x * m_camera_angular_velocity_horizontal * unscaled_delta_time;
			m_camera_angle_horizontal = MyMath::Wrap(m_camera_angle_horizontal, -180.0f, 180.0f);
			if (m_current_camera_state == CameraState::Cover)
			{
				//// カバー状態なら中央値から90度範囲に収める
				const float difference = MyMath::Wrap(m_camera_angle_horizontal - m_cover_base_angle, -180.0f, 180.0f);
				// Max
				if (difference > 90.0f)
				{
					m_camera_angle_horizontal = MyMath::Wrap(m_cover_base_angle + 90.0f, -180.0f, 180.0f);
				}
				// Min
				if (difference < -90.0f)
				{
					m_camera_angle_horizontal = MyMath::Wrap(m_cover_base_angle - 90.0f, -180.0f, 180.0f);
				}
			}

			// Y
			m_camera_angle_vertical += m_right_stick_input.GetInput().y * m_camera_angular_velocity_vertical * unscaled_delta_time;
			m_camera_angle_vertical = MyMath::Clamp(m_camera_angle_vertical, m_max_camera_angle_vertical_negative, m_max_camera_angle_vertical_positive);

			// 回転からカメラの方向を求める
			const float sin_phy = MyMath::Sin(MyMath::AngleToRadian(m_camera_angle_vertical));
			const float cos_phy = MyMath::Cos(MyMath::AngleToRadian(m_camera_angle_vertical));
			const float sin_theta = MyMath::Sin(MyMath::AngleToRadian(m_camera_angle_horizontal));
			const float cos_theta = MyMath::Cos(MyMath::AngleToRadian(m_camera_angle_horizontal));

			m_current_camera_dir = VECTOR3(sin_theta * cos_phy, sin_phy, cos_theta * cos_phy).Normalized();
		}
		// 左スティック入力値をワールド空間に変換する
		{
			const float camera_radian_horizontal = MyMath::AngleToRadian(m_camera_angle_horizontal);
			const float cos_horizontal = MyMath::Cos(camera_radian_horizontal);
			const float sin_horizontal = MyMath::Sin(camera_radian_horizontal);
			// Z軸反転の回転行列でワールドX-Z平面に変換
			m_input_direction_world_space = VECTOR2(
				-cos_horizontal * m_left_stick_input.GetDirection().x + sin_horizontal * m_left_stick_input.GetDirection().y,
				sin_horizontal * m_left_stick_input.GetDirection().x + cos_horizontal * m_left_stick_input.GetDirection().y
			).Normalized();
		}
	}

	void CameraController::FixedRayTest()
	{
		// ターゲット位置からカメラ位置への方向、距離
		VECTOR3 target_to_camera = m_current_camera_pos - m_current_target_pos;
		const float length_target_to_camera = target_to_camera.Length();
		target_to_camera = target_to_camera.Normalized();

		// レイを飛ばして位置を補正する
		Physics::RaycastHit ray_data;
		constexpr int hit_layer = (1 << static_cast<int>(Layer::Stage));
		if (Physics::Raycast(m_current_target_pos, target_to_camera, ray_data, length_target_to_camera, false, hit_layer))
		{
			m_current_camera_pos = m_current_target_pos + target_to_camera * ray_data.distance;
		}
	}

	void CameraController::ApplyToCamera()
	{
		// 参照先の取得
		const auto transform = GetTransform();
		const auto target_transform = m_target.lock();
		if (target_transform == nullptr)
		{
			LOG_ASSERT("failed weak_ptr::lock() CameraController");
			return;
		}
		const auto camera_transform = m_camera_transform.lock();
		if (camera_transform == nullptr)
		{
			LOG_ASSERT("failed weak_ptr::lock() CameraController");
			return;
		}

		// ターゲットから注視点を求める
		CalculateFocusPosition();

		// カメラの位置を注視点と視線方向から計算する
		CalculateCameraPosition();

		// カメラが埋まらないようにレイで補正する
		FixedRayTest();

		// 注視点の位置
		transform->Position(m_current_focus_pos);
		// カメラの位置、方向
		camera_transform->Position(m_current_camera_pos);
		camera_transform->Rotation(Quaternion::LookRotation(m_current_camera_dir));
	}

	void CameraController::CalculateFocusPosition()
	{
		switch (m_current_camera_state)
		{
			case CameraState::Explore:
			{
				OnCalculateFocusPositionStateExplore();
			}
			break;
			case CameraState::Cover:
			{
				OnCalculateFocusPositionStateCover();
			}
			break;
			case CameraState::Slide:
			{
				OnCalculateFocusPositionStateSlide();
			}
			break;
		}
	}

	void CameraController::CalculateCameraPosition()
	{
		switch (m_current_camera_state)
		{
			case CameraState::Explore:
			{
				OnCalculateCameraPositionStateExplore();
			}
			break;
			case CameraState::Cover:
			{
				OnCalculateCameraPositionStateCover();
			}
			break;
			case CameraState::Slide:
			{
				OnCalculateCameraPositionStateSlide();
			}
			break;
		}
	}

	void CameraController::OnCalculateFocusPositionStateExplore()
	{
		const auto target_transform = m_target.lock();
		const auto player = m_player.lock();
		const auto weapon_controller = m_weapon_controller.lock();

		const float aim_weight = weapon_controller->GetAimWeight();
		const float delta_time = ITime::Get().DeltaTime();
		const float camera_speed_height = delta_time * m_target_height_change_speed;
		const float camera_speed_distance = delta_time * m_target_distance_change_speed;

		// ターゲットから注視点にずらす方向を求める
		const VECTOR3 target_to_focus_dir = VECTOR3::Cross(m_current_camera_dir, VECTOR3::Up).Normalized();
		// 高さを考慮したターゲット位置を求める
		const float crouch_weight = player->GetCrouchRatio();
		const float height_for_posture = m_target_height * (1.0f - crouch_weight) + m_target_crouch_height * crouch_weight;
		const float current_height = height_for_posture * (1.0f - aim_weight) + m_target_aim_height * aim_weight;
		// カメラの変化速度に収める
		{
			if (MyMath::Abs(current_height - m_current_target_height) > camera_speed_height)
			{
				if (current_height < m_current_target_height)
					m_current_target_height = m_current_target_height - camera_speed_height;
				else
					m_current_target_height = m_current_target_height + camera_speed_height;
			}
			else
			{
				m_current_target_height = current_height;
			}
		}
		m_current_target_pos = target_transform->Position() + VECTOR3::Up * m_current_target_height;
		// Aim状態によって離す距離を決める
		const float current_distance = m_target_distance * (1.0f - aim_weight) + m_target_aim_distance * aim_weight;
		// カメラの変化速度に収める
		{
			if(MyMath::Abs(current_distance - m_current_target_distance) > camera_speed_distance)
			{
				if (current_distance < m_current_target_distance)
					m_current_target_distance = m_current_target_distance - camera_speed_distance;
				else
					m_current_target_distance = m_current_target_distance + camera_speed_distance;
			}
			else
			{
				m_current_target_distance = current_distance;
			}
		}
		// 注視点を決める
		const VECTOR3 focus_pos = m_current_target_pos + target_to_focus_dir * m_current_target_distance;
		m_current_focus_pos = focus_pos;
	}

	void CameraController::OnCalculateCameraPositionStateExplore()
	{
		const auto weapon_controller = m_weapon_controller.lock();

		const float delta_time = ITime::Get().DeltaTime();
		const float camera_speed_distance = delta_time * m_camera_distance_change_speed;

		// カメラの位置を注視点と視線方向から計算する
		const float aim_weight = weapon_controller->GetAimWeight();
		const float current_distance = m_camera_distance * (1.0f - aim_weight) + m_camera_aim_distance * aim_weight;
		// カメラの変化速度に収める
		{
			if (MyMath::Abs(current_distance - m_current_camera_distance) > camera_speed_distance)
			{
				if (current_distance < m_current_camera_distance)
					m_current_camera_distance = m_current_camera_distance - camera_speed_distance;
				else
					m_current_camera_distance = m_current_camera_distance + camera_speed_distance;
			}
			else
			{
				m_current_camera_distance = current_distance;
			}
		}
		const auto new_camera_pos = m_current_focus_pos - m_current_camera_dir * m_current_camera_distance;

		m_current_camera_pos = new_camera_pos;
	}

	void CameraController::OnCalculateFocusPositionStateCover()
	{
		const auto target_transform = m_target.lock();
		const auto weapon_controller = m_weapon_controller.lock();

		const float aim_weight = weapon_controller->GetAimWeight();
		const float delta_time = ITime::Get().DeltaTime();
		const float camera_speed_height = delta_time * m_target_height_change_speed;
		const float camera_speed_distance = delta_time * m_target_distance_change_speed;

		// ターゲットから注視点にずらす方向を求める
		const VECTOR3 target_to_focus_dir = VECTOR3::Cross(m_current_camera_dir, VECTOR3::Up).Normalized();
		// 高さを考慮したターゲット位置を求める
		const float current_height = m_target_cover_height * (1.0f - aim_weight) + m_target_aim_height * aim_weight;
		// カメラの変化速度に収める
		{
			if (MyMath::Abs(current_height - m_current_target_height) > camera_speed_height)
			{
				if (current_height < m_current_target_height)
					m_current_target_height = m_current_target_height - camera_speed_height;
				else
					m_current_target_height = m_current_target_height + camera_speed_height;
			}
			else
			{
				m_current_target_height = current_height;
			}
		}
		m_current_target_pos = target_transform->Position() + VECTOR3::Up * m_current_target_height;
		// Aim状態によって離す距離を決める
		const float current_distance = m_target_cover_distance * (1.0f - aim_weight) + m_target_aim_distance * aim_weight;
		// カメラの変化速度に収める
		{
			if (MyMath::Abs(current_distance - m_current_target_distance) > camera_speed_distance)
			{
				if (current_distance < m_current_target_distance)
					m_current_target_distance = m_current_target_distance - camera_speed_distance;
				else
					m_current_target_distance = m_current_target_distance + camera_speed_distance;
			}
			else
			{
				m_current_target_distance = current_distance;
			}
		}
		// 注視点を決める
		const VECTOR3 focus_pos = m_current_target_pos + target_to_focus_dir * m_current_target_distance;
		m_current_focus_pos = focus_pos;
	}

	void CameraController::OnCalculateCameraPositionStateCover()
	{
		const auto weapon_controller = m_weapon_controller.lock();

		const float delta_time = ITime::Get().DeltaTime();
		const float camera_speed_distance = delta_time * m_camera_distance_change_speed;

		// カメラの位置を注視点と視線方向から計算する
		const float aim_weight = weapon_controller->GetAimWeight();
		const float current_distance = m_camera_cover_distance * (1.0f - aim_weight) + m_camera_aim_distance * aim_weight;
		// カメラの変化速度に収める
		{
			if (MyMath::Abs(current_distance - m_current_camera_distance) > camera_speed_distance)
			{
				if (current_distance < m_current_camera_distance)
					m_current_camera_distance = m_current_camera_distance - camera_speed_distance;
				else
					m_current_camera_distance = m_current_camera_distance + camera_speed_distance;
			}
			else
			{
				m_current_camera_distance = current_distance;
			}
		}
		const auto new_camera_pos = m_current_focus_pos - m_current_camera_dir * m_current_camera_distance;

		m_current_camera_pos = new_camera_pos;
	}

	void CameraController::OnCalculateFocusPositionStateSlide()
	{
		const auto target_transform = m_target.lock();

		const float delta_time = ITime::Get().DeltaTime();
		const float camera_speed_height = delta_time * m_target_height_change_speed;
		const float camera_speed_distance = delta_time * m_target_distance_change_speed;

		// ターゲットから注視点にずらす方向を求める
		const VECTOR3 target_to_focus_dir = VECTOR3::Cross(m_current_camera_dir, VECTOR3::Up).Normalized();
		// 高さを考慮したターゲット位置を求める
		const float current_height = m_target_slide_height;
		// カメラの変化速度に収める
		{
			if (MyMath::Abs(current_height - m_current_target_height) > camera_speed_height)
			{
				if (current_height < m_current_target_height)
					m_current_target_height = m_current_target_height - camera_speed_height;
				else
					m_current_target_height = m_current_target_height + camera_speed_height;
			}
			else
			{
				m_current_target_height = current_height;
			}
		}
		m_current_target_pos = target_transform->Position() + VECTOR3::Up * m_current_target_height;
		// Aim状態によって離す距離を決める
		const float current_distance = m_target_slide_distance;
		// カメラの変化速度に収める
		{
			if (MyMath::Abs(current_distance - m_current_target_distance) > camera_speed_distance)
			{
				if (current_distance < m_current_target_distance)
					m_current_target_distance = m_current_target_distance - camera_speed_distance;
				else
					m_current_target_distance = m_current_target_distance + camera_speed_distance;
			}
			else
			{
				m_current_target_distance = current_distance;
			}
		}
		// 注視点を決める
		const VECTOR3 focus_pos = m_current_target_pos + target_to_focus_dir * m_current_target_distance;
		m_current_focus_pos = focus_pos;
	}

	void CameraController::OnCalculateCameraPositionStateSlide()
	{
		const float delta_time = ITime::Get().DeltaTime();
		const float camera_speed_distance = delta_time * m_camera_distance_change_speed;

		// カメラの位置を注視点と視線方向から計算する
		const float current_distance = m_camera_slide_distance;
		// カメラの変化速度に収める
		{
			if (MyMath::Abs(current_distance - m_current_camera_distance) > camera_speed_distance)
			{
				if (current_distance < m_current_camera_distance)
					m_current_camera_distance = m_current_camera_distance - camera_speed_distance;
				else
					m_current_camera_distance = m_current_camera_distance + camera_speed_distance;
			}
			else
			{
				m_current_camera_distance = current_distance;
			}
		}
		const auto new_camera_pos = m_current_focus_pos - m_current_camera_dir * m_current_camera_distance;

		m_current_camera_pos = new_camera_pos;
	}


	/////////////////////////////////////////////////////////////////////////
	// GUI設定
	/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	void CameraController::OnGUI()
	{
		// 注視点、カメラの距離
		ImGui::Text("Distance");
		ImGui::Indent(ImGui::INDENT_VALUE);
		ImGui::Text("Target Height");
		ImGui::AlignedSameLine(0.5f);
		ImGui::SliderFloat("##Target Height", &m_target_height, 0.0f, 5.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::Text("Target Dist");
		ImGui::AlignedSameLine(0.5f);
		ImGui::SliderFloat("##Target Dist", &m_target_distance, 0.0f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::Text("Camera Dist");
		ImGui::AlignedSameLine(0.5f);
		ImGui::SliderFloat("##Camera Dist", &m_camera_distance, 0.0f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::Unindent(ImGui::INDENT_VALUE);
		// 角速度
		ImGui::Text("Angular velocity");
		ImGui::Indent(ImGui::INDENT_VALUE);
		ImGui::Text("Horizontal");
		ImGui::AlignedSameLine(0.5f);
		ImGui::SliderFloat("##Angular Horizontal", &m_camera_angular_velocity_horizontal, 10.0f, 180.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::Text("Vertical");
		ImGui::AlignedSameLine(0.5f);
		ImGui::SliderFloat("##Angular Vertical", &m_camera_angular_velocity_vertical, 10.0f, 180.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::Unindent(ImGui::INDENT_VALUE);
		// カメラ角度上限
		ImGui::Text("Max angular");
		ImGui::Indent(ImGui::INDENT_VALUE);
		ImGui::Text("Positive");
		ImGui::AlignedSameLine(0.5f);
		ImGui::SliderFloat("##Angular Positive", &m_max_camera_angle_vertical_positive, 10.0f, 80.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::Text("Negative");
		ImGui::AlignedSameLine(0.5f);
		ImGui::SliderFloat("##Angular Negative", &m_max_camera_angle_vertical_negative, -80.0f, -10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::Unindent(ImGui::INDENT_VALUE);

	}
#endif
}
