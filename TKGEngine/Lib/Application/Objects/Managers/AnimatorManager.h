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

		// 更新を行うものをリストに登録する
		static void CheckUpdatable();
		// アニメーションTransformの適用
		static void ApplyAnimationTransform();
		// アニメーション行列の生成
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
		// Animatorの参照リスト
		static std::unordered_map<int, std::shared_ptr<IAnimator>> m_animator_index;
		// unique ID
		static int m_current_id;
		// 非同期対策用の更新実行待機するリスト
		static std::list<std::shared_ptr<IAnimator>> m_update_animator_list;

		// このマネージャーのmutex
		static std::mutex m_mutex;
		// 更新用thread
		static constexpr int THREAD_NUM = 12;
		static ThreadPool m_threads;
		static std::vector<std::future<void>> m_thread_result;
	};
}// namespace TKGEngine