
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
		// Edit���̂� : �N�����ȊO�̏�������Temp�t�H���_�����[�h���邽�ߏ����V�[���̍쐬�A���[�h�͍s��Ȃ�
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
				// �ݒ肳�ꂽ�V�[�������[�h����
				LoadScene(SystemSetting::START_SCENE_FILEPATH);
				// ���[�h�Ɏ��s�������̃V�[�����쐬
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
		// �V�[���̏I������������
		{
			// �V�[�����̏�����
			m_active_scenes.clear();
			m_main_scene_id = 0;
			m_total_time = 0.0f;
			m_current_unique_id = 0;

			// �V�[���̍폜
			{
				// �񓯊����[�h���S�ďI������܂Ń��[�v
				while (!CheckAsyncLoad(true))
				{
					/* �񓯊����[�h���X�g����ɂȂ�܂Ń��[�v */
				}
				// ���[�h�ς݃V�[���̔j��
				for (const auto& scene : m_scene_list)
				{
					scene.second->Unload();
				}
				// �񓯊��A�����[�h���S�ďI������܂Ń��[�v
				while (!CheckAsyncUnload())
				{
					/* �񓯊��A�����[�h���X�g����ɂȂ�܂Ń��[�v */
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
				// ��A�N�e�B�u�ȃV�[���͕\���F��ύX����
				if (!scene->GetActive())
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.19f, 0.36f, 0.68f, 0.41f));
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.06f, 0.39f, 0.58f, 0.60f));
					ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.06f, 0.29f, 0.48f, 0.80f));
				}
				// �V�[�����Ƃ̃q�G�����L�[�\��
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
				// ��A�N�e�B�u�ȃV�[���ŕύX�����\���F��߂�
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
						// �V�K�V�[�����쐬���ă��C���V�[���ɃZ�b�g����
						CreateScene("New Scene");
						SetMainScene(m_current_unique_id - 1);

						// ���ݑ��݂���S�ẴV�[����j������
						std::list<SceneID> delete_index_list;
						for (const auto& index : m_scene_list)
						{
							// �V�K�ō쐬�����V�[���ȊO��index���X�^�b�N����
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
						// �t�@�C�����擾
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
						// �V�K�V�[�����쐬���ă��C���V�[���ɃZ�b�g����
						CreateScene("New Scene");
						SetMainScene(m_current_unique_id - 1);
					}
					// Load
					ImGui::SameLine();
					if (ImGui::Button("Load Scene##SceneManager Additive Button"))
					{
						// �t�@�C�����擾
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

					// �폜���X�g�ɕ\������index���X�g
					static std::list<SceneID> remove_select_list;

					// �����̃V�[�������݂���Ƃ��̂ݏ������\
					if (ImGui::Button("Remove Scene##SceneManager Button"))
					{
						remove_select_list.clear();

						// �폜�\�ȃV�[����index���X�^�b�N����
						for (const auto& id : m_key_table)
						{
							// �V�K�ō쐬�����V�[���ȊO��index���X�^�b�N����
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
					// ���C���V�[���̑I��
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
							// �V�[�����X�g�ɑ��݂�����̂������C���V�[���ɂł���
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

				// �I�𒆂̃V�[��index
				static SceneID selecting_scene_index = INVALID_SCENE_INDEX;
				// Set active and Select scene
				ImGui::Separator();
				ImGui::Text("Select Scene / Set Active");
				if (ImGui::BeginChild("Select Scene and Set Active", ImVec2(0, 100), true))
				{
					// ���ݑ��݂���V�[��index�����ɋl�߂Ă���
					std::list<SceneID> exist_scene_indices;
					for (const auto& key : m_key_table)
					{
						// �A�����[�h�����X�g�̂��̂͏���
						if (m_scene_list.find(key.second) == m_scene_list.end())
							continue;

						exist_scene_indices.emplace_back(key.first);
					}
					// �V�[���̍쐬���Ƀ\�[�g����
					exist_scene_indices.sort();
					// ���ɕ\�����Ă���
					for (const auto index : exist_scene_indices)
					{
						ImGui::PushID(index);
						const auto& scene = m_scene_list.at(m_key_table.at(index));

						// Active�`�F�b�N�{�b�N�X
						bool is_active = scene->GetActive();
						if (ImGui::Checkbox("##Scene Active", &is_active))
						{
							SetActive(index, is_active);
						}
						// GUI�\������V�[���̑I��
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
				// �I�𒆂�Scene��GUI
				ImGui::Separator();
				ImGui::Text("Scene Setting");
				if (ImGui::BeginChild("Selecting Scene GUI", ImVec2(0, 0), true, ImGuiWindowFlags_MenuBar))
				{
					ImGui::PushID(selecting_scene_index);

					// �I�𒆂̃V�[��index�̑��݊m�F
					const auto itr_name_find = m_key_table.find(selecting_scene_index);
					// �L�[�e�[�u���̑��݊m�F
					if (itr_name_find == m_key_table.end())
					{
						// ������Ȃ���΃��C���V�[�����Z�b�g����
						selecting_scene_index = m_main_scene_id;
					}
					else
					{
						// �V�[�����X�g�̑��݊m�F
						const auto itr_scene_find = m_scene_list.find(itr_name_find->second);
						if (itr_scene_find == m_scene_list.end())
						{
							// �폜����Ă����烁�C���V�[�����Z�b�g����
							selecting_scene_index = m_main_scene_id;
						}
					}
					// �I�𒆂̃V�[����GUI�\��
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
		// ���C���V�[���̃Z�[�u
		m_scene_list.at(m_key_table.at(m_main_scene_id))->Save(folder + "/M" + SCENE_EXTENSION);
		// ���C���V�[���������V�[�����X�g�̃Z�[�u
		const auto itr_end = m_scene_list.end();
		int scene_count = 0;
		for (auto itr = m_scene_list.begin(); itr != itr_end; ++itr)
		{
			const auto& scene = itr->second;
			// ���C���V�[���̓X�L�b�v
			if (scene->GetSceneID() == m_main_scene_id)
			{
				continue;
			}

			std::string filepath = folder + '/' + std::to_string(scene_count++);
			// �A�N�e�B�u�Ȃ�XXX_T.scene
			if (scene->GetActive())
			{
				filepath = filepath + "_T" + SCENE_EXTENSION;
			}
			// ��A�N�e�B�u�Ȃ�XXX_F.scene
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
		// ���C���V�[�����Ƀ��[�h����
		{
			const std::string main_scene_filepath = folder + "/M" + SCENE_EXTENSION;
			// �t�@�C���̑��݊m�F
			if(!fs::exists(fs::status(main_scene_filepath)))
			{
				assert(0 && "failed save main scene. SceneManager::LoadSceneList()");
				return;
			}
			// ./XXX/M.scene�̃��[�h
			LoadScene(main_scene_filepath);
		}
		// �t�H���_�𑖍�
		fs::directory_iterator itr(folder);
		for (const auto& file : itr)
		{
			// �g���q���`�F�b�N����
			if (file.path().extension() != SCENE_EXTENSION)
			{
				continue;
			}
			// �t�H���_���̃t�@�C�������擾����
			const std::string filepath = file.path().generic_string();
			// ���C���V�[�����`�F�b�N
			{
				const auto split_pos_slash = filepath.find_last_of('/');
				if (split_pos_slash == std::string::npos)
				{
					// '/'��������Ȃ�
					continue;
				}
				// ���C���V�[���Ȃ�X�L�b�v
				if (filepath.at(split_pos_slash + 1) == 'M')
				{
					continue;
				}
			}
			// .scene�̑O��T��F���ŏ�Ԃ��擾
			const auto split_pos_bar = filepath.find_last_of('_');
			if (split_pos_bar == std::string::npos)
			{
				// '_'��������Ȃ�
				continue;
			}
			// �V�[���̃��[�h
			LoadScene(filepath, true);
			// �����o������True�Ȃ�SetActive�Őݒ�
			if (filepath.at(split_pos_bar + 1) == 'T')
			{
				SetActive(filepath, true);
			}
		}
	}
#endif// #ifdef USE_IMGUI

	void SceneManager::SetPipeline()
	{
		// ���ׂĂ�Context��CB_Time, CB_Environment���Z�b�g����,���e���X�V����
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
		// TODO : ���u�� BackBuffer
		IGraphics::Get().SetRTBackBuffer(context, true);
#endif

		m_skybox_shader.Activate(context, false);
		m_scene_list.at(m_key_table.at(m_main_scene_id))->RenderSkyBox(context);
	}

	void SceneManager::FrameUpdate()
	{
		// �񓯊��A�����[�h���X�g
		{
			CheckAsyncUnload();
		}
		// �񓯊����[�h���X�g
		{
			CheckAsyncLoad();
		}

		// Time�̍X�V
		const float delta_time = ITime::Get().DeltaTime();
		const float unscaled_delta_time = ITime::Get().UnscaledDeltaTime();
		m_total_time += delta_time;

		// CBuffer�̍X�V
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
		// �V�K�V�[���̍쐬
		auto p_new_scene = std::make_unique<Scene>();
		p_new_scene->Create(scene_name, m_current_unique_id);

		// �L�[�e�[�u���ɃZ�b�g
		m_key_table.emplace(m_current_unique_id, std::to_string(m_current_unique_id));
		// �V�[�����X�g�ɃZ�b�g
		m_scene_list.emplace(std::to_string(m_current_unique_id), std::move(p_new_scene));

		// �V�[�����A�N�e�B�u�ɂ���
		m_active_scenes.insert(m_current_unique_id++);
	}

	void SceneManager::LoadScene(const std::string& filepath, bool additive)
	{
		// ���[�h�����X�g�Ɋ��ɑ��݂���ꍇ�͑������^�[��
		if (m_loading_scene_list.count(filepath) != 0)
		{
			LOG_DEBUG("Failed Scene::Load(). This scene is already loading. \"%s\"", filepath.c_str());
			return;
		}
		// �V�[�����X�g�Ɋ��ɑ��݂���ꍇ�͑������^�[��
		if (m_scene_list.count(filepath) != 0)
		{
			return;
		}

		// �V�[�������[�h����
		{
			// �쐬
			auto p_new_scene = std::make_unique<Scene>();
			// ���[�h
			p_new_scene->Load(filepath, m_current_unique_id, additive);
			// ���[�h�Ɏ��s�����ꍇ�͔j�����ă��^�[��
			if (!p_new_scene->HasScene())
			{
				p_new_scene.reset();
				return;
			}
			
			// �L�[�e�[�u���ɃZ�b�g
			m_key_table.emplace(m_current_unique_id, filepath);
			// �V�[�����X�g�ɓo�^
			m_scene_list.emplace(filepath, std::move(p_new_scene));
		}
		// ���[�h�����V�[����Single�Ȃ烁�C���V�[���ɂ��鏈��������
		if (!additive)
		{
			// ���[�h�����V�[�����A�N�e�B�u�ɂ���
			SetActive(m_current_unique_id, true);
			// ���[�h�����V�[�������C���V�[���ɂ���
			SetMainScene(m_current_unique_id);
			// ���[�h�����V�[���ȊO���A�����[�h����
			{
				// �A�����[�h����V�[��ID���܂Ƃ߂�
				std::list<SceneID> unload_scene_indices;
				for (const auto& index : m_scene_list)
				{
					if (index.first != filepath)
					{
						unload_scene_indices.emplace_back(index.second->GetSceneID());
					}
				}
				// �A�����[�h����ID���X�g�����ɃA�����[�h���Ă���
				for (const auto id : unload_scene_indices)
				{
					UnloadSceneAsync(id);
				}
			}
		}
		// ���j�[�N�ȃV�[���ԍ���i�߂�
		++m_current_unique_id;
	}

	bool SceneManager::LoadSceneAsync(const std::string& filepath, bool additive)
	{
		// ���[�h�����X�g�Ɋ��ɑ��݂���ꍇ�͑������^�[��false
		if (m_loading_scene_list.count(filepath) != 0)
		{
			return false;
		}
		// �V�[�����X�g�Ɋ��ɑ��݂���ꍇ�͑������^�[��true
		if (m_scene_list.count(filepath) != 0)
		{
			return true;
		}

		// �V�[���̔񓯊����[�h���J�n
		{
			// �쐬
			auto p_new_scene = std::make_unique<Scene>();
			// ���[�h
			p_new_scene->LoadAsync(filepath, m_current_unique_id, additive);
			// �L�[�e�[�u���ɃZ�b�g
			m_key_table.emplace(m_current_unique_id, filepath);
			// ���[�h�����X�g�ɓo�^
			m_loading_scene_list.emplace(filepath, std::move(p_new_scene));
		}
		// ���j�[�N�ȃV�[���ԍ���i�߂�
		++m_current_unique_id;

		// ���[�h�J�n����false��Ԃ�
		return false;
	}

	void SceneManager::UnloadScene(SceneID scene_id)
	{
		UnloadScene(m_key_table.at(scene_id));
	}

	void SceneManager::UnloadSceneAsync(SceneID scene_id)
	{
		// �V�[���̃L�[���̎擾
		const auto itr_find = m_key_table.find(scene_id);
		if (itr_find == m_key_table.end())
			return;
		UnloadSceneAsync(itr_find->second);
	}

	void SceneManager::UnloadScene(const std::string& scene_filepath)
	{
		// �폜����V�[���̎Q�Ƃ��擾
		const auto itr = m_scene_list.find(scene_filepath);
		if (itr == m_scene_list.end())
			return;

		const auto& scene = itr->second;
		const auto unload_scene_id = scene->GetSceneID();
		// �������C���V�[�����j�������Ȃ�Areturn
		if (m_main_scene_id == unload_scene_id)
			return;

		// �V�[�����A�N�e�B�u�ɂ���
		{
			scene->SetActive(false);
			m_active_scenes.erase(unload_scene_id);
		}
		// �V�[���̃A�����[�h
		scene->Unload();
		// �V�[�����X�g����폜
		m_scene_list.erase(itr);
		// �L�[�e�[�u������폜
		m_key_table.erase(unload_scene_id);
	}

	void SceneManager::UnloadSceneAsync(const std::string& scene_filepath)
	{
		// �폜����V�[���̎Q�Ƃ��擾
		const auto itr = m_scene_list.find(scene_filepath);
		if (itr == m_scene_list.end())
			return;

		// TODO : ���[�h���������Ă��Ȃ��V�[����j�����悤�Ƃ����ꍇ�A�X���[����Ă��܂����߁A�j�����ꂸ�Ɏc��\������

		const auto unload_scene_id = itr->second->GetSceneID();

		// �������C���V�[�����j�������Ȃ�Areturn
		if (m_main_scene_id == unload_scene_id)
			return;

		// �V�[�����A�N�e�B�u�ɂ���
		{
			itr->second->SetActive(false);
			m_active_scenes.erase(unload_scene_id);
		}
		// �V�[���̎��̂��ꎞ�ێ�
		std::unique_ptr<Scene> unload_scene_ref = std::move(itr->second);
		// �A�����[�h�����X�g��move
		const auto& itr_emplaced = m_unloading_scene_list.emplace(scene_filepath, std::move(unload_scene_ref)).first;
		// �V�[�����X�g����폜
		m_scene_list.erase(itr);
		// �A�����[�h�J�n
		itr_emplaced->second->UnloadAsync();
	}

	void SceneManager::DontDestroyOnLoad(GameObjectID goid)
	{
		// GameObject�̎擾
		const auto p_go = GameObjectManager::GetGameObject(goid);
		DontDestroyOnLoad(p_go);
	}

	void SceneManager::DontDestroyOnLoad(const std::shared_ptr<IGameObject>& gameobject)
	{
		const auto goid = gameobject->GetGameObjectID();

		// GameObject�����ɑ��̃V�[���Ɏg�p����Ă���̂ŁA��������폜����(INVALID_SCENE_INDEX�������l)
		const int scene_id = gameobject->GetScene();
		m_scene_list.at(m_key_table.at(scene_id))->PopGameObject(goid);
		// GameObjectID��o�^����
		m_dont_destroy_scene->PushGameObject(goid);
	}

	void SceneManager::MoveGameObjectToScene(GameObjectID goid, SceneID scene_id)
	{
		// GameObject�̎擾
		const auto p_go = GameObjectManager::GetGameObject(goid);
		if (p_go == nullptr)
			return;
		// �V�[���̃L�[���̎擾
		const auto itr_find = m_key_table.find(scene_id);
		if (itr_find == m_key_table.end())
			return;

		MoveGameObjectToScene(p_go, itr_find->second);
	}

	void SceneManager::MoveGameObjectToScene(GameObjectID goid, const std::string& scene_filepath)
	{
		// GameObject�̎擾
		const auto p_go = GameObjectManager::GetGameObject(goid);
		if (p_go == nullptr)
			return;
		MoveGameObjectToScene(p_go, scene_filepath);
	}

	void SceneManager::MoveGameObjectToScene(const std::shared_ptr<IGameObject>& gameobject, SceneID scene_id)
	{
		// �V�[���̃L�[���̎擾
		const auto itr_find = m_key_table.find(scene_id);
		if (itr_find == m_key_table.end())
			return;
		MoveGameObjectToScene(gameobject, itr_find->second);
	}

	void SceneManager::MoveGameObjectToScene(const std::shared_ptr<IGameObject>& gameobject, const std::string& scene_filepath)
	{
		// ********************
		// �e�I�u�W�F�N�g�����V�[���̃��[�g�ɂ͑��݂��Ȃ�����SetParent�Ȃǂ̂��߂ɑ������^�[���͕s��
		// ********************

		// GameObject�������̎q�������ꍇ�͐�ɐe����؂藣���ăV�[�����ڂ��悤�ɂ���
		{
			const auto p_tr = gameobject->GetTransform();
			if (p_tr->GetParent())
			{
				p_tr->SetParent();
			}
		}
		// GameObject�̃V�[��ID���猻�݂̃V�[�����擾
		const SceneID current_scene_id = gameobject->GetScene();
		const auto& current_scene = m_scene_list.at(m_key_table.at(current_scene_id));

		// �V�[��ID���玟�̃V�[��������
		const auto itr = m_scene_list.find(scene_filepath);
		if (itr == m_scene_list.end())
			return;
		const auto& next_scene = itr->second;
		const GameObjectID goid = gameobject->GetGameObjectID();

		// ���̃V�[������GameObject�����o��
		current_scene->PopGameObject(goid);

		// �V�����V�[����GameObject������
		gameobject->SetScene(next_scene->GetSceneID());
		next_scene->PushGameObject(goid);
	}

	void SceneManager::RegisterGameObjectToScene(GameObjectID goid, SceneID scene_id)
	{
		// �V�[���̃L�[���̎擾
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
		// GameObject�����݂��Ȃ��Ȃ�GameObjectManager����폜
		if (!s_ptr)
		{
			GameObjectManager::EraseGameObject(goid);
			return;
		}
		// GameObject�̏�������V�[������폜����
		const SceneID scene_id = s_ptr->GetScene();
		const auto key_ref = m_key_table.at(scene_id);
		// �폜���X�g���猟��
		{
			const auto itr_find = m_unloading_scene_list.find(key_ref);
			if(itr_find != m_unloading_scene_list.end())
			{
				itr_find->second->PopGameObject(goid);
			}
		}
		// SceneList���猟��
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
		// INVALID_SCENE_ID�͑������^�[��
		if(scene_id == INVALID_SCENE_INDEX)
		{
			return false;
		}
		// Don't Destroy�V�[���͏�ɃA�N�e�B�u
		if (scene_id == DONT_DESTROY_SCENE_INDEX)
		{
			return true;
		}
		// �V�[�����X�g���猟������
		const auto itr_find = m_scene_list.find(m_key_table.at(scene_id));
		if (itr_find == m_scene_list.end())
			return false;
		return itr_find->second->GetActive();
	}

	void SceneManager::SetActive(SceneID scene_id, bool is_active)
	{
		// �V�[���̃L�[���̎擾
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
		// ���̂̂���V�[������Y������C���f�b�N�X������
		const auto itr_find = m_scene_list.find(scene_filepath);
		// ������Ȃ���Α������^�[��
		if (itr_find == m_scene_list.end())
			return;

		// �V�[�����A�N�e�B�u������Ƃ��̓V�[����1�ȏ�A���C���V�[����False�ɂ���ꍇ��assert
		if (!is_active)
		{
			// ���݂�Active�V�[�������`�F�b�N���āA��ɂP�̓A�N�e�B�u�ȃV�[�������݂���悤�ɂ���
			if (m_active_scenes.size() <= 1)
			{
				LOG_ASSERT("Invalid argument. SceneManager::SetActive() Active Scene Num is always > 0.");
				return;
			}

			// ���C���V�[����false�ɂ����Ȃ��Ƀ��C���V�[����؂�ւ��Ă���ɂ���悤�x������
			if (scene_filepath == m_key_table.at(m_main_scene_id))
			{
				LOG_ASSERT("Invalid argument. SceneManager::SceneActive(). Trying to deactivate the current main scene.");
				return;
			}
		}

		// �A�N�e�B�u�Ɣ�A�N�e�B�u��؂�ւ��Ȃ��Ȃ瑁�����^�[��
		const auto& scene = itr_find->second;
		if (scene->GetActive() == is_active)
			return;

		// �A�N�e�B�u�Ɣ�A�N�e�B�u��؂�ւ���
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
		// �V�[���̃L�[���̎擾
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
		// �V�[���̏������������Ă��邩�`�F�b�N
		const auto& scene = itr->second;
		if (!scene->HasScene())
			return false;
		// ���C���V�[���̕ύX����
		SetActive(scene_filepath, true);
		m_main_scene_id = scene->GetSceneID();
		m_total_time = 0.0f;

		return true;
	}

	Scene* SceneManager::GetScene(SceneID scene_id)
	{
		// �V�[���̃L�[���̎擾
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
			// ���[�h���I�����Ă��邩
			if (!itr->second->IsLoaded())
			{
				// �܂��I�����Ă��Ȃ�
				++itr;
				continue;
			}
			// ���\�[�X�����������[�h�ł��Ă��邩
			if (!itr->second->HasScene())
			{
				// ���[�h�Ɏ��s
				LOG_ASSERT("Failed Async Scene Load. \"%s\"", itr->first.c_str());
				itr = m_loading_scene_list.erase(itr);
				continue;
			}

			// ���[�h������̏���
			// Additive
			if (itr->second->IsAdditive() || termination)
			{
				// ���[�h���SetActive����܂Ŕ�A�N�e�B�u�ɂ���
				itr->second->SetActive(false);
				// �V�[�����X�g��move����
				m_scene_list.emplace(itr->first, std::move(itr->second));
				// ���[�h�����X�g����폜
				itr = m_loading_scene_list.erase(itr);
			}
			// Single
			else
			{
				const auto loaded_scene_id = itr->second->GetSceneID();
				const std::string loaded_scene_name = itr->first;
				// �V�[�����X�g��move
				m_scene_list.emplace(itr->first, std::move(itr->second));
				// ���[�h�����X�g����폜
				itr = m_loading_scene_list.erase(itr);
				// ���C���V�[���ɐݒ�
				SetMainScene(loaded_scene_id);
				// ���̃V�[����񓯊��A�����[�h����
				unsigned itr_index = 0;
				while(itr_index < m_scene_list.size())
				{
					// �V�[�����X�g���Ō��ݎw���C�e���[�^���L���b�V��
					const auto scene_index = std::next(m_scene_list.begin(), itr_index);
					// ���[�h�������̂łȂ���Δ񓯊��V�[���폜�����s
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
		// ���X�g���󂩃`�F�b�N
		if (m_loading_scene_list.empty())
			return true;
		return false;
	}

	bool SceneManager::CheckAsyncUnload()
	{
		const auto itr_end = m_unloading_scene_list.end();
		for (auto itr = m_unloading_scene_list.begin(); itr != itr_end;)
		{
			// ���\�[�X�̉�����I��������
			if (itr->second->HasScene())
			{
				// �܂��c���Ă���
				++itr;
				continue;
			}

			// �A�����[�h������̏���
			// �L�[�e�[�u������폜����
			m_key_table.erase(itr->second->GetSceneID());
			// �A�����[�h�����X�g����폜����
			itr = m_unloading_scene_list.erase(itr);
		}
		// ���X�g���󂩃`�F�b�N
		if (m_unloading_scene_list.empty())
			return true;
		return false;
	}

}// namespace TKGEngine