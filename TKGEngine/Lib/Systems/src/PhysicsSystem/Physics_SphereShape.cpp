

#include "../../inc/Physics_Shape.h"

#include <btBulletDynamicsCommon.h>


namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// SphereShape Class Methods
	////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	bool SphereShape::OnGUI()
	{
		// Mass
		bool need_update = ShapeBase::OnGUI();
		// Radius
		ImGui::Text("Radius");
		ImGui::DragFloat("##radius", &m_radius, 0.005f, 0.0f, FLT_MAX, "%.3f[m]", ImGuiSliderFlags_AlwaysClamp);
		if (ImGui::IsItemActive() || ImGui::IsItemDeactivated())
		{
			SetRadius(m_radius);
		}
		
		return need_update;
	}
#endif// #ifdef USE_IMGUI

	ShapeID SphereShape::CreateShape()
	{
		// Collision Shapeを作る
		m_shape = std::make_unique<btSphereShape>(m_radius);

		// btCollisonShape*として持っておく
		m_shape_ptr = m_shape.get();

		// IDを割り振る
		m_shape_id = UniqueID();
		m_shape->setUserIndex(m_shape_id);
		// パラメータのセット
		m_shape->setUnscaledRadius(btScalar(m_radius));

		return m_shape_id;
	}

	void SphereShape::SetRadius(float radius)
	{
		// パラメータのクランプ
		if (radius < 0.0f)
		{
			radius = 0.0f;
		}

		// パラメータのセット
		m_radius = radius;
		m_shape->setUnscaledRadius(btScalar(m_radius));
	}


}// namespace TKGEngine