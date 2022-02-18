#pragma once

#include "ObjectManager.h"

#include "Systems/inc/LogSystem.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <vector>

#include <typeinfo>
#include <utility>
#include <functional>
#include <memory>
#include <cassert>
#include <type_traits>
#include <mutex>


namespace TKGEngine
{
	class Component;

	/// <summary>
	/// Component manager. (static class)
	/// </summary>
	class ComponentManager
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		ComponentManager() = default;
		virtual ~ComponentManager() = default;
		ComponentManager(const ComponentManager&) = delete;
		ComponentManager& operator=(const ComponentManager&) = delete;

		static void SetUpComponentTypeID();

#ifdef USE_IMGUI
		static void OnGUI(GameObjectID goid);
		static void OnGUIAddComponent(GameObjectID goid);
		static void OnGUIRemoveComponent(GameObjectID goid);
#endif // USE_IMGUI

		// Component���A�^�b�`����Ă��邩
		template<class T>
		static bool HasComponent(GameObjectID owner_id);

		// Component�ǉ�
		template<class T>
		static std::shared_ptr<T> AddComponent(GameObjectID owner_id);
		static std::shared_ptr<Component> AddComponent(ComponentTypeID type_id, GameObjectID owner_id);
		static std::shared_ptr<Component> AddComponent(const std::string& component_name, GameObjectID owner_id);
		// Serialize���ɂ̂ݎg�p����
		static void AddComponentOnSerialize(GameObjectID owner_id, std::shared_ptr<Component>& component);

		// Component�擾
		template<class T>
		static std::shared_ptr<T> GetComponent(GameObjectID owner_id);
		static std::vector<std::weak_ptr<Component>> GetComponents(GameObjectID owner_id);
		static void GetComponents(GameObjectID owner_id, std::vector<std::weak_ptr<Component>>& vec);

		// Component�폜
		template<class T>
		static void RemoveComponent(GameObjectID owner_id);
		static void RemoveComponent(const std::string& component_type_name/* "class xxx::yyy" */, GameObjectID owner_id);
		static void RemoveAllComponent(GameObjectID owner_id);

		// GameObjectID����A�^�b�`���ꂽComponent�����X�g���擾����
		static std::list<std::string>& GetComponentNameList(GameObjectID owner_id);


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================
		static void RegisterComponentType();

		// ==============================================
		// private variables
		// ==============================================

		// <typeid().name(), TypeID = �n�b�V���l>
		static std::unordered_map<std::string, ComponentTypeID> m_typeName_id_index;
		// <component name, TypeID>
		static std::unordered_map<std::string, ComponentTypeID> m_componentName_id_index;
		// <TypeID, typeid().name()
		static std::unordered_map<ComponentTypeID, std::string> m_componentTypeID_typeName_index;

		// <ID, AddComponent Lambda>
		static std::unordered_map<ComponentTypeID, std::function<std::shared_ptr<Component>(GameObjectID)>> m_add_functions;
		// GameObject�ɃA�^�b�`���ꂽ�R���|�[�l���g��map
		static std::unordered_map<GameObjectID, std::unordered_map<std::string, std::weak_ptr<Component>>> m_gameObject_components_typename_index;
		// GameObject�ɃA�^�b�`���ꂽ�R���|�[�l���g�����X�g(���Ԉێ��p)
		static std::unordered_map<GameObjectID, std::list<std::string>> m_gameObject_attached_components_list;

		static const std::string m_transform_class_name;
		static const std::string m_transform_type_name;
		static std::mutex m_mutex;
	};

	// ------------------------------------------
	// template
	// ------------------------------------------
	template<class T>
	bool ComponentManager::HasComponent(GameObjectID owner_id)
	{
		static_assert(std::is_base_of_v<Component, T>, "Type is not base of Component.");
		std::lock_guard<std::mutex> lock(m_mutex);
		// �T��
		const std::string type_name(typeid(T).name());
		auto&& component_index = m_gameObject_components_typename_index.at(owner_id);
		const auto itr = component_index.find(type_name);
		// �A�^�b�`����Ă��Ȃ�
		if (itr == component_index.end())
			return false;
		return true;
	}

	template<class T>
	std::shared_ptr<T> ComponentManager::AddComponent(GameObjectID owner_id)
	{
		static_assert(std::is_base_of_v<Component, T>, "Type is not base of Component.");
		m_mutex.lock();

		//����R���|�[�l���g�̃A�^�b�`���ł��Ȃ��悤�Ɍ������āA���ɑ��݂����炻���Ԃ�
		const std::string type_name(typeid(T).name());
		auto&& go_components_name_index = m_gameObject_components_typename_index[owner_id];
		{
			const auto itr = go_components_name_index.find(type_name);

			// ���������ꍇ�͂����Ԃ�
			if (itr != go_components_name_index.end())
			{
				const std::shared_ptr<Component> s_ptr = itr->second.lock();
				if (s_ptr == nullptr)
				{
					go_components_name_index.erase(itr);
				}
				else
				{
					m_mutex.unlock();
					return std::static_pointer_cast<T>(s_ptr);
				}
			}
		}
		// Component��REGISTERCOMPONENT�ɂ���ēo�^����Ă��邩
		const auto itr = m_typeName_id_index.find(type_name);
		if (itr == m_typeName_id_index.end())
		{
			m_mutex.unlock();
			LOG_ASSERT("The component (%s) is not registered component list.", type_name.c_str());
			return std::shared_ptr<T>();
		}
		// Component���̐���
		std::shared_ptr<T> s_ptr = std::make_shared<T>();
		// Component��GameObject�ƃR���|�[�l���gID�̃Z�b�g
		s_ptr->SetComponentTypeID(itr->second);
		s_ptr->SetOwnerIDAndTransform(owner_id);
		// GameObject�ɃA�^�b�`���ꂽ�R���|�[�l���g�����X�g�ɒǉ�
		m_gameObject_attached_components_list[owner_id].emplace_back(type_name);
		// Object��Component�̃}�l�[�W���ɓo�^
		ObjectManager::Set(s_ptr);
		go_components_name_index.emplace(type_name, s_ptr);
		m_mutex.unlock();
		// �e�R���|�[�l���g�̏����쐬�֐������s
		std::static_pointer_cast<Object>(s_ptr)->OnCreate(s_ptr);
		return s_ptr;
	}

	template<class T>
	std::shared_ptr<T> ComponentManager::GetComponent(GameObjectID owner_id)
	{
		static_assert(std::is_base_of_v<Component, T>, "Type is not base of Component.");
		std::lock_guard<std::mutex> lock(m_mutex);
		// �R���|�[�l���g����
		const std::string type_name(typeid(T).name());
		auto&& component_index = m_gameObject_components_typename_index.at(owner_id);
		const auto itr = component_index.find(type_name);

		// �A�^�b�`����Ă��Ȃ��Ȃ�null
		if (itr == component_index.end())
		{
			return std::shared_ptr<T>();
		}

		const std::shared_ptr<Component> s_ptr = itr->second.lock();
		if (s_ptr == nullptr)
		{
			component_index.erase(itr);
			return std::shared_ptr<T>();
		}

		std::shared_ptr<T> casted_s_ptr = std::static_pointer_cast<T>(s_ptr);
		return casted_s_ptr;
	}

	template<class T>
	void ComponentManager::RemoveComponent(GameObjectID owner_id)
	{
		static_assert(std::is_base_of_v<Component, T>, "Type is not base of Component.");
		m_mutex.lock();

		const std::string type_name(typeid(T).name());
		// Transform�͍폜�ł��Ȃ�
		if (type_name == m_transform_type_name)
		{
			m_mutex.unlock();
			LOG_ASSERT("Transform component can not remove!");
			return;
		}
		// owner_id����GameObject�̂��R���|�[�l���g���X�g���擾
		auto&& component_index = m_gameObject_components_typename_index.at(owner_id);
		const auto itr = component_index.find(type_name);

		// �A�^�b�`����Ă��Ȃ���Α������^�[��
		if (itr == component_index.end())
		{
			m_mutex.unlock();
			return;
		}
		// Component��Owner����폜
		std::shared_ptr<Component> s_ptr = itr->second.lock();
		if (!s_ptr)
		{
			component_index.erase(itr);
		}
		// Component����Owner����폜
		{
			auto& name_list = m_gameObject_attached_components_list.at(owner_id);
			const auto itr_name_find = std::find(name_list.begin(), name_list.end(), type_name);
			if (itr_name_find != name_list.end())
			{
				name_list.erase(itr_name_find);
			}
		}
		m_mutex.unlock();
		ObjectManager::Destroy(std::static_pointer_cast<Object>(s_ptr)->GetInstanceID(), -1.0f);

		m_mutex.lock();
		component_index.erase(itr);
		m_mutex.unlock();
	}

}// namespace TKGEngine

	/// <summary>
	/// Component Register module
	/// </summary>
namespace Preprocess
{
	class RegisterModule
	{
	public:
		struct ComponentRegisteredData
		{
			ComponentRegisteredData(const std::string& type_name, const std::function<std::shared_ptr<TKGEngine::Component>(TKGEngine::GameObjectID)>& add_func)
				: type_name(type_name), add_func(add_func)
			{
				/* nothing */
			}
			std::string type_name;
			std::function<std::shared_ptr<TKGEngine::Component>(TKGEngine::GameObjectID)> add_func;
		};

		RegisterModule()
		{
			CheckAndCreateMap();
		}
		virtual ~RegisterModule() = default;
		RegisterModule(const RegisterModule&) = delete;
		RegisterModule& operator=(const RegisterModule&) = delete;

		// �o�^����Ă��邩�ǂ����̃`�F�b�N������
		static bool CheckRegistered(const std::string& name)
		{
			// map����������Ă��Ȃ��Ȃ�ΐ�������
			CheckAndCreateMap();

			// �܂��o�^����Ă��Ȃ��Ȃ�false��Ԃ�
			if (m_component_function_index->find(name) == m_component_function_index->end())
			{
				return false;
			}
			return true;
		}

		// AddComponent���s�����ׂẴR���|�[�l���g��o�^����map
		static std::unique_ptr<std::unordered_map<std::string, ComponentRegisteredData>> m_component_function_index;

	private:
		// map����������Ă��Ȃ��Ȃ�ΐ�������
		static void CheckAndCreateMap()
		{
			if (m_component_function_index == nullptr)
			{
				m_component_function_index = std::make_unique<std::unordered_map<std::string, ComponentRegisteredData>>();
			}
		}
	};

	// �e�R���|�[�l���g��AddComponent�֐���o�^����e���v���[�g
	template<class T>
	class Register
		: public RegisterModule
	{
	public:
		Register(const std::string& name, const std::string& type_name, const std::function<std::shared_ptr<TKGEngine::Component>(TKGEngine::GameObjectID)>& func)
		{
			if (RegisterModule::CheckRegistered(name))
				return;

			m_component_function_index->emplace(name, ComponentRegisteredData(type_name, func));
		}
		virtual ~Register() = default;
		Register(const Register&) = delete;
		Register& operator=(const Register&) = delete;
	};
}


#define REGISTERCOMPONENT(TYPE) namespace Preprocess\
{\
	static Register<TYPE> Register_Data(\
		std::string(#TYPE),\
		std::string(typeid(TYPE).name()),\
		[](TKGEngine::GameObjectID goid)->std::shared_ptr<TKGEngine::Component>{\
			return TKGEngine::ComponentManager::AddComponent<TYPE>(goid);\
		}\
	);\
}
