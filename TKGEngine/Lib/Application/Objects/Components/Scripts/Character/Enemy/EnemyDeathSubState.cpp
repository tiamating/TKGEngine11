#include "EnemyDeathSubState.h"

#include "EnemyReactionState.h"
#include "Components/inc/CAnimator.h"
#include "Components/inc/CRigidBody.h"

REGISTERCOMPONENT(TKGEngine::EnemyDeathSubState);

namespace TKGEngine
{
	void EnemyDeathSubState::Awake()
	{
		// 毎フレーム更新は行わない
		m_can_call_every_frame_update = false;
	}
	
	void EnemyDeathSubState::Start()
	{
		// 親ステートに登録する
		const auto parent_state = GetComponent<EnemyReactionState>();
		m_parent_state = parent_state;
		if (!parent_state)
		{
			LOG_ASSERT("failed get EnemyReactionState component. (EnemyDeathSubState)");
			return;
		}
		parent_state->RegisterSubState(GetComponent<EnemyDeathSubState>(), static_cast<int>(EnemyReactionState::ReactionState::Death));

		// コンポーネントの取得
		// Animator
		m_animator = GetComponent<Animator>();
		// RigidBody
		m_rigidbody = GetComponent<RigidBody>();
	}

	void EnemyDeathSubState::Enter()
	{
		// モーションを遷移
		if(const auto animator = m_animator.GetWeak().lock())
		{
			animator->SetTrigger("OnDeath");
		}
		// 衝突をなくす
		if(const auto rigidbody = m_rigidbody.GetWeak().lock())
		{
			rigidbody->Enabled(false);
		}
	}

	void EnemyDeathSubState::BeforeExecute()
	{
	}

	void EnemyDeathSubState::AfterExecute()
	{
	}

	void EnemyDeathSubState::Execute()
	{
	}

	void EnemyDeathSubState::Exit()
	{
	}
	

	/////////////////////////////////////////////////////////////////////////
	// GUI設定
	/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	void EnemyDeathSubState::OnGUI()
	{
	}
#endif
}
