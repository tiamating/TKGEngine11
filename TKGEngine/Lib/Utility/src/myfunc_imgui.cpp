
#include "../inc/myfunc_imgui.h"
#include "Systems/inc/IWindow.h"
#include "Utility/inc/myfunc_math.h"

#include <Windows.h>

namespace ImGui
{
	bool ComboString(const char* label, int* current_item, const std::vector<std::string>& items)
	{
		const int item_size = static_cast<int>(items.size());
		// �v�f���Ȃ���Α������^�[��
		if (item_size <= 0) return false;
		// Combo�\��
		const int init_value = *current_item;
		bool is_selected = false;
		// Combo���J���Ă��Ȃ���΃��^�[��
		if (!BeginCombo(label, items.at(init_value).c_str()))
			return is_selected;
		for (int i = 0; i < item_size; ++i)
		{
			IDWrapped combo_id(i);
			if (Selectable(items.at(i).c_str(), i == init_value))
			{
				// �I�����ꂽ��l���㏑��
				*current_item = i;
				is_selected = true;
			}
		}
		EndCombo();
		// �I�����ꂽ��true��Ԃ�
		return is_selected;
	}

	bool DragFloatNoRange(
		const char* label,
		float* v,
		float v_speed,
		float v_min,
		float v_max,
		const char* format,
		ImGuiSliderFlags flags
	)
	{
		assert(0);

		// �N���b�N�ʒu�ƃN���b�N��Ԃ�ۂ�
		static ImVec2 click_pos(0, 0);
		static bool is_pushing = false;
		static bool is_clicked = false;

		bool ret_val = ImGui::DragFloat(label, v, v_speed, v_min, v_max, format, flags);
		// �A�C�e�����I������Ă��Ȃ����A���N���b�N�𗣂����瑀���Ԃ���������
		if (!ImGui::IsItemActive() || ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			is_pushing = false;
			is_clicked = false;
		}
		// �A�C�e����ō��N���b�N����Ă����瑀���Ԃɂ���
		else if (ImGui::IsItemActive() && ImGui::IsMouseClicked(ImGuiMouseButton_Left, true))
		{
			is_pushing = true;
		}

		if (is_pushing)
		{
			// �N���b�N����Ă�����}�E�X�ʒu���N���b�N�ʒu�Ɉړ�����
			if (is_clicked)
			{
				//POINT p = { static_cast<int>(click_pos.x), static_cast<int>(click_pos.y) };
				//::SetCursorPos(p.x, p.y);
				ImGui::GetIO().MousePos.x = ImGui::GetIO().MousePos.x - ImGui::GetIO().MouseDelta.x;
				ImGui::GetIO().WantSetMousePos = true;
			}
			// ���߂ăN���b�N���ꂽ��N���b�N�ʒu��ۑ�����
			else
			{
				is_clicked = true;
				click_pos = ImGui::GetMousePos();
				ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
			}
		}
		else
		{
			ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NavEnableSetMousePos;
		}
		ImGui::Text("xyz"); ImGui::SameLine(); ImGui::Button("aaaaaaaa");
		return ret_val;
	}

	bool ButtonColorSettable(
		const char* label,
		const ImVec4& color,
		const ImVec4& hovered_color,
		const ImVec4& active_color,
		const ImVec2& size
	)
	{
		bool is_pushed = false;

		// �F�̐ݒ�
		ImGui::PushStyleColor(ImGuiCol_Button, color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hovered_color);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, active_color);

		is_pushed = ImGui::Button(label, size);

		// �F��߂�
		ImGui::PopStyleColor(3);

		return is_pushed;
	}

	void HelpText(const char* desc)
	{
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	void HelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	bool IsAnyMouseClicked()
	{
		return ImGui::IsMouseClicked(ImGuiMouseButton_Left) ||
			ImGui::IsMouseClicked(ImGuiMouseButton_Middle) ||
			ImGui::IsMouseClicked(ImGuiMouseButton_Right);
	}

	bool IsAnyMouseReleased()
	{
		return ImGui::IsMouseReleased(ImGuiMouseButton_Left) ||
			ImGui::IsMouseReleased(ImGuiMouseButton_Middle) ||
			ImGui::IsMouseReleased(ImGuiMouseButton_Right);
	}

	void AlignedSameLine(float occupancy_ratio)
	{
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvailWidth() - ImGui::GetWindowWidth() * occupancy_ratio, 0.f));
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvailWidth());
	}

	void ImageWindowButton(ImTextureID user_texture_id, void* unique_id, const ImVec2& texture_size, const ImVec2& button_size, const ImVec2& uv0, const ImVec2& uv1)
	{
		// �ʃE�B���h�E�\���p�ϐ�
		static bool draw_popup = false;
		static const void* this_id = nullptr;
		static ImVec4 image_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		static float rate_of_magnification = 1.0f;	// �e�N�X�`���̊g�嗦

		const float button_width = ImGui::GetWindowWidth() * 0.3f;
		if (ImGui::ImageButton(user_texture_id, ImVec2(button_width, button_width), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(1, 1, 1, 1)))
		{
			if (this_id != unique_id)
			{
				draw_popup = true;
				this_id = unique_id;
			}
			else
			{
				draw_popup = !draw_popup;
				this_id = draw_popup ? unique_id : nullptr;
			}
			image_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			rate_of_magnification = 1.0f;
		}
		// �|�b�v�A�b�v�t���O��true��ID�������Ȃ�E�B���h�E�\��
		if (this_id != nullptr && this_id == unique_id)
		{
			const ImVec2 _button_size = ImVec2(40.0f, 20.0f);
			// �e�N�X�`���T�C�Y
			constexpr float max_draw_texture_size = 512.0f;
			// �ő�T�C�Y�ɃN�����v����
			const ImVec2 text_size =
				ImVec2(TKGEngine::MyMath::Min(max_draw_texture_size, texture_size.x), TKGEngine::MyMath::Min(max_draw_texture_size, texture_size.y));
			// �g�嗦�X���C�_�[�̕�
			const float slider_size = ImGui::GetStyle().ItemSpacing.y * 2.0f + ImGui::GetStyle().FramePadding.y * 2.0f + ImGui::GetFontSize();
			// �����̃E�B���h�E�T�C�Y
			const ImVec2 window_size = ImVec2(
				text_size.x + ImGui::GetStyle().WindowPadding.x * 2.0f + ImGui::GetStyle().FramePadding.x * 2.0f + ImGui::GetStyle().ItemSpacing.x,
				text_size.y + ImGui::GetStyle().WindowPadding.y * 2.0f + ImGui::GetStyle().FramePadding.y * 2.0f + ImGui::GetStyle().ItemSpacing.y * 2.0f + slider_size + _button_size.y
			);
			ImGui::SetNextWindowSize(window_size, ImGuiCond_Appearing);
			if (ImGui::Begin("Texture Window", &draw_popup, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_HorizontalScrollbar))
			{
				const float space_size = ImGui::GetCursorPosY();
				// �g�嗦
				{
					ImGui::SliderFloat("##Magnification", &rate_of_magnification, 0.1f, 10.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
				}
				// RGBA�{�^��
				// R
				if (image_color.x > 0.0f)
				{
					if (ImGui::ColorButton("R", ImVec4(0.8f, 0.f, 0.f, 1.0f), ImGuiColorEditFlags_NoTooltip, _button_size))
						image_color.x = 0.0f;
				}
				else
				{
					if (ImGui::Button("R", _button_size))
						image_color.x = 1.0f;
				}
				ImGui::SameLine();
				// G
				if (image_color.y > 0.0f)
				{
					if (ImGui::ColorButton("G", ImVec4(0.f, 0.8f, 0.f, 1.0f), ImGuiColorEditFlags_NoTooltip, _button_size))
						image_color.y = 0.0f;
				}
				else
				{
					if (ImGui::Button("G", _button_size))
						image_color.y = 1.0f;
				}
				ImGui::SameLine();
				// B
				if (image_color.z > 0.0f)
				{
					if (ImGui::ColorButton("B", ImVec4(0.f, 0.f, 0.8f, 1.0f), ImGuiColorEditFlags_NoTooltip, _button_size))
						image_color.z = 0.0f;
				}
				else
				{
					if (ImGui::Button("B", _button_size))
						image_color.z = 1.0f;
				}
				ImGui::SameLine();
				// A
				if (image_color.w > 0.0f)
				{
					if (ImGui::ColorButton("A", ImVec4(0.4f, 0.4f, 0.4f, 1.0f), ImGuiColorEditFlags_NoTooltip, _button_size))
						image_color.w = 0.0f;
				}
				else
				{
					if (ImGui::Button("A", _button_size))
						image_color.w = 1.0f;
				}
				ImGui::SetCursorPosY(space_size + _button_size.y + slider_size);
				// Image
				ImGui::Image(
					user_texture_id,
					ImVec2(
						text_size.x * rate_of_magnification - ImGui::GetStyle().FramePadding.x * 2.0f - ImGui::GetStyle().WindowPadding.x * 2.0f,
						text_size.y * rate_of_magnification - ImGui::GetStyle().FramePadding.y * 2.0f - ImGui::GetStyle().WindowPadding.y * 2.0f),
					ImVec2(0, 0), ImVec2(1, 1), image_color
				);
			}
			ImGui::End();
		}
		// �e�N�X�`���E�B���h�E������ꂽ��\�����Ă����������������
		if (!draw_popup)
		{
			this_id = nullptr;
			image_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			rate_of_magnification = 1.0f;
		}
	}


}// namespace ImGui