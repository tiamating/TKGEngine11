#pragma once


#include "TKGEngine_Defined.h"

#include <string>
#include <mutex>

namespace TKGEngine
{
	class VECTOR2;
	class VECTOR3;
	class VECTOR4;
	class Quaternion;
}

namespace TKGEngine::Time
{
	class ITimeSystem;
}// namespace TKGEngine::Time

namespace TKGEngine::Log
{
	class LogSystem
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static void Create(Time::ITimeSystem* t);
		static void Destroy();
		static LogSystem* GetInstance();

		void PrintLog(const char* fmt, ...);

		void PrintWnd(const char* fmt, ...);

		void Draw();

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================
		LogSystem() = default;
		virtual ~LogSystem() = default;
		LogSystem(const LogSystem&) = delete;
		LogSystem operator=(const LogSystem&) = delete;

		void Clear();

		// ==============================================
		// private variables
		// ==============================================
		static LogSystem* m_instance;

#ifdef USE_IMGUI
		ImGuiTextBuffer m_imgui_log_buf;
		ImGuiTextBuffer m_imgui_print_buf;
		ImFont* m_font;
#endif// USE_IMGUI
		bool m_scroll_to_bottom = false;
		std::mutex m_log_mutex;
		std::mutex m_print_mutex;
	};



}// namespace TKGEngine::Log

#ifdef USE_IMGUI
#define LOG_ASSERT(fmt, ...) \
	if(TKGEngine::Log::LogSystem::GetInstance() != nullptr){\
	TKGEngine::Log::LogSystem::GetInstance()->PrintLog("ASSERT : File(%s) Line(%d)", __FILE__, __LINE__);\
	TKGEngine::Log::LogSystem::GetInstance()->PrintLog(fmt, __VA_ARGS__);}
#else
#define LOG_ASSERT(fmt, ...) ((void)0);
#endif// USE_IMGUI

#ifdef USE_IMGUI
#define LOG_DEBUG(fmt, ...) \
	if(TKGEngine::Log::LogSystem::GetInstance() != nullptr){\
	TKGEngine::Log::LogSystem::GetInstance()->PrintLog(fmt, __VA_ARGS__);}
#else
#define LOG_DEBUG(fmt, ...) ((void)0);
#endif// USE_IMGUI

#ifdef USE_IMGUI
#define PRINTD(fmt, ...) \
	if(TKGEngine::Log::LogSystem::GetInstance() != nullptr){\
	TKGEngine::Log::LogSystem::GetInstance()->PrintWnd(fmt, __VA_ARGS__);}
#else
#define PRINTD(fmt, ...) ((void)0);
#endif// USE_IMGUI