#pragma once

#include "Utility/inc/template_thread.h"

#include <unordered_map>
#include <list>
#include <memory>
#include <mutex>


namespace TKGEngine
{
	class IAnimator;

	/// <summary>
	/// Management Animator components.
	/// </summary>
	class AnimatorManager
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static int RegisterManager(const std::shared_ptr<IAnimator>& p_anim);
		static void UnregisterManager(int id);

		// �X�V���s�����̂����X�g�ɓo�^����
		static void CheckUpdatable();
		// �A�j���[�V����Transform�̓K�p
		static void ApplyAnimationTransform();
		// �A�j���[�V�����s��̐���
		static void UpdateAnimationMatrix();


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================
		/* nothing */


		//==============================================
		//private variables
		//==============================================
		// Animator�̎Q�ƃ��X�g
		static std::unordered_map<int, std::shared_ptr<IAnimator>> m_animator_index;
		// unique ID
		static int m_current_id;
		// �񓯊��΍��p�̍X�V���s�ҋ@���郊�X�g
		static std::list<std::shared_ptr<IAnimator>> m_update_animator_list;

		// ���̃}�l�[�W���[��mutex
		static std::mutex m_mutex;
		// �X�V�pthread
		static constexpr int THREAD_NUM = 12;
		static ThreadPool m_threads;
		static std::vector<std::future<void>> m_thread_result;
	};
}// namespace TKGEngine