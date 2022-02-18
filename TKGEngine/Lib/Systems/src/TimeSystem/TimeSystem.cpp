
#include "../../inc/ITimeSystem.h"
#include "../../inc/ITime.h"

#include "Utility/inc/myfunc_math.h"

#include <Windows.h>

namespace TKGEngine::Time
{
	/// ========================================================
	/// @class	TimeSystem
	/// @brief	Time system
	/// 
	/// ========================================================
	class TimeSystem
		: public ITimeSystem,
		public ITime
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		TimeSystem();
		virtual ~TimeSystem() = default;
		TimeSystem(TimeSystem&&) = default;
		TimeSystem(const TimeSystem&) = delete;
		TimeSystem& operator=(const TimeSystem&) = delete;

		bool OnInit() override;
		void OnTerm() override;
		void OnFrameBegin(const FrameEventArgs& args) override;
		void OnFrameRender(const FrameEventArgs& args) override;
		void OnFrameEnd(const FrameEventArgs& args) override;
		void OnResize(const ResizeEventArgs& args) override;

		inline void Reset() override;
		inline void Start() override;
		inline void Stop() override;
		inline bool Tick() override;
		inline void Advance(float ad_time) override;
		inline bool IsStopped() const override;
		inline void IsVariableFrameRate(bool is_variable) override;
		inline void SetFrameRate(float frame_rate) override;
		inline void CalcFramePerSec() override;	//!< frame count per 0.5sec. Call every frame
		inline unsigned GetFPS() const override;


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */

	private:
		// ==============================================
		// private methods
		// ==============================================
		inline void TimeScale(float scale) override;
		inline float TimeScale() const override;
		inline double TotalTime() const override;
		inline double UnscaledTotalTime() const override;
		inline float DeltaTime() const override;
		inline float UnscaledDeltaTime() const override;

		inline LONGLONG GetAdjustedThisTime();

		// ==============================================
		// private variables
		// ==============================================
		double seconds_per_count = 0.0;	//!< timer counting cycle[s]
		LONGLONG counts_per_sec = 0;
		// Fixed frame variable
		LONGLONG frames_per_sec = 60;
		// Count variables
		LONGLONG awake_count = 0;	//!< count when start application
		LONGLONG this_count = 0;	//!< current count
		LONGLONG last_count = 0;	//!< one previous count
		LONGLONG paused_count = 0;	//!< The count that had stopped the timer
		LONGLONG stop_count = 0;	//!< The count when you stop the timer

		double time_scale = 1.0;	//!< time scale

		double total_time = 0.0;			//!< total time from start to this frame
		double unscaled_total_time = 0.0;	//!< this time is not effected by time scale
		double delta_time = -1.0;			//!< frame interval time
		double unscaled_delta_time = -1.0;	//!< this timee is not effected by time scale

		bool is_variable_frame_rate = true;	//!< It is true, if variable frame rate
		bool is_stopped = false;

		double m_time_tlapsed = 0.0;
		unsigned m_frames = 0;
		unsigned m_fps = 0;
	};
	////////////////////////////////////////////////////////////////////
	//
	// inline
	//
	////////////////////////////////////////////////////////////////////
	// class TimeSystem
	std::unique_ptr<ITimeSystem> ITimeSystem::CreateInterface()
	{
		return std::make_unique<TimeSystem>();
	}

	TimeSystem::TimeSystem()
	{
		ITime::Init(this);
	}

	inline bool TimeSystem::OnInit()
	{
		DEVMODE dev = {};
		if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dev) == 0)
		{
			assert(0 && "failed get display settings TimeSystem::OnInit()");
			return false;
		}

		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&counts_per_sec));	// Hz
		seconds_per_count = 1.0 / static_cast<double>(counts_per_sec);
		frames_per_sec = static_cast<LONGLONG>(counts_per_sec / dev.dmDisplayFrequency);

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&this_count));
		awake_count = this_count;
		last_count = this_count;

		return true;
	}

	inline void TimeSystem::OnTerm()
	{
		/* nothing */
	}

	inline void TimeSystem::OnFrameBegin(const FrameEventArgs& args)
	{
		/* nothing */
	}

	inline void TimeSystem::OnFrameRender(const FrameEventArgs& args)
	{
		/* nothing */
	}

	inline void TimeSystem::OnFrameEnd(const FrameEventArgs& args)
	{
		/* nothing */
	}

	inline void TimeSystem::OnResize(const ResizeEventArgs& args)
	{
		/* nothing */
	}

	inline void TimeSystem::Reset()
	{
		this_count = GetAdjustedThisTime();
		awake_count = this_count;
		last_count = this_count;
		stop_count = 0;
		is_stopped = false;
	}

	inline void TimeSystem::Start()
	{
		LARGE_INTEGER now_count = {};
		QueryPerformanceCounter(&now_count);

		if (is_stopped)
		{
			paused_count += (now_count.QuadPart - stop_count);
		}
		last_count = now_count.QuadPart;
		stop_count = 0;
		is_stopped = false;
	}

	inline void TimeSystem::Stop()
	{
		if (!is_stopped)
		{
			QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&stop_count));
			is_stopped = true;
		}
	}

	inline bool TimeSystem::Tick()
	{
		if (is_stopped)
		{
			delta_time = 0.0;
			unscaled_delta_time = 0.0;
			return true;
		}

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&this_count));

		if (!is_variable_frame_rate)
		{
			if (this_count < last_count + frames_per_sec)
			{
				return false;
			}
		}
		// Time difference between this frame and the previous
		const LONGLONG subtract_delta_count = this_count - last_count;
		unscaled_delta_time = subtract_delta_count * seconds_per_count;
		delta_time = unscaled_delta_time * time_scale;

		unscaled_total_time += unscaled_delta_time;
		total_time += delta_time;

		// Prepare for next frame
		last_count = this_count;

		if (unscaled_delta_time < 0.0)
		{
			unscaled_delta_time = 0.0;
			delta_time = 0.0;
		}
		return true;
	}

	inline void TimeSystem::Advance(const float ad_time)
	{
		stop_count += static_cast<LONGLONG>(counts_per_sec * static_cast<double>(ad_time));
	}

	inline bool TimeSystem::IsStopped() const
	{
		return is_stopped;
	}

	inline void TimeSystem::TimeScale(const float scale)
	{
		time_scale = static_cast<double>(MyMath::Max(0.0f, scale));
	}

	inline float TimeSystem::TimeScale() const
	{
		return static_cast<float>(time_scale);
	}

	inline double TimeSystem::TotalTime() const
	{
		return total_time;
	}

	inline double TimeSystem::UnscaledTotalTime() const
	{
		return unscaled_total_time;
	}

	inline float TimeSystem::DeltaTime() const
	{
		return static_cast<float>(delta_time);
	}

	inline float TimeSystem::UnscaledDeltaTime() const
	{
		return static_cast<float>(unscaled_delta_time);
	}

	inline void TimeSystem::IsVariableFrameRate(const bool is_variable)
	{
		is_variable_frame_rate = is_variable;
	}

	inline void TimeSystem::SetFrameRate(const float frame_rate)
	{
		//QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&counts_per_sec));
		seconds_per_count = 1.0 / static_cast<double>(counts_per_sec);
		frames_per_sec = static_cast<LONGLONG>(counts_per_sec / frame_rate);
	}

	inline void TimeSystem::CalcFramePerSec()
	{
		++m_frames;

		double elapsed_time;
		if (is_stopped)
		{
			elapsed_time = ((stop_count - paused_count) - awake_count) * seconds_per_count;
		}
		else
		{
			elapsed_time = ((this_count - paused_count) - awake_count) * seconds_per_count;
		}

		// Calculate averages over half second period
		if (elapsed_time - m_time_tlapsed >= 0.5)
		{
			m_fps = m_frames * 2;

			// Reset for next average
			m_frames = 0;
			m_time_tlapsed += 0.5;
		}
	}

	inline unsigned TimeSystem::GetFPS() const
	{
		return m_fps;
	}

	inline LONGLONG TimeSystem::GetAdjustedThisTime()
	{
		LARGE_INTEGER ret_time;
		if (stop_count != 0)
		{
			ret_time.QuadPart = stop_count;
		}
		else
		{
			QueryPerformanceCounter(&ret_time);
		}
		return ret_time.QuadPart;
	}

}	// namespace TKGEngine::Time