
#include "../../inc/Scene.h"

#include "IResScene.h"
#include "Managers/SceneManager.h"


namespace TKGEngine
{
	// ==================================================
	// class methods
	// ==================================================
	Scene::Scene()
	{
		/* nothing */
	}

	Scene::~Scene()
	{
		/* nothing */
	}

	Scene::Scene(const Scene& scene)
	{
		this->m_res_scene = scene.m_res_scene;
	}

#ifdef USE_IMGUI
	void Scene::OnGUI() const
	{
		if (m_res_scene)
			m_res_scene->OnGUI();
	}

	void Scene::OnGUIHierarchy() const
	{
		if (m_res_scene)
			m_res_scene->OnGUIHierarchy();
	}

	bool Scene::Save(const std::string& filepath)
	{
		if (m_res_scene)
			return m_res_scene->Save(filepath);
		return false;
	}
#endif // USE_IMGUI
	
	void Scene::Create(const std::string& scene_name, const SceneID scene_id)
	{
		m_res_scene = IResScene::Create(scene_name, scene_id);
	}

	void Scene::Load(const std::string& filepath, const SceneID scene_id, bool additive)
	{
		m_res_scene = IResScene::Load(filepath, scene_id, additive);
	}

	void Scene::LoadAsync(const std::string& filepath, const SceneID scene_id, bool additive)
	{
		m_res_scene = IResScene::LoadAsync(filepath, scene_id, additive);
	}

	void Scene::Unload() const
	{
		if (m_res_scene)
			IResScene::Unload(m_res_scene);
	}

	void Scene::UnloadAsync() const
	{
		if (m_res_scene)
			IResScene::UnloadAsync(m_res_scene);
	}

	bool Scene::GetActive() const
	{
		return m_res_scene == nullptr ? false : m_res_scene->GetActive();
	}

	void Scene::SetActive(bool is_active) const
	{
		if (m_res_scene)
			m_res_scene->SetActive(is_active);
	}

	SceneID Scene::GetSceneID() const
	{
		return m_res_scene ? m_res_scene->GetSceneID() : SceneManager::INVALID_SCENE_INDEX;
	}

	void Scene::PushGameObject(GameObjectID goid) const
	{
		if (m_res_scene)
			m_res_scene->PushGameObject(goid);
	}

	void Scene::PopGameObject(GameObjectID goid) const
	{
		if (m_res_scene)
			m_res_scene->PopGameObject(goid);
	}

	bool Scene::SetSkyMap(ID3D11DeviceContext* p_context, int slot) const
	{
		return m_res_scene == nullptr ? false : m_res_scene->SetSkyMap(p_context, slot);
	}

	void Scene::RenderSkyBox(ID3D11DeviceContext* p_context) const
	{
		if (m_res_scene)
			m_res_scene->RenderSkyBox(p_context);
	}

	void Scene::SetCBParam(CB_Environment& env) const
	{
		if (m_res_scene)
			m_res_scene->SetCBParam(env);
	}

	Environment* Scene::GetEnvironment() const
	{
		if (m_res_scene)
			return m_res_scene->GetEnvironment();
		return nullptr;
	}

	bool Scene::IsLoaded() const
	{
		return m_res_scene != nullptr ? m_res_scene->IsLoaded() : false;
	}

	bool Scene::HasScene() const
	{
		return m_res_scene == nullptr ? false : m_res_scene->HasResource();
	}

	const char* Scene::GetName() const
	{
		return m_res_scene == nullptr ? nullptr : m_res_scene->GetName();
	}

	unsigned Scene::GetHash() const
	{
		return m_res_scene == nullptr ? 0 : m_res_scene->GetHash();
	}

	bool Scene::IsAdditive() const
	{
		return m_res_scene ? m_res_scene->IsAdditive() : false;
	}

}// namespace TKGEngine