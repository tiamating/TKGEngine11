#include "PlayerActionState.h"
#include "PlayerStateMachine.h"

REGISTERCOMPONENT(TKGEngine::PlayerActionState);

namespace TKGEngine
{
	void PlayerActionState::Awake()
	{
		// 毎フレーム更新は行わない
		m_can_call_every_frame_update = false;
	}

	void PlayerActionState::Start()
	{
		// ステートマシンに登録する
		const auto statemachine = GetComponent<PlayerStateMachine>();
		if (!statemachine)
		{
			LOG_ASSERT("failed get PlayerStateMachine component. (PlayerActionState)");
			return;
		}
		statemachine->RegisterState(GetComponent<PlayerActionState>(), static_cast<int>(PlayerStateMachine::PlayerState::Action));
	}

	void PlayerActionState::Enter()
	{
		// 初期状態をセット
		SetSubState(static_cast<int>(ActionState::Explore));
	}

	void PlayerActionState::BeforeExecute()
	{
		// サブステートを実行
		if (const auto& current_sub_state = GetSubState())
		{
			current_sub_state->BeforeExecute();
		}
	}

	void PlayerActionState::Execute()
	{
		// サブステートを実行
		if(const auto& current_sub_state = GetSubState())
		{
			current_sub_state->Execute();
		}
	}

	void PlayerActionState::AfterExecute()
	{
		// サブステートを実行
		if (const auto& current_sub_state = GetSubState())
		{
			current_sub_state->AfterExecute();
		}
	}

	void PlayerActionState::Exit()
	{
	}


	/////////////////////////////////////////////////////////////////////////
	// GUI設定
	/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	void PlayerActionState::OnGUI()
	{
		// SubState
		ImGui::Text("SubState:");
		ImGui::AlignedSameLine(0.5f);
		const int current_state = GetSubStateIndex();
		if (current_state < 0)
			ImGui::TextColored(ImVec4(0.8f, 0.0f, 0.0f, 1.0f), "No Registered");
		else
			ImGui::Text(NAMEOF_ENUM(static_cast<ActionState>(current_state)).data());
	}
#endif
}
