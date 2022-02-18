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
		// �A�j���[�V�����C�x���g�ɕʖ������āA�N���X���ƕʖ��ŌĂяo����悤�ɂ��邽�߂̃N���X�ɓo�^���邽�߂̃N���X
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
				// ����Ăяo�����ɐ���
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
		// TODO : Event Message�����s����K�v�Ƃ���Ȃ�\�[�g����ׂ��H
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
		// ���s
		static void ExecuteFunction(GameObjectID id, const std::string& class_type_name, const std::string& func_name);
		static void ExecuteFunction(GameObjectID id, const std::string& class_type_name, const std::string& func_name, const int param);
		static void ExecuteFunction(GameObjectID id, const std::string& class_type_name, const std::string& func_name, const float param);
		// �N���X���ɑ��݂���֐������X�g�̎擾
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
		// �A�j���[�V�����C�x���g�ɕʖ������āA�N���X���ƕʖ��ŌĂяo����悤�ɂ��邽�߂̃N���X�̃f�[�^����
		class EventFunctionManager
		{
		public:
			// �o�^
			void RegisterEventFunction(const std::string& class_type_name, const std::string& func_name, void(*func)(const std::shared_ptr<MonoBehaviour>&));
			void RegisterEventFunction(const std::string& class_type_name, const std::string& func_name, void(*func)(const std::shared_ptr<MonoBehaviour>&, int));
			void RegisterEventFunction(const std::string& class_type_name, const std::string& func_name, void(*func)(const std::shared_ptr<MonoBehaviour>&, float));

			// ���s
			void ExecuteFunction(const std::shared_ptr<MonoBehaviour>& monoBehaviour, const std::string& func_name);
			void ExecuteFunction(const std::shared_ptr<MonoBehaviour>& monoBehaviour, const std::string& func_name, const int param);
			void ExecuteFunction(const std::shared_ptr<MonoBehaviour>& monoBehaviour, const std::string& func_name, const float param);
			// �N���X���ɑ��݂���֐������X�g�̎擾
			void GetFunctionVoidNames(const std::string& class_type_name, std::list<std::string>& func_names);
			void GetFunctionIntNames(const std::string& class_type_name, std::list<std::string>& func_names);
			void GetFunctionFloatNames(const std::string& class_type_name, std::list<std::string>& func_names);

		private:
			// map<�^��, map<�֐��̕ʖ�,�֐��|�C���^>>
			std::unordered_map<std::string, std::unordered_map<std::string, void(*)(const std::shared_ptr<MonoBehaviour>&)>> m_function_void_name_map;
			std::unordered_map<std::string, std::unordered_map<std::string, void(*)(const std::shared_ptr<MonoBehaviour>&, int)>> m_function_int_name_map;
			std::unordered_map<std::string, std::unordered_map<std::string, void(*)(const std::shared_ptr<MonoBehaviour>&, float)>> m_function_float_name_map;
		};


		// ==============================================
		// private methods
		// ==============================================
		// ���ꂼ����s�O��Priority�\�[�g���s��
		static void ExecuteStart();
		static void ExecuteUpdate();
		static void ExecuteLateUpdate();


		// ==============================================
		// private variables
		// ==============================================
		// GameObject�ɃA�^�b�`���ꂽMonoBehaviour�̃��X�g
		static std::unordered_map<GameObjectID, std::list<std::shared_ptr<MonoBehaviour>>> m_attached_monoBehaviours;
		// GameObject�ɃA�^�b�`���ꂽMonoBehaviour���N���X���Ō������邽�߂̃}�b�v
		static std::unordered_map<GameObjectID, std::unordered_map<std::string, std::shared_ptr<MonoBehaviour>>> m_monoBeahviour_access_name_list;
		// ���j�[�N��ID��U�邽�߂̌��݂̃C���f�b�N�X
		static int m_current_index;
		// �����OnEnable���Ă΂�Ă��Ȃ����̃��X�g
		static std::unordered_map<int, std::shared_ptr<MonoBehaviour>> m_store_list;
		// Start���Ăԗ\��̃��X�g�ɕύX����������
		static bool m_start_list_changed;
		static std::list<std::shared_ptr<MonoBehaviour>> m_start_list;
		// Update���Ăԗ\��̃��X�g�ɕύX����������
		static bool m_update_list_changed;
		static std::list<std::shared_ptr<MonoBehaviour>> m_update_list;
		
		// �A�j���[�V�����C�x���g�Ǘ��p�}�l�[�W��
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