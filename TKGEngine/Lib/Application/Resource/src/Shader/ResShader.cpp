
#include "IResShader.h"

#include "../../../Lib/Systems/inc/AssetSystem.h"

#include "../../../../Utility/inc/myfunc_string.h"

#include <vector>
#include <unordered_map>
#include <memory>
#include <wrl.h>

#include <d3d11.h>

namespace TKGEngine
{
	/// <summary>
	/// Vertex Shader Resource module
	/// </summary>
	class ResVS
		: public IResVS
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		ResVS();
		ResVS(const std::string& entry_point, const std::string& shader_model);
		virtual ~ResVS() = default;
		ResVS(const ResVS&) = delete;
		ResVS& operator=(const ResVS&) = delete;

		// AssetDataBase
#ifdef USE_IMGUI
		void OnGUI() override {}
#endif // USE_IMGUI
		// ~AssetDataBase

		int GetSlotFromType(VERTEX_ELEMENT_TYPE type) override;
		ID3D11ShaderReflection* GetReflection() const override;

		void Activate(ID3D11DeviceContext* p_context) override;


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================
		void SetAsyncOnCompile() override;
		void OnCompile() override;
		void SetAsyncOnLoad() override;
		void OnLoad() override;

		bool CreateInputLayout() override;


		// ==============================================
		// private variables
		// ==============================================
		Microsoft::WRL::ComPtr<ID3DBlob> m_blob = nullptr;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VS = nullptr;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_IL = nullptr;
		Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_reflection = nullptr;

		std::vector<D3D11_INPUT_ELEMENT_DESC> m_input_elements;
		std::vector<int> m_vertex_types;
		std::vector<std::string> m_semantic_names;

		std::string m_entry_point;
		std::string m_shader_model;
	};

	/// <summary>
	/// Pixel Shader Resource module
	/// </summary>
	class ResPS
		: public IResPS
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		ResPS() = default;
		ResPS(const std::string& entry_point, const std::string& shader_model);
		virtual ~ResPS() = default;
		ResPS(const ResPS&) = delete;
		ResPS& operator=(const ResPS&) = delete;

		// AssetDataBase
#ifdef USE_IMGUI
		void OnGUI() override {}
#endif // USE_IMGUI
		// ~AssetDataBase

		ID3D11ShaderReflection* GetReflection() const override;

		void Activate(ID3D11DeviceContext* p_context) override;


		// ==============================================
		// public variables
		// ==============================================


	private:
		// ==============================================
		// private methods
		// ==============================================
		void SetAsyncOnCompile() override;
		void OnCompile() override;
		void SetAsyncOnLoad() override;
		void OnLoad() override;


		// ==============================================
		// private variables
		// ==============================================
		Microsoft::WRL::ComPtr<ID3DBlob> m_blob = nullptr;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PS = nullptr;
		Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_reflection = nullptr;

		std::string m_entry_point;
		std::string m_shader_model;
	};

	/// <summary>
	/// Geometry Shader Resource module
	/// </summary>
	class ResGS
		: public IResGS
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		ResGS() = default;
		ResGS(const std::string& entry_point, const std::string& shader_model);
		virtual ~ResGS() = default;
		ResGS(const ResGS&) = delete;
		ResGS& operator=(const ResGS&) = delete;

		// AssetDataBase
#ifdef USE_IMGUI
		void OnGUI() override {}
#endif // USE_IMGUI
		// ~AssetDataBase

		void Activate(ID3D11DeviceContext* p_context) override;


		// ==============================================
		// public variables
		// ==============================================


	private:
		// ==============================================
		// private methods
		// ==============================================
		void SetAsyncOnCompile() override;
		void OnCompile() override;
		void SetAsyncOnLoad() override;
		void OnLoad() override;


		// ==============================================
		// private variables
		// ==============================================
		Microsoft::WRL::ComPtr<ID3DBlob> m_blob = nullptr;
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_GS = nullptr;

		std::string m_entry_point;
		std::string m_shader_model;
	};

	/// <summary>
	/// Domain Shader Resource module
	/// </summary>
	class ResDS
		: public IResDS
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		ResDS() = default;
		ResDS(const std::string& entry_point, const std::string& shader_model);
		virtual ~ResDS() = default;
		ResDS(const ResDS&) = delete;
		ResDS& operator=(const ResDS&) = delete;

		// AssetDataBase
#ifdef USE_IMGUI
		void OnGUI() override {}
#endif // USE_IMGUI
		// ~AssetDataBase

		void Activate(ID3D11DeviceContext* p_context) override;


		// ==============================================
		// public variables
		// ==============================================


	private:
		// ==============================================
		// private methods
		// ==============================================
		void SetAsyncOnCompile() override;
		void OnCompile() override;
		void SetAsyncOnLoad() override;
		void OnLoad() override;


		// ==============================================
		// private variables
		// ==============================================
		Microsoft::WRL::ComPtr<ID3DBlob> m_blob = nullptr;
		Microsoft::WRL::ComPtr<ID3D11DomainShader> m_DS = nullptr;

		std::string m_entry_point;
		std::string m_shader_model;
	};

	/// <summary>
	/// Hull Shader Resource module
	/// </summary>
	class ResHS
		: public IResHS
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		ResHS() = default;
		ResHS(const std::string& entry_point, const std::string& shader_model);
		virtual ~ResHS() = default;
		ResHS(const ResHS&) = delete;
		ResHS& operator=(const ResHS&) = delete;

		// AssetDataBase
#ifdef USE_IMGUI
		void OnGUI() override {}
#endif // USE_IMGUI
		// ~AssetDataBase

		void Activate(ID3D11DeviceContext* p_context) override;


		// ==============================================
		// public variables
		// ==============================================


	private:
		// ==============================================
		// private methods
		// ==============================================
		void SetAsyncOnCompile() override;
		void OnCompile() override;
		void SetAsyncOnLoad() override;
		void OnLoad() override;


		// ==============================================
		// private variables
		// ==============================================
		Microsoft::WRL::ComPtr<ID3DBlob> m_blob = nullptr;
		Microsoft::WRL::ComPtr<ID3D11HullShader> m_HS = nullptr;

		std::string m_entry_point;
		std::string m_shader_model;
	};


	/// <summary>
	/// Compute Shader Resource module
	/// </summary>
	class ResCS
		: public IResCS
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		ResCS() = default;
		ResCS(const std::string& entry_point, const std::string& shader_model);
		virtual ~ResCS() = default;
		ResCS(const ResCS&) = delete;
		ResCS& operator=(const ResCS&) = delete;

		// AssetDataBase
#ifdef USE_IMGUI
		void OnGUI() override {}
#endif // USE_IMGUI
		// ~AssetDataBase

		void Activate(ID3D11DeviceContext* p_context) override;


		// ==============================================
		// public variables
		// ==============================================


	private:
		// ==============================================
		// private methods
		// ==============================================
		void SetAsyncOnCompile() override;
		void OnCompile() override;
		void SetAsyncOnLoad() override;
		void OnLoad() override;


		// ==============================================
		// private variables
		// ==============================================
		Microsoft::WRL::ComPtr<ID3DBlob> m_blob = nullptr;
		Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_CS = nullptr;

		std::string m_entry_point;
		std::string m_shader_model;
	};

	/// <summary>
	/// HLSLの#includeの処理
	///	https://www.cxybb.com/article/Pobber/51972027
	///	https://github.com/TheRealMJP/BakingLab/blob/master/SampleFramework11/v1.02/Graphics/ShaderCompilation.cpp#L131
	/// </summary>
	class FrameworkInclude : public ID3DInclude
	{
	public:
		FrameworkInclude(const std::wstring& wfilepath)
			:m_wfilepath(wfilepath) {}

	private:
		// ロードするファイルパス
		std::wstring m_wfilepath;
		// インクルードされるデータとディレクトリのmap
		std::unordered_map<LPCVOID, std::wstring> m_directory_index;

		HRESULT _stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override
		{
			// ファイル名
			std::filesystem::path fs_filename = pFileName;
			const std::wstring filename = fs_filename.filename().wstring();

			//// ファイルディレクトリ取得
			const auto itr_find = m_directory_index.find(pParentData);
			std::wstring filedir;
			// 始めのインクルードパスならファイルパスを使用
			if (itr_find == m_directory_index.end())
			{
				std::filesystem::path fs_filepath = m_wfilepath;
				std::filesystem::path fs_filedir = fs_filepath.remove_filename().make_preferred();
				filedir = fs_filedir.wstring();
			}
			// インクルードファイル内でインクルードされているなら階層を変更する
			else
			{
				// 親の階層を取得
				filedir = itr_find->second;

				//// includeパスから../の個数分階層を上にする
				// ../の形にする
				std::wstring wpath = fs_filename.make_preferred().wstring();
				while(true)
				{
					constexpr size_t parent_dir_char_size = std::size(L"..\\") - 1;
					constexpr size_t split_dir_char_size = std::size(L"\\") - 1;

					// "..\\"が見つからなければbreak
					const auto pos = wpath.find(L"..\\");
					if(pos == std::wstring::npos)
					{
						break;
					}
					// "..\\"を削除
					wpath.erase(pos, parent_dir_char_size);
					// ディレクトリを1つ上の階層に移動
					filedir.erase(filedir.size() - split_dir_char_size, split_dir_char_size);
					const auto split_pos = filedir.find_last_of(L"\\");
					if(split_pos != std::wstring::npos)
					{
						filedir.erase(split_pos + split_dir_char_size);
					}
				}

				// filenameだけ残してディレクトリ部分はfiledirに合成する
				std::filesystem::path processed_filepath = wpath;
				filedir += processed_filepath.remove_filename();
			}

			//// ファイルパスの取得
			std::wstring filepath;
			// "xxx"
			if (IncludeType == D3D_INCLUDE_LOCAL)
			{
				filepath = filedir + filename;
			}
			// <xxx>
			else if (IncludeType == D3D_INCLUDE_SYSTEM)
			{
				return E_FAIL;
				constexpr DWORD buf_size = 255;
				wchar_t buf[buf_size];
				GetCurrentDirectory(buf_size, buf);
				filepath = std::wstring(buf) + filedir + filename;
			}
			else
			{
				return E_FAIL;
			}
			//// ファイルを開いてデータを取得
			std::ifstream ifstr(filepath.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
			// ファイルオープン成功か
			if (!ifstr.is_open())
			{
				return E_FAIL;
			}
			// データサイズ取得
			const auto filesize = static_cast<UINT>(ifstr.tellg());
			*pBytes = filesize;
			ifstr.seekg(0, std::ios::beg);
			// ファイルデータを取得
			char* file_data = new char[filesize];
			ifstr.read(file_data, filesize);
			*ppData = file_data;

			// データアドレスとパスを紐づける
			m_directory_index.emplace(file_data, filedir);

			return S_OK;
		}

		HRESULT _stdcall Close(LPCVOID pData) override
		{
			// mapから削除
			m_directory_index.erase(pData);
			// メモリ解放
			delete[] static_cast<char*>(const_cast<void*>(pData));
			return S_OK;
		}
	};

	////////////////////////////////////////////////////////
	// Static variable declaration
	////////////////////////////////////////////////////////
	// IResVS
	ResourceManager<IResVS> IResVS::m_caches;
	std::mutex IResVS::m_cache_mutex;
	// ~IResVS

	// IResPS
	ResourceManager<IResPS> IResPS::m_caches;
	std::mutex IResPS::m_cache_mutex;
	// ~IResPS

	// IResGS
	ResourceManager<IResGS> IResGS::m_caches;
	std::mutex IResGS::m_cache_mutex;
	// ~IResGS

	// IResDS
	ResourceManager<IResDS> IResDS::m_caches;
	std::mutex IResDS::m_cache_mutex;
	// ~IResDS

	// IResHS
	ResourceManager<IResHS> IResHS::m_caches;
	std::mutex IResHS::m_cache_mutex;
	// ~IResHS

	// IResCS
	ResourceManager<IResCS> IResCS::m_caches;
	std::mutex IResCS::m_cache_mutex;
	// ~IResCS

	// ResVS
	namespace /* anonymous */
	{
		constexpr char g_str_TEXCOORD[] = "TEXCOORD";
	}	// /* anonymous */
	// ~ResVS

	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
#pragma region IResVS
	IResVS* IResVS::CreateInterface()
	{
		return new ResVS();
	}
	IResVS* IResVS::CreateInterface(const std::string& entry_point, const std::string& shader_model)
	{
		return new ResVS(entry_point, shader_model);
	}

	std::shared_ptr<IResVS> IResVS::Compile(
		const std::string& filename,
		const std::string& entry_point,
		const std::string& shader_model,
		bool force_compile
	)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResVS> res_find = m_caches.Search(filename);
		std::shared_ptr<IResVS> res_new;
		if (res_find)
		{
			if (force_compile)
			{
				// すでに登録されているものを削除する
				m_caches.RemoveCache(filename);
			}
			else
			{
				m_cache_mutex.unlock();
				return res_find;
			}
		}
		// 新しいシェーダの実体の作成
		res_new.reset(IResVS::CreateInterface(entry_point, shader_model));

		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Set async loader
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		res_new->OnCompile();

		return res_new;
	}

	std::shared_ptr<IResVS> IResVS::CompileAsync(
		const std::string& filename,
		const std::string& entry_point,
		const std::string& shader_model,
		bool force_compile
	)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResVS> res_find = m_caches.Search(filename);
		std::shared_ptr<IResVS> res_new;
		if (res_find)
		{
			if (force_compile)
			{
				// すでに登録されているものを削除する
				m_caches.RemoveCache(filename);
			}
			else
			{
				m_cache_mutex.unlock();
				return res_find;
			}
		}
		// 新しいシェーダの実体の作成
		res_new.reset(IResVS::CreateInterface(entry_point, shader_model));

		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Set async loader
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		AssetSystem::GetInstance().Add(std::bind(&IResVS::SetAsyncOnCompile, res_new));

		return res_new;
	}

	std::shared_ptr<IResVS> IResVS::Load(const std::string& filename)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResVS> res_find = m_caches.Search(filename);
		if (res_find)
		{
			m_cache_mutex.unlock();
			return res_find;
		}
		std::shared_ptr<IResVS> res_new(IResVS::CreateInterface());
		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Set sync loader
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		res_new->OnLoad();
		if (!res_new->HasResource())
		{
			res_new->Release();
			res_new->m_load_state_mutex.unlock();
			res_new.reset();
			return std::shared_ptr<IResVS>();
		}

		return res_new;
	}

	std::shared_ptr<IResVS> IResVS::LoadAsync(const std::string& filename)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResVS> res_find = m_caches.Search(filename);
		if (res_find)
		{
			m_cache_mutex.unlock();
			return res_find;
		}
		std::shared_ptr<IResVS> res_new(IResVS::CreateInterface());
		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Set async loader
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		AssetSystem::GetInstance().Add(std::bind(&IResVS::SetAsyncOnLoad, res_new));

		return res_new;
	}

	void IResVS::RemoveUnused()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveUnusedCache();
	}

	void IResVS::Release()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveCache(this->GetFilePath());
	}
#pragma endregion

#pragma region IResPS
	IResPS* IResPS::CreateInterface()
	{
		return new ResPS();
	}
	IResPS* IResPS::CreateInterface(const std::string& entry_point, const std::string& shader_model)
	{
		return new ResPS(entry_point, shader_model);
	}

	std::shared_ptr<IResPS> IResPS::Compile(
		const std::string& filename,
		const std::string& entry_point,
		const std::string& shader_model,
		bool force_compile
	)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResPS> res_find = m_caches.Search(filename);
		std::shared_ptr<IResPS> res_new;
		if (res_find)
		{
			if (force_compile)
			{
				// すでに登録されているものを削除する
				m_caches.RemoveCache(filename);
			}
			else
			{
				m_cache_mutex.unlock();
				return res_find;
			}
		}
		// 新しいシェーダの実体の作成
		res_new.reset(IResPS::CreateInterface(entry_point, shader_model));

		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Compile
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		res_new->OnCompile();

		return res_new;
	}

	std::shared_ptr<IResPS> IResPS::CompileAsync(
		const std::string& filename,
		const std::string& entry_point,
		const std::string& shader_model,
		bool force_compile
	)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResPS> res_find = m_caches.Search(filename);
		std::shared_ptr<IResPS> res_new;
		if (res_find)
		{
			if (force_compile)
			{
				// すでに登録されているものを削除する
				m_caches.RemoveCache(filename);
			}
			else
			{
				m_cache_mutex.unlock();
				return res_find;
			}
		}
		// 新しいシェーダの実体の作成
		res_new.reset(IResPS::CreateInterface(entry_point, shader_model));

		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Set async loader
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		AssetSystem::GetInstance().Add(std::bind(&IResPS::SetAsyncOnCompile, res_new));

		return res_new;
	}

	std::shared_ptr<IResPS> IResPS::Load(const std::string& filename)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResPS> res_find = m_caches.Search(filename);
		if (res_find)
		{
			m_cache_mutex.unlock();
			return res_find;
		}
		std::shared_ptr<IResPS> res_new(IResPS::CreateInterface());
		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Load
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		res_new->OnLoad();
		if (!res_new->HasResource())
		{
			res_new->Release();
			res_new->m_load_state_mutex.unlock();
			res_new.reset();
			return std::shared_ptr<IResPS>();
		}

		return res_new;
	}

	std::shared_ptr<IResPS> IResPS::LoadAsync(const std::string& filename)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResPS> res_find = m_caches.Search(filename);
		if (res_find)
		{
			m_cache_mutex.unlock();
			return res_find;
		}
		std::shared_ptr<IResPS> res_new(IResPS::CreateInterface());
		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Set async loader
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		AssetSystem::GetInstance().Add(std::bind(&IResPS::SetAsyncOnLoad, res_new));

		return res_new;
	}

	void IResPS::RemoveUnused()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveUnusedCache();
	}

	void IResPS::Release()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveCache(this->GetFilePath());
	}
#pragma endregion

#pragma region IResGS
	IResGS* IResGS::CreateInterface()
	{
		return new ResGS();
	}
	IResGS* IResGS::CreateInterface(const std::string& entry_point, const std::string& shader_model)
	{
		return new ResGS(entry_point, shader_model);
	}

	std::shared_ptr<IResGS> IResGS::Compile(
		const std::string& filename,
		const std::string& entry_point,
		const std::string& shader_model,
		bool force_compile
	)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResGS> res_find = m_caches.Search(filename);
		std::shared_ptr<IResGS> res_new;
		if (res_find)
		{
			if (force_compile)
			{
				// すでに登録されているものを削除する
				m_caches.RemoveCache(filename);
			}
			else
			{
				m_cache_mutex.unlock();
				return res_find;
			}
		}
		// 新しいシェーダの実体の作成
		res_new.reset(IResGS::CreateInterface(entry_point, shader_model));

		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Compile
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		res_new->OnCompile();

		return res_new;
	}

	std::shared_ptr<IResGS> IResGS::CompileAsync(
		const std::string& filename,
		const std::string& entry_point,
		const std::string& shader_model,
		bool force_compile
	)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResGS> res_find = m_caches.Search(filename);
		std::shared_ptr<IResGS> res_new;
		if (res_find)
		{
			if (force_compile)
			{
				// すでに登録されているものを削除する
				m_caches.RemoveCache(filename);
			}
			else
			{
				m_cache_mutex.unlock();
				return res_find;
			}
		}
		// 新しいシェーダの実体の作成
		res_new.reset(IResGS::CreateInterface(entry_point, shader_model));

		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Set async loader
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		AssetSystem::GetInstance().Add(std::bind(&IResGS::SetAsyncOnCompile, res_new));

		return res_new;
	}

	std::shared_ptr<IResGS> IResGS::Load(const std::string& filename)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResGS> res_find = m_caches.Search(filename);
		if (res_find)
		{
			m_cache_mutex.unlock();
			return res_find;
		}
		std::shared_ptr<IResGS> res_new(IResGS::CreateInterface());
		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Load
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		res_new->OnLoad();
		if (!res_new->HasResource())
		{
			res_new->Release();
			res_new->m_load_state_mutex.unlock();
			res_new.reset();
			return std::shared_ptr<IResGS>();
		}

		return res_new;
	}

	std::shared_ptr<IResGS> IResGS::LoadAsync(const std::string& filename)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResGS> res_find = m_caches.Search(filename);
		if (res_find)
		{
			m_cache_mutex.unlock();
			return res_find;
		}
		std::shared_ptr<IResGS> res_new(IResGS::CreateInterface());
		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Set async loader
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		AssetSystem::GetInstance().Add(std::bind(&IResGS::SetAsyncOnLoad, res_new));

		return res_new;
	}

	void IResGS::RemoveUnused()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveUnusedCache();
	}

	void IResGS::Release()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveCache(this->GetFilePath());
	}
#pragma endregion

#pragma region IResDS
	IResDS* IResDS::CreateInterface()
	{
		return new ResDS();
	}
	IResDS* IResDS::CreateInterface(const std::string& entry_point, const std::string& shader_model)
	{
		return new ResDS(entry_point, shader_model);
	}

	std::shared_ptr<IResDS> IResDS::Compile(
		const std::string& filename,
		const std::string& entry_point,
		const std::string& shader_model,
		bool force_compile
	)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResDS> res_find = m_caches.Search(filename);
		std::shared_ptr<IResDS> res_new;
		if (res_find)
		{
			if (force_compile)
			{
				// すでに登録されているものを削除する
				m_caches.RemoveCache(filename);
			}
			else
			{
				m_cache_mutex.unlock();
				return res_find;
			}
		}
		// 新しいシェーダの実体の作成
		res_new.reset(IResDS::CreateInterface(entry_point, shader_model));

		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Compile
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		res_new->OnCompile();

		return res_new;
	}

	std::shared_ptr<IResDS> IResDS::CompileAsync(
		const std::string& filename,
		const std::string& entry_point,
		const std::string& shader_model,
		bool force_compile
	)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResDS> res_find = m_caches.Search(filename);
		std::shared_ptr<IResDS> res_new;
		if (res_find)
		{
			if (force_compile)
			{
				// すでに登録されているものを削除する
				m_caches.RemoveCache(filename);
			}
			else
			{
				m_cache_mutex.unlock();
				return res_find;
			}
		}
		// 新しいシェーダの実体の作成
		res_new.reset(IResDS::CreateInterface(entry_point, shader_model));

		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Set async loader
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		AssetSystem::GetInstance().Add(std::bind(&IResDS::SetAsyncOnCompile, res_new));

		return res_new;
	}

	std::shared_ptr<IResDS> IResDS::Load(const std::string& filename)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResDS> res_find = m_caches.Search(filename);
		if (res_find != nullptr)
		{
			m_cache_mutex.unlock();
			return res_find;
		}
		std::shared_ptr<IResDS> res_new(IResDS::CreateInterface());
		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Load
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		res_new->OnLoad();
		if (!res_new->HasResource())
		{
			res_new->Release();
			res_new->m_load_state_mutex.unlock();
			res_new.reset();
			return std::shared_ptr<IResDS>();
		}

		return res_new;
	}

	std::shared_ptr<IResDS> IResDS::LoadAsync(const std::string& filename)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResDS> res_find = m_caches.Search(filename);
		if (res_find != nullptr)
		{
			m_cache_mutex.unlock();
			return res_find;
		}
		std::shared_ptr<IResDS> res_new(IResDS::CreateInterface());
		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Set async loader
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		AssetSystem::GetInstance().Add(std::bind(&IResDS::SetAsyncOnLoad, res_new));

		return res_new;
	}

	void IResDS::RemoveUnused()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveUnusedCache();
	}

	void IResDS::Release()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveCache(this->GetFilePath());
	}
#pragma endregion

#pragma region IResHS
	IResHS* IResHS::CreateInterface()
	{
		return new ResHS();
	}
	IResHS* IResHS::CreateInterface(const std::string& entry_point, const std::string& shader_model)
	{
		return new ResHS(entry_point, shader_model);
	}

	std::shared_ptr<IResHS> IResHS::Compile(
		const std::string& filename,
		const std::string& entry_point,
		const std::string& shader_model,
		bool force_compile
	)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResHS> res_find = m_caches.Search(filename);
		std::shared_ptr<IResHS> res_new;
		if (res_find)
		{
			if (force_compile)
			{
				// すでに登録されているものを削除する
				m_caches.RemoveCache(filename);
			}
			else
			{
				m_cache_mutex.unlock();
				return res_find;
			}
		}
		// 新しいシェーダの実体の作成
		res_new.reset(IResHS::CreateInterface(entry_point, shader_model));

		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Compile
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		res_new->OnCompile();

		return res_new;
	}

	std::shared_ptr<IResHS> IResHS::CompileAsync(
		const std::string& filename,
		const std::string& entry_point,
		const std::string& shader_model,
		bool force_compile
	)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResHS> res_find = m_caches.Search(filename);
		std::shared_ptr<IResHS> res_new;
		if (res_find)
		{
			if (force_compile)
			{
				// すでに登録されているものを削除する
				m_caches.RemoveCache(filename);
			}
			else
			{
				m_cache_mutex.unlock();
				return res_find;
			}
		}
		// 新しいシェーダの実体の作成
		res_new.reset(IResHS::CreateInterface(entry_point, shader_model));

		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Set async loader
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		AssetSystem::GetInstance().Add(std::bind(&IResHS::SetAsyncOnCompile, res_new));

		return res_new;
	}

	std::shared_ptr<IResHS> IResHS::Load(const std::string& filename)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResHS> res_find = m_caches.Search(filename);
		if (res_find != nullptr)
		{
			m_cache_mutex.unlock();
			return res_find;
		}
		std::shared_ptr<IResHS> res_new(IResHS::CreateInterface());
		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Load
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		res_new->OnLoad();
		if (!res_new->HasResource())
		{
			res_new->Release();
			res_new->m_load_state_mutex.unlock();
			res_new.reset();
			return std::shared_ptr<IResHS>();
		}

		return res_new;
	}

	std::shared_ptr<IResHS> IResHS::LoadAsync(const std::string& filename)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResHS> res_find = m_caches.Search(filename);
		if (res_find != nullptr)
		{
			m_cache_mutex.unlock();
			return res_find;
		}
		std::shared_ptr<IResHS> res_new(IResHS::CreateInterface());
		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Set async loader
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		AssetSystem::GetInstance().Add(std::bind(&IResHS::SetAsyncOnLoad, res_new));

		return res_new;
	}

	void IResHS::RemoveUnused()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveUnusedCache();
	}

	void IResHS::Release()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveCache(this->GetFilePath());
	}
#pragma endregion


#pragma region IResCS
	IResCS* IResCS::CreateInterface()
	{
		return new ResCS();
	}
	IResCS* IResCS::CreateInterface(const std::string& entry_point, const std::string& shader_model)
	{
		return new ResCS(entry_point, shader_model);
	}

	std::shared_ptr<IResCS> IResCS::Compile(
		const std::string& filename,
		const std::string& entry_point,
		const std::string& shader_model,
		bool force_compile
	)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResCS> res_find = m_caches.Search(filename);
		std::shared_ptr<IResCS> res_new;
		if (res_find)
		{
			if (force_compile)
			{
				// すでに登録されているものを削除する
				m_caches.RemoveCache(filename);
			}
			else
			{
				m_cache_mutex.unlock();
				return res_find;
			}
		}
		// 新しいシェーダの実体の作成
		res_new.reset(IResCS::CreateInterface(entry_point, shader_model));

		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Compile
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		res_new->OnCompile();

		return res_new;
	}

	std::shared_ptr<IResCS> IResCS::CompileAsync(
		const std::string& filename,
		const std::string& entry_point,
		const std::string& shader_model,
		bool force_compile
	)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResCS> res_find = m_caches.Search(filename);
		std::shared_ptr<IResCS> res_new;
		if (res_find)
		{
			if (force_compile)
			{
				// すでに登録されているものを削除する
				m_caches.RemoveCache(filename);
			}
			else
			{
				m_cache_mutex.unlock();
				return res_find;
			}
		}
		// 新しいシェーダの実体の作成
		res_new.reset(IResCS::CreateInterface(entry_point, shader_model));

		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Set async loader
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		AssetSystem::GetInstance().Add(std::bind(&IResCS::SetAsyncOnCompile, res_new));

		return res_new;
	}

	std::shared_ptr<IResCS> IResCS::Load(const std::string& filename)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResCS> res_find = m_caches.Search(filename);
		if (res_find != nullptr)
		{
			m_cache_mutex.unlock();
			return res_find;
		}
		std::shared_ptr<IResCS> res_new(IResCS::CreateInterface());
		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Load
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		res_new->OnLoad();
		if (!res_new->HasResource())
		{
			res_new->Release();
			res_new->m_load_state_mutex.unlock();
			res_new.reset();
			return std::shared_ptr<IResCS>();
		}

		return res_new;
	}

	std::shared_ptr<IResCS> IResCS::LoadAsync(const std::string& filename)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResCS> res_find = m_caches.Search(filename);
		if (res_find != nullptr)
		{
			m_cache_mutex.unlock();
			return res_find;
		}
		std::shared_ptr<IResCS> res_new(IResCS::CreateInterface());
		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Set async loader
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		AssetSystem::GetInstance().Add(std::bind(&IResCS::SetAsyncOnLoad, res_new));

		return res_new;
	}

	void IResCS::RemoveUnused()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveUnusedCache();
	}

	void IResCS::Release()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveCache(this->GetFilePath());
	}
#pragma endregion


#pragma region ResVS
	ResVS::ResVS()
	{
		m_vertex_types.resize(static_cast<int>(VERTEX_ELEMENT_TYPE::MAX_TYPE_NUM), -1);
	}
	ResVS::ResVS(const std::string& entry_point, const std::string& shader_model)
		: m_entry_point(entry_point), m_shader_model(shader_model)
	{
		m_vertex_types.resize(static_cast<int>(VERTEX_ELEMENT_TYPE::MAX_TYPE_NUM), -1);
	}

	inline int ResVS::GetSlotFromType(VERTEX_ELEMENT_TYPE type)
	{
		return m_vertex_types.at(static_cast<int>(type));
	}

	ID3D11ShaderReflection* ResVS::GetReflection() const
	{
		if (m_reflection)
		{
			return m_reflection.Get();
		}
		return nullptr;
	}

	void ResVS::Activate(ID3D11DeviceContext* p_context)
	{
		assert(p_context != nullptr);

		p_context->VSSetShader(m_VS.Get(), nullptr, 0);
		p_context->IASetInputLayout(m_IL.Get());
	}

	bool ResVS::CreateInputLayout()
	{
		HRESULT hr = S_OK;

		D3D11_SHADER_DESC shader_desc = {};
		hr = m_reflection->GetDesc(&shader_desc);
		if (FAILED(hr))
		{
			return false;
		}
		// 前までのデータを初期化
		for (auto&& i : m_vertex_types)
		{
			i = -1;
		}
		m_input_elements.clear();
		m_semantic_names.clear();

		const int num_input_params = shader_desc.InputParameters;
		int slot_offset = 0;
		bool find_instance_slot = false;
		int instance_slot = -1;
		// Instance type checker
		//bool has_instance_color = false;
		bool has_texture_transform = false;

		for (int i = 0; i < num_input_params; ++i)
		{
			bool is_instance_data = false;
			D3D11_SIGNATURE_PARAMETER_DESC param_desc = {};
			m_reflection->GetInputParameterDesc(i, &param_desc);

			D3D11_INPUT_ELEMENT_DESC input_element_desc = {};
			std::string semantic_name = param_desc.SemanticName;
			input_element_desc.SemanticName = param_desc.SemanticName;
			input_element_desc.SemanticIndex = param_desc.SemanticIndex;
			// MatrixとInstanceデータの場合スロットがずれるので例外処理する
			input_element_desc.InputSlot = i - slot_offset;
			input_element_desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

			// Get vertex type name
			{
				if (semantic_name == g_str_TEXCOORD)
				{
					semantic_name += std::to_string(input_element_desc.SemanticIndex);
				}

				int idx = 0;
				bool find = false;
				while (true)
				{
					if (idx >= static_cast<int>(VERTEX_ELEMENT_TYPE::MAX_TYPE_NUM))
					{
						break;
					}

					if (VERTEX_ELEMENT::SEMANTIC_NAME[idx] == semantic_name)
					{
						find = true;
						break;
					}
					++idx;
				}
				// VERTEX_ELEMENT_TYPEから見つからないものは登録せずにとばす
				if (find == false)
				{
					++slot_offset;
					continue;
				}

				if (idx <= static_cast<int>(VERTEX_ELEMENT_TYPE::TEXCOORD7))
				{
					// Vertex data
					input_element_desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
					input_element_desc.InstanceDataStepRate = 0;
				}
				else
				{
					// Instance data
					input_element_desc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
					input_element_desc.InstanceDataStepRate = 1;
					is_instance_data = true;

					if (find_instance_slot == true)
					{
						input_element_desc.InputSlot = instance_slot;
						++slot_offset;
					}
					else
					{
						find_instance_slot = true;
						instance_slot = input_element_desc.InputSlot;
					}

					switch (static_cast<VERTEX_ELEMENT_TYPE>(idx))
					{
						//case VERTEX_ELEMENT_TYPE::INSTANCE_COLOR:
						//	has_instance_color = true;
						//	break;
						case VERTEX_ELEMENT_TYPE::TEXCOORD_TRANSFORM:
							has_texture_transform = true;
							break;
					}
				}

				if (is_instance_data == false)
				{
					m_vertex_types[/*static_cast<VERTEX_ELEMENT_TYPE>(idx)*/idx] = input_element_desc.InputSlot;
				}
			}

			// get vertex buffer format
			{
				if (param_desc.Mask == 1)// x
				{
					switch (param_desc.ComponentType)
					{
						case D3D_REGISTER_COMPONENT_UINT32:
							input_element_desc.Format = DXGI_FORMAT_R32_UINT;
							break;

						case D3D_REGISTER_COMPONENT_SINT32:
							input_element_desc.Format = DXGI_FORMAT_R32_SINT;
							break;

						case D3D_REGISTER_COMPONENT_FLOAT32:
							input_element_desc.Format = DXGI_FORMAT_R32_FLOAT;
							break;

						case D3D_REGISTER_COMPONENT_UNKNOWN:
						default:
							input_element_desc.Format = DXGI_FORMAT_UNKNOWN;
							break;
					}
				}
				else if (param_desc.Mask <= 3)// xy
				{
					switch (param_desc.ComponentType)
					{
						case D3D_REGISTER_COMPONENT_UINT32:
							input_element_desc.Format = DXGI_FORMAT_R32G32_UINT;
							break;

						case D3D_REGISTER_COMPONENT_SINT32:
							input_element_desc.Format = DXGI_FORMAT_R32G32_SINT;
							break;

						case D3D_REGISTER_COMPONENT_FLOAT32:
							input_element_desc.Format = DXGI_FORMAT_R32G32_FLOAT;
							break;

						case D3D_REGISTER_COMPONENT_UNKNOWN:
						default:
							input_element_desc.Format = DXGI_FORMAT_UNKNOWN;
							break;
					}
				}
				else if (param_desc.Mask <= 7)// xyz
				{
					switch (param_desc.ComponentType)
					{
						case D3D_REGISTER_COMPONENT_UINT32:
							input_element_desc.Format = DXGI_FORMAT_R32G32B32_UINT;
							break;

						case D3D_REGISTER_COMPONENT_SINT32:
							input_element_desc.Format = DXGI_FORMAT_R32G32B32_SINT;
							break;

						case D3D_REGISTER_COMPONENT_FLOAT32:
							input_element_desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
							break;

						case D3D_REGISTER_COMPONENT_UNKNOWN:
						default:
							input_element_desc.Format = DXGI_FORMAT_UNKNOWN;
							break;
					}
				}
				else if (param_desc.Mask <= 15)// xyzw
				{
					switch (param_desc.ComponentType)
					{
						case D3D_REGISTER_COMPONENT_UINT32:
							input_element_desc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
							break;

						case D3D_REGISTER_COMPONENT_SINT32:
							input_element_desc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
							break;

						case D3D_REGISTER_COMPONENT_FLOAT32:
							if (semantic_name == "POSITION" ||
								semantic_name == "NORMAL" ||
								semantic_name == "TANGENT" ||
								semantic_name == "BINORMAL")
							{
								input_element_desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
							}
							else
							{
								input_element_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
							}
							break;

						case D3D_REGISTER_COMPONENT_UNKNOWN:
						default:
							input_element_desc.Format = DXGI_FORMAT_UNKNOWN;
							break;
					}
				}
			}

			// set input element desc
			m_semantic_names.emplace_back(semantic_name);
			m_input_elements.emplace_back(input_element_desc);
		}

		// Check Instance type
		if (find_instance_slot == true)
		{
			//if (has_instance_color == true)
			{
				if (has_texture_transform == true)
				{
					// UIInstance
					m_vertex_types[static_cast<int>(VERTEX_ELEMENT_TYPE::UI_INSTANCE)] = instance_slot;
				}
				else
				{
					// MainInstance
					m_vertex_types[static_cast<int>(VERTEX_ELEMENT_TYPE::MAIN_INSTANCE)] = instance_slot;
				}
			}
			//else
			//{
			//	// DepthInstance
			//	m_vertex_types[static_cast<int>(VERTEX_ELEMENT_TYPE::DEPTH_INSTANCE)] = instance_slot;
			//}
		}
		const auto elem_size = m_input_elements.size();
		if (elem_size > 0)
		{
			hr = AssetSystem::GetInstance().GetDevice()->CreateInputLayout(
				m_input_elements.data(),
				elem_size,
				m_blob->GetBufferPointer(),
				m_blob->GetBufferSize(),
				m_IL.GetAddressOf()
			);
			if (FAILED(hr))
			{
				return false;
			}
		}

		return true;
	}

	void ResVS::SetAsyncOnCompile()
	{
		// Start compileing
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Compile vertex shader
		HRESULT hr = S_OK;
		{
			UINT compile_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
			// デバッグ用かつ最適化なし
			compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			// load shader
			FrameworkInclude inc_obj(wfilename);
			hr = D3DCompileFromFile(
				wfilename.c_str(),
				nullptr,
				&inc_obj,
				m_entry_point.c_str(),
				m_shader_model.c_str(),
				compile_flags,
				0,
				m_blob.GetAddressOf(),
				nullptr
			);

			// Create shader and reflection
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreateVertexShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_VS.GetAddressOf()
				);

				if (SUCCEEDED(hr))
				{
					hr = D3DReflect(
						m_blob->GetBufferPointer(),
						m_blob->GetBufferSize(),
						IID_PPV_ARGS(m_reflection.GetAddressOf())
					);

					// Create Input Layout
					if (SUCCEEDED(hr))
					{
						if (CreateInputLayout() == false)
						{
							hr = E_FAIL;
						}
					}
				}
			}
		}

		// Finish compileing
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_is_loading = false;
			m_is_loaded = true;
			if (FAILED(hr))
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}

	void ResVS::OnCompile()
	{
		// Start compileing
		{
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Compile vertex shader
		HRESULT hr = S_OK;
		{
			UINT compile_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
			// デバッグ用かつ最適化なし
			compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			// load shader
			FrameworkInclude inc_obj(wfilename);
			hr = D3DCompileFromFile(
				wfilename.c_str(),
				nullptr,
				&inc_obj,
				m_entry_point.c_str(),
				m_shader_model.c_str(),
				compile_flags,
				0,
				m_blob.GetAddressOf(),
				nullptr
			);

			// Create shader and reflection
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreateVertexShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_VS.GetAddressOf()
				);

				if (SUCCEEDED(hr))
				{
					hr = D3DReflect(
						m_blob->GetBufferPointer(),
						m_blob->GetBufferSize(),
						IID_PPV_ARGS(m_reflection.GetAddressOf())
					);

					// Create Input Layout
					if (SUCCEEDED(hr))
					{
						if (CreateInputLayout() == false)
						{
							hr = E_FAIL;
						}
					}
				}
			}
		}

		// Finish compileing
		{
			m_is_loading = false;
			m_is_loaded = true;
			if (FAILED(hr))
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}

	void ResVS::SetAsyncOnLoad()
	{
		// Start loading
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Load vertex shader
		HRESULT hr = S_OK;
		{
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			hr = D3DReadFileToBlob(wfilename.c_str(), m_blob.GetAddressOf());

			// Create shader and reflection
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreateVertexShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_VS.GetAddressOf()
				);

				if (SUCCEEDED(hr))
				{
					hr = D3DReflect(
						m_blob->GetBufferPointer(),
						m_blob->GetBufferSize(),
						IID_PPV_ARGS(m_reflection.GetAddressOf())
					);

					if (SUCCEEDED(hr))
					{
						// Create input layout
						if (SUCCEEDED(hr))
						{
							if (CreateInputLayout() == false)
							{
								hr = E_FAIL;
							}
						}
					}
				}
			}
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
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}
	void ResVS::OnLoad()
	{
		// Start loading
		{
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Load vertex shader
		HRESULT hr = S_OK;
		{
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			hr = D3DReadFileToBlob(wfilename.c_str(), m_blob.GetAddressOf());

			// Create shader and reflection
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreateVertexShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_VS.GetAddressOf()
				);

				if (SUCCEEDED(hr))
				{
					hr = D3DReflect(
						m_blob->GetBufferPointer(),
						m_blob->GetBufferSize(),
						IID_PPV_ARGS(m_reflection.GetAddressOf())
					);

					if (SUCCEEDED(hr))
					{
						// Create input layout
						if (SUCCEEDED(hr))
						{
							if (!CreateInputLayout())
							{
								hr = E_FAIL;
							}
						}
					}
				}
			}
		}

		// Finish loading
		{
			m_is_loading = false;
			m_is_loaded = true;
			if (FAILED(hr))
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}
#pragma endregion

#pragma region ResPS
	ResPS::ResPS(const std::string& entry_point, const std::string& shader_model)
		: m_entry_point(entry_point), m_shader_model(shader_model)
	{
		/* nothing */
	}

	ID3D11ShaderReflection* ResPS::GetReflection() const
	{
		if (m_reflection)
		{
			return m_reflection.Get();
		}
		return nullptr;
	}

	void ResPS::Activate(ID3D11DeviceContext* p_context)
	{
		assert(p_context);

		p_context->PSSetShader(m_PS.Get(), nullptr, 0);
	}

	void ResPS::SetAsyncOnCompile()
	{
		// Start compileing
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Compile pixel shader
		HRESULT hr = S_OK;
		{
			UINT compile_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
			// デバッグ用かつ最適化なし
			compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			// load shader
			FrameworkInclude inc_obj(wfilename);
			hr = D3DCompileFromFile(
				wfilename.c_str(),
				nullptr,
				&inc_obj,
				m_entry_point.c_str(),
				m_shader_model.c_str(),
				compile_flags,
				0,
				m_blob.GetAddressOf(),
				nullptr
			);

			// Create shader and reflection
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreatePixelShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_PS.GetAddressOf()
				);

				if (SUCCEEDED(hr))
				{
					hr = D3DReflect(
						m_blob->GetBufferPointer(),
						m_blob->GetBufferSize(),
						IID_PPV_ARGS(m_reflection.GetAddressOf())
					);
				}
			}
		}

		// Finish compileing
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_is_loading = false;
			m_is_loaded = true;
			if (FAILED(hr))
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}

	void ResPS::OnCompile()
	{
		// Start compileing
		{
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Compile pixel shader
		HRESULT hr = S_OK;
		{
			UINT compile_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
			// デバッグ用かつ最適化なし
			compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			// load shader
			FrameworkInclude inc_obj(wfilename);
			hr = D3DCompileFromFile(
				wfilename.c_str(),
				nullptr,
				&inc_obj,
				m_entry_point.c_str(),
				m_shader_model.c_str(),
				compile_flags,
				0,
				m_blob.GetAddressOf(),
				nullptr
			);

			// Create shader and reflection
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreatePixelShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_PS.GetAddressOf()
				);

				if (SUCCEEDED(hr))
				{
					hr = D3DReflect(
						m_blob->GetBufferPointer(),
						m_blob->GetBufferSize(),
						IID_PPV_ARGS(m_reflection.GetAddressOf())
					);
				}
			}
		}

		// Finish compileing
		{
			m_is_loading = false;
			m_is_loaded = true;
			if (FAILED(hr))
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}

	void ResPS::SetAsyncOnLoad()
	{
		// Start loading
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Load pixel shader
		HRESULT hr = S_OK;
		{
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			hr = D3DReadFileToBlob(wfilename.c_str(), m_blob.GetAddressOf());

			// Create shader and reflection
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreatePixelShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_PS.GetAddressOf()
				);

				if (SUCCEEDED(hr))
				{
					hr = D3DReflect(
						m_blob->GetBufferPointer(),
						m_blob->GetBufferSize(),
						IID_PPV_ARGS(m_reflection.GetAddressOf())
					);
				}
			}
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
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}
	void ResPS::OnLoad()
	{
		// Start loading
		{
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Load pixel shader
		HRESULT hr = S_OK;
		{
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			hr = D3DReadFileToBlob(wfilename.c_str(), m_blob.GetAddressOf());

			// Create shader and reflection
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreatePixelShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_PS.GetAddressOf()
				);

				if (SUCCEEDED(hr))
				{
					hr = D3DReflect(
						m_blob->GetBufferPointer(),
						m_blob->GetBufferSize(),
						IID_PPV_ARGS(m_reflection.GetAddressOf())
					);
				}
			}
		}

		// Finish loading
		{
			m_is_loading = false;
			m_is_loaded = true;
			if (FAILED(hr))
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}
#pragma endregion

#pragma region ResGS
	ResGS::ResGS(const std::string& entry_point, const std::string& shader_model)
		: m_entry_point(entry_point), m_shader_model(shader_model)
	{
		/* nothing */
	}

	void ResGS::Activate(ID3D11DeviceContext* p_context)
	{
		assert(p_context != nullptr);

		p_context->GSSetShader(m_GS.Get(), nullptr, 0);
	}

	void ResGS::SetAsyncOnCompile()
	{
		// Start compileing
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Compile geometry shader
		HRESULT hr = S_OK;
		{
			UINT compile_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
			// デバッグ用かつ最適化なし
			compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			// load shader
			FrameworkInclude inc_obj(wfilename);
			hr = D3DCompileFromFile(
				wfilename.c_str(),
				nullptr,
				&inc_obj,
				m_entry_point.c_str(),
				m_shader_model.c_str(),
				compile_flags,
				0,
				m_blob.GetAddressOf(),
				nullptr
			);

			// Create shader
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreateGeometryShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_GS.GetAddressOf()
				);
			}
		}

		// Finish compileing
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_is_loading = false;
			m_is_loaded = true;
			if (FAILED(hr))
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}

	void ResGS::OnCompile()
	{
		// Start compileing
		{
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Compile geometry shader
		HRESULT hr = S_OK;
		{
			UINT compile_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
			// デバッグ用かつ最適化なし
			compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			// load shader
			FrameworkInclude inc_obj(wfilename);
			hr = D3DCompileFromFile(
				wfilename.c_str(),
				nullptr,
				&inc_obj,
				m_entry_point.c_str(),
				m_shader_model.c_str(),
				compile_flags,
				0,
				m_blob.GetAddressOf(),
				nullptr
			);

			// Create shader
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreateGeometryShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_GS.GetAddressOf()
				);
			}
		}

		// Finish compileing
		{
			m_is_loading = false;
			m_is_loaded = true;
			if (FAILED(hr))
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}

	void ResGS::SetAsyncOnLoad()
	{
		// Start loading
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Load geometry shader
		HRESULT hr = S_OK;
		{
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			hr = D3DReadFileToBlob(wfilename.c_str(), m_blob.GetAddressOf());

			// Create shader
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreateGeometryShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_GS.GetAddressOf()
				);
			}
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
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}
	void ResGS::OnLoad()
	{
		// Start loading
		{
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Load geometry shader
		HRESULT hr = S_OK;
		{
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			hr = D3DReadFileToBlob(wfilename.c_str(), m_blob.GetAddressOf());

			// Create shader
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreateGeometryShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_GS.GetAddressOf()
				);
			}
		}

		// Finish loading
		{
			m_is_loading = false;
			m_is_loaded = true;
			if (FAILED(hr))
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}
#pragma endregion

#pragma region ResDS
	ResDS::ResDS(const std::string& entry_point, const std::string& shader_model)
		: m_entry_point(entry_point), m_shader_model(shader_model)
	{
		/* nothing */
	}

	void ResDS::Activate(ID3D11DeviceContext* p_context)
	{
		assert(p_context != nullptr);

		p_context->DSSetShader(m_DS.Get(), nullptr, 0);
	}

	void ResDS::SetAsyncOnCompile()
	{
		// Start compileing
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Compile domain shader
		HRESULT hr = S_OK;
		{
			UINT compile_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
			// デバッグ用かつ最適化なし
			compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			// load shader
			FrameworkInclude inc_obj(wfilename);
			hr = D3DCompileFromFile(
				wfilename.c_str(),
				nullptr,
				&inc_obj,
				m_entry_point.c_str(),
				m_shader_model.c_str(),
				compile_flags,
				0,
				m_blob.GetAddressOf(),
				nullptr
			);

			// Create shader
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreateDomainShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_DS.GetAddressOf()
				);
			}
		}

		// Finish compileing
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_is_loading = false;
			m_is_loaded = true;
			if (FAILED(hr))
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}

	void ResDS::OnCompile()
	{
		// Start compileing
		{
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Compile domain shader
		HRESULT hr = S_OK;
		{
			UINT compile_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
			// デバッグ用かつ最適化なし
			compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			// load shader
			FrameworkInclude inc_obj(wfilename);
			hr = D3DCompileFromFile(
				wfilename.c_str(),
				nullptr,
				&inc_obj,
				m_entry_point.c_str(),
				m_shader_model.c_str(),
				compile_flags,
				0,
				m_blob.GetAddressOf(),
				nullptr
			);

			// Create shader
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreateDomainShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_DS.GetAddressOf()
				);
			}
		}

		// Finish compileing
		{
			m_is_loading = false;
			m_is_loaded = true;
			if (FAILED(hr))
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}

	void ResDS::SetAsyncOnLoad()
	{
		// Start loading
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Load domain shader
		HRESULT hr = S_OK;
		{
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			hr = D3DReadFileToBlob(wfilename.c_str(), m_blob.GetAddressOf());

			// Create shader
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreateDomainShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_DS.GetAddressOf()
				);
			}
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
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}
	void ResDS::OnLoad()
	{
		// Start loading
		{
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Load domain shader
		HRESULT hr = S_OK;
		{
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			hr = D3DReadFileToBlob(wfilename.c_str(), m_blob.GetAddressOf());

			// Create shader
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreateDomainShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_DS.GetAddressOf()
				);
			}
		}

		// Finish loading
		{
			m_is_loading = false;
			m_is_loaded = true;
			if (FAILED(hr))
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}
#pragma endregion

#pragma region ResHS
	ResHS::ResHS(const std::string& entry_point, const std::string& shader_model)
		: m_entry_point(entry_point), m_shader_model(shader_model)
	{
		/* nothing */
	}

	void ResHS::Activate(ID3D11DeviceContext* p_context)
	{
		assert(p_context != nullptr);

		p_context->HSSetShader(m_HS.Get(), nullptr, 0);
	}

	void ResHS::SetAsyncOnCompile()
	{
		// Start compileing
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Compile hull shader
		HRESULT hr = S_OK;
		{
			UINT compile_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
			// デバッグ用かつ最適化なし
			compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			// load shader
			FrameworkInclude inc_obj(wfilename);
			hr = D3DCompileFromFile(
				wfilename.c_str(),
				nullptr,
				&inc_obj,
				m_entry_point.c_str(),
				m_shader_model.c_str(),
				compile_flags,
				0,
				m_blob.GetAddressOf(),
				nullptr
			);

			// Create shader
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreateHullShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_HS.GetAddressOf()
				);
			}
		}

		// Finish compileing
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_is_loading = false;
			m_is_loaded = true;
			if (FAILED(hr))
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}

	void ResHS::OnCompile()
	{
		// Start compileing
		{
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Compile hull shader
		HRESULT hr = S_OK;
		{
			UINT compile_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
			// デバッグ用かつ最適化なし
			compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			// load shader
			FrameworkInclude inc_obj(wfilename);
			hr = D3DCompileFromFile(
				wfilename.c_str(),
				nullptr,
				&inc_obj,
				m_entry_point.c_str(),
				m_shader_model.c_str(),
				compile_flags,
				0,
				m_blob.GetAddressOf(),
				nullptr
			);

			// Create shader
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreateHullShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_HS.GetAddressOf()
				);
			}
		}

		// Finish compileing
		{
			m_is_loading = false;
			m_is_loaded = true;
			if (FAILED(hr))
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}

	void ResHS::SetAsyncOnLoad()
	{
		// Start loading
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Load hull shader
		HRESULT hr = S_OK;
		{
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			hr = D3DReadFileToBlob(wfilename.c_str(), m_blob.GetAddressOf());

			// Create shader
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreateHullShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_HS.GetAddressOf()
				);
			}
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
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}
	void ResHS::OnLoad()
	{
		// Start loading
		{
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Load hull shader
		HRESULT hr = S_OK;
		{
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			hr = D3DReadFileToBlob(wfilename.c_str(), m_blob.GetAddressOf());

			// Create shader
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreateHullShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_HS.GetAddressOf()
				);
			}
		}

		// Finish loading
		{
			m_is_loading = false;
			m_is_loaded = true;
			if (FAILED(hr))
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}
#pragma endregion


#pragma region ResCS
	ResCS::ResCS(const std::string& entry_point, const std::string& shader_model)
		: m_entry_point(entry_point), m_shader_model(shader_model)
	{
		/* nothing */
	}

	void ResCS::Activate(ID3D11DeviceContext* p_context)
	{
		assert(p_context != nullptr);

		p_context->CSSetShader(m_CS.Get(), nullptr, 0);
	}

	void ResCS::SetAsyncOnCompile()
	{
		// Start compileing
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Compile hull shader
		HRESULT hr = S_OK;
		{
			UINT compile_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
			// デバッグ用かつ最適化なし
			compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			// load shader
			FrameworkInclude inc_obj(wfilename);
			hr = D3DCompileFromFile(
				wfilename.c_str(),
				nullptr,
				&inc_obj,
				m_entry_point.c_str(),
				m_shader_model.c_str(),
				compile_flags,
				0,
				m_blob.GetAddressOf(),
				nullptr
			);

			// Create shader
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreateComputeShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_CS.GetAddressOf()
				);
			}
		}

		// Finish compileing
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_is_loading = false;
			m_is_loaded = true;
			if (FAILED(hr))
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}

	void ResCS::OnCompile()
	{
		// Start compileing
		{
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Compile hull shader
		HRESULT hr = S_OK;
		{
			UINT compile_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
			// デバッグ用かつ最適化なし
			compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			// load shader
			FrameworkInclude inc_obj(wfilename);
			hr = D3DCompileFromFile(
				wfilename.c_str(),
				nullptr,
				&inc_obj,
				m_entry_point.c_str(),
				m_shader_model.c_str(),
				compile_flags,
				0,
				m_blob.GetAddressOf(),
				nullptr
			);

			// Create shader
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreateComputeShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_CS.GetAddressOf()
				);
			}
		}

		// Finish compileing
		{
			m_is_loading = false;
			m_is_loaded = true;
			if (FAILED(hr))
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}

	void ResCS::SetAsyncOnLoad()
	{
		// Start loading
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Load hull shader
		HRESULT hr = S_OK;
		{
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			hr = D3DReadFileToBlob(wfilename.c_str(), m_blob.GetAddressOf());

			// Create shader
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreateComputeShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_CS.GetAddressOf()
				);
			}
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
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}
	void ResCS::OnLoad()
	{
		// Start loading
		{
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}

		// Load hull shader
		HRESULT hr = S_OK;
		{
			std::wstring wfilename = MyFunc::ConvertStringToWstring(this->GetFilePath());
			hr = D3DReadFileToBlob(wfilename.c_str(), m_blob.GetAddressOf());

			// Create shader
			if (SUCCEEDED(hr))
			{
				hr = AssetSystem::GetInstance().GetDevice()->CreateComputeShader(
					m_blob->GetBufferPointer(),
					m_blob->GetBufferSize(),
					nullptr,
					m_CS.GetAddressOf()
				);
			}
		}

		// Finish loading
		{
			m_is_loading = false;
			m_is_loaded = true;
			if (FAILED(hr))
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
			else
			{
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;
			}
		}
	}
#pragma endregion

}	// namespace TKGEngine