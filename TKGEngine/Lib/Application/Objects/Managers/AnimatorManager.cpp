
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
		// ���X�g�̃N���A
		m_update_animator_list.clear();

		std::lock_guard<std::mutex> lock(m_mutex);
		// �񓯊��΍�̍X�V�\�񃊃X�g�ɂ����
		for (auto& anim_pair : m_animator_index)
		{
			const auto& animator = anim_pair.second;
			// �A�N�e�B�u�`�F�b�N
			if (animator->IsActiveAnimator())
			{
				// �A�N�e�B�u�Ȃ烊�X�g�ɒǉ�
				m_update_animator_list.emplace_back(animator);
			}
		}
	}

	void AnimatorManager::ApplyAnimationTransform()
	{
		// �A�j���[�V����Transform�̓K�p
		for (const auto& animator : m_update_animator_list)
		{
			// �A�N�e�B�u�`�F�b�N
			if (animator->IsActiveAnimator())
			{
				m_thread_result.emplace_back(m_threads.Add([&]() {animator->ApplyAnimationTransform(); }));
			}
		}
		// Thread�̏I����ҋ@
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
			// �A�N�e�B�u�`�F�b�N
			if (animator->IsActiveAnimator())
			{
				// �A�j���[�V�����s��̐���
				animator->UpdateAnimationMatrix();
			}
		}
	}

}// namespace TKGEngine