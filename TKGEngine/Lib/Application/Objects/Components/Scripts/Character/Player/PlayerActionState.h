#pragma once
#include "Components/inc/CMonoBehaviour.h"
#include "../ICharacterState.h"

namespace TKGEngine
{
	class PlayerActionState
		: public MonoBehaviour
		, public ICharacterState
	{
	public:
		PlayerActionState()
			: ICharacterState(static_cast<int>(ActionState::Max_ActionState)){}
		virtual ~PlayerActionState() override = default;
		PlayerActionState(const PlayerActionState&) = delete;
		PlayerActionState& operator=(const PlayerActionState&) = delete;

	private:
		virtual void Awake() override;
		virtual void Start() override;

		virtual void Enter() override;
		virtual void BeforeExecute() override;
		virtual void Execute() override;
		virtual void AfterExecute() override;
		virtual void Exit() override;

	public:
		// アクションステート内のサブステートの種類
		enum class ActionState
		{
			Explore,	// 探索
			Slide,		// スライディング
			Aim,		// エイム中
			Cover,		// カバーアクション
			Melee,		// 近接攻撃
			Climb,		// ブロックに乗る
			ClimbOver,	// 壁の乗り越え
			Fall,		// 落下中
			Landing,	// 着地時

			Max_ActionState
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
CEREAL_CLASS_VERSION(TKGEngine::PlayerActionState, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::PlayerActionState, "TKGEngine::PlayerActionState")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::PlayerActionState)
