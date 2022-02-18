
#include "Graphics_Queue.h"

#include "Graphics_ICommandList.h"

#include <cassert>

namespace TKGEngine::Graphics
{
	IQueue* IQueue::CreateInterface()
	{
		return new Queue();
	}

	Queue::~Queue()
	{
		Release();
	}

	bool Queue::Init(
		ID3D11Device* p_device,
		int num_passes/* number of using pass */,
		int num_threads/* number of using thread */)
	{
		assert(p_device != nullptr);
		assert(num_passes > 0);
		assert(num_threads > 0);

		m_is_init = true;

		// Create CommandList
		m_num_passes = num_passes;
		m_num_threads = num_threads;
		const int num_elem = num_passes * num_threads;
		m_cmd_lists.resize(num_elem);
		for (int i = 0; i < num_elem; ++i)
		{
			m_cmd_lists.at(i).reset(ICommandList::CreateInterface());
			if (m_cmd_lists.at(i)->Init(p_device) == false)
			{
				Release();
				return  false;
			}
		}

		return true;
	}

	void Queue::Release()
	{
		if (m_is_init)
		{
			const int num_elem = m_num_passes * m_num_threads;
			for (int i = 0; i < num_elem; ++i)
			{
				m_cmd_lists.at(i).reset();
			}
			m_cmd_lists.clear();
			m_cmd_lists.shrink_to_fit();

			m_num_passes = 0;
			m_num_threads = 0;

			m_is_init = false;
		}
	}

	void Queue::Execute(ID3D11DeviceContext* p_ic, int thread_idx, int pass_idx)
	{
		assert(pass_idx < m_num_passes);
		assert(thread_idx < m_num_threads);
		

		auto cmd_list = m_cmd_lists.at(pass_idx * m_num_threads + thread_idx).get();
		cmd_list->FinishCommandList();
		cmd_list->ExecuteCommandList(p_ic);
	}

	ICommandList* Queue::GetCommandList(int thread_idx, int pass_idx) const
	{
		assert(pass_idx < m_num_passes);
		assert(thread_idx < m_num_threads);

		return m_cmd_lists.at(pass_idx * m_num_threads + thread_idx).get();
	}

}	// namespace TKGEngine::Graphics