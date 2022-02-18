#include "CameraController.h"

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

		m_camera_transform = IGameObject::Find("MainCamera")->GetTransform();
		const auto s_ptr_player = IGameObject::Find("unitychan")->GetTransform();
		m_target = s_ptr_player->GetTransform();
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

	void CameraController::InputUpdate()
	{
		// パッド入力データの取得
		m_right_stick_input.SetInput(VECTOR2(IInput::Get().GetPadStickRX(0), IInput::Get().GetPadStickRY(0)));
		m_left_stick_input.SetInput(VECTOR2(IInput::Get().GetPadStickLX(0), IInput::Get().GetPadStickLY(0)));
		// カメラの位置を計算
		{
			const float unscaled_delta_time = ITime::Get().UnscaledDeltaTime();
			// X
			m_camera_angle_horizontal += m_right_stick_input.GetInput().x * m_camera_angular_velocity_horizontal * unscaled_delta_time;
			m_camera_angle_horizontal = MyMath::Wrap(m_camera_angle_horizontal, -180.0f, 180.0f);
			// Y
			m_camera_angle_vertical += m_right_stick_input.GetInput().y * m_camera_angular_velocity_vertical * unscaled_delta_time;
			m_camera_angle_vertical = MyMath::Clamp(m_camera_angle_vertical, m_max_camera_angle_vertical_negative, m_max_camera_angle_vertical_positive);

			// 回転からカメラの方向を求める
			const float sin_phy = MyMath::Sin(MyMath::AngleToRadian(m_camera_angle_vertical));
			const float cos_phy = MyMath::Cos(MyMath::AngleToRadian(m_camera_angle_vertical));
			const float sin_theta = MyMath::Sin(MyMath::AngleToRadian(m_camera_angle_horizontal));
			const float cos_theta = MyMath::Cos(MyMath::AngleToRadian(m_camera_angle_horizontal));

			m_current_camera_dir = VECTOR3(sin_theta * cos_phy, sin_phy, -cos_theta * cos_phy).Normalized();
		}
		// 左スティック入力値をワールド空間に変換する
		{
			const float camera_radian_horizontal = MyMath::AngleToRadian(m_camera_angle_horizontal);
			const float cos_horizontal = MyMath::Cos(camera_radian_horizontal);
			const float sin_horizontal = MyMath::Sin(camera_radian_horizontal);
			// Z軸反転の回転行列でワールドX-Z平面に変換
			m_input_direction_world_space = VECTOR2(
				cos_horizontal * m_left_stick_input.GetDirection().x + sin_horizontal * m_left_stick_input.GetDirection().y,
				sin_horizontal * m_left_stick_input.GetDirection().x - cos_horizontal * m_left_stick_input.GetDirection().y
			).Normalized();
		}
	}

	void CameraController::FixedRayTest()
	{

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
		if(camera_transform == nullptr)
		{
			LOG_ASSERT("failed weak_ptr::lock() CameraController");
			return;
		}

		// ターゲットから注視点を求める
		const VECTOR3 target_to_focus_dir = VECTOR3::Cross(m_current_camera_dir, VECTOR3::Up).Normalized();
		const VECTOR3 focus_pos = target_transform->Position() + target_to_focus_dir * m_target_distance + VECTOR3::Up * m_target_height;
		// 注視点の位置
		transform->Position(focus_pos);

		// カメラの位置を注視点と視線方向から計算する
		m_current_camera_pos = focus_pos - m_current_camera_dir * m_camera_distance;
		camera_transform->Position(m_current_camera_pos);
		camera_transform->LookAt(focus_pos);
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
