#pragma once

#include "CRenderer.h"

#include "Application/Resource/inc/Texture.h"

namespace TKGEngine
{
	/// <summary>
	/// Rendering Sprite Management module.
	/// </summary>
	class SpriteRenderer
		: public Renderer
	{
	public:
		enum class BillboardType
		{
			None,		// Ç»Çµ
			Vertical,	// èÌÇ…Yé≤ÇÃÇ›âÒì]
			Horizontal,	// èÌÇ…Yé≤à»äOÇÃÇ›âÒì]
			View,		// ÉJÉÅÉâï˚å¸

			Max_BillboardType
		};

		// ==============================================
		// public methods
		// ==============================================
		SpriteRenderer(const SpriteRenderer&) = delete;
		SpriteRenderer& operator=(const SpriteRenderer&) = delete;

		SpriteRenderer();
		virtual ~SpriteRenderer();

		bool SetTexture(const char* filepath, bool force_srgb = false);
		bool SetTexture(const Texture& texture);
		bool HasTexture() const;

		inline void SetBillboardType(BillboardType type);
		

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
					cereal::base_class<Renderer>(this),
					CEREAL_NVP(m_texture_filedata),
					CEREAL_NVP(m_force_srgb),
					CEREAL_NVP(m_billboard_type)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		void OnGUI() override;
#endif // USE_IMGUI

		// Renderer
		void OnCreated() override;
		void OnDestroyed() override {}
		// ~Renderer

		// IRenderer
		void Render(
			ID3D11DeviceContext* p_context,
			int index, int start_index, int instance_count,
			VertexBuffer& instance_buffer,
			const std::shared_ptr<ICamera>& camera,
			bool write_depth
		) override;
		void RenderShadow(
			ID3D11DeviceContext* p_context,
			int index, int start_index, int instance_count,
			VertexBuffer& instance_buffer
		) override;

		int GetSubsetCount() const override;
		unsigned GetHashMesh() const override;
		unsigned GetHashMaterial(int index) const override;
		void CalculateRenderParameter(const std::shared_ptr<ICamera>& camera) override;
		// ~IRenderer


		// ==============================================
		// private variables
		// ==============================================
		Texture m_texture;
		FileLoadStateData m_texture_filedata;
		bool m_force_srgb = false;

		BillboardType m_billboard_type = BillboardType::None;

	};

	// -----------------------------------
	// inline
	// -----------------------------------
	inline void SpriteRenderer::SetBillboardType(BillboardType type)
	{
		m_billboard_type = type;
	}


}// namespaace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::SpriteRenderer, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::SpriteRenderer, "TKGEngine::SpriteRenderer")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Renderer, TKGEngine::SpriteRenderer)