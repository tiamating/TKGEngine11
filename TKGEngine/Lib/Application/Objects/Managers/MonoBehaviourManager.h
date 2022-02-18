#pragma once

#include "../../Objects/inc/IGameObject.h"

#include <list>
#include <unordered_map>

#include <memory>
#include <mutex>

namespace TKGEngine
{
	class MonoBehaviour;
	class Collider;

	class MonoBehaviourManager
	{
	public:
		// ==============================================
		// private class
		// ==============================================
		// アニメーションイベントに別名をつけて、クラス名と別名で呼び出せるようにするためのクラスに登録するためのクラス
		class RegisterFunction
		{
		public:
			RegisterFunction(const std::string& class_type_name, const std::string& func_name, void(*func)(const std::shared_ptr<MonoBehaviour>&))
			{
				CreateOnce();
				m_event_func_manager->RegisterEventFunction(class_type_name, func_name, func);
			}
			RegisterFunction(const std::string& class_type_name, const std::string& func_name, void(*func)(const std::shared_ptr<MonoBehaviour>&, int))
			{
				CreateOnce();
				m_event_func_manager->RegisterEventFunction(class_type_name, func_name, func);
			}
			RegisterFunction(const std::string& class_type_name, const std::string& func_name, void(*func)(const std::shared_ptr<MonoBehaviour>&, float))
			{
				CreateOnce();
				m_event_func_manager->RegisterEventFunction(class_type_name, func_name, func);
			}

		private:
			void CreateOnce() const
			{
				// 初回呼び出し時に生成
				if (!m_event_func_manager)
				{
					m_event_func_manager = std::make_unique<EventFunctionManager>();
				}
			}
		};


		// ==============================================
		// public methods
		// ==============================================
		MonoBehaviourManager() = default;
		virtual ~MonoBehaviourManager() = default;
		MonoBehaviourManager(const MonoBehaviourManager&) = delete;
		MonoBehaviourManager& operator=(const MonoBehaviourManager&) = delete;

		static void RegisterMonoBehaviour(GameObjectID goid, const std::shared_ptr<MonoBehaviour>& monoBehaviour);
		static void UnregisterMonoBehaviour(GameObjectID goid, InstanceID instance_id);

		static void AddStartList(int id);

		static void Run();

		// Event message
		// TODO : Event Messageも実行順を必要とするならソートするべき？
		static void OnEnable(GameObjectID id);
		static void OnDisable(GameObjectID id);
		static void OnDestroyed(GameObjectID id);

		// Collision Event
		static void OnCollisionEnter(GameObjectID id, std::shared_ptr<Collider>& collider);
		static void OnCollisionStay(GameObjectID id, std::shared_ptr<Collider>& collider);
		static void OnCollisionExit(GameObjectID id, std::shared_ptr<Collider>& collider);
		// Trigger Event
		static void OnTriggerEnter(GameObjectID id, std::shared_ptr<Collider>& collider);
		static void OnTriggerStay(GameObjectID id, std::shared_ptr<Collider>& collider);
		static void OnTriggerExit(GameObjectID id, std::shared_ptr<Collider>& collider);

		// Animation Event
		// 実行
		static void ExecuteFunction(GameObjectID id, const std::string& class_type_name, const std::string& func_name);
		static void ExecuteFunction(GameObjectID id, const std::string& class_type_name, const std::string& func_name, const int param);
		static void ExecuteFunction(GameObjectID id, const std::string& class_type_name, const std::string& func_name, const float param);
		// クラス内に存在する関数名リストの取得
		static void GetFunctionVoidNames(const std::string& class_type_name, std::list<std::string>& func_names);
		static void GetFunctionIntNames(const std::string& class_type_name, std::list<std::string>& func_names);
		static void GetFunctionFloatNames(const std::string& class_type_name, std::list<std::string>& func_names);


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private class
		// ==============================================
		// アニメーションイベントに別名をつけて、クラス名と別名で呼び出せるようにするためのクラスのデータ部分
		class EventFunctionManager
		{
		public:
			// 登録
			void RegisterEventFunction(const std::string& class_type_name, const std::string& func_name, void(*func)(const std::shared_ptr<MonoBehaviour>&));
			void RegisterEventFunction(const std::string& class_type_name, const std::string& func_name, void(*func)(const std::shared_ptr<MonoBehaviour>&, int));
			void RegisterEventFunction(const std::string& class_type_name, const std::string& func_name, void(*func)(const std::shared_ptr<MonoBehaviour>&, float));

			// 実行
			void ExecuteFunction(const std::shared_ptr<MonoBehaviour>& monoBehaviour, const std::string& func_name);
			void ExecuteFunction(const std::shared_ptr<MonoBehaviour>& monoBehaviour, const std::string& func_name, const int param);
			void ExecuteFunction(const std::shared_ptr<MonoBehaviour>& monoBehaviour, const std::string& func_name, const float param);
			// クラス内に存在する関数名リストの取得
			void GetFunctionVoidNames(const std::string& class_type_name, std::list<std::string>& func_names);
			void GetFunctionIntNames(const std::string& class_type_name, std::list<std::string>& func_names);
			void GetFunctionFloatNames(const std::string& class_type_name, std::list<std::string>& func_names);

		private:
			// map<型名, map<関数の別名,関数ポインタ>>
			std::unordered_map<std::string, std::unordered_map<std::string, void(*)(const std::shared_ptr<MonoBehaviour>&)>> m_function_void_name_map;
			std::unordered_map<std::string, std::unordered_map<std::string, void(*)(const std::shared_ptr<MonoBehaviour>&, int)>> m_function_int_name_map;
			std::unordered_map<std::string, std::unordered_map<std::string, void(*)(const std::shared_ptr<MonoBehaviour>&, float)>> m_function_float_name_map;
		};


		// ==============================================
		// private methods
		// ==============================================
		// それぞれ実行前にPriorityソートを行う
		static void ExecuteStart();
		static void ExecuteUpdate();
		static void ExecuteLateUpdate();


		// ==============================================
		// private variables
		// ==============================================
		// GameObjectにアタッチされたMonoBehaviourのリスト
		static std::unordered_map<GameObjectID, std::list<std::shared_ptr<MonoBehaviour>>> m_attached_monoBehaviours;
		// GameObjectにアタッチされたMonoBehaviourをクラス名で検索するためのマップ
		static std::unordered_map<GameObjectID, std::unordered_map<std::string, std::shared_ptr<MonoBehaviour>>> m_monoBeahviour_access_name_list;
		// ユニークなIDを振るための現在のインデックス
		static int m_current_index;
		// 初回のOnEnableを呼ばれていないものリスト
		static std::unordered_map<int, std::shared_ptr<MonoBehaviour>> m_store_list;
		// Startを呼ぶ予定のリストに変更があったか
		static bool m_start_list_changed;
		static std::list<std::shared_ptr<MonoBehaviour>> m_start_list;
		// Updateを呼ぶ予定のリストに変更があったか
		static bool m_update_list_changed;
		static std::list<std::shared_ptr<MonoBehaviour>> m_update_list;
		
		// アニメーションイベント管理用マネージャ
		static std::unique_ptr<EventFunctionManager> m_event_func_manager;

		static std::mutex m_mutex;
	};
}// namespace TKGEngine

#define REGISTER_EVENT_FUNCTION_VOID(CLASS, FUNC_NAME) namespace{\
	TKGEngine::MonoBehaviourManager::RegisterFunction ___VOID_EVENT_INSTANCE_##FUNC_NAME\
	(\
		typeid(CLASS).name(), #FUNC_NAME,\
		[](const std::shared_ptr<TKGEngine::MonoBehaviour>& mb)\
		{\
			std::static_pointer_cast<CLASS>(mb)->FUNC_NAME();\
		}\
	);\
}

#define REGISTER_EVENT_FUNCTION_INT(CLASS, FUNC_NAME) namespace{\
	TKGEngine::MonoBehaviourManager::RegisterFunction ___INT_EVENT_INSTANCE_##FUNC_NAME\
	(\
		typeid(CLASS).name(), #FUNC_NAME,\
		[](const std::shared_ptr<TKGEngine::MonoBehaviour>& mb, int param)\
		{\
			std::static_pointer_cast<CLASS>(mb)->FUNC_NAME(param);\
		}\
	);\
}

#define REGISTER_EVENT_FUNCTION_FLOAT(CLASS, FUNC_NAME) namespace{\
	TKGEngine::MonoBehaviourManager::RegisterFunction ___FLOAT_EVENT_INSTANCE_##FUNC_NAME\
	(\
		typeid(CLASS).name(), #FUNC_NAME,\
		[](const std::shared_ptr<TKGEngine::MonoBehaviour>& mb, float param)\
		{\
			std::static_pointer_cast<CLASS>(mb)->FUNC_NAME(param);\
		}\
	);\
}