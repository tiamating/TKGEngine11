
#include "../inc/Object.h"
#include "../Managers/ObjectManager.h"

#include "Systems/inc/TKGEngine_Defined.h"

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	Object::Object(OBJECT_TYPE type)
		: m_object_type(type)
	{
		// Attach InstanceID
		m_instance_id = ObjectManager::AttachID();
		m_is_exist = true;
	}

	Object::~Object()
	{
		/* nothing */
	}

	void Object::Destroy(const std::shared_ptr<Object>& obj, float delay)
	{
		if (!obj->m_is_exist)
			return;

		ObjectManager::Destroy(obj->GetInstanceID(), delay);
	}

	void Object::Destroy(InstanceID object_id, float delay)
	{
		const auto s_ptr = ObjectManager::GetInstance(object_id).lock();
		if (!s_ptr)
			return;

		Object::Destroy(s_ptr, delay);
	}


}// namespace TKGEngine