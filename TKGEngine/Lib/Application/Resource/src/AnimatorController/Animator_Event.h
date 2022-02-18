#pragma once

#include "Systems/inc/TKGEngine_Defined.h"

#include <string>

namespace TKGEngine::Animations
{
	/// <summary>
	/// �A�j���[�V�����̍Đ����ɌĂяo�����֐��̐ݒ�
	/// </summary>
	class AnimatorEvent
	{
	public:
		// ==============================================
		// public enum
		// ==============================================
		// �C�x���g�̈����̌^�̎��
		enum class EventParameterType
		{
			Void,
			Int,
			Float
		};

		// ==============================================
		// public methods
		// ==============================================
		AnimatorEvent();
		AnimatorEvent(const std::string& class_type_name, const std::string& func_name);
		AnimatorEvent(const std::string& class_type_name, const std::string& func_name, const float normalize_time);
		AnimatorEvent(const std::string& class_type_name, const std::string& func_name, const float normalize_time, int parameter);
		AnimatorEvent(const std::string& class_type_name, const std::string& func_name, const float normalize_time, float parameter);
		virtual ~AnimatorEvent() = default;
		AnimatorEvent(const AnimatorEvent& _event);
		AnimatorEvent& operator=(const AnimatorEvent& _event);

#ifdef USE_IMGUI
		void OnGUI(const GameObjectID goid);
#endif// #ifdef USE_IMGUI

		// �N���X�^��
		void SetClassTypeName(const std::string& class_type_name);
		inline const std::string& GetClassTypeName() const;
		// �֐���
		void SetFunctionName(const std::string& func_name);
		inline const std::string& GetFunctionName() const;

		// �p�����[�^
		void SetParameterType(EventParameterType type);
		// Int
		void SetParameterInt(const int param);
		inline int GetParameterInt() const;
		// Float
		void SetParameterFloat(const float param);
		inline float GetParameterFloat() const;

		// Time(���K������)
		void SetEventTime(const float time);
		inline float GetEventTime() const;

		// ���s�ς݃t���O�̎擾
		inline bool IsExecutedEvent() const;
		// ���s�ς݃t���O�̃��Z�b�g
		inline void ResetExecutedFlag();

		/// <summary>
		/// �C�x���g���΂��鎞�Ԃ��`�F�b�N
		/// </summary>
		/// <param name="normalized_time">���K���o�ߎ���</param>
		/// <returns>���s�\�Ȃ�true</returns>
		bool CheckTime(const float normalized_time) const;

		// �C�x���g���s
		void ExecuteEvent(GameObjectID goid);


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(m_float_parameter),
					CEREAL_NVP(m_int_parameter),
					CEREAL_NVP(m_time),
					CEREAL_NVP(m_event_parameter_type),
					CEREAL_NVP(m_class_type_name),
					CEREAL_NVP(m_function_name)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
		/* nothing */


		// ==============================================
		// private variables
		// ==============================================
		// float�������̃p�����[�^
		float m_float_parameter = 0.0f;
		// int�������̃p�����[�^
		int m_int_parameter = 0;
		// �C�x���g���΂��鐳�K������(0.0 ~ 1.0)
		float m_time = 0.0f;

		// �C�x���g�̈����̌^
		EventParameterType m_event_parameter_type = EventParameterType::Void;
		// ���݂̃��[�v�Ŋ֐����Ă΂ꂽ��
		bool m_is_called = false;

		// �N���X��
		std::string m_class_type_name;
		// �֐���
		std::string m_function_name;
	};

	// -----------------------------------
	// inline
	// -----------------------------------
	const std::string& AnimatorEvent::GetClassTypeName() const
	{
		return m_class_type_name;
	}

	inline const std::string& AnimatorEvent::GetFunctionName() const
	{
		return m_function_name;
	}

	inline int AnimatorEvent::GetParameterInt() const
	{
		return m_int_parameter;
	}

	inline float AnimatorEvent::GetParameterFloat() const
	{
		return m_float_parameter;
	}

	inline float AnimatorEvent::GetEventTime() const
	{
		return m_time;
	}

	inline bool AnimatorEvent::IsExecutedEvent() const
	{
		return m_is_called;
	}

	inline void AnimatorEvent::ResetExecutedFlag()
	{
		m_is_called = false;
	}


}// namespace TKGEngine::Animations

CEREAL_CLASS_VERSION(TKGEngine::Animations::AnimatorEvent, 1)