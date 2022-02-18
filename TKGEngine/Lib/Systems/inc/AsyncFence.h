#pragma once

#include <mutex>

namespace TKGEngine
{
	/// <summary>
	/// Load Async Fence module
	/// </summary>
	class AsyncFence
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		AsyncFence() = default;
		virtual ~AsyncFence() = default;
		AsyncFence(const AsyncFence&) = delete;
		AsyncFence& operator=(const AsyncFence&) = delete;

		void SetChecker();
		bool HasFinished() const;

		void Reset();


		// ==============================================
		// public variables
		// ==============================================


	private:
		// ==============================================
		// private methods
		// ==============================================
		void Checker();



		// ==============================================
		// private variables
		// ==============================================
		bool m_has_finished = false;
		std::mutex m_mutex;

	};

	// --------------------------------------------------------
	// inline
	// --------------------------------------------------------
	// ------------------
	// AsyncFence
	// ------------------
	inline bool AsyncFence::HasFinished() const
	{
		return m_has_finished;
	}

	inline void AsyncFence::Reset()
	{
		m_has_finished = false;
	}


}// namespace TKGEngine