#pragma once

#include "Systems/inc/TKGEngine_Defined.h"
#include "Application/Resource/inc/Shader_Defined.h"
#include "Texture_Defined.h"

#include <memory>
#include <string>

struct ID3D11ShaderResourceView;
struct ID3D11Resource;

namespace TKGEngine
{
	class IResTexture;

	class Texture
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Texture() = default;
		virtual ~Texture() = default;
		Texture(const Texture&);
		Texture& operator=(const Texture&);

		static void RemoveUnused();

		static void SetDummyWhiteSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility);
		static void SetDummyBlackSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility);
		static ID3D11ShaderResourceView* const* GetDummyWhiteAddressOfSRV();
		static ID3D11ShaderResourceView* const* GetDummyBlackAddressOfSRV();
		static const TEX_DESC* GetDummyTextureData();

#ifdef USE_IMGUI
		void OnGUI();
#endif // USE_IMGUI

		void LoadAsync(const std::string& filename, bool force_srgb = false);
		void Load(const std::string& filename, bool force_srgb = false);
		void SetForceSRGB(bool force_srgb);
		bool GetForceSRGB() const;
		void Create(const TEX_DESC& desc, bool create_srv, bool create_uav, const void* p_src);
		bool Save(const std::string& filename);

		/// <summary>
		/// Write to subresource from source data
		/// </summary>
		bool UpdateTexture(const void* p_src_data);

		void Release();

		void SetSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility) const;
		void SetUAV(ID3D11DeviceContext* p_context, int slot) const;

		const TEX_DESC* GetData() const;
		ID3D11ShaderResourceView* GetSRV() const;
		ID3D11ShaderResourceView* const* GetAddressOfSRV() const;
		ID3D11Resource* GetResource() const;

		bool IsLoaded() const;
		bool HasTexture() const;
		const char* GetName() const;
		void SetName(const std::string& name);

		unsigned GetHash() const;

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================
		/* nothing */


		// ==============================================
		// private variables
		// ==============================================
		std::shared_ptr<IResTexture> m_res_texture = nullptr;
		bool m_force_srgb = false;
	};


}	// namespace TKGEngine