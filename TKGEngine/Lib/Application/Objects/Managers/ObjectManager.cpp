
#include "ObjectManager.h"

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Static variable declaration
	////////////////////////////////////////////////////////
	std::unordered_map<InstanceID, std::shared_ptr<Object>> ObjectManager::m_object_index;
	std::list<ObjectManager::DeletionData> ObjectManager::m_deletion_list;
	std::deque<InstanceID> ObjectManager::m_empty_id_pool;
	std::mutex ObjectManager::m_id_pool_mutex;
	InstanceID ObjectManager::m_current_id = 0;


	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	void ObjectManager::FrameUpdate(float elapsed_time)
	{
		// Sort Deletion Data from time is most early.
		m_deletion_list.sort(
			[](const ObjectManager::DeletionData& l, const ObjectManager::DeletionData& r)
			{
				return l.first < r.first;
			});

		// If time is less equal 0, call OnDestroy.
		const auto itr_end = m_deletion_list.end();
		for (auto itr = m_deletion_list.begin(); itr != itr_end;)
		{
			if (itr->first <= 0.0f)
			{
				{
					const int dest_id = itr->second;
					auto&& inst = m_object_index.at(dest_id);
					// nullptrチェック
					if (inst)
					{
						inst->OnDestroy();
						inst->Object::OnDestroy();
					}
					// destroyリストから消去
					m_object_index.erase(dest_id);
					// InstanceIDの使いまわしリストに入れる
					std::lock_guard<std::mutex> lock(m_id_pool_mutex);
					m_empty_id_pool.push_back(dest_id);
				}
				itr = m_deletion_list.erase(itr);
			}
			else
			{
				// Sortしてあるのでこれ以降は確認不要
				// DEBUG : OnDestroy内でリストに登録した場合ソートが無駄になる
				break;
			}
		}

		// Caluclate time with elapsed time.
		for (auto&& pair : m_deletion_list)
		{
			pair.first -= elapsed_time;
		}

		return;
	}

	std::weak_ptr<Object> ObjectManager::Set(const std::shared_ptr<Object>& p_obj)
	{
		const InstanceID obj_id = p_obj->GetInstanceID();
		std::weak_ptr<Object> weak;

		const auto ret_val = m_object_index.emplace(obj_id, p_obj);
		if (!ret_val.second)
		{
			assert(0 && "failed Insert ObjectManager Index ObjectManager::Set()");
			return weak;
		}
		weak = p_obj;
		return weak;
	}

	void ObjectManager::Destroy(InstanceID id, float delay)
	{
		if (delay < 0.0f)
		{
			DestroyImmediate(id);
			return;
		}

		m_deletion_list.emplace_back(delay, id);
	}

	void ObjectManager::DestroyImmediate(InstanceID id)
	{
		{
			const auto itr = m_object_index.find(id);
			if (itr != m_object_index.end())
			{
				itr->second->OnDestroy();
				itr->second->Object::OnDestroy();
				m_object_index.erase(itr);
			}
		}
		std::lock_guard<std::mutex> lock(m_id_pool_mutex);
		m_empty_id_pool.push_back(id);
	}

	InstanceID ObjectManager::AttachID()
	{
		std::lock_guard<std::mutex> lock(m_id_pool_mutex);
		if (m_empty_id_pool.empty())
		{
			return ++m_current_id;
		}
		else
		{
			const InstanceID ret_id = m_empty_id_pool.front();
			m_empty_id_pool.pop_front();
			return ret_id;
		}
		// No reached
		assert(0);
	}

	std::weak_ptr<Object> ObjectManager::GetInstance(InstanceID id)
	{
		std::weak_ptr<Object> ret_ptr = m_object_index.at(id);
		return ret_ptr;
	}

}//namespace TKGEngine 