#pragma once

#include "../../../Systems/inc/TKGEngine_Defined.h"
#include "VertexElement.h"

#include <string>
#include <memory>

struct ID3D11ShaderReflection;
struct ID3D11DeviceContext;

namespace TKGEngine
{
	class IResVS;
	class IResPS;
	class IResGS;
	class IResDS;
	class IResHS;

	class IResCS;


	/// <summary>
	/// Vertex Shader interface manage module
	/// </summary>
	class VertexShader
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		VertexShader() = default;
		virtual ~VertexShader() = default;
		VertexShader(const VertexShader&);
		VertexShader& operator=(const VertexShader&);

		static void RemoveUnused();

		void Compile(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile = false
		);
		void CompileAsync(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile = false
		);

		void Load(const std::string& filename);
		void LoadAsync(const std::string& filename);

		/// <summary>
		/// Resource.Reset()
		/// </summary>
		void Remove();
		/// <summary>
		/// Resource.eraseCache()
		/// </summary>
		void Release();

		/// <summary>
		/// Get vertex slot num from vertex_element type
		/// </summary>
		/// <param name="type">vertex element type</param>
		/// <returns>-1 : No Input Element , retval > 0 : Slot Num</returns>
		int GetSlotFromType(VERTEX_ELEMENT_TYPE type);
		ID3D11ShaderReflection* GetReflection() const;

		bool Activate(ID3D11DeviceContext* p_context);
		static void Deactivate(ID3D11DeviceContext* p_context);

		bool HasResource() const;
		const char* GetName() const;
		void SetName(const std::string& name);
		const char* GetFilePath() const;

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
		std::shared_ptr<IResVS> m_res_vs = nullptr;

	};

	/// <summary>
	/// Pixel Shader interface manage module
	/// </summary>
	class PixelShader
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		PixelShader() = default;
		virtual ~PixelShader() = default;
		PixelShader(const PixelShader&);
		PixelShader& operator=(const PixelShader&);

		static void RemoveUnused();

		void Compile(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile = false
		);
		void CompileAsync(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile = false
		);

		void Load(const std::string& filename);
		void LoadAsync(const std::string& filename);

		/// <summary>
		/// Resource.Reset()
		/// </summary>
		void Remove();
		/// <summary>
		/// Resource.eraseCache()
		/// </summary>
		void Release();

		ID3D11ShaderReflection* GetReflection() const;

		bool Activate(ID3D11DeviceContext* p_context);
		static void Deactivate(ID3D11DeviceContext* p_context);

		bool HasResource() const;
		const char* GetName() const;
		void SetName(const std::string& name);
		const char* GetFilePath() const;

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
		std::shared_ptr<IResPS> m_res_ps = nullptr;
	};

	/// <summary>
	/// Geometry Shader interface manage module
	/// </summary>
	class GeometryShader
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		GeometryShader() = default;
		virtual ~GeometryShader() = default;
		GeometryShader(const GeometryShader&);
		GeometryShader& operator=(const GeometryShader&);

		static void RemoveUnused();

		void Compile(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile = false
		);
		void CompileAsync(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile = false
		);

		void Load(const std::string& filename);
		void LoadAsync(const std::string& filename);

		/// <summary>
		/// Resource.Reset()
		/// </summary>
		void Remove();
		/// <summary>
		/// Resource.eraseCache()
		/// </summary>
		void Release();

		bool Activate(ID3D11DeviceContext* p_context);
		static void Deactivate(ID3D11DeviceContext* p_context);

		bool HasResource() const;
		const char* GetName() const;
		void SetName(const std::string& name);
		const char* GetFilePath() const;

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
		std::shared_ptr<IResGS> m_res_gs = nullptr;
	};

	/// <summary>
	/// Domain Shader interface manage module
	/// </summary>
	class DomainShader
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		DomainShader() = default;
		virtual ~DomainShader() = default;
		DomainShader(const DomainShader&);
		DomainShader& operator=(const DomainShader&);

		static void RemoveUnused();

		void Compile(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile = false
		);
		void CompileAsync(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile = false
		);

		void Load(const std::string& filename);
		void LoadAsync(const std::string& filename);

		/// <summary>
		/// Resource.Reset()
		/// </summary>
		void Remove();
		/// <summary>
		/// Resource.eraseCache()
		/// </summary>
		void Release();

		bool Activate(ID3D11DeviceContext* p_context);
		static void Deactivate(ID3D11DeviceContext* p_context);

		bool HasResource() const;
		const char* GetName() const;
		void SetName(const std::string& name);
		const char* GetFilePath() const;

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
		std::shared_ptr<IResDS> m_res_ds = nullptr;
	};

	/// <summary>
	/// Hull Shader interface manage module
	/// </summary>
	class HullShader
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		HullShader() = default;
		virtual ~HullShader() = default;
		HullShader(const HullShader&);
		HullShader& operator=(const HullShader&);

		static void RemoveUnused();

		void Compile(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile = false
		);
		void CompileAsync(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile = false
		);

		void Load(const std::string& filename);
		void LoadAsync(const std::string& filename);

		/// <summary>
		/// Resource.Reset()
		/// </summary>
		void Remove();
		/// <summary>
		/// Resource.eraseCache()
		/// </summary>
		void Release();

		bool Activate(ID3D11DeviceContext* p_context);
		static void Deactivate(ID3D11DeviceContext* p_context);

		bool HasResource() const;
		const char* GetName() const;
		void SetName(const std::string& name);
		const char* GetFilePath() const;

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
		std::shared_ptr<IResHS> m_res_hs = nullptr;
	};


	/// <summary>
	/// Compute Shader interface manage module
	/// </summary>
	class ComputeShader
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		ComputeShader() = default;
		virtual ~ComputeShader() = default;
		ComputeShader(const ComputeShader&);
		ComputeShader& operator=(const ComputeShader&);

		static void RemoveUnused();

		void Compile(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile = false
		);
		void CompileAsync(
			const std::string& filename,
			const std::string& entry_point,
			const std::string& shader_model,
			bool force_compile = false
		);

		void Load(const std::string& filename);
		void LoadAsync(const std::string& filename);

		/// <summary>
		/// Resource.Reset()
		/// </summary>
		void Remove();
		/// <summary>
		/// Resource.eraseCache()
		/// </summary>
		void Release();

		// TODO : DispatchとUAV,SRVのセットと外すことに対応する、シェーダ実行用クラスを作成する
		bool Activate(ID3D11DeviceContext* p_context);
		static void Deactivate(ID3D11DeviceContext* p_context);

		bool HasResource() const;
		const char* GetName() const;
		void SetName(const std::string& name);
		const char* GetFilePath() const;

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
		std::shared_ptr<IResCS> m_res_cs = nullptr;
	};

}// namespace TKGEngine

namespace TKGEngine
{
	/// <summary>
	/// Shader wrap module
	/// </summary>
	class Shader
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Shader() = default;
		virtual ~Shader() = default;
		Shader(const Shader&);
		Shader& operator=(const Shader&);

		/// <summary>
		/// シェーダを有効化する
		/// </summary>
		/// <returns>頂点シェーダーが存在しないときfalse</returns>
		bool Activate(ID3D11DeviceContext* p_context, bool use_depth_ps);
		static void Deactivate(ID3D11DeviceContext* p_context);

		VertexShader& VS();
		PixelShader& PS();
		GeometryShader& GS();
		DomainShader& DS();
		HullShader& HS();


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
		VertexShader m_vs;
		PixelShader m_ps;
		GeometryShader m_gs;
		DomainShader m_ds;
		HullShader m_hs;

	};

}// namespace TKGEngine