#pragma once

#include "Utility/inc/myfunc_vector.h"

namespace TKGEngine
{
	/// <summary>
	/// パッドのスティック入力データ用構造体
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
			// 入力値を保持する
			m_input_buffers[1 - m_current_buffer_idx] = _input;

			// 瞬間的な0入力をなくすために、平均を取る
			m_input = (m_input_buffers[0] + m_input_buffers[1]) * 0.5f;
			m_length = m_input.Length();
			// デッドゾーン判定
			if(m_length < m_dead_zone)
			{
				m_length = 0.0f;
				m_direction = VECTOR2::Zero;
			}
			else
			{
				m_direction = (m_input_buffers[0] + m_input_buffers[1]).Normalized();
			}

			// 現在のインデックスを変更する
			m_current_buffer_idx = 1 - m_current_buffer_idx;
		}

	private:
		static constexpr float m_dead_zone = 0.09f;

		VECTOR2 m_input = VECTOR2::Zero;
		VECTOR2 m_direction = VECTOR2::Zero;
		float m_length = 0.0f;

		// 瞬間的な0入力をなくすために、2フレームの平均を取って入力値とするようにする
		VECTOR2 m_input_buffers[2] = { VECTOR2::Zero, VECTOR2::Zero };
		int m_current_buffer_idx = 0;
	};
}
