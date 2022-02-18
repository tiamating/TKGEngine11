
#include "../../inc/Texture.h"

#include "IResTexture.h"
#include "Utility/inc/myfunc_math.h"


namespace TKGEngine
{
	// ==================================================
	// class methods
	// ==================================================
	Texture::Texture(const Texture& tex)
	{
		m_res_texture = tex.m_res_texture;
	}

	Texture& Texture::operator=(const Texture& tex)
	{
		m_res_texture = tex.m_res_texture;
		return *this;
	}

	void Texture::RemoveUnused()
	{
		IResTexture::RemoveUnused();
	}

	void Texture::SetDummyWhiteSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility)
	{
		IResTexture::SetDummyWhiteSRV(p_context, slot, visibility);
	}

	void Texture::SetDummyBlackSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility)
	{
		IResTexture::SetDummyBlackSRV(p_context, slot, visibility);
	}

	ID3D11ShaderResourceView* const* Texture::GetDummyWhiteAddressOfSRV()
	{
		return IResTexture::GetDummyWhiteAddressOfSRV();
	}

	ID3D11ShaderResourceView* const* Texture::GetDummyBlackAddressOfSRV()
	{
		return IResTexture::GetDummyBlackAddressOfSRV();
	}

	const TEX_DESC* Texture::GetDummyTextureData()
	{
		return IResTexture::GetDummyTextureData();
	}

#ifdef USE_IMGUI
	void Texture::OnGUI()
	{
		// テクスチャを持っていたら表示する
		if (m_res_texture && m_res_texture->HasResource())
		{
			const auto tex_data = m_res_texture->GetData();
			// テクスチャデータ
			{
				// Dimension
				ImGui::Text("Texture %dD", static_cast<int>(tex_data->dimension) - 1);
				// W and H
				ImGui::Text("Width:(%d) Height(%d)", tex_data->width, tex_data->height);
				// Format
				ImGui::Text(NAMEOF_ENUM(tex_data->format).data());
				// array size
				ImGui::Text("Array size(%d)", tex_data->array_size);
				// mip level
				ImGui::Text("Mip Level(%d)", tex_data->mip_levels);
			}
			// 画像
			{
				const float button_width = ImGui::GetWindowWidth() * 0.3f;
				ImGui::ImageWindowButton(
					m_res_texture->GetSRV(), this,
					ImVec2(static_cast<float>(tex_data->width), static_cast<float>(tex_data->height)), 
					ImVec2(button_width, button_width)
				);
			}
		}
		// なければテキスト表示
		else
		{
			ImGui::TextColored(ImVec4(0.8f, 0.0f, 0.0f, 1.0f), "No Texture.");
		}
	}
#endif // USE_IMGUI

	void Texture::LoadAsync(const std::string& filename, bool force_srgb)
	{
		m_force_srgb = force_srgb;
		m_res_texture = IResTexture::LoadAsync(filename, force_srgb);
	}

	void Texture::Load(const std::string& filename, bool force_srgb)
	{
		m_force_srgb = force_srgb;
		m_res_texture = IResTexture::Load(filename, force_srgb);
	}

	void Texture::SetForceSRGB(bool force_srgb)
	{
		if (force_srgb != m_force_srgb)
		{
			m_force_srgb = force_srgb;
			if (m_res_texture->HasResource() == true)
			{
				IResTexture::Reload(m_res_texture->GetFilePath(), force_srgb);
			}
		}
	}

	bool Texture::GetForceSRGB() const
	{
		return m_force_srgb;
	}

	void Texture::Create(const TEX_DESC& desc, bool create_srv, bool create_uav, const void* p_src)
	{
		m_res_texture = IResTexture::Create(desc, create_srv, create_uav, p_src);
	}

	bool Texture::Save(const std::string& filename)
	{
		return m_res_texture->Save(filename);
	}

	bool Texture::UpdateTexture(const void* p_src_data)
	{
		// TODO : 使用状況が分からないため、作成保留
		assert(0);


		return false;
	}

	void Texture::Release()
	{
		m_res_texture->Release();
		m_res_texture.reset();
	}

	void Texture::SetSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility) const
	{
		if (m_res_texture != nullptr)
			m_res_texture->SetSRV(p_context, slot, visibility);
	}

	void Texture::SetUAV(ID3D11DeviceContext* p_context, int slot) const
	{
		if (m_res_texture != nullptr)
			m_res_texture->SetUAV(p_context, slot);
	}

	const TEX_DESC* Texture::GetData() const
	{
		return m_res_texture != nullptr ? m_res_texture->GetData() : nullptr;
	}

	ID3D11ShaderResourceView* Texture::GetSRV() const
	{
		return m_res_texture != nullptr ? m_res_texture->GetSRV() : nullptr;
	}

	ID3D11ShaderResourceView* const* Texture::GetAddressOfSRV() const
	{
		return m_res_texture != nullptr ? m_res_texture->GetAddressOfSRV() : nullptr;;
	}

	ID3D11Resource* Texture::GetResource() const
	{
		return m_res_texture != nullptr ? m_res_texture->GetResource() : nullptr;
	}

	bool Texture::IsLoaded() const
	{
		return m_res_texture != nullptr ? m_res_texture->IsLoaded() : false;
	}

	bool Texture::HasTexture() const
	{
		return m_res_texture == nullptr ? false : m_res_texture->HasResource();
	}

	const char* Texture::GetName() const
	{
		return m_res_texture == nullptr ? nullptr : m_res_texture->GetName();
	}

	void Texture::SetName(const std::string& name)
	{
		if (m_res_texture != nullptr)
		{
			m_res_texture->SetName(name);
		}
	}

	unsigned Texture::GetHash() const
	{
		return m_res_texture == nullptr ? 0 : m_res_texture->GetHash();
	}

}// namespace TKGEngine