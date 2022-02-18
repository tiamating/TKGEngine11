#pragma once

#include "../inc/Object.h"

#include <unordered_map>
#include <list>
#include <deque>
#include <utility>

#include <memory>

namespace TKGEngine
{
	/// <summary>
	/// Object manager. (static class)
	/// </summary>
	class ObjectManager
	{
		using DeletionData = std::pair<float/*time[s]*/, InstanceID>;

	public:
		// ==============================================
		// public methods
		// ==============================================
		ObjectManager() = default;
		virtual ~ObjectManager() = default;
		ObjectManager(const ObjectManager&) = delete;
		ObjectManager& operator=(const ObjectManager&) = delete;
		

		static void FrameUpdate(float elapsed_time);	// TODO : 削除リストの更新、SceneのframeUpdateの最後に呼ぶ

		static std::weak_ptr<Object> Set(const std::shared_ptr<Object>& p_obj);

		// delay < 0.0fならその場で削除
		static void Destroy(InstanceID id, float delay = 0.0f);

		static InstanceID AttachID();
		static std::weak_ptr<Object> GetInstance(InstanceID id);


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================
		static void DestroyImmediate(InstanceID id);


		// ==============================================
		// private variables
		// ==============================================
		static std::unordered_map<InstanceID, std::shared_ptr<Object>> m_object_index;	// object instance map
		static std::list<DeletionData> m_deletion_list;
		static std::deque<InstanceID> m_empty_id_pool;	// Empty pooling ID
		static std::mutex m_id_pool_mutex;
		static InstanceID m_current_id;

	};

}// namespace TKGEngine