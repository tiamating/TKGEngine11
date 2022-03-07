#include "EnemyActionState.h"
#include "EnemyStateMachine.h"

REGISTERCOMPONENT(TKGEngine::EnemyActionState);

namespace TKGEngine
{
	void EnemyActionState::Awake()
	{
		// 毎フレーム更新は行わない
		m_can_call_every_frame_update = false;
	}
	
	void EnemyActionState::Start()
	{
		// ステートマシンに登録する
		m_state_machine = GetComponent<EnemyStateMachine>();
		const auto s_ptr_state_machine = m_state_machine.lock();
		if (!s_ptr_state_machine)
		{
			LOG_ASSERT("failed get EnemyStateMachine component. (EnemyActionState)");
			return;
		}
		s_ptr_state_machine->RegisterState(GetComponent<EnemyActionState>(), static_cast<int>(EnemyStateMachine::EnemyState::Action));
	}

	void EnemyActionState::Enter()
	{
		// 初期状態をセット
		SetSubState(static_cast<int>(ActionState::Search));
	}

	void EnemyActionState::BeforeExecute()
	{
		// サブステートを実行
		if (const auto& current_sub_state = GetSubState())
		{
			current_sub_state->BeforeExecute();
		}
	}

	void EnemyActionState::Execute()
	{
		// サブステートを実行
		if (const auto& current_sub_state = GetSubState())
		{
			current_sub_state->Execute();
		}
	}

	void EnemyActionState::AfterExecute()
	{
		// サブステートを実行
		if (const auto& current_sub_state = GetSubState())
		{
			current_sub_state->AfterExecute();
		}
	}

	void EnemyActionState::Exit()
	{
	}

	/////////////////////////////////////////////////////////////////////////
	// GUI設定
	/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	void EnemyActionState::OnGUI()
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
