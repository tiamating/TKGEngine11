#pragma once

#include "ICharacterSubState.h"

namespace TKGEngine
{
	/// <summary>
	/// �T�u�X�e�[�g�Ǘ��@�\�����X�e�[�g�C���^�[�t�F�[�X
	/// </summary>
	class ICharacterState
		: public ICharacterSubState
	{
	public:
		ICharacterState(const int state_num)
		{
			// �T�u�X�e�[�g�����v�[�����m��
			m_sub_state_pool.resize(state_num);
		}
		virtual ~ICharacterState() = default;
		ICharacterState(const ICharacterState&) = delete;
		ICharacterState& operator=(const ICharacterState&) = delete;
		ICharacterState& operator=(ICharacterState&&) = default;


		// �T�u�X�e�[�g�o�^
		void RegisterSubState(const std::shared_ptr<ICharacterSubState>& sub_state_ref, const int sub_state)
		{
			// �T�u�X�e�[�g��o�^
			m_sub_state_pool.at(sub_state) = sub_state_ref;

			// �s���ȏ�ԂɂȂ�Ȃ��悤�ɑ��݂���T�u�X�e�[�g�������X�e�[�g�Ƃ���
			if(m_current_sub_state < 0)
			{
				m_current_sub_state = sub_state;
			}
		}

		// �T�u�X�e�[�g�ݒ�
		void SetSubState(const int new_state)
		{
			// �X�e�[�g�ݒ�
			m_current_sub_state = new_state;
			m_sub_state_pool.at(m_current_sub_state)->Enter();
		}

		// �T�u�X�e�[�g�ύX
		// ����ԂƓ���̃T�u�X�e�[�g�̑J�ڂ����e
		void ChangeSubState(const int next_state)
		{
			m_sub_state_pool.at(m_current_sub_state)->Exit();
			m_current_sub_state = next_state;
			m_sub_state_pool.at(m_current_sub_state)->Enter();
		}

		// �T�u�X�e�[�g�擾
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
