#pragma once
#include "Components/inc/CMonoBehaviour.h"
#include "../ICharacterSubState.h"

namespace TKGEngine
{
	class PlayerActionState;
	class PlayerController;
	class CharacterMoveController;
	class CharacterWeaponController;
	class CameraController;
	class RigidBody;
	class Animator;
	class UIRenderer;


	class PlayerExploreSubState
		: public MonoBehaviour
		, public ICharacterSubState
	{
	public:
		PlayerExploreSubState() = default;
		virtual ~PlayerExploreSubState() override = default;
		PlayerExploreSubState(const PlayerExploreSubState&) = delete;
		PlayerExploreSubState& operator=(const PlayerExploreSubState&) = delete;

	private:
		virtual void Awake() override;
		virtual void Start() override;

		virtual void Enter() override;
		virtual void BeforeExecute() override;
		virtual void AfterExecute() override;
		virtual void Execute() override;
		virtual void Exit() override;

		virtual void OnTriggerEnter(std::shared_ptr<Collider>& collider) override;
		virtual void OnTriggerExit(std::shared_ptr<Collider>& collider) override;

		//////////////////////////////////
		// functions
		//////////////////////////////////
		// 姿勢の変更
		void ChangePostureState();

		// 方向の更新
		void DirectionUpdate();
		// 速度の更新
		void VelocityUpdate();
		// アニメーションコントローラにセット
		void ApplyToParameter();

		// エイム状態用更新
		void DirectionUpdateOnAim();
		void VelocityUpdateOnAim();
		void ApplyToParameterOnAim();


		// 壁乗り越えチェックと遷移
		bool OnPushClimbOver();	// 遷移時はtrue
		bool CheckClimbOver(Physics::RaycastHit& ray_data);
		void TransitionClimbOver(const Physics::RaycastHit& ray_data);

		// カバー遷移
		bool OnPushCover();	// 遷移時はtrue
		bool CheckCover(Physics::RaycastHit& ray_data, const VECTOR3& ray_direction);
		void TransitionCover(const Physics::RaycastHit& ray_data, const VECTOR3& ray_direction);

			
		//////////////////////////////////
		// variables
		//////////////////////////////////
		// 参照
		SWPtr<PlayerActionState> m_parent_state;
		SWPtr<PlayerController> m_player;
		SWPtr<CharacterMoveController> m_mover;
		SWPtr<CharacterWeaponController> m_weapon_controller;
		SWPtr<CameraController> m_camera;
		SWPtr<RigidBody> m_rigidbody;
		SWPtr<Animator> m_animator;
		SWPtr<UIRenderer> m_cover_ui;
		SWPtr<UIRenderer> m_climb_over_ui;

		// 入力値
		VECTOR2 m_left_stick_input_world_space = VECTOR2::Zero;

		// 旋回情報
		float m_different_angle_calc_input = 0.0f;	// 現在の速度方向と目的方向の角度差
		float m_different_angle_calc_next_dir = 0.0f;	// フレーム間の角度差
		float m_max_different_angle_end_turn = 15.0f;	// 旋回終了とする差分角度
		float m_turn_weight_on_move_begin = 0.0f;	// 移動開始時の回転速度重み
		VECTOR2 m_turn_to_direction_on_begin = VECTOR2::Zero;	// 移動開始時の目標方向
		
		// 状態フラグ
		bool m_on_move_begin = false;	// 移動開始時判定フラグ
		bool m_is_running_on_begin = false;	// 移動開始時に走り状態か

		// 速度設定
		float m_max_run_speed = 3.0f;	// 最大走り速度
		float m_max_walk_speed = 1.5f;	// 最大歩き速度
		float m_max_crouch_speed = 0.8f;	// 最大しゃがみ速度
		float m_linear_run_acceleration = 6.0f;		// 走り加速度
		float m_linear_walk_acceleration = 2.5f;	// 歩き加速度
		float m_linear_crouch_acceleration = 1.5f;	// しゃがみ加速度
		float m_linear_brake = 10.0f;	// 停止時減速加速度
		float m_sliding_threshold = 2.8f;	// スライディング可能閾値

		// 振り向き設定
		float m_angular_speed = 200.0f;	// 旋回角速度
		float m_angular_speed_on_move_begin = 200.0f;	// 移動開始時の旋回速度

		// 乗り越え判定
		bool m_can_climb_over = false;
		float m_climb_over_check_ray_length = 2.0f;	// 乗り越え判定をするレイの長さ
		float m_climb_over_max_different_angle = 60.0f;	// レイと衝突した面法線のなす角度の許容する最大角度差

		// カバー判定
		bool m_can_cover = false;
		float m_cover_check_ray_length = 2.0f;	// カバー判定をするレイの長さ

		// エイム状態
		float m_max_aim_speed = 1.0f;	// 最大エイム時速度
		float m_aim_acceleration = 3.0f;	// エイム時加速度
		float m_aim_brake = 10.0f;	// エイム時減速加速度
		float m_aim_angular_speed = 540.0f;	// エイム時回転速度
		float m_aim_focus_distance = 20.0f;	// エイム注視点距離
		VECTOR2 m_aim_current_forward = VECTOR2::Zero;	// エイム時の現在の体正面
		VECTOR2 m_aim_current_toward = VECTOR2::Zero;	// エイム時現在の体目的方向
		float m_aim_horizontal_angle = 0.0f;	// エイム時の上半身の水平角度
		float m_aim_vertical_angle = 0.0f;	// エイム時の上半身の垂直角度


	public:
		//////////////////////////////////
		// アニメーションイベント
		//////////////////////////////////
		// 移動開始時に初速度と回転角度をセットする
		void OnMoveBegin();


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
CEREAL_CLASS_VERSION(TKGEngine::PlayerExploreSubState, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::PlayerExploreSubState, "TKGEngine::PlayerExploreSubState")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::PlayerExploreSubState)
