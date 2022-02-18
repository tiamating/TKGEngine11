#pragma once
#include "Systems/inc/TKGEngine_Defined.h"
#ifdef USE_IMGUI
#include "Application/Objects/Components/interface/ICamera.h"
#include "Application/Resource/inc/ITarget.h"
#include "Application/Resource/inc/Texture.h"
#include "Application/Resource/inc/ConstantBuffer.h"
#include "Application/Resource/inc/VertexBuffer.h"
#include "Application/Resource/inc/Shader.h"
#include "Utility/inc/myfunc_vector.h"
#include "Utility/inc/Frustum.h"

#include "memory"

namespace TKGEngine
{
	class DebugCamera
		: public ICamera
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		DebugCamera();
		virtual ~DebugCamera();
		DebugCamera(const DebugCamera&) = delete;
		DebugCamera& operator=(const DebugCamera&) = delete;

		void OnGUIRayPick();

		bool Create();
		void RenderGrid();

		// View行列を渡すことでカメラのTransformを変化させる
		void SetWorldToViewMatrix(const MATRIX& v);
		// Cameraの座標をセットする
		void SetWorldPosition(const VECTOR3& pos);


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================
		// ICamera
		bool ResizeBuffers() override;
		
		void SetCBCamera(ID3D11DeviceContext* p_context) override;
		void SetCBViewProjection(ID3D11DeviceContext* p_context) override;

		void SetSRVMain(ID3D11DeviceContext* p_context, int slot, bool use_compute = false) override;
		void SetSRVNormal(ID3D11DeviceContext* p_context, int slot, bool use_compute = false) override;
		void SetSRVDiffuseSpecular(ID3D11DeviceContext* p_context, int slot, bool use_compute = false) override;
		void SetSRVMainRoughness(ID3D11DeviceContext* p_context, int slot, bool use_compute = false) override;
		void SetSRVMainDepth(ID3D11DeviceContext* p_context, int slot, bool use_compute = false) override;

		void SetRTVs(ID3D11DeviceContext* p_context, bool set_color, bool set_depth) override;
		void RemoveRTVs(ID3D11DeviceContext* p_context) override;
		void ClearRTVs(ID3D11DeviceContext* p_context, const VECTOR4& color) override;

		void CopyAndBindTargets(ID3D11DeviceContext* p_context) override;
		void CopyColorTarget(ID3D11DeviceContext* p_context, Texture& texture) override;

		void SetViewport(ID3D11DeviceContext* p_context) override;
		float GetViewportWidth() const override;
		float GetViewportHeight() const override;

		VECTOR3 GetWorldPosition() const override;
		Quaternion GetWorldRotation() const override;

		float Depth() const override;
		int GetCullingLayer() const override;

		void Update(ID3D11DeviceContext* p_context) override;

		inline MATRIX GetWorldToViewMatrix() const override;
		inline MATRIX GetViewToWorldMatrix() const override;
		inline MATRIX GetProjectionMatrix() const override;
		const Frustum& GetFrustum() const override;
		Frustum GetFrustum(const float near_dist, const float far_dist) const override;
		inline float GetNear() const override;
		inline float GetFar() const override;

		bool IsActive() override;
		bool IsOutputTarget() const override { return true; };
		// ~ICamera

		bool CreateTargetsAndTexture();
		void ReleaseTargets();
		void OnUpdate();

		// ==============================================
		// private variables
		// ==============================================

		// Target
		std::unique_ptr<IColorTarget> m_main_target;
		std::unique_ptr<IColorTarget> m_normal_target;
		std::unique_ptr<IColorTarget> m_diffuse_target;
		std::unique_ptr<IColorTarget> m_specular_target;
		std::unique_ptr<IColorTarget> m_roughness_target;

		std::unique_ptr<IDepthTarget> m_depth_target;

		Texture m_copy_main_target_texture;
		Texture m_copy_depth_target_texture;
		// ~Target

		int m_msaa_count = 1;
		int m_msaa_quarity = 0;	// ID3D11Device::CheckMultisampleQualityLevels

		// Shader
		Shader m_shader;
		// ~Shader

		// Vertex Buffer
		static constexpr int m_axis_line_count_half = 50;
		static constexpr int m_axis_line_count = m_axis_line_count_half * 2 + 1;
		VECTOR3 m_points[m_axis_line_count * 4 * 2];
		VertexBuffer m_vb;
		// ~Vertex Buffer

		// CBuffer
		ConstantBuffer m_cb_camera;
		ConstantBuffer m_cb_VP;
		// ~CBuffer

		// View port
		bool m_is_perspective = true;
		float m_vp_width = 0.0f;
		float m_vp_height = 0.0f;
		float m_top_left_x = 0.0f;
		float m_top_left_y = 0.0f;
		float m_near_clip_plane = 0.01f;
		float m_far_clip_plane = 10000.0f;

		// Perspective param
		float m_fov_y = 60.0f;	// degree(全角)
		// ~Perspective param

		// Orthographic param
		float m_projection_width = 0.0f;
		float m_projection_height = 0.0f;
		// ~Orthographic param

		Frustum m_frustum;

		MATRIX m_view_matrix = MATRIX::Identity;
		MATRIX m_projection_matrix = MATRIX::Identity;
		MATRIX m_reversed_projection_matrix = MATRIX::Identity;

		MATRIX m_previous_view_projection_matrix = MATRIX::Identity;

		// Transform param
		VECTOR3 m_position;
		Quaternion m_rotation;
		// ~Transform param

	};

	// ------------------------------------------
	// inline
	// ------------------------------------------
	inline MATRIX DebugCamera::GetWorldToViewMatrix() const
	{
		return m_view_matrix;
	}
	inline MATRIX DebugCamera::GetViewToWorldMatrix() const
	{
		return m_view_matrix.Invert();
	}
	inline MATRIX DebugCamera::GetProjectionMatrix() const
	{
		return m_reversed_projection_matrix;
	}
	inline float DebugCamera::GetNear() const
	{
		return m_near_clip_plane;
	}
	inline float DebugCamera::GetFar() const
	{
		return m_far_clip_plane;
	}

}// namespace TKGEngine
#endif // USE_IMGUI