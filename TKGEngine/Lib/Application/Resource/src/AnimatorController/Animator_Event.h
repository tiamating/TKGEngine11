#pragma once

#include "Systems/inc/TKGEngine_Defined.h"

#include <string>

namespace TKGEngine::Animations
{
	/// <summary>
	/// アニメーションの再生中に呼び出される関数の設定
	/// </summary>
	class AnimatorEvent
	{
	public:
		// ==============================================
		// public enum
		// ==============================================
		// イベントの引数の型の種類
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

		// クラス型名
		void SetClassTypeName(const std::string& class_type_name);
		inline const std::string& GetClassTypeName() const;
		// 関数名
		void SetFunctionName(const std::string& func_name);
		inline const std::string& GetFunctionName() const;

		// パラメータ
		void SetParameterType(EventParameterType type);
		// Int
		void SetParameterInt(const int param);
		inline int GetParameterInt() const;
		// Float
		void SetParameterFloat(const float param);
		inline float GetParameterFloat() const;

		// Time(正規化時間)
		void SetEventTime(const float time);
		inline float GetEventTime() const;

		// 実行済みフラグの取得
		inline bool IsExecutedEvent() const;
		// 実行済みフラグのリセット
		inline void ResetExecutedFlag();

		/// <summary>
		/// イベント発火する時間かチェック
		/// </summary>
		/// <param name="normalized_time">正規化経過時間</param>
		/// <returns>実行可能ならtrue</returns>
		bool CheckTime(const float normalized_time) const;

		// イベント実行
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
		// float引数時のパラメータ
		float m_float_parameter = 0.0f;
		// int引数時のパラメータ
		int m_int_parameter = 0;
		// イベント発火する正規化時間(0.0 ~ 1.0)
		float m_time = 0.0f;

		// イベントの引数の型
		EventParameterType m_event_parameter_type = EventParameterType::Void;
		// 現在のループで関数が呼ばれたか
		bool m_is_called = false;

		// クラス名
		std::string m_class_type_name;
		// 関数名
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