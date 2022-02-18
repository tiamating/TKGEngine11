
#include "../../inc/Shader.h"

#include "IResShader.h"

#include <d3d11.h>

namespace TKGEngine
{
#pragma region Vertex Shader
	VertexShader::VertexShader(const VertexShader& vs)
	{
		m_res_vs = vs.m_res_vs;
	}

	VertexShader& VertexShader::operator=(const VertexShader& vs)
	{
		m_res_vs = vs.m_res_vs;
		return *this;
	}

	void VertexShader::RemoveUnused()
	{
		IResVS::RemoveUnused();
	}

	void VertexShader::Compile(const std::string& filename, const std::string& entry_point, const std::string& shader_model, bool force_compile)
	{
		m_res_vs = IResVS::Compile(filename, entry_point, shader_model, force_compile);
	}

	void VertexShader::CompileAsync(const std::string& filename, const std::string& entry_point, const std::string& shader_model, bool force_compile)
	{
		m_res_vs = IResVS::CompileAsync(filename, entry_point, shader_model, force_compile);
	}

	void VertexShader::Load(const std::string& filename)
	{
		m_res_vs = IResVS::Load(filename);
	}

	void VertexShader::LoadAsync(const std::string& filename)
	{
		m_res_vs = IResVS::LoadAsync(filename);
	}

	void VertexShader::Remove()
	{
		m_res_vs.reset();
	}

	void VertexShader::Release()
	{
		if (m_res_vs)
		{
			m_res_vs->Release();
		}
		m_res_vs.reset();
	}

	int VertexShader::GetSlotFromType(VERTEX_ELEMENT_TYPE type)
	{
		if (!m_res_vs)
		{
			return -1;
		}
		return m_res_vs->GetSlotFromType(type);
	}

	ID3D11ShaderReflection* VertexShader::GetReflection() const
	{
		return m_res_vs != nullptr ? m_res_vs->GetReflection() : nullptr;
	}

	bool VertexShader::Activate(ID3D11DeviceContext* p_context)
	{
		if (m_res_vs)
		{
			if (m_res_vs->HasResource() == true)
			{
				m_res_vs->Activate(p_context);
				return true;
			}
		}
		p_context->VSSetShader(nullptr, nullptr, 0);
		p_context->IASetInputLayout(nullptr);
		return false;
	}

	void VertexShader::Deactivate(ID3D11DeviceContext* p_context)
	{
		p_context->VSSetShader(nullptr, nullptr, 0);
		p_context->IASetInputLayout(nullptr);
	}

	bool VertexShader::HasResource() const
	{
		return m_res_vs == nullptr ? false : m_res_vs->HasResource();
	}

	const char* VertexShader::GetName() const
	{
		return m_res_vs == nullptr ? nullptr : m_res_vs->GetName();
	}

	void VertexShader::SetName(const std::string& name)
	{
		if (m_res_vs)
		{
			m_res_vs->SetName(name);
		}
	}
	const char* VertexShader::GetFilePath() const
	{
		return m_res_vs ? m_res_vs->GetFilePath() : "";
	}
#pragma endregion

#pragma region Pixel Shader
	PixelShader::PixelShader(const PixelShader& ps)
	{
		m_res_ps = ps.m_res_ps;
	}

	PixelShader& PixelShader::operator=(const PixelShader& ps)
	{
		m_res_ps = ps.m_res_ps;
		return *this;
	}

	void PixelShader::RemoveUnused()
	{
		IResPS::RemoveUnused();
	}

	void PixelShader::Compile(const std::string& filename, const std::string& entry_point, const std::string& shader_model, bool force_compile)
	{
		m_res_ps = IResPS::Compile(filename, entry_point, shader_model, force_compile);
	}

	void PixelShader::CompileAsync(const std::string& filename, const std::string& entry_point, const std::string& shader_model, bool force_compile)
	{
		m_res_ps = IResPS::CompileAsync(filename, entry_point, shader_model, force_compile);
	}

	void PixelShader::Load(const std::string& filename)
	{
		m_res_ps = IResPS::Load(filename);
	}

	void PixelShader::LoadAsync(const std::string& filename)
	{
		m_res_ps = IResPS::LoadAsync(filename);
	}

	void PixelShader::Remove()
	{
		m_res_ps.reset();
	}

	void PixelShader::Release()
	{
		if (m_res_ps)
		{
			m_res_ps->Release();
		}
		m_res_ps.reset();
	}

	ID3D11ShaderReflection* PixelShader::GetReflection() const
	{
		return m_res_ps != nullptr ? m_res_ps->GetReflection() : nullptr;
	}

	bool PixelShader::Activate(ID3D11DeviceContext* p_context)
	{
		if (m_res_ps)
		{
			if (m_res_ps->HasResource())
			{
				m_res_ps->Activate(p_context);
				return true;
			}
		}
		p_context->PSSetShader(nullptr, nullptr, 0);
		return false;
	}

	void PixelShader::Deactivate(ID3D11DeviceContext* p_context)
	{
		p_context->PSSetShader(nullptr, nullptr, 0);
	}

	bool PixelShader::HasResource() const
	{
		return m_res_ps == nullptr ? false : m_res_ps->HasResource();
	}

	const char* PixelShader::GetName() const
	{
		return m_res_ps == nullptr ? nullptr : m_res_ps->GetName();
	}

	void PixelShader::SetName(const std::string& name)
	{
		if (m_res_ps)
		{
			m_res_ps->SetName(name);
		}
	}
	const char* PixelShader::GetFilePath() const
	{
		return m_res_ps ? m_res_ps->GetFilePath() : "";
	}
#pragma endregion

#pragma region Geometry Shader
	GeometryShader::GeometryShader(const GeometryShader& gs)
	{
		m_res_gs = gs.m_res_gs;
	}

	GeometryShader& GeometryShader::operator=(const GeometryShader& gs)
	{
		m_res_gs = gs.m_res_gs;
		return *this;
	}

	void GeometryShader::RemoveUnused()
	{
		IResGS::RemoveUnused();
	}

	void GeometryShader::Compile(const std::string& filename, const std::string& entry_point, const std::string& shader_model, bool force_compile)
	{
		m_res_gs = IResGS::Compile(filename, entry_point, shader_model, force_compile);
	}

	void GeometryShader::CompileAsync(const std::string& filename, const std::string& entry_point, const std::string& shader_model, bool force_compile)
	{
		m_res_gs = IResGS::CompileAsync(filename, entry_point, shader_model, force_compile);
	}

	void GeometryShader::Load(const std::string& filename)
	{
		m_res_gs = IResGS::Load(filename);
	}

	void GeometryShader::LoadAsync(const std::string& filename)
	{
		m_res_gs = IResGS::LoadAsync(filename);
	}

	void GeometryShader::Remove()
	{
		m_res_gs.reset();
	}

	void GeometryShader::Release()
	{
		if (m_res_gs)
		{
			m_res_gs->Release();
		}
		m_res_gs.reset();
	}

	bool GeometryShader::Activate(ID3D11DeviceContext* p_context)
	{
		if (m_res_gs)
		{
			if (m_res_gs->HasResource())
			{
				m_res_gs->Activate(p_context);
				return true;
			}
		}
		p_context->GSSetShader(nullptr, nullptr, 0);
		return false;
	}

	void GeometryShader::Deactivate(ID3D11DeviceContext* p_context)
	{
		p_context->GSSetShader(nullptr, nullptr, 0);
	}

	bool GeometryShader::HasResource() const
	{
		return m_res_gs == nullptr ? false : m_res_gs->HasResource();
	}

	const char* GeometryShader::GetName() const
	{
		return m_res_gs == nullptr ? nullptr : m_res_gs->GetName();
	}

	void GeometryShader::SetName(const std::string& name)
	{
		if (m_res_gs)
		{
			m_res_gs->SetName(name);
		}
	}
	const char* GeometryShader::GetFilePath() const
	{
		return m_res_gs ? m_res_gs->GetFilePath() : "";
	}
#pragma endregion

#pragma region Domain Shader
	DomainShader::DomainShader(const DomainShader& ds)
	{
		m_res_ds = ds.m_res_ds;
	}

	DomainShader& DomainShader::operator=(const DomainShader& ds)
	{
		m_res_ds = ds.m_res_ds;
		return *this;
	}

	void DomainShader::RemoveUnused()
	{
		IResDS::RemoveUnused();
	}

	void DomainShader::Compile(const std::string& filename, const std::string& entry_point, const std::string& shader_model, bool force_compile)
	{
		m_res_ds = IResDS::Compile(filename, entry_point, shader_model, force_compile);
	}

	void DomainShader::CompileAsync(const std::string& filename, const std::string& entry_point, const std::string& shader_model, bool force_compile)
	{
		m_res_ds = IResDS::CompileAsync(filename, entry_point, shader_model, force_compile);
	}

	void DomainShader::Load(const std::string& filename)
	{
		m_res_ds = IResDS::Load(filename);
	}

	void DomainShader::LoadAsync(const std::string& filename)
	{
		m_res_ds = IResDS::LoadAsync(filename);
	}

	void DomainShader::Remove()
	{
		m_res_ds.reset();
	}

	void DomainShader::Release()
	{
		if (m_res_ds)
		{
			m_res_ds->Release();
		}
		m_res_ds.reset();
	}

	bool DomainShader::Activate(ID3D11DeviceContext* p_context)
	{
		if (m_res_ds)
		{
			if (m_res_ds->HasResource())
			{
				m_res_ds->Activate(p_context);
				return true;
			}
		}
		p_context->DSSetShader(nullptr, nullptr, 0);
		return false;
	}

	void DomainShader::Deactivate(ID3D11DeviceContext* p_context)
	{
		p_context->DSSetShader(nullptr, nullptr, 0);
	}

	bool DomainShader::HasResource() const
	{
		return m_res_ds == nullptr ? false : m_res_ds->HasResource();
	}

	const char* DomainShader::GetName() const
	{
		return m_res_ds == nullptr ? nullptr : m_res_ds->GetName();
	}

	void DomainShader::SetName(const std::string& name)
	{
		if (m_res_ds)
		{
			m_res_ds->SetName(name);
		}
	}
	const char* DomainShader::GetFilePath() const
	{
		return m_res_ds ? m_res_ds->GetFilePath() : "";
	}
#pragma endregion

#pragma region Hull Shader
	HullShader::HullShader(const HullShader& hs)
	{
		m_res_hs = hs.m_res_hs;
	}

	HullShader& HullShader::operator=(const HullShader& hs)
	{
		m_res_hs = hs.m_res_hs;
		return *this;
	}

	void HullShader::RemoveUnused()
	{
		IResHS::RemoveUnused();
	}

	void HullShader::Compile(const std::string& filename, const std::string& entry_point, const std::string& shader_model, bool force_compile)
	{
		m_res_hs = IResHS::Compile(filename, entry_point, shader_model, force_compile);
	}

	void HullShader::CompileAsync(const std::string& filename, const std::string& entry_point, const std::string& shader_model, bool force_compile)
	{
		m_res_hs = IResHS::CompileAsync(filename, entry_point, shader_model, force_compile);
	}

	void HullShader::Load(const std::string& filename)
	{
		m_res_hs = IResHS::Load(filename);
	}

	void HullShader::LoadAsync(const std::string& filename)
	{
		m_res_hs = IResHS::LoadAsync(filename);
	}

	void HullShader::Remove()
	{
		m_res_hs.reset();
	}

	void HullShader::Release()
	{
		if (m_res_hs)
		{
			m_res_hs->Release();
		}
		m_res_hs.reset();
	}

	bool HullShader::Activate(ID3D11DeviceContext* p_context)
	{
		if (m_res_hs)
		{
			if (m_res_hs->HasResource())
			{
				m_res_hs->Activate(p_context);
				return true;
			}
		}
		p_context->HSSetShader(nullptr, nullptr, 0);
		return false;
	}

	void HullShader::Deactivate(ID3D11DeviceContext* p_context)
	{
		p_context->HSSetShader(nullptr, nullptr, 0);
	}

	bool HullShader::HasResource() const
	{
		return m_res_hs == nullptr ? false : m_res_hs->HasResource();
	}

	const char* HullShader::GetName() const
	{
		return m_res_hs == nullptr ? nullptr : m_res_hs->GetName();
	}

	void HullShader::SetName(const std::string& name)
	{
		if (m_res_hs)
		{
			m_res_hs->SetName(name);
		}
	}
	const char* HullShader::GetFilePath() const
	{
		return m_res_hs ? m_res_hs->GetFilePath() : "";
	}
#pragma endregion


#pragma region Compute Shader
	ComputeShader::ComputeShader(const ComputeShader& cs)
	{
		m_res_cs = cs.m_res_cs;
	}

	ComputeShader& ComputeShader::operator=(const ComputeShader& cs)
	{
		m_res_cs = cs.m_res_cs;
		return *this;
	}

	void ComputeShader::RemoveUnused()
	{
		IResCS::RemoveUnused();
	}

	void ComputeShader::Compile(const std::string& filename, const std::string& entry_point, const std::string& shader_model, bool force_compile)
	{
		m_res_cs = IResCS::Compile(filename, entry_point, shader_model, force_compile);
	}

	void ComputeShader::CompileAsync(const std::string& filename, const std::string& entry_point, const std::string& shader_model, bool force_compile)
	{
		m_res_cs = IResCS::CompileAsync(filename, entry_point, shader_model, force_compile);
	}

	void ComputeShader::Load(const std::string& filename)
	{
		m_res_cs = IResCS::Load(filename);
	}

	void ComputeShader::LoadAsync(const std::string& filename)
	{
		m_res_cs = IResCS::LoadAsync(filename);
	}

	void ComputeShader::Remove()
	{
		m_res_cs.reset();
	}

	void ComputeShader::Release()
	{
		if (m_res_cs)
		{
			m_res_cs->Release();
		}
		m_res_cs.reset();
	}

	bool ComputeShader::Activate(ID3D11DeviceContext* p_context)
	{
		if (m_res_cs)
		{
			if (m_res_cs->HasResource())
			{
				m_res_cs->Activate(p_context);
				return true;
			}
		}
		p_context->CSSetShader(nullptr, nullptr, 0);
		return false;
	}

	void ComputeShader::Deactivate(ID3D11DeviceContext* p_context)
	{
		p_context->CSSetShader(nullptr, nullptr, 0);
	}

	bool ComputeShader::HasResource() const
	{
		return m_res_cs ? m_res_cs->HasResource() : false;
	}

	const char* ComputeShader::GetName() const
	{
		return m_res_cs ? m_res_cs->GetName() : nullptr;
	}

	void ComputeShader::SetName(const std::string& name)
	{
		if (m_res_cs)
		{
			m_res_cs->SetName(name);
		}
	}
	const char* ComputeShader::GetFilePath() const
	{
		return m_res_cs ? m_res_cs->GetFilePath() : "";
	}
#pragma endregion
}// namespace TKGEngine

namespace TKGEngine
{
	Shader::Shader(const Shader& s)
	{
		m_vs = s.m_vs;
		m_ps = s.m_ps;
		m_gs = s.m_gs;
		m_ds = s.m_ds;
		m_hs = s.m_hs;
	}

	Shader& Shader::operator=(const Shader& s)
	{
		m_vs = s.m_vs;
		m_ps = s.m_ps;
		m_gs = s.m_gs;
		m_ds = s.m_ds;
		m_hs = s.m_hs;
		return *this;
	}

	bool Shader::Activate(ID3D11DeviceContext* p_context, bool use_depth_ps)
	{
		bool ret = true;
		// 頂点シェーダーをセットできていないと描画に失敗するためfalseを返す
		ret = m_vs.Activate(p_context) == false ? false : (ret && true);
		if (!ret)
		{
			return false;
		}
		if (!use_depth_ps)
		{
			m_ps.Activate(p_context);
		}
		m_gs.Activate(p_context);
		m_ds.Activate(p_context);
		m_hs.Activate(p_context);

		return true;
	}

	void Shader::Deactivate(ID3D11DeviceContext* p_context)
	{
		VertexShader::Deactivate(p_context);
		PixelShader::Deactivate(p_context);
		GeometryShader::Deactivate(p_context);
		DomainShader::Deactivate(p_context);
		HullShader::Deactivate(p_context);
	}

	VertexShader& Shader::VS()
	{
		return m_vs;
	}

	PixelShader& Shader::PS()
	{
		return m_ps;
	}

	GeometryShader& Shader::GS()
	{
		return m_gs;
	}

	DomainShader& Shader::DS()
	{
		return m_ds;
	}

	HullShader& Shader::HS()
	{
		return m_hs;
	}

}// namespace TKGEngine