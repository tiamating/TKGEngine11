
#include "../inc/CMonoBehaviour.h"

#include "Systems/inc/IGUI.h"

namespace TKGEngine
{
	MonoBehaviour::MonoBehaviour()
		: Behaviour(COMPONENT_TYPE::COMPONENT_SCRIPT)
	{
		/* nothing */
	}

	MonoBehaviour::~MonoBehaviour()
	{
		/* nothing */
	}

#ifdef USE_IMGUI
	void MonoBehaviour::OnGUI()
	{
		ImGui::IDWrapped mono_behaviour_id(this);
		Behaviour::OnGUI();
		// Update関数を呼ぶかどうか
		ImGui::Checkbox("Call Update", &m_can_call_every_frame_update);
		ImGui::SameLine();
		ImGui::HelpMarker("True : Can call every frame update function.\nFalse : Not call update function.");
		// 実行優先順
		ImGui::Text("Priority");
		ImGui::AlignedSameLine(0.6f);
		ImGui::InputInt("##Script Priority", &m_priority, 1, 100);
		ImGui::Separator();
	}
#endif // USE_IMGUI

	void MonoBehaviour::CheckMonoBehaviourState(MonoBehaviourEventType type)
	{
		switch (m_state)
		{
			// まだAwakeが呼ばれていない状態
			case MonoBehaviourStandByState::BeforeAwake:
			{
				// 現在呼ばれているイベント関数
				switch (type)
				{
					case MonoBehaviourEventType::OnAwake:
						m_state = MonoBehaviourStandByState::BeforeEnable;
						break;
					case MonoBehaviourEventType::OnEnable:
						// Awakeが呼ばれていないので呼んでから状態を変更する
						Awake();
						m_state = MonoBehaviourStandByState::BeforeStart;
						MonoBehaviourManager::AddStartList(m_store_list_id);
						break;
					case MonoBehaviourEventType::OnStart:
						// Awakeが呼ばれていない状態ではStartListに存在しないので呼ばれるはずがない
						assert(0 && "falied called function.");
						break;
				}
			}
			break;

			// まだOnEnableが呼ばれていない状態
			case MonoBehaviourStandByState::BeforeEnable:
			{
				switch (type)
				{
					case MonoBehaviourEventType::OnAwake:
						/* nothing */
						break;
					case MonoBehaviourEventType::OnEnable:
						m_state = MonoBehaviourStandByState::BeforeStart;
						MonoBehaviourManager::AddStartList(m_store_list_id);
						break;
					case MonoBehaviourEventType::OnStart:
						// OnEnableが呼ばれていない状態ではStartListに存在しないので呼ばれるはずがない
						assert(0 && "falied called function.");
						break;
				}
			}
			break;

			// まだStartが呼ばれていない状態
			case MonoBehaviourStandByState::BeforeStart:
			{
				switch (type)
				{
					case MonoBehaviourEventType::OnAwake:
						/* nothing */
						break;
					case MonoBehaviourEventType::OnEnable:
						/* nothing */
						break;
					case MonoBehaviourEventType::OnStart:
						m_state = MonoBehaviourStandByState::AllReady;
						break;
				}
			}
			break;

			// UpdateListに含まれている状態
			case MonoBehaviourStandByState::AllReady:
			{
				switch (type)
				{
					case MonoBehaviourEventType::OnAwake:
						assert(0 && "falied called function.");
						break;
					case MonoBehaviourEventType::OnEnable:
						break;
					case MonoBehaviourEventType::OnStart:
						assert(0 && "falied called function.");
						break;
				}
			}
			break;
		}
	}

	void MonoBehaviour::OnCreate(const std::shared_ptr<Object>& p_self)
	{
		// Register MonoBehaviour Manager
		MonoBehaviourManager::RegisterMonoBehaviour(GetOwnerID(), std::static_pointer_cast<MonoBehaviour>(p_self));
	}

	void MonoBehaviour::OnDestroy()
	{
		MonoBehaviourManager::UnregisterMonoBehaviour(GetOwnerID(), GetInstanceID());
	}

	void MonoBehaviour::OnEnabled()
	{
#ifdef USE_IMGUI
		// エディタ時は何もしない
		if (!IGUI::Get().IsPlaying())
			return;
#endif // USE_IMGUI

		const auto s_ptr = GetGameObject();
		if (s_ptr->GetActiveHierarchy() && IsActiveScene())
		{
			CheckMonoBehaviourState(MonoBehaviourEventType::OnEnable);
			OnEnable();
		}
	}

	void MonoBehaviour::OnDisabled()
	{
#ifdef USE_IMGUI
		// エディタ時は何もしない
		if (!IGUI::Get().IsPlaying())
			return;
#endif // USE_IMGUI

		const auto s_ptr = GetGameObject();
		if (s_ptr->GetActiveHierarchy() && IsActiveScene())
		{
			OnDisable();
		}
	}

}// namespace TKGEngine