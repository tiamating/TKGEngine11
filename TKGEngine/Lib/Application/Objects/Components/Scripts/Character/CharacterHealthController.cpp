#include "CharacterHealthController.h"

#include "Player/PlayerStateMachine.h"
#include "Enemy/EnemyStateMachine.h"

REGISTERCOMPONENT(TKGEngine::CharacterHealthController);

namespace TKGEngine
{
	/////////////////////////////////////////////////////////////////////////
	// 実体生成時に呼び出される
	/////////////////////////////////////////////////////////////////////////
	void CharacterHealthController::Awake()
	{
		// 初期は最大体力
		m_health = m_max_health;
	}

	/////////////////////////////////////////////////////////////////////////
	// 毎フレーム呼び出される
	/////////////////////////////////////////////////////////////////////////
	void CharacterHealthController::Update()
	{
		// 無敵時間のカウントダウン
		if (m_is_invincible_on_damage)
		{
			m_invincible_timer -= ITime::Get().DeltaTime();
			if (m_invincible_timer <= 0.0f)
			{
				// 無敵終了
				m_is_invincible_on_damage = false;
			}
		}
	}


	bool CharacterHealthController::ApplyDamage(const int damage)
	{
		// ダメージが0
		if (damage == 0)
		{
			return false;
		}

		// 既に死んでいる
		if (m_health <= 0)
		{
			return false;
		}
		// 無敵中
		if (m_is_invincible_state || m_is_invincible_on_damage)
		{
			return false;
		}

		//// ダメージ処理
		// 被弾後は無敵にする
		m_is_invincible_on_damage = true;
		m_health -= damage;
		m_health = MyMath::Max(0, m_health);

		//// ReactionStateに遷移
		// Player
		if (GetGameObject()->GetLayer() == Layer::Player)
		{
			if (const auto sm = GetComponent<PlayerStateMachine>())
			{
				sm->SetState(static_cast<int>(PlayerStateMachine::PlayerState::Reaction));
			}
		}
		// Enemy
		else
		{
			if (const auto sm = GetComponent<EnemyStateMachine>())
			{
				sm->SetState(static_cast<int>(EnemyStateMachine::EnemyState::Reaction));
			}
		}
		
		return true;
	}



	/////////////////////////////////////////////////////////////////////////
	// GUI設定
	/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	void CharacterHealthController::OnGUI()
	{
		// Max Health
		ImGui::Text("Max Health");
		ImGui::AlignedSameLine(0.5f);
		ImGui::InputInt("##Max Health", &m_max_health);
		// Current Health
		ImGui::Text("Current Health");
		ImGui::AlignedSameLine(0.5f);
		ImGui::InputInt("##Current Health", &m_health);

		// Invincible
		ImGui::Text("Invincible");
		{
			ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
			// Current
			ImGui::Text("State");
			ImGui::AlignedSameLine(0.5f);
			bool current_state = m_is_invincible_on_damage;
			ImGui::Checkbox("##State", &current_state);
			// Time
			ImGui::Text("Time");
			ImGui::AlignedSameLine(0.5f);
			ImGui::DragFloat("##Invincible Time", &m_invincible_time, 0.02f, 0.0f, FLT_MAX);
		}
	}
#endif
}
