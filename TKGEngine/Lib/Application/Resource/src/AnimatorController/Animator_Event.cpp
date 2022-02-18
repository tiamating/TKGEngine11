
#include "Animator_Event.h"

#include "Utility/inc/myfunc_math.h"
#include "Managers/MonoBehaviourManager.h"
#include "Managers/ComponentManager.h"

namespace TKGEngine::Animations
{
	AnimatorEvent::AnimatorEvent()
	{
		/* nothing */
	}

	AnimatorEvent::AnimatorEvent(const std::string& class_type_name, const std::string& func_name)
	{
		SetClassTypeName(class_type_name);
		SetFunctionName(func_name);
	}
	AnimatorEvent::AnimatorEvent(const std::string& class_type_name, const std::string& func_name, const float normalize_time)
		: AnimatorEvent(class_type_name, func_name)
	{
		SetEventTime(normalize_time);
	}
	AnimatorEvent::AnimatorEvent(const std::string& class_type_name, const std::string& func_name, const float normalize_time, const int parameter)
		: AnimatorEvent(class_type_name, func_name, normalize_time)
	{
		SetParameterType(EventParameterType::Int);
		SetParameterInt(parameter);
	}
	AnimatorEvent::AnimatorEvent(const std::string& class_type_name, const std::string& func_name, const float normalize_time, const float parameter)
		: AnimatorEvent(class_type_name, func_name, normalize_time)
	{
		SetParameterType(EventParameterType::Float);
		SetParameterFloat(parameter);
	}

	AnimatorEvent::AnimatorEvent(const AnimatorEvent& _event)
	{
		m_class_type_name = _event.m_class_type_name;
		m_function_name = _event.m_function_name;
		m_time = _event.m_time;
		m_event_parameter_type = _event.m_event_parameter_type;
		m_float_parameter = _event.m_float_parameter;
		m_int_parameter = _event.m_int_parameter;
	}

	AnimatorEvent& AnimatorEvent::operator=(const AnimatorEvent& _event)
	{
		m_class_type_name = _event.m_class_type_name;
		m_function_name = _event.m_function_name;
		m_time = _event.m_time;
		m_event_parameter_type = _event.m_event_parameter_type;
		m_float_parameter = _event.m_float_parameter;
		m_int_parameter = _event.m_int_parameter;
		return *this;
	}

#ifdef USE_IMGUI
	void AnimatorEvent::OnGUI(const GameObjectID goid)
	{
		ImGui::IDWrapped gui_id(this);
		// Time設定
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
		ImGui::DragFloat("##Event Time", &m_time, 0.002f, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
		// Class名選択
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
		if (ImGui::BeginCombo("##Select Event Class", m_class_type_name.c_str()))
		{
			// GameObjectにアタッチされたComponent名リストを取得する
			const auto& comp_names = ComponentManager::GetComponentNameList(goid);
			const auto itr_end = comp_names.end();
			for (auto itr = comp_names.begin(); itr != itr_end; ++itr)
			{
				if (ImGui::Selectable(itr->c_str(), (*itr) == m_class_type_name ? true : false))
				{
					m_class_type_name = *itr;
					m_function_name.clear();
				}
			}
			ImGui::EndCombo();
		}
		// Func名選択
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.2f);
		if (ImGui::BeginCombo("##Select Event Func", m_function_name.c_str()))
		{
			// Void
			if (ImGui::BeginMenu("Void##Param"))
			{
				std::list<std::string> func_names;
				MonoBehaviourManager::GetFunctionVoidNames(m_class_type_name, func_names);
				for (auto& func_name : func_names)
				{
					// 選択された関数名とパラメータの型をセットする
					if (ImGui::Selectable(func_name.c_str(), func_name == m_function_name ? true : false))
					{
						m_function_name = func_name;
						m_event_parameter_type = EventParameterType::Void;
					}
				}
				ImGui::EndMenu();
			}
			// Int
			if (ImGui::BeginMenu("Int##Param"))
			{
				std::list<std::string> func_names;
				MonoBehaviourManager::GetFunctionIntNames(m_class_type_name, func_names);
				for (auto& func_name : func_names)
				{
					// 選択された関数名とパラメータの型をセットする
					if (ImGui::Selectable(func_name.c_str(), func_name == m_function_name ? true : false))
					{
						m_function_name = func_name;
						m_event_parameter_type = EventParameterType::Int;
					}
				}
				ImGui::EndMenu();
			}
			// Float
			if (ImGui::BeginMenu("Float##Param"))
			{
				std::list<std::string> func_names;
				MonoBehaviourManager::GetFunctionFloatNames(m_class_type_name, func_names);
				for (auto& func_name : func_names)
				{
					// 選択された関数名とパラメータの型をセットする
					if (ImGui::Selectable(func_name.c_str(), func_name == m_function_name ? true : false))
					{
						m_function_name = func_name;
						m_event_parameter_type = EventParameterType::Float;
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndCombo();
		}
		// Parameter設定
		if (m_event_parameter_type != EventParameterType::Void)
		{
			ImGui::SameLine();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
			switch (m_event_parameter_type)
			{
				case EventParameterType::Int:
				{
					ImGui::DragInt("##Int Param", &m_int_parameter, 0.02f, 0, 0, "Int:%d");
				}
				break;
				case EventParameterType::Float:
				{
					ImGui::DragFloat("##Float Param", &m_float_parameter, 0.002f, 0.0f, 0.0f, "Flt:%.3f");
				}
				break;
			}
		}
	}
#endif// #ifdef USE_IMGUI

	void AnimatorEvent::SetClassTypeName(const std::string& class_type_name)
	{
		m_class_type_name = class_type_name;
	}

	void AnimatorEvent::SetFunctionName(const std::string& func_name)
	{
		m_function_name = func_name;
	}

	void AnimatorEvent::SetParameterType(const EventParameterType type)
	{
		m_event_parameter_type = type;
	}

	void AnimatorEvent::SetParameterInt(const int param)
	{
		m_int_parameter = param;
	}

	void AnimatorEvent::SetParameterFloat(const float param)
	{
		m_float_parameter = param;
	}

	void AnimatorEvent::SetEventTime(const float time)
	{
		m_time = MyMath::Clamp(time, 0.0f, 1.0f);
	}

	bool AnimatorEvent::CheckTime(const float normalized_time) const
	{
		if (m_time <= normalized_time)
			return true;
		return false;
	}

	void AnimatorEvent::ExecuteEvent(GameObjectID goid)
	{
		// 対応するパラメータを渡して、関数を実行
		switch (m_event_parameter_type)
		{
			case AnimatorEvent::EventParameterType::Void:
				MonoBehaviourManager::ExecuteFunction(goid, m_class_type_name, m_function_name);
				break;
			case AnimatorEvent::EventParameterType::Int:
				MonoBehaviourManager::ExecuteFunction(goid, m_class_type_name, m_function_name, m_int_parameter);
				break;
			case AnimatorEvent::EventParameterType::Float:
				MonoBehaviourManager::ExecuteFunction(goid, m_class_type_name, m_function_name, m_float_parameter);
				break;
		}
		// 実行済みフラグを立てる
		m_is_called = true;
	}


}// namespace TKGEngine::Animations

