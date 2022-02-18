#pragma once
#include "Components/inc/CMonoBehaviour.h"

namespace TKGEngine
{
	/// <summary>
	/// Playerの位置を監視して、随時ステージをロード、アンロード、アクティブ変更する
	/// </summary>
	class GameStageManager
		: public MonoBehaviour
	{
	public:
		GameStageManager() = default;
		virtual ~GameStageManager() override = default;
		GameStageManager(const GameStageManager&) = delete;
		GameStageManager& operator=(const GameStageManager&) = delete;

	private:
		/////////////////////////////////////////////////////////////////////////
		// 使用するものをコメント解除して使用
		/////////////////////////////////////////////////////////////////////////
		//virtual void Awake() override;
		virtual void Start() override;
		virtual void Update() override;
		//virtual void LateUpdate() override;
		//virtual void OnEnable() override;
		//virtual void OnDisable() override;
		virtual void OnDestroyed() override;

		/////////////////////////////////////////////////////////////////////////
		// コリジョン用メッセージ
		/////////////////////////////////////////////////////////////////////////
		// 物体同士の衝突時
		//virtual void OnCollisionEnter(std::shared_ptr<TKGEngine::Collider>& collider) override;
		//virtual void OnCollisionStay(std::shared_ptr<TKGEngine::Collider>& collider) override;
		//virtual void OnCollisionExit(std::shared_ptr<TKGEngine::Collider>& collider) override;
		// トリガーオブジェクトの衝突時
		//virtual void OnTriggerEnter(std::shared_ptr<TKGEngine::Collider>& collider) override;
		//virtual void OnTriggerStay(std::shared_ptr<TKGEngine::Collider>& collider) override;
		//virtual void OnTriggerExit(std::shared_ptr<TKGEngine::Collider>& collider) override;
		
	private:
		//////////////////////////////////
		// functions
		//////////////////////////////////



		//////////////////////////////////
		// variables
		//////////////////////////////////

		// プレイヤー参照
		SWPtr<Transform> m_player;

		// 背景の分割位置
		float m_split_0 = -12.0f;
		float m_split_1 = -25.0f;

		// 背景シーンファイル名
		const char* m_bg_scene_names[3] =
		{
			"./Asset/Scenes/BG_0Scene.scene",
			"./Asset/Scenes/BG_1Scene.scene",
			"./Asset/Scenes/BG_2Scene.scene"
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
CEREAL_CLASS_VERSION(TKGEngine::GameStageManager, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::GameStageManager, "TKGEngine::GameStageManager")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::GameStageManager)
