#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

namespace TKGEngine
{
	/// <summary>
	/// スレッドプール管理クラス
	/// </summary>
	class ThreadPool {
	public:
		ThreadPool(size_t);
		template<class F, class... Args>
		auto Add(F&& f, Args&&... args)
			->std::future<typename std::result_of<F(Args...)>::type>;
		~ThreadPool();

	private:
		// プーリングしているスレッド
		std::vector<std::thread> m_workers;
		// タスクリスト
		std::queue< std::function<void()> > m_tasks;

		std::mutex m_mutex;
		std::condition_variable m_cv;
		bool m_is_stop;
	};

	// ワーカースレッドを起動する
	inline ThreadPool::ThreadPool(size_t threads)
		: m_is_stop(false)
	{
		for (size_t i = 0; i < threads; ++i)
			m_workers.emplace_back(
				[this]
				{
					for (;;)
					{
						std::function<void()> task;
						{
							std::unique_lock<std::mutex> lock(this->m_mutex);
							this->m_cv.wait(lock,
								[this] { return this->m_is_stop || !this->m_tasks.empty(); });
							if (this->m_is_stop && this->m_tasks.empty())
								return;
							task = std::move(this->m_tasks.front());
							this->m_tasks.pop();
						}

						task();
					}
				}
				);
	}

	// プーリングに新しい処理を追加
	template<class F, class... Args>
	auto ThreadPool::Add(F&& f, Args&&... args)
		-> std::future<typename std::result_of<F(Args...)>::type>
	{
		using return_type = typename std::result_of<F(Args...)>::type;

		auto task = std::make_shared< std::packaged_task<return_type()> >(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
			);

		std::future<return_type> res = task->get_future();
		{
			std::unique_lock<std::mutex> lock(m_mutex);

			// プールを停止した後の追加を許可しない
			if (m_is_stop)
				throw std::runtime_error("enqueue on stopped ThreadPool");

			m_tasks.emplace([task]() { (*task)(); });
		}
		m_cv.notify_one();
		return res;
	}

	// 全てのスレッドをjoinする
	inline ThreadPool::~ThreadPool()
	{
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_is_stop = true;
		}
		m_cv.notify_all();
		for (std::thread& worker : m_workers)
			worker.join();
	}
}