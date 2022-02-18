#include "PlayerClimbOverSubState.h"
#include "PlayerActionState.h"
#include "PlayerController.h"
#include "../CharacterMoveController.h"
#include "../CharacterWeaponController.h"
#include "Components/inc/CRigidBody.h"
#include "Components/inc/CAnimator.h"

REGISTERCOMPONENT(TKGEngine::PlayerClimbOverSubState);

namespace TKGEngine
{
	void PlayerClimbOverSubState::Awake()
	{
		// 毎フレーム更新は行わない
		m_can_call_every_frame_update = false;
	}

	void PlayerClimbOverSubState::Start()
	{
		// 親ステートに登録する
		const auto parent_state = GetComponent<PlayerActionState>();
		m_parent_state = parent_state;
		if (!parent_state)
		{
			LOG_ASSERT("failed get PlayerActionState component. (PlayerClimbOverSubState)");
			return;
		}
		parent_state->RegisterSubState(GetComponent<PlayerClimbOverSubState>(), static_cast<int>(PlayerActionState::ActionState::ClimbOver));

		// コンポーネントの取得
		// PlayerController
		m_player = GetComponent<PlayerController>();
		// CharacterMoveController
		m_mover = GetComponent<CharacterMoveController>();
		// CharacterWeaponController
		m_weapon_controller = GetComponent<CharacterWeaponController>();
		// RigidBody
		m_rigidbody = GetComponent<RigidBody>();
		// Animator
		m_animator = GetComponent<Animator>();
	}

	void PlayerClimbOverSubState::Enter()
	{
		// RigidBodyを非アクティブにする
		if (const auto rigidbody = m_rigidbody.GetWeak().lock())
		{
			rigidbody->Enabled(false);
		}

		// プレイヤーの移動更新を停止
		if (const auto mover = m_mover.GetWeak().lock())
		{
			mover->Enabled(false);
		}

		// リロードキャンセル
		if(const auto weapon = m_weapon_controller.GetWeak().lock())
		{
			weapon->CancelReload();
		}

		// 始点と終点をセット
		if (const auto player = m_player.GetWeak().lock())
		{
			m_begin_position = player->GetClimbOverBegin();
			m_direction = player->GetClimbOverDirection();
			m_end_position = m_begin_position + m_direction * m_move_distance;
		}

		// アニメーション情報のセット
		if (const auto animator = m_animator.GetWeak().lock())
		{
			// 更新されないアニメーション情報をリセット
			animator->SetTrigger("OnClimbOver");
			// 立ち状態における正規化速度
			animator->SetFloat("NormVelocity", 0.0f);
		}

		// 状態をセット
		m_timer = 0.0f;
		m_state = State::Ready;
	}

	void PlayerClimbOverSubState::BeforeExecute()
	{
		// コンポーネントを取得
		m_rigidbody.Lock();
	}

	void PlayerClimbOverSubState::AfterExecute()
	{
		// コンポーネントの破棄
		m_rigidbody.Unlock();
	}

	void PlayerClimbOverSubState::Execute()
	{
		const auto transform = GetTransform();
		const float delta_time = ITime::Get().DeltaTime();
		// タイマー加算
		m_timer += delta_time;
		
		switch (m_state)
		{
			case State::Ready:
			{
				// 移動処理の速度を0にする
				if (const auto mover = m_mover.GetWeak().lock())
				{
					mover->SetSpeed(0.0f);
				}

				m_state = State::MoveBegin;
			}
			break;
			case State::MoveBegin:
			{
				// 移動距離と移動方向
				const VECTOR3 current_pos = transform->Position();

				VECTOR3 dir = m_begin_position - current_pos;
				const float distance = dir.Length();
				dir = dir.Normalized();

				//// 始点まで移動する
				// このフレームの移動量
				float move_distance = m_move_velocity * delta_time;
				if (distance < move_distance)
				{
					move_distance = distance;
				}
				// 座標をセット
				transform->Position(current_pos + move_distance * dir);

				// 飛び越える方向に回転する
				const auto forward = transform->Forward();
				// このフレームの回転量
				const float rotate_velocity = 360.0f / m_begin_time;
				float rotate_angle = rotate_velocity * delta_time;
				// 目的方向との差分のほうが回転量より小さいなら代入する
				const float angle_difference = VECTOR3::Angle(forward, m_direction);
				if (angle_difference < rotate_angle)
				{
					rotate_angle = angle_difference;
				}
				// 回転方向を計算する
				const auto forward_cross_toward = VECTOR3::Cross(forward, m_direction);
				rotate_angle *= VECTOR3::Dot(VECTOR3::Up, forward_cross_toward);
				// 回転をセット
				transform->Forward(VECTOR3::TransformRotate(forward, Quaternion::AxisAngle(VECTOR3::Up, rotate_angle)));
				// タイマーチェック
				if (m_timer >= m_begin_time)
				{
					m_timer = 0.0f;
					m_state = State::ClimbOver;
				}
			}
			break;
			case State::ClimbOver:
			{
				const VECTOR3 current_pos = transform->Position();

				//// 終点まで移動する
				// このフレームの移動量
				const float move_distance = m_move_distance * delta_time;
				// 座標をセット
				transform->Position(current_pos + move_distance * m_direction);

				// タイマーチェック
				if (m_timer >= m_climb_over_time)
				{
					m_timer = 0.0f;
					m_state = State::MoveEnd;
					if(const auto animator = m_animator.GetWeak().lock())
					{
						animator->SetTrigger("EndClimbOver");
					}
				}
			}
			break;
			case State::MoveEnd:
			{
				// 着地しきるまで停止

				// タイマーチェック
				if (m_timer >= m_end_transition_time)
				{
					m_parent_state.Lock();
					m_parent_state->ChangeSubState(static_cast<int>(PlayerActionState::ActionState::Explore));
					m_parent_state.Unlock();
				}
			}
			break;
		}
	}

	void PlayerClimbOverSubState::Exit()
	{
		// RigidBodyをアクティブにする
		if (const auto rigidbody = m_rigidbody.GetWeak().lock())
		{
			rigidbody->Enabled(true);
		}

		// プレイヤーの移動更新を再開
		if (const auto mover = m_mover.GetWeak().lock())
		{
			mover->Enabled(true);
			mover->SetForward(VECTOR2(m_direction.x, m_direction.z));
		}
	}


	/////////////////////////////////////////////////////////////////////////
	// GUI設定
	/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	void PlayerClimbOverSubState::OnGUI()
	{
		ImGui::Text("Current State");
		ImGui::AlignedSameLine(0.5f);
		ImGui::Text(NAMEOF_ENUM(m_state).data());

		ImGui::Text("State Time");
		{
			ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
			ImGui::Text("Begin"); ImGui::AlignedSameLine(0.5f);
			ImGui::SliderFloat("##Begin Timer", &m_begin_time, 0.1f, 1.0f);
			ImGui::Text("Stay"); ImGui::AlignedSameLine(0.5f);
			ImGui::SliderFloat("##Stay Timer", &m_climb_over_time, 0.1f, 1.0f);
			ImGui::Text("End"); ImGui::AlignedSameLine(0.5f);
			ImGui::SliderFloat("##End Timer", &m_end_transition_time, 0.0f, 1.0f);
		}

		ImGui::Text("Move");
		{
			ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
			ImGui::Text("Length"); ImGui::AlignedSameLine(0.5f);
			ImGui::SliderFloat("##Move Length", &m_move_distance, 0.1f, 2.0f);
		}
	}
#endif
}
