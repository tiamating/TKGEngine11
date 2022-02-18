
#include "AnimatorManager.h"

#include "Components/interface/IAnimator.h"

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Static member definition
	////////////////////////////////////////////////////////
	std::unordered_map<int, std::shared_ptr<IAnimator>> AnimatorManager::m_animator_index;
	int AnimatorManager::m_current_id = 0;

	std::list<std::shared_ptr<IAnimator>> AnimatorManager::m_update_animator_list;

	std::mutex AnimatorManager::m_mutex;
	ThreadPool AnimatorManager::m_threads = ThreadPool(THREAD_NUM);
	std::vector<std::future<void>> AnimatorManager::m_thread_result = std::vector<std::future<void>>(THREAD_NUM);


	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	int AnimatorManager::RegisterManager(const std::shared_ptr<IAnimator>& p_anim)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		m_animator_index.emplace(++m_current_id, p_anim);
		return m_current_id;
	}

	void AnimatorManager::UnregisterManager(const int id)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		m_animator_index.erase(id);
	}

	void AnimatorManager::CheckUpdatable()
	{
		// リストのクリア
		m_update_animator_list.clear();

		std::lock_guard<std::mutex> lock(m_mutex);
		// 非同期対策の更新予約リストにいれる
		for (auto& anim_pair : m_animator_index)
		{
			const auto& animator = anim_pair.second;
			// アクティブチェック
			if (animator->IsActiveAnimator())
			{
				// アクティブならリストに追加
				m_update_animator_list.emplace_back(animator);
			}
		}
	}

	void AnimatorManager::ApplyAnimationTransform()
	{
		// アニメーションTransformの適用
		for (const auto& animator : m_update_animator_list)
		{
			// アクティブチェック
			if (animator->IsActiveAnimator())
			{
				m_thread_result.emplace_back(m_threads.Add([&]() {animator->ApplyAnimationTransform(); }));
			}
		}
		// Threadの終了を待機
		for(const auto& result : m_thread_result)
		{
			if (!result.valid())
				continue;
			result.wait();
		}
		m_thread_result.clear();
	}

	void AnimatorManager::UpdateAnimationMatrix()
	{
		for (const auto& animator : m_update_animator_list)
		{
			// アクティブチェック
			if (animator->IsActiveAnimator())
			{
				// アニメーション行列の生成
				animator->UpdateAnimationMatrix();
			}
		}
	}

}// namespace TKGEngine