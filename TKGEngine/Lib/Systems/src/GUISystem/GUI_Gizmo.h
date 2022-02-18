#pragma once

#include "../../inc/TKGEngine_Defined.h"
#include "Utility/inc/myfunc_vector.h"

#include <vector>

#ifdef USE_IMGUI
namespace TKGEngine::GUI::Gizmo
{
	/// ========================================================
	/// @class	GUIGizmo
	/// @brief	GUI内ギズモ管理用クラス
	/// 
	/// ========================================================
	class GUIGizmo
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		
		/// <summary>
		/// ImGui_XXX_BeginFrame()直後に呼ぶ
		/// </summary>
		void OnFrameBegin();

		/// <summary>
		/// ギズモの描画
		/// </summary>
		void OnFrameRender();

		/// <summary>
		/// ギズモに行われた変更の適用
		/// (OnFrameRenderとの間にObjectの親子、行列を変更する処理を入れてはいけない)
		/// </summary>
		void OnFrameEnd();

		// GUIによってTransformに変更が与えられたときにギズモイベント関数を呼ぶ用
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
			NoSelect,	// 何もしていない状態
			Enter,	// ギズモを選択した
			Stay,	// ギズモを選択している
			Exit,	// ギズモから離した
		};
		
		// ==============================================
		// private methods
		// ==============================================

		/// <summary>
		/// ギズモ操作を選択するためにボタンを表示する
		/// </summary>
		void SelectGizmoOperationUseButton();

		/// <summary>
		/// ギズモを表示、操作する
		/// </summary>
		void DrawAndControlGizmo();

		/// <summary>
		/// OnGizmo関数を呼ぶ
		/// </summary>
		void CallGizmoEvent();

		/// <summary>
		/// ギズモで操作するパラメータを選択する
		/// </summary>
		void SelectGizmoOperation();

		/// <summary>
		/// ギズモで変換した相対行列を適用する
		/// </summary>
		void ApplyRelativeMatrix();


		// ==============================================
		// private variables
		// ==============================================
		
		// 選択中のオブジェクトリストの一時保存配列
		std::vector<GameObjectID> m_selecting_objects;

		// ギズモの選択状態
		GizmoSelectState m_select_state = GizmoSelectState::NoSelect;

		// ギズモの現在の設定
		ImGuizmo::OPERATION m_current_gizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
		ImGuizmo::MODE m_current_gizmo_mode = ImGuizmo::MODE::WORLD;

		// ギズモを描画するかどうか
		bool m_draw_gizmo = true;
		// ギズモかGUIによってトランスフォームが操作されているか
		bool m_has_control_transform = false;
		// TransformがGUIによって操作されているか
		bool m_is_controlled_with_gui = false;
		// GUI操作を離した瞬間
		bool m_on_exit_control_gui = false;

		// ギズモによって変更された量の変換行列
		MATRIX m_change_value_matrix = MATRIX::Identity;
	};




}// namespace TKGEngine::GUI::Gizmo
#endif// #ifdef USE_IMGUI