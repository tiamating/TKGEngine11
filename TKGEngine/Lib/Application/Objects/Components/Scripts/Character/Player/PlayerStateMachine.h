#pragma once
#include "Components/inc/CMonoBehaviour.h"
#include "../ICharacterStateMachine.h"

namespace TKGEngine
{
	class PlayerStateMachine
		: public MonoBehaviour
		, public ICharacterStateMachine
	{
	public:
		PlayerStateMachine()
			: ICharacterStateMachine(static_cast<int>(PlayerState::Max_PlayerState)){}
		virtual ~PlayerStateMachine() override = default;
		PlayerStateMachine(const PlayerStateMachine&) = delete;
		PlayerStateMachine& operator=(const PlayerStateMachine&) = delete;

	private:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Update() override;


	public:
		enum class PlayerState
		{
			Action,
			Reaction,

			Max_PlayerState
		};



	private:
		/////////////////////////////////////////////////////////////////////////
		// Serialize設定
		/////////////////////////////////////////////////////////////////////////
		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			if (version == 1)
			{
				archive(
					cereal::base_class<TKGEngine::MonoBehaviour>(this)
				);
			}
		}

		/////////////////////////////////////////////////////////////////////////
		// GUI設定
		/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
		void OnGUI() override;
#endif // USE_IMGUI
	};
}

/////////////////////////////////////////////////////////////////////////
// Serialize登録
/////////////////////////////////////////////////////////////////////////
CEREAL_CLASS_VERSION(TKGEngine::PlayerStateMachine, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::PlayerStateMachine, "TKGEngine::PlayerStateMachine")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::PlayerStateMachine)
