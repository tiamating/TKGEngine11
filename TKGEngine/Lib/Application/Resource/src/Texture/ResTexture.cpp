
#include "IResTexture.h"

#include "Application/Resource/inc/Shader_Defined.h"
#include "Systems/inc/LogSystem.h"
#include "Systems/inc/AssetSystem.h"
#include "Systems/inc/Graphics_Defined.h"
#include "Utility/inc/myfunc_string.h"

#include "../../DirectXTK/Inc/DDSTextureLoader.h"
#include "../../DirectXTK/Inc/WICTextureLoader.h"
#include "../../../external/DirectXTex/Inc/DirectXTex/DirectXTex.h"

#include <d3d11.h>

#include <unordered_map>
#include <vector>
#include <functional>
#include <utility>
#include <wrl.h>
#include <cassert>

#pragma comment(lib, "DirectXTK.lib")
#pragma comment(lib, "DirectXTex.lib")


namespace TKGEngine
{
	class ResTexture
		: public IResTexture
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		ResTexture() = default;
		virtual ~ResTexture() = default;
		ResTexture(const ResTexture&) = delete;
		ResTexture& operator=(const ResTexture&) = delete;

		// AssetDataBase
#ifdef USE_IMGUI
		void OnGUI() override {}
#endif // USE_IMGUI
		// ~AssetDataBase

		// IResTexture
		bool Save(const std::string& filename) override;
		void SetSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility) const override;
		void SetUAV(ID3D11DeviceContext* p_context, int slot) const override;
		const TEX_DESC* GetData() const override;
		ID3D11ShaderResourceView* GetSRV() const override;
		ID3D11ShaderResourceView* const* GetAddressOfSRV() const override;
		ID3D11Resource* GetResource() const override;
		// ~IResTexture

		static HRESULT LoadWICTexture(
			ID3D11Device* p_device,
			ID3D11DeviceContext* p_context,
			const std::wstring& path,
			const bool force_srgb,
			ID3D11Resource** pp_resource,
			ID3D11ShaderResourceView** pp_srv);

		static HRESULT LoadDDSTexture(
			ID3D11Device* p_device,
			ID3D11DeviceContext* p_context,
			const std::wstring& path,
			const bool force_srgb,
			ID3D11Resource** pp_resource,
			ID3D11ShaderResourceView** pp_srv);

		static HRESULT LoadTGATexture(
			ID3D11Device* p_device,
			ID3D11DeviceContext* p_context,
			const std::wstring& path,
			const bool force_srgb,
			ID3D11Resource** pp_resource,
			ID3D11ShaderResourceView** pp_srv);

		static HRESULT LoadHDRTexture(
			ID3D11Device* p_device,
			ID3D11DeviceContext* p_context,
			const std::wstring& path,
			const bool force_srgb,
			ID3D11Resource** pp_resource,
			ID3D11ShaderResourceView** pp_srv);


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================
		void SetAsyncOnLoad() override;
		void OnLoad() override;

		void SetForceSRGB(bool force_srgb) override;
		bool GetForceSRGB() const override;

		void GetDescTexture2D();
		void GetDescTexture3D();

		bool CreateTexture(const TEX_DESC& desc, bool create_srv, bool create_uav, const void* p_data) override;
		static bool CreateTexture1D(const TEX_DESC& desc, bool create_srv, bool create_uav, const void* p_data, ID3D11Resource** pp_resource);
		static bool CreateTexture2D(const TEX_DESC& desc, bool create_srv, bool create_uav, const void* p_data, ID3D11Resource** pp_resource);
		static bool CreateTexture3D(const TEX_DESC& desc, bool create_srv, bool create_uav, const void* p_data, ID3D11Resource** pp_resource);

		static HRESULT CreateTextureFromImage(
			ID3D11Device* p_device,
			const DirectX::Image* p_image,
			const size_t image_count,
			const DirectX::TexMetadata& meta,
			const bool force_srgb,
			ID3D11Resource** pp_resource,
			ID3D11ShaderResourceView** pp_srv);


		// ==============================================
		// private variables
		// ==============================================
		bool m_force_srgb = false;
		TEX_DESC m_tex_desc;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV = nullptr;
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_UAV = nullptr;
		Microsoft::WRL::ComPtr<ID3D11Resource> m_resource = nullptr;

		static std::unordered_map<
			std::string,
			std::function<HRESULT(ID3D11Device*, ID3D11DeviceContext*, const std::wstring&, bool, ID3D11Resource**, ID3D11ShaderResourceView**)>
		> m_load_lambda_table;
	};


	////////////////////////////////////////////////////////
	// Static variable declaration
	////////////////////////////////////////////////////////
	namespace /* anonymous */
	{
		std::pair<
			std::string,
			std::function< HRESULT(ID3D11Device*, ID3D11DeviceContext*, const std::wstring&, bool, ID3D11Resource**, ID3D11ShaderResourceView**)>
		> g_load_lambdas[] =
		{
			{"bmp", [](ID3D11Device* p_device, ID3D11DeviceContext* p_context, const std::wstring& path, bool force_srgb, ID3D11Resource** pp_resource, ID3D11ShaderResourceView** pp_srv)-> HRESULT
				{
					return DirectX::CreateWICTextureFromFileEx(
						p_device,path.c_str(),0,D3D11_USAGE_DEFAULT,D3D11_BIND_SHADER_RESOURCE,0,0,force_srgb ? DirectX::WIC_LOADER_FORCE_SRGB : DirectX::WIC_LOADER_DEFAULT,pp_resource,pp_srv);
				}
			},
			{"png", [](ID3D11Device* p_device, ID3D11DeviceContext* p_context, const std::wstring& path, bool force_srgb, ID3D11Resource** pp_resource, ID3D11ShaderResourceView** pp_srv)-> HRESULT
				{
					return DirectX::CreateWICTextureFromFileEx(
						p_device,path.c_str(),0,D3D11_USAGE_DEFAULT,D3D11_BIND_SHADER_RESOURCE,0,0,force_srgb ? DirectX::WIC_LOADER_FORCE_SRGB : DirectX::WIC_LOADER_DEFAULT,pp_resource,pp_srv);
				}
			},
			{"gif", [](ID3D11Device* p_device, ID3D11DeviceContext* p_context, const std::wstring& path, bool force_srgb, ID3D11Resource** pp_resource, ID3D11ShaderResourceView** pp_srv)-> HRESULT
				{
					return DirectX::CreateWICTextureFromFileEx(
						p_device,path.c_str(),0,D3D11_USAGE_DEFAULT,D3D11_BIND_SHADER_RESOURCE,0,0,force_srgb ? DirectX::WIC_LOADER_FORCE_SRGB : DirectX::WIC_LOADER_DEFAULT,pp_resource,pp_srv);
				}
			},
			{"tiff", [](ID3D11Device* p_device, ID3D11DeviceContext* p_context, const std::wstring& path, bool force_srgb, ID3D11Resource** pp_resource, ID3D11ShaderResourceView** pp_srv)-> HRESULT
				{
					return DirectX::CreateWICTextureFromFileEx(
						p_device,path.c_str(),0,D3D11_USAGE_DEFAULT,D3D11_BIND_SHADER_RESOURCE,0,0,force_srgb ? DirectX::WIC_LOADER_FORCE_SRGB : DirectX::WIC_LOADER_DEFAULT,pp_resource,pp_srv);
				}
			},
			{"jpeg", [](ID3D11Device* p_device, ID3D11DeviceContext* p_context, const std::wstring& path, bool force_srgb, ID3D11Resource** pp_resource, ID3D11ShaderResourceView** pp_srv)-> HRESULT
				{
					return DirectX::CreateWICTextureFromFileEx(
						p_device,path.c_str(),0,D3D11_USAGE_DEFAULT,D3D11_BIND_SHADER_RESOURCE,0,0,force_srgb ? DirectX::WIC_LOADER_FORCE_SRGB : DirectX::WIC_LOADER_DEFAULT,pp_resource,pp_srv);
				}
			},
			{"jpg", [](ID3D11Device* p_device, ID3D11DeviceContext* p_context, const std::wstring& path, bool force_srgb, ID3D11Resource** pp_resource, ID3D11ShaderResourceView** pp_srv)-> HRESULT
				{
					return DirectX::CreateWICTextureFromFileEx(
						p_device,path.c_str(),0,D3D11_USAGE_DEFAULT,D3D11_BIND_SHADER_RESOURCE,0,0,force_srgb ? DirectX::WIC_LOADER_FORCE_SRGB : DirectX::WIC_LOADER_DEFAULT,pp_resource,pp_srv);
				}
			},
			{"dds", [](ID3D11Device* p_device, ID3D11DeviceContext* p_context, const std::wstring& path, bool force_srgb, ID3D11Resource** pp_resource, ID3D11ShaderResourceView** pp_srv)-> HRESULT
				{
					return DirectX::CreateDDSTextureFromFileEx(
						p_device,path.c_str(),0,D3D11_USAGE_DEFAULT,D3D11_BIND_SHADER_RESOURCE,0,0,force_srgb ? DirectX::WIC_LOADER_FORCE_SRGB : DirectX::WIC_LOADER_DEFAULT,pp_resource,pp_srv);
				}
			},
			{"tga", [](ID3D11Device* p_device, ID3D11DeviceContext* p_context, const std::wstring& path, bool force_srgb, ID3D11Resource** pp_resource, ID3D11ShaderResourceView** pp_srv)-> HRESULT
				{
					return ResTexture::LoadTGATexture(p_device,p_context,path.c_str(),force_srgb,pp_resource,pp_srv);
				}
			},
			{"hdr", [](ID3D11Device* p_device, ID3D11DeviceContext* p_context, const std::wstring& path, bool force_srgb, ID3D11Resource** pp_resource, ID3D11ShaderResourceView** pp_srv)-> HRESULT
				{
					return ResTexture::LoadHDRTexture(p_device,p_context,path.c_str(),force_srgb,pp_resource,pp_srv);
				}
			}
		};
	}// namespace /* anonymous */


	////////////////////////////////////////////////////////
	// Static member definition
	////////////////////////////////////////////////////////
	// IResTexture
	ResourceManager<IResTexture> IResTexture::m_caches;
	std::mutex IResTexture::m_cache_mutex;
	bool IResTexture::m_dummy_created = false;
	std::unique_ptr<IResTexture> IResTexture::m_dummy_tex_white = nullptr;
	std::unique_ptr<IResTexture> IResTexture::m_dummy_tex_black = nullptr;
	TEX_DESC IResTexture::m_dummy_tex_data;

	// ResTexture
	std::unordered_map<
		std::string,
		std::function<HRESULT(ID3D11Device*, ID3D11DeviceContext*, const std::wstring&, bool, ID3D11Resource**, ID3D11ShaderResourceView**)>
	> ResTexture::m_load_lambda_table(g_load_lambdas, g_load_lambdas + ARRAYSIZE(g_load_lambdas));

	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	std::shared_ptr<IResTexture> IResTexture::CreateInterface()
	{
		if (!m_dummy_created)
		{
			CreateDummyTexture();
			m_dummy_created = true;
		}
		return std::static_pointer_cast<IResTexture>(std::make_shared<ResTexture>());
	}

	std::shared_ptr<IResTexture> IResTexture::LoadAsync(const std::string& filename, bool force_srgb)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResTexture> res_ptr = m_caches.Search(filename);
		if (res_ptr)
		{
			// SRGBに変更がない
			if (res_ptr->GetForceSRGB() == force_srgb)
			{
				m_cache_mutex.unlock();
				return res_ptr;
			}
			// SRGBに変更があり同名ファイルが存在する
			else
			{
				m_cache_mutex.unlock();
				Reload(filename, force_srgb);
				return res_ptr;
			}
		}
		else
		{
			res_ptr = IResTexture::CreateInterface();
		}
		res_ptr->SetFilePath(filename);
		m_caches.Set(filename, res_ptr);
		m_cache_mutex.unlock();

		// Set async loader
		res_ptr->SetForceSRGB(force_srgb);
		std::lock_guard<std::mutex> lock(res_ptr->m_load_state_mutex);
		res_ptr->m_is_loading = true;
		res_ptr->m_is_loaded = false;
		res_ptr->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		AssetSystem::GetInstance().Add(std::bind(&IResTexture::SetAsyncOnLoad, res_ptr));

		return res_ptr;
	}

	std::shared_ptr<IResTexture> IResTexture::Load(const std::string& filename, bool force_srgb)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResTexture> res_ptr = m_caches.Search(filename);
		if (res_ptr)
		{
			// SRGBに変更がない
			if (res_ptr->GetForceSRGB() == force_srgb)
			{
				m_cache_mutex.unlock();
				return res_ptr;
			}
			// SRGBに変更があり同名ファイルが存在する
			else
			{
				m_cache_mutex.unlock();
				Reload(filename, force_srgb);
				return res_ptr;
			}
		}
		else
		{
			res_ptr = IResTexture::CreateInterface();
		}
		res_ptr->SetFilePath(filename);
		m_caches.Set(filename, res_ptr);
		m_cache_mutex.unlock();

		// Load Resource
		res_ptr->SetForceSRGB(force_srgb);
		res_ptr->m_load_state_mutex.lock();
		res_ptr->m_is_loading = true;
		res_ptr->m_is_loaded = false;
		res_ptr->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		res_ptr->OnLoad();
		if (!res_ptr->HasResource())
		{
			res_ptr->Release();
			res_ptr->m_load_state_mutex.unlock();
			res_ptr.reset();
		}
		else
		{
			res_ptr->m_load_state_mutex.unlock();
		}
		return res_ptr;
	}

	void IResTexture::Reload(const std::string& filename, bool force_srgb)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResTexture> res_find = m_caches.Search(filename);
		m_cache_mutex.unlock();
		if (res_find == nullptr)
		{
			return;
		}

		// Load Resource
		res_find->SetForceSRGB(force_srgb);
		res_find->m_load_state_mutex.lock();
		res_find->m_is_loading = true;
		res_find->m_is_loaded = false;
		res_find->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		res_find->OnLoad();
		if (!res_find->HasResource())
		{
			res_find->Release();
			res_find.reset();
		}
		res_find->m_load_state_mutex.unlock();
		return;
	}

	std::shared_ptr<IResTexture> IResTexture::Create(const TEX_DESC& desc, bool create_srv, bool create_uav, const void* p_src)
	{
		std::shared_ptr<IResTexture> res_new = IResTexture::CreateInterface();

		if (res_new->CreateTexture(desc, create_srv, create_uav, p_src))
		{
			res_new->m_has_resource = true;
		}
		else
		{
			return std::shared_ptr<IResTexture>();
		}

		return res_new;
	}

	void IResTexture::RemoveUnused()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveUnusedCache();
	}

	void IResTexture::SetDummyWhiteSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility)
	{
		m_dummy_tex_white->SetSRV(p_context, slot, visibility);
	}

	void IResTexture::SetDummyBlackSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility)
	{
		m_dummy_tex_black->SetSRV(p_context, slot, visibility);
	}

	ID3D11ShaderResourceView* const* IResTexture::GetDummyWhiteAddressOfSRV()
	{
		return m_dummy_tex_white->GetAddressOfSRV();
	}

	ID3D11ShaderResourceView* const* IResTexture::GetDummyBlackAddressOfSRV()
	{
		return m_dummy_tex_black->GetAddressOfSRV();
	}

	const TEX_DESC* IResTexture::GetDummyTextureData()
	{
		return &m_dummy_tex_data;
	}

	bool IResTexture::CreateDummyTexture()
	{
		std::vector<unsigned char> datas;
		// Create Texture Description
		{
			m_dummy_tex_data.format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 4bytes
			m_dummy_tex_data.width = 4;		// 4 elements
			m_dummy_tex_data.height = 4;	// 4 elements
			m_dummy_tex_data.depth = 1;
			m_dummy_tex_data.array_size = 1;
			m_dummy_tex_data.mip_levels = 1;
			m_dummy_tex_data.row_pitch = m_dummy_tex_data.width * 4;
			m_dummy_tex_data.slice_pitch = m_dummy_tex_data.height * m_dummy_tex_data.row_pitch;
			m_dummy_tex_data.option = TEXTURE_OPTION::TEXTURE_OPTION_NONE;
			m_dummy_tex_data.dimension = TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE2D;
		}

		// Create Dummy Black
		{
			IResTexture::m_dummy_tex_black = std::make_unique<ResTexture>();
			
			// Create Texture Data
			{
				datas.resize(m_dummy_tex_data.slice_pitch);

				for (int i = 0; i < m_dummy_tex_data.slice_pitch; ++i)
				{
					if (i % 4 == 3)
					{
						datas.at(i) = static_cast<unsigned char>(0xff);
					}
					else
					{
						datas.at(i) = static_cast<unsigned char>(0);
					}
				}
			}

			if (m_dummy_tex_black.get()->CreateTexture(m_dummy_tex_data, true, false, datas.data()) == false)
			{
				assert(0 && "failed Create Dummy Black IResTexture::CreateDummyTexture()");
				return false;
			}
		}

		// Create Dummy White
		{
			IResTexture::m_dummy_tex_white = std::make_unique<ResTexture>();

			// Create Texture Data
			{
				for (auto&& clr : datas)
				{
					clr = static_cast<unsigned char>(0xff);
				}
			}

			if (m_dummy_tex_white.get()->CreateTexture(m_dummy_tex_data, true, false, datas.data()) == false)
			{
				assert(0 && "failed Create Dummy White IResTexture::CreateDummyTexture()");
				return false;
			}
		}
		return true;
	}

	void IResTexture::Release()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveCache(this->GetFilePath());
	}

	bool ResTexture::Save(const std::string& filename)
	{
		// Name resource's filepath
		SetFilePath(filename);

		assert(0);

		return true;
	}

	void ResTexture::SetSRV(ID3D11DeviceContext* p_context, int slot, ShaderVisibility visibility) const
	{
		switch (visibility)
		{
		case ShaderVisibility::VS:
			p_context->VSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::PS:
			p_context->PSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::GS:
			p_context->GSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::DS:
			p_context->DSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::HS:
			p_context->HSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::ALL:
			p_context->VSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			p_context->PSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			p_context->GSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			p_context->DSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			p_context->HSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;

		case ShaderVisibility::CS:
			p_context->CSSetShaderResources(slot, 1, m_SRV.GetAddressOf());
			break;
		}
	}

	void ResTexture::SetUAV(ID3D11DeviceContext* p_context, int slot) const
	{
		p_context->CSSetUnorderedAccessViews(slot, 1, m_UAV.GetAddressOf(), nullptr);
	}

	const TEX_DESC* ResTexture::GetData() const
	{
		return  &m_tex_desc;
	}

	ID3D11ShaderResourceView* ResTexture::GetSRV() const
	{
		return m_has_resource ? m_SRV.Get() : nullptr;
	}

	ID3D11ShaderResourceView* const* ResTexture::GetAddressOfSRV() const
	{
		return m_has_resource ? m_SRV.GetAddressOf() : nullptr;
	}

	ID3D11Resource* ResTexture::GetResource() const
	{
		return m_has_resource ? m_resource.Get() : nullptr;
	}

	HRESULT ResTexture::LoadWICTexture(
		ID3D11Device* p_device,
		ID3D11DeviceContext* p_context,
		const std::wstring& path,
		const bool force_srgb,
		ID3D11Resource** pp_resource,
		ID3D11ShaderResourceView** pp_srv
	)
	{
		using namespace DirectX;

		auto image = std::make_unique<ScratchImage>();
		TexMetadata meta = {};
		const HRESULT hr = LoadFromWICFile(path.c_str(), 0, &meta, *image);
		if (FAILED(hr))
		{
			return hr;
		}

		const Image* img = image->GetImage(0, 0, 0);
		if (img == nullptr)
		{
			return E_FAIL;
		}
		// TODO : GetImagesではなくGetImageで一枚だけしか対応しないようにしている
		return CreateTextureFromImage(p_device, img,/*image->GetImageCount()*/1, meta, force_srgb, pp_resource, pp_srv);
	}

	HRESULT ResTexture::LoadDDSTexture(
		ID3D11Device* p_device,
		ID3D11DeviceContext* p_context,
		const std::wstring& path,
		const bool force_srgb,
		ID3D11Resource** pp_resource,
		ID3D11ShaderResourceView** pp_srv
	)
	{
		using namespace DirectX;

		auto image = std::make_unique<ScratchImage>();
		TexMetadata meta = {};
		const HRESULT hr = LoadFromDDSFile(path.c_str(), 0, &meta, *image);
		if (FAILED(hr))
		{
			return hr;
		}

		const Image* img = image->GetImage(0, 0, 0);
		if (!img)
		{
			return E_FAIL;
		}
		// TODO : GetImagesではなくGetImageで一枚だけしか対応しないようにしている
		return CreateTextureFromImage(p_device, img,/*image->GetImageCount()*/1, meta, force_srgb, pp_resource, pp_srv);
	}

	HRESULT ResTexture::LoadTGATexture(
		ID3D11Device* p_device,
		ID3D11DeviceContext* p_context,
		const std::wstring& path,
		const bool force_srgb,
		ID3D11Resource** pp_resource,
		ID3D11ShaderResourceView** pp_srv)
	{
		using namespace DirectX;

		auto image = std::make_unique<ScratchImage>();
		TexMetadata meta = {};
		const HRESULT hr = LoadFromTGAFile(path.c_str(), &meta, *image);
		if (FAILED(hr))
		{
			return hr;
		}

		const Image* img = image->GetImage(0, 0, 0);
		if (!img)
		{
			return E_FAIL;
		}
		// TODO : GetImagesではなくGetImageで一枚だけしか対応しないようにしている
		return CreateTextureFromImage(p_device, img,/*image->GetImageCount()*/1, meta, force_srgb, pp_resource, pp_srv);
	}

	HRESULT ResTexture::LoadHDRTexture(
		ID3D11Device* p_device,
		ID3D11DeviceContext* p_context,
		const std::wstring& path,
		const bool force_srgb,
		ID3D11Resource** pp_resource,
		ID3D11ShaderResourceView** pp_srv)
	{
		using namespace DirectX;

		auto image = std::make_unique<ScratchImage>();
		TexMetadata meta = {};
		const HRESULT hr = LoadFromHDRFile(path.c_str(), &meta, *image);
		if (FAILED(hr))
		{
			return hr;
		}

		const Image* img = image->GetImage(0, 0, 0);
		if (!img)
		{
			return E_FAIL;
		}

		// TODO : GetImagesではなくGetImageで一枚だけしか対応しないようにしている
		return CreateTextureFromImage(p_device, img,/*image->GetImageCount()*/1, meta, force_srgb, pp_resource, pp_srv);
	}

	void ResTexture::SetAsyncOnLoad()
	{
		// Start loading
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Load Texture
		HRESULT hr = S_OK;
		{
			const std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			std::string extend = MyFunc::GetExtension(this->GetFilePath());
			MyFunc::ToLower(extend);

			hr = m_load_lambda_table[extend](
				AssetSystem::GetInstance().GetDevice(), nullptr,
				wfilename, m_force_srgb,
				m_resource.GetAddressOf(), m_SRV.GetAddressOf());
		}

		// Finish loading
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_is_loading = false;
			m_is_loaded = true;
			if (FAILED(hr))
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
				m_tex_desc.Clear();
			}
			else
			{
				D3D11_RESOURCE_DIMENSION dim;
				m_resource->GetType(&dim);
				switch (dim)
				{
				case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
					GetDescTexture2D();
					break;
				case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
					GetDescTexture3D();
					break;
				default:
					assert(0 && "invalid resource dimension");
				}

				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}

	void ResTexture::OnLoad()
	{
		// Ready loading
		{
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Load Texture
		HRESULT hr = S_OK;
		{
			const std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			std::string extend = MyFunc::GetExtension(this->GetFilePath());
			MyFunc::ToLower(extend);

			const auto itr_find = m_load_lambda_table.find(extend);
			if (itr_find == m_load_lambda_table.end())
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_tex_desc.Clear();
				return;
			}

			hr = itr_find->second(
				AssetSystem::GetInstance().GetDevice(), nullptr,
				wfilename, m_force_srgb,
				m_resource.GetAddressOf(), m_SRV.GetAddressOf()
			);
		}

		// Finish loading
		{
			m_is_loading = false;
			m_is_loaded = true;
			if (FAILED(hr))
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
				m_tex_desc.Clear();
			}
			else
			{
				D3D11_RESOURCE_DIMENSION dim;
				m_resource->GetType(&dim);
				switch (dim)
				{
				case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
					GetDescTexture2D();
					break;
				case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
					GetDescTexture3D();
					break;
				default:
					assert(0 && "Invalid resource dimension");
				}

				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}

	void ResTexture::SetForceSRGB(bool force_srgb)
	{
		m_force_srgb = force_srgb;
	}

	bool ResTexture::GetForceSRGB() const
	{
		return m_force_srgb;
	}

	void ResTexture::GetDescTexture2D()
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2d;
		const auto hr = m_resource->QueryInterface(tex2d.GetAddressOf());
		if (FAILED(hr))
		{
			assert(0 && "failed ID3D11Resource::QueryInterface() ResTexture::GetDescTexture2D()");
			return;
		}
		D3D11_TEXTURE2D_DESC desc = {};
		tex2d->GetDesc(&desc);

		m_tex_desc.width = desc.Width;
		m_tex_desc.height = desc.Height;
		m_tex_desc.depth = 1;
		m_tex_desc.array_size = desc.ArraySize;
		m_tex_desc.mip_levels = desc.MipLevels;
		m_tex_desc.row_pitch;
		m_tex_desc.slice_pitch;
		m_tex_desc.format = desc.Format;
		m_tex_desc.option =
			(desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE) > 0 ? TEXTURE_OPTION::TEXTURE_OPTION_CUBEMAP : TEXTURE_OPTION::TEXTURE_OPTION_NONE;
		m_tex_desc.dimension = TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE2D;
	}

	void ResTexture::GetDescTexture3D()
	{
		Microsoft::WRL::ComPtr<ID3D11Texture3D> tex3d;
		const auto hr = m_resource->QueryInterface(tex3d.GetAddressOf());
		if (FAILED(hr))
		{
			assert(0 && "failed ID3D11Resource::QueryInterface() ResTexture::GetDescTexture3D()");
			return;
		}
		D3D11_TEXTURE3D_DESC desc = {};
		tex3d->GetDesc(&desc);

		m_tex_desc.width = desc.Width;
		m_tex_desc.height = desc.Height;
		m_tex_desc.depth = desc.Depth;
		m_tex_desc.array_size = 1;
		m_tex_desc.mip_levels = desc.MipLevels;
		m_tex_desc.row_pitch;
		m_tex_desc.slice_pitch;
		m_tex_desc.format = desc.Format;
		m_tex_desc.option = TEXTURE_OPTION::TEXTURE_OPTION_VOLUME;
		m_tex_desc.dimension = TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE3D;
	}

	bool ResTexture::CreateTexture(const TEX_DESC& desc, bool create_srv, bool create_uav, const void* p_data)
	{
		if (m_resource || m_SRV || m_UAV)
		{
			m_UAV.Reset();
			m_SRV.Reset();
			m_resource.Reset();
			m_tex_desc.Clear();
		}
		HRESULT hr = S_OK;

		m_tex_desc = desc;
		// 深度テクスチャの場合ならフォーマットを変える必要がある
		DXGI_FORMAT res_format;
		Graphics::ConvertDepthFormat(desc.format, m_tex_desc.format, res_format);

		// Create Resource.
		{
			bool is_success = false;
			switch (desc.dimension)
			{
			case TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE1D:
				is_success = CreateTexture1D(m_tex_desc, create_srv, create_uav, p_data, m_resource.GetAddressOf());
				break;

			case TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE2D:
				is_success = CreateTexture2D(m_tex_desc, create_srv, create_uav, p_data, m_resource.GetAddressOf());
				break;

			case TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE3D:
				is_success = CreateTexture3D(m_tex_desc, create_srv, create_uav, p_data, m_resource.GetAddressOf());
				break;
			}

			if (!is_success)
			{
				m_resource.Reset();
				return false;
			}
		}

		assert(desc.array_size != 0);
		// Create SRV.
		if (create_srv)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
			srvd.Format = res_format;
			switch (desc.dimension)
			{
			case TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE1D:
				if (desc.array_size == 1)
				{
					srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
					srvd.Texture1D.MostDetailedMip = 0;
					srvd.Texture1D.MipLevels = (desc.mip_levels == 0) ? -1 : desc.mip_levels;
				}
				else
				{
					srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
					srvd.Texture1DArray.ArraySize = desc.array_size;
					srvd.Texture1DArray.FirstArraySlice = 0;
					srvd.Texture1DArray.MostDetailedMip = 0;
					srvd.Texture1DArray.MipLevels = (desc.mip_levels == 0) ? -1 : desc.mip_levels;
				}
				break;

			case TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE2D:
				if (desc.array_size == 1)
				{
					srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					srvd.Texture2D.MostDetailedMip = 0;
					srvd.Texture2D.MipLevels = (desc.mip_levels == 0) ? -1 : desc.mip_levels;
				}
				else if (desc.array_size == 6 && desc.option == TEXTURE_OPTION::TEXTURE_OPTION_CUBEMAP)
				{
					srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
					srvd.TextureCube.MostDetailedMip = 0;
					srvd.TextureCube.MipLevels = (desc.mip_levels == 0) ? -1 : desc.mip_levels;
				}
				else
				{
					srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
					srvd.Texture2DArray.ArraySize = desc.array_size;
					srvd.Texture2DArray.FirstArraySlice = 0;
					srvd.Texture2DArray.MostDetailedMip = 0;
					srvd.Texture2DArray.MipLevels = (desc.mip_levels == 0) ? -1 : desc.mip_levels;
				}
				break;

			case TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE3D:
				if (desc.array_size == 1)
				{
					srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
					srvd.Texture3D.MostDetailedMip = 0;
					srvd.Texture3D.MipLevels = (desc.mip_levels == 0) ? -1 : desc.mip_levels;
				}
				else
				{
					// 3DTextureの配列は未対応
					// TODO : 3DTextureの配列を雲などに使用する場合、要対応
					assert(0);
				}
				break;
			}

			hr = AssetSystem::GetInstance().GetDevice()->CreateShaderResourceView(
				m_resource.Get(), &srvd, m_SRV.GetAddressOf()
			);
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateShaderResourceView() ResTexture::CreateTexture()");
				return false;
			}
		}
		// Create UAV.
		if (create_uav)
		{
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavd = {};
			uavd.Format = desc.format;

			switch (desc.dimension)
			{
			case TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE1D:
				if (desc.array_size == 1)
				{
					uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
					uavd.Texture1D.MipSlice = 0;
				}
				else
				{
					uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
					uavd.Texture1DArray.ArraySize = desc.array_size;
					uavd.Texture1DArray.FirstArraySlice = 0;
					uavd.Texture1DArray.MipSlice = 0;
				}
				break;

			case TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE2D:
				if (desc.array_size == 1)
				{
					uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
					uavd.Texture2D.MipSlice = 0;
				}
				else
				{
					uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
					uavd.Texture2DArray.ArraySize = desc.array_size;
					uavd.Texture2DArray.FirstArraySlice = 0;
					uavd.Texture2DArray.MipSlice = 0;
				}
				break;

			case TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE3D:
				uavd.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
				uavd.Texture3D.MipSlice = 0;
				uavd.Texture3D.FirstWSlice = 0;
				// TODO : 3Dテクスチャの設定はよくわからない
				uavd.Texture3D.WSize = desc.depth;
				break;
			}

			hr = AssetSystem::GetInstance().GetDevice()->CreateUnorderedAccessView(
				m_resource.Get(), &uavd, m_UAV.GetAddressOf()
			);
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device::CreateUnorderedAccessView() ResTexture::CreateTexture()");
				return false;
			}
		}

		return true;
	}

	bool ResTexture::CreateTexture1D(const TEX_DESC& desc, bool create_srv, bool create_uav, const void* p_data, ID3D11Resource** pp_resource)
	{
		D3D11_TEXTURE1D_DESC _desc = {};
		_desc.Format = desc.format;
		_desc.Width = desc.width;
		_desc.MipLevels = desc.mip_levels;
		_desc.ArraySize = desc.array_size;
		_desc.Usage = D3D11_USAGE_DEFAULT;
		_desc.BindFlags = 0;
		if (create_srv)
			_desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
		if (create_uav)
			_desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
		_desc.CPUAccessFlags = 0;
		_desc.MiscFlags = 0;
		//if (desc.mip_levels != 1)
		//	_desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		//if (desc.format == DXGI_FORMAT_UNKNOWN)
		//	_desc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		D3D11_SUBRESOURCE_DATA init_data = {};
		init_data.pSysMem = p_data;
		init_data.SysMemPitch = desc.row_pitch;
		init_data.SysMemSlicePitch = desc.slice_pitch;

		Microsoft::WRL::ComPtr<ID3D11Texture1D> texture1d;

		HRESULT hr = AssetSystem::GetInstance().GetDevice()->CreateTexture1D(
			&_desc,
			(p_data == nullptr) ? nullptr : &init_data,
			texture1d.GetAddressOf()
		);
		if (FAILED(hr))
		{
			assert(0 && "failed ID3D11Device::CreateTexture1D() ResTexture::CreateTexture1D()");
			return false;
		}

		hr = texture1d.Get()->QueryInterface(pp_resource);
		if (FAILED(hr))
		{
			assert(0 && "failed QueryInterface(ID3D11Texture1D) ResTexture::CreateTexture1D()");
			return false;
		}

		return true;
	}

	bool ResTexture::CreateTexture2D(const TEX_DESC& desc, bool create_srv, bool create_uav, const void* p_data, ID3D11Resource** pp_resource)
	{
		D3D11_TEXTURE2D_DESC _desc = {};
		_desc.Format = desc.format;
		_desc.Width = desc.width;
		_desc.Height = desc.height;
		_desc.MipLevels = desc.mip_levels;
		_desc.ArraySize = desc.array_size;
		_desc.SampleDesc.Count = 1;
		_desc.SampleDesc.Quality = 0;
		_desc.Usage = D3D11_USAGE_DEFAULT;
		_desc.BindFlags = 0;
		if (create_srv)
			_desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
		if (create_uav)
			_desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
		_desc.CPUAccessFlags = 0;
		_desc.MiscFlags = 0;
		if (desc.option == TEXTURE_OPTION::TEXTURE_OPTION_CUBEMAP && _desc.ArraySize == 6)
			_desc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
		// http://maverickproj.web.fc2.com/d3d11_27.html
		// https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_resource_misc_flag
		// generate mipsするにはsrvかつrtvである必要がある
		//if (desc.mip_levels != 1)
		//	_desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		
		// TODO : http://www.project-asura.com/program/d3d11/d3d11_007.html
		//if (create_uav)
		//{
		//	_desc.MiscFlags |= 
		//}

		D3D11_SUBRESOURCE_DATA init_data = {};
		init_data.pSysMem = p_data;
		init_data.SysMemPitch = desc.row_pitch;
		init_data.SysMemSlicePitch = desc.slice_pitch;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;

		HRESULT hr = AssetSystem::GetInstance().GetDevice()->CreateTexture2D(
			&_desc,
			(p_data == nullptr) ? nullptr : &init_data,
			texture2d.GetAddressOf()
		);
		if (FAILED(hr))
		{
			assert(0 && "failed ID3D11Device::CreateTexture2D() ResTexture::CreateTexture2D()");
			return false;
		}

		hr = texture2d.Get()->QueryInterface(pp_resource);
		if (FAILED(hr))
		{
			assert(0 && "failed QueryInterface(ID3D11Texture2D) ResTexture::CreateTexture2D()");
			return false;
		}

		return true;
	}

	bool ResTexture::CreateTexture3D(const TEX_DESC& desc, bool create_srv, bool create_uav, const void* p_data, ID3D11Resource** pp_resource)
	{
		D3D11_TEXTURE3D_DESC _desc = {};
		_desc.Format = desc.format;
		_desc.Width = desc.width;
		_desc.Height = desc.height;
		_desc.Depth = desc.depth;
		_desc.MipLevels = desc.mip_levels;
		_desc.Usage = D3D11_USAGE_DEFAULT;
		_desc.BindFlags = 0;// D3D11_BIND_RENDER_TARGET;
		if (create_srv)
			_desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
		if (create_uav)
			_desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
		_desc.CPUAccessFlags = 0;
		_desc.MiscFlags = 0;
		// generate mipsするにはsrvかつrtvである必要がある
		//if (desc.mip_levels != 1)
		//	_desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		// D3D11_RESOURCE_MISC_TEXTURECUBE (0), D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS (0), D3D11_RESOURCE_MISC_BUFFER_STRUCTURED はTexture3Dでは有効でない
		//if (desc.format == DXGI_FORMAT_UNKNOWN)
		//	_desc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		D3D11_SUBRESOURCE_DATA init_data = {};
		init_data.pSysMem = p_data;
		init_data.SysMemPitch = desc.row_pitch;
		init_data.SysMemSlicePitch = desc.slice_pitch;

		Microsoft::WRL::ComPtr<ID3D11Texture3D> texture3d;

		HRESULT hr = AssetSystem::GetInstance().GetDevice()->CreateTexture3D(
			&_desc,
			(p_data == nullptr) ? nullptr : &init_data,
			texture3d.GetAddressOf()
		);
		if (FAILED(hr))
		{
			assert(0 && "failed ID3D11Device::CreateTexture3D() ResTexture::CreateTexture3D()");
			return false;
		}

		hr = texture3d.Get()->QueryInterface(pp_resource);
		if (FAILED(hr))
		{
			assert(0 && "failed QueryInterface(ID3D11Texture3D) ResTexture::CreateTexture3D()");
			return false;
		}

		return true;
	}

	HRESULT ResTexture::CreateTextureFromImage(
		ID3D11Device* p_device,
		const DirectX::Image* p_image,
		const size_t image_count,
		const DirectX::TexMetadata& meta,
		const bool force_srgb,
		ID3D11Resource** pp_resource,
		ID3D11ShaderResourceView** pp_srv)
	{
		// Create TEX_DESC.
		TEX_DESC desc = {};
		{
			desc.width = meta.width;
			desc.height = meta.height;
			desc.depth = meta.depth;
			desc.array_size = meta.arraySize;
			desc.mip_levels = meta.mipLevels;
			desc.row_pitch = p_image->rowPitch;
			desc.slice_pitch = p_image->slicePitch;
			desc.format = meta.format;
			desc.option = TEXTURE_OPTION::TEXTURE_OPTION_NONE;
			if (meta.IsCubemap())
			{
				desc.option = TEXTURE_OPTION::TEXTURE_OPTION_CUBEMAP;
			}

			if (meta.dimension == DirectX::TEX_DIMENSION::TEX_DIMENSION_TEXTURE1D)
			{
				desc.dimension = TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE1D;
			}
			else if (meta.dimension == DirectX::TEX_DIMENSION::TEX_DIMENSION_TEXTURE2D)
			{
				desc.dimension = TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE2D;
			}
			else if (meta.dimension == DirectX::TEX_DIMENSION::TEX_DIMENSION_TEXTURE3D)
			{
				desc.dimension = TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE3D;
				desc.option = TEXTURE_OPTION::TEXTURE_OPTION_VOLUME;
			}
			else
			{
				return E_FAIL;
			}
		}
		// Create Resource.
		{
			bool is_success = false;
			switch (desc.dimension)
			{
			case TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE1D:
				is_success = CreateTexture1D(desc, true, false, p_image->pixels, pp_resource);
				break;

			case TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE2D:
				is_success = CreateTexture2D(desc, true, false, p_image->pixels, pp_resource);
				break;

			case TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE3D:
				is_success = CreateTexture3D(desc, true, false, p_image->pixels, pp_resource);
				break;
			}

			if (is_success == false)
			{
				return E_FAIL;
			}
		}
		// Create SRV.
		{
			//D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
			//srvd.Format = desc.format;
			//switch (desc.dimension)
			//{
			//case TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE1D:
			//	if (desc.array_size == 1)
			//	{
			//		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
			//		srvd.Texture1D.MostDetailedMip = 0;
			//		srvd.Texture1D.MipLevels = (desc.mip_levels == 0) ? -1 : desc.mip_levels;
			//	}
			//	else
			//	{
			//		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
			//		srvd.Texture1DArray.ArraySize = desc.array_size;
			//		srvd.Texture1DArray.FirstArraySlice = 0;
			//		srvd.Texture1DArray.MostDetailedMip = 0;
			//		srvd.Texture1DArray.MipLevels = (desc.mip_levels == 0) ? -1 : desc.mip_levels;
			//	}
			//	break;

			//case TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE2D:
			//	if (desc.array_size == 1)
			//	{
			//		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			//		srvd.Texture2D.MostDetailedMip = 0;
			//		srvd.Texture2D.MipLevels = (desc.mip_levels == 0) ? -1 : desc.mip_levels;
			//	}
			//	else if (desc.array_size == 6 && desc.option == TEXTURE_OPTION::TEXTURE_OPTION_CUBEMAP)
			//	{
			//		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			//		srvd.TextureCube.MostDetailedMip = 0;
			//		srvd.TextureCube.MipLevels = (desc.mip_levels == 0) ? -1 : desc.mip_levels;
			//	}
			//	else
			//	{
			//		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			//		srvd.Texture2DArray.ArraySize = desc.array_size;
			//		srvd.Texture2DArray.FirstArraySlice = 0;
			//		srvd.Texture2DArray.MostDetailedMip = 0;
			//		srvd.Texture2DArray.MipLevels = (desc.mip_levels == 0) ? -1 : desc.mip_levels;
			//	}
			//	break;

			//case TEXTURE_DIMENSION::TEXTURE_DIMENSION_TEXTURE3D:
			//	if (desc.array_size == 1)
			//	{
			//		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
			//		srvd.Texture3D.MostDetailedMip = 0;
			//		srvd.Texture3D.MipLevels = (desc.mip_levels == 0) ? -1 : desc.mip_levels;
			//	}
			//	else
			//	{
			//		return E_FAIL;
			//	}
			//	break;
			//}

			//HRESULT hr = AssetSystem::GetInstance().GetDevice()->CreateShaderResourceView(
			//	*pp_resource, &srvd, pp_srv
			//);
			const auto hr = DirectX::CreateShaderResourceViewEx
			(
				AssetSystem::GetInstance().GetDevice(),
				p_image,
				image_count,
				meta,
				D3D11_USAGE_DEFAULT,
				D3D11_BIND_SHADER_RESOURCE,
				0,
				D3D11_RESOURCE_MISC_TEXTURECUBE,
				force_srgb,
				pp_srv
			);

				if (FAILED(hr))
				{
					assert(0 && "failed ID3D11Device::CreateShaderResourceView() ResTexture::CreateTextureFromImage()");
					return E_FAIL;
				}
		}

		return S_OK;
	}

}	// namespace TKGEngine