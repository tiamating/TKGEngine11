#pragma once

#include "Application/Resource/inc/AssetDataBase.h"

#include "Application/inc/SystemSetting.h"
#include "Application/Resource/inc/Environment.h"
#include "Application/Objects/inc/IGameObject.h"

#include "Utility/inc/myfunc_vector.h"

#include <string>

#include <memory>

struct ID3D11DeviceContext;

namespace TKGEngine
{
	class ICamera;

}// namespace TKGEngie

namespace TKGEngine
{
	/// <summary>
	/// Scene Resource interface
	/// </summary>
	class IResScene
		: public AssetDataBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		IResScene() = default;
		virtual ~IResScene() = default;
		IResScene(const IResScene&) = delete;
		IResScene& operator=(const IResScene&) = delete;

#ifdef USE_IMGUI
		virtual void OnGUIHierarchy() = 0;
		virtual bool Save(const std::string& filepath) = 0;
#endif // USE_IMGUI
		
		static std::shared_ptr<IResScene> Create(const std::string& scene_name, const SceneID scene_id);
		static std::shared_ptr<IResScene> LoadAsync(const std::string& filepath, const SceneID scene_id, bool additive);
		static std::shared_ptr<IResScene> Load(const std::string& filepath, const SceneID scene_id, bool additive);

		static void UnloadAsync(const std::shared_ptr<IResScene>& scene);
		static void Unload(const std::shared_ptr<IResScene>& scene);

		// AssetDataBase
		void Release() override;
		// ~AssetDataBase

		virtual bool GetActive() const = 0;
		virtual void SetActive(bool active) = 0;

		virtual SceneID GetSceneID() const = 0;
		virtual void SetSceneID(const SceneID scene_id) = 0;

		virtual void PushGameObject(GameObjectID goid) = 0;
		virtual void PopGameObject(GameObjectID goid) = 0;

		virtual bool SetSkyMap(ID3D11DeviceContext* p_context, int slot) = 0;
		virtual void RenderSkyBox(ID3D11DeviceContext* p_context) = 0;

		virtual void SetCBParam(CB_Environment& env) = 0;
		virtual Environment* GetEnvironment() = 0;

		virtual bool IsAdditive() const = 0;

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
					cereal::base_class<AssetDataBase>(this)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
		virtual void SetAsyncOnLoad() = 0;
		virtual void OnLoad() = 0;

		virtual void SetAsyncOnUnload() = 0;
		virtual void OnUnload() = 0;

		virtual void SetLoadAdditive(bool additive) = 0;

		static std::shared_ptr<IResScene> CreateInterface();

		// ==============================================
		// private variables
		// ==============================================
		/* nothing */

	};

}// namespace TKGEngine

CEREAL_REGISTER_TYPE(TKGEngine::IResScene)
CEREAL_CLASS_VERSION(TKGEngine::IResScene, 1)
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::AssetDataBase, TKGEngine::IResScene)