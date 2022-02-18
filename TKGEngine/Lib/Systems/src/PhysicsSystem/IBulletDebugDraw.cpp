#include "IBulletDebugDraw.h"

#include "../../inc/IGraphics.h"
#include "Systems/inc/Physics_Defined.h"

#include <cassert>

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	bool IBulletDebugDraw::Initialize()
	{
		// Create Line data.
		{
			if (!m_line_data.Create())
			{
				assert(0 && "failed IBulletDebugDraw::Initialize::DebugLineData::Create()");
				return false;
			}
		}

		// Create shader object.
		{
			m_shader.VS().Load("./Shader/BulletDebug_VS.cso");
			m_shader.PS().Load("./Shader/BulletDebug_PS.cso");
		}

		return true;
	}

	void IBulletDebugDraw::DrawLine(const VECTOR3& from, const VECTOR3& to, const VECTOR3& from_color, const VECTOR3& to_color)
	{
		if (m_line_data.Size() + 1 >= DebugLineData::MAX_LINE) return;

		m_line_data.PushBack(from, to, from_color, to_color);
	}

	void IBulletDebugDraw::DrawSphere(const VECTOR3& pos, const float radius, const VECTOR3& color)
	{
		drawSphere(ConvertVectorTobtVector(pos), btScalar(radius), ConvertVectorTobtVector(color));
	}

	void IBulletDebugDraw::DrawBox(const VECTOR3& pos, const Quaternion& rot, const VECTOR3& half_extents, const VECTOR3& color)
	{
		drawBox(
			ConvertVectorTobtVector(VECTOR3(-half_extents)),
			ConvertVectorTobtVector(VECTOR3(+half_extents)),
			btTransform(ConvertQuaternionTobtQuaternion(rot), ConvertVectorTobtVector(pos)),
			ConvertVectorTobtVector(color)
		);
	}

	void IBulletDebugDraw::DrawCapsule(const VECTOR3& pos, const Quaternion& rot, const float radius, const float height, const VECTOR3& color)
	{
		drawCapsule(
			btScalar(radius),
			btScalar(height * 0.5f),
			1/* Y up */,
			btTransform(ConvertQuaternionTobtQuaternion(rot), ConvertVectorTobtVector(pos)),
			ConvertVectorTobtVector(color)
		);
	}

	void IBulletDebugDraw::DrawCone(const VECTOR3& pos, const Quaternion& rot, const float radius, const float height, const VECTOR3& color)
	{
		drawCone(
			btScalar(radius),
			btScalar(height),
			1/* Y up */,
			btTransform(ConvertQuaternionTobtQuaternion(rot), ConvertVectorTobtVector(pos)),
			ConvertVectorTobtVector(color)
		);
	}

	void IBulletDebugDraw::DrawContactPoint(const VECTOR3& pos, const VECTOR3& normal, const float distance, const VECTOR3& color)
	{
		drawContactPoint(
			ConvertVectorTobtVector(pos),
			ConvertVectorTobtVector(normal),
			btScalar(distance),
			0,
			ConvertVectorTobtVector(color)
		);
	}

	void IBulletDebugDraw::FlushLines()
	{
		if (m_line_data.Size() == 0) return;

		ID3D11DeviceContext* context = IGraphics::Get().DC(0, Graphics::DC_RENDER_PATH::DC_RP_MAIN);

		m_line_data.Render(context, m_shader);
	}

	void IBulletDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& from_color, const btVector3& to_color)
	{
		if (m_line_data.Size() + 1 >= DebugLineData::MAX_LINE) return;

		m_line_data.PushBack
		(
			ConvertbtVectorToVector(from),
			ConvertbtVectorToVector(to),
			ConvertbtVectorToVector(from_color),
			ConvertbtVectorToVector(to_color)
		);
	}

	void IBulletDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
	{
		drawLine(from, to, color, color);
	}

	// IBulletDebugDraw::DebugLineData
	bool IBulletDebugDraw::DebugLineData::Create()
	{
		// Create buffer.
		{
			m_position.reserve(MAX_LINE);
			m_color.reserve(MAX_LINE);

			if (m_vb_position.Create(
				nullptr, sizeof(VECTOR3), MAX_LINE,
				VERTEX_ELEMENT_TYPE::POSITION,
				BUFFER_HEAP_TYPE::BUFFER_HEAP_DYNAMIC
			) == false)
			{
				assert(0 && "failed IBulletDebugDraw::DebugLineData::Create()::VertexBuffer::Create() POSITION.");
				return false;
			}
			if (m_vb_color.Create(
				nullptr, sizeof(VECTOR4), MAX_LINE,
				VERTEX_ELEMENT_TYPE::COLOR,
				BUFFER_HEAP_TYPE::BUFFER_HEAP_DYNAMIC
			) == false)
			{
				assert(0 && "failed IBulletDebugDraw::DebugLineData::Create()::VertexBuffer::Create() COLOR.");
				return false;
			}

		}

		return true;
	}

	void IBulletDebugDraw::DebugLineData::Render(ID3D11DeviceContext* context, Shader& shader)
	{
		// Vertex Bufferのコピーとセット
		SetCBVertex(context);

		// パイプラインステートのセット
		shader.Activate(context, false);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		StateManager::SetState(context, StateManager::BS::Opaque);
		StateManager::SetState(context, StateManager::DS::BackGround, Stencil::Mask::None,false);
		StateManager::SetState(context, StateManager::RS::FillNone);

		// 描画命令
		context->Draw(Size(), 0);

		// Clear vectors.
		Clear();
	}

	void IBulletDebugDraw::DebugLineData::SetCBVertex(ID3D11DeviceContext* context)
	{
		// Copy vertex buffers.
		{
			const size_t vec_size = m_position.size();
			// Position
			{
				VECTOR3* mapped_data = nullptr;
				m_vb_position.Map(context, reinterpret_cast<void**>(&mapped_data));

				memcpy(mapped_data, m_position.data(), vec_size * sizeof(VECTOR3));

				m_vb_position.Unmap(context);
			}
			// Color
			{
				VECTOR3* mapped_data = nullptr;
				m_vb_color.Map(context, reinterpret_cast<void**>(&mapped_data));

				memcpy(mapped_data, m_color.data(), vec_size * sizeof(VECTOR4));

				m_vb_color.Unmap(context);
			}
		}

		// Set vertex buffers.
		m_vb_position.Set(context, 0);
		m_vb_color.Set(context, 1);
	}

	void IBulletDebugDraw::DebugLineData::PushBack
	(
		const VECTOR3& from_pos, const VECTOR3& to_pos,
		const VECTOR3& from_color, const VECTOR3& to_color
	)
	{
		m_position.emplace_back(from_pos);
		m_position.emplace_back(to_pos);

		m_color.emplace_back(VECTOR4(from_color, 1.0f));
		m_color.emplace_back(VECTOR4(to_color, 1.0f));
	}

	size_t IBulletDebugDraw::DebugLineData::Size() const
	{
		return m_position.size();
	}

	void IBulletDebugDraw::DebugLineData::Clear()
	{
		m_position.clear();
		m_color.clear();
	}
	// ~IBulletDebugDraw::DebugLineData

}// namespace TKGEngine