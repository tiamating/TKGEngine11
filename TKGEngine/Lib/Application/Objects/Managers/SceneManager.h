#pragma once

#include "Systems/inc/TKGEngine_Defined.h"

#include "Application/Resource/inc/Scene.h"
#include "Application/Resource/inc/ConstantBuffer.h"
#include "Application/Resource/inc/Shader.h"
#include "Application/Objects/inc/IGameObject.h"

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <memory>

struct ID3D11DeviceContext;

namespace TKGEngine
{
	class ICamera;

	class SceneManager
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static bool Initialize();
		static void Terminate();

#ifdef USE_IMGUI
		static void OnGUI();

		// シーンリストの一括セーブ、ロード
		static void SaveSceneList(const std::string& folder);
		static void LoadSceneList(const std::string& folder);
#endif// #ifdef USE_IMGUI

		// CBufferをセットする
		static void SetPipeline();

		static bool SetSkyMap(ID3D11DeviceContext* p_context, int slot);
		// MainシーンのSceneSettingを使用して描画する
		static void RenderSkyBox(const std::shared_ptr<ICamera>& camera);

		// Asyncにセットされたものの状態を更新する
		// MainSceneが変わるとTimeの更新をする
		static void FrameUpdate();

		// シーン作成
		static void CreateScene(const std::string& scene_name);

		// ロード
		static void LoadScene(const std::string& filepath, bool additive = false);

		/// <summary>
		/// 非同期ロード
		/// </summary>
		/// <returns>ロード済みシーンリストに存在する場合はTrue</returns>
		/// <returns>ロードが完了したらTrue</returns>
		/// <returns>ロード中はFalse</returns>
		static bool LoadSceneAsync(const std::string& filepath, bool additive = false);

		// アンロード
		static void UnloadScene(SceneID scene_id);
		static void UnloadSceneAsync(SceneID scene_id);
		static void UnloadScene(const std::string& scene_filepath);
		static void UnloadSceneAsync(const std::string& scene_filepath);

		// GOのシーン移動用関数
		static void DontDestroyOnLoad(GameObjectID goid);
		static void DontDestroyOnLoad(const std::shared_ptr<IGameObject>& gameobject);
		static void MoveGameObjectToScene(GameObjectID goid, SceneID scene_id);
		static void MoveGameObjectToScene(GameObjectID goid, const std::string& scene_filepath);
		static void MoveGameObjectToScene(const std::shared_ptr<IGameObject>& gameobject, SceneID scene_id);
		static void MoveGameObjectToScene(const std::shared_ptr<IGameObject>& gameobject, const std::string& scene_filepath);

		// GOの生成時用関数
		static void RegisterGameObjectToScene(GameObjectID goid, SceneID scene_id);

		// GOの破棄時用関数
		static void EraseGameObjectFromScene(GameObjectID goid);

		// シーン状態取得、変更用関数
		static bool GetActive(SceneID scene_id);
		static void SetActive(SceneID scene_id, bool is_active);
		static bool GetActive(const std::string& scene_filepath);
		static void SetActive(const std::string& scene_filepath, bool is_active);

		// 有効なシーンIDをすべて取得
		static void GetActiveSceneList(std::unordered_set<SceneID>& buf);
		// メインシーンを取得
		static SceneID GetMainSceneID();
		static Scene* GetMainScene();
		// Sceneが存在しないときfalse
		static bool SetMainScene(SceneID scene_id);
		static bool SetMainScene(const std::string& scene_filepath);

		static Scene* GetScene(SceneID scene_id);
		static Scene* GetScene(const std::string& scene_filepath);

		// ロード、アンロード中のシーン数
		static int GetLoadingSceneNum();
		static int GetUnloadingSceneNum();


		// ==============================================
		// public variables
		// ==============================================
		// シーンの範囲外を示すシーン番号
		static constexpr SceneID INVALID_SCENE_INDEX = -2;


	private:
		// ==============================================
		// private struct
		// ==============================================
		struct CB_Time
		{
			VECTOR4 time;		// ( t/20, t, 2t, 3t )
			VECTOR4 sin_time;	// ( t/8, t/4, t/2, t )
			VECTOR4 cos_time;	// ( t/8, t/4, t/2, t )
			VECTOR4 delta_time;	// ( dt, 1/dt, unscaled_dt, 1/unscaled_dt )
		};

		// ==============================================
		// private methods
		// ==============================================
		// GOの登録、破棄用関数
		static void PushGameObject(GameObjectID goid, SceneID scene_id);
		static void PopGameObject(GameObjectID goid, SceneID scene_id);

		static bool CreateConstantBuffer();
		
		/// <summary>
		/// 非同期チェック
		/// </summary>
		/// <param name="termination">マネージャー破棄時はSingleの処理を行わない</param>
		/// <returns>チェック後にリスト内が空ならtrue</returns>
		static bool CheckAsyncLoad(bool termination = false);
		static bool CheckAsyncUnload();

		// ==============================================
		// private variables
		// ==============================================
		// DONT_DESTROY_SCENEのシーン番号
		static constexpr SceneID DONT_DESTROY_SCENE_INDEX = -1;

		static Shader m_skybox_shader;
		static ConstantBuffer m_cb_environment;
		static ConstantBuffer m_cb_time;
		// 現在のメインシーンが開始してからの経過時間
		static float m_total_time;
		// 現在のメインシーンID
		static SceneID m_main_scene_id;
		// 有効なシーンのリスト
		static std::unordered_set<SceneID> m_active_scenes;
		// シーンリスト
		static std::unordered_map<std::string, std::unique_ptr<Scene>> m_scene_list;
		// ロード、破棄を行っているシーンを管理するリスト
		static std::unordered_map<std::string, std::unique_ptr<Scene>> m_loading_scene_list;
		static std::unordered_map<std::string, std::unique_ptr<Scene>> m_unloading_scene_list;
		// <SceneID, Key文字列>対応リスト
		static std::unordered_map<SceneID, std::string> m_key_table;
		// 破棄されないシーン
		static std::unique_ptr<Scene> m_dont_destroy_scene;
		// 現在のユニークなシーン番号
		static SceneID m_current_unique_id;
	};


}// namespace TKGEngine