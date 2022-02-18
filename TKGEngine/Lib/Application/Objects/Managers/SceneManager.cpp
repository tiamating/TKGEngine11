
#include "SceneManager.h"

#include "Components/inc/CTransform.h"
#include "Components/interface/ICamera.h"
#include "Systems/inc/ITime.h"
#include "Systems/inc/IGUI.h"
#include "Systems/inc/IGraphics.h"
#include "Systems/inc/StateManager.h"
#include "Utility/inc/myfunc_file.h"
#include "Utility/inc/myfunc_math.h"
#include "Managers/GameObjectManager.h"
#include "Application/inc/SystemSetting.h"

#include <filesystem>

namespace /* anonymous */
{
	constexpr float Div_2 = 1.0f / 2.0f;
	constexpr float Div_3 = 1.0f / 3.0f;
	constexpr float Div_4 = 1.0f / 4.0f;
	constexpr float Div_8 = 1.0f / 8.0f;
	constexpr float Div_20 = 1.0f / 20.0f;
}// namespace /* anonymous */

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Static member definition
	////////////////////////////////////////////////////////
	Shader SceneManager::m_skybox_shader;
	ConstantBuffer SceneManager::m_cb_environment;
	ConstantBuffer SceneManager::m_cb_time;
	float SceneManager::m_total_time = 0.0f;

	SceneID SceneManager::m_main_scene_id = 0;
	std::unordered_set<SceneID> SceneManager::m_active_scenes;

	std::unordered_map<std::string, std::unique_ptr<Scene>> SceneManager::m_scene_list;
	std::unordered_map<std::string, std::unique_ptr<Scene>> SceneManager::m_loading_scene_list;
	std::unordered_map<std::string, std::unique_ptr<Scene>> SceneManager::m_unloading_scene_list;
	std::unordered_map<SceneID, std::string> SceneManager::m_key_table;

	std::unique_ptr<Scene> SceneManager::m_dont_destroy_scene;

	SceneID SceneManager::m_current_unique_id = 0;


	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	bool SceneManager::Initialize()
	{
		// Create DontDestroy scene
		m_dont_destroy_scene = std::make_unique<Scene>();
		m_dont_destroy_scene->Create("DontDestroyScene", DONT_DESTROY_SCENE_INDEX);

#ifdef USE_IMGUI
		// Edit時のみ : 起動時以外の初期化はTempフォルダをロードするため初期シーンの作成、ロードは行わない
		static bool is_first_initialize = true;
		if (is_first_initialize)
		{
			is_first_initialize = false;
#endif// #ifdef USE_IMGUI
			if (SystemSetting::NEW_SCENE_ON_BEGIN)
			{
				CreateScene("Default Scene");
			}
			else
			{
				// 設定されたシーンをロードする
				LoadScene(SystemSetting::START_SCENE_FILEPATH);
				// ロードに失敗したら空のシーンを作成
				if (m_scene_list.empty())
				{
					CreateScene("Empty Scene");
					LOG_ASSERT("Failed load setting scene file. SceneManager::Initialize()");
				}
			}
#ifdef USE_IMGUI
		}
#endif// #ifdef USE_IMGUI


		// Create ConstantBuffer
		if (!CreateConstantBuffer())
		{
			assert(0 && "failed CreateConstantBuffer() SceneManager::Initialize()");
			return false;
		}

		// Load SkyBox shader
		{
			m_skybox_shader.VS().Load("./Shader/SkyBox/SkyBox_VS.cso");
			m_skybox_shader.PS().Load("./Shader/SkyBox/SkyBox_PS.cso");
		}

		return true;
	}

	void SceneManager::Terminate()
	{
		// シーンの終了処理をする
		{
			// シーン情報の初期化
			m_active_scenes.clear();
			m_main_scene_id = 0;
			m_total_time = 0.0f;
			m_current_unique_id = 0;

			// シーンの削除
			{
				// 非同期ロードが全て終了するまでループ
				while (!CheckAsyncLoad(true))
				{
					/* 非同期ロードリストが空になるまでループ */
				}
				// ロード済みシーンの破棄
				for (const auto& scene : m_scene_list)
				{
					scene.second->Unload();
				}
				// 非同期アンロードが全て終了するまでループ
				while (!CheckAsyncUnload())
				{
					/* 非同期アンロードリストが空になるまでループ */
				}
				m_scene_list.clear();
				m_key_table.clear();
			}

			// Destroy dont_destroy_scene
			m_dont_destroy_scene->Unload();
			m_dont_destroy_scene.reset();
		}
	}

#ifdef USE_IMGUI
	void SceneManager::OnGUI()
	{
		// Hierarchy
		if (ImGui::Begin("Hierarchy"))
		{
			for (auto& scene_map : m_scene_list)
			{
				auto& scene = scene_map.second;
				// 非アクティブなシーンは表示色を変更する
				if (!scene->GetActive())
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.19f, 0.36f, 0.68f, 0.41f));
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.06f, 0.39f, 0.58f, 0.60f));
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.06f, 0.29f, 0.48f, 0.80f));
				}
				// シーンごとのヒエラルキー表示
				ImGui::PushID(scene.get());
				{
					constexpr auto flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick;
					ImGui::SetNextItemOpen(true, ImGuiCond_Once);
					if (ImGui::TreeNodeEx(scene->GetName(), flags))
					{
						scene->OnGUIHierarchy();
						ImGui::TreePop();
					}
				}
				ImGui::PopID();
				// 非アクティブなシーンで変更した表示色を戻す
				if (!scene->GetActive())
				{
					ImGui::PopStyleColor(4);
				}
			}
		}
		ImGui::End();
		// Inspector
		{
			if (IGUI::Get().GetSelectionObjects().size() > 0)
			{
				const auto goid = IGUI::Get().GetSelectionObjects().at(0);
				const auto gameobject = GameObjectManager::GetGameObject(goid);
				if (gameobject)
				{
					gameobject->OnGUI();
				}
			}
		}
		// Scene
		{
			ImGui::PushID("SceneManager GUI");
			if (ImGui::Begin("Scene##Window", nullptr, ImGuiWindowFlags_NoFocusOnAppearing))
			{
				// Single
				ImGui::Text("Single");
				{
					// Create
					if (ImGui::Button("Create Scene##SceneManager Single Button"))
					{
						// 新規シーンを作成してメインシーンにセットする
						CreateScene("New Scene");
						SetMainScene(m_current_unique_id - 1);

						// 現在存在する全てのシーンを破棄する
						std::list<SceneID> delete_index_list;
						for (const auto& index : m_scene_list)
						{
							// 新規で作成したシーン以外のindexをスタックする
							const auto id = index.second->GetSceneID();
							if (id != m_current_unique_id - 1)
							{
								delete_index_list.emplace_back(id);
							}
						}
						for (const SceneID index : delete_index_list)
						{
							UnloadScene(index);
						}
					}
					// Load
					ImGui::SameLine();
					if (ImGui::Button("Load Scene##SceneManager Single Button"))
					{
						// ファイル名取得
						std::string filepath;
						if (MyFunc::FetchOpenFileName(filepath, TEXT("Scene(*.scene)\0*.scene\0"), TEXT("Load Scene"), TEXT(".\\Asset\\Scenes")))
						{
							LoadScene(filepath, false);
						}
					}
				}
				// Additive
				ImGui::Text("Additive");
				{
					// Create
					if (ImGui::Button("Create Scene##SceneManager Additive Button"))
					{
						// 新規シーンを作成してメインシーンにセットする
						CreateScene("New Scene");
						SetMainScene(m_current_unique_id - 1);
					}
					// Load
					ImGui::SameLine();
					if (ImGui::Button("Load Scene##SceneManager Additive Button"))
					{
						// ファイル名取得
						std::string filepath;
						if (MyFunc::FetchOpenFileName(filepath, TEXT("Scene(*.scene)\0*.scene\0"), TEXT("Load Scene"), TEXT(".\\Asset\\Scenes")))
						{
							LoadScene(filepath, true);
						}
					}
				}
				// Remove
				if (m_scene_list.size() >= 2)
				{
					ImGui::Separator();
					ImGui::Text("Select Remove Scene");
					ImGui::SameLine();
					ImGui::HelpMarker("Select remove scene without Main Scene.");

					// 削除リストに表示するindexリスト
					static std::list<SceneID> remove_select_list;

					// 複数のシーンが存在するときのみ消去が可能
					if (ImGui::Button("Remove Scene##SceneManager Button"))
					{
						remove_select_list.clear();

						// 削除可能なシーンのindexをスタックする
						for (const auto& id : m_key_table)
						{
							// 新規で作成したシーン以外のindexをスタックする
							if (id.first != m_main_scene_id)
							{
								remove_select_list.emplace_back(id.first);
							}
						}
						ImGui::OpenPopup("Remove Scene Popup");
					}
					if (ImGui::BeginPopup("Remove Scene Popup"))
					{
						for (const auto id : remove_select_list)
						{
							if (ImGui::Selectable(m_scene_list.at(m_key_table.at(id))->GetName()))
							{
								UnloadScene(id);
							}
						}
						ImGui::EndPopup();
					}
				}
				// Select Main Scene
				{
					// メインシーンの選択
					ImGui::Separator();
					ImGui::Text("Main Scene : ");
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), m_scene_list.at(m_key_table.at(m_main_scene_id))->GetName());
					if (ImGui::Button("Select Main Scene##SceneManager Button"))
					{
						ImGui::OpenPopup("Select Main Scene Popup");
					}
					if (ImGui::BeginPopup("Select Main Scene Popup"))
					{
						for (const auto& key : m_key_table)
						{
							// シーンリストに存在するものだけメインシーンにできる
							if (m_scene_list.count(key.second) == 0)
							{
								continue;
							}
							ImGui::PushID(key.first);
							if (ImGui::Selectable(m_scene_list.at(key.second)->GetName(), key.first == m_main_scene_id))
							{
								SetMainScene(key.second);
							}
							ImGui::PopID();
						}
						ImGui::EndPopup();
					}
				}

				// 選択中のシーンindex
				static SceneID selecting_scene_index = INVALID_SCENE_INDEX;
				// Set active and Select scene
				ImGui::Separator();
				ImGui::Text("Select Scene / Set Active");
				if (ImGui::BeginChild("Select Scene and Set Active", ImVec2(0, 100), true))
				{
					// 現在存在するシーンindexを順に詰めていく
					std::list<SceneID> exist_scene_indices;
					for (const auto& key : m_key_table)
					{
						// アンロード中リストのものは除く
						if (m_scene_list.find(key.second) == m_scene_list.end())
							continue;

						exist_scene_indices.emplace_back(key.first);
					}
					// シーンの作成順にソートする
					exist_scene_indices.sort();
					// 順に表示していく
					for (const auto index : exist_scene_indices)
					{
						ImGui::PushID(index);
						const auto& scene = m_scene_list.at(m_key_table.at(index));

						// Activeチェックボックス
						bool is_active = scene->GetActive();
						if (ImGui::Checkbox("##Scene Active", &is_active))
						{
							SetActive(index, is_active);
						}
						// GUI表示するシーンの選択
						ImGui::SameLine();
						if (ImGui::Selectable(scene->GetName(), selecting_scene_index == index))
						{
							selecting_scene_index = index;
						}

						ImGui::Separator();
						ImGui::PopID();
					}
					ImGui::EndChild();
				}
				// 選択中のSceneのGUI
				ImGui::Separator();
				ImGui::Text("Scene Setting");
				if (ImGui::BeginChild("Selecting Scene GUI", ImVec2(0, 0), true, ImGuiWindowFlags_MenuBar))
				{
					ImGui::PushID(selecting_scene_index);

					// 選択中のシーンindexの存在確認
					const auto itr_name_find = m_key_table.find(selecting_scene_index);
					// キーテーブルの存在確認
					if (itr_name_find == m_key_table.end())
					{
						// 見つからなければメインシーンをセットする
						selecting_scene_index = m_main_scene_id;
					}
					else
					{
						// シーンリストの存在確認
						const auto itr_scene_find = m_scene_list.find(itr_name_find->second);
						if (itr_scene_find == m_scene_list.end())
						{
							// 削除されていたらメインシーンをセットする
							selecting_scene_index = m_main_scene_id;
						}
					}
					// 選択中のシーンのGUI表示
					m_scene_list.at(m_key_table.at(selecting_scene_index))->OnGUI();

					ImGui::PopID();
					ImGui::EndChild();
				}
			}
			ImGui::End();
			ImGui::PopID();
		}
	}

	void SceneManager::SaveSceneList(const std::string& folder)
	{
		// メインシーンのセーブ
		m_scene_list.at(m_key_table.at(m_main_scene_id))->Save(folder + "/M" + SCENE_EXTENSION);
		// メインシーンを除くシーンリストのセーブ
		const auto itr_end = m_scene_list.end();
		int scene_count = 0;
		for (auto itr = m_scene_list.begin(); itr != itr_end; ++itr)
		{
			const auto& scene = itr->second;
			// メインシーンはスキップ
			if (scene->GetSceneID() == m_main_scene_id)
			{
				continue;
			}

			std::string filepath = folder + '/' + std::to_string(scene_count++);
			// アクティブならXXX_T.scene
			if (scene->GetActive())
			{
				filepath = filepath + "_T" + SCENE_EXTENSION;
			}
			// 非アクティブならXXX_F.scene
			else
			{
				filepath = filepath + "_F" + SCENE_EXTENSION;
			}
			scene->Save(filepath);
		}
	}

	void SceneManager::LoadSceneList(const std::string& folder)
	{
		namespace fs = std::filesystem;
		// メインシーンを先にロードする
		{
			const std::string main_scene_filepath = folder + "/M" + SCENE_EXTENSION;
			// ファイルの存在確認
			if(!fs::exists(fs::status(main_scene_filepath)))
			{
				assert(0 && "failed save main scene. SceneManager::LoadSceneList()");
				return;
			}
			// ./XXX/M.sceneのロード
			LoadScene(main_scene_filepath);
		}
		// フォルダを走査
		fs::directory_iterator itr(folder);
		for (const auto& file : itr)
		{
			// 拡張子をチェックする
			if (file.path().extension() != SCENE_EXTENSION)
			{
				continue;
			}
			// フォルダ内のファイル名を取得する
			const std::string filepath = file.path().generic_string();
			// メインシーンかチェック
			{
				const auto split_pos_slash = filepath.find_last_of('/');
				if (split_pos_slash == std::string::npos)
				{
					// '/'が見つからない
					continue;
				}
				// メインシーンならスキップ
				if (filepath.at(split_pos_slash + 1) == 'M')
				{
					continue;
				}
			}
			// .sceneの前がTかFかで状態を取得
			const auto split_pos_bar = filepath.find_last_of('_');
			if (split_pos_bar == std::string::npos)
			{
				// '_'が見つからない
				continue;
			}
			// シーンのロード
			LoadScene(filepath, true);
			// 書き出し時にTrueならSetActiveで設定
			if (filepath.at(split_pos_bar + 1) == 'T')
			{
				SetActive(filepath, true);
			}
		}
	}
#endif// #ifdef USE_IMGUI

	void SceneManager::SetPipeline()
	{
		// すべてのContextにCB_Time, CB_Environmentをセットする,内容を更新する
		for (int i = 0; i < static_cast<int>(Graphics::DC_RENDER_PATH::DC_RP_MAX_NUM); ++i)
		{
			const auto context = IGraphics::Get().DC(0, static_cast<Graphics::DC_RENDER_PATH>(i));
			m_cb_environment.SetVS(context, CBS_ENVIRONMENT);
			m_cb_environment.SetGS(context, CBS_ENVIRONMENT);
			m_cb_environment.SetPS(context, CBS_ENVIRONMENT);

			m_cb_time.SetVS(context, CBS_TIME);
			m_cb_time.SetGS(context, CBS_TIME);
			m_cb_time.SetPS(context, CBS_TIME);

			// Set Sampler
			StateManager::SetAllSampler(context);
		}
	}

	bool SceneManager::SetSkyMap(ID3D11DeviceContext* p_context, int slot)
	{
		return m_scene_list.at(m_key_table.at(m_main_scene_id))->SetSkyMap(p_context, slot);
	}

	void SceneManager::RenderSkyBox(const std::shared_ptr<ICamera>& camera)
	{
		constexpr int max_dc_rp = static_cast<int>(Graphics::DC_RENDER_PATH::DC_RP_MAX_NUM);
		for (int i = 0; i < max_dc_rp; ++i)
		{
			m_scene_list.at(m_key_table.at(m_main_scene_id))->SetSkyMap(IGraphics::Get().DC(0, static_cast<Graphics::DC_RENDER_PATH>(i)), TEXSLOT_GLOBALENVMAP);
		}

		const auto context = IGraphics::Get().DC(0, Graphics::DC_RENDER_PATH::DC_RP_MAIN);

		camera->SetViewport(context);
		camera->SetCBCamera(context);
		camera->SetCBViewProjection(context);

#if 1
		camera->SetRTVs(context, true, true);
#else
		// TODO : 仮置き BackBuffer
		IGraphics::Get().SetRTBackBuffer(context, true);
#endif

		m_skybox_shader.Activate(context, false);
		m_scene_list.at(m_key_table.at(m_main_scene_id))->RenderSkyBox(context);
	}

	void SceneManager::FrameUpdate()
	{
		// 非同期アンロードリスト
		{
			CheckAsyncUnload();
		}
		// 非同期ロードリスト
		{
			CheckAsyncLoad();
		}

		// Timeの更新
		const float delta_time = ITime::Get().DeltaTime();
		const float unscaled_delta_time = ITime::Get().UnscaledDeltaTime();
		m_total_time += delta_time;

		// CBufferの更新
		// CB_Time
		{
			CB_Time cb = {};
			cb.time = VECTOR4(m_total_time * Div_20, m_total_time, m_total_time * 2, m_total_time * 3);
			cb.sin_time =
				VECTOR4(
					MyMath::Sin(m_total_time * Div_8 * MyMath::PI),
					MyMath::Sin(m_total_time * Div_4 * MyMath::PI),
					MyMath::Sin(m_total_time * Div_2 * MyMath::PI),
					MyMath::Sin(m_total_time * MyMath::PI)
				);
			cb.cos_time =
				VECTOR4(
					MyMath::Cos(m_total_time * Div_8 * MyMath::PI),
					MyMath::Cos(m_total_time * Div_4 * MyMath::PI),
					MyMath::Cos(m_total_time * Div_2 * MyMath::PI),
					MyMath::Cos(m_total_time * MyMath::PI)
				);
			cb.delta_time = VECTOR4(delta_time, 1.0f / delta_time, unscaled_delta_time, 1.0f / unscaled_delta_time);

			m_cb_time.SetParam(&cb, sizeof(cb));
		}
		// CB_Environment
		{
			CB_Environment cb = {};
			m_scene_list.at(m_key_table.at(m_main_scene_id))->SetCBParam(cb);

			m_cb_environment.SetParam(&cb, sizeof(cb));
		}
	}

	void SceneManager::CreateScene(const std::string& scene_name)
	{
		// 新規シーンの作成
		auto p_new_scene = std::make_unique<Scene>();
		p_new_scene->Create(scene_name, m_current_unique_id);

		// キーテーブルにセット
		m_key_table.emplace(m_current_unique_id, std::to_string(m_current_unique_id));
		// シーンリストにセット
		m_scene_list.emplace(std::to_string(m_current_unique_id), std::move(p_new_scene));

		// シーンをアクティブにする
		m_active_scenes.insert(m_current_unique_id++);
	}

	void SceneManager::LoadScene(const std::string& filepath, bool additive)
	{
		// ロード中リストに既に存在する場合は早期リターン
		if (m_loading_scene_list.count(filepath) != 0)
		{
			LOG_DEBUG("Failed Scene::Load(). This scene is already loading. \"%s\"", filepath.c_str());
			return;
		}
		// シーンリストに既に存在する場合は早期リターン
		if (m_scene_list.count(filepath) != 0)
		{
			return;
		}

		// シーンをロードする
		{
			// 作成
			auto p_new_scene = std::make_unique<Scene>();
			// ロード
			p_new_scene->Load(filepath, m_current_unique_id, additive);
			// ロードに失敗した場合は破棄してリターン
			if (!p_new_scene->HasScene())
			{
				p_new_scene.reset();
				return;
			}
			
			// キーテーブルにセット
			m_key_table.emplace(m_current_unique_id, filepath);
			// シーンリストに登録
			m_scene_list.emplace(filepath, std::move(p_new_scene));
		}
		// ロードしたシーンがSingleならメインシーンにする処理をする
		if (!additive)
		{
			// ロードしたシーンをアクティブにする
			SetActive(m_current_unique_id, true);
			// ロードしたシーンをメインシーンにする
			SetMainScene(m_current_unique_id);
			// ロードしたシーン以外をアンロードする
			{
				// アンロードするシーンIDをまとめる
				std::list<SceneID> unload_scene_indices;
				for (const auto& index : m_scene_list)
				{
					if (index.first != filepath)
					{
						unload_scene_indices.emplace_back(index.second->GetSceneID());
					}
				}
				// アンロードするIDリストを順にアンロードしていく
				for (const auto id : unload_scene_indices)
				{
					UnloadSceneAsync(id);
				}
			}
		}
		// ユニークなシーン番号を進める
		++m_current_unique_id;
	}

	bool SceneManager::LoadSceneAsync(const std::string& filepath, bool additive)
	{
		// ロード中リストに既に存在する場合は早期リターンfalse
		if (m_loading_scene_list.count(filepath) != 0)
		{
			return false;
		}
		// シーンリストに既に存在する場合は早期リターンtrue
		if (m_scene_list.count(filepath) != 0)
		{
			return true;
		}

		// シーンの非同期ロードを開始
		{
			// 作成
			auto p_new_scene = std::make_unique<Scene>();
			// ロード
			p_new_scene->LoadAsync(filepath, m_current_unique_id, additive);
			// キーテーブルにセット
			m_key_table.emplace(m_current_unique_id, filepath);
			// ロード中リストに登録
			m_loading_scene_list.emplace(filepath, std::move(p_new_scene));
		}
		// ユニークなシーン番号を進める
		++m_current_unique_id;

		// ロード開始時はfalseを返す
		return false;
	}

	void SceneManager::UnloadScene(SceneID scene_id)
	{
		UnloadScene(m_key_table.at(scene_id));
	}

	void SceneManager::UnloadSceneAsync(SceneID scene_id)
	{
		// シーンのキー名の取得
		const auto itr_find = m_key_table.find(scene_id);
		if (itr_find == m_key_table.end())
			return;
		UnloadSceneAsync(itr_find->second);
	}

	void SceneManager::UnloadScene(const std::string& scene_filepath)
	{
		// 削除するシーンの参照を取得
		const auto itr = m_scene_list.find(scene_filepath);
		if (itr == m_scene_list.end())
			return;

		const auto& scene = itr->second;
		const auto unload_scene_id = scene->GetSceneID();
		// もしメインシーンが破棄されるなら、return
		if (m_main_scene_id == unload_scene_id)
			return;

		// シーンを非アクティブにする
		{
			scene->SetActive(false);
			m_active_scenes.erase(unload_scene_id);
		}
		// シーンのアンロード
		scene->Unload();
		// シーンリストから削除
		m_scene_list.erase(itr);
		// キーテーブルから削除
		m_key_table.erase(unload_scene_id);
	}

	void SceneManager::UnloadSceneAsync(const std::string& scene_filepath)
	{
		// 削除するシーンの参照を取得
		const auto itr = m_scene_list.find(scene_filepath);
		if (itr == m_scene_list.end())
			return;

		// TODO : ロードが完了していないシーンを破棄しようとした場合、スルーされてしまうため、破棄されずに残る可能性あり

		const auto unload_scene_id = itr->second->GetSceneID();

		// もしメインシーンが破棄されるなら、return
		if (m_main_scene_id == unload_scene_id)
			return;

		// シーンを非アクティブにする
		{
			itr->second->SetActive(false);
			m_active_scenes.erase(unload_scene_id);
		}
		// シーンの実体を一時保持
		std::unique_ptr<Scene> unload_scene_ref = std::move(itr->second);
		// アンロード中リストにmove
		const auto& itr_emplaced = m_unloading_scene_list.emplace(scene_filepath, std::move(unload_scene_ref)).first;
		// シーンリストから削除
		m_scene_list.erase(itr);
		// アンロード開始
		itr_emplaced->second->UnloadAsync();
	}

	void SceneManager::DontDestroyOnLoad(GameObjectID goid)
	{
		// GameObjectの取得
		const auto p_go = GameObjectManager::GetGameObject(goid);
		DontDestroyOnLoad(p_go);
	}

	void SceneManager::DontDestroyOnLoad(const std::shared_ptr<IGameObject>& gameobject)
	{
		const auto goid = gameobject->GetGameObjectID();

		// GameObjectが既に他のシーンに使用されているので、そこから削除する(INVALID_SCENE_INDEXが初期値)
		const int scene_id = gameobject->GetScene();
		m_scene_list.at(m_key_table.at(scene_id))->PopGameObject(goid);
		// GameObjectIDを登録する
		m_dont_destroy_scene->PushGameObject(goid);
	}

	void SceneManager::MoveGameObjectToScene(GameObjectID goid, SceneID scene_id)
	{
		// GameObjectの取得
		const auto p_go = GameObjectManager::GetGameObject(goid);
		if (p_go == nullptr)
			return;
		// シーンのキー名の取得
		const auto itr_find = m_key_table.find(scene_id);
		if (itr_find == m_key_table.end())
			return;

		MoveGameObjectToScene(p_go, itr_find->second);
	}

	void SceneManager::MoveGameObjectToScene(GameObjectID goid, const std::string& scene_filepath)
	{
		// GameObjectの取得
		const auto p_go = GameObjectManager::GetGameObject(goid);
		if (p_go == nullptr)
			return;
		MoveGameObjectToScene(p_go, scene_filepath);
	}

	void SceneManager::MoveGameObjectToScene(const std::shared_ptr<IGameObject>& gameobject, SceneID scene_id)
	{
		// シーンのキー名の取得
		const auto itr_find = m_key_table.find(scene_id);
		if (itr_find == m_key_table.end())
			return;
		MoveGameObjectToScene(gameobject, itr_find->second);
	}

	void SceneManager::MoveGameObjectToScene(const std::shared_ptr<IGameObject>& gameobject, const std::string& scene_filepath)
	{
		// ********************
		// 親オブジェクトしかシーンのルートには存在しないためSetParentなどのために早期リターンは不可
		// ********************

		// GameObjectが何かの子だった場合は先に親から切り離してシーンを移れるようにする
		{
			const auto p_tr = gameobject->GetTransform();
			if (p_tr->GetParent())
			{
				p_tr->SetParent();
			}
		}
		// GameObjectのシーンIDから現在のシーンを取得
		const SceneID current_scene_id = gameobject->GetScene();
		const auto& current_scene = m_scene_list.at(m_key_table.at(current_scene_id));

		// シーンIDから次のシーンを検索
		const auto itr = m_scene_list.find(scene_filepath);
		if (itr == m_scene_list.end())
			return;
		const auto& next_scene = itr->second;
		const GameObjectID goid = gameobject->GetGameObjectID();

		// 元のシーンからGameObjectを取り出す
		current_scene->PopGameObject(goid);

		// 新しいシーンにGameObjectを入れる
		gameobject->SetScene(next_scene->GetSceneID());
		next_scene->PushGameObject(goid);
	}

	void SceneManager::RegisterGameObjectToScene(GameObjectID goid, SceneID scene_id)
	{
		// シーンのキー名の取得
		const auto itr_name_find = m_key_table.find(scene_id);
		if (itr_name_find == m_key_table.end())
			return;
		const auto itr_scene_find = m_scene_list.find(itr_name_find->second);
		if (itr_scene_find == m_scene_list.end())
			return;
		itr_scene_find->second->PushGameObject(goid);
	}

	void SceneManager::EraseGameObjectFromScene(GameObjectID goid)
	{
		const auto s_ptr = GameObjectManager::GetGameObject(goid);
		// GameObjectが存在しないならGameObjectManagerから削除
		if (!s_ptr)
		{
			GameObjectManager::EraseGameObject(goid);
			return;
		}
		// GameObjectの所属するシーンから削除する
		const SceneID scene_id = s_ptr->GetScene();
		const auto key_ref = m_key_table.at(scene_id);
		// 削除リストから検索
		{
			const auto itr_find = m_unloading_scene_list.find(key_ref);
			if(itr_find != m_unloading_scene_list.end())
			{
				itr_find->second->PopGameObject(goid);
			}
		}
		// SceneListから検索
		{
			const auto itr_find = m_scene_list.find(key_ref);
			if (itr_find != m_scene_list.end())
			{
				itr_find->second->PopGameObject(goid);
			}
		}
	}

	bool SceneManager::GetActive(SceneID scene_id)
	{
		// INVALID_SCENE_IDは早期リターン
		if(scene_id == INVALID_SCENE_INDEX)
		{
			return false;
		}
		// Don't Destroyシーンは常にアクティブ
		if (scene_id == DONT_DESTROY_SCENE_INDEX)
		{
			return true;
		}
		// シーンリストから検索する
		const auto itr_find = m_scene_list.find(m_key_table.at(scene_id));
		if (itr_find == m_scene_list.end())
			return false;
		return itr_find->second->GetActive();
	}

	void SceneManager::SetActive(SceneID scene_id, bool is_active)
	{
		// シーンのキー名の取得
		const auto itr_find = m_key_table.find(scene_id);
		if (itr_find == m_key_table.end())
			return;
		SetActive(itr_find->second, is_active);
	}

	bool SceneManager::GetActive(const std::string& scene_filepath)
	{
		const auto itr_find = m_scene_list.find(scene_filepath);
		if (itr_find == m_scene_list.end())
			return false;
		return itr_find->second->GetActive();
	}

	void SceneManager::SetActive(const std::string& scene_filepath, bool is_active)
	{
		// 実体のあるシーンから該当するインデックスを検索
		const auto itr_find = m_scene_list.find(scene_filepath);
		// 見つからなければ早期リターン
		if (itr_find == m_scene_list.end())
			return;

		// シーンを非アクティブ化するときはシーンが1つ以上、メインシーンをFalseにする場合にassert
		if (!is_active)
		{
			// 現在のActiveシーン数をチェックして、常に１つはアクティブなシーンが存在するようにする
			if (m_active_scenes.size() <= 1)
			{
				LOG_ASSERT("Invalid argument. SceneManager::SetActive() Active Scene Num is always > 0.");
				return;
			}

			// メインシーンがfalseにされるなら先にメインシーンを切り替えてからにするよう警告する
			if (scene_filepath == m_key_table.at(m_main_scene_id))
			{
				LOG_ASSERT("Invalid argument. SceneManager::SceneActive(). Trying to deactivate the current main scene.");
				return;
			}
		}

		// アクティブと非アクティブを切り替えないなら早期リターン
		const auto& scene = itr_find->second;
		if (scene->GetActive() == is_active)
			return;

		// アクティブと非アクティブを切り替える
		scene->SetActive(is_active);
		if (is_active)
		{
			m_active_scenes.insert(scene->GetSceneID());
		}
		else
		{
			m_active_scenes.erase(scene->GetSceneID());
		}
	}

	void SceneManager::GetActiveSceneList(std::unordered_set<SceneID>& buf)
	{
		buf.clear();
		const auto itr_end = m_active_scenes.end();
		for (auto itr = m_active_scenes.begin(); itr != itr_end; ++itr)
		{
			buf.emplace(*itr);
		}
	}

	SceneID SceneManager::GetMainSceneID()
	{
		return m_main_scene_id;
	}

	Scene* SceneManager::GetMainScene()
	{
		return m_scene_list.at(m_key_table.at(m_main_scene_id)).get();
	}

	bool SceneManager::SetMainScene(SceneID scene_id)
	{
		// シーンのキー名の取得
		const auto itr_find = m_key_table.find(scene_id);
		if (itr_find == m_key_table.end())
			return false;
		return SetMainScene(itr_find->second);
	}

	bool SceneManager::SetMainScene(const std::string& scene_filepath)
	{
		const auto itr = m_scene_list.find(scene_filepath);
		if (itr == m_scene_list.end())
			return false;
		// シーンの準備が完了しているかチェック
		const auto& scene = itr->second;
		if (!scene->HasScene())
			return false;
		// メインシーンの変更処理
		SetActive(scene_filepath, true);
		m_main_scene_id = scene->GetSceneID();
		m_total_time = 0.0f;

		return true;
	}

	Scene* SceneManager::GetScene(SceneID scene_id)
	{
		// シーンのキー名の取得
		const auto itr_find = m_key_table.find(scene_id);
		if (itr_find == m_key_table.end())
			return nullptr;
		return GetScene(itr_find->second);
	}

	Scene* SceneManager::GetScene(const std::string& scene_filepath)
	{
		const auto itr = m_scene_list.find(scene_filepath);
		if (itr == m_scene_list.end())
			return nullptr;
		return itr->second.get();
	}

	int SceneManager::GetLoadingSceneNum()
	{
		return m_loading_scene_list.size();
	}

	int SceneManager::GetUnloadingSceneNum()
	{
		return m_unloading_scene_list.size();
	}

	void SceneManager::PushGameObject(GameObjectID goid, SceneID scene_id)
	{
		const auto itr = m_scene_list.find(m_key_table.at(scene_id));
		if (itr == m_scene_list.end())
			return;
		itr->second->PushGameObject(goid);
	}

	void SceneManager::PopGameObject(GameObjectID goid, SceneID scene_id)
	{
		const auto itr = m_scene_list.find(m_key_table.at(scene_id));
		if (itr == m_scene_list.end())
			return;
		itr->second->PopGameObject(goid);
	}

	bool SceneManager::CreateConstantBuffer()
	{
		// CB_Environment
		if (!m_cb_environment.Create(static_cast<int>(sizeof(CB_Environment)), false))
		{
			assert(0 && "failed SceneManager::CreateConstantBuffer(CB_Environment)");
			return false;
		}

		// CB_Time
		if (!m_cb_time.Create(static_cast<int>(sizeof(CB_Time)), false))
		{
			assert(0 && "failed SceneManager::CreateConstantBuffer(CB_Time)");
			return false;
		}

		return true;
	}

	bool SceneManager::CheckAsyncLoad(bool termination)
	{
		const auto itr_end = m_loading_scene_list.end();
		for (auto itr = m_loading_scene_list.begin(); itr != itr_end;)
		{
			// ロードが終了しているか
			if (!itr->second->IsLoaded())
			{
				// まだ終了していない
				++itr;
				continue;
			}
			// リソースが正しくロードできているか
			if (!itr->second->HasScene())
			{
				// ロードに失敗
				LOG_ASSERT("Failed Async Scene Load. \"%s\"", itr->first.c_str());
				itr = m_loading_scene_list.erase(itr);
				continue;
			}

			// ロード成功後の処理
			// Additive
			if (itr->second->IsAdditive() || termination)
			{
				// ロード後にSetActiveするまで非アクティブにする
				itr->second->SetActive(false);
				// シーンリストにmoveする
				m_scene_list.emplace(itr->first, std::move(itr->second));
				// ロード中リストから削除
				itr = m_loading_scene_list.erase(itr);
			}
			// Single
			else
			{
				const auto loaded_scene_id = itr->second->GetSceneID();
				const std::string loaded_scene_name = itr->first;
				// シーンリストにmove
				m_scene_list.emplace(itr->first, std::move(itr->second));
				// ロード中リストから削除
				itr = m_loading_scene_list.erase(itr);
				// メインシーンに設定
				SetMainScene(loaded_scene_id);
				// 他のシーンを非同期アンロードする
				unsigned itr_index = 0;
				while(itr_index < m_scene_list.size())
				{
					// シーンリスト内で現在指すイテレータをキャッシュ
					const auto scene_index = std::next(m_scene_list.begin(), itr_index);
					// ロードしたものでなければ非同期シーン削除を実行
					if(loaded_scene_name != scene_index->first)
					{
						UnloadSceneAsync(scene_index->first);
					}
					else
					{
						++itr_index;
					}
				}
			}
		}
		// リストが空かチェック
		if (m_loading_scene_list.empty())
			return true;
		return false;
	}

	bool SceneManager::CheckAsyncUnload()
	{
		const auto itr_end = m_unloading_scene_list.end();
		for (auto itr = m_unloading_scene_list.begin(); itr != itr_end;)
		{
			// リソースの解放が終了したか
			if (itr->second->HasScene())
			{
				// まだ残っている
				++itr;
				continue;
			}

			// アンロード完了後の処理
			// キーテーブルから削除する
			m_key_table.erase(itr->second->GetSceneID());
			// アンロード中リストから削除する
			itr = m_unloading_scene_list.erase(itr);
		}
		// リストが空かチェック
		if (m_unloading_scene_list.empty())
			return true;
		return false;
	}

}// namespace TKGEngine