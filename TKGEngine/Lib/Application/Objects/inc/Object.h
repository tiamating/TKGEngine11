#pragma once

#include "Systems/inc/TKGEngine_Defined.h"

#include <string>
#include <memory>


namespace TKGEngine
{
	/// <summary>
	/// Object Type
	/// </summary>
	enum class OBJECT_TYPE
	{
		OBJECT_GAMEOBJECT = 0,
		OBJECT_COMPONENT,
		//OBJECT_ASSET,
	};


	/// <summary>
	/// All Object Base module
	/// </summary>
	class Object
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Object(const Object&) = delete;
		Object& operator=(const Object&) = delete;

		Object(OBJECT_TYPE type);
		virtual ~Object();

#ifdef USE_IMGUI
		virtual void OnGUI() = 0;

		// ギズモの選択時に各コンポーネントのものが呼ばれる
		virtual void OnGizmoEnter() {}
		virtual void OnGizmoStay() {}
		virtual void OnGizmoExit() {}
#endif // USE_IMGUI

		virtual void OnCreate(const std::shared_ptr<Object>& p_self) = 0;

		// When this item destroied, call this func.
		// 派生先の破棄時に呼ばれる処理を書く
		// Object::OnDestroyはObjectManager::Destroyで使用する
		virtual void OnDestroy();

		inline bool IsExist() const;

		InstanceID GetInstanceID() const;
		virtual const char* GetName();
		void SetName(const std::string& name);

		static void Destroy(const std::shared_ptr<Object>& obj, float delay = 0.0f);
		static void Destroy(InstanceID object_id, float delay = 0.0f);


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	protected:
		// ==============================================
		// protected methods
		// ==============================================
		/* nothing */


		// ==============================================
		// protected variables
		// ==============================================
		/* nothing */


	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(m_object_type),
					CEREAL_NVP(m_name)
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
		OBJECT_TYPE m_object_type;
		InstanceID m_instance_id = 0;
		std::string m_name;

		bool m_is_exist = false;	// オブジェクトのOnDestroyが呼ばれたらfalseになる
	};

	// ----------------------------------------
	// inline
	// ----------------------------------------
	inline void Object::OnDestroy()
	{
		m_is_exist = false;
	}

	inline bool Object::IsExist() const
	{
		return m_is_exist;
	}

	inline InstanceID Object::GetInstanceID() const
	{
		return m_instance_id;
	}

	inline const char* Object::GetName()
	{
		return m_name.c_str();
	}

	inline void Object::SetName(const std::string& name)
	{
		m_name = name;
	}

}// namespace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::Object, 1)