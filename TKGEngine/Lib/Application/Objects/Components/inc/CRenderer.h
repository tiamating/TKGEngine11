#pragma once

#include "../Component.h"
#include "../interface/IRenderer.h"
#include "Components/inc/CTransform.h"

#include "Utility/inc/bounds.h"
#include "Utility/inc/myfunc_vector.h"
#include "Utility/inc/myfunc_string.h"
#include "Application/Resource/inc/Material.h"
#include "Application/Resource/inc/Mesh.h"
#include "Application/Resource/inc/Shader_Defined.h"


namespace TKGEngine
{
	/// <summary>
	/// Rendering module Base.
	/// </summary>
	class Renderer
		: public Component, public IRenderer
	{
	public:
		enum class RendererType
		{
			Mesh = 0,
			UI,
			// Can't Batching
			Skin,
			Particle
			// ~Can't Batching
		};

		// ==============================================
		// public methods
		// ==============================================
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		Renderer();
		Renderer(RendererType type);
		virtual ~Renderer();

		// Main
		Material* GetMaterial(int index);
		void AddMaterial(int index, const std::string& filepath);
		void AddMaterial(int index, const Material& material);
		Mesh* GetMesh();
		virtual void AddMesh(const std::string& filepath, bool set_global = true);
		virtual void AddMesh(const Mesh& mesh, bool set_global = true);
		// Shadow
		Material* GetShadowMaterial(int index);
		void AddShadowMaterial(int index, const std::string& filepath);
		void AddShadowMaterial(int index, const Material& material);
		Mesh* GetShadowMesh();
		virtual void AddShadowMesh(const std::string& filepath, bool set_global = true);
		virtual void AddShadowMesh(const Mesh& mesh, bool set_global = true);

		[[nodiscard]] inline bool Enabled() const;
		inline void Enabled(bool enable);

		[[nodiscard]] inline bool IsVisible() const;
		inline void SetShadowCastMode(ShadowCastingMode mode);
		inline void SetInstanceColor(const VECTOR4& color);

		virtual inline const Bounds& GetRendererBounds() const override;

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	protected:
		// ==============================================
		// protected methods
		// ==============================================
#ifdef USE_IMGUI
		virtual void OnGUI() override;
		void OnGUIMesh();
		void OnGUIMaterial();
#endif // USE_IMGUI

		virtual void OnCreated() = 0;
		virtual void OnDestroyed() = 0;

		void ResizeMaterial(int size);
		void ResizeShadowMaterial(int size);

		static Mesh& GetQuad();

		virtual void RenderShadow(
			ID3D11DeviceContext* p_context,
			int index, int start_index, int instance_count,
			VertexBuffer& instance_buffer
		) override;

		bool IsActiveAndEnabled() override;

		// ==============================================
		// protected variables
		// ==============================================
		Mesh m_mesh;
		std::vector<Material> m_materials;
		Mesh m_shadow_mesh;
		std::vector<Material> m_shadow_materials;

		Bounds m_bounds;
		MATRIX m_current_transform_matrix = MATRIX::Identity;
		VECTOR3 m_current_world_position = VECTOR3::Zero;
		VECTOR4 m_instance_color = VECTOR4::One;

	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			if (version == 2)
			{
				archive(
					cereal::base_class<Component>(this),
					CEREAL_NVP(m_instance_color),
					CEREAL_NVP(m_renderer_type),
					CEREAL_NVP(m_is_enabled),
					CEREAL_NVP(m_mesh_filedata),
					CEREAL_NVP(m_material_filedata_list),
					CEREAL_NVP(m_shadow_mesh_filedata),
					CEREAL_NVP(m_shadow_material_filedata_list),
					CEREAL_NVP(m_shadow_casting_mode)
				);
			}
			else if (version == 1)
			{
				archive(
					cereal::base_class<Component>(this),
					CEREAL_NVP(m_instance_color),
					CEREAL_NVP(m_renderer_type),
					CEREAL_NVP(m_is_enabled),
					CEREAL_NVP(m_mesh_filedata),
					CEREAL_NVP(m_material_filedata_list),
					CEREAL_NVP(m_shadow_casting_mode)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		void OnButtonCreateMaterial(int mat_index);
		void OnButtonLoadMaterial(int mat_index);
		void OnButtonCreateShadowMaterial(int mat_index);
		void OnButtonLoadShadowMaterial(int mat_index);
#endif // USE_IMGUI

		void OnCreate(const std::shared_ptr<Object>& p_self) override;
		void OnDestroy() override;

		// IRenderer
		void SetInstance(MainInstance* p_instance) override;
		virtual void SetInstance(UIInstance* p_instance) override;

		// Main
		[[nodiscard]] virtual int GetSubsetCount() const override;
		[[nodiscard]] virtual unsigned GetHashMesh() const override;
		[[nodiscard]] virtual unsigned GetHashMaterial(int index) const override;
		[[nodiscard]] virtual int GetRenderQueue(int index) const override;
		// Shadow
		[[nodiscard]] virtual int GetShadowSubsetCount() const override;
		[[nodiscard]] virtual unsigned GetShadowHashMesh() const override;
		[[nodiscard]] virtual unsigned GetShadowHashMaterial(int index) const override;
		[[nodiscard]] virtual int GetShadowRenderQueue(int index) const override;

		[[nodiscard]] inline bool IsUI() const override;
		[[nodiscard]] inline bool IsParticle() const override;
		[[nodiscard]] virtual inline bool IsThroughFrustumCulling() const override;
		[[nodiscard]] inline bool CanBatching() const override;
		[[nodiscard]] virtual inline unsigned GetHashTexture() const override;	// UI用
		[[nodiscard]] virtual inline int GetDepth() const override;		// UI用
		[[nodiscard]] virtual inline bool IsUsedTarget(int index) const override;

		inline void SetVisible(bool is_visible) override;
		[[nodiscard]] virtual inline ShadowCastingMode GetShadowCastMode() const override;

		virtual void CalculateRenderParameter(const std::shared_ptr<ICamera>& camera) override;
		[[nodiscard]] inline const MATRIX& GetWorldMatrix() const override;
		[[nodiscard]] inline const VECTOR3& GetWorldPosition() const override;
		Layer GetLayer() override;
		// ~IRenderer

		void SetGlobalTransform(const Mesh& mesh) const;
		static void CreateQuad();

		// ==============================================
		// private variables
		// ==============================================
		static std::mutex m_mutex;
		static bool m_do_init;
		static Mesh m_quad;

		std::list<std::shared_ptr<IRenderer>>::iterator m_manager_iterator;

		RendererType m_renderer_type = RendererType::Mesh;

		// 描画するか
		bool m_is_enabled = true;
		// カメラに映っているか
		bool m_is_visible = false;

		// Main
		FileLoadStateData m_mesh_filedata;
		std::vector<FileLoadStateData> m_material_filedata_list;
		// Shadow
		FileLoadStateData m_shadow_mesh_filedata;
		std::vector<FileLoadStateData> m_shadow_material_filedata_list;

		ShadowCastingMode m_shadow_casting_mode = ShadowCastingMode::ON;
	};

	// -----------------------------------
	// inline
	// -----------------------------------
	inline bool Renderer::Enabled() const
	{
		return m_is_enabled;
	}
	inline void Renderer::Enabled(bool enable)
	{
		m_is_enabled = enable;
	}

	inline bool Renderer::IsVisible() const
	{
		return m_is_visible;
	}

	inline void Renderer::SetShadowCastMode(ShadowCastingMode mode)
	{
		m_shadow_casting_mode = mode;
	}

	inline void Renderer::SetInstanceColor(const VECTOR4& color)
	{
		m_instance_color = color;
	}

	inline const Bounds& Renderer::GetRendererBounds() const
	{
		return m_bounds;
	}

	inline Mesh& Renderer::GetQuad()
	{
		return m_quad;
	}

	inline bool Renderer::IsUI() const
	{
		return m_renderer_type == RendererType::UI ? true : false;
	}

	inline bool Renderer::IsThroughFrustumCulling() const
	{
		// パーティクルか、Boundsを設定しないスキンメッシュは
		return IsParticle() ? true : false;
	}

	inline bool Renderer::IsParticle() const
	{
		return m_renderer_type == RendererType::Particle;
	}

	inline bool Renderer::CanBatching() const
	{
		return (static_cast<int>(m_renderer_type) >= static_cast<int>(RendererType::Skin)) ? false : true;
	}

	inline unsigned Renderer::GetHashTexture() const
	{
		return 0;
	}

	inline int Renderer::GetDepth() const
	{
		return 0;
	}

	inline bool Renderer::IsUsedTarget(const int index) const
	{
		return m_materials.at(index).IsUsedTarget();
	}

	inline void Renderer::SetVisible(const bool is_visible)
	{
		m_is_visible = is_visible;
	}

	inline ShadowCastingMode Renderer::GetShadowCastMode() const
	{
		return m_shadow_casting_mode;
	}

	inline const MATRIX& Renderer::GetWorldMatrix() const
	{
		return m_current_transform_matrix;
	}

	inline const VECTOR3& Renderer::GetWorldPosition() const
	{
		return m_current_world_position;
	}

}

// Renderer
CEREAL_CLASS_VERSION(TKGEngine::Renderer, 2)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::Renderer, "TKGEngine::Renderer")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Component, TKGEngine::Renderer)