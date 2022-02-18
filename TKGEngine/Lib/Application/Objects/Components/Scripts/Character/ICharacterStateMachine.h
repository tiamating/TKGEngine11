#pragma once

#include "ICharacterState.h"

namespace TKGEngine
{
	/// <summary>
	/// ステート管理インターフェース
	/// </summary>
	class ICharacterStateMachine
	{
	public:
		ICharacterStateMachine(const int state_num)
		{
			// ステート数分プールを確保
			m_state_pool.resize(state_num);
		}
		virtual ~ICharacterStateMachine() = default;
		ICharacterStateMachine(const ICharacterStateMachine&) = delete;
		ICharacterStateMachine& operator=(const ICharacterStateMachine&) = delete;
		ICharacterStateMachine& operator=(ICharacterStateMachine&&) = default;


		// ステート登録
		void RegisterState(const std::shared_ptr<ICharacterState>& state_ref, const int state)
		{
			// ステートを登録
			m_state_pool.at(state) = state_ref;

			// 不正な状態にならないように存在するステートを初期ステートとする
			if (m_current_state < 0)
			{
				m_current_state = state;
			}
		}

		// ステート設定
		void SetState(const int new_state)
		{
			// ステート設定
			m_current_state = new_state;
			m_state_pool.at(m_current_state)->Enter();
		}

		// ステート変更
		void ChangeState(const int next_state)
		{
			m_state_pool.at(m_current_state)->GetSubState()->Exit();
			m_state_pool.at(m_current_state)->Exit();
			m_current_state = next_state;
			m_state_pool.at(m_current_state)->Enter();
		}

		// ステート取得
		std::shared_ptr<ICharacterState>& GetState()
		{
			return m_state_pool.at(m_current_state);
		}
		int GetStateIndex() const
		{
			return m_current_state;
		}

	protected:
		// ステート管理プール
		std::vector<std::shared_ptr<ICharacterState>> m_state_pool;
		// 現状態
		int m_current_state = -1;
	};
}
