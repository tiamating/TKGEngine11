#pragma once
#include "Components/inc/CMonoBehaviour.h"

namespace TKGEngine
{
	class UIRenderer;

	class InitSceneManager
		: public MonoBehaviour
	{
	public:
		InitSceneManager() = default;
		virtual ~InitSceneManager() override = default;
		InitSceneManager(const InitSceneManager&) = delete;
		InitSceneManager& operator=(const InitSceneManager&) = delete;

	private:
		virtual void Start() override;
		virtual void Update() override;

		//////////////////////////////////
		// functions
		//////////////////////////////////
		

		//////////////////////////////////
		// variables
		//////////////////////////////////
		SWPtr<UIRenderer> m_unitychan_ui;
		SWPtr<UIRenderer> m_button_ui;

		// UIの回転速度
		VECTOR3 m_rotate_speed = VECTOR3(120.0f, 60.0f, 0.0f);
		// ロード完了
		bool m_finish_loading = false;

		// 回転終了用フラグ
		bool m_finish_rotate_x = false;
		bool m_finish_rotate_y = false;

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
CEREAL_CLASS_VERSION(TKGEngine::InitSceneManager, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::InitSceneManager, "TKGEngine::InitSceneManager")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::InitSceneManager)
