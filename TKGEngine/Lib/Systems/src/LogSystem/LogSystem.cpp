
#include "../../inc/LogSystem.h"

#include "../../inc/ITimeSystem.h"
#include "../../inc/IWindow.h"

#include "Utility/inc/myfunc_math.h"
#include "Utility/inc/myfunc_vector.h"

#include <cassert>

namespace TKGEngine::Log
{
	////////////////////////////////////////////////////////
	// Internal linkage Global Class
	////////////////////////////////////////////////////////
#pragma region Global
	namespace /* anonymous */
	{
		class LogGlobal
		{
		public:
			void Set(Time::ITimeSystem* t);
			void Reset();

			Time::ITimeSystem* Time() const;

		private:
			Time::ITimeSystem* p_time = nullptr;

		};

		void LogGlobal::Set(Time::ITimeSystem* t)
		{
			p_time = t;
		}
		void LogGlobal::Reset()
		{
			p_time = nullptr;

		}
		Time::ITimeSystem* LogGlobal::Time() const
		{
			return p_time;
		}

		LogGlobal g_systems;

		const char* g_new_line_str = "\n";
		const char* g_timer_str = "[Time:%5.2fsec] ";
	}	// /* anonymous */
#pragma endregion

	////////////////////////////////////////////////////////
	// Static definition
	////////////////////////////////////////////////////////
	LogSystem* LogSystem::m_instance;


	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	void LogSystem::Create(Time::ITimeSystem* t)
	{
		if (m_instance == nullptr)
		{
			g_systems.Set(t);
			m_instance = new LogSystem();
		}

#ifdef USE_IMGUI
		const ImGuiIO& io = ImGui::GetIO();
		m_instance->m_font = io.Fonts->AddFontFromFileTTF(
			"./Asset/Fonts/corbelz.ttf", 18.0f);
		assert(m_instance->m_font != nullptr);
#endif// USE_IMGUI
	}

	void LogSystem::Destroy()
	{
		if (m_instance != nullptr)
		{
			delete m_instance;
			m_instance = nullptr;
		}
	}

	LogSystem* LogSystem::GetInstance()
	{
		return m_instance;
	}

	void LogSystem::PrintLog(const char* fmt, ...)
	{
#ifdef USE_IMGUI
		std::lock_guard<std::mutex> lock(m_log_mutex);
		m_imgui_log_buf.appendf(g_timer_str, ImGui::GetTime());
		va_list args;
		va_start(args, fmt);
		m_imgui_log_buf.appendfv(fmt, args);
		va_end(args);
		m_imgui_log_buf.append(g_new_line_str);
		m_scroll_to_bottom = true;
#endif// USE_IMGUI
	}

	void LogSystem::PrintWnd(const char* fmt, ...)
	{
#ifdef USE_IMGUI
		std::lock_guard<std::mutex> lock(m_print_mutex);
		va_list args;
		va_start(args, fmt);
		m_imgui_print_buf.appendfv(fmt, args);
		va_end(args);
		m_imgui_print_buf.append(g_new_line_str);
#endif// USE_IMGUI
	}

	void LogSystem::Draw()
	{
#ifdef USE_IMGUI
		// Draw Log window
		{
			std::lock_guard<std::mutex> lock(m_log_mutex);
			ImGui::Begin("Log");
			if (ImGui::Button("Clear"))
			{
				Clear();
			}
			ImGui::SameLine();
			ImGui::Text("FPS:%u", g_systems.Time()->GetFPS());
			ImGui::Separator();
			ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
			ImGui::TextUnformatted(m_imgui_log_buf.begin());
			if (m_scroll_to_bottom)
			{
				ImGui::SetScrollHereY(1.0f);// 0.0 ~ 1.0でスクロールバーの初期位置を決める
			}
			m_scroll_to_bottom = false;
			ImGui::EndChild();
			ImGui::End();
		}

		// Draw On window
		{
			std::lock_guard<std::mutex> lock(m_log_mutex);

			ImVec2 wnd_size = ImVec2(500.0f, 0.0f);
			ImVec2 wnd_pos(0.f, 0.f);
			const RECT rect = IWindow::Get().GetScreenRect();
			const float wnd_width = static_cast<float>(rect.right - rect.left);
			{
				constexpr float WINDOW_TITLE_BAR_SIZE = 55.0f;
				constexpr float REMAIN_SPACE = 10.0f;
				constexpr float TRANSFORM_BUTTON_SPACE = 30.0f;

				wnd_size.x = wnd_size.x > wnd_width ? wnd_width : wnd_size.x;
				wnd_size.y = static_cast<float>(rect.bottom - rect.top) - WINDOW_TITLE_BAR_SIZE - REMAIN_SPACE - TRANSFORM_BUTTON_SPACE;
				wnd_pos.x = static_cast<float>(rect.left) + REMAIN_SPACE;
				wnd_pos.y = static_cast<float>(rect.top) + WINDOW_TITLE_BAR_SIZE + TRANSFORM_BUTTON_SPACE;
			}

			ImGui::PushFont(m_font);
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::SetNextWindowPos(wnd_pos, ImGuiCond_Always);
			ImGui::SetNextWindowSize(wnd_size, ImGuiCond_Always);
			ImGui::Begin("Debug print on window", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoInputs);
			{
				ImGui::TextUnformatted(m_imgui_print_buf.begin());
			}
			ImGui::End();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopFont();

			m_imgui_print_buf.clear();
		}
#endif// USE_IMGUI
	}

	void LogSystem::Clear()
	{
#ifdef USE_IMGUI
		m_imgui_log_buf.clear();
#endif// USE_IMGUI
	}

}// namespace TKGEngine::Log