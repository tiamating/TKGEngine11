#include "PlayerStateMachine.h"

REGISTERCOMPONENT(TKGEngine::PlayerStateMachine);

namespace TKGEngine
{
	void PlayerStateMachine::Awake()
	{
		// 更新は各ステートの後、移動コントローラの前
		Priority(8);
	}

	void PlayerStateMachine::Start()
	{
		// 初期ステートのセット
		SetState(static_cast<int>(PlayerState::Action));
	}

	void PlayerStateMachine::Update()
	{
		// 現在のステートの処理を実行
		m_state_pool.at(m_current_state)->BeforeExecute();
		m_state_pool.at(m_current_state)->Execute();
		m_state_pool.at(m_current_state)->AfterExecute();
	}


	/////////////////////////////////////////////////////////////////////////
	// GUI設定
	/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	void PlayerStateMachine::OnGUI()
	{
		// State
		ImGui::Text("State:");
		ImGui::AlignedSameLine(0.5f);
		const int current_state = GetStateIndex();
		if (current_state < 0)
			ImGui::TextColored(ImVec4(0.8f, 0.0f, 0.0f, 1.0f), "No Registered");
		else
			ImGui::Text(NAMEOF_ENUM(static_cast<PlayerState>(current_state)).data());
	}
#endif
}
