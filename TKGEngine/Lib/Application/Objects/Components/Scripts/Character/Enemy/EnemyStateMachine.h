#pragma once
#include "Components/inc/CMonoBehaviour.h"
#include "../ICharacterStateMachine.h"

namespace TKGEngine
{
	class EnemyStateMachine
		: public TKGEngine::MonoBehaviour
		, public ICharacterStateMachine
	{
	public:
		EnemyStateMachine()
			: ICharacterStateMachine(static_cast<int>(EnemyState::Max_EnemyState)) {}
		virtual ~EnemyStateMachine() override = default;
		EnemyStateMachine(const EnemyStateMachine&) = delete;
		EnemyStateMachine& operator=(const EnemyStateMachine&) = delete;

	private:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void Update() override;

	public:
		enum class EnemyState
		{
			Action,
			Reaction,

			Max_EnemyState
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
CEREAL_CLASS_VERSION(TKGEngine::EnemyStateMachine, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::EnemyStateMachine, "TKGEngine::EnemyStateMachine")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::EnemyStateMachine)
