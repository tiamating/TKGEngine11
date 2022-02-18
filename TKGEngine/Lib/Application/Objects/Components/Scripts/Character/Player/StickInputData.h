#pragma once

#include "Utility/inc/myfunc_vector.h"

namespace TKGEngine
{
	/// <summary>
	/// �p�b�h�̃X�e�B�b�N���̓f�[�^�p�\����
	/// </summary>
	struct StickInputData
	{
	public:
		StickInputData& operator=(const StickInputData& ref) = default;
		StickInputData& operator=(StickInputData&& ref) = default;


		[[nodiscard]] const VECTOR2& GetInput() const { return m_input; }
		[[nodiscard]] const VECTOR2& GetDirection() const { return m_direction; }
		[[nodiscard]] float GetLength() const { return m_length; }

		void SetInput(const VECTOR2& _input)
		{
			// ���͒l��ێ�����
			m_input_buffers[1 - m_current_buffer_idx] = _input;

			// �u�ԓI��0���͂��Ȃ������߂ɁA���ς����
			m_input = (m_input_buffers[0] + m_input_buffers[1]) * 0.5f;
			m_length = m_input.Length();
			// �f�b�h�]�[������
			if(m_length < m_dead_zone)
			{
				m_length = 0.0f;
				m_direction = VECTOR2::Zero;
			}
			else
			{
				m_direction = (m_input_buffers[0] + m_input_buffers[1]).Normalized();
			}

			// ���݂̃C���f�b�N�X��ύX����
			m_current_buffer_idx = 1 - m_current_buffer_idx;
		}

	private:
		static constexpr float m_dead_zone = 0.09f;

		VECTOR2 m_input = VECTOR2::Zero;
		VECTOR2 m_direction = VECTOR2::Zero;
		float m_length = 0.0f;

		// �u�ԓI��0���͂��Ȃ������߂ɁA2�t���[���̕��ς�����ē��͒l�Ƃ���悤�ɂ���
		VECTOR2 m_input_buffers[2] = { VECTOR2::Zero, VECTOR2::Zero };
		int m_current_buffer_idx = 0;
	};
}
