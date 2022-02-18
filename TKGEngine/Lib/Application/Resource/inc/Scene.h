#pragma once
#include "Systems/inc/TKGEngine_Defined.h"

#include "Environment.h"
#include "Utility/inc/myfunc_vector.h"
#include "Application/Resource/src/Scene/IResScene.h"
#include "Application/Objects/inc/IGameObject.h"

#include <string>

#include <memory>


struct ID3D11DeviceContext;

namespace TKGEngine
{
	class ICamera;
}// namespace TKGEngie

namespace TKGEngine
{
	class Scene
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Scene();
		virtual ~Scene();
		Scene(const Scene&);
		void operator=(const Scene&) = delete;

#ifdef USE_IMGUI
		void OnGUI() const;
		void OnGUIHierarchy() const;
		bool Save(const std::string& filepath);
#endif // USE_IMGUI
		
		void Create(const std::string& scene_name, const SceneID scene_id);

		void Load(const std::string& filepath, const SceneID scene_id, bool additive);
		void LoadAsync(const std::string& filepath, const SceneID scene_id, bool additive);

		void Unload() const;
		void UnloadAsync() const;

		[[nodiscard]] bool GetActive() const;
		void SetActive(bool is_active) const;

		SceneID GetSceneID() const;

		void PushGameObject(GameObjectID goid) const;
		void PopGameObject(GameObjectID goid) const;

		bool SetSkyMap(ID3D11DeviceContext* p_context, int slot) const;
		void RenderSkyBox(ID3D11DeviceContext* p_context) const;

		void SetCBParam(CB_Environment& env) const;
		[[nodiscard]] Environment* GetEnvironment() const;

		[[nodiscard]] bool IsLoaded() const;
		[[nodiscard]] bool HasScene() const;
		[[nodiscard]] const char* GetName() const;

		[[nodiscard]] unsigned GetHash() const;

		// 追加シーンとしてロードされたか
		[[nodiscard]] bool IsAdditive() const;

		// ==============================================
		// public variables
		// ==============================================


	private:
		// ==============================================
		// private methods
		// ==============================================



		// ==============================================
		// private variables
		// ==============================================
		std::shared_ptr<IResScene> m_res_scene = nullptr;
	};

}	// namespace TKGEngine
