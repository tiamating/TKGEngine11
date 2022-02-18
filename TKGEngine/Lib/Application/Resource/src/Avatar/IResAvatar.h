#pragma once

#include "../../inc/AssetDataBase.h"

#include "../ResourceManager.h"
#include "Utility/inc/myfunc_vector.h"
#include "Systems/inc/TKGEngine_Defined.h"

#include <string>
#include <vector>

#include <memory>
#include <mutex>

#ifdef USE_IMGUI
#include <fbxsdk.h>
#endif// USE_IMGUI

namespace TKGEngine
{
	struct Node;

	/// <summary>
	/// Avatar Resource interface
	/// </summary>
	class IResAvatar
		: public AssetDataBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		IResAvatar() = default;
		virtual ~IResAvatar() = default;
		IResAvatar(const IResAvatar&) = delete;
		IResAvatar& operator=(const IResAvatar&) = delete;

		static std::shared_ptr<IResAvatar> Load(const std::string& filename);
		static std::shared_ptr<IResAvatar> LoadAsync(const std::string& filename);
#ifdef USE_IMGUI
		static void CreateBinaryFromFBX(
			const std::string& filepath,
			FbxScene* fbx_scene,
			std::vector<FbxMesh*>& fbx_meshes,
			std::unordered_map<std::string, int>& bone_name_index,
			bool is_save
		);
#endif// USE_IMGUI
		static void RemoveUnused();

		// AssetDataBase
		void Release() override;
		// ~AssetDataBase

		virtual int GetBoneCount() const = 0;
		virtual int GetBoneIndex(const std::string& bone_name) = 0;
		virtual int GetEnabledBoneCount() const = 0;
		virtual int GetRootCount() const = 0;
		virtual const char* GetRootName(int index) = 0;
		virtual int GetRootIndex(int index) = 0;
		virtual const Node* GetNode(int index) = 0;
		virtual const Node* GetNode(const std::string& bone_name) = 0;
		virtual const std::unordered_map<std::string, int>* GetNodeIndex() const = 0;
		virtual const std::vector<std::string>* GetAlignedBoneNames() const = 0;

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

		static std::shared_ptr<IResAvatar> CreateInterface();

		// ==============================================
		// private variables
		// ==============================================
		static ResourceManager<IResAvatar> m_caches;
		static std::mutex m_cache_mutex;
	};

}// namespace TKGEngine

CEREAL_REGISTER_TYPE(TKGEngine::IResAvatar)
CEREAL_CLASS_VERSION(TKGEngine::IResAvatar, 1)
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::AssetDataBase, TKGEngine::IResAvatar)
