#pragma once

#include "Object.h"
#include "Managers/ComponentManager.h"

#include "Application/inc/ProjectSetting.h"
#include "Systems/inc/TKGEngine_Defined.h"

#include <memory>
#include <vector>

namespace TKGEngine
{
	class Transform;
	class GameObject;
}

namespace TKGEngine
{
	/// <summary>
	/// Starting at 1. 0 is initial value.
	/// </summary>
	using GameObjectID = InstanceID;

	/// <summary>
	/// GameObject interface
	/// </summary>
	class IGameObject
		: public Object
	{
		friend Transform;
		friend GameObject;

	public:
		// ==============================================
		// public methods
		// ==============================================
		virtual ~IGameObject() = default;
		IGameObject(const IGameObject&) = delete;
		IGameObject& operator=(const IGameObject&) = delete;

		IGameObject();

		// !内部でのみ使用
		static IGameObject* CreateInterface();

		// GameObjectの生成
		static std::shared_ptr<IGameObject> Create();
		static std::shared_ptr<IGameObject> Create(const char* name);
		// GameObjectの複製
		static std::shared_ptr<IGameObject> Duplicate(const std::shared_ptr<IGameObject>& gameobject);
		// GameObjectの検索
		static std::shared_ptr<IGameObject> Find(const std::string& name);

#ifdef USE_IMGUI
		virtual void OnGUIHierarchy() = 0;
#endif // USE_IMGUI

		virtual void Destroy(float delay = 0.0f) = 0;

		virtual GameObjectID GetGameObjectID() const = 0;
		virtual std::shared_ptr<Transform> GetTransform() = 0;
		virtual void SetActive(bool is_active) = 0;
		virtual bool GetActiveSelf() const = 0;
		virtual bool GetActiveHierarchy() const = 0;

		virtual Tag GetTag() const = 0;
		virtual void SetTag(Tag tag) = 0;
		virtual Layer GetLayer() const = 0;
		virtual void SetLayer(Layer layer) = 0;
		virtual SceneID GetScene() const = 0;
		virtual void SetScene(SceneID scene_id) = 0;

		// Componentがアタッチされているか
		template<class T>
		bool HasComponent();
		// Component追加
		template<class T>
		std::shared_ptr<T> AddComponent();
		// Component取得
		template<class T>
		std::shared_ptr<T> GetComponent();
		inline std::vector<std::weak_ptr<Component>> GetComponents();
		inline void GetComponents(std::vector<std::weak_ptr<Component>>& vec);
		// Component削除
		template<class T>
		void RemoveComponent();

	protected:
		virtual void RecurseSetScene(SceneID scene_id) = 0;
		virtual void OnCreate(const std::shared_ptr<Object>& p_self) = 0;

		// SetActive時に変更をヒエラルキーに適用する
		virtual void ApplyActiveInHierarchy(const bool is_hierarchy_active, const bool is_scene_active) = 0;


	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					cereal::base_class<Object>(this)
				);
			}
		}
	};

	// ----------------------------------------
	// inline
	// ----------------------------------------
	inline std::vector<std::weak_ptr<Component>> IGameObject::GetComponents()
	{
		return ComponentManager::GetComponents(GetGameObjectID());
	}
	inline void IGameObject::GetComponents(std::vector<std::weak_ptr<Component>>& vec)
	{
		ComponentManager::GetComponents(GetGameObjectID(), vec);
	}

	// ----------------------------------------
	// template
	// ----------------------------------------
	template<class T>
	bool IGameObject::HasComponent()
	{
		return ComponentManager::HasComponent<T>(GetGameObjectID());
	}

	template<class T>
	inline std::shared_ptr<T> IGameObject::AddComponent()
	{
		return ComponentManager::AddComponent<T>(GetGameObjectID());
	}

	template<class T>
	inline std::shared_ptr<T> IGameObject::GetComponent()
	{
		return ComponentManager::GetComponent<T>(GetGameObjectID());
	}

	template<class T>
	inline void IGameObject::RemoveComponent()
	{
		ComponentManager::RemoveComponent<T>(GetGameObjectID());
	}

}// namespace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::IGameObject, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::IGameObject, "TKGEngine::IGameObject")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Object, TKGEngine::IGameObject)
