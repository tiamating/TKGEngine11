#pragma once
#include "../../../Systems/inc/TKGEngine_Defined.h"

#include "Application/Resource/src/Material/IResMaterial.h"
#include "Application/Resource/inc/Material_Defined.h"
#include "Systems/inc/StateManager.h"
#include "VertexElement.h"

#include <string>

#include <memory>


struct ID3D11DeviceContext;

namespace TKGEngine
{
	class Material
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Material() = default;
		virtual ~Material() = default;
		Material(const Material&);
		void operator=(const Material&);

		static void RemoveUnused();

#ifdef USE_IMGUI
		// save時にtrue
		bool OnGUI(bool is_shadow_material) const;
#endif // USE_IMGUI

		void Create();	// リソースマネージャーにセットされない作成
		void Create(const std::string& name);
		void LoadAsync(const std::string& filename);
		void Load(const std::string& filename);

		void Release();

		bool Activate(ID3D11DeviceContext* p_context, bool write_depth, bool set_depth_ps) const;

		int GetRenderQueue() const;
		void SetRenderQueue(int queue) const;
		void UseTarget(bool use_target) const;
		bool IsUsedTarget() const;
		void SetStencilMask(Stencil::Mask mask) const;
		Stencil::Mask GetStencilMask() const;

		void SetTopology(PRIMITIVE_TOPOLOGY topology) const;
		void SetDepthState(StateManager::DS state) const;
		void SetRasterizerState(StateManager::RS state) const;
		void SetBlendState(StateManager::BS state) const;

		int GetInputSlotFromType(VERTEX_ELEMENT_TYPE type) const;
		int GetInputSlotFromType(int type) const;
		void SetVertexShader(const std::string& filepath) const;
		void SetPixelShader(const std::string& filepath) const;
		void SetDepthPixelShader(const std::string& filepath) const;
		void SetGeometryShader(const std::string& filepath) const;
		void SetDomainShader(const std::string& filepath) const;
		void SetHullShader(const std::string& filepath) const;
		void RemoveVertexShader() const;
		void RemovePixelShader() const;
		void RemoveDepthPixelShader() const;
		void RemoveGeometryShader() const;
		void RemoveDomainShader() const;
		void RemoveHullShader() const;

		void AddTexture(int slot, const std::string& filepath, bool force_srgb = false) const;
		void RemoveTexture() const;

		void CreateCBuffer() const;
		template<class T>
		void SetParam(const std::string& param_name, const T& value);
		template<class T>
		void GetParam(const std::string& param_name, T& value) const;

		void SetTextureOffset(const VECTOR2& offset) const;
		void SetTextureTilling(const VECTOR2& tilling) const;
		void SetTextureCutout(const float cutout) const;

		bool IsLoaded() const;
		bool HasMaterial() const;
		bool HasVertexShader() const;
		bool HasPixelShader() const;
		bool HasDepthPixelShader() const;
		bool HasGeometryShader() const;
		bool HasDomainShader() const;
		bool HasHullShader() const;
		bool HasTextures() const;
		const char* GetName() const;
		void SetName(const std::string& name) const;
		const char* GetFilePath() const;

		unsigned GetHash() const;

		static Material& GetDefaultMaterial();

		// ==============================================
		// public variables
		// ==============================================


	private:
		// ==============================================
		// private methods
		// ==============================================
		static void CreateDefaultMaterial();


		// ==============================================
		// private variables
		// ==============================================
		std::shared_ptr<IResMaterial> m_res_material = nullptr;

		static bool m_created_default_material;
		static Material m_default_material;
	};


	template<class T>
	inline void Material::SetParam(const std::string& param_name, const T& value)
	{
		m_res_material->SetParam(param_name, &value, sizeof(T));
	}

	template<class T>
	inline void Material::GetParam(const std::string& param_name, T& value) const
	{
		m_res_material->GetParam(param_name, &value, sizeof(T));
	}

}	// namespace TKGEngine