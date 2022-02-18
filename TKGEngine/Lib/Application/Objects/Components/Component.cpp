
#include "Component.h"

#include "../../Objects/inc/IGameObject.h"
#include "Components/inc/CTransform.h"
#include "Managers/GameObjectManager.h"

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	Component::Component()
		: Object(OBJECT_TYPE::OBJECT_COMPONENT)
	{
		/* nothing */
	}

	Component::Component(COMPONENT_TYPE type)
		: Object(OBJECT_TYPE::OBJECT_COMPONENT)
	{
		m_component_type = type;
	}

	Component::~Component()
	{
		/* nothing */
	}

	const char* Component::GetName()
	{
		return GetGameObject()->GetName();
	}

	std::shared_ptr<IGameObject> Component::GetGameObject()
	{
		return GameObjectManager::GetGameObject(m_owner_id);
	}

	std::shared_ptr<Transform> Component::GetTransform() const
	{
		auto s_ptr = m_transform.lock();
		assert(s_ptr != nullptr);
		if (s_ptr == nullptr)
			return std::shared_ptr<Transform>();
		return s_ptr;
	}

	void Component::SetComponentTypeID(ComponentTypeID id)
	{
		m_type_id = id;
	}

	void Component::SetOwnerIDAndTransform(GameObjectID id)
	{
		m_owner_id = id;
		const auto s_ptr = GameObjectManager::GetGameObject(id);
		if (s_ptr == nullptr)
		{
			return;
		}
		m_transform = s_ptr->GetTransform();
	}

}// namespace TKGEngine