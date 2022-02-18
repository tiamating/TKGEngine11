#pragma once

#include "Application/Resource/inc/VertexElement.h"
#include "Application/inc/ProjectSetting.h"

#include "Utility/inc/myfunc_vector.h"
#include "Utility/inc/bounds.h"

#include <memory>

struct ID3D11DeviceContext;


namespace TKGEngine
{
	struct MainInstance;
	struct DepthInstance;
	struct UIInstance;

	class IRenderer;
	class ICamera;
	class VertexBuffer;

	// 影をどうするか
	enum class ShadowCastingMode
	{
		ON = 0,
		OFF,
		ShadowsOnly,
		MAX_ShadowCastingMode
	};
	
	// メイン描画に使用するデータ
	struct MainData
	{
		bool do_render = false;
		int queue;
		unsigned mesh_hash;
		unsigned material_hash;
		int subset_idx;
		float distance;
		std::shared_ptr<IRenderer> renderer;
		bool can_batching;
		bool use_target;
	};

	// UI描画時に使用するデータ
	struct UIData
	{
		int depth;
		//unsigned mesh_hash;
		unsigned material_hash;
		unsigned texture_hash;
		std::shared_ptr<IRenderer> renderer;
		bool use_target;
	};


	/// <summary>
	/// Renderer interface.
	/// </summary>
	class IRenderer
	{
	public:
		virtual void Render(
			ID3D11DeviceContext* p_context,
			int index, int start_index, int instance_count,
			VertexBuffer& instance_buffer,
			const std::shared_ptr<ICamera>& camera,
			bool write_depth
		) = 0;
		virtual void RenderShadow(
			ID3D11DeviceContext* p_context,
			int index, int start_index, int instance_count,
			VertexBuffer& instance_buffer
		) = 0;
		virtual void SetInstance(MainInstance* p_instance) = 0;
		virtual void SetInstance(UIInstance* p_instance) = 0;

		// Main
		virtual int GetSubsetCount() const = 0;
		virtual unsigned GetHashMesh() const = 0;
		virtual unsigned GetHashMaterial(int index) const = 0;
		virtual unsigned GetHashTexture() const = 0;	// UI用
		virtual int GetRenderQueue(int index) const = 0;
		virtual inline int GetDepth() const = 0;		// UI用
		// Shadow
		virtual int GetShadowSubsetCount() const = 0;
		virtual unsigned GetShadowHashMesh() const = 0;
		virtual unsigned GetShadowHashMaterial(int index) const = 0;
		virtual int GetShadowRenderQueue(int index) const = 0;

		virtual bool IsActiveAndEnabled() = 0;
		virtual inline bool IsUI() const = 0;
		virtual inline bool IsParticle() const = 0;
		virtual inline bool IsThroughFrustumCulling() const = 0;
		virtual inline bool CanBatching() const = 0;
		virtual inline bool IsUsedTarget(int index) const = 0;
		virtual inline void SetVisible(bool is_visible) = 0;
		virtual inline ShadowCastingMode GetShadowCastMode() const = 0;

		virtual void CalculateRenderParameter(const std::shared_ptr<ICamera>& camera) = 0;
		virtual inline const Bounds& GetRendererBounds() const = 0;
		virtual inline const MATRIX& GetWorldMatrix() const = 0;
		virtual inline const VECTOR3& GetWorldPosition() const = 0;

		virtual Layer GetLayer() = 0;

	};
}// namespace TKGEngine