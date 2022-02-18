#include "GameStageManager.h"

REGISTERCOMPONENT(TKGEngine::GameStageManager);

namespace TKGEngine
{
	/////////////////////////////////////////////////////////////////////////
	// 実体生成時に呼び出される
	/////////////////////////////////////////////////////////////////////////
	//void GameStageManager::Awake(){}

	/////////////////////////////////////////////////////////////////////////
	// Updateが呼ばれる前に一度だけ呼び出される
	/////////////////////////////////////////////////////////////////////////
	void GameStageManager::Start()
	{
		// プレイヤーを探索
		const auto player = IGameObject::Find("unitychan");
		if (!player)
		{
			return;
		}
		m_player = player->GetTransform();

		// 背景シーンをロード
		const int bg_num = std::size(m_bg_scene_names);
		for (int i = 0; i < bg_num; ++i)
		{
			SceneManager::LoadSceneAsync(m_bg_scene_names[i], true);
		}
	}

	/////////////////////////////////////////////////////////////////////////
	// 毎フレーム呼び出される
	/////////////////////////////////////////////////////////////////////////
	void GameStageManager::Update()
	{
		const auto transform = m_player.GetWeak().lock();
		if (transform == nullptr)
			return;
		// プレイヤーの位置からアクティブシーンを判断する
		const float player_pos = transform->Position().z;
		if(player_pos > m_split_0)
		{
			// ロード完了チェック
			if(SceneManager::LoadSceneAsync(m_bg_scene_names[0], true))
			{
				SceneManager::SetActive(m_bg_scene_names[0], true);
				SceneManager::SetActive(m_bg_scene_names[1], false);
				SceneManager::SetActive(m_bg_scene_names[2], false);
			}
		}
		else if(player_pos > m_split_1)
		{
			// ロード完了チェック
			if (SceneManager::LoadSceneAsync(m_bg_scene_names[1], true))
			{
				SceneManager::SetActive(m_bg_scene_names[0], false);
				SceneManager::SetActive(m_bg_scene_names[1], true);
				SceneManager::SetActive(m_bg_scene_names[2], false);
			}
		}
		else
		{
			// ロード完了チェック
			if (SceneManager::LoadSceneAsync(m_bg_scene_names[2], true))
			{
				SceneManager::SetActive(m_bg_scene_names[0], false);
				SceneManager::SetActive(m_bg_scene_names[1], false);
				SceneManager::SetActive(m_bg_scene_names[2], true);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////
	// Update後に毎フレーム呼び出される
	/////////////////////////////////////////////////////////////////////////
	//void GameStageManager::LateUpdate(){}

	/////////////////////////////////////////////////////////////////////////
	// 実体が有効になった時に呼び出される
	/////////////////////////////////////////////////////////////////////////
	//void GameStageManager::OnEnable(){}

	/////////////////////////////////////////////////////////////////////////
	// 実体が無効になった時に呼び出される
	/////////////////////////////////////////////////////////////////////////
	//void GameStageManager::OnDisable(){}

	/////////////////////////////////////////////////////////////////////////
	// 実体破棄時に呼び出される
	/////////////////////////////////////////////////////////////////////////
	void GameStageManager::OnDestroyed()
	{
		// 背景シーンを破棄
		const int bg_num = std::size(m_bg_scene_names);
		for (int i = 0; i < bg_num; ++i)
		{
			SceneManager::UnloadSceneAsync(m_bg_scene_names[i]);
		}
	}


	/////////////////////////////////////////////////////////////////////////
	// GUI設定
	/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	void GameStageManager::OnGUI()
	{
	}
#endif
}
