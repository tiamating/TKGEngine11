#pragma once

#include "../../inc/TKGEngine_Defined.h"
#include "Utility/inc/myfunc_vector.h"

#include <vector>

#ifdef USE_IMGUI
namespace TKGEngine::GUI::Gizmo
{
	/// ========================================================
	/// @class	GUIGizmo
	/// @brief	GUI���M�Y���Ǘ��p�N���X
	/// 
	/// ========================================================
	class GUIGizmo
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		
		/// <summary>
		/// ImGui_XXX_BeginFrame()����ɌĂ�
		/// </summary>
		void OnFrameBegin();

		/// <summary>
		/// �M�Y���̕`��
		/// </summary>
		void OnFrameRender();

		/// <summary>
		/// �M�Y���ɍs��ꂽ�ύX�̓K�p
		/// (OnFrameRender�Ƃ̊Ԃ�Object�̐e�q�A�s���ύX���鏈�������Ă͂����Ȃ�)
		/// </summary>
		void OnFrameEnd();

		// GUI�ɂ����Transform�ɕύX���^����ꂽ�Ƃ��ɃM�Y���C�x���g�֐����Ăԗp
		void EnterControlTransform();
		void ExitControlTransform();


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private enum
		// ==============================================
		enum class GizmoSelectState
		{
			NoSelect,	// �������Ă��Ȃ����
			Enter,	// �M�Y����I������
			Stay,	// �M�Y����I�����Ă���
			Exit,	// �M�Y�����痣����
		};
		
		// ==============================================
		// private methods
		// ==============================================

		/// <summary>
		/// �M�Y�������I�����邽�߂Ƀ{�^����\������
		/// </summary>
		void SelectGizmoOperationUseButton();

		/// <summary>
		/// �M�Y����\���A���삷��
		/// </summary>
		void DrawAndControlGizmo();

		/// <summary>
		/// OnGizmo�֐����Ă�
		/// </summary>
		void CallGizmoEvent();

		/// <summary>
		/// �M�Y���ő��삷��p�����[�^��I������
		/// </summary>
		void SelectGizmoOperation();

		/// <summary>
		/// �M�Y���ŕϊ��������΍s���K�p����
		/// </summary>
		void ApplyRelativeMatrix();


		// ==============================================
		// private variables
		// ==============================================
		
		// �I�𒆂̃I�u�W�F�N�g���X�g�̈ꎞ�ۑ��z��
		std::vector<GameObjectID> m_selecting_objects;

		// �M�Y���̑I�����
		GizmoSelectState m_select_state = GizmoSelectState::NoSelect;

		// �M�Y���̌��݂̐ݒ�
		ImGuizmo::OPERATION m_current_gizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
		ImGuizmo::MODE m_current_gizmo_mode = ImGuizmo::MODE::WORLD;

		// �M�Y����`�悷�邩�ǂ���
		bool m_draw_gizmo = true;
		// �M�Y����GUI�ɂ���ăg�����X�t�H�[�������삳��Ă��邩
		bool m_has_control_transform = false;
		// Transform��GUI�ɂ���đ��삳��Ă��邩
		bool m_is_controlled_with_gui = false;
		// GUI����𗣂����u��
		bool m_on_exit_control_gui = false;

		// �M�Y���ɂ���ĕύX���ꂽ�ʂ̕ϊ��s��
		MATRIX m_change_value_matrix = MATRIX::Identity;
	};




}// namespace TKGEngine::GUI::Gizmo
#endif// #ifdef USE_IMGUI