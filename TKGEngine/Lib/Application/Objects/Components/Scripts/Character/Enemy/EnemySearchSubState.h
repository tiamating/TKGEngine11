#pragma once
#include "Components/inc/CMonoBehaviour.h"
#include "../ICharacterSubState.h"

namespace TKGEngine
{
	class EnemyActionState;
	class Animator;
	class RigidBody;

	class EnemySearchSubState
		: public MonoBehaviour
		, public ICharacterSubState
	{
	public:
		EnemySearchSubState() = default;
		virtual ~EnemySearchSubState() override = default;
		EnemySearchSubState(const EnemySearchSubState&) = delete;
		EnemySearchSubState& operator=(const EnemySearchSubState&) = delete;

	private:
		/////////////////////////////////////////////////////////////////////////
		// 使用するものをコメント解除して使用
		/////////////////////////////////////////////////////////////////////////
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
		SWPtr<EnemyActionState> m_parent_state;
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
CEREAL_CLASS_VERSION(TKGEngine::EnemySearchSubState, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::EnemySearchSubState, "TKGEngine::EnemySearchSubState")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::EnemySearchSubState)
