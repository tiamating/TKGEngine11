#pragma once

#include "../../Objects/inc/IGameObject.h"

#include <unordered_map>
#include <memory>
#include <mutex>


namespace TKGEngine
{
	/// <summary>
	/// GameObject manager. (static class)
	/// </summary>
	class GameObjectManager
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		GameObjectManager() = default;
		virtual ~GameObjectManager() = default;
		GameObjectManager(const GameObjectManager&) = delete;
		GameObjectManager& operator=(const GameObjectManager&) = delete;

		static std::shared_ptr<IGameObject> CreateGameObject();
		static std::shared_ptr<IGameObject> CreateGameObject(const char* name);
		static std::shared_ptr<IGameObject> CreateChild(const std::shared_ptr<IGameObject>& parent);
		static std::shared_ptr<IGameObject> CreateChild(const std::shared_ptr<IGameObject>& parent, const char* name);

		// Create primitive
		static std::shared_ptr<IGameObject> CreateCube();
		static std::shared_ptr<IGameObject> CreateCube(const char* name);
		static std::shared_ptr<IGameObject> CreateCube(const std::shared_ptr<IGameObject>& parent);
		static std::shared_ptr<IGameObject> CreateCube(const std::shared_ptr<IGameObject>& parent, const char* name);
		static std::shared_ptr<IGameObject> CreateSphereSmooth();
		static std::shared_ptr<IGameObject> CreateSphereSmooth(const char* name);
		static std::shared_ptr<IGameObject> CreateSphereSmooth(const std::shared_ptr<IGameObject>& parent);
		static std::shared_ptr<IGameObject> CreateSphereSmooth(const std::shared_ptr<IGameObject>& parent, const char* name);
		static std::shared_ptr<IGameObject> CreateSphereFlat();
		static std::shared_ptr<IGameObject> CreateSphereFlat(const char* name);
		static std::shared_ptr<IGameObject> CreateSphereFlat(const std::shared_ptr<IGameObject>& parent);
		static std::shared_ptr<IGameObject> CreateSphereFlat(const std::shared_ptr<IGameObject>& parent, const char* name);
		static std::shared_ptr<IGameObject> CreateCapsule();
		static std::shared_ptr<IGameObject> CreateCapsule(const char* name);
		static std::shared_ptr<IGameObject> CreateCapsule(const std::shared_ptr<IGameObject>& parent);
		static std::shared_ptr<IGameObject> CreateCapsule(const std::shared_ptr<IGameObject>& parent, const char* name);
		static std::shared_ptr<IGameObject> CreateCylinder();
		static std::shared_ptr<IGameObject> CreateCylinder(const char* name);
		static std::shared_ptr<IGameObject> CreateCylinder(const std::shared_ptr<IGameObject>& parent);
		static std::shared_ptr<IGameObject> CreateCylinder(const std::shared_ptr<IGameObject>& parent, const char* name);
		static std::shared_ptr<IGameObject> CreatePlane();
		static std::shared_ptr<IGameObject> CreatePlane(const char* name);
		static std::shared_ptr<IGameObject> CreatePlane(const std::shared_ptr<IGameObject>& parent);
		static std::shared_ptr<IGameObject> CreatePlane(const std::shared_ptr<IGameObject>& parent, const char* name);
		// ~Create Primitive

		static std::shared_ptr<IGameObject> GetGameObject(GameObjectID id);
		static std::shared_ptr<IGameObject> FindGameObject(const std::string& go_name);
		static void EraseGameObject(GameObjectID id);

		// GameObjectを複製する
		static std::shared_ptr<IGameObject> DuplicateGameObject(const std::shared_ptr<IGameObject>& gameobject);
		// GameObjectを.objectとして外部にセーブ、ロードする
		static void SaveGameObject(GameObjectID goid);
		static void SaveGameObject(GameObjectID goid, const std::string& filepath);
		static std::shared_ptr<IGameObject> LoadGameObject(const std::string& filepath);
		static std::shared_ptr<IGameObject> LoadGameObject(const std::string& filepath, const int scene_idx);
		// TransformがGameObjectの情報をあらかじめ有効にするために使用
		static void RegisterGameObjectOnDeserialize(const std::shared_ptr<IGameObject>& gameobject);


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================

		// ロード、複製したオブジェクトのOnEnableを実行する
		static void ActivateLoadObject(const std::shared_ptr<Transform>& root_transform);
		// OnEnableをルートから順に再帰的に実行する
		static void RecurseOnActivateLoadObject(const std::shared_ptr<Transform>& transform);

		// ==============================================
		// private variables
		// ==============================================
		static std::unordered_map<GameObjectID, std::weak_ptr<IGameObject>> m_gameObject_index;

		static std::mutex m_mutex;
	};

}// namespace TKGEngine