#pragma once

#include "ISystem.h"

#include "TKGEngine_Defined.h"

#include <memory>

namespace TKGEngine
{
	struct FrameEventArgs;
	struct ResizeEventArgs;
}// namespace TKGEngine

namespace TKGEngine::Time
{
	/// ========================================================
	/// @class	ITimeSystem
	/// @brief	Time system interface
	/// 
	/// ========================================================
	class ITimeSystem
		: public ISystem
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static std::unique_ptr<ITimeSystem> CreateInterface();

		virtual ~ITimeSystem() = default;

		virtual bool OnInit() = 0;
		virtual void OnTerm() = 0;
		virtual void OnFrameBegin(const FrameEventArgs& args) = 0;
		virtual void OnFrameRender(const FrameEventArgs& args) = 0;
		virtual void OnFrameEnd(const FrameEventArgs& args) = 0;
		virtual void OnResize(const ResizeEventArgs& args) = 0;

		virtual inline void Reset() = 0;
		virtual inline void Start() = 0;
		virtual inline void Stop() = 0;
		virtual inline bool Tick() = 0;
		virtual inline void Advance(float ad_time) = 0;
		virtual inline bool IsStopped() const = 0;
		virtual inline void IsVariableFrameRate(bool is_variable) = 0;
		virtual inline void SetFrameRate(float frame_rate) = 0;
		virtual inline void CalcFramePerSec() = 0;	//!< frame count per 0.5sec. Call every frame
		virtual inline unsigned GetFPS() const = 0;
	};
}	// namespace TKGEngine::Time