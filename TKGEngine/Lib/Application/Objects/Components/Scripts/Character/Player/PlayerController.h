#pragma once
#include "Components/inc/CMonoBehaviour.h"

#include "StickInputData.h"

namespace TKGEngine
{
	class Animator;
	class CharacterMoveController;
	class CharacterWeaponController;;

	class PlayerController
		: public TKGEngine::MonoBehaviour
	{
	public:
		PlayerController() = default;
		virtual ~PlayerController() override = default;
		PlayerController(const PlayerController&) = delete;
		PlayerController& operator=(const PlayerController&) = delete;


	public:
		// 姿勢状態
		enum class PostureState
		{
			Standing,
			Crouching,
			Max_PostureState
		};
		//////////////////////////////////
		// functions
		//////////////////////////////////
		// 姿勢状態
		void SetPostureState(PostureState state);
		PostureState GetPostureState() const;
		float GetCrouchRatio() const { return m_crouch_ratio; }

		// 左スティック入力状態
		const StickInputData& GetLStickData() const { return m_left_stick_input; }
		// 左スティックの入力を受けつけるか
		void SetAllowLStickInput(const bool allow) { m_allow_left_stick_input = allow; }

		// プレイヤーの半径
		float GetPlayerRadius() const { return m_player_radius; }
		// プレイヤーの高さ
		float GetPlayerHeight() const { return m_player_height; }

		// 走り状態
		void SetRunState(const bool on_running) { m_on_running = on_running; }
		bool IsRunning() const { return m_on_running; }
		// スライディング
		bool CanSliding() const { return m_sliding_timer <= 0.0f; }
		// 壁乗り越え
		void SetClimbOverData(const VECTOR3& begin, const VECTOR3& direction);
		const VECTOR3& GetClimbOverBegin() const { return m_climb_over_begin_position; }
		const VECTOR3& GetClimbOverDirection() const { return m_climb_over_direction; }
		bool CanClimbOver() const { return m_can_climb_over; }
		// カバー
		void SetCoverData(const VECTOR3& position, const VECTOR3& normal);
		const VECTOR3& GetCoverPosition() const { return m_cover_position; }
		const VECTOR3& GetCoverNormal() const { return m_cover_wall_normal; }



	private:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Update() override;
		virtual void LateUpdate() override;


		//////////////////////////////////
		// functions
		//////////////////////////////////
		// フレーム最初に実行
		void OnUpdateBegin();
		// フレーム最後に実行
		void OnUpdateEnd();

		// 姿勢の更新
		void UpdatePosture();
		// モーション更新
		void MotionUpdate();
		

		//////////////////////////////////
		// variables
		//////////////////////////////////
		// 参照
		SWPtr<Animator> m_animator;
		SWPtr<CharacterMoveController> m_mover;
		SWPtr<CharacterWeaponController> m_weapon_controller;

		// スティック入力データ
		StickInputData m_left_stick_input;
		// 左スティックの入力を受けつけるか
		bool m_allow_left_stick_input = true;

		// プレイヤーの半径
		float m_player_radius = 0.5f;
		// プレイヤーの高さ
		float m_player_height = 1.4f;


		// 姿勢用変数
		PostureState m_posture_state = PostureState::Standing;
		float m_crouch_ratio = 0.0f;	// しゃがみのブレンド率
		float m_crouch_weight = 0.0f;	// しゃがみの重み
		float m_crouch_time = 0.35f;	// しゃがみにかかる時間
		float m_stand_time = 0.45f;		// 立ち上がりにかかる時間

		// 走り状態
		bool m_on_running = false;
		// スライディング
		float m_sliding_recast_time = 0.5f;
		float m_sliding_timer = 0.0f;
		// 壁乗り越えの始点、方向
		VECTOR3 m_climb_over_begin_position = VECTOR3::Zero;
		VECTOR3 m_climb_over_direction = VECTOR3::Zero;
		bool m_can_climb_over = true;
		// カバー開始の目的位置と壁の法線方向
		VECTOR3 m_cover_position = VECTOR3::Zero;
		VECTOR3 m_cover_wall_normal = VECTOR3::Zero;



	public:
		//////////////////////////////////
		// アニメーションイベント
		//////////////////////////////////
		// 壁乗り越え終了時に次の入力を許可する
		void OnEndClimbOver();



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
CEREAL_CLASS_VERSION(TKGEngine::PlayerController, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::PlayerController, "TKGEngine::PlayerController")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::PlayerController)
