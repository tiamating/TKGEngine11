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

		// �V�[�����X�g�̈ꊇ�Z�[�u�A���[�h
		static void SaveSceneList(const std::string& folder);
		static void LoadSceneList(const std::string& folder);
#endif// #ifdef USE_IMGUI

		// CBuffer���Z�b�g����
		static void SetPipeline();

		static bool SetSkyMap(ID3D11DeviceContext* p_context, int slot);
		// Main�V�[����SceneSetting���g�p���ĕ`�悷��
		static void RenderSkyBox(const std::shared_ptr<ICamera>& camera);

		// Async�ɃZ�b�g���ꂽ���̂̏�Ԃ��X�V����
		// MainScene���ς���Time�̍X�V������
		static void FrameUpdate();

		// �V�[���쐬
		static void CreateScene(const std::string& scene_name);

		// ���[�h
		static void LoadScene(const std::string& filepath, bool additive = false);

		/// <summary>
		/// �񓯊����[�h
		/// </summary>
		/// <returns>���[�h�ς݃V�[�����X�g�ɑ��݂���ꍇ��True</returns>
		/// <returns>���[�h������������True</returns>
		/// <returns>���[�h����False</returns>
		static bool LoadSceneAsync(const std::string& filepath, bool additive = false);

		// �A�����[�h
		static void UnloadScene(SceneID scene_id);
		static void UnloadSceneAsync(SceneID scene_id);
		static void UnloadScene(const std::string& scene_filepath);
		static void UnloadSceneAsync(const std::string& scene_filepath);

		// GO�̃V�[���ړ��p�֐�
		static void DontDestroyOnLoad(GameObjectID goid);
		static void DontDestroyOnLoad(const std::shared_ptr<IGameObject>& gameobject);
		static void MoveGameObjectToScene(GameObjectID goid, SceneID scene_id);
		static void MoveGameObjectToScene(GameObjectID goid, const std::string& scene_filepath);
		static void MoveGameObjectToScene(const std::shared_ptr<IGameObject>& gameobject, SceneID scene_id);
		static void MoveGameObjectToScene(const std::shared_ptr<IGameObject>& gameobject, const std::string& scene_filepath);

		// GO�̐������p�֐�
		static void RegisterGameObjectToScene(GameObjectID goid, SceneID scene_id);

		// GO�̔j�����p�֐�
		static void EraseGameObjectFromScene(GameObjectID goid);

		// �V�[����Ԏ擾�A�ύX�p�֐�
		static bool GetActive(SceneID scene_id);
		static void SetActive(SceneID scene_id, bool is_active);
		static bool GetActive(const std::string& scene_filepath);
		static void SetActive(const std::string& scene_filepath, bool is_active);

		// �L���ȃV�[��ID�����ׂĎ擾
		static void GetActiveSceneList(std::unordered_set<SceneID>& buf);
		// ���C���V�[�����擾
		static SceneID GetMainSceneID();
		static Scene* GetMainScene();
		// Scene�����݂��Ȃ��Ƃ�false
		static bool SetMainScene(SceneID scene_id);
		static bool SetMainScene(const std::string& scene_filepath);

		static Scene* GetScene(SceneID scene_id);
		static Scene* GetScene(const std::string& scene_filepath);

		// ���[�h�A�A�����[�h���̃V�[����
		static int GetLoadingSceneNum();
		static int GetUnloadingSceneNum();


		// ==============================================
		// public variables
		// ==============================================
		// �V�[���͈̔͊O�������V�[���ԍ�
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
		// GO�̓o�^�A�j���p�֐�
		static void PushGameObject(GameObjectID goid, SceneID scene_id);
		static void PopGameObject(GameObjectID goid, SceneID scene_id);

		static bool CreateConstantBuffer();
		
		/// <summary>
		/// �񓯊��`�F�b�N
		/// </summary>
		/// <param name="termination">�}�l�[�W���[�j������Single�̏������s��Ȃ�</param>
		/// <returns>�`�F�b�N��Ƀ��X�g������Ȃ�true</returns>
		static bool CheckAsyncLoad(bool termination = false);
		static bool CheckAsyncUnload();

		// ==============================================
		// private variables
		// ==============================================
		// DONT_DESTROY_SCENE�̃V�[���ԍ�
		static constexpr SceneID DONT_DESTROY_SCENE_INDEX = -1;

		static Shader m_skybox_shader;
		static ConstantBuffer m_cb_environment;
		static ConstantBuffer m_cb_time;
		// ���݂̃��C���V�[�����J�n���Ă���̌o�ߎ���
		static float m_total_time;
		// ���݂̃��C���V�[��ID
		static SceneID m_main_scene_id;
		// �L���ȃV�[���̃��X�g
		static std::unordered_set<SceneID> m_active_scenes;
		// �V�[�����X�g
		static std::unordered_map<std::string, std::unique_ptr<Scene>> m_scene_list;
		// ���[�h�A�j�����s���Ă���V�[�����Ǘ����郊�X�g
		static std::unordered_map<std::string, std::unique_ptr<Scene>> m_loading_scene_list;
		static std::unordered_map<std::string, std::unique_ptr<Scene>> m_unloading_scene_list;
		// <SceneID, Key������>�Ή����X�g
		static std::unordered_map<SceneID, std::string> m_key_table;
		// �j������Ȃ��V�[��
		static std::unique_ptr<Scene> m_dont_destroy_scene;
		// ���݂̃��j�[�N�ȃV�[���ԍ�
		static SceneID m_current_unique_id;
	};


}// namespace TKGEngine