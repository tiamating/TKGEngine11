#include "EnemySearchSubState.h"
#include "EnemyActionState.h"
#include "Components/inc/CAnimator.h"
#include "Components/inc/CRigidBody.h"

REGISTERCOMPONENT(TKGEngine::EnemySearchSubState);

namespace TKGEngine
{
	void EnemySearchSubState::Awake()
	{
		// 毎フレーム更新は行わない
		m_can_call_every_frame_update = false;
	}

	void EnemySearchSubState::Start()
	{
		// 親ステートに登録する
		const auto parent_state = GetComponent<EnemyActionState>();
		m_parent_state = parent_state;
		if (!parent_state)
		{
			LOG_ASSERT("failed get EnemyActionState component. (EnemySearchSubState)");
			return;
		}
		parent_state->RegisterSubState(GetComponent<EnemySearchSubState>(), static_cast<int>(EnemyActionState::ActionState::Search));

	}

	void EnemySearchSubState::Enter()
	{
	}

	void EnemySearchSubState::BeforeExecute()
	{
	}

	void EnemySearchSubState::AfterExecute()
	{
	}

	void EnemySearchSubState::Execute()
	{
	}

	void EnemySearchSubState::Exit()
	{
	}





	/////////////////////////////////////////////////////////////////////////
	// GUI設定
	/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	void EnemySearchSubState::OnGUI()
	{
	}
#endif
}
