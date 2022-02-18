#pragma once

#include "../../inc/AssetDataBase.h"

#include "../ResourceManager.h"
#include "../../inc/Texture_Defined.h"
#include "../../inc/Shader_Defined.h"

#include <string>
#include <memory>
#include <mutex>

namespace TKGEngine
{
	class IResTexture
		: public AssetDataBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		IResTexture() = default;
		virtual ~IResTexture() = default;
		IResTexture(const IResTexture&) = delete;
		IResTexture& operator=(const IResTexture&) = delete;

		static bool CreateDummyTexture();
		static std::shared_ptr<IResTexture> LoadAsync(const std::string& filename, bool force_srgb);
		static std::shared_ptr<IResTexture> Load(const std::string& filename, bool force_srgb);
		static void Reload(const std::string& filename, bool force_srgb);
		static std::shared_ptr<IResTexture> Create(const TEX_DESC& desc, bool create_srv, bool create_uav, const void* p_src);
		static void RemoveUnused();

		static void SetDummyWhiteSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility);
		static void SetDummyBlackSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility);
		static ID3D11ShaderResourceView* const* GetDummyWhiteAddressOfSRV();
		static ID3D11ShaderResourceView* const* GetDummyBlackAddressOfSRV();
		const static TEX_DESC* GetDummyTextureData();

		virtual bool Save(const std::string& filename) = 0;

		virtual void SetSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility) const = 0;
		virtual void SetUAV(ID3D11DeviceContext* p_context, int slot) const = 0;

		virtual const TEX_DESC* GetData() const = 0;
		virtual ID3D11ShaderResourceView* GetSRV() const = 0;
		virtual ID3D11ShaderResourceView* const* GetAddressOfSRV() const = 0;
		virtual ID3D11Resource* GetResource() const = 0;

		// AssetDataBase
		void Release() override;
		// ~AssetDataBase

	private:
		// ==============================================
		// private methods
		// ==============================================
		virtual void SetAsyncOnLoad() = 0;
		virtual void OnLoad() = 0;

		virtual void SetForceSRGB(bool force_srgb) = 0;
		virtual bool GetForceSRGB() const = 0;
		virtual bool CreateTexture(const TEX_DESC& desc, bool create_srv, bool create_uav, const void* p_data) = 0;

		static std::shared_ptr<IResTexture> CreateInterface();

		// ==============================================
		// private variables
		// ==============================================
		static ResourceManager<IResTexture> m_caches;
		static std::mutex m_cache_mutex;

		static bool m_dummy_created;
		static std::unique_ptr<IResTexture> m_dummy_tex_white;
		static std::unique_ptr<IResTexture> m_dummy_tex_black;
		static TEX_DESC m_dummy_tex_data;
	};


}	// namespace TKGEngine