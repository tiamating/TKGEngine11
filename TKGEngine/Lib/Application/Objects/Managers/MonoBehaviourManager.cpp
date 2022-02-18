
#include "MonoBehaviourManager.h"

#include "../Components/inc/CMonoBehaviour.h"
#include "../Components/inc/CCollider.h"

#include "SceneManager.h"

#ifdef USE_IMGUI
#include <Systems/inc/IGUI.h>
#endif // USE_IMGUI

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Static variable declaration
	////////////////////////////////////////////////////////
	std::unordered_map<GameObjectID, std::list<std::shared_ptr<MonoBehaviour>>> MonoBehaviourManager::m_attached_monoBehaviours;
	std::unordered_map<GameObjectID, std::unordered_map<std::string, std::shared_ptr<MonoBehaviour>>> MonoBehaviourManager::m_monoBeahviour_access_name_list;

	int MonoBehaviourManager::m_current_index = 0;
	std::unordered_map<int, std::shared_ptr<MonoBehaviour>> MonoBehaviourManager::m_store_list;
	bool MonoBehaviourManager::m_start_list_changed = false;
	std::list<std::shared_ptr<MonoBehaviour>> MonoBehaviourManager::m_start_list;
	bool MonoBehaviourManager::m_update_list_changed = false;
	std::list<std::shared_ptr<MonoBehaviour>> MonoBehaviourManager::m_update_list;

	std::unique_ptr<MonoBehaviourManager::EventFunctionManager> MonoBehaviourManager::m_event_func_manager;

	std::mutex MonoBehaviourManager::m_mutex;

	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	void MonoBehaviourManager::RegisterMonoBehaviour(GameObjectID goid, const std::shared_ptr<MonoBehaviour>& monoBehaviour)
	{
#ifdef USE_IMGUI
		// エディタ時は何もしない
		if (!IGUI::Get().IsPlaying())
			return;
#endif // USE_IMGUI

		m_mutex.lock();
		m_attached_monoBehaviours[goid].emplace_back(monoBehaviour);
		m_monoBeahviour_access_name_list[goid].emplace(std::make_pair(typeid(*monoBehaviour.get()).name(), monoBehaviour));
		m_mutex.unlock();

		// GameObjectが有効かチェック
		const auto s_go = monoBehaviour->GetGameObject();
		if (s_go == nullptr)
		{
			assert(0 && "invalid variable. MonoBehaviourManager::RegisterMonoBehaviour()");
			return;
		}

		if (s_go->GetActiveHierarchy())
		{
			monoBehaviour->CheckMonoBehaviourState(MonoBehaviour::MonoBehaviourEventType::OnAwake);
			monoBehaviour->Awake();

			// シーンロード時は
			// MonoBehaviourがenableでなければOnEnableは実行されない
			if (SceneManager::GetActive(s_go->GetScene()) && monoBehaviour->Enabled())
			{
				monoBehaviour->CheckMonoBehaviourState(MonoBehaviour::MonoBehaviourEventType::OnEnable);
				monoBehaviour->OnEnable();
				m_start_list.emplace_back(monoBehaviour);
				m_start_list_changed = true;
			}
			else
			{
				// OnEnable実行前のものをリストに登録する
				m_store_list.emplace(++m_current_index, monoBehaviour);
				monoBehaviour->SetStoreId(m_current_index);
			}
		}
		else
		{
			// Awake実行前のものをリストに登録する
			m_store_list.emplace(++m_current_index, monoBehaviour);
			monoBehaviour->SetStoreId(m_current_index);
		}
	}

	void MonoBehaviourManager::UnregisterMonoBehaviour(GameObjectID goid, InstanceID instance_id)
	{
		// GameObjectIDで検索
		const auto itr_go = m_attached_monoBehaviours.find(goid);
		if (itr_go == m_attached_monoBehaviours.end())
		{
			return;
		}
		// アタッチされているものを順に削除していく
		auto&& scripts = itr_go->second;
		const auto itr_end = scripts.end();
		for (auto itr = scripts.begin(); itr != itr_end; ++itr)
		{
			if ((*itr)->GetInstanceID() == instance_id)
			{
				// itrから名前を取得してアクセスリストからも削除する
				m_monoBeahviour_access_name_list[goid].erase(typeid(*(itr->get())).name());

				scripts.erase(itr);

				return;
			}
		}
	}

	void MonoBehaviourManager::AddStartList(const int id)
	{
		const auto itr = m_store_list.find(id);
		if (itr == m_store_list.end())
		{
			return;
		}

		m_start_list.emplace_back(itr->second);
		m_start_list_changed = true;
		m_store_list.erase(itr);
	}

	void MonoBehaviourManager::Run()
	{
		// Startリストにセットされたものを実行してUpdateリストに移す
		ExecuteStart();
		ExecuteUpdate();

		// Update内でStartリストにセットされたものを実行してUpdateリストに移す
		ExecuteStart();
		ExecuteLateUpdate();
	}

	void MonoBehaviourManager::OnEnable(const GameObjectID id)
	{
		// Check existence.
		const auto check_itr = m_attached_monoBehaviours.find(id);
		if (check_itr == m_attached_monoBehaviours.end())
			return;

		auto&& scripts = check_itr->second;
		const auto itr_end = scripts.end();
		for (auto itr = scripts.begin(); itr != itr_end; )
		{
			const auto& mono_behaviour = *itr;
			// Awakeが呼ばれていないときにGameObjectが有効ならAwakeを実行
			if (!mono_behaviour->IsCalledAwake() && mono_behaviour->GetGameObject()->GetActiveHierarchy())
			{
				mono_behaviour->CheckMonoBehaviourState(MonoBehaviour::MonoBehaviourEventType::OnAwake);
				mono_behaviour->Awake();
			}
			// Behaviourが有効でシーンがアクティブならEnabledを実行
			if (mono_behaviour->Enabled() && mono_behaviour->IsActiveScene())
			{
				mono_behaviour->CheckMonoBehaviourState(MonoBehaviour::MonoBehaviourEventType::OnEnable);
				mono_behaviour->OnEnable();
			}
			++itr;
		}
	}

	void MonoBehaviourManager::OnDisable(GameObjectID id)
	{
		// Check existence.
		const auto check_itr = m_attached_monoBehaviours.find(id);
		if (check_itr == m_attached_monoBehaviours.end())
			return;

		auto&& scripts = check_itr->second;
		const auto itr_end = scripts.end();
		for (auto itr = scripts.begin(); itr != itr_end; )
		{
			const auto& mono_behaviour = *itr;
			if (mono_behaviour->Enabled())
			{
				mono_behaviour->OnDisable();
			}
			++itr;
		}
	}

	void MonoBehaviourManager::OnDestroyed(GameObjectID id)
	{
		// Check existence.
		const auto check_itr = m_attached_monoBehaviours.find(id);
		if (check_itr == m_attached_monoBehaviours.end())
			return;

		// Call OnDisable
		auto&& scripts = check_itr->second;

		const auto itr_end = scripts.end();
		for (auto itr = scripts.begin(); itr != itr_end; )
		{
			// Awakeが呼ばれていないならOnDestroyも呼ばれない
			if ((*itr)->IsCalledAwake())
			{
				// Activeチェック
				if (((*itr)->IsActiveAndEnabled()) && (*itr)->IsActiveScene())
				{
					(*itr)->OnDisable();
				}
			}
			++itr;
		}

		// Call OnDestroyed
		for (auto itr = scripts.begin(); itr != itr_end; ++itr)
		{
			(*itr)->OnDestroyed();
		}

		// Erase from unordered_map
		m_attached_monoBehaviours.erase(id);
		m_monoBeahviour_access_name_list.erase(id);
	}

	void MonoBehaviourManager::OnCollisionEnter(GameObjectID id, std::shared_ptr<Collider>& collider)
	{
		// GameObjectIDからアタッチされているScriptのリストを取得する
		const auto itr_list_find = m_attached_monoBehaviours.find(id);
		// 存在確認
		if (itr_list_find == m_attached_monoBehaviours.end())
		{
			// 早期リターン
			return;
		}

		// そのGameObjectにアタッチされたScriptsすべての関数を呼ぶ
		for (const auto& mb : itr_list_find->second)
		{
			mb->OnCollisionEnter(collider);
		}
	}

	void MonoBehaviourManager::OnCollisionStay(GameObjectID id, std::shared_ptr<Collider>& collider)
	{
		// GameObjectIDからアタッチされているScriptのリストを取得する
		const auto itr_list_find = m_attached_monoBehaviours.find(id);
		// 存在確認
		if (itr_list_find == m_attached_monoBehaviours.end())
		{
			// 早期リターン
			return;
		}

		// そのGameObjectにアタッチされたScriptsすべての関数を呼ぶ
		for (const auto& mb : itr_list_find->second)
		{
			mb->OnCollisionStay(collider);
		}
	}

	void MonoBehaviourManager::OnCollisionExit(GameObjectID id, std::shared_ptr<Collider>& collider)
	{
		// GameObjectIDからアタッチされているScriptのリストを取得する
		const auto itr_list_find = m_attached_monoBehaviours.find(id);
		// 存在確認
		if (itr_list_find == m_attached_monoBehaviours.end())
		{
			// 早期リターン
			return;
		}

		// そのGameObjectにアタッチされたScriptsすべての関数を呼ぶ
		for (const auto& mb : itr_list_find->second)
		{
			mb->OnCollisionExit(collider);
		}
	}

	void MonoBehaviourManager::OnTriggerEnter(GameObjectID id, std::shared_ptr<Collider>& collider)
	{
		// GameObjectIDからアタッチされているScriptのリストを取得する
		const auto itr_list_find = m_attached_monoBehaviours.find(id);
		// 存在確認
		if (itr_list_find == m_attached_monoBehaviours.end())
		{
			// 早期リターン
			return;
		}

		// そのGameObjectにアタッチされたScriptsすべての関数を呼ぶ
		for (const auto& mb : itr_list_find->second)
		{
			mb->OnTriggerEnter(collider);
		}
	}

	void MonoBehaviourManager::OnTriggerStay(GameObjectID id, std::shared_ptr<Collider>& collider)
	{
		// GameObjectIDからアタッチされているScriptのリストを取得する
		const auto itr_list_find = m_attached_monoBehaviours.find(id);
		// 存在確認
		if (itr_list_find == m_attached_monoBehaviours.end())
		{
			// 早期リターン
			return;
		}

		// そのGameObjectにアタッチされたScriptsすべての関数を呼ぶ
		for (const auto& mb : itr_list_find->second)
		{
			mb->OnTriggerStay(collider);
		}
	}

	void MonoBehaviourManager::OnTriggerExit(GameObjectID id, std::shared_ptr<Collider>& collider)
	{
		// GameObjectIDからアタッチされているScriptのリストを取得する
		const auto itr_list_find = m_attached_monoBehaviours.find(id);
		// 存在確認
		if (itr_list_find == m_attached_monoBehaviours.end())
		{
			// 早期リターン
			return;
		}

		// そのGameObjectにアタッチされたScriptsすべての関数を呼ぶ
		for (const auto& mb : itr_list_find->second)
		{
			mb->OnTriggerExit(collider);
		}
	}

	void MonoBehaviourManager::ExecuteFunction(GameObjectID id, const std::string& class_type_name, const std::string& func_name)
	{
		// GameObjectID検索
		const auto itr_goid_end = m_monoBeahviour_access_name_list.end();
		const auto itr_goid_find = m_monoBeahviour_access_name_list.find(id);
		if (itr_goid_find == itr_goid_end)
		{
			return;
		}
		// Class型名検索
		const auto itr_name_map_end = itr_goid_find->second.end();
		const auto itr_name_map_find = itr_goid_find->second.find(class_type_name);
		if (itr_name_map_find == itr_name_map_end)
		{
			return;
		}
		// 実行
		if (m_event_func_manager)
			m_event_func_manager->ExecuteFunction(itr_name_map_find->second, func_name);
	}
	void MonoBehaviourManager::ExecuteFunction(GameObjectID id, const std::string& class_type_name, const std::string& func_name, const int param)
	{
		// GameObjectID検索
		const auto itr_goid_end = m_monoBeahviour_access_name_list.end();
		const auto itr_goid_find = m_monoBeahviour_access_name_list.find(id);
		if (itr_goid_find == itr_goid_end)
		{
			return;
		}
		// Class型名検索
		const auto itr_name_map_end = itr_goid_find->second.end();
		const auto itr_name_map_find = itr_goid_find->second.find(class_type_name);
		if (itr_name_map_find == itr_name_map_end)
		{
			return;
		}
		// 実行
		if (m_event_func_manager)
			m_event_func_manager->ExecuteFunction(itr_name_map_find->second, func_name, param);
	}
	void MonoBehaviourManager::ExecuteFunction(GameObjectID id, const std::string& class_type_name, const std::string& func_name, const float param)
	{
		// GameObjectID検索
		const auto itr_goid_end = m_monoBeahviour_access_name_list.end();
		const auto itr_goid_find = m_monoBeahviour_access_name_list.find(id);
		if (itr_goid_find == itr_goid_end)
		{
			return;
		}
		// Class型名検索
		const auto itr_name_map_end = itr_goid_find->second.end();
		const auto itr_name_map_find = itr_goid_find->second.find(class_type_name);
		if (itr_name_map_find == itr_name_map_end)
		{
			return;
		}
		// 実行
		if (m_event_func_manager)
			m_event_func_manager->ExecuteFunction(itr_name_map_find->second, func_name, param);
	}

	void MonoBehaviourManager::GetFunctionVoidNames(const std::string& class_type_name, std::list<std::string>& func_names)
	{
		// クリア
		func_names.clear();

		if (m_event_func_manager)
			m_event_func_manager->GetFunctionVoidNames(class_type_name, func_names);
	}
	void MonoBehaviourManager::GetFunctionIntNames(const std::string& class_type_name, std::list<std::string>& func_names)
	{
		// クリア
		func_names.clear();

		if (m_event_func_manager)
			m_event_func_manager->GetFunctionIntNames(class_type_name, func_names);
	}
	void MonoBehaviourManager::GetFunctionFloatNames(const std::string& class_type_name, std::list<std::string>& func_names)
	{
		// クリア
		func_names.clear();

		if (m_event_func_manager)
			m_event_func_manager->GetFunctionFloatNames(class_type_name, func_names);
	}

	void MonoBehaviourManager::ExecuteStart()
	{
		// 追加されたときはソートをしなおす
		if (m_start_list_changed)
		{
			m_start_list.sort([](const std::shared_ptr<MonoBehaviour>& l, const std::shared_ptr<MonoBehaviour>& r) {return l->Priority() < r->Priority(); });
			m_start_list_changed = false;
		}

		// Startが呼ばれる条件を満たしたならばリストから削除してUpdateListに移動する
		const auto itr_end = m_start_list.end();
		for (auto itr = m_start_list.begin(); itr != itr_end;)
		{
			// Check object existence.
			if (!(*itr)->IsExist())
			{
				itr = m_start_list.erase(itr);
				continue;
			}

			// Check Active and Enable.
			if (!(*itr)->IsActiveAndEnabled() || !(*itr)->IsActiveScene())
			{
				++itr;
				continue;
			}

			// Do func.
			(*itr)->CheckMonoBehaviourState(MonoBehaviour::MonoBehaviourEventType::OnStart);
			(*itr)->Start();

			// 毎フレーム更新を実行するScriptかチェック
			if ((*itr)->CanCallEveryFrameUpdate())
			{
				// Add Update list.
				m_update_list.emplace_back(*itr);
				m_update_list_changed = true;
			}

			itr = m_start_list.erase(itr);
		}
	}

	void MonoBehaviourManager::ExecuteUpdate()
	{
		// 追加されたときはソートをしなおす
		if (m_update_list_changed)
		{
			m_update_list.sort([](const std::shared_ptr<MonoBehaviour>& l, const std::shared_ptr<MonoBehaviour>& r) {return l->Priority() < r->Priority(); });
			m_update_list_changed = false;
		}

		// 状態をチェックしながら順に呼んでいく
		const auto itr_end = m_update_list.end();
		for (auto itr = m_update_list.begin(); itr != itr_end;)
		{
			// Check object existence.
			if (!(*itr)->IsExist())
			{
				itr = m_update_list.erase(itr);
				continue;
			}

			// Check Active and Enable.
			if (!(*itr)->IsActiveAndEnabled() || !(*itr)->IsActiveScene())
			{
				++itr;
				continue;
			}

			// Do func.
			(*itr)->Update();
			++itr;
		}
	}

	void MonoBehaviourManager::ExecuteLateUpdate()
	{
		// 追加されたときはソートをしなおす
		if (m_update_list_changed)
		{
			m_update_list.sort([](const std::shared_ptr<MonoBehaviour>& l, const std::shared_ptr<MonoBehaviour>& r) {return l->Priority() < r->Priority(); });
			m_update_list_changed = false;
		}

		// 状態をチェックしながら順に呼んでいく
		const auto itr_end = m_update_list.end();
		for (auto itr = m_update_list.begin(); itr != itr_end;)
		{
			// Check object existence.
			if (!(*itr)->IsExist())
			{
				itr = m_update_list.erase(itr);
				continue;
			}

			// Check Active and Enable.
			if (!(*itr)->IsActiveAndEnabled() || !(*itr)->IsActiveScene())
			{
				++itr;
				continue;
			}

			// Do func.
			(*itr)->LateUpdate();
			++itr;
		}
	}

	// ------------------------------------------------------------
	// EventFunctionManager
	// ------------------------------------------------------------
	void MonoBehaviourManager::EventFunctionManager::RegisterEventFunction(const std::string& class_type_name, const std::string& func_name, void(*func)(const std::shared_ptr<MonoBehaviour>&))
	{
		m_function_void_name_map[class_type_name][func_name] = func;
	}
	void MonoBehaviourManager::EventFunctionManager::RegisterEventFunction(const std::string& class_type_name, const std::string& func_name, void(*func)(const std::shared_ptr<MonoBehaviour>&, int))
	{
		m_function_int_name_map[class_type_name][func_name] = func;
	}
	void MonoBehaviourManager::EventFunctionManager::RegisterEventFunction(const std::string& class_type_name, const std::string& func_name, void(*func)(const std::shared_ptr<MonoBehaviour>&, float))
	{
		m_function_float_name_map[class_type_name][func_name] = func;
	}

	void MonoBehaviourManager::EventFunctionManager::ExecuteFunction(const std::shared_ptr<MonoBehaviour>& monoBehaviour, const std::string& func_name)
	{
		// クラス名から関数マップを探索
		const auto itr_class_map = m_function_void_name_map.find(typeid(*monoBehaviour.get()).name());
		// 早期リターン
		if (itr_class_map == m_function_void_name_map.end())
			return;

		// 関数名から関数ポインタを探索
		const auto itr_func_map = itr_class_map->second.find(func_name);
		// 早期リターン
		if (itr_func_map == itr_class_map->second.end())
			return;

		// 関数の実行
		itr_func_map->second(monoBehaviour);
	}
	void MonoBehaviourManager::EventFunctionManager::ExecuteFunction(const std::shared_ptr<MonoBehaviour>& monoBehaviour, const std::string& func_name, const int param)
	{
		// クラス名から関数マップを探索
		const auto itr_class_map = m_function_int_name_map.find(typeid(*monoBehaviour.get()).name());
		// 早期リターン
		if (itr_class_map == m_function_int_name_map.end())
			return;

		// 関数名から関数ポインタを探索
		const auto itr_func_map = itr_class_map->second.find(func_name);
		// 早期リターン
		if (itr_func_map == itr_class_map->second.end())
			return;

		// 関数の実行
		itr_func_map->second(monoBehaviour, param);
	}
	void MonoBehaviourManager::EventFunctionManager::ExecuteFunction(const std::shared_ptr<MonoBehaviour>& monoBehaviour, const std::string& func_name, const float param)
	{
		// クラス名から関数マップを探索
		const auto itr_class_map = m_function_float_name_map.find(typeid(*monoBehaviour.get()).name());
		// 早期リターン
		if (itr_class_map == m_function_float_name_map.end())
			return;

		// 関数名から関数ポインタを探索
		const auto itr_func_map = itr_class_map->second.find(func_name);
		// 早期リターン
		if (itr_func_map == itr_class_map->second.end())
			return;

		// 関数の実行
		itr_func_map->second(monoBehaviour, param);
	}

	void MonoBehaviourManager::EventFunctionManager::GetFunctionVoidNames(const std::string& class_type_name, std::list<std::string>& func_names)
	{
		// クラス名から関数マップを探索
		const auto itr_class_map = m_function_void_name_map.find(class_type_name);
		// 早期リターン
		if (itr_class_map == m_function_void_name_map.end())
			return;

		// mapのendまでfirstの関数名を詰め込んでいく
		for (auto& func_data : itr_class_map->second)
		{
			func_names.emplace_back(func_data.first);
		}
	}
	void MonoBehaviourManager::EventFunctionManager::GetFunctionIntNames(const std::string& class_type_name, std::list<std::string>& func_names)
	{
		// クラス名から関数マップを探索
		const auto itr_class_map = m_function_int_name_map.find(class_type_name);
		// 早期リターン
		if (itr_class_map == m_function_int_name_map.end())
			return;

		// mapのendまでfirstの関数名を詰め込んでいく
		for (auto& func_data : itr_class_map->second)
		{
			func_names.emplace_back(func_data.first);
		}
	}
	void MonoBehaviourManager::EventFunctionManager::GetFunctionFloatNames(const std::string& class_type_name, std::list<std::string>& func_names)
	{
		// クラス名から関数マップを探索
		const auto itr_class_map = m_function_float_name_map.find(class_type_name);
		// 早期リターン
		if (itr_class_map == m_function_float_name_map.end())
			return;

		// mapのendまでfirstの関数名を詰め込んでいく
		for (auto& func_data : itr_class_map->second)
		{
			func_names.emplace_back(func_data.first);
		}
	}

	// ------------------------------------------------------------
	// ~EventFunctionManager
	// ------------------------------------------------------------

}// namespace TKGEngine