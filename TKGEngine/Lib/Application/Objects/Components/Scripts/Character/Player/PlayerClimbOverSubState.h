#pragma once
#include "Components/inc/CMonoBehaviour.h"
#include "../ICharacterSubState.h"

namespace TKGEngine
{
	class PlayerActionState;
	class PlayerController;
	class CameraController;
	class CharacterMoveController;
	class CharacterWeaponController;
	class RigidBody;
	class Animator;

	class PlayerClimbOverSubState
		: public MonoBehaviour
		, public ICharacterSubState
	{
	public:
		PlayerClimbOverSubState() = default;
		virtual ~PlayerClimbOverSubState() override = default;
		PlayerClimbOverSubState(const PlayerClimbOverSubState&) = delete;
		PlayerClimbOverSubState& operator=(const PlayerClimbOverSubState&) = delete;

	private:
		virtual void Awake() override;
		virtual void Start() override;

		virtual void Enter() override;
		virtual void BeforeExecute() override;
		virtual void AfterExecute() override;
		virtual void Execute() override;
		virtual void Exit() override;

		//////////////////////////////////
		// functions
		//////////////////////////////////



		//////////////////////////////////
		// variables
		//////////////////////////////////
		// 参照
		SWPtr<PlayerActionState> m_parent_state;
		SWPtr<PlayerController> m_player;
		SWPtr<CameraController> m_camera;
		SWPtr<CharacterMoveController> m_mover;
		SWPtr<CharacterWeaponController> m_weapon_controller;
		SWPtr<RigidBody> m_rigidbody;
		SWPtr<Animator> m_animator;

		// モーションの状態
		enum class State
		{
			Ready,
			MoveBegin,
			ClimbOver,
			MoveEnd
		};
		State m_state = State::Ready;
		// タイマー
		float m_timer = 0.0f;

		// 始点
		VECTOR3 m_begin_position = VECTOR3::Zero;
		// 終点
		VECTOR3 m_end_position = VECTOR3::Zero;
		// 始点から終点への方向
		VECTOR3 m_direction = VECTOR3::Zero;
		// 始点までの移動速度
		float m_move_velocity = 3.0f;
		// 移動量
		float m_move_distance = 1.5f;
		// 始点まで移動する時間
		float m_begin_time = 0.5f;
		// 始点から終点までの時間
		float m_climb_over_time = 1.0f;
		// 終点についてから遷移するまでの時間
		float m_end_transition_time = 0.0f;

		// しゃがみ状態だったか
		bool m_is_crouching = false;

	public:
		//////////////////////////////////
		// アニメーションイベント
		//////////////////////////////////



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
CEREAL_CLASS_VERSION(TKGEngine::PlayerClimbOverSubState, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::PlayerClimbOverSubState, "TKGEngine::PlayerClimbOverSubState")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::PlayerClimbOverSubState)
