

#include "../../inc/Physics_Shape.h"

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Static member definition
	////////////////////////////////////////////////////////
	ShapeID ShapeBase::m_current_shape_id = 0;



	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	ShapeBase::~ShapeBase() = default;

#ifdef USE_IMGUI
	bool ShapeBase::OnGUI()
	{
		bool need_update = false;

		// Mass
		ImGui::Text("Mass (minimum mass is 1g)");
		ImGui::DragFloat("##mass", &m_mass, 0.01f, MIN_MASS, FLT_MAX, "%.3f[kg]", ImGuiSliderFlags_AlwaysClamp);
		// Translation
		ImGui::Text("Translation [m]");
		ImGui::DragFloat3("##translate", &m_translation.x, 0.005f, 0.0f, 0.0f, "%.2f");
		if (ImGui::IsItemActive() || ImGui::IsItemDeactivated())
		{
			need_update = true;
		}
		// Rotation
		ImGui::Text("Rotation [deg]");
		VECTOR3 euler = m_rotation.ToEulerAngles();
		const float item_width = ImGui::GetWindowWidth() / 4.5f;
		// X
		ImGui::SetNextItemWidth(item_width);
		ImGui::DragFloat("##rotate x", &euler.x, 0.3f, -89.0f, 89.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
		if (ImGui::IsItemActive() || ImGui::IsItemDeactivated())
		{
			need_update = true;
			m_rotation = euler.ToQuaternion();
		}
		// Y
		ImGui::SetNextItemWidth(item_width);
		ImGui::SameLine(0.0f, 0.5f);
		ImGui::DragFloat("##rotate y", &euler.y, 0.3f, -180.0f, 180.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
		if (ImGui::IsItemActive() || ImGui::IsItemDeactivated())
		{
			need_update = true;
			m_rotation = euler.ToQuaternion();
		}
		// Z
		ImGui::SetNextItemWidth(item_width);
		ImGui::SameLine(0.0f, 0.3f);
		ImGui::DragFloat("##rotate z", &euler.z, 0.3f, -180.0f, 180.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
		if (ImGui::IsItemActive() || ImGui::IsItemDeactivated())
		{
			need_update = true;
			m_rotation = euler.ToQuaternion();
		}

		return need_update;
	}
#endif// #ifdef USE_IMGUI

	void ShapeBase::SetTranslation(const VECTOR3& tr)
	{
		m_translation = tr;
	}

	void ShapeBase::SetRotation(const Quaternion& quat)
	{
		m_rotation = quat;
	}

	ShapeID ShapeBase::CreateShape(
		const VECTOR3& translate,
		const Quaternion& quat
	)
	{
		// à íuÅAâÒì]ÇÃílÇéÛÇØéÊÇÈ
		m_translation = translate;
		m_rotation = quat;

		// Collision ShapeÇçÏÇÈ
		return CreateShape();
	}

	ShapeID ShapeBase::UniqueID()
	{
		return m_current_shape_id++;
	}

}// namespace TKGEngine