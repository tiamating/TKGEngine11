
#include "../../inc/Mesh.h"

#include "IResMesh.h"

#include "Utility/inc/bounds.h"


namespace TKGEngine
{
	// ==================================================
	// class methods
	// ==================================================
	Mesh::Mesh(const Mesh& mesh)
	{
		m_res_mesh = mesh.m_res_mesh;
	}

	void Mesh::operator=(const Mesh& mesh)
	{
		m_res_mesh = mesh.m_res_mesh;
	}

	void Mesh::RemoveUnused()
	{
		IResMesh::RemoveUnused();
	}

	void Mesh::Load(const std::string& filename)
	{
		m_res_mesh = IResMesh::Load(filename);
	}

	void Mesh::LoadAsync(const std::string& filename)
	{
		m_res_mesh = IResMesh::LoadAsync(filename);
	}

	void Mesh::Create()
	{
		m_res_mesh = IResMesh::Create();
	}

	void Mesh::Release()
	{
		if (m_res_mesh)
		{
			m_res_mesh->Release();
		}
		m_res_mesh.reset();
	}

	void Mesh::ActivateVB(ID3D11DeviceContext* p_context, int slot, VERTEX_ELEMENT_TYPE type)
	{
		if (m_res_mesh)
			m_res_mesh->ActivateVB(p_context, slot, type);
	}

	void Mesh::ActivateVB(ID3D11DeviceContext* p_context, int slot, int type)
	{
		if (type < 0 || type >= static_cast<int>(VERTEX_ELEMENT_TYPE::MAX_TYPE_NUM))
		{
			return;
		}

		ActivateVB(p_context, slot, static_cast<VERTEX_ELEMENT_TYPE>(type));
	}

	void Mesh::ActivateIB(ID3D11DeviceContext* p_context)
	{
		if (m_res_mesh)
			m_res_mesh->ActivateIB(p_context);
	}

	VertexBuffer* Mesh::GetVertexBuffer(VERTEX_ELEMENT_TYPE type)
	{
		return m_res_mesh ? m_res_mesh->GetVertexBuffer(type) : nullptr;
	}

	void Mesh::Clear()
	{
		if (m_res_mesh)
			m_res_mesh->Clear();
	}

	void Mesh::ReCalculateBounds()
	{
		if (m_res_mesh)
			m_res_mesh->ReCalculateBounds();
	}

	void Mesh::ReCalculateNormals()
	{
		if (m_res_mesh)
			m_res_mesh->ReCalculateNormals();
	}

	void Mesh::UploadMeshData()
	{
		if (m_res_mesh)
			m_res_mesh->UploadMeshData();
	}

	const std::vector<VECTOR3>* Mesh::GetPositions() const
	{
		return m_res_mesh ? m_res_mesh->GetPositions() : nullptr;
	}

	const std::vector<VECTOR3>* Mesh::GetNormals() const
	{
		return m_res_mesh ? m_res_mesh->GetNormals() : nullptr;
	}

	const std::vector<VECTOR2>* Mesh::GetUVs() const
	{
		return m_res_mesh ? m_res_mesh->GetUVs() : nullptr;
	}

	const Bounds* Mesh::GetBounds() const
	{
		return m_res_mesh ? m_res_mesh->GetBounds() : nullptr;
	}

	const std::vector<unsigned>* Mesh::GetIndices() const
	{
		return m_res_mesh ? m_res_mesh->GetIndices() : nullptr;
	}

	void Mesh::SetPositions(const std::vector<VECTOR3>& pos)
	{
		if (m_res_mesh)
			m_res_mesh->SetPositions(pos);
	}

	void Mesh::SetNormals(const std::vector<VECTOR3>& norm)
	{
		if (m_res_mesh)
			m_res_mesh->SetNormals(norm);
	}

	void Mesh::SetUVs(const std::vector<VECTOR2>& uv)
	{
		if (m_res_mesh)
			m_res_mesh->SetUVs(uv);
	}

	void Mesh::SetBounds(const Bounds& bounds)
	{
		if (m_res_mesh)
			m_res_mesh->SetBounds(bounds);
	}

	void Mesh::SetIndices(const std::vector<unsigned>& indices)
	{
		if (m_res_mesh)
			m_res_mesh->SetIndices(indices);
	}

	void Mesh::SetIndexCount(int index, int count)
	{
		if (m_res_mesh)
			m_res_mesh->SetIndexCount(index, count);
	}

	int Mesh::GetVertexCount() const
	{
		return m_res_mesh ? m_res_mesh->GetVertexCount() : 0;
	}

	int Mesh::GetIndexCount() const
	{
		return m_res_mesh ? m_res_mesh->GetIndexCount() : 0;
	}

	int Mesh::GetSubsetCount() const
	{
		return m_res_mesh ? m_res_mesh->GetSubsetCount() : 0;
	}

	std::pair<int, int> Mesh::GetSubset(int index) const
	{
		return m_res_mesh ? m_res_mesh->GetSubset(index) : std::pair<int, int>(0, 0);
	}

	VECTOR3 Mesh::GetGlobalTranslate() const
	{
		return m_res_mesh ? m_res_mesh->GetGlobalTranslate() : VECTOR3::Zero;
	}

	Quaternion Mesh::GetGlobalRotate() const
	{
		return m_res_mesh ? m_res_mesh->GetGlobalRotate() : Quaternion::Identity;
	}

	VECTOR3 Mesh::GetGlobalScale() const
	{
		return m_res_mesh ? m_res_mesh->GetGlobalScale() : VECTOR3::One;
	}

	bool Mesh::IsLoaded() const
	{
		return m_res_mesh ? m_res_mesh->IsLoaded() : false;
	}

	bool Mesh::HasMesh() const
	{
		return m_res_mesh ? m_res_mesh->HasResource() : false;
	}

	const char* Mesh::GetName() const
	{
		return m_res_mesh ? m_res_mesh->GetName() : nullptr;
	}

	const char* Mesh::GetFilePath() const
	{
		return m_res_mesh ? m_res_mesh->GetFilePath() : "";
	}

	unsigned Mesh::GetHash() const
	{
		return m_res_mesh ? m_res_mesh->GetHash() : 0;
	}

}// namespace TKGEngine