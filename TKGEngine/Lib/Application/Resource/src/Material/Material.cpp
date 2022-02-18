
#include "../../inc/Material.h"


namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Static member definition
	////////////////////////////////////////////////////////
	bool Material::m_created_default_material = false;
	Material Material::m_default_material;
	static constexpr const char* DEFAULT_MATERIAL_VS = "./Shader/BasicVS.cso";
	static constexpr const char* DEFAULT_MATERIAL_PS = "./Shader/BasicPS.cso";


	////////////////////////////////////////////////////////
	// class methods
	////////////////////////////////////////////////////////
	Material::Material(const Material& m)
	{
		m_res_material = m.m_res_material;
	}

	void Material::operator=(const Material& m)
	{
		m_res_material = m.m_res_material;
	}

	void Material::RemoveUnused()
	{
		IResMaterial::RemoveUnused();
	}

#ifdef USE_IMGUI
	bool Material::OnGUI(const bool is_shadow_material) const
	{
		if (m_res_material)
		{
			m_res_material->OnGUI(is_shadow_material);
			return m_res_material->IsSavedOnFrame();
		}
		return false;
	}
#endif // USE_IMGUI

	void Material::Create()
	{
		m_res_material = IResMaterial::Create();
	}

	void Material::Create(const std::string& name)
	{
		m_res_material = IResMaterial::Create(name);
	}

	void Material::LoadAsync(const std::string& filename)
	{
		m_res_material = IResMaterial::LoadAsync(filename);
	}

	void Material::Load(const std::string& filename)
	{
		m_res_material = IResMaterial::Load(filename);
	}

	void Material::Release()
	{
		if (m_res_material)
		{
			m_res_material->Release();
		}
		m_res_material.reset();
	}

	bool Material::Activate(ID3D11DeviceContext* p_context, const bool write_depth, const bool set_depth_ps) const
	{
		if (!m_res_material)
			return false;

		return m_res_material->Activate(p_context, write_depth, set_depth_ps);
	}

	int Material::GetRenderQueue() const
	{
		return m_res_material == nullptr ? -1 : m_res_material->GetRenderQueue();
	}

	void Material::SetRenderQueue(int queue) const
	{
		if (!m_res_material)
			return;

		m_res_material->SetRenderQueue(queue);
	}

	void Material::UseTarget(bool use_target) const
	{
		if (!m_res_material)
			return;

		m_res_material->UseTarget(use_target);
	}

	bool Material::IsUsedTarget() const
	{
		return m_res_material == nullptr ? false : m_res_material->IsUsedTarget();
	}

	void Material::SetStencilMask(Stencil::Mask mask) const
	{
		if (!m_res_material)
			return;

		m_res_material->SetStencilMask(mask);
	}

	Stencil::Mask Material::GetStencilMask() const
	{
		return m_res_material == nullptr ? Stencil::Mask::None : m_res_material->GetStencilMask();
	}

	void Material::SetTopology(PRIMITIVE_TOPOLOGY topology) const
	{
		if (!m_res_material)
			return;

		m_res_material->SetTopology(topology);
	}

	void Material::SetDepthState(StateManager::DS state) const
	{
		if (!m_res_material)
			return;

		m_res_material->SetDepthState(state);
	}

	void Material::SetRasterizerState(StateManager::RS state) const
	{
		if (!m_res_material)
			return;

		m_res_material->SetRasterizerState(state);
	}

	void Material::SetBlendState(StateManager::BS state) const
	{
		if (!m_res_material)
			return;

		m_res_material->SetBlendState(state);
	}

	int Material::GetInputSlotFromType(VERTEX_ELEMENT_TYPE type) const
	{
		if (!m_res_material)
			return -1;

		return m_res_material->GetInputSlotFromType(type);
	}

	int Material::GetInputSlotFromType(int type) const
	{
		if (type < 0 || type >= static_cast<int>(VERTEX_ELEMENT_TYPE::MAX_TYPE_NUM))
		{
			return -1;
		}

		return GetInputSlotFromType(static_cast<VERTEX_ELEMENT_TYPE>(type));
	}

	void Material::SetVertexShader(const std::string& filepath) const
	{
		if (!m_res_material)
			return;

		m_res_material->SetVertexShader(filepath);
	}

	void Material::SetPixelShader(const std::string& filepath) const
	{
		if (!m_res_material)
			return;

		m_res_material->SetPixelShader(filepath);
	}

	void Material::SetDepthPixelShader(const std::string& filepath) const
	{
		if (!m_res_material)
			return;

		m_res_material->SetDepthPixelShader(filepath);
	}

	void Material::SetGeometryShader(const std::string& filepath) const
	{
		if (!m_res_material)
			return;

		m_res_material->SetGeometryShader(filepath);
	}

	void Material::SetDomainShader(const std::string& filepath) const
	{
		if (!m_res_material)
			return;

		m_res_material->SetDomainShader(filepath);
	}

	void Material::SetHullShader(const std::string& filepath) const
	{
		if (!m_res_material)
			return;

		m_res_material->SetHullShader(filepath);
	}

	void Material::RemoveVertexShader() const
	{
		if (!m_res_material)
			return;

		m_res_material->RemoveVertexShader();
	}

	void Material::RemovePixelShader() const
	{
		if (!m_res_material)
			return;

		m_res_material->RemovePixelShader();
	}

	void Material::RemoveDepthPixelShader() const
	{
		if (!m_res_material)
			return;

		m_res_material->RemoveDepthPixelShader();
	}

	void Material::RemoveGeometryShader() const
	{
		if (!m_res_material)
			return;

		m_res_material->RemoveGeometryShader();
	}

	void Material::RemoveDomainShader() const
	{
		if (!m_res_material)
			return;

		m_res_material->RemoveDomainShader();
	}

	void Material::RemoveHullShader() const
	{
		if (!m_res_material)
			return;

		m_res_material->RemoveHullShader();
	}

	void Material::AddTexture(int slot, const std::string& filepath, bool force_srgb) const
	{
		if (!m_res_material)
			return;

		m_res_material->AddTexture(slot, filepath, force_srgb);
	}

	void Material::RemoveTexture() const
	{
		if (!m_res_material)
			return;

		m_res_material->RemoveTexture();
	}

	void Material::CreateCBuffer() const
	{
		if (!m_res_material)
			return;

		m_res_material->CreateCBMaterial();
	}

	void Material::SetTextureOffset(const VECTOR2& offset) const
	{
		if (!m_res_material)
			return;

		m_res_material->SetTextureOffset(offset);
	}

	void Material::SetTextureTilling(const VECTOR2& tilling) const
	{
		if (!m_res_material)
			return;

		m_res_material->SetTextureTilling(tilling);
	}

	void Material::SetTextureCutout(const float cutout) const
	{
		if (!m_res_material)
			return;

		m_res_material->SetTextureCutout(cutout);
	}

	bool Material::IsLoaded() const
	{
		return m_res_material != nullptr ? m_res_material->IsLoaded() : false;
	}

	bool Material::HasMaterial() const
	{
		if (!m_res_material)
			return false;

		return m_res_material->HasResource();
	}

	bool Material::HasVertexShader() const
	{
		return m_res_material != nullptr ? m_res_material->HasVertexShader() : false;
	}

	bool Material::HasPixelShader() const
	{
		return m_res_material != nullptr ? m_res_material->HasPixelShader() : false;
	}

	bool Material::HasDepthPixelShader() const
	{
		return m_res_material != nullptr ? m_res_material->HasDepthPixelShader() : false;
	}

	bool Material::HasGeometryShader() const
	{
		return m_res_material != nullptr ? m_res_material->HasGeometryShader() : false;
	}

	bool Material::HasDomainShader() const
	{
		return m_res_material != nullptr ? m_res_material->HasDomainShader() : false;
	}

	bool Material::HasHullShader() const
	{
		return m_res_material != nullptr ? m_res_material->HasHullShader() : false;
	}

	bool Material::HasTextures() const
	{
		return m_res_material != nullptr ? m_res_material->HasTextures() : false;
	}

	const char* Material::GetName() const
	{
		return m_res_material == nullptr ? nullptr : m_res_material->GetName();
	}

	void Material::SetName(const std::string& name) const
	{
		if (m_res_material)
			m_res_material->SetName(name);
	}

	const char* Material::GetFilePath() const
	{
		return m_res_material ? m_res_material->GetFilePath() : "";
	}

	unsigned Material::GetHash() const
	{
		return m_res_material == nullptr ? 0 : m_res_material->GetHash();
	}

	Material& Material::GetDefaultMaterial()
	{
		if (m_created_default_material == false)
		{
			CreateDefaultMaterial();
			m_created_default_material = true;
		}
		return m_default_material;
	}

	void Material::CreateDefaultMaterial()
	{
		m_default_material.Create();
		m_default_material.SetVertexShader(DEFAULT_MATERIAL_VS);
		m_default_material.SetPixelShader(DEFAULT_MATERIAL_PS);
	}

}// namespace TKGEngine