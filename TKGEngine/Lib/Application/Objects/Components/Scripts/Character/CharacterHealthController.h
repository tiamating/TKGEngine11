#pragma once
#include "Components/inc/CMonoBehaviour.h"

namespace TKGEngine
{
	class CharacterHealthController
		: public MonoBehaviour
	{
	public:
		CharacterHealthController() = default;
		virtual ~CharacterHealthController() override = default;
		CharacterHealthController(const CharacterHealthController&) = delete;
		CharacterHealthController& operator=(const CharacterHealthController&) = delete;


		//////////////////////////////////
		// functions
		//////////////////////////////////
		// 最大体力を取得
		int GetMaxHealth() const { return m_max_health; }
		// 現在の体力
		int GetHealth() const { return m_health; }
		// 生存しているか
		bool IsAlive() const { return m_health > 0; }
		// 無敵中か
		bool IsInvincible() const { return m_is_invincible; }

		// ダメージ処理
		bool ApplyDamage(int damage);


	private:
		/////////////////////////////////////////////////////////////////////////
		// 使用するものをコメント解除して使用
		/////////////////////////////////////////////////////////////////////////
		virtual void Awake() override;
		virtual void Update() override;


		//////////////////////////////////
		// variables
		//////////////////////////////////

		// 体力
		int m_max_health = 5;
		int m_health = 0;

		// 無敵状態
		float m_invincible_timer = 0.0f;
		float m_invincible_time = 0.1f;
		bool m_is_invincible = false;


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
CEREAL_CLASS_VERSION(TKGEngine::CharacterHealthController, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::CharacterHealthController, "TKGEngine::CharacterHealthController")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::CharacterHealthController)
