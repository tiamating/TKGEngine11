#include "InitSceneManager.h"

#include "Components/inc/CUIRenderer.h"
#include "../../Application/Resource/inc/Mesh.h"

REGISTERCOMPONENT(TKGEngine::InitSceneManager);

namespace TKGEngine
{
	/////////////////////////////////////////////////////////////////////////
	// 実体生成時に呼び出される
	/////////////////////////////////////////////////////////////////////////
	//void InitSceneManager::Awake(){}

	/////////////////////////////////////////////////////////////////////////
	// Updateが呼ばれる前に一度だけ呼び出される
	/////////////////////////////////////////////////////////////////////////
	void InitSceneManager::Start()
	{
		// UIの参照取得
		if (const auto ui_object = IGameObject::Find("unitychanUI"))
		{
			m_unitychan_ui = ui_object->GetComponent<UIRenderer>();
		}
		if (const auto button_obj = IGameObject::Find("PushButton"))
		{
			m_button_ui = button_obj->GetComponent<UIRenderer>();
		}

		// 未使用Meshの破棄
		Mesh::RemoveUnused();
	}

	/////////////////////////////////////////////////////////////////////////
	// 毎フレーム呼び出される
	/////////////////////////////////////////////////////////////////////////
	void InitSceneManager::Update()
	{
		const float delta_time = ITime::Get().DeltaTime();

		// UIに動きをつける
		if (const auto ui = m_unitychan_ui.GetWeak().lock())
		{
			const auto current_euler = ui->EulerAngles();
			VECTOR3 next_euler = VECTOR3::Zero;
			// ロード中
			if (!m_finish_loading)
			{
				// 回転
				next_euler = current_euler + m_rotate_speed * delta_time;
			}
			// ロード完了
			else
			{
				// 正面で止める
				auto rotation_euler = current_euler + m_rotate_speed * 3.0f * delta_time;
				// X
				if(!m_finish_rotate_x)
				{
					if(rotation_euler.x >= 360.0f)
					{
						m_finish_rotate_x = true;
						rotation_euler.x = 0.0f;
					}
					next_euler.x = rotation_euler.x;
				}
				// Y
				if (!m_finish_rotate_y)
				{
					if (rotation_euler.y >= 360.0f)
					{
						m_finish_rotate_y = true;
						rotation_euler.y = 0.0f;
					}
					next_euler.y = rotation_euler.y;
				}
				// Z
				next_euler.z= rotation_euler.z;
			}
			ui->EulerAngles(next_euler);
		}

		// シーン破棄完了を待機
		if (SceneManager::GetUnloadingSceneNum() != 0)
			return;

		// ロード完了待機
		if (!m_finish_loading && SceneManager::LoadSceneAsync("./Asset/Scenes/PlayerScene.scene", true))
		{
			m_finish_loading = true;
			// PushButtonを表示
			if(const auto button_ui = m_button_ui.GetWeak().lock())
			{
				button_ui->Enabled(true);
			}
		}

		// ロード終了時
		if (m_finish_loading)
		{
			if (IInput::Get().GetPadDown(0, Input::GamePad::A))
			{
				const auto current_id = SceneManager::GetMainSceneID();
				SceneManager::SetMainScene("./Asset/Scenes/PlayerScene.scene");
				SceneManager::UnloadSceneAsync(current_id);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////
	// Update後に毎フレーム呼び出される
	/////////////////////////////////////////////////////////////////////////
	//void InitSceneManager::LateUpdate(){}

	/////////////////////////////////////////////////////////////////////////
	// 実体が有効になった時に呼び出される
	/////////////////////////////////////////////////////////////////////////
	//void InitSceneManager::OnEnable(){}

	/////////////////////////////////////////////////////////////////////////
	// 実体が無効になった時に呼び出される
	/////////////////////////////////////////////////////////////////////////
	//void InitSceneManager::OnDisable(){}

	/////////////////////////////////////////////////////////////////////////
	// 実体破棄時に呼び出される
	/////////////////////////////////////////////////////////////////////////
	//void InitSceneManager::OnDestroyed(){}


	/////////////////////////////////////////////////////////////////////////
	// GUI設定
	/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	void InitSceneManager::OnGUI()
	{
	}
#endif
}
