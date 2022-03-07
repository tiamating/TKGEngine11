#pragma once
#include "Components/inc/CMonoBehaviour.h"

#include "../Player/StickInputData.h"

namespace TKGEngine
{
	class PlayerController;
	class CharacterWeaponController;;

	class CameraController
		: public TKGEngine::MonoBehaviour
	{
	public:
		CameraController() = default;
		virtual ~CameraController() override = default;
		CameraController(const CameraController&) = delete;
		CameraController& operator=(const CameraController&) = delete;

		// カメラの種類
		enum class CameraState
		{
			Explore,
			Cover,
			Slide,

			Max_CameraState
		};


	private:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Update() override;
		virtual void LateUpdate() override;

	private:
		std::weak_ptr<Transform> m_camera_transform;
		std::weak_ptr<Transform> m_target;
		std::weak_ptr<PlayerController> m_player;
		std::weak_ptr<CharacterWeaponController> m_weapon_controller;

		// カメラの状態パラメータ
		CameraState m_current_camera_state = CameraState::Explore;
		
		//// ターゲットからの高さ
		// カメラの変化速度
		float m_target_height_change_speed = 1.0f;
		// 現在の高さ
		float m_current_target_height = 1.4f;
		// 立ち状態での高さ
		float m_target_height = 1.4f;
		// しゃがみ状態での高さ
		float m_target_crouch_height = 0.8f;
		// カバー状態での高さ
		float m_target_cover_height = 1.4f;
		// Aim状態の高さ
		float m_target_aim_height = 1.2f;
		// slide状態の高さ
		float m_target_slide_height = 1.0f;

		//// ターゲットから離す距離
		// カメラの変化速度
		float m_target_distance_change_speed = 2.0f;
		// 現在の離す距離
		float m_current_target_distance = 0.8f;
		// 立ち状態の離す距離
		float m_target_distance = 0.8f;
		// しゃがみ時の離す距離
		float m_target_crouch_distance = 0.6f;
		// カバー時の離す距離
		float m_target_cover_distance = 0.8f;
		// Aim時の離す距離
		float m_target_aim_distance = 0.4f;
		// slide状態の距離
		float m_target_slide_distance = 0.8f;

		//// 注視点からカメラまでの長さ
		// カメラの変化速度
		float m_camera_distance_change_speed = 5.0f;
		// 現在の離す距離
		float m_current_camera_distance = 2.0f;
		// 立ち状態のカメラまでの長さ
		float m_camera_distance = 2.0f;
		// しゃがみ時のカメラまでの長さ
		float m_camera_crouch_distance = 1.5f;
		// カバー時のカメラまでの長さ
		float m_camera_cover_distance = 2.0f;
		// Aim時のカメラまでの長さ
		float m_camera_aim_distance = 0.5f;
		// slide時のカメラまでの長さ
		float m_camera_slide_distance = 2.0f;

		// カメラの最大回転速度
		float m_camera_angular_velocity_vertical = 80.0f;
		float m_camera_angular_velocity_horizontal = 100.0f;
		// カメラの回転角度[degree]
		float m_camera_angle_horizontal = -180.0f;
		float m_camera_angle_vertical = 0.0f;
		// カメラの角度上限
		float m_max_camera_angle_vertical_positive = 50.0f;
		float m_max_camera_angle_vertical_negative = -50.0f;
		// カメラ位置
		VECTOR3 m_prev_camera_pos = VECTOR3::Zero;
		VECTOR3 m_current_camera_pos = VECTOR3::Zero;
		// カメラ方向
		VECTOR3 m_prev_camera_dir = VECTOR3::Forward;
		VECTOR3 m_current_camera_dir = VECTOR3::Forward;
		// 注視点
		VECTOR3 m_current_focus_pos = VECTOR3::Zero;
		// 高さを考慮したターゲット位置
		VECTOR3 m_current_target_pos = VECTOR3::Zero;

		// スティック入力データ
		StickInputData m_right_stick_input;
		StickInputData m_left_stick_input;
		// 左スティックのワールド変換値
		VECTOR2 m_input_direction_world_space = VECTOR2::Zero;

		// カメラシェイクのパラメータ
		VECTOR2 m_shake_displacement = VECTOR2::Zero;
		float m_shake_time = 0.0f;
		float m_shake_timer = 0.0f;
		float m_shake_begin_amplitude = 0.0f;

		// カバー状態のカメラのパラメータ
		float m_cover_base_angle = 0.0f;


	public:
		// カメラの回転量を返す
		float GetCameraAngleVertical() const;
		float GetCameraAngleHorizontal() const;
		// カメラ方向を考慮した左スティック入力XZ平面方向を返す
		const VECTOR2& GetWorldSpaceInputLStick() const
		{
			return m_input_direction_world_space;
		}
		// カメラの視線方向
		const VECTOR3& GetCameraDirection() const { return m_current_camera_dir; }
		// カメラ座標
		const VECTOR3& GetCameraPosition() const { return m_current_camera_pos; }

		// カメラシェイク
		void AddShake(float begin_amplitude, float time);

		// カメラ状態
		void SetState(const CameraState state);
		CameraState GetState() const { return m_current_camera_state; }
		

	private:
		/// <summary>
		/// 入力によるカメラ操作
		/// </summary>
		void InputUpdate();

		/// <summary>
		/// Rayによるカメラ位置の補正
		/// </summary>
		void FixedRayTest();

		/// <summary>
		/// カメラに結果を適用する
		/// </summary>
		void ApplyToCamera();

		/// <summary>
		/// カメラの状態ごとに注視点とカメラ位置を計算した結果を返す
		/// </summary>
		void CalculateFocusPosition();
		void CalculateCameraPosition();
		// State::Explore
		void OnCalculateFocusPositionStateExplore();
		void OnCalculateCameraPositionStateExplore();
		// State::Cover
		void OnCalculateFocusPositionStateCover();
		void OnCalculateCameraPositionStateCover();
		// State::Slide
		void OnCalculateFocusPositionStateSlide();
		void OnCalculateCameraPositionStateSlide();

	private:
		/////////////////////////////////////////////////////////////////////////
		// Serialize設定
		/////////////////////////////////////////////////////////////////////////
		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			if (version == 3)
			{
				archive(
					cereal::base_class<TKGEngine::MonoBehaviour>(this),
					CEREAL_NVP(m_target_height),
					CEREAL_NVP(m_target_distance),
					CEREAL_NVP(m_camera_distance),
					CEREAL_NVP(m_camera_angular_velocity_vertical),
					CEREAL_NVP(m_camera_angular_velocity_horizontal),
					CEREAL_NVP(m_max_camera_angle_vertical_positive),
					CEREAL_NVP(m_max_camera_angle_vertical_negative)
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

	// ------------------------------------------------
	// inline 
	// ------------------------------------------------
	inline float CameraController::GetCameraAngleVertical() const
	{
		return m_camera_angle_vertical;
	}

	inline float CameraController::GetCameraAngleHorizontal() const
	{
		return m_camera_angle_horizontal;
	}


}

/////////////////////////////////////////////////////////////////////////
// Serialize登録
/////////////////////////////////////////////////////////////////////////
CEREAL_CLASS_VERSION(TKGEngine::CameraController, 3)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::CameraController, "TKGEngine::CameraController")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::CameraController)
