
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
		// マネージャークラスの初期生成
		bool OnInitManager();
		// マネージャークラスの終端処理
		void OnTermManager();

#ifdef USE_IMGUI
		// Playボタンが押された
		void OnPushPlay();
		// Stopボタンが押された
		void OnPushStop();

		// GlobalSettingのセーブ
		void SaveGlobalSetting();
#endif
		// GlobalSettingのロード
		void LoadGlobalSetting();

		// ==============================================
		// private variables
		// ==============================================

		// Global Settingのファイルパス
		static constexpr const char* GLOBAL_SETTING_FILEPATH = "./Setting/GlobalSetting.system";
#ifdef SAVE_JSON
		static constexpr const char* GLOBAL_SETTING_FILEPATH_J = "./Setting/GlobalSetting.systemJ";
#endif// ~SAVE_JSON

#ifdef USE_IMGUI
		static constexpr const char* TempFolderPath = "./Temporary_Scene_Save_Folder";
#endif

		// Effect更新用スレッド
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
		// コンポーネントの登録処理
		ComponentManager::SetUpComponentTypeID();
		// レンダラー管理クラスの初期処理
		RendererManager::Initialize();

		// マネージャークラスの初期処理
		const bool result_init = OnInitManager();

		// Global Settingのロード
		LoadGlobalSetting();

		return result_init;
	}

	void SceneSystem::OnTerm()
	{
		OnTermManager();

#ifdef USE_IMGUI
		// Tempフォルダを破棄する
		std::filesystem::remove_all(TempFolderPath);
#endif
	}

	void SceneSystem::OnFrameBegin(const FrameEventArgs& args)
	{
		// アニメーションの更新を行うものをリストに取り出す
		AnimatorManager::CheckUpdatable();

#ifdef USE_IMGUI
		if (IGUI::Get().IsPlaying())
#endif
		{
			// アニメーションの更新をして、トランスフォームに適用する
			AnimatorManager::ApplyAnimationTransform();

			// MonoBehaviour継承のクラスのOnFrameな関数を実行する
			MonoBehaviourManager::Run();
		}

		// Effectの更新スレッド作成
		// (joinするまで、Effectに触らない)
		m_effect_thread_result = m_effect_update_thread.Add(Effect::OnFrameUpdate, args.unscaled_delta_time, args.delta_time);

		// 物理の更新
		PhysicsSystem::FrameUpdate(args.delta_time);

		// ボーンのトランスフォームをたどってアニメーション行列を計算する
		AnimatorManager::UpdateAnimationMatrix();

		// Scene管理CBufferと非同期リストの更新
		SceneManager::FrameUpdate();

		// Effectスレッドをjoin
		m_effect_thread_result.wait();

		// Object::DestroyPoolの更新
		ObjectManager::FrameUpdate(args.unscaled_delta_time);
	}

	void SceneSystem::OnFrameRender(const FrameEventArgs& args)
	{
		// シーン内のライトをソートする
		// TODO : カメラごとに使用するライトをカリングする処理が必要
		LightManager::SortSceneLight();

		// 描画
		CameraManager::Run();
	}

	void SceneSystem::OnFrameEnd(const FrameEventArgs& args)
	{
		Effect::OnFrameEnd();

#ifdef USE_IMGUI
		// Global SettingのSaveボタンが押された時はセーブする
		if (IGUI::Get().IsPushSaveGlobalSetting())
		{
			SaveGlobalSetting();
		}

		// Playを押すと現在のシーンをTempに書き出し、マネージャークラスを作り直してシーンをロードする
		if (IGUI::Get().IsPlayedOnFrame())
		{
			OnPushPlay();
		}
		// Stopを押すとシーンシステムを作り直して、書き出したシーンをロードする
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
		// Tempフォルダに現在のシーンを書き出す
		{
			// 残っている可能性があるためTempフォルダを破棄する
			std::filesystem::remove_all(TempFolderPath);
			// Tempフォルダ作成
			std::filesystem::create_directory(TempFolderPath);
			// 現在のシーンリストをフォルダに書き出す
			SceneManager::SaveSceneList(TempFolderPath);
		}
		// マネージャークラスを作り直す
		OnTermManager();
		OnInitManager();
		// Global Settingのロード
		LoadGlobalSetting();
		// Tempフォルダ内のシーンをロードする
		SceneManager::LoadSceneList(TempFolderPath);
		// InputモードをGameモードにする
		IGUI::Get().SetActiveInputScene(true);
	}

	void SceneSystem::OnPushStop()
	{
		// マネージャークラスを作り直す
		OnTermManager();
		OnInitManager();
		// Global Settingのロード
		LoadGlobalSetting();
		// Tempフォルダ内のシーンをロードする
		SceneManager::LoadSceneList(TempFolderPath);
		// Tempフォルダを破棄する
		std::filesystem::remove_all(TempFolderPath);
		// InputモードをEditモードにする
		IGUI::Get().SetActiveInputScene(false);
	}

	void SceneSystem::SaveGlobalSetting()
	{
		// シリアライズ
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
		// ファイルがなければ早期リターン
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