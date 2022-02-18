#pragma once

#include <functional>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>
#include <deque>

#include <Windows.h>
#include <cassert>

struct ID3D11Device;
struct ID3D11DeviceContext;


namespace TKGEngine
{
	class AssetSystem
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static void Create();
		static void Destroy();
		static AssetSystem& GetInstance();

		bool Add(const std::function<void()>& func);
		bool Add(std::function<void()>&& func);

		ID3D11Device* GetDevice() const;
		ID3D11DeviceContext* GetCopyDC() const;


		// ==============================================
		// public variables
		// ==============================================


	private:
		// ==============================================
		// private methods
		// ==============================================
		void Init();
		void Term();


		// ==============================================
		// private variables
		// ==============================================
		AssetSystem() = default;
		virtual ~AssetSystem() = default;
		AssetSystem(const AssetSystem&) = delete;
		AssetSystem& operator=(const AssetSystem&) = delete;

		static AssetSystem* m_instance;

		bool m_is_termination_requested = true;
		std::mutex m_mutex;
		std::condition_variable m_cv;
		std::vector<std::thread> m_threads;
		std::deque<std::function<void()>> m_func_deque;
		std::function<void()> m_load_func = [this]()
		{
			// スレッドの初期準備
			if (FAILED(CoInitialize(NULL)))
			{
				assert(0 && "failed create thread AssetSystem");
				return;
			}
			// 関数の実行ループ
			while (true)
			{
				std::function<void()> func;
				{
					std::unique_lock<std::mutex> lock(m_mutex);
					// Dequeにデータが存在しない間は待機
					while (m_func_deque.empty())
					{
						// インスタンスの終了時はスレッドの処理を終了
						if (m_is_termination_requested)
						{
							return;
						}
						m_cv.wait(lock);
					}
					func = std::move(m_func_deque.front());
					m_func_deque.pop_front();
				}
				func();
			}
			// スレッドの終端処理
			CoUninitialize();
		};
	};


}// namespace TKGEngine