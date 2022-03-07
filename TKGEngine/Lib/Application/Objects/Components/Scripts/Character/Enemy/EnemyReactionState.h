#pragma once
#include "Components/inc/CMonoBehaviour.h"
#include "../ICharacterState.h"

namespace TKGEngine
{
	class EnemyReactionState
		: public TKGEngine::MonoBehaviour
		, public ICharacterState
	{
	public:
		EnemyReactionState()
			: ICharacterState(static_cast<int>(ReactionState::Max_ReactionState)) {}
		virtual ~EnemyReactionState() override = default;
		EnemyReactionState(const EnemyReactionState&) = delete;
		EnemyReactionState& operator=(const EnemyReactionState&) = delete;

	public:
		// アクションステート内のサブステートの種類
		enum class ReactionState
		{
			Damage,
			Death,

			Max_ReactionState
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
CEREAL_CLASS_VERSION(TKGEngine::EnemyReactionState, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::EnemyReactionState, "TKGEngine::EnemyReactionState")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::EnemyReactionState)
