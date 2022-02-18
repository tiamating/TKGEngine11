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

	class PlayerCoverSubState
		: public MonoBehaviour
		, public ICharacterSubState
	{
	public:
		PlayerCoverSubState() = default;
		virtual ~PlayerCoverSubState() override = default;
		PlayerCoverSubState(const PlayerCoverSubState&) = delete;
		PlayerCoverSubState& operator=(const PlayerCoverSubState&) = delete;

	private:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void OnTriggerEnter(std::shared_ptr<Collider>& collider) override;
		virtual void OnTriggerExit(std::shared_ptr<Collider>& collider) override;

		virtual void Enter() override;
		virtual void BeforeExecute() override;
		virtual void AfterExecute() override;
		virtual void Execute() override;
		virtual void Exit() override;

		//////////////////////////////////
		// functions
		//////////////////////////////////
		// 遷移チェック
		void TransitionCheck();

		// 方向の更新
		void DirectionUpdate();

		// 速度更新
		void VelocityUpdate();


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

		// 開始時の目標位置
		VECTOR3 m_begin_position = VECTOR3::Zero;
		// 壁の法線
		VECTOR3 m_wall_normal = VECTOR3::Zero;
		// 壁基準右+X方向
		VECTOR3 m_wall_right_axis = VECTOR3::UnitX;

		// モーションの状態
		enum class State
		{
			Ready,
			BeginCover,
			Covering,
			EndCover
		};
		State m_state = State::Ready;
		// タイマー
		float m_timer = 0.0f;
		// 遷移先がClimbOverか
		bool m_next_is_climb_over = false;
		// カバー開始が終了したか
		bool m_on_end_cover_begin = false;
		// カバー終了が終了したか
		bool m_on_end_cover_end = false;
		// 移動上限に触れているか
		bool m_hit_limit_left = false;
		bool m_hit_limit_right = false;

		// 始点までの移動速度
		float m_move_velocity_on_begin = 3.0f;
		// 始点まで移動する時間
		float m_begin_time = 0.5f;

		// 移動速度
		float m_max_velocity = 0.6f;
		float m_acceleration = 2.0f;
		float m_brake = 15.0f;

		// 壁に沿って移動できる入力角度と移動方向の角度差
		float m_allow_input_different_angle = 50.0f;

		// エイム状態
		float m_aim_focus_distance = 20.0f;	// エイム注視点距離


	public:
		//////////////////////////////////
		// アニメーションイベント
		//////////////////////////////////

		// カバー開始のモーション終了時
		void OnEndCoverBegin();
		// カバー終了モーションの終了時
		void OnEndCoverEnd();



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
CEREAL_CLASS_VERSION(TKGEngine::PlayerCoverSubState, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::PlayerCoverSubState, "TKGEngine::PlayerCoverSubState")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::PlayerCoverSubState)
