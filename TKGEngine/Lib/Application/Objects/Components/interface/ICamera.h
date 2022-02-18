#pragma once

#include "Application/inc/ProjectSetting.h"

#include "Utility/inc/myfunc_vector.h"
#include "Utility/inc/Frustum.h"

#include <d3d11.h>

namespace TKGEngine
{
	class Texture;
	typedef int CameraID;

	/// <summary>
	/// Camera module interface.
	/// </summary>
	class ICamera
	{
	public:
		/// <summary>
		/// CBuffer Camera parameter.
		/// </summary>
		struct CB_Camera
		{
			MATRIX V;
			MATRIX P;
			MATRIX IV;
			MATRIX IP;
			MATRIX IVP;
			MATRIX prev_VP;
			float near_dist;
			float far_dist;
			float far_subst_near;
			float inv_far_subst_near;
			VECTOR3 pos;
			float dummy1;
			VECTOR3 direction;
			float dummy2;
			float target_width;
			float target_height;
			float dummy3;
			float dummy4;
		};

		virtual bool ResizeBuffers() = 0;
		// CBufferのセット
		virtual void SetCBCamera(ID3D11DeviceContext* p_context) = 0;
		virtual void SetCBViewProjection(ID3D11DeviceContext* p_context) = 0;
		// SRVのセット
		virtual void SetSRVMain(ID3D11DeviceContext* p_context, int slot, bool use_compute = false) = 0;
		virtual void SetSRVNormal(ID3D11DeviceContext* p_context, int slot, bool use_compute = false) = 0;
		virtual void SetSRVDiffuseSpecular(ID3D11DeviceContext* p_context, int slot, bool use_compute = false) = 0;
		virtual void SetSRVMainRoughness(ID3D11DeviceContext* p_context, int slot, bool use_compute = false) = 0;
		virtual void SetSRVMainDepth(ID3D11DeviceContext* p_context, int slot, bool use_compute = false) = 0;
		// RTV
		virtual void SetRTVs(ID3D11DeviceContext* p_context, bool set_color, bool set_depth) = 0;
		virtual void RemoveRTVs(ID3D11DeviceContext* p_context) = 0;
		virtual void ClearRTVs(ID3D11DeviceContext* p_context, const VECTOR4& color) = 0;
		// Targetのコピー
		virtual void CopyAndBindTargets(ID3D11DeviceContext* p_context) = 0;
		virtual void CopyColorTarget(ID3D11DeviceContext* p_context, Texture& texture) = 0;
		// Viewport
		virtual void SetViewport(ID3D11DeviceContext* p_context) = 0;
		virtual float GetViewportWidth() const = 0;
		virtual float GetViewportHeight() const = 0;
		// カメラ情報
		virtual VECTOR3 GetWorldPosition() const = 0;
		virtual Quaternion GetWorldRotation() const = 0;
		virtual float Depth() const = 0;
		virtual int GetCullingLayer() const = 0;

		virtual void Update(ID3D11DeviceContext* p_context) = 0;

		// ワールド座標系 --> ビュー座標系
		virtual MATRIX GetWorldToViewMatrix() const = 0;
		// ビュー座標系 --> ワールド座標系
		virtual MATRIX GetViewToWorldMatrix() const = 0;
		virtual MATRIX GetProjectionMatrix() const = 0;
		virtual const Frustum& GetFrustum() const = 0;
		virtual Frustum GetFrustum(const float near_dist, const float far_dist) const = 0;
		virtual float GetNear() const = 0;
		virtual float GetFar() const = 0;
		// 有効なカメラか
		virtual bool IsActive() = 0;
		// 画面に出力するか
		virtual bool IsOutputTarget() const = 0;
		
	};


}// namespace TKGEngine