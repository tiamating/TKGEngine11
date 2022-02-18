#pragma once

#include "ICharacterState.h"

namespace TKGEngine
{
	/// <summary>
	/// �X�e�[�g�Ǘ��C���^�[�t�F�[�X
	/// </summary>
	class ICharacterStateMachine
	{
	public:
		ICharacterStateMachine(const int state_num)
		{
			// �X�e�[�g�����v�[�����m��
			m_state_pool.resize(state_num);
		}
		virtual ~ICharacterStateMachine() = default;
		ICharacterStateMachine(const ICharacterStateMachine&) = delete;
		ICharacterStateMachine& operator=(const ICharacterStateMachine&) = delete;
		ICharacterStateMachine& operator=(ICharacterStateMachine&&) = default;


		// �X�e�[�g�o�^
		void RegisterState(const std::shared_ptr<ICharacterState>& state_ref, const int state)
		{
			// �X�e�[�g��o�^
			m_state_pool.at(state) = state_ref;

			// �s���ȏ�ԂɂȂ�Ȃ��悤�ɑ��݂���X�e�[�g�������X�e�[�g�Ƃ���
			if (m_current_state < 0)
			{
				m_current_state = state;
			}
		}

		// �X�e�[�g�ݒ�
		void SetState(const int new_state)
		{
			// �X�e�[�g�ݒ�
			m_current_state = new_state;
			m_state_pool.at(m_current_state)->Enter();
		}

		// �X�e�[�g�ύX
		void ChangeState(const int next_state)
		{
			m_state_pool.at(m_current_state)->GetSubState()->Exit();
			m_state_pool.at(m_current_state)->Exit();
			m_current_state = next_state;
			m_state_pool.at(m_current_state)->Enter();
		}

		// �X�e�[�g�擾
		std::shared_ptr<ICharacterState>& GetState()
		{
			return m_state_pool.at(m_current_state);
		}
		int GetStateIndex() const
		{
			return m_current_state;
		}

	protected:
		// �X�e�[�g�Ǘ��v�[��
		std::vector<std::shared_ptr<ICharacterState>> m_state_pool;
		// �����
		int m_current_state = -1;
	};
}
