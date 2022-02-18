#pragma once

#include "Graphics_IQueue.h"

#include "../../inc/Graphics_Defined.h"

#include <vector>

#include <d3d11.h>
#include <memory>

namespace TKGEngine::Graphics
{
	//class IQueryDisjoint;
	//class IQueryTimeStamp;

	class Queue
		: public IQueue
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Queue() = default;
		virtual ~Queue();
		Queue(const Queue&) = delete;
		Queue& operator=(const Queue&) = delete;

		bool Init(
			ID3D11Device* p_device, 
			int num_passes/* number of using pass */,
			int num_threads/* number of using thread */) override;
		void Release() override;

		void Execute(ID3D11DeviceContext* p_ic, int thread_idx, int pass_idx) override;

		ICommandList* GetCommandList(int thread_idx, int pass_idx) const override;

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================
		/* nothing */


		// ==============================================
		// private variables
		// ==============================================
		bool m_is_init = false;

		//std::unique_ptr<IQueryDisjoint> m_query_disjoint = nullptr;
		//std::unique_ptr<IQueryTimeStamp> m_query_ts_begin = nullptr;
		//std::unique_ptr<IQueryTimeStamp> m_query_ts_end = nullptr;
		// pass0[th0,th1,...,thn],pass1[th0,th1,...,thn],...,passn[th0,th1,...,thn]
		std::vector<std::unique_ptr<ICommandList>> m_cmd_lists;

		int m_num_passes = 0;
		int m_num_threads = 0;
	};


}	// namespace TKGEngine::Graphics