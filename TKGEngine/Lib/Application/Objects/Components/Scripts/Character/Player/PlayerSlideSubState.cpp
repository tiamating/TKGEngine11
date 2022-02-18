#include "PlayerSlideSubState.h"
#include "PlayerActionState.h"
#include "PlayerController.h"
#include "../CharacterMoveController.h"
#include "../Camera/CameraController.h"
#include "Components/inc/CAnimator.h"

REGISTERCOMPONENT(TKGEngine::PlayerSlideSubState);

REGISTER_EVENT_FUNCTION_VOID(TKGEngine::PlayerSlideSubState, OnEndSlideEnd);

namespace TKGEngine
{
	void PlayerSlideSubState::Awake()
	{
		// 毎フレーム更新は行わない
		m_can_call_every_frame_update = false;
	}

	void PlayerSlideSubState::Start()
	{
		// 親ステートに登録する
		const auto parent_state = GetComponent<PlayerActionState>();
		m_parent_state = parent_state;
		if (!parent_state)
		{
			LOG_ASSERT("failed get PlayerActionState component. (PlayerSlideSubState)");
			return;
		}
		parent_state->RegisterSubState(GetComponent<PlayerSlideSubState>(), static_cast<int>(PlayerActionState::ActionState::Slide));

		// コンポーネントの取得
		// PlayerController
		m_player = GetComponent<PlayerController>();
		// CharacterMoveController
		m_mover = GetComponent<CharacterMoveController>();
		// CameraController
		const auto camera_target = IGameObject::Find("CameraTarget");
		if (camera_target == nullptr)
		{
			LOG_ASSERT("failed find \"CameraTarget\"");
			return;
		}
		m_camera = camera_target->GetComponent<CameraController>();
		// Animator
		m_animator = GetComponent<Animator>();
	}

	void PlayerSlideSubState::OnTriggerEnter(std::shared_ptr<Collider>& collider)
	{
		const auto gameobject = collider->GetGameObject();
		const Layer layer = gameobject->GetLayer();
		// カバー入力可能状態切り替え
		if (layer == Layer::CoverField)
		{
			m_block_position = gameobject->GetTransform()->Position();
			m_can_cover = true;
		}
	}

	void PlayerSlideSubState::OnTriggerExit(std::shared_ptr<Collider>& collider)
	{
		const auto gameobject = collider->GetGameObject();
		const Layer layer = gameobject->GetLayer();
		// カバー入力可能状態切り替え
		if (layer == Layer::CoverField)
		{
			m_can_cover = false;
		}
	}

	void PlayerSlideSubState::Enter()
	{
		// 立ち状態にする
		if (const auto player = m_player.GetWeak().lock())
		{
			player->SetPostureState(PlayerController::PostureState::Standing);
		}
		// フラグのリセット
		m_on_transition_cover = false;
		// 初速度をセット
		m_current_speed = m_initial_speed;
		// スライディング中にする
		m_on_sliding = true;
		m_on_end_slide_end = false;
	}

	void PlayerSlideSubState::BeforeExecute()
	{
		// コンポーネントを取得
		m_player.Lock();
		m_mover.Lock();
		m_camera.Lock();
		m_animator.Lock();
	}

	void PlayerSlideSubState::AfterExecute()
	{
		// コンポーネントの破棄
		m_player.Unlock();
		m_mover.Unlock();
		m_camera.Unlock();
		m_animator.Unlock();
	}

	void PlayerSlideSubState::Execute()
	{
		// 入力方向が進行方向と異なるか、入力量が0の場合はスライディングを終了
		const auto world_input_dir = m_camera->GetWorldSpaceInputLStick();
		const float diff_angle = m_mover->GetVelocityDirection().Dot(world_input_dir);
		if (diff_angle > m_allow_different_angle || MyMath::Approximately(m_player->GetLStickData().GetLength(), 0.0f))
		{
			m_on_sliding = false;
		}
		// 速度が閾値以下になったらスライディングを終了
		if (m_current_speed < m_slide_threshold_speed)
		{
			m_on_sliding = false;
		}
		// スライディング中にしゃがみ入力を行った場合はスライディングを終了
		if (IInput::Get().GetPadDown(0, Input::GamePad::B))
		{
			m_on_sliding = false;
		}

		// スライディング実行中にカバー可能範囲に入ったらチェックを実行
		if(m_on_sliding && m_can_cover)
		{
			if(CheckCoverTransition())
			{
				// スライディング状態の終了
				m_on_sliding = false;
				m_on_transition_cover = true;
				// しゃがみ状態にする
				m_player->SetPostureState(PlayerController::PostureState::Crouching);
			}
		}

		// スライディング起き上がりが終了したら探索ステートにする
		if (m_on_end_slide_end)
		{
			m_parent_state.Lock();
			m_parent_state->ChangeSubState(static_cast<int>(PlayerActionState::ActionState::Explore));
			m_parent_state.Unlock();
		}

		// 速度の更新
		UpdateVelocity();

		// Animatorにセット
		ApplyToParameter();
	}

	void PlayerSlideSubState::Exit()
	{
		/* nothing */
	}

	void PlayerSlideSubState::UpdateVelocity()
	{
		// カバーに遷移するときは速度を更新しない
		if (m_on_transition_cover)
			return;

		const float delta_time = ITime::Get().DeltaTime();
		// スライディング中
		if (m_on_sliding)
		{
			// 減速処理
			m_current_speed -= m_brake * delta_time;
		}
		// 0以上にする
		m_current_speed = MyMath::Max(0.0f, m_current_speed);
		// 速度をセットする
		m_mover->SetSpeed(m_current_speed);
	}

	void PlayerSlideSubState::ApplyToParameter()
	{
		// スライディング状態のフラグをコントローラにセット
		m_animator->SetBool("OnSliding", m_on_sliding);
	}

	bool PlayerSlideSubState::CheckCoverTransition()
	{
		Physics::RaycastHit ray_data;

		// ブロックに衝突した位置を求める
		const auto transform = GetTransform();
		const VECTOR3 pos = transform->Position();
		const VECTOR3 ray_origin = pos + VECTOR3(0.0f, 0.5f, 0.0f);
		VECTOR3 ray_end = m_block_position;
		ray_end.y = ray_origin.y;
		if (Physics::Linecast(ray_origin, ray_end, ray_data, false, 1 << static_cast<int>(Layer::Stage)))
		{
			// 進行方向とブロックの方向の角度差が最大角度差に収まるか
			const auto vel_dir_vec2 = m_mover->GetVelocityDirection();
			const VECTOR3 vel_dir = VECTOR3(vel_dir_vec2.x, 0.0f, vel_dir_vec2.y).Normalized();
			if(VECTOR3::Angle(vel_dir, -ray_data.normal) < m_max_cover_different_angle)
			{
				// カバーに遷移
				TransitionCover(ray_data, (ray_end - ray_origin).Normalized());
				return true;
			}
		}
		return false;
	}

	void PlayerSlideSubState::TransitionCover(const Physics::RaycastHit& ray_data, const VECTOR3& ray_direction)
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


	void PlayerSlideSubState::OnEndSlideEnd()
	{
		m_on_end_slide_end = true;
	}

	/////////////////////////////////////////////////////////////////////////
	// GUI設定
	/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	void PlayerSlideSubState::OnGUI()
	{
		ImGui::Text("Init Speed");
		ImGui::AlignedSameLine(0.5f);
		ImGui::SliderFloat("##Init Speed", &m_initial_speed, 0.0f, 20.0f);
		ImGui::Text("Slide Brake");
		ImGui::AlignedSameLine(0.5f);
		ImGui::SliderFloat("##Slide Brake", &m_brake, 0.0f, 10.0f);
		ImGui::Text("End Threshold");
		ImGui::AlignedSameLine(0.5f);
		ImGui::SliderFloat("##End Threshold", &m_slide_threshold_speed, 0.0f, 10.0f);
	}
#endif
}
