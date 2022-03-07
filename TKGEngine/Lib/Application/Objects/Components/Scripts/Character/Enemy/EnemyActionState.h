#pragma once
#include "Components/inc/CMonoBehaviour.h"
#include "../ICharacterState.h"

namespace TKGEngine
{
	class EnemyStateMachine;

	class EnemyActionState
		: public TKGEngine::MonoBehaviour
		, public ICharacterState
	{
	public:
		EnemyActionState()
			: ICharacterState(static_cast<int>(ActionState::Max_ActionState)) {}
		virtual ~EnemyActionState() override = default;
		EnemyActionState(const EnemyActionState&) = delete;
		EnemyActionState& operator=(const EnemyActionState&) = delete;

	public:
		// アクションステート内のサブステートの種類
		enum class ActionState
		{
			Search,
			Attack,
			AttackIdle,

			Max_ActionState
		};

	private:
		/////////////////////////////////////////////////////////////////////////
		// 使用するものをコメント解除して使用
		/////////////////////////////////////////////////////////////////////////
		virtual void Awake() override;
		virtual void Start() override;

		virtual void Enter() override;
		virtual void BeforeExecute() override;
		virtual void Execute() override;
		virtual void AfterExecute() override;
		virtual void Exit() override;


		//////////////////////////////////
		// functions
		//////////////////////////////////


		//////////////////////////////////
		// variables
		//////////////////////////////////
		// 参照
		std::weak_ptr<EnemyStateMachine> m_state_machine;

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
CEREAL_CLASS_VERSION(TKGEngine::EnemyActionState, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::EnemyActionState, "TKGEngine::EnemyActionState")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::EnemyActionState)
