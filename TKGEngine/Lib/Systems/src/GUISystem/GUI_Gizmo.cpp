#include "GUI_Gizmo.h"

#ifdef USE_IMGUI
#include "Systems/inc/IWindow.h"
#include "Systems/inc/IGraphics.h"
#include "Systems/inc/IGUI.h"
#include "Systems/inc/LogSystem.h"

#include "Application/Objects/Components/inc/CTransform.h"
#include "Application/Objects/Managers/CameraManager.h"
#include "Application/Objects/Managers/GameObjectManager.h"

#include <vector>

namespace TKGEngine::GUI::Gizmo
{
	void GUIGizmo::OnFrameBegin()
	{
		// ���t���[���J�n���ɌĂ�
		ImGuizmo::BeginFrame();
	}

	void GUIGizmo::OnFrameRender()
	{
		// ImGui::Button�ɂ�鑀��p�����[�^�I��
		SelectGizmoOperationUseButton();
		// �V���[�g�J�b�g�ɂ��M�Y���̑���p�����[�^�I��
		SelectGizmoOperation();

		// �M�Y���̕\���Ƒ���
		DrawAndControlGizmo();

		// �M�Y���̃C�x���g�֐����Ă�
		CallGizmoEvent();
	}

	void GUIGizmo::OnFrameEnd()
	{
		// �M�Y�������삳��Ă���ΐV�����ϊ��s����Z�b�g����
		ApplyRelativeMatrix();
	}

	void GUIGizmo::EnterControlTransform()
	{
		m_has_control_transform = true;
		CallGizmoEvent();
		m_is_controlled_with_gui = true;
	}

	void GUIGizmo::ExitControlTransform()
	{
		// Transform::GUI�̑���𗣂����u�Ԃ����
		if (m_is_controlled_with_gui)
			m_on_exit_control_gui = true;
		else
			m_on_exit_control_gui = false;

		m_is_controlled_with_gui = false;
	}

	void GUIGizmo::SelectGizmoOperationUseButton()
	{
		// �\������ʒu�����߂�
		const RECT rect = IWindow::Get().GetScreenRect();
		constexpr float WINDOW_TITLE_BAR_SIZE = 55.0f;
		constexpr float REMAIN_SPACE = 10.0f;
		constexpr float TRANSFORM_BUTTON_SPACE = 30.0f;
		const ImVec2 wnd_size(0.0f, TRANSFORM_BUTTON_SPACE);
		ImVec2 wnd_pos(0.f, 0.f);
		{
			wnd_pos.x = static_cast<float>(rect.left) + REMAIN_SPACE;
			wnd_pos.y = static_cast<float>(rect.top) + WINDOW_TITLE_BAR_SIZE;
		}
		ImGui::SetNextWindowPos(wnd_pos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(wnd_size, ImGuiCond_Always);

		// Window color�̐ݒ�
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		// Window�̕\��
		ImGui::Begin("Operation Select Window", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking);
		{
			// ���C�����j���[�o�[�ɂ��I��
			// Local / World
			if (ImGui::Button(m_current_gizmo_mode == ImGuizmo::MODE::LOCAL ? "Local" : "World", ImVec2(64.f, 0.f)))
			{
				m_current_gizmo_mode =
					(m_current_gizmo_mode == ImGuizmo::MODE::LOCAL) ?
					ImGuizmo::MODE::WORLD :
					ImGuizmo::MODE::LOCAL;
			}
			ImGui::SameLine();
			// �\����\��
			if (ImGui::RadioButton("Visible", m_draw_gizmo))
			{
				m_draw_gizmo = !m_draw_gizmo;
			}
			ImGui::SameLine();

			// �M�Y���̃I�y���[�V�����ƕ\���{�^������v������F��ς����{�^����\������
			auto gizmo_button_func = [&](const char* label, ImGuizmo::OPERATION operation)->bool
			{
				bool is_pushed = false;

				// ��v���邩���ׂ�
				if (m_current_gizmo_operation == operation)
				{
					// �I�𒆂̃{�^���̓{�^���F���Z�b�g
					const ImVec4 button_color = ImVec4(0.7f, 0.3f, 0.3f, 1.0f);
					const ImVec4 hovered_color = ImVec4(0.5f, 0.2f, 0.2f, 1.0f);
					const ImVec4 active_color = ImVec4(0.35f, 0.2f, 0.2f, 1.0f);
					is_pushed = ImGui::ButtonColorSettable(label, button_color, hovered_color, active_color);
				}
				else
				{
					// ��I�𒆂̃{�^���\��
					is_pushed = ImGui::Button(label);
				}

				return is_pushed;
			};

			// Translate
			if (gizmo_button_func("Translate", ImGuizmo::OPERATION::TRANSLATE))
			{
				m_current_gizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
				m_draw_gizmo = true;
			}
			ImGui::SameLine();
			// Rotate
			if (gizmo_button_func("Rotate", ImGuizmo::OPERATION::ROTATE))
			{
				m_current_gizmo_operation = ImGuizmo::OPERATION::ROTATE;
				m_draw_gizmo = true;
			}
			ImGui::SameLine();
			// Scale
			if (gizmo_button_func("Scale", ImGuizmo::OPERATION::SCALE))
			{
				m_current_gizmo_operation = ImGuizmo::OPERATION::SCALE;
				m_draw_gizmo = true;
			}
		}
		// Window�̌�n��
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	void GUIGizmo::DrawAndControlGizmo()
	{
		// �J�[�\�������̃E�B���h�E�ɏ���Ă���Ƃ��̓M�Y���̑�����󂯕t���Ȃ�
		const bool is_hovering_other_window =
			ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || !IGUI::Get().IsActiveInputScene();

		constexpr MATRIX reversed_matrix =
			MATRIX(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, -1, 0,
				0, 0, 1, 1
			);
		MATRIX view = CameraManager::GetSceneView();
		MATRIX proj = CameraManager::GetSceneProjection() * reversed_matrix;

		// �M�Y���̕\���̈���Z�b�g����(�K�{)
		auto rect = IWindow::Get().GetScreenRect();
		{
			ImGuizmo::SetRect(
				static_cast<float>(rect.left),
				static_cast<float>(rect.top),
				static_cast<float>(rect.right - rect.left),
				static_cast<float>(rect.bottom - rect.top)
			);
		}

		// �J��������]������M�Y���\��
		//{
		//	const ImVec2 guizmo_size(150.f, 150.f);
		//	ImGuizmo::ViewManipulate(&view._11, 1.0f, ImVec2(10.0f + rect.left, 30.0f + rect.top), guizmo_size, ImU32(0x0f000000u));// Left-Top
		//	// �J�[�\�������̃E�B���h�E�ɏ���Ă��Ȃ��Ƃ��J�����̉�]��K�p
		//	if (is_hovering_other_window == false)
		//	{
		//		CameraManager::SetSceneView(view);
		//	}
		//}

		// �I�u�W�F�N�g�p�M�Y���\��
		{
			// �M�Y����`�悵�Ȃ��Ȃ瑁�����^�[��
			if (!m_draw_gizmo)
				return;

			// �I�����Ă���I�u�W�F�N�g��0�Ȃ瑁�����^�[��
			{
				// �󂯎��z����N���A
				m_selecting_objects.clear();

				// �I�𒆂̃I�u�W�F�N�g���擾
				m_selecting_objects = IGUI::Get().GetSelectionObjects();
				const int selection_cnt = static_cast<int>(m_selecting_objects.size());
				if (selection_cnt == 0)
					return;
			}

			{
				// �I�𒆂̃I�u�W�F�N�g�̐擪���擾
				auto selecting_object = GameObjectManager::GetGameObject(m_selecting_objects.at(0));
				// �I�𒆂̃I�u�W�F�N�g��nullptr�Ȃ烊�^�[��
				if (selecting_object == nullptr)
					return;

				// �I�𒆂̃I�u�W�F�N�g�̐擪�̈ʒu�ɃM�Y�����o��
				auto transform = selecting_object->GetTransform();

				MATRIX world = MATRIX::Identity;
				if (transform)
				{
					world = transform->GetLocalToWorldMatrix();
				}
				MATRIX w_mat = world;
				MATRIX delta_mat = MATRIX::Identity;

				// Scale��Local�ł����ҏW�ł��Ȃ�
				ImGuizmo::MODE mode = m_current_gizmo_mode;
				if (mode == ImGuizmo::MODE::WORLD)
				{
					if (m_current_gizmo_operation == ImGuizmo::OPERATION::SCALE)
					{
						mode = ImGuizmo::MODE::LOCAL;
					}
				}

				// �I�u�W�F�N�g�̃J�����ʒu�Ώۂ̏ꏊ�ɃM�Y�����\�������̂�h��
				if(transform)
				{
					VECTOR3 forward = -VECTOR3(view._13, view._23, view._33);
					VECTOR3 direction = transform->Position() - CameraManager::GetDebugCamera()->GetWorldPosition();
					// Camera��Forward�ƃI�u�W�F�N�g��center�ւ̕����̓��ς��瑁�����^�[��
					if (VECTOR3::Dot(forward, direction.Normalized()) < 0.0f)
					{
						return;
					}
				}

				// �M�Y���̕\���Ƒ���
				ImGuizmo::Manipulate(
					&view._11, &proj._11,
					m_current_gizmo_operation,
					mode,
					&w_mat._11,
					&delta_mat._11,
					/*m_use_snap ? &snap.x : */nullptr,
					/*&b[0].x*/nullptr,
					nullptr
				);

				// Transform��GUI�ő��삳��Ă���Ȃ�
				if (m_is_controlled_with_gui)
				{
					// ���삳��Ă���t���O��true
					m_change_value_matrix = MATRIX::Identity;
					m_has_control_transform = true;
					return;
				}

				// �J�[�\�������̃E�B���h�E�ɏ���Ă��Ȃ����A�M�Y�������삳��Ă�����
				if (!is_hovering_other_window && ImGuizmo::IsUsing())
				{
					// �M�Y�������삳��Ă���t���O��true
					m_has_control_transform = true;

					// �ω��ʂ̍s���ۑ�����
					m_change_value_matrix = delta_mat;
				}
				else
				{
					// �M�Y�������삳��Ă���t���O��false
					m_change_value_matrix = MATRIX::Identity;
					m_has_control_transform = false;
				}
			}
		}
	}

	void GUIGizmo::CallGizmoEvent()
	{
		// Transform��GUI����Œl��ύX�����ꍇ�͂��łɌĂ΂�Ă���
		if (m_is_controlled_with_gui)
		{
			return;
		}
		// �M�Y����SelectState���X�V����
		if (m_has_control_transform)
		{
			switch (m_select_state)
			{
				// �����n��
				case GizmoSelectState::NoSelect:
					m_select_state = GizmoSelectState::Enter;
					break;

					// ��������
				case GizmoSelectState::Enter:
					m_select_state = GizmoSelectState::Stay;
					break;
			}
		}
		else
		{
			switch (m_select_state)
			{
				// �����n��
				case GizmoSelectState::Enter:
				case GizmoSelectState::Stay:
					m_select_state = GizmoSelectState::Exit;
					break;

					// �����Ă����Ԃ�����
				case GizmoSelectState::Exit:
					m_select_state = GizmoSelectState::NoSelect;
					break;
			}
		}
		// �֐����Ăԏ�ԂłȂ��ꍇ�͑������^�[��
		if (m_select_state == GizmoSelectState::NoSelect)
		{
			return;
		}
		// �I�𒆂̃I�u�W�F�N�g�̐��������[�v
		const size_t select_cnt = m_selecting_objects.size();
		for (size_t i = 0; i < select_cnt; ++i)
		{
			// �I�u�W�F�N�g���擾
			const auto object = GameObjectManager::GetGameObject(m_selecting_objects.at(i));
			if (object == nullptr)
				continue;

			// �I�u�W�F�N�g�̎��R���|�[�l���g���X�g���擾
			std::vector<std::weak_ptr<Component>> cmp_vec;
			object->GetComponents(cmp_vec);

			// �R���|�[�l���g�̐���0�ȉ��Ȃ瑁�����^�[��
			const size_t component_cnt = cmp_vec.size();
			if (component_cnt <= 0)
			{
				continue;
			}

			// �R���|�[�l���g���ƂɌ��݂̏�ԂɑΉ������֐����Ă�
			for (size_t cmp_i = 0; cmp_i < component_cnt; ++cmp_i)
			{
				// shared_ptr���擾
				const auto s_cmp = cmp_vec.at(cmp_i).lock();

				// ���݊m�F�Ƒ������^�[��
				if (s_cmp == nullptr)
				{
					continue;
				}

				// ��ԂɑΉ������֐����Ă�
				switch (m_select_state)
				{
					case GizmoSelectState::Enter:
						s_cmp->OnGizmoEnter();
						break;

					case GizmoSelectState::Stay:
						s_cmp->OnGizmoStay();
						break;

					case GizmoSelectState::Exit:
						s_cmp->OnGizmoExit();
						break;
				}
			}
		}
	}

	void GUIGizmo::SelectGizmoOperation()
	{
		// �V���[�g�J�b�g�ɂ��I��
		// �}�E�X���͂��������ꍇ�͍X�V���Ȃ�
		if (ImGui::IsAnyMouseDown())
			return;

		// ImGui�̂����ꂩ��Window���I�𒆂Ȃ�V�[���ɓ��͂���Ȃ�
		if (!IGUI::Get().IsActiveInputScene())
			return;

		// �V���[�g�J�b�g�X�V
		if (ImGui::IsKeyPressed(static_cast<int>('Q')))
			m_draw_gizmo = !m_draw_gizmo;
		if (ImGui::IsKeyPressed(static_cast<int>('W')))
		{
			m_current_gizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
			m_draw_gizmo = true;
		}
		if (ImGui::IsKeyPressed(static_cast<int>('E')))
		{
			m_current_gizmo_operation = ImGuizmo::OPERATION::ROTATE;
			m_draw_gizmo = true;
		}
		if (ImGui::IsKeyPressed(static_cast<int>('R')))
		{
			m_current_gizmo_operation = ImGuizmo::OPERATION::SCALE;
			m_draw_gizmo = true;
		}
	}

	void GUIGizmo::ApplyRelativeMatrix()
	{
		// �v�f������̎��������^�[��
		if (m_selecting_objects.empty())
			return;

		// �M�Y���ɂ��ύX���������Ȃ�V�����s����Z�b�g����
		if (m_has_control_transform && !m_is_controlled_with_gui && !m_on_exit_control_gui)
		{
			const size_t selecting_object_cnt = m_selecting_objects.size();

			for (size_t i = 0; i < selecting_object_cnt; ++i)
			{
				// �ϊ��̓K�p
				const auto object = GameObjectManager::GetGameObject(m_selecting_objects.at(i));
				if (object == nullptr)
					continue;
				const auto transform = object->GetTransform();
				switch (m_current_gizmo_operation)
				{
					case ImGuizmo::TRANSLATE:
						transform->Translate(VECTOR3(m_change_value_matrix._41, m_change_value_matrix._42, m_change_value_matrix._43), false);
						break;
					case ImGuizmo::ROTATE:
						transform->Rotate(m_change_value_matrix.ToQuaternion(), false);
						break;
					case ImGuizmo::SCALE:
						transform->SetRelativeMatrix(m_change_value_matrix * transform->GetAffineTransform());
						break;
				}
			}
		}
	}
	
}// namespace TKGEngine::GUI::Gizmo
#endif// #ifdef USE_IMGUI