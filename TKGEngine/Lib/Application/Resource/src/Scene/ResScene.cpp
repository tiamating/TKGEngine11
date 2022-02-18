
#include "IResScene.h"

#include "../../inc/Texture.h"
#include "Application/Objects/Components/inc/CTransform.h"
#include "Managers/GameObjectManager.h"
#include "Managers/MonoBehaviourManager.h"
#include "Managers/SceneManager.h"
#include "Systems/inc/LogSystem.h"
#include "Systems/inc/PhysicsSystem.h"
#include "Systems/inc/AssetSystem.h"
#include "Systems/inc/IGUI.h"

#include <vector>
#include <list>
#include <unordered_set>
#include <string>

#include <cassert>


namespace TKGEngine
{
	class ResScene
		: public IResScene
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		ResScene() = default;
		virtual ~ResScene() = default;
		ResScene(const ResScene&) = delete;
		ResScene& operator=(const ResScene&) = delete;

		// AssetDataBase
#ifdef USE_IMGUI
		void OnGUI() override;
		void OnGUIHierarchy() override;
#endif // USE_IMGUI
		// ~AssetDataBase



		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		friend class cereal::access;
		template <class Archive>
		void load(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					cereal::base_class<IResScene>(this),
					CEREAL_NVP(m_environment)
				);
				// Root�I�u�W�F�N�g��
				int root_count;
				archive(cereal::make_nvp("Root Object Count", root_count));
				// Root�I�u�W�F�N�g�����[�h���āA�����V�[���Ɏ��g�̃V�[��ID���Z�b�g����
				for (int i = 0; i < root_count; ++i)
				{
					std::shared_ptr<Transform> root_transform;
					// �f�V���A���C�Y
					archive(root_transform);
					// GameObject�̃V�[���ɃZ�b�g
					root_transform->GetGameObject()->SetScene(m_scene_id);
					// ���̃V�[���̊Ǘ�����GameObjectID��Root��ID���Z�b�g
					PushGameObject(root_transform->GetOwnerID());
				}

			}
		}
		template <class Archive>
		void save(Archive& archive, const std::uint32_t version) const
		{
			//if (version > 0)
			{
				archive(
					cereal::base_class<IResScene>(this),
					CEREAL_NVP(m_environment)
				);

				// Root�I�u�W�F�N�g��
				archive(cereal::make_nvp("Root Object Count", static_cast<int>(m_root_goid.size())));
				// GameObjectID����Transform���擾���ď����o��
				for (const auto goid : m_root_goid)
				{
					const auto transform = GameObjectManager::GetGameObject(goid)->GetTransform();
					archive(transform);
				}
			}
		}

		// ==============================================
		// private methods
		// ==============================================
		bool OnCreate();

		void SetAsyncOnLoad() override;
		void OnLoad() override;

		void SetAsyncOnUnload() override;
		void OnUnload() override;

		void SetLoadAdditive(bool additive) override;
		bool IsAdditive() const override;

		bool GetActive() const override;
		void SetActive(bool active) override;

		SceneID GetSceneID() const override;
		void SetSceneID(const SceneID scene_id) override;

		void PushGameObject(GameObjectID goid) override;
		void PopGameObject(GameObjectID goid) override;

		bool SetSkyMap(ID3D11DeviceContext* p_context, int slot) override;
		void RenderSkyBox(ID3D11DeviceContext* p_context) override;

		void SetCBParam(CB_Environment& env) override;
		Environment* GetEnvironment() override;

#ifdef USE_IMGUI
		bool Save(const std::string& filepath) override;
#endif // USE_IMGUI

		// SetActive���ɍċA�I�ɏ���
		static void RecursiveSetActive(const std::shared_ptr<Transform>& transform, const bool is_active);


		// ==============================================
		// private variables
		// ==============================================
		bool m_is_active = false;
		SceneID m_scene_id = SceneManager::INVALID_SCENE_INDEX;

		// Async���[�h���ɎQ�Ƃ��邽�߂����̈ꎞ�I�ȕϐ�
		bool m_load_type_additive = false;

		std::unordered_set<GameObjectID> m_root_goid;

		Environment m_environment;
	};


	////////////////////////////////////////////////////////
	// Static member definition
	////////////////////////////////////////////////////////
	static constexpr const char* DEFAULT_NAME = "unnamed";


	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	// IResScene
	std::shared_ptr<IResScene> IResScene::CreateInterface()
	{
		return std::static_pointer_cast<IResScene>(std::make_shared<ResScene>());
	}

	std::shared_ptr<IResScene> IResScene::Create(const std::string& scene_name, const SceneID scene_id)
	{
		std::shared_ptr<IResScene> s_ptr = IResScene::CreateInterface();
		s_ptr->m_has_resource = true;
		s_ptr->SetName(scene_name);
		s_ptr->SetActive(true);
		s_ptr->SetSceneID(scene_id);

		return s_ptr;
	}

	std::shared_ptr<IResScene> IResScene::LoadAsync(const std::string& filepath, const SceneID scene_id, bool additive)
	{
		// ���̍쐬
		std::shared_ptr<IResScene> s_ptr(IResScene::CreateInterface());
		// ���[�h�ɕK�v�ȏ����Z�b�g
		s_ptr->SetFilePath(filepath);
		s_ptr->SetSceneID(scene_id);
		s_ptr->SetLoadAdditive(additive);

		// Set async loader
		std::lock_guard<std::mutex> lock(s_ptr->m_load_state_mutex);
		s_ptr->m_is_loading = true;
		s_ptr->m_is_loaded = false;
		s_ptr->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		AssetSystem::GetInstance().Add(std::bind(&IResScene::SetAsyncOnLoad, s_ptr));

		return s_ptr;
	}

	std::shared_ptr<IResScene> IResScene::Load(const std::string& filepath, const SceneID scene_id, bool additive)
	{
		// ���̍쐬
		std::shared_ptr<IResScene> s_ptr(IResScene::CreateInterface());
		// ���[�h�ɕK�v�ȏ����Z�b�g
		s_ptr->SetFilePath(filepath);
		s_ptr->SetSceneID(scene_id);
		s_ptr->SetLoadAdditive(additive);

		// Load Resource
		s_ptr->m_is_loading = true;
		s_ptr->m_is_loaded = false;
		s_ptr->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		s_ptr->OnLoad();
		// ���[�h��̏�ԃ`�F�b�N
		if (!s_ptr->HasResource())
		{
			s_ptr->Release();
			s_ptr.reset();
			return std::shared_ptr<IResScene>();
		}

		return s_ptr;
	}

	void IResScene::UnloadAsync(const std::shared_ptr<IResScene>& scene)
	{
		AssetSystem::GetInstance().Add(std::bind(&IResScene::SetAsyncOnUnload, scene));
	}

	void IResScene::Unload(const std::shared_ptr<IResScene>& scene)
	{
		scene->OnUnload();
	}

	void IResScene::Release()
	{
		/* nothing */
	}
	// ~IResScene

	// ResScene
#ifdef USE_IMGUI
	void ResScene::OnGUI()
	{
		// Save Button
		if (ImGui::BeginMenuBar())
		{
			// �Z�[�u�{�^��
			std::string button_label = "Save [";
			button_label += GetName();
			button_label += "]##Scene Menu Bar";
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
			if (ImGui::MenuItem(button_label.c_str()))
			{
				// �t�@�C�����擾
				std::string filepath;
				if (MyFunc::FetchSaveFileName(filepath, TEXT("Scene(*.scene)\0*.scene\0"), TEXT("Save Scene"), TEXT(".\\Asset\\Scenes")))
				{
					// �t�@�C���̏㏑�����͊g���q�����邽�߃`�F�b�N
					if (MyFunc::GetExtension(filepath).empty())
					{
						filepath += SCENE_EXTENSION;
					}
					Save(filepath);
				}
			}
			ImGui::PopStyleColor(1);
			ImGui::EndMenuBar();
		}
		// �V�[����
		{
			ImGui::IDWrapped id("Scene Name");
			std::string str = GetName();
			if(ImGui::InputText("##input scene name", &str, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				SetName(str);
			}
		}
		// �����
		m_environment.OnGUI();
	}

	void ResScene::OnGUIHierarchy()
	{
		// �q�G�����L�[�̃I�u�W�F�N�g�\��
		for (auto& goid : m_root_goid)
		{
			GameObjectManager::GetGameObject(goid)->OnGUIHierarchy();
		}
	}
#endif // USE_IMGUI

	bool ResScene::OnCreate()
	{
		/* nothing */
		return false;
	}

	void ResScene::SetAsyncOnLoad()
	{
		// Start loading
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Load Scene
		bool b_result = false;
		{
			std::ifstream ifs(this->GetFilePath(), std::ios::in | std::ios::binary);

			if (!ifs.is_open())
			{
				b_result = false;
			}
			else
			{
				cereal::BinaryInputArchive ar(ifs);
				ar(*this);
				b_result = true;
			}
		}

		// Finish loading
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_is_loading = false;
			m_is_loaded = true;
			if (b_result)
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
		}
	}

	void ResScene::OnLoad()
	{
		// Ready loading
		{
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Load Scene
		bool b_result = false;
		{
			std::ifstream ifs(this->GetFilePath(), std::ios::in | std::ios::binary);

			if (!ifs.is_open())
			{
				b_result = false;
			}
			else
			{
				cereal::BinaryInputArchive ar(ifs);

				ar(*this);
				b_result = true;
			}
		}

		// Finish loading
		{
			m_is_loading = false;
			m_is_loaded = true;
			if (b_result)
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
		}
	}

	void ResScene::SetAsyncOnUnload()
	{
		// Root�Q�[���I�u�W�F�N�g��j������
		while (!m_root_goid.empty())
		{
			const auto goid = *(m_root_goid.begin());
			const auto p_go = GameObjectManager::GetGameObject(goid);
			p_go->Destroy(-1.0f);
		}

		// �V�[���̔j��������������HasResource��false�ɂ���
		std::lock_guard<std::mutex> lock(m_load_state_mutex);
		m_has_resource = false;
	}

	void ResScene::OnUnload()
	{
		// Root�Q�[���I�u�W�F�N�g��j������
		while (!m_root_goid.empty())
		{
			const auto goid = *(m_root_goid.begin());
			const auto p_go = GameObjectManager::GetGameObject(goid);
			p_go->Destroy(-1.0f);
		}

		// �V�[���̔j��������������HasResource��false�ɂ���
		m_has_resource = false;
	}

	void ResScene::SetLoadAdditive(bool additive)
	{
		m_load_type_additive = additive;
	}

	bool ResScene::IsAdditive() const
	{
		return m_load_type_additive;
	}

	bool ResScene::GetActive() const
	{
		return m_is_active;
	}

	void ResScene::SetActive(const bool active)
	{
		// ����Ԃ���ω����Ȃ��Ȃ瑁�����^�[��
		if (m_is_active == active)
			return;
		// ����ԂɍX�V
		m_is_active = active;

		// Root���珇�ɏ�ԃ`�F�b�N
		for (const auto& goid : m_root_goid)
		{
			const auto& root_gameobject = GameObjectManager::GetGameObject(goid);
			RecursiveSetActive(root_gameobject->GetTransform(), active);
		}
	}

	SceneID ResScene::GetSceneID() const
	{
		return m_scene_id;
	}

	void ResScene::SetSceneID(const SceneID scene_id)
	{
		m_scene_id = scene_id;
	}

	void ResScene::PushGameObject(GameObjectID goid)
	{
		m_root_goid.insert(goid);
	}

	void ResScene::PopGameObject(GameObjectID goid)
	{
		m_root_goid.erase(goid);
	}

	bool ResScene::SetSkyMap(ID3D11DeviceContext* p_context, int slot)
	{
		return m_environment.SetSkyMap(p_context, slot);
	}

	void ResScene::RenderSkyBox(ID3D11DeviceContext* p_context)
	{
		m_environment.RenderSkyBox(p_context);
	}

	void ResScene::SetCBParam(CB_Environment& env)
	{
		m_environment.SetParam(env);
	}

	Environment* ResScene::GetEnvironment()
	{
		return &m_environment;
	}

#ifdef USE_IMGUI
	bool ResScene::Save(const std::string& filepath)
	{
		// �V�����t�@�C���p�X���Z�b�g
		SetFilePath(filepath);
		// �V���A���C�Y
		// Binary
		{
			std::ofstream ofs(filepath.c_str(), std::ios::out | std::ios::binary);
			if (!ofs.is_open())
			{
				assert(0 && "failed save file. ResScene::Save()");
				return false;
			}
			cereal::BinaryOutputArchive ar(ofs);
			ar(*this);
		}
		// Json
#ifdef SAVE_JSON
		{
			const std::string json_filepath = filepath + "J";
			std::ofstream ofs(json_filepath.c_str(), std::ios::out | std::ios::binary);
			if (!ofs.is_open())
			{
				assert(0 && "failed save file. ResScene::Save()");
				return false;
			}
			cereal::JSONOutputArchive ar(ofs);
			ar(*this);
		}
#endif// ~SAVE_JSON

		return true;
	}
#endif // USE_IMGUI

	void ResScene::RecursiveSetActive(const std::shared_ptr<Transform>& transform, const bool is_active)
	{
		const auto gameobject = transform->GetGameObject();
		// ��A�N�e�B�u�Ȃ炻��ȉ��͒T�����Ȃ�
		if (!gameobject->GetActiveHierarchy())
		{
			return;
		}
		// �ω�������ԂɑΉ������֐����Ă�
		{
			const auto gameobject_id = gameobject->GetGameObjectID();
			if (is_active)
			{
				// MonoBehaviour::OnEnable
				MonoBehaviourManager::OnEnable(gameobject_id);
			}
			else
			{
				// MonoBehaviour::OnDisable
				MonoBehaviourManager::OnDisable(gameobject_id);
			}
			// �R���W�����̗L���A�����؂�ւ�
			PhysicsSystem::GetInstance()->SetCollisionActive(gameobject_id, is_active);
		}
		// �q�I�u�W�F�N�g�����݂���Ȃ�ċA�����Ŏ��s����
		const auto child_num = transform->GetChildCount();
		if (child_num > 0)
		{
			for (int i = 0; i < child_num; ++i)
			{
				RecursiveSetActive(transform->GetChild(i), is_active);
			}
		}
	}

	// ~ResScene


}// namespace TKGEngine

CEREAL_REGISTER_TYPE(TKGEngine::ResScene);
CEREAL_CLASS_VERSION(TKGEngine::ResScene, 1);
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::IResScene, TKGEngine::ResScene)
// archive����load save���g�p����B�������
CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(TKGEngine::ResScene, cereal::specialization::member_load_save)