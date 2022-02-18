
#include "../inc/CBehaviour.h"

#include "Components/inc/CTransform.h"
#include "Application/Objects/inc/IGameObject.h"
#include "Application/Objects/Managers/SceneManager.h"


namespace TKGEngine
{
	Behaviour::Behaviour(COMPONENT_TYPE type)
		: Component(type)
	{
		/* nothing */
	}

	Behaviour::~Behaviour()
	{
		/* nothing */
	}

#ifdef USE_IMGUI
	void Behaviour::OnGUI()
	{
		ImGui::PushID(this);
		bool is_enable = m_enabled;
		if (ImGui::Checkbox("Enable", &is_enable))
		{
			Enabled(is_enable);
		}
		ImGui::Separator();
		ImGui::PopID();
	}
#endif // USE_IMGUI

	void Behaviour::Enabled(const bool is_enable)
	{
		// 新しい状態をセット
		m_enabled = is_enable;

		// GameObjectの状態をチェック
		const auto s_ptr = GetGameObject();
		if (s_ptr == nullptr)
		{
			LOG_ASSERT("invalid shared pointer. Behaviour::Enabled()");
			return;
		}

		// GameObjectか、シーンが非アクティブなら関数を呼ばない
		if (!s_ptr->GetActiveHierarchy() || !SceneManager::GetActive(s_ptr->GetScene()))
			return;
		is_enable == true ? OnEnabled() : OnDisabled();

	}

	bool Behaviour::IsActiveAndEnabled()
	{
		const auto s_ptr = GetGameObject();
		if (s_ptr == nullptr)
		{
			LOG_ASSERT("invalid shared pointer. Behaviour::IsActiveAndEnabled()");
			return false;
		}
		return m_enabled && s_ptr->GetActiveHierarchy();
	}

	bool Behaviour::IsActiveScene()
	{
		const auto s_ptr = GetGameObject();
		if (s_ptr == nullptr)
		{
			LOG_ASSERT("invalid shared pointer. Behaviour::IsActiveScene()");
			return false;
		}
		return SceneManager::GetActive(s_ptr->GetScene());
	}

	bool Behaviour::IsActiveBehaviour()
	{
		const auto s_ptr = GetGameObject();
		if (s_ptr == nullptr)
		{
			LOG_ASSERT("invalid shared pointer. Behaviour::IsActiveBehaviour()");
			return false;
		}
		return m_enabled && s_ptr->GetActiveHierarchy() && SceneManager::GetActive(s_ptr->GetScene());
	}

}// namespace TKGEngine