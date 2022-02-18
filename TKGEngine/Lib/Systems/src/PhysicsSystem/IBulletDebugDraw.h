#pragma once

#include "../../inc/StateManager.h"
#include "../../../Lib/Application/Resource/inc/Shader.h"
#include "../../../Lib/Application/Resource/inc/VertexBuffer.h"

#include "../../../Utility/inc/myfunc_vector.h"

#include <LinearMath/btIDebugDraw.h>

#include <vector>

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace TKGEngine
{
	class IBulletDebugDraw
		: public btIDebugDraw
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		IBulletDebugDraw() = default;
		virtual ~IBulletDebugDraw() = default;

		bool Initialize();

		void SetDebugMode(int mode) { setDebugMode(mode); }
		DebugDrawModes GetDebugMode() const { return static_cast<DebugDrawModes>(getDebugMode()); }

		// デバッグ用形状描画
		void DrawLine(const VECTOR3& from, const VECTOR3& to, const VECTOR3& from_color, const VECTOR3& to_color);
		void DrawSphere(const VECTOR3& pos, const float radius, const VECTOR3& color);
		void DrawBox(const VECTOR3& pos, const Quaternion& rot, const VECTOR3& half_extents, const VECTOR3& color);
		void DrawCapsule(const VECTOR3& pos, const Quaternion& rot, const float radius, const float height, const VECTOR3& color);
		void DrawCone(const VECTOR3& pos, const Quaternion& rot, const float radius, const float height, const VECTOR3& color);
		void DrawContactPoint(const VECTOR3& pos, const VECTOR3& normal, const float distance, const VECTOR3& color);

		// ==============================================
		// public methods (virtual)
		// ==============================================



		// ==============================================
		// public variables
		// ==============================================




	private:
		// ==============================================
		// private methods
		// ==============================================
		void FlushLines();


		// ==============================================
		// private methods (virtual)
		// ==============================================
		virtual void setDebugMode(int debug_mode) override { m_bit_debug_mode = debug_mode; }
		virtual int getDebugMode() const override { return m_bit_debug_mode; }

		virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& from_color, const btVector3& to_color) override;
		virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;

		virtual void flushLines() override { FlushLines(); }
		virtual void drawContactPoint(const btVector3& Point_on_B, const btVector3& normal_on_B, btScalar distance, int life_time, const btVector3& color) override
		{
			drawLine(Point_on_B, Point_on_B + normal_on_B * distance, color);
			btVector3 ncolor(0, 0, 0);
			drawLine(Point_on_B, Point_on_B + normal_on_B * 0.01f, ncolor);
		}

		//未対応
		virtual void reportErrorWarning(const char* warning_string) {}
		virtual void draw3dText(const btVector3& location, const char* text_string) {}


		// ==============================================
		// private variables
		// ==============================================
		struct DebugLineData
		{
			bool Create();
			
			void Render(ID3D11DeviceContext* context, Shader& shader);

			void PushBack
			(
				const VECTOR3& from_pos, const VECTOR3& to_pos,
				const VECTOR3& from_color, const VECTOR3& to_color
			);

			size_t Size() const;

			static constexpr const int MAX_LINE = 30000;
		private:
			void SetCBVertex(ID3D11DeviceContext* context);
			void Clear();

			std::vector<VECTOR3> m_position;
			std::vector<VECTOR4> m_color;

			VertexBuffer m_vb_position;
			VertexBuffer m_vb_color;

		};
		DebugLineData m_line_data;

		Stencil::Mask m_stencil_mask = Stencil::Mask::None;
		StateManager::DS m_depth_state = StateManager::DS::Geometry;
		StateManager::RS m_rasterizer_state = StateManager::RS::WireNone;
		StateManager::BS m_blend_state = StateManager::BS::Opaque;

		Shader m_shader;

		int m_bit_debug_mode = 0;
	};



}// namespace TKGEngine