

#include "../../inc/Physics_Shape.h"

#include <btBulletDynamicsCommon.h>


namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// ConeShape Class Methods
	////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	bool ConeShape::OnGUI()
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
		// Height
		ImGui::Text("Height");
		ImGui::DragFloat("##height", &m_height, 0.005f, 0.0f, FLT_MAX, "%.3f[m]", ImGuiSliderFlags_AlwaysClamp);
		if (ImGui::IsItemActive() || ImGui::IsItemDeactivated())
		{
			SetHeight(m_height);
		}

		return need_update;
	}
#endif// #ifdef USE_IMGUI

	ShapeID ConeShape::CreateShape()
	{
		// Collision Shape�����
		m_shape = std::make_unique<btConeShape>(m_radius, m_height);

		// btCollisonShape*�Ƃ��Ď����Ă���
		m_shape_ptr = m_shape.get();

		// ID������U��
		m_shape_id = UniqueID();
		m_shape->setUserIndex(m_shape_id);
		// �p�����[�^�̃Z�b�g
		SetRadius(m_radius);
		SetHeight(m_height);

		return m_shape_id;
	}

	void ConeShape::SetRadius(float radius)
	{
		// �p�����[�^�̃N�����v
		if (radius < 0.0f)
		{
			radius = 0.0f;
		}

		// �p�����[�^�̃Z�b�g
		m_radius = radius;
		m_shape->setRadius(btScalar(m_radius));
	}

	void ConeShape::SetHeight(float height)
	{
		// �p�����[�^�̃N�����v
		if (height < 0.0f)
		{
			height = 0.0f;
		}

		// �p�����[�^�̃Z�b�g
		m_height = height;
		m_shape->setHeight(btScalar(m_height));
	}

}// namespace TKGEngine