#pragma once

#include "../../inc/AssetDataBase.h"
#include "../ResourceManager.h"
#include "../../inc/VertexElement.h"

#include <string>
#include <mutex>

#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

struct ID3D11DeviceContext;

namespace TKGEngine
{
	/// <summary>
	/// Vertex Shader Resource interface
	/// </summary>
	class IResVS
		: public AssetDataBase
	{
	public:
		IResVS() = default;
		virtual ~IResVS() = default;
		IResVS(const IResVS&) = delete;
		IResVS& operator=(const IResVS&) = delete;

		static std::shared_ptr<IResVS> Compile(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile
		);
		static std::shared_ptr<IResVS> CompileAsync(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile
		);
		static std::shared_ptr<IResVS> Load(const std::string& filename);
		static std::shared_ptr<IResVS> LoadAsync(const std::string& filename);

		static void RemoveUnused();

		// AssetDataBase
		void Release() override;
		// ~AssetDataBase

		virtual int GetSlotFromType(VERTEX_ELEMENT_TYPE type) = 0;
		virtual ID3D11ShaderReflection* GetReflection() const = 0;

		virtual void Activate(ID3D11DeviceContext* p_context) = 0;

	private:
		virtual void SetAsyncOnCompile() = 0;
		virtual void OnCompile() = 0;
		virtual void SetAsyncOnLoad() = 0;
		virtual void OnLoad() = 0;

		virtual bool CreateInputLayout() = 0;

		static IResVS* CreateInterface();
		static IResVS* CreateInterface(const std::string& entry_point, const std::string& shader_model);
		static ResourceManager<IResVS> m_caches;
		static std::mutex m_cache_mutex;
	};

	/// <summary>
	/// Pixel Shader Resource interface
	/// </summary>
	class IResPS
		: public AssetDataBase
	{
	public:
		IResPS() = default;
		virtual ~IResPS() = default;
		IResPS(const IResPS&) = delete;
		IResPS& operator=(const IResPS&) = delete;

		static std::shared_ptr<IResPS> Compile(
			const std::string & filename,
			const std::string & entry_point,
			const std::string & shader_model,
			bool force_compile
		);
		static std::shared_ptr<IResPS> CompileAsync(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile
		);
		static std::shared_ptr<IResPS> Load(const std::string & filename);
		static std::shared_ptr<IResPS> LoadAsync(const std::string& filename);

		static void RemoveUnused();

		// AssetDataBase
		void Release() override;
		// ~AssetDataBase

		virtual ID3D11ShaderReflection* GetReflection() const = 0;

		virtual void Activate(ID3D11DeviceContext* p_context) = 0;

	private:
		virtual void SetAsyncOnCompile() = 0;
		virtual void OnCompile() = 0;
		virtual void SetAsyncOnLoad() = 0;
		virtual void OnLoad() = 0;

		static IResPS* CreateInterface();
		static IResPS* CreateInterface(const std::string& entry_point, const std::string& shader_model);
		static ResourceManager<IResPS> m_caches;
		static std::mutex m_cache_mutex;
	};

	/// <summary>
	/// Geometry Shader Resource interface
	/// </summary>
	class IResGS
		: public AssetDataBase
	{
	public:
		IResGS() = default;
		virtual ~IResGS() = default;
		IResGS(const IResGS&) = delete;
		IResGS& operator=(const IResGS&) = delete;

		static std::shared_ptr<IResGS> Compile(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile
		);
		static std::shared_ptr<IResGS> CompileAsync(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile
		);
		static std::shared_ptr<IResGS> Load(const std::string& filename);
		static std::shared_ptr<IResGS> LoadAsync(const std::string& filename);

		static void RemoveUnused();

		// AssetDataBase
		void Release() override;
		// ~AssetDataBase

		virtual void Activate(ID3D11DeviceContext* p_context) = 0;

	private:
		virtual void SetAsyncOnCompile() = 0;
		virtual void OnCompile() = 0;
		virtual void SetAsyncOnLoad() = 0;
		virtual void OnLoad() = 0;

		static IResGS* CreateInterface();
		static IResGS* CreateInterface(const std::string& entry_point, const std::string& shader_model);
		static ResourceManager<IResGS> m_caches;
		static std::mutex m_cache_mutex;
	};

	/// <summary>
	/// Domain Shader Resource interface
	/// </summary>
	class IResDS
		: public AssetDataBase
	{
	public:
		IResDS() = default;
		virtual ~IResDS() = default;
		IResDS(const IResDS&) = delete;
		IResDS& operator=(const IResDS&) = delete;

		static std::shared_ptr<IResDS> Compile(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile
		);
		static std::shared_ptr<IResDS> CompileAsync(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile
		);
		static std::shared_ptr<IResDS> Load(const std::string& filename);
		static std::shared_ptr<IResDS> LoadAsync(const std::string& filename);

		static void RemoveUnused();

		// AssetDataBase
		void Release() override;
		// ~AssetDataBase

		virtual void Activate(ID3D11DeviceContext* p_context) = 0;

	private:
		virtual void SetAsyncOnCompile() = 0;
		virtual void OnCompile() = 0;
		virtual void SetAsyncOnLoad() = 0;
		virtual void OnLoad() = 0;

		static IResDS* CreateInterface();
		static IResDS* CreateInterface(const std::string& entry_point, const std::string& shader_model);
		static ResourceManager<IResDS> m_caches;
		static std::mutex m_cache_mutex;
	};

	/// <summary>
	/// Hull Shader Resource interface
	/// </summary>
	class IResHS
		: public AssetDataBase
	{
	public:
		IResHS() = default;
		virtual ~IResHS() = default;
		IResHS(const IResHS&) = delete;
		IResHS& operator=(const IResHS&) = delete;

		static std::shared_ptr<IResHS> Compile(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile
		);
		static std::shared_ptr<IResHS> CompileAsync(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile
		);
		static std::shared_ptr<IResHS> Load(const std::string& filename);
		static std::shared_ptr<IResHS> LoadAsync(const std::string& filename);

		static void RemoveUnused();

		// AssetDataBase
		void Release() override;
		// ~AssetDataBase

		virtual void Activate(ID3D11DeviceContext* p_context) = 0;

	private:
		virtual void SetAsyncOnCompile() = 0;
		virtual void OnCompile() = 0;
		virtual void SetAsyncOnLoad() = 0;
		virtual void OnLoad() = 0;

		static IResHS* CreateInterface();
		static IResHS* CreateInterface(const std::string& entry_point, const std::string& shader_model);
		static ResourceManager<IResHS> m_caches;
		static std::mutex m_cache_mutex;
	};


	/// <summary>
	/// Compute Shader Resource interface
	/// </summary>
	class IResCS
		: public AssetDataBase
	{
	public:
		IResCS() = default;
		virtual ~IResCS() = default;
		IResCS(const IResCS&) = delete;
		IResCS& operator=(const IResCS&) = delete;

		static std::shared_ptr<IResCS> Compile(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile
		);
		static std::shared_ptr<IResCS> CompileAsync(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile
		);
		static std::shared_ptr<IResCS> Load(const std::string& filename);
		static std::shared_ptr<IResCS> LoadAsync(const std::string& filename);

		static void RemoveUnused();

		// AssetDataBase
		void Release() override;
		// ~AssetDataBase

		virtual void Activate(ID3D11DeviceContext* p_context) = 0;

	private:
		virtual void SetAsyncOnCompile() = 0;
		virtual void OnCompile() = 0;
		virtual void SetAsyncOnLoad() = 0;
		virtual void OnLoad() = 0;

		static IResCS* CreateInterface();
		static IResCS* CreateInterface(const std::string& entry_point, const std::string& shader_model);
		static ResourceManager<IResCS> m_caches;
		static std::mutex m_cache_mutex;
	};

}	// namespace TKGEngine