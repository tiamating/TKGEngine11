#pragma once

#include "Systems/inc/TKGEngine_Defined.h"
#include "Utility/inc/myfunc_vector.h"
#include "Application/Resource/inc/VertexElement.h"
#include "Application/Resource/inc/Asset_Defined.h"

#include <string>
#include <vector>

#include <memory>

struct ID3D11DeviceContext;

namespace TKGEngine
{
	class Bounds;
	struct Subset;
	class VertexBuffer;
}// namespace TKGEngine

namespace TKGEngine
{
	class IResMesh;

	class Mesh
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Mesh() = default;
		virtual ~Mesh() = default;
		Mesh(const Mesh&);
		void operator=(const Mesh&);

		static void RemoveUnused();

		void Load(const std::string& filename);
		void LoadAsync(const std::string& filename);
		void Create();

		void Release();

		void ActivateVB(ID3D11DeviceContext* p_context, int slot, VERTEX_ELEMENT_TYPE type);
		void ActivateVB(ID3D11DeviceContext* p_context, int slot, int type);
		void ActivateIB(ID3D11DeviceContext* p_context);

		VertexBuffer* GetVertexBuffer(VERTEX_ELEMENT_TYPE type);

		// Dynamic mesh only
		void Clear();
		void ReCalculateBounds();
		void ReCalculateNormals();

		/// <summary>
		/// 頂点データをバッファにコピーする
		/// </summary>
		void UploadMeshData();

		const std::vector<VECTOR3>* GetPositions() const;
		const std::vector<VECTOR3>* GetNormals() const;
		const std::vector<VECTOR2>* GetUVs() const;
		const Bounds* GetBounds() const;
		const std::vector<unsigned>* GetIndices() const;

		void SetPositions(const std::vector<VECTOR3>& pos);
		void SetNormals(const std::vector<VECTOR3>& norm);
		void SetUVs(const std::vector<VECTOR2>& uv);
		void SetBounds(const Bounds& bounds);
		void SetIndices(const std::vector<unsigned>& indices);

		/// <summary>
		/// サブセットの描画するインデックス数をセットする
		/// </summary>
		/// <param name="index">subsets.at(index)</param>
		/// <param name="count">インデックス描画頂点数</param>
		void SetIndexCount(int index, int count);
		// ~Dynamic mesh only

		int GetVertexCount() const;
		int GetIndexCount() const;
		int GetSubsetCount() const;
		/// <returns> (StartIndex, IndexCount) </returns>
		std::pair<int, int> GetSubset(int index) const;

		VECTOR3 GetGlobalTranslate() const;
		Quaternion GetGlobalRotate() const;
		VECTOR3 GetGlobalScale() const;

		bool IsLoaded() const;
		bool HasMesh() const;
		const char* GetName() const;
		const char* GetFilePath() const;

		unsigned GetHash() const;

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
		std::shared_ptr<IResMesh> m_res_mesh = nullptr;
	};


}	// namespace TKGEngine