
#include "Systems/inc/ISceneSystem.h"
#include "Systems/inc/IGraphics.h"
#include "Systems/inc/IWindow.h"

#ifdef USE_IMGUI
#include "Systems/inc/IGUI.h"
#endif

#include "Managers/ObjectManager.h"
#include "Managers/GameObjectManager.h"
#include "Managers/ComponentManager.h"
#include "Managers/MonoBehaviourManager.h"
#include "Managers/RendererManager.h"
#include "Managers/CameraManager.h"
#include "Managers/LightManager.h"
#include "Managers/SceneManager.h"
#include "Managers/AnimatorManager.h"
#include "Systems/inc/PhysicsSystem.h"
#include "Application/Resource/inc/Effect.h"
#include "Utility/inc/template_thread.h"

#include <cassert>
#include <filesystem>


namespace TKGEngine
{
	/// ========================================================
	/// @class	SceneSystem
	/// @brief	Scene system
	/// 
	/// ========================================================
	class SceneSystem
		: public ISceneSystem
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		SceneSystem();
		virtual ~SceneSystem();
		SceneSystem(SceneSystem&&) = default;
		SceneSystem(const SceneSystem&) = delete;
		SceneSystem& operator=(const SceneSystem&) = delete;

		// ISystem
		bool OnInit() override;
		void OnTerm() override;
		void OnFrameBegin(const FrameEventArgs& args) override;
		void OnFrameRender(const FrameEventArgs& args) override;
		void OnFrameEnd(const FrameEventArgs& args) override;
		void OnResize(const ResizeEventArgs& args) override;
		// ~ISystem


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					cereal::make_nvp("PhysicsSystem", *PhysicsSystem::GetInstance())
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
		// �}�l�[�W���[�N���X�̏�������
		bool OnInitManager();
		// �}�l�[�W���[�N���X�̏I�[����
		void OnTermManager();

#ifdef USE_IMGUI
		// Play�{�^���������ꂽ
		void OnPushPlay();
		// Stop�{�^���������ꂽ
		void OnPushStop();

		// GlobalSetting�̃Z�[�u
		void SaveGlobalSetting();
#endif
		// GlobalSetting�̃��[�h
		void LoadGlobalSetting();

		// ==============================================
		// private variables
		// ==============================================

		// Global Setting�̃t�@�C���p�X
		static constexpr const char* GLOBAL_SETTING_FILEPATH = "./Setting/GlobalSetting.system";
#ifdef SAVE_JSON
		static constexpr const char* GLOBAL_SETTING_FILEPATH_J = "./Setting/GlobalSetting.systemJ";
#endif// ~SAVE_JSON

#ifdef USE_IMGUI
		static constexpr const char* TempFolderPath = "./Temporary_Scene_Save_Folder";
#endif

		// Effect�X�V�p�X���b�h
		ThreadPool m_effect_update_thread = ThreadPool(4);
		std::future<void> m_effect_thread_result;
	};

	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	std::unique_ptr<ISceneSystem> ISceneSystem::CreateInterface()
	{
		return std::make_unique<SceneSystem>();
	}

	SceneSystem::SceneSystem()
	{
		/* nothing */
	}

	SceneSystem::~SceneSystem()
	{
		/* nothing */
	}


	bool SceneSystem::OnInit()
	{
		// �R���|�[�l���g�̓o�^����
		ComponentManager::SetUpComponentTypeID();
		// �����_���[�Ǘ��N���X�̏�������
		RendererManager::Initialize();

		// �}�l�[�W���[�N���X�̏�������
		const bool result_init = OnInitManager();

		// Global Setting�̃��[�h
		LoadGlobalSetting();

		return result_init;
	}

	void SceneSystem::OnTerm()
	{
		OnTermManager();

#ifdef USE_IMGUI
		// Temp�t�H���_��j������
		std::filesystem::remove_all(TempFolderPath);
#endif
	}

	void SceneSystem::OnFrameBegin(const FrameEventArgs& args)
	{
		// �A�j���[�V�����̍X�V���s�����̂����X�g�Ɏ��o��
		AnimatorManager::CheckUpdatable();

#ifdef USE_IMGUI
		if (IGUI::Get().IsPlaying())
#endif
		{
			// �A�j���[�V�����̍X�V�����āA�g�����X�t�H�[���ɓK�p����
			AnimatorManager::ApplyAnimationTransform();

			// MonoBehaviour�p���̃N���X��OnFrame�Ȋ֐������s����
			MonoBehaviourManager::Run();
		}

		// Effect�̍X�V�X���b�h�쐬
		// (join����܂ŁAEffect�ɐG��Ȃ�)
		m_effect_thread_result = m_effect_update_thread.Add(Effect::OnFrameUpdate, args.unscaled_delta_time, args.delta_time);

		// �����̍X�V
		PhysicsSystem::FrameUpdate(args.delta_time);

		// �{�[���̃g�����X�t�H�[�������ǂ��ăA�j���[�V�����s����v�Z����
		AnimatorManager::UpdateAnimationMatrix();

		// Scene�Ǘ�CBuffer�Ɣ񓯊����X�g�̍X�V
		SceneManager::FrameUpdate();

		// Effect�X���b�h��join
		m_effect_thread_result.wait();

		// Object::DestroyPool�̍X�V
		ObjectManager::FrameUpdate(args.unscaled_delta_time);
	}

	void SceneSystem::OnFrameRender(const FrameEventArgs& args)
	{
		// �V�[�����̃��C�g���\�[�g����
		// TODO : �J�������ƂɎg�p���郉�C�g���J�����O���鏈�����K�v
		LightManager::SortSceneLight();

		// �`��
		CameraManager::Run();
	}

	void SceneSystem::OnFrameEnd(const FrameEventArgs& args)
	{
		Effect::OnFrameEnd();

#ifdef USE_IMGUI
		// Global Setting��Save�{�^���������ꂽ���̓Z�[�u����
		if (IGUI::Get().IsPushSaveGlobalSetting())
		{
			SaveGlobalSetting();
		}

		// Play�������ƌ��݂̃V�[����Temp�ɏ����o���A�}�l�[�W���[�N���X����蒼���ăV�[�������[�h����
		if (IGUI::Get().IsPlayedOnFrame())
		{
			OnPushPlay();
		}
		// Stop�������ƃV�[���V�X�e������蒼���āA�����o�����V�[�������[�h����
		else if (IGUI::Get().IsStoppedOnFrame())
		{
			OnPushStop();
		}
#endif
	}

	void SceneSystem::OnResize(const ResizeEventArgs& args)
	{
		CameraManager::OnResizeWindow();
		Effect::OnResizeWindow(args.width, args.height);
	}

	bool SceneSystem::OnInitManager()
	{
		PhysicsSystem::Create();

		if (!CameraManager::Initialize())
		{
			assert(0 && "failed CameraManager::Initialize() SceneSystem::OnInit()");
			return false;
		}
		if (!LightManager::Create())
		{
			assert(0 && "failed LightManager::Create() SceneSystem::OnInit()");
			return false;
		}
		Effect::Initialize(IGraphics::Get().Device(), IGraphics::Get().DC(0, Graphics::DC_RENDER_PATH::DC_RP_MAIN), IWindow::Get().GetScreenWidth(), IWindow::Get().GetScreenHeight());

		if (!SceneManager::Initialize())
		{
			assert(0 && "failed SceneManager::Initialize() SceneSystem::OnInit()");
			return false;
		}

		return true;
	}

	void SceneSystem::OnTermManager()
	{
		Effect::Destroy();
		LightManager::Destroy();
		SceneManager::Terminate();
		CameraManager::Terminate();
		PhysicsSystem::Destroy();
	}

#ifdef USE_IMGUI
	void SceneSystem::OnPushPlay()
	{
		// Temp�t�H���_�Ɍ��݂̃V�[���������o��
		{
			// �c���Ă���\�������邽��Temp�t�H���_��j������
			std::filesystem::remove_all(TempFolderPath);
			// Temp�t�H���_�쐬
			std::filesystem::create_directory(TempFolderPath);
			// ���݂̃V�[�����X�g���t�H���_�ɏ����o��
			SceneManager::SaveSceneList(TempFolderPath);
		}
		// �}�l�[�W���[�N���X����蒼��
		OnTermManager();
		OnInitManager();
		// Global Setting�̃��[�h
		LoadGlobalSetting();
		// Temp�t�H���_���̃V�[�������[�h����
		SceneManager::LoadSceneList(TempFolderPath);
		// Input���[�h��Game���[�h�ɂ���
		IGUI::Get().SetActiveInputScene(true);
	}

	void SceneSystem::OnPushStop()
	{
		// �}�l�[�W���[�N���X����蒼��
		OnTermManager();
		OnInitManager();
		// Global Setting�̃��[�h
		LoadGlobalSetting();
		// Temp�t�H���_���̃V�[�������[�h����
		SceneManager::LoadSceneList(TempFolderPath);
		// Temp�t�H���_��j������
		std::filesystem::remove_all(TempFolderPath);
		// Input���[�h��Edit���[�h�ɂ���
		IGUI::Get().SetActiveInputScene(false);
	}

	void SceneSystem::SaveGlobalSetting()
	{
		// �V���A���C�Y
		// Binary
		{
			std::ofstream ofs(GLOBAL_SETTING_FILEPATH, std::ios::out | std::ios::binary);
			if (!ofs.is_open())
			{
				assert(0 && "failed save file. Save Global Setting");
				return;
			}
			cereal::BinaryOutputArchive ar(ofs);
			ar(*this);
		}
		// Json
#ifdef SAVE_JSON
		{
			std::ofstream ofs(GLOBAL_SETTING_FILEPATH_J, std::ios::out | std::ios::binary);
			if (!ofs.is_open())
			{
				assert(0 && "failed save file. Save Global Setting");
				return;
			}
			cereal::JSONOutputArchive ar(ofs);
			ar(*this);
		}
#endif// ~SAVE_JSON
	}
#endif

	void SceneSystem::LoadGlobalSetting()
	{
		// �t�@�C�����Ȃ���Α������^�[��
		std::ifstream ifs(GLOBAL_SETTING_FILEPATH, std::ios::in | std::ios::binary);
		if (!ifs.is_open())
		{
			return;
		}
		cereal::BinaryInputArchive ar(ifs);
		ar(*this);
	}


}// namespace TKGEngine

CEREAL_REGISTER_TYPE(TKGEngine::SceneSystem);