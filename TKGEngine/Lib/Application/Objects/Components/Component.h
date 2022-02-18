#pragma once

#include "../../Objects/inc/Object.h"

#include "Managers/ComponentManager.h"

namespace TKGEngine
{
	class IGameObject;
	class Transform;
}

namespace TKGEngine
{
	/// <summary>
	/// Component type.
	/// </summary>
	enum class COMPONENT_TYPE
	{
		COMPONENT_GENERAL = 0,
		COMPONENT_COLLIDER,
		COMPONENT_SCRIPT
	};

	/// <summary>
	/// Component Base module
	/// </summary>
	class Component
		: public Object
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Component(const Component&) = delete;
		Component& operator=(const Component&) = delete;
		
		// serialize用のデフォルトコンストラクタなので使用しない
		Component();
		// 基本的にはこちらを使用
		Component(COMPONENT_TYPE type);
		virtual ~Component();

		const char* GetName() override;

		virtual std::shared_ptr<IGameObject> GetGameObject();
		std::shared_ptr<Transform> GetTransform() const;

		// コンポーネントの種類判別用ID
		void SetComponentTypeID(ComponentTypeID id);
		inline ComponentTypeID GetComponentTypeID() const;
		// コンポーネントの種類
		inline COMPONENT_TYPE GetComponentType() const;

		// 作成時にのみ呼ばれる
		void SetOwnerIDAndTransform(GameObjectID id);
		inline GameObjectID GetOwnerID() const;

		// コンポーネントがアタッチされているか
		template<class T>
		bool HasComponent();
		// コンポーネントの取得
		template<class T>
		std::shared_ptr<T> GetComponent();


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	protected:
		// ==============================================
		// protected methods
		// ==============================================
		void SetTransform(const std::weak_ptr<Transform>& p_transform);


	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					cereal::base_class<Object>(this),
					CEREAL_NVP(m_component_type),
					CEREAL_NVP(m_transform)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
		/* nothing */


		// ==============================================

		// private variables
		// ==============================================
		COMPONENT_TYPE m_component_type = COMPONENT_TYPE::COMPONENT_GENERAL;
		ComponentTypeID m_type_id = 0;
		GameObjectID m_owner_id = 0;

		std::weak_ptr<Transform> m_transform;

	};

	// ----------------------------------------
	// inline
	// ----------------------------------------
	inline ComponentTypeID Component::GetComponentTypeID() const
	{
		return m_type_id;
	}

	inline COMPONENT_TYPE Component::GetComponentType() const
	{
		return m_component_type;
	}

	inline GameObjectID Component::GetOwnerID() const
	{
		return m_owner_id;
	}

	inline void Component::SetTransform(const std::weak_ptr<Transform>& p_transform)
	{
		m_transform = p_transform;
	}

	// ----------------------------------------
	// template
	// ----------------------------------------
	template<class T>
	bool Component::HasComponent()
	{
		return ComponentManager::HasComponent<T>(m_owner_id);
	}

	template<class T>
	std::shared_ptr<T> Component::GetComponent()
	{
		return ComponentManager::GetComponent<T>(m_owner_id);
	}

}// namespace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::Component, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::Component, "TKGEngine::Component")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Object, TKGEngine::Component)