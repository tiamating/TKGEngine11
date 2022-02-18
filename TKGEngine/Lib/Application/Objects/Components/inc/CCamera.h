#pragma once

#include "Components/inc/CBehaviour.h"
#include "../interface/ICamera.h"

#include "Application/Resource/inc/ITarget.h"
#include "Application/Resource/inc/Texture.h"
#include "Application/Resource/inc/ConstantBuffer.h"
#include "Utility/inc/myfunc_vector.h"
#include "Utility/inc/Frustum.h"


namespace TKGEngine
{
	/// <summary>
	/// Management Camera parameter.
	/// </summary>
	class Camera
		: public Behaviour
		, public ICamera
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Camera(const Camera&) = delete;
		Camera& operator=(const Camera&) = delete;

		Camera();
		virtual ~Camera();


		inline MATRIX GetWorldToViewMatrix() const override;
		inline MATRIX GetViewToWorldMatrix() const override;
		inline MATRIX GetProjectionMatrix() const override;

		// ICamera
		inline float GetViewportWidth() const override;
		inline float GetViewportHeight() const override;
		// ~ICamera
		inline void SetViewportWidth(const float width);
		inline void SetViewportHeight(const float height);

		inline void SetCullingLayer(Layer layer);
		inline void EraseCullingLayer(Layer layer);

		inline void SetNear(const float d);
		inline void SetFar(const float d);

		// 視野角
		inline void SetFovY(const float fov);
		inline float GetForY() const;

		inline void SetViewportWidthRatio(const float ratio);
		inline void SetViewportHeightRatio(const float ratio);

		inline void Depth(float depth);
		inline float Depth() const;

		/// <summary>
		/// World座標からNDC座標を求める
		/// </summary>
		/// <param name="point">World空間上の点</param>
		/// <returns>NDC座標</returns>
		VECTOR2 WorldToViewport(const VECTOR3& point) const;
		/// <summary>
		/// NDC座標からWorld座標を求める
		/// </summary>
		/// <param name="point">Projection空間上の点</param>
		/// <returns>World座標</returns>
		VECTOR3 ProjectionToWorld(const VECTOR3& point) const;


		// ==============================================
		// public variables
		// ==============================================


	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			if (version == 2)
			{
				archive(
					cereal::base_class<Behaviour>(this),
					CEREAL_NVP(m_is_output_target),
					CEREAL_NVP(m_culling_layer),
					CEREAL_NVP(m_depth),
					CEREAL_NVP(m_msaa_count),
					CEREAL_NVP(m_msaa_quarity),
					CEREAL_NVP(m_window_width),
					CEREAL_NVP(m_window_height),
					CEREAL_NVP(m_is_perspective),
					CEREAL_NVP(m_vp_width_ratio),
					CEREAL_NVP(m_vp_height_ratio),
					CEREAL_NVP(m_top_left_x),
					CEREAL_NVP(m_top_left_y),
					CEREAL_NVP(m_near_clip_plane),
					CEREAL_NVP(m_far_clip_plane),
					CEREAL_NVP(m_fov_y),
					CEREAL_NVP(m_projection_width),
					CEREAL_NVP(m_projection_height)
				);
			}
			else if (version == 1)
			{
				archive(
					cereal::base_class<Behaviour>(this),
					CEREAL_NVP(m_is_output_target),
					CEREAL_NVP(m_culling_layer),
					CEREAL_NVP(m_depth),
					CEREAL_NVP(m_msaa_count),
					CEREAL_NVP(m_msaa_quarity),
					CEREAL_NVP(m_shadow_threshold),
					CEREAL_NVP(m_bias),
					CEREAL_NVP(m_slope_scaled_bias),
					CEREAL_NVP(m_depth_bias_clamp),
					CEREAL_NVP(m_window_width),
					CEREAL_NVP(m_window_height),
					CEREAL_NVP(m_is_perspective),
					CEREAL_NVP(m_vp_width_ratio),
					CEREAL_NVP(m_vp_height_ratio),
					CEREAL_NVP(m_top_left_x),
					CEREAL_NVP(m_top_left_y),
					CEREAL_NVP(m_near_clip_plane),
					CEREAL_NVP(m_far_clip_plane),
					CEREAL_NVP(m_fov_y),
					CEREAL_NVP(m_projection_width),
					CEREAL_NVP(m_projection_height)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		void OnGUI() override;
		void OnGUICullingLayer();
#endif // USE_IMGUI

		void OnCreate(const std::shared_ptr<Object>& p_self) override;
		bool CreateTargetsAndTexture();
		void ReleaseTargets();

		void OnDestroy() override;

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

		VECTOR3 GetWorldPosition() const override;
		Quaternion GetWorldRotation() const override;

		inline int GetCullingLayer() const override;

		void Update(ID3D11DeviceContext* p_context) override;
		const Frustum& GetFrustum() const override;
		Frustum GetFrustum(const float near_dist, const float far_dist) const override;
		inline float GetNear() const override;
		inline float GetFar() const override;

		bool IsActive() override;
		bool IsOutputTarget() const override;
		// ~ICamera

		// Behaviour
		void OnEnabled() override {};
		void OnDisabled() override {};
		// ~Behaviour

		// ==============================================
		// private variables
		// ==============================================
#ifdef USE_IMGUI
		bool m_is_opened_culling_layer_window = false;
#endif // USE_IMGUI

		CameraID m_camera_id = -1;
		bool m_is_output_target = true;

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

		// Param
		int m_culling_layer = 0;
		float m_depth = 0.f;
		// ~Param

		// Rendering Pathで Deferredを選択している場合は、MSAAは使えない
		// https://docs.microsoft.com/ja-jp/visualstudio/debugger/graphics/0x-2x-4x-msaa-variants?view=vs-2019
		int m_msaa_count = 1;
		int m_msaa_quarity = 0;	// ID3D11Device::CheckMultisampleQualityLevels

		// CBuffer
		ConstantBuffer m_cb_camera;
		ConstantBuffer m_cb_VP;
		// ~CBuffer

		// TODO : 削除予定
		// Shadow param
		float m_shadow_threshold = 0.0f;
		float m_bias = 0.1f;
		float m_slope_scaled_bias = 0.0f;
		float m_depth_bias_clamp = 0.0f;
		// ~Shadow param

		float m_window_width = 0.f;
		float m_window_height = 0.f;

		// View port
		bool m_is_perspective = true;
		float m_vp_width_ratio = 1.0f;
		float m_vp_height_ratio = 1.0f;
		float m_top_left_x = 0.0f;
		float m_top_left_y = 0.0f;
		float m_near_clip_plane = 0.01f;
		float m_far_clip_plane = 5000.0f;

		// Perspective param
		float m_fov_y = 60.0f;	// degree(全角)
		// ~Perspective param

		// Orthographic param
		float m_projection_width = 1000.0f;
		float m_projection_height = 1000.0f;
		// ~Orthographic param

		// ~View port

		MATRIX m_view_matrix = MATRIX::Identity;
		MATRIX m_projection_matrix = MATRIX::Identity;
		MATRIX m_reversed_projection_matrix = MATRIX::Identity;

		MATRIX m_previous_view_projection_matrix = MATRIX::Identity;

		Frustum m_frustum;
	};

	// --------------------------------------------------------------
	// inline
	// --------------------------------------------------------------
	inline MATRIX Camera::GetWorldToViewMatrix() const
	{
		return m_view_matrix;
	}

	inline MATRIX Camera::GetViewToWorldMatrix() const
	{
		return m_view_matrix.Invert();
	}

	inline MATRIX Camera::GetProjectionMatrix() const
	{
		return m_reversed_projection_matrix;
	}

	inline float Camera::GetViewportWidth() const
	{
		return m_vp_width_ratio * m_window_width;
	}
	inline float Camera::GetViewportHeight() const
	{
		return m_vp_height_ratio * m_window_height;
	}

	inline void Camera::SetViewportWidth(const float width)
	{
		m_window_width = width;
	}

	inline void Camera::SetViewportHeight(const float height)
	{
		m_window_height = height;
	}

	inline void Camera::SetCullingLayer(Layer layer)
	{
		m_culling_layer |= (1 << static_cast<int>(layer));
	}
	inline void Camera::EraseCullingLayer(Layer layer)
	{
		m_culling_layer &= ~(1 << static_cast<int>(layer));
	}
	inline int Camera::GetCullingLayer() const
	{
		return m_culling_layer;
	}

	inline void Camera::SetNear(const float d)
	{
		m_near_clip_plane = d;
	}

	inline void Camera::SetFar(const float d)
	{
		m_far_clip_plane = d;
	}

	inline void Camera::SetFovY(const float fov)
	{
		m_fov_y = fov;
	}

	inline float Camera::GetForY() const
	{
		return m_fov_y;
	}

	inline void Camera::SetViewportWidthRatio(const float ratio)
	{
		m_vp_width_ratio = ratio;
	}

	inline void Camera::SetViewportHeightRatio(const float ratio)
	{
		m_vp_height_ratio = ratio;
	}

	inline void Camera::Depth(const float depth)
	{
		m_depth = depth;
	}
	inline float Camera::Depth() const
	{
		return m_depth;
	}

	inline float Camera::GetNear() const
	{
		return m_near_clip_plane;
	}

	inline float Camera::GetFar() const
	{
		return m_far_clip_plane;
	}

}// namespace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::Camera, 2)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::Camera, "TKGEngine::Camera")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Behaviour, TKGEngine::Camera)
