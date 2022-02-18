

#include "../../inc/Physics_Shape.h"

#include <btBulletDynamicsCommon.h>


namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// BoxShape Class Methods
	////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	bool BoxShape::OnGUI()
	{
		// Mass
		bool need_update = ShapeBase::OnGUI();
		// Half Extents
		ImGui::Text("Half Extents [m]");
		ImGui::DragFloat3("##half extents", &m_half_extents.x, 0.005f, 0.001f, FLT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp);
		if (ImGui::IsItemActive() || ImGui::IsItemDeactivated())
		{
			m_shape->setImplicitShapeDimensions(ConvertVectorTobtVector(m_half_extents));
		}

		return need_update;
	}
#endif// #ifdef USE_IMGUI

	ShapeID BoxShape::CreateShape()
	{
		// Collision Shapeを作る
		m_shape = std::make_unique<btBoxShape>(ConvertVectorTobtVector(m_half_extents));

		// btCollisonShape*として持っておく
		m_shape_ptr = m_shape.get();

		// IDを割り振る
		m_shape_id = UniqueID();
		m_shape->setUserIndex(m_shape_id);
		// パラメータのセット
		m_shape->setImplicitShapeDimensions(ConvertVectorTobtVector(m_half_extents));

		return m_shape_id;
	}

	// 範囲
	void BoxShape::SetExtents(const VECTOR3& extents)
	{
		// パラメータのクランプ
		m_half_extents = extents / 2.0f;
		if (m_half_extents.x < 0.0f)
		{
			m_half_extents.x = 0.0f;
		}
		if (m_half_extents.y < 0.0f)
		{
			m_half_extents.y = 0.0f;
		}
		if (m_half_extents.z < 0.0f)
		{
			m_half_extents.z = 0.0f;
		}

		// パラメータのセット
		m_shape->setImplicitShapeDimensions(ConvertVectorTobtVector(m_half_extents));
	}


}// namespace TKGEngine