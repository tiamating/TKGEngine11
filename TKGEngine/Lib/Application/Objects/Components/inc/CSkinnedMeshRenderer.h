#pragma once

#include "CRenderer.h"

#include "Application/Resource/inc/Shader.h"
#include "Application/Resource/inc/Mesh.h"
#include "Application/Resource/inc/VertexBuffer.h"
#include "Utility/inc/bounds.h"


namespace TKGEngine
{
	class Animator;
	class IGameObject;

	/// <summary>
	/// Rendering SkinnedMesh Management module.
	/// </summary>
	class SkinnedMeshRenderer
		: public Renderer
	{
	public:
		// ==============================================
		// public declaration
		// ==============================================


		// ==============================================
		// public methods
		// ==============================================
		SkinnedMeshRenderer(const SkinnedMeshRenderer&) = delete;
		SkinnedMeshRenderer& operator=(const SkinnedMeshRenderer&) = delete;

		SkinnedMeshRenderer();
		virtual ~SkinnedMeshRenderer();

		/// <summary>
		/// Mesh追加時に頂点バッファを作成する
		/// </summary>
		virtual void AddMesh(const std::string& filepath, bool set_global = true) override;
		virtual void AddMesh(const Mesh& mesh, bool set_global = true) override;

		// Animator
		void AddAnimator(const std::shared_ptr<Animator>& animator);
		void AddAnimator(const std::shared_ptr<IGameObject>& gameobject);
		inline void RemoveAnimator();

		// Transform
		void SetRootTransform(const std::shared_ptr<Transform>& transform);
		void SetRootTransform(const std::shared_ptr<IGameObject>& gameobject);
		void RemoveRootTransform();

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			if (version == 2)
			{
				archive(
					cereal::base_class<Renderer>(this),
					CEREAL_NVP(m_animator),
					CEREAL_NVP(m_root_transform),
					CEREAL_NVP(m_culling_always_through),
					CEREAL_NVP(m_local_bounds)
				);
			}
			else if (version == 1)
			{
				archive(
					cereal::base_class<Renderer>(this),
					CEREAL_NVP(m_animator),
					CEREAL_NVP(m_root_transform)
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
		[[nodiscard]] bool IsThroughFrustumCulling() const override;
		virtual void CalculateRenderParameter(const std::shared_ptr<ICamera>& camera) override;
		void Render(
			ID3D11DeviceContext* p_context,
			int index, int start_index, int index_count,
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

		/// <summary>
		/// Meshセット時に頂点バッファを作成する
		/// </summary>
		bool CreateSkinningedVertexBuffer();


		// ==============================================
		// private variables
		// ==============================================
		// スキニング用コンピュートシェーダ
		static ComputeShader m_skinning_shader;
		// スキニングするときのコンピュート同期用
		static std::mutex m_skinninged_mutex;
		// スキニング後頂点バッファ
		VertexBuffer m_skinninged_vs_position;
		VertexBuffer m_skinninged_vs_normal;
		VertexBuffer m_skinninged_vs_tangent;
		// スキニング済みフラグ
		bool m_is_skinninged_on_frame = false;
		
		// アニメーションデータをもつAnimatorの参照
		std::shared_ptr<Animator> m_animator = nullptr;
		// メッシュのアニメーションのルートにあたるTransform
		std::shared_ptr<Transform> m_root_transform = nullptr;

		// スキンメッシュ用バウンディングボックスの設定
		bool m_culling_always_through = true;
		Bounds m_local_bounds;
	};

	// -----------------------------------
	// inline
	// -----------------------------------
	inline void SkinnedMeshRenderer::RemoveAnimator()
	{
		m_animator.reset();
	}

	inline bool SkinnedMeshRenderer::IsThroughFrustumCulling() const
	{
		return m_culling_always_through;
	}

}// namespaace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::SkinnedMeshRenderer, 2)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::SkinnedMeshRenderer, "TKGEngine::SkinnedMeshRenderer")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Renderer, TKGEngine::SkinnedMeshRenderer)