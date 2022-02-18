#pragma once

#include "Application/Resource/inc/AssetDataBase.h"

#include "Application/Resource/inc/Material_Defined.h"
#include "Application/Resource/inc/VertexElement.h"
#include "Application/Resource/src/ResourceManager.h"
#include "../ResourceManager.h"
#include "Systems/inc/StateManager.h"

#include <string>

#include <memory>
#include <mutex>

struct ID3D11DeviceContext;

namespace TKGEngine
{
	/// <summary>
	/// Material Resource interface
	/// </summary>
	class IResMaterial
		: public AssetDataBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		IResMaterial() = default;
		virtual ~IResMaterial() = default;
		IResMaterial(const IResMaterial&) = delete;
		IResMaterial& operator=(const IResMaterial&) = delete;

		static std::shared_ptr<IResMaterial> Create();
		static std::shared_ptr<IResMaterial> Create(const std::string& name);
		static std::shared_ptr<IResMaterial> LoadAsync(const std::string& filename);
		static std::shared_ptr<IResMaterial> Load(const std::string& filename);
		static void RemoveUnused();

#ifdef USE_IMGUI
		virtual void OnGUI(bool is_shadow_material) = 0;
		// セーブされたフレームのみtrue
		virtual bool IsSavedOnFrame() const = 0;
#endif // USE_IMGUI

		// AssetDataBase
		void Release() override;
		// ~AssetDataBase

		virtual bool Activate(ID3D11DeviceContext* p_context, bool write_depth, bool set_depth_ps) = 0;

		virtual int GetRenderQueue() const = 0;
		virtual void SetRenderQueue(int queue) = 0;
		virtual void UseTarget(bool use_target) = 0;
		virtual bool IsUsedTarget() const = 0;
		virtual void SetStencilMask(Stencil::Mask mask) = 0;
		virtual Stencil::Mask GetStencilMask() const = 0;

		virtual void SetTopology(PRIMITIVE_TOPOLOGY topology) = 0;
		virtual void SetDepthState(StateManager::DS state) = 0;
		virtual void SetRasterizerState(StateManager::RS state) = 0;
		virtual void SetBlendState(StateManager::BS state) = 0;

		// Texture
		virtual void AddTexture(int slot, const std::string& filepath, bool force_srgb = false) = 0;
		virtual void RemoveTexture() = 0;
		// ~Texture

		// Shader
		virtual int GetInputSlotFromType(VERTEX_ELEMENT_TYPE type) = 0;
		virtual void SetVertexShader(const std::string& filepath) = 0;
		virtual void SetPixelShader(const std::string& filepath) = 0;
		virtual void SetDepthPixelShader(const std::string& filepath) = 0;
		virtual void SetGeometryShader(const std::string& filepath) = 0;
		virtual void SetDomainShader(const std::string& filepath) = 0;
		virtual void SetHullShader(const std::string& filepath) = 0;
		virtual void RemoveVertexShader() = 0;
		virtual void RemovePixelShader() = 0;
		virtual void RemoveDepthPixelShader() = 0;
		virtual void RemoveGeometryShader() = 0;
		virtual void RemoveDomainShader() = 0;
		virtual void RemoveHullShader() = 0;
		// ~Shader

		virtual bool HasVertexShader() = 0;
		virtual bool HasPixelShader() = 0;
		virtual bool HasDepthPixelShader() = 0;
		virtual bool HasGeometryShader() = 0;
		virtual bool HasDomainShader() = 0;
		virtual bool HasHullShader() = 0;
		virtual bool HasTextures() const = 0;

		virtual void CreateCBMaterial(bool force_clear = true) = 0;
		virtual void SetParam(const std::string& param_name, const void* p_src, size_t size) = 0;
		virtual void GetParam(const std::string& param_name, void* p_dst, size_t size) const = 0;

		virtual void CreateCBTexture() = 0;
		virtual void SetTextureOffset(const VECTOR2& offset) = 0;
		virtual void SetTextureTilling(const VECTOR2& tilling) = 0;
		virtual void SetTextureCutout(const float cutout) = 0;


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */

	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					cereal::base_class<AssetDataBase>(this)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
		virtual void SetAsyncOnLoad() = 0;
		virtual void OnLoad() = 0;

		static IResMaterial* CreateInterface();

		// ==============================================
		// private variables
		// ==============================================
		static ResourceManager<IResMaterial> m_caches;
		static std::mutex m_cache_mutex;
	};

}// namespace TKGEngine

CEREAL_REGISTER_TYPE(TKGEngine::IResMaterial)
CEREAL_CLASS_VERSION(TKGEngine::IResMaterial, 1)
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::AssetDataBase, TKGEngine::IResMaterial)