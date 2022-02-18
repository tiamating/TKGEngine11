#pragma once
#include "Components/inc/CMonoBehaviour.h"

namespace TKGEngine
{
	/// <summary>
	/// レイで接地判定
	/// </summary>
	class CharacterGroundChecker
		: public TKGEngine::MonoBehaviour
	{
	public:
		CharacterGroundChecker() = default;
		virtual ~CharacterGroundChecker() override = default;
		CharacterGroundChecker(const CharacterGroundChecker&) = delete;
		CharacterGroundChecker& operator=(const CharacterGroundChecker&) = delete;

	private:
		virtual void Awake() override;
		virtual void Update() override;

	public:
		bool IsGround() const;

	private:
		// レイを飛ばす距離
		float m_ray_distance = 0.4f;
		// レイの原点位置のオフセット
		float m_ray_offset = -0.2f;
		// 接地状態
		bool m_is_ground = true;

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
CEREAL_CLASS_VERSION(TKGEngine::CharacterGroundChecker, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::CharacterGroundChecker, "TKGEngine::CharacterGroundChecker")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::CharacterGroundChecker)
