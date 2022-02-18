
#include "../../inc/AsyncFence.h"

#include "../../inc/AssetSystem.h"


namespace TKGEngine
{
	void AsyncFence::SetChecker()
	{
		AssetSystem::GetInstance().Add(std::bind(&AsyncFence::Checker, this));
	}

	void AsyncFence::Checker()
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_has_finished = true;
	}

}// namespace TKGEngine