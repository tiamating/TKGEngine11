#pragma once

#include "ICharacterSubState.h"

namespace TKGEngine
{
	/// <summary>
	/// サブステート管理機能を持つステートインターフェース
	/// </summary>
	class ICharacterState
		: public ICharacterSubState
	{
	public:
		ICharacterState(const int state_num)
		{
			// サブステート数分プールを確保
			m_sub_state_pool.resize(state_num);
		}
		virtual ~ICharacterState() = default;
		ICharacterState(const ICharacterState&) = delete;
		ICharacterState& operator=(const ICharacterState&) = delete;
		ICharacterState& operator=(ICharacterState&&) = default;


		// サブステート登録
		void RegisterSubState(const std::shared_ptr<ICharacterSubState>& sub_state_ref, const int sub_state)
		{
			// サブステートを登録
			m_sub_state_pool.at(sub_state) = sub_state_ref;

			// 不正な状態にならないように存在するサブステートを初期ステートとする
			if(m_current_sub_state < 0)
			{
				m_current_sub_state = sub_state;
			}
		}

		// サブステート設定
		void SetSubState(const int new_state)
		{
			// ステート設定
			m_current_sub_state = new_state;
			m_sub_state_pool.at(m_current_sub_state)->Enter();
		}

		// サブステート変更
		// 現状態と同一のサブステートの遷移を許容
		void ChangeSubState(const int next_state)
		{
			m_sub_state_pool.at(m_current_sub_state)->Exit();
			m_current_sub_state = next_state;
			m_sub_state_pool.at(m_current_sub_state)->Enter();
		}

		// サブステート取得
		std::shared_ptr<ICharacterSubState>& GetSubState()
		{
			return m_sub_state_pool.at(m_current_sub_state);
		}
		int GetSubStateIndex() const
		{
			return m_current_sub_state;
		}

	private:
		std::vector<std::shared_ptr<ICharacterSubState>> m_sub_state_pool;
		int m_current_sub_state = -1;
	};
}
