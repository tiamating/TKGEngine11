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
		// 毎フレーム開始時に呼ぶ
		ImGuizmo::BeginFrame();
	}

	void GUIGizmo::OnFrameRender()
	{
		// ImGui::Buttonによる操作パラメータ選択
		SelectGizmoOperationUseButton();
		// ショートカットによるギズモの操作パラメータ選択
		SelectGizmoOperation();

		// ギズモの表示と操作
		DrawAndControlGizmo();

		// ギズモのイベント関数を呼ぶ
		CallGizmoEvent();
	}

	void GUIGizmo::OnFrameEnd()
	{
		// ギズモが操作されていれば新しい変換行列をセットする
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
		// Transform::GUIの操作を離した瞬間を取る
		if (m_is_controlled_with_gui)
			m_on_exit_control_gui = true;
		else
			m_on_exit_control_gui = false;

		m_is_controlled_with_gui = false;
	}

	void GUIGizmo::SelectGizmoOperationUseButton()
	{
		// 表示する位置を決める
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

		// Window colorの設定
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

		// Windowの表示
		ImGui::Begin("Operation Select Window", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking);
		{
			// メインメニューバーによる選択
			// Local / World
			if (ImGui::Button(m_current_gizmo_mode == ImGuizmo::MODE::LOCAL ? "Local" : "World", ImVec2(64.f, 0.f)))
			{
				m_current_gizmo_mode =
					(m_current_gizmo_mode == ImGuizmo::MODE::LOCAL) ?
					ImGuizmo::MODE::WORLD :
					ImGuizmo::MODE::LOCAL;
			}
			ImGui::SameLine();
			// 表示非表示
			if (ImGui::RadioButton("Visible", m_draw_gizmo))
			{
				m_draw_gizmo = !m_draw_gizmo;
			}
			ImGui::SameLine();

			// ギズモのオペレーションと表示ボタンが一致したら色を変えたボタンを表示する
			auto gizmo_button_func = [&](const char* label, ImGuizmo::OPERATION operation)->bool
			{
				bool is_pushed = false;

				// 一致するか調べて
				if (m_current_gizmo_operation == operation)
				{
					// 選択中のボタンはボタン色をセット
					const ImVec4 button_color = ImVec4(0.7f, 0.3f, 0.3f, 1.0f);
					const ImVec4 hovered_color = ImVec4(0.5f, 0.2f, 0.2f, 1.0f);
					const ImVec4 active_color = ImVec4(0.35f, 0.2f, 0.2f, 1.0f);
					is_pushed = ImGui::ButtonColorSettable(label, button_color, hovered_color, active_color);
				}
				else
				{
					// 非選択中のボタン表示
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
		// Windowの後始末
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	void GUIGizmo::DrawAndControlGizmo()
	{
		// カーソルが他のウィンドウに乗っているときはギズモの操作を受け付けない
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

		// ギズモの表示領域をセットする(必須)
		auto rect = IWindow::Get().GetScreenRect();
		{
			ImGuizmo::SetRect(
				static_cast<float>(rect.left),
				static_cast<float>(rect.top),
				static_cast<float>(rect.right - rect.left),
				static_cast<float>(rect.bottom - rect.top)
			);
		}

		// カメラを回転させるギズモ表示
		//{
		//	const ImVec2 guizmo_size(150.f, 150.f);
		//	ImGuizmo::ViewManipulate(&view._11, 1.0f, ImVec2(10.0f + rect.left, 30.0f + rect.top), guizmo_size, ImU32(0x0f000000u));// Left-Top
		//	// カーソルが他のウィンドウに乗っていないときカメラの回転を適用
		//	if (is_hovering_other_window == false)
		//	{
		//		CameraManager::SetSceneView(view);
		//	}
		//}

		// オブジェクト用ギズモ表示
		{
			// ギズモを描画しないなら早期リターン
			if (!m_draw_gizmo)
				return;

			// 選択しているオブジェクトが0個なら早期リターン
			{
				// 受け取る配列をクリア
				m_selecting_objects.clear();

				// 選択中のオブジェクトを取得
				m_selecting_objects = IGUI::Get().GetSelectionObjects();
				const int selection_cnt = static_cast<int>(m_selecting_objects.size());
				if (selection_cnt == 0)
					return;
			}

			{
				// 選択中のオブジェクトの先頭を取得
				auto selecting_object = GameObjectManager::GetGameObject(m_selecting_objects.at(0));
				// 選択中のオブジェクトがnullptrならリターン
				if (selecting_object == nullptr)
					return;

				// 選択中のオブジェクトの先頭の位置にギズモを出す
				auto transform = selecting_object->GetTransform();

				MATRIX world = MATRIX::Identity;
				if (transform)
				{
					world = transform->GetLocalToWorldMatrix();
				}
				MATRIX w_mat = world;
				MATRIX delta_mat = MATRIX::Identity;

				// ScaleはLocalでしか編集できない
				ImGuizmo::MODE mode = m_current_gizmo_mode;
				if (mode == ImGuizmo::MODE::WORLD)
				{
					if (m_current_gizmo_operation == ImGuizmo::OPERATION::SCALE)
					{
						mode = ImGuizmo::MODE::LOCAL;
					}
				}

				// オブジェクトのカメラ位置対象の場所にギズモが表示されるのを防ぐ
				if(transform)
				{
					VECTOR3 forward = -VECTOR3(view._13, view._23, view._33);
					VECTOR3 direction = transform->Position() - CameraManager::GetDebugCamera()->GetWorldPosition();
					// CameraのForwardとオブジェクトのcenterへの方向の内積から早期リターン
					if (VECTOR3::Dot(forward, direction.Normalized()) < 0.0f)
					{
						return;
					}
				}

				// ギズモの表示と操作
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

				// TransformのGUIで操作されているなら
				if (m_is_controlled_with_gui)
				{
					// 操作されているフラグをtrue
					m_change_value_matrix = MATRIX::Identity;
					m_has_control_transform = true;
					return;
				}

				// カーソルが他のウィンドウに乗っていないかつ、ギズモが操作されていたら
				if (!is_hovering_other_window && ImGuizmo::IsUsing())
				{
					// ギズモが操作されているフラグをtrue
					m_has_control_transform = true;

					// 変化量の行列を保存する
					m_change_value_matrix = delta_mat;
				}
				else
				{
					// ギズモが操作されているフラグをfalse
					m_change_value_matrix = MATRIX::Identity;
					m_has_control_transform = false;
				}
			}
		}
	}

	void GUIGizmo::CallGizmoEvent()
	{
		// TransformのGUI操作で値を変更した場合はすでに呼ばれている
		if (m_is_controlled_with_gui)
		{
			return;
		}
		// ギズモのSelectStateを更新する
		if (m_has_control_transform)
		{
			switch (m_select_state)
			{
				// 押し始め
				case GizmoSelectState::NoSelect:
					m_select_state = GizmoSelectState::Enter;
					break;

					// 押し続け
				case GizmoSelectState::Enter:
					m_select_state = GizmoSelectState::Stay;
					break;
			}
		}
		else
		{
			switch (m_select_state)
			{
				// 離し始め
				case GizmoSelectState::Enter:
				case GizmoSelectState::Stay:
					m_select_state = GizmoSelectState::Exit;
					break;

					// 離している状態が続く
				case GizmoSelectState::Exit:
					m_select_state = GizmoSelectState::NoSelect;
					break;
			}
		}
		// 関数を呼ぶ状態でない場合は早期リターン
		if (m_select_state == GizmoSelectState::NoSelect)
		{
			return;
		}
		// 選択中のオブジェクトの数だけループ
		const size_t select_cnt = m_selecting_objects.size();
		for (size_t i = 0; i < select_cnt; ++i)
		{
			// オブジェクトを取得
			const auto object = GameObjectManager::GetGameObject(m_selecting_objects.at(i));
			if (object == nullptr)
				continue;

			// オブジェクトの持つコンポーネントリストを取得
			std::vector<std::weak_ptr<Component>> cmp_vec;
			object->GetComponents(cmp_vec);

			// コンポーネントの数が0以下なら早期リターン
			const size_t component_cnt = cmp_vec.size();
			if (component_cnt <= 0)
			{
				continue;
			}

			// コンポーネントごとに現在の状態に対応した関数を呼ぶ
			for (size_t cmp_i = 0; cmp_i < component_cnt; ++cmp_i)
			{
				// shared_ptrを取得
				const auto s_cmp = cmp_vec.at(cmp_i).lock();

				// 存在確認と早期リターン
				if (s_cmp == nullptr)
				{
					continue;
				}

				// 状態に対応した関数を呼ぶ
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
		// ショートカットによる選択
		// マウス入力があった場合は更新しない
		if (ImGui::IsAnyMouseDown())
			return;

		// ImGuiのいずれかのWindowが選択中ならシーンに入力されない
		if (!IGUI::Get().IsActiveInputScene())
			return;

		// ショートカット更新
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
		// 要素数が空の時早期リターン
		if (m_selecting_objects.empty())
			return;

		// ギズモによる変更があったなら新しい行列をセットする
		if (m_has_control_transform && !m_is_controlled_with_gui && !m_on_exit_control_gui)
		{
			const size_t selecting_object_cnt = m_selecting_objects.size();

			for (size_t i = 0; i < selecting_object_cnt; ++i)
			{
				// 変換の適用
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