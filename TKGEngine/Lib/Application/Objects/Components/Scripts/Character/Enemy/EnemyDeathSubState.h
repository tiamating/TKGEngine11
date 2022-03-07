#pragma once
#include "Components/inc/CMonoBehaviour.h"
#include "../ICharacterSubState.h"

namespace TKGEngine
{
	class EnemyReactionState;
	class Animator;
	class RigidBody;

	class EnemyDeathSubState
		: public TKGEngine::MonoBehaviour
		, public ICharacterSubState
	{
	public:
		EnemyDeathSubState() = default;
		virtual ~EnemyDeathSubState() override = default;
		EnemyDeathSubState(const EnemyDeathSubState&) = delete;
		EnemyDeathSubState& operator=(const EnemyDeathSubState&) = delete;

	private:
		virtual void Awake() override;
		virtual void Start() override;

		virtual void Enter() override;
		virtual void BeforeExecute() override;
		virtual void AfterExecute() override;
		virtual void Execute() override;
		virtual void Exit() override;

		//////////////////////////////////
		// functions
		//////////////////////////////////


		//////////////////////////////////
		// variables
		//////////////////////////////////
		// 参照
		SWPtr<EnemyReactionState> m_parent_state;
		SWPtr<Animator> m_animator;
		SWPtr<RigidBody> m_rigidbody;


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
CEREAL_CLASS_VERSION(TKGEngine::EnemyDeathSubState, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::EnemyDeathSubState, "TKGEngine::EnemyDeathSubState")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::EnemyDeathSubState)
