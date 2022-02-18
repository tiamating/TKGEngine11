#pragma once

#include "../Component.h"

#include <memory>

namespace TKGEngine
{
	/// <summary>
	/// This component can change a component is enable or disable.
	/// </summary>
	class Behaviour
		: public Component
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Behaviour(const Behaviour&) = delete;
		Behaviour& operator=(const Behaviour&) = delete;

		Behaviour(COMPONENT_TYPE type);
		virtual ~Behaviour();

#ifdef USE_IMGUI
		virtual void OnGUI() override;
#endif // USE_IMGUI

		inline bool Enabled() const;
		void Enabled(bool is_enable);

		/// <summary>
		/// Check GameObject::Active and Behaviour::Enable
		/// </summary>
		bool IsActiveAndEnabled();
		/// <summary>
		/// Check Scene::Active
		/// </summary>
		bool IsActiveScene();
		/// <summary>
		/// Check GameObject::Active, Behaviour::Enable and Scene::Active
		/// </summary>
		bool IsActiveBehaviour();


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	protected:
		// ==============================================
		// protected methods
		// ==============================================
		
		// Enabled()ì‡Ç≈åƒÇŒÇÍÇÈîhê∂êÊÇ≤Ç∆ÇÃèàóù
		// çÏê¨éûÇ…ÇÕåƒÇŒÇÍÇ∏ÅAïœçXéûÇ…ÇÃÇ›åƒÇŒÇÍÇÈ
		virtual void OnEnabled() = 0;
		virtual void OnDisabled() = 0;

	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					cereal::base_class<Component>(this),
					CEREAL_NVP(m_enabled)
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
		bool m_enabled = true;


	};

	// -----------------------------------------------
	// inline
	// -----------------------------------------------
	inline bool Behaviour::Enabled() const
	{
		return m_enabled;
	}



}// namespace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::Behaviour, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::Behaviour, "TKGEngine::Behaviour")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Component, TKGEngine::Behaviour)