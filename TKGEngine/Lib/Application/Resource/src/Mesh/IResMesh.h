#pragma once

#include "../../inc/AssetDataBase.h"
#include "../ResourceManager.h"
#include "../../inc/VertexElement.h"
#include "../../inc/Buffer_Defined.h"
#include "Utility/inc/myfunc_vector.h"
#include "Systems/inc/TKGEngine_Defined.h"

#include <string>
#include <vector>
#include <mutex>
#include <unordered_map>


struct ID3D11DeviceContext;

#ifdef USE_IMGUI
#include <fbxsdk.h>
#endif// USE_IMGUI

namespace TKGEngine
{
	class Bounds;
	class VertexBuffer;

	struct Subset
	{
		int start_index = 0;
		int index_count = 0;

	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive)
		{
			archive(
				start_index,
				index_count
			);
		}
	};

}// namespace TKGEngine

namespace TKGEngine
{
	/// <summary>
	/// Mesh Resource interface
	/// </summary>
	class IResMesh
		: public AssetDataBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		IResMesh() = default;
		virtual ~IResMesh() = default;
		IResMesh(const IResMesh&) = delete;
		IResMesh& operator=(const IResMesh&) = delete;

		static std::shared_ptr<IResMesh> Load(const std::string& filename);
		static std::shared_ptr<IResMesh> LoadAsync(const std::string& filename);
#ifdef USE_IMGUI
		static void CreateBinaryFromFBX(
			const std::string& filepath,
			FbxScene* fbx_scene,
			FbxNode* fbx_node,
			FbxMesh* fbx_mesh,
			std::unordered_map<std::string, int>& bone_name_index,
			bool& is_skinned
		);
#endif// USE_IMGUI
		static std::shared_ptr<IResMesh> Create();
		static void RemoveUnused();

		// AssetDataBase
		void Release() override;
		// ~AssetDataBase

		virtual void ActivateVB(ID3D11DeviceContext* p_context, int slot, VERTEX_ELEMENT_TYPE type) = 0;
		virtual void ActivateIB(ID3D11DeviceContext* p_context) = 0;

		virtual VertexBuffer* GetVertexBuffer(VERTEX_ELEMENT_TYPE type) = 0;

		virtual void Clear() = 0;
		virtual void ReCalculateBounds() = 0;
		virtual void ReCalculateNormals() = 0;
		/// <summary>
		/// ÉÅÉbÉVÉÖÇ…â¡Ç¶ÇΩïœçXÇGPUÇ…è„Ç∞ÇÈ
		/// </summary>
		virtual void UploadMeshData() = 0;

		virtual const std::vector<VECTOR3>* GetPositions() const = 0;
		virtual const std::vector<VECTOR3>* GetNormals() const = 0;
		virtual const std::vector<VECTOR2>* GetUVs() const = 0;
		virtual const Bounds* GetBounds() const = 0;
		virtual const std::vector<unsigned>* GetIndices() const = 0;

		virtual void SetPositions(const std::vector<VECTOR3>& pos) = 0;
		virtual void SetNormals(const std::vector<VECTOR3>& norm) = 0;
		virtual void SetUVs(const std::vector<VECTOR2>& uv) = 0;
		virtual void SetBounds(const Bounds& bounds) = 0;
		virtual void SetIndices(const std::vector<unsigned>& indices) = 0;

		virtual void SetIndexCount(int index, int count) = 0;

		virtual int GetVertexCount() const = 0;
		virtual int GetIndexCount() const = 0;
		virtual int GetSubsetCount() const = 0;
		virtual std::pair<int, int> GetSubset(int index) const = 0;

		virtual VECTOR3 GetGlobalTranslate() const = 0;
		virtual Quaternion GetGlobalRotate() const = 0;
		virtual VECTOR3 GetGlobalScale() const = 0;

		// ==============================================
		// public variables
		// ==============================================
		static constexpr int MAX_BONE_INFLUENCE = 4;


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

		static IResMesh* CreateInterface(BUFFER_HEAP_TYPE type);


		// ==============================================
		// private variables
		// ==============================================
		static ResourceManager<IResMesh> m_caches;
		static std::mutex m_cache_mutex;
	};

}// namespace TKGEngine

CEREAL_REGISTER_TYPE(TKGEngine::IResMesh)
CEREAL_CLASS_VERSION(TKGEngine::IResMesh, 1)
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::AssetDataBase, TKGEngine::IResMesh)
