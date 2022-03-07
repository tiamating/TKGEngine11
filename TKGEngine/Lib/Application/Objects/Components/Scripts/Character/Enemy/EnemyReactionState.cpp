#include "EnemyReactionState.h"
#include "EnemyStateMachine.h"
#include "../CharacterHealthController.h"

REGISTERCOMPONENT(TKGEngine::EnemyReactionState);

namespace TKGEngine
{
	void EnemyReactionState::Awake()
	{
		// 毎フレーム更新は行わない
		m_can_call_every_frame_update = false;
	}

	void EnemyReactionState::Start()
	{
		// ステートマシンに登録する
		const auto statemachine = GetComponent<EnemyStateMachine>();
		if (!statemachine)
		{
			LOG_ASSERT("failed get EnemyStateMachine component. (EnemyReactionState)");
			return;
		}
		statemachine->RegisterState(GetComponent<EnemyReactionState>(), static_cast<int>(EnemyStateMachine::EnemyState::Reaction));
	}

	void EnemyReactionState::Enter()
	{
		// 残りHPを見て遷移
		if(const auto health = GetComponent<CharacterHealthController>())
		{
			// TODO : 現在ダメージステートがないため、被弾時は即死
			
			// Damage
			if(health->GetHealth() > 0)
			{
				SetSubState(static_cast<int>(ReactionState::Death));
			}
			// Death
			else
			{
				SetSubState(static_cast<int>(ReactionState::Death));
			}
		}
		else
		{
			LOG_ASSERT("failed transition enemy reaction state.");
			SetSubState(static_cast<int>(ReactionState::Death));
		}
	}

	void EnemyReactionState::BeforeExecute()
	{
		// サブステートを実行
		if (const auto& current_sub_state = GetSubState())
		{
			current_sub_state->BeforeExecute();
		}
	}

	void EnemyReactionState::Execute()
	{
		// サブステートを実行
		if (const auto& current_sub_state = GetSubState())
		{
			current_sub_state->Execute();
		}
	}

	void EnemyReactionState::AfterExecute()
	{
		// サブステートを実行
		if (const auto& current_sub_state = GetSubState())
		{
			current_sub_state->AfterExecute();
		}
	}

	void EnemyReactionState::Exit()
	{
	}


	/////////////////////////////////////////////////////////////////////////
	// GUI設定
	/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	void EnemyReactionState::OnGUI()
	{
	}
#endif
}
