#pragma once

#include "../../inc/Graphics_Defined.h"
#include "Systems/inc/TKGEngine_Defined.h"

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace TKGEngine::Graphics
{
	/// <summary>
	/// Graphics profiler interface
	/// </summary>
	class IProfiler
	{
	public:
		static IProfiler* CreateInterface();

#ifdef USE_IMGUI
		// プロファイラのデバッグ表示
		virtual void OnGUI() = 0;
#endif// #ifdef USE_IMGUI

		IProfiler() = default;
		virtual ~IProfiler() = default;
		IProfiler(const IProfiler&) = delete;
		IProfiler& operator=(const IProfiler&) = delete;

		virtual bool Init(ID3D11Device* p_device) = 0;
		virtual void Release() = 0;

		virtual void BeginFrame(ID3D11DeviceContext* p_ic) = 0;
		virtual void TimeStamp(ID3D11DeviceContext* p_ic, TS_TYPE type, int thread_idx) = 0;
		virtual void EndFrame(ID3D11DeviceContext* p_ic) = 0;

		virtual void WaitAndUpdate(ID3D11DeviceContext* p_ic) = 0;
		virtual void WaitForIdle(ID3D11DeviceContext* p_ic) = 0;

		virtual float GetElapsedTime(TS_TYPE type, int thread_idx) = 0;
		virtual float GetAverageTime(TS_TYPE type, int thread_idx) = 0;
	};


}// namespace TKGEngine::Graphics