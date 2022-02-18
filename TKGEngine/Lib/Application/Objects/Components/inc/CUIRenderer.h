#pragma once

#include "CRenderer.h"

#include "Application/Resource/inc/Texture.h"
#include "Utility/inc/myfunc_vector.h"

namespace TKGEngine
{
	/// <summary>
	/// Rendering UI Management module.
	/// </summary>
	class UIRenderer
		: public Renderer
	{
	public:
		// 座標、回転、スケーリングの基準
		enum class Pivot
		{
			Center,
			LeftTop,
			RightTop,
			LeftBottom,
			RightBottom,
			Top,
			Bottom,
			Left,
			Right,

			Max_Pivot
		};

		// ==============================================
		// public methods
		// ==============================================
		UIRenderer(const UIRenderer&) = delete;
		UIRenderer& operator=(const UIRenderer&) = delete;

		UIRenderer();
		virtual ~UIRenderer();

		bool SetTexture(const char* filepath, bool force_srgb = false);
		bool SetTexture(const Texture& texture);
		bool HasTexture() const;

		void Depth(int depth);
		int Depth() const;

		void SetPivot(Pivot pivot);

		// Screen ratio.
		void DrawPos(const VECTOR2& pos);
		void DrawPos(const float x, const float y);
		void DrawPosX(const float x);
		void DrawPosY(const float y);
		VECTOR2 DrawPos() const;
		// On Screen size ratio.
		void DrawSize(const VECTOR2& size);
		void DrawSize(const float x, const float y);
		void DrawSizeX(const float x);
		void DrawSizeY(const float y);
		VECTOR2 DrawSize() const;

		// Texture pos.
		void SamplePos(const VECTOR2& pos);
		void SamplePos(const float x, const float y);
		void SamplePosX(const float x);
		void SamplePosY(const float y);
		VECTOR2 SamplePos() const;
		// Texture size(texel).
		void SampleSize(const VECTOR2& size);
		void SampleSize(const float x, const float y);
		void SampleSizeX(const float x);
		void SampleSizeY(const float y);
		VECTOR2 SampleSize() const;

		void Rotation(const Quaternion& quat);
		Quaternion Rotation() const;

		void EulerAngles(const VECTOR3& euler);
		void EulerAngles(const float degree_x, const float degree_y, const float degree_z);
		void EulerAngleX(const float degree_x);
		void EulerAngleY(const float degree_y);
		void EulerAngleZ(const float degree_z);
		VECTOR3 EulerAngles() const;


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
					CEREAL_NVP(m_depth),
					CEREAL_NVP(m_pivot),
					CEREAL_NVP(m_draw_pos),
					CEREAL_NVP(m_draw_size),
					CEREAL_NVP(m_sample_pos),
					CEREAL_NVP(m_sample_size),
					CEREAL_NVP(m_rotation),
					CEREAL_NVP(m_euler_angles)
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

		void SetInstance(UIInstance* p_instance) override;

		int GetSubsetCount() const override;
		unsigned GetHashMesh() const override;
		unsigned GetHashMaterial(int index) const override;
		int GetRenderQueue(int index) const override;
		inline bool IsUsedTarget(int index) const override;
		ShadowCastingMode GetShadowCastMode() const override;
		void CalculateRenderParameter(const std::shared_ptr<ICamera>& camera) override;
		unsigned GetHashTexture() const override;	// UI用
		inline int GetDepth() const override;		// UI用
		// ~IRenderer

		void CalculateOriginFromPivot(VECTOR3& origin, VECTOR2& offset) const;


		// ==============================================
		// private variables
		// ==============================================
		Texture m_texture;
		FileLoadStateData m_texture_filedata;
		bool m_force_srgb = false;

		int m_depth = 0;
		Pivot m_pivot = Pivot::Center;

		VECTOR4 m_texcoord_transform = VECTOR4::Zero;

		// 画面内(ビューポート内)の描画位置、サイズ( viewport左上( 0,0 )とする )
		VECTOR2 m_draw_pos = VECTOR2::Zero;		// 画面上の(x,y)の割合位置
		VECTOR2 m_draw_size = VECTOR2::Zero;	// 画面上の割合サイズ
		// テクスチャ内の描画位置、サイズ(texel)
		VECTOR2 m_sample_pos = VECTOR2::Zero;	// テクスチャ上の(x,y)
		VECTOR2 m_sample_size = VECTOR2::Zero;	// テクスチャ上のサイズ

		// 回転角度(degrees)
		Quaternion m_rotation = Quaternion::Identity;
		VECTOR3 m_euler_angles = VECTOR3::Zero;
	};

	// -----------------------------------
	// inline
	// -----------------------------------
	inline int UIRenderer::GetDepth() const
	{
		return m_depth;
	}

	inline bool UIRenderer::IsUsedTarget(int index) const
	{
		assert(index == 0);

		return m_materials.at(index).IsUsedTarget();
	}

}

CEREAL_CLASS_VERSION(TKGEngine::UIRenderer, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::UIRenderer, "TKGEngine::UIRenderer")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Renderer, TKGEngine::UIRenderer)