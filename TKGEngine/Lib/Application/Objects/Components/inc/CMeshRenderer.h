#pragma once

#include "CRenderer.h"

#include "Application/Resource/inc/Mesh.h"

namespace TKGEngine
{
	/// <summary>
	/// Rendering Mesh Management module.
	/// </summary>
	class MeshRenderer
		: public Renderer
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		MeshRenderer(const MeshRenderer&) = delete;
		MeshRenderer& operator=(const MeshRenderer&) = delete;

		MeshRenderer();
		virtual ~MeshRenderer();


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
					cereal::base_class<Renderer>(this)
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
		void OnCreated() override {}
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
		// ~IRenderer


		// ==============================================
		// private variables
		// ==============================================




	};

}// namespaace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::MeshRenderer, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::MeshRenderer, "TKGEngine::MeshRenderer")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Renderer, TKGEngine::MeshRenderer)