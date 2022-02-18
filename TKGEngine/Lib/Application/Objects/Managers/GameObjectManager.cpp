
#include "GameObjectManager.h"
#include "Managers/ObjectManager.h"
#include "Managers/SceneManager.h"
#include "Managers/MonoBehaviourManager.h"

#include "Systems/inc/LogSystem.h"
#include "Systems/inc/PhysicsSystem.h"
#include "Systems/inc/TKGEngine_Defined.h"
#include "Components/inc/CMeshRenderer.h"
#include "Components/inc/CTransform.h"

#include "Utility/inc/myfunc_file.h"

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Static variable declaration
	////////////////////////////////////////////////////////
	std::unordered_map<GameObjectID, std::weak_ptr<IGameObject>> GameObjectManager::m_gameObject_index;
	std::mutex GameObjectManager::m_mutex;

	namespace /* anonymous */
	{
		int gameObject_created_index = 0;
	}/* anonymous */

	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	std::shared_ptr<IGameObject> GameObjectManager::CreateGameObject()
	{
		m_mutex.lock();
		// 作成とInstanceIDの割り振り
		std::shared_ptr<IGameObject> p_go(IGameObject::CreateInterface());
		p_go->SetName("GameObject" + std::to_string(gameObject_created_index++));
		const auto p_object = std::static_pointer_cast<Object>(p_go);
		// InstanceIDでマネージャにセット
		ObjectManager::Set(p_object);
		// GameObjectマネージャにセット
		m_gameObject_index.emplace(p_go->GetGameObjectID(), p_go);

		m_mutex.unlock();

		// Transformをアタッチする(Transform::OnCreateでGetGameObjectが呼ばれる)
		p_object->OnCreate(p_object);

		const int scene_idx = SceneManager::GetMainSceneID();
		p_go->SetScene(scene_idx);
		SceneManager::RegisterGameObjectToScene(p_go->GetGameObjectID(), scene_idx);

		return p_go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateGameObject(const char* name)
	{
		m_mutex.lock();

		std::shared_ptr<IGameObject> p_go(IGameObject::CreateInterface());
		p_go->SetName(name);
		const auto p_object = std::static_pointer_cast<Object>(p_go);
		ObjectManager::Set(p_object);
		m_gameObject_index.emplace(p_go->GetGameObjectID(), p_go);

		m_mutex.unlock();

		// Transformをアタッチする(Transform::OnCreateでGetGameObjectが呼ばれる)
		p_object->OnCreate(p_object);

		const int scene_idx = SceneManager::GetMainSceneID();
		p_go->SetScene(scene_idx);
		SceneManager::RegisterGameObjectToScene(p_go->GetGameObjectID(), scene_idx);

		return p_go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateChild(const std::shared_ptr<IGameObject>& parent)
	{
		m_mutex.lock();

		std::shared_ptr<IGameObject> p_go(IGameObject::CreateInterface());
		p_go->SetName("GameObject" + std::to_string(gameObject_created_index++));
		const auto p_object = std::static_pointer_cast<Object>(p_go);
		ObjectManager::Set(p_object);
		m_gameObject_index.emplace(p_go->GetGameObjectID(), p_go);

		m_mutex.unlock();

		// Transformをアタッチする(Transform::OnCreateでGetGameObjectが呼ばれる)
		p_object->OnCreate(p_object);

		// Get parent info.
		p_go->SetScene(parent->GetScene());
		p_go->GetTransform()->SetParent(parent->GetTransform());

		return p_go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateChild(const std::shared_ptr<IGameObject>& parent, const char* name)
	{
		m_mutex.lock();

		std::shared_ptr<IGameObject> p_go(IGameObject::CreateInterface());
		p_go->SetName(name);
		const auto p_object = std::static_pointer_cast<Object>(p_go);
		ObjectManager::Set(p_object);
		m_gameObject_index.emplace(p_go->GetGameObjectID(), p_go);

		m_mutex.unlock();

		// Transformをアタッチする(Transform::OnCreateでGetGameObjectが呼ばれる)
		p_object->OnCreate(p_object);

		// Get parent info.
		p_go->SetScene(parent->GetScene());
		p_go->GetTransform()->SetParent(parent->GetTransform());

		return p_go;
	}

#pragma region Create Primitive
	std::shared_ptr<IGameObject> GameObjectManager::CreateCube()
	{
		auto go = CreateGameObject();
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/cube/Cube.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateCube(const char* name)
	{
		auto go = CreateGameObject(name);
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/cube/Cube.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateCube(const std::shared_ptr<IGameObject>& parent)
	{
		auto go = CreateChild(parent);
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/cube/Cube.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateCube(const std::shared_ptr<IGameObject>& parent, const char* name)
	{
		auto go = CreateChild(parent, name);
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/cube/Cube.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateSphereSmooth()
	{
		auto go = CreateGameObject();
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/sphere_s/Sphere.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateSphereSmooth(const char* name)
	{
		auto go = CreateGameObject(name);
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/sphere_s/Sphere.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateSphereSmooth(const std::shared_ptr<IGameObject>& parent)
	{
		auto go = CreateChild(parent);
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/sphere_s/Sphere.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateSphereSmooth(const std::shared_ptr<IGameObject>& parent, const char* name)
	{
		auto go = CreateChild(parent, name);
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/sphere_s/Sphere.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateSphereFlat()
	{
		auto go = CreateGameObject();
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/sphere_f/Sphere.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateSphereFlat(const char* name)
	{
		auto go = CreateGameObject(name);
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/sphere_f/Sphere.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateSphereFlat(const std::shared_ptr<IGameObject>& parent)
	{
		auto go = CreateChild(parent);
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/sphere_f/Sphere.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateSphereFlat(const std::shared_ptr<IGameObject>& parent, const char* name)
	{
		auto go = CreateChild(parent, name);
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/sphere_f/Sphere.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateCapsule()
	{
		auto go = CreateGameObject();
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/capsule/Capsule.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateCapsule(const char* name)
	{
		auto go = CreateGameObject(name);
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/capsule/Capsule.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateCapsule(const std::shared_ptr<IGameObject>& parent)
	{
		auto go = CreateChild(parent);
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/capsule/Capsule.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateCapsule(const std::shared_ptr<IGameObject>& parent, const char* name)
	{
		auto go = CreateChild(parent, name);
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/capsule/Capsule.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateCylinder()
	{
		auto go = CreateGameObject();
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/cylinder/Cylinder.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateCylinder(const char* name)
	{
		auto go = CreateGameObject(name);
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/cylinder/Cylinder.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateCylinder(const std::shared_ptr<IGameObject>& parent)
	{
		auto go = CreateChild(parent);
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/cylinder/Cylinder.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreateCylinder(const std::shared_ptr<IGameObject>& parent, const char* name)
	{
		auto go = CreateChild(parent, name);
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/cylinder/Cylinder.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreatePlane()
	{
		auto go = CreateGameObject();
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/plane/Plane.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreatePlane(const char* name)
	{
		auto go = CreateGameObject(name);
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/plane/Plane.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreatePlane(const std::shared_ptr<IGameObject>& parent)
	{
		auto go = CreateChild(parent);
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/plane/Plane.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}

	std::shared_ptr<IGameObject> GameObjectManager::CreatePlane(const std::shared_ptr<IGameObject>& parent, const char* name)
	{
		auto go = CreateChild(parent, name);
		const auto mr = go->AddComponent<MeshRenderer>();
		mr->AddMesh("./Asset/Models/Primitive/plane/Plane.mesh");
		mr->AddMaterial(0, Material::GetDefaultMaterial());
		return go;
	}
#pragma endregion


	std::shared_ptr<IGameObject> GameObjectManager::GetGameObject(GameObjectID id)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		const auto itr = m_gameObject_index.find(id);
		if (itr == m_gameObject_index.end())
		{
			return std::shared_ptr<IGameObject>();
		}

		auto s_ptr = itr->second.lock();
		if (s_ptr == nullptr)
		{
			m_gameObject_index.erase(itr);
			return std::shared_ptr<IGameObject>();
		}
		return s_ptr;
	}

	std::shared_ptr<IGameObject> GameObjectManager::FindGameObject(const std::string& go_name)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		const auto itr_end = m_gameObject_index.end();
		for (auto itr = m_gameObject_index.begin(); itr != itr_end;)
		{
			auto s_ptr = itr->second.lock();
			if (s_ptr == nullptr)
			{
				itr = m_gameObject_index.erase(itr);
				continue;
			}
			if (go_name == s_ptr->GetName())
			{
				return s_ptr;
			}
			++itr;
		}
		// Can not find
		return std::shared_ptr<IGameObject>();
	}

	void GameObjectManager::EraseGameObject(GameObjectID id)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		// Find from GameObjectIndex and Erase GOManager.
		const auto itr = m_gameObject_index.find(id);
		if (itr != m_gameObject_index.end())
		{
			m_gameObject_index.erase(itr);
		}
	}

	std::shared_ptr<IGameObject> GameObjectManager::DuplicateGameObject(const std::shared_ptr<IGameObject>& gameobject)
	{
		const auto original_transform = gameobject->GetTransform();
		// Cerealを使用してメモリ内にコピーの作成
		std::stringstream buf;
		// バイナリ書き出し
		{
			cereal::BinaryOutputArchive ar(buf);
			ar(original_transform);
		}
		// バイナリ読み込み
		std::shared_ptr<Transform> dupli_transform;
		{
			cereal::BinaryInputArchive ar(buf);
			ar(dupli_transform);
		}

		// シーンに登録
		const auto dupli_object = dupli_transform->GetGameObject();
		const auto parent_transform = original_transform->GetParent();
		if (parent_transform)
		{
			// Parentがいるなら同一シーンに登録
			dupli_object->SetScene(parent_transform->GetGameObject()->GetScene());
			// Parentのchildrenに登録
			dupli_transform->SetParent(parent_transform, false);
		}
		else
		{
			// Parentがnullptrならシーンのルートオブジェクトに登録
			const int scene_idx = SceneManager::GetMainSceneID();
			dupli_object->SetScene(scene_idx);
			SceneManager::RegisterGameObjectToScene(dupli_object->GetGameObjectID(), scene_idx);
		}

		// 複製したGameObject内のコンポーネントを有効化する
		ActivateLoadObject(dupli_transform);

		return dupli_object;
	}

	void GameObjectManager::SaveGameObject(GameObjectID goid)
	{
		// ファイルパス取得
		std::string filepath;
		if (!MyFunc::FetchSaveFileName(filepath, TEXT("GameObject(*.object)\0*.object\0"), TEXT("Save GameObject")))
		{
			return;
		}
		// ファイルの上書き時は拡張子があるためチェック
		if (MyFunc::GetExtension(filepath).empty())
		{
			filepath += GAMEOBJECT_EXTENSION;
		}
		SaveGameObject(goid, filepath);
	}

	void GameObjectManager::SaveGameObject(GameObjectID goid, const std::string& filepath)
	{
		// Transform取得
		auto original_transform = GetGameObject(goid)->GetTransform();
		// シリアライズ
		// Binary
		{
			std::ofstream ofs(filepath.c_str(), std::ios::out | std::ios::binary);
			if (!ofs.is_open())
			{
				assert(0 && "failed save file. GameObjectManager::SaveGameObject()");
				return;
			}
			cereal::BinaryOutputArchive ar(ofs);
			ar(original_transform);
		}
		// Json
#ifdef SAVE_JSON
		{
			const std::string filepath_J = filepath + "J";
			std::ofstream ofs(filepath_J.c_str(), std::ios::out | std::ios::binary);
			if (!ofs.is_open())
			{
				assert(0 && "failed save file. GameObjectManager::SaveGameObject()");
				return;
			}
			cereal::JSONOutputArchive ar(ofs);
			ar(original_transform);
		}
#endif// ~SAVE_JSON
	}

	std::shared_ptr<IGameObject> GameObjectManager::LoadGameObject(const std::string& filepath)
	{
		const int scene_idx = SceneManager::GetMainSceneID();
		return LoadGameObject(filepath, scene_idx);
	}

	std::shared_ptr<IGameObject> GameObjectManager::LoadGameObject(const std::string& filepath, const int scene_idx)
	{
		// 空オブジェクト作成
		std::shared_ptr<Transform> transform;
		{
			// ファイルオープン
			std::ifstream ifs(filepath.c_str(), std::ios::in | std::ios::binary);
			if (!ifs.is_open())
			{
				assert(0 && "failed load file. GameObjectManager::LoadGameObject()");
				return std::shared_ptr<IGameObject>();
			}

			// デシリアライズ
			cereal::BinaryInputArchive ar(ifs);
			{
				// Transform
				ar(transform);
			}
		}
		// シーンに登録
		auto gameobject = transform->GetGameObject();
		gameobject->SetScene(scene_idx);
		SceneManager::RegisterGameObjectToScene(gameobject->GetGameObjectID(), scene_idx);
		// ロードしたGameObject内のコンポーネントを有効化する
		ActivateLoadObject(transform);

		return gameobject;
	}

	void GameObjectManager::RegisterGameObjectOnDeserialize(const std::shared_ptr<IGameObject>& gameobject)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		// GameObjectをGOManagerとObjectManagerに登録
		const auto p_object = std::static_pointer_cast<Object>(gameobject);
		ObjectManager::Set(p_object);
		m_gameObject_index.emplace(gameobject->GetGameObjectID(), gameobject);
	}

	void GameObjectManager::ActivateLoadObject(const std::shared_ptr<Transform>& root_transform)
	{
		// Rootから順にアクティブかチェックして、対応する関数を呼ぶ再帰処理
		RecurseOnActivateLoadObject(root_transform);
	}

	void GameObjectManager::RecurseOnActivateLoadObject(const std::shared_ptr<Transform>& transform)
	{
		const auto gameobject = transform->GetGameObject();
		// 非アクティブならそれ以下は探索しない
		if (!gameobject->GetActiveHierarchy())
		{
			return;
		}
		// 有効時の関数を呼ぶ
		{
			const auto gameobject_id = gameobject->GetGameObjectID();
			// MonoBehaviour::OnEnable
			MonoBehaviourManager::OnEnable(gameobject_id);
			// コリジョンの有効、無効切り替え
			PhysicsSystem::GetInstance()->SetCollisionActive(gameobject_id, true);

		}
		// 子オブジェクトが存在するなら再帰処理で実行する
		const auto child_num = transform->GetChildCount();
		if (child_num > 0)
		{
			for (int i = 0; i < child_num; ++i)
			{
				RecurseOnActivateLoadObject(transform->GetChild(i));
			}
		}
	}

}//namespace TKGEngine 