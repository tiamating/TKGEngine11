#pragma once

#include <imgui.h>
#include <imgui_stdlib.h>

#include "Utility/inc/myfunc_string.h"

namespace ImGui
{
	// �i���̕�
	constexpr float INDENT_VALUE = 10.0f;
	// �E�B���h�E���ŗL���ȉ�����̈ʒu
	constexpr float ENABLE_SIZE_FROM_BOTTOM = 20.0f;
	// ��{�Ŏg�p����X�y�[�X�̃T�C�Y
	constexpr float DEFAULT_SPACE_SIZE = 20.0f;
	// �G���[�p�t�H���g�J���[
	static const ::ImVec4 ERROR_FONT_COLOR(0.8f, 0.2f, 0.2f, 1.0f);

	/// <summary>
	/// PushID��PopID�̃w���p�[�N���X
	/// </summary>
	struct IDWrapped
	{
		// Constructor
		IDWrapped(const int int_id)
		{
			PushID(int_id);
		}
		explicit IDWrapped(const std::string& str_id)
		{
			PushID(str_id.c_str());
		}
		IDWrapped(const char* str_id)
		{
			PushID(str_id);
		}
		IDWrapped(const void* ptr_id)
		{
			PushID(ptr_id);
		}
		// Destructor
		~IDWrapped()
		{
			PopID();
		}
	};

	/// <summary>
	/// GUI�̃C���f���g�w���p�[�N���X
	/// </summary>
	struct IndentWrapped
	{
		// Constructor
		IndentWrapped(const float indent_value)
			: indent_value(indent_value)
		{
			Indent(indent_value);
		}
		// Destructor
		~IndentWrapped()
		{
			Unindent(indent_value);
		}
	private:
		float indent_value = 0.0f;
	};

	/// <summary>
	/// Enum��Combo�Ƃ��ĕ\������
	/// </summary>
	template <typename Ty, Ty MAX_ELEMENT>
	bool ComboEnum(const char* label, Ty* current_item);

	/// <summary>
	/// ������z���Combo�Ƃ��ĕ\������
	/// </summary>
	bool ComboString(const char* label, int* current_item, const std::vector<std::string>& items);

	/// <summary>
	/// �}�E�X����ʂ̒[�Ŏ~�܂邱�ƂȂ���������悤�ɂ���DragFloat
	/// </summary>
	bool DragFloatNoRange(
		const char* label,
		float* v,
		float v_speed,
		float v_min = 0.0f,
		float v_max = 0.0f,
		const char* format = "%.3f",
		ImGuiSliderFlags flags = 0
	);

	/// <summary>
	/// �{�^���̐F��ݒ�\��Button
	/// </summary>
	/// <param name="label">�{�^�����x��</param>
	/// <param name="color">�\���F</param>
	/// <param name="hovered_color">�}�E�X�I�[�o�[���̐F</param>
	/// <param name="active_color">�}�E�X�N���b�N���̐F</param>
	/// <param name="size">�{�^���T�C�Y</param>
	/// <returns>�{�^�����������Ƃ�true��Ԃ�</returns>
	bool ButtonColorSettable(
		const char* label,
		const ImVec4& color,
		const ImVec4& hovered_color,
		const ImVec4& active_color,
		const ImVec2& size = ImVec2(0, 0)
	);

	/// <summary>
	/// �w���v�p������\��
	/// </summary>
	/// <param name="desc">�w���v�ڍו�</param>
	void HelpText(const char* desc);
	void HelpMarker(const char* desc);

	/// <summary>
	/// �}�E�X�̂����ꂩ�̃{�^������������
	/// </summary>
	bool IsAnyMouseClicked();

	/// <summary>
	/// �}�E�X�̂����ꂩ�̃{�^���𗣂�����
	/// </summary>
	bool IsAnyMouseReleased();

	/// <summary>
	/// ���̃A�C�e���𓯂��s�ɉE�l�߂Ŕz�u����
	/// </summary>
	/// <param name="occupancy_ratio">�E�[����̃E�B���h�E��L����</param>
	void AlignedSameLine(float occupancy_ratio);

	/// <summary>
	/// �e�N�X�`����\������E�B���h�E�Ƃ��̃{�^��
	/// </summary>
	/// <param name="user_texture_id">�e�N�X�`��ID</param>
	/// <param name="unique_id">����ID</param>
	/// <param name="texture_size">�e�N�X�`���T�C�Y</param>
	/// <param name="button_size">�{�^���T�C�Y</param>
	/// <param name="uv0">�\�����鍶��UV</param>
	/// <param name="uv1">�\������E��UV</param>
	void ImageWindowButton(ImTextureID user_texture_id, void* unique_id, const ImVec2& texture_size, const ImVec2& button_size, const ImVec2& uv0 = ImVec2(0.0f, 0.0f), const ImVec2& uv1 = ImVec2(1.0f, 1.0f));


	////////////////////////////////////////////////////////
	// Template definition
	////////////////////////////////////////////////////////
	template<typename Ty, Ty MAX_ELEMENT>
	bool ComboEnum(const char* label, Ty* current_item)
	{
		// ���݂̃A�C�e����int�ɕϊ�����
		int current_value = static_cast<int>(*current_item);
		if (ComboString(label, &current_value, TKGEngine::MyFunc::EnumNameList<Ty, MAX_ELEMENT>::Get()))
		{
			// �I�����ꂽint��ϊ����ăA�C�e���ɑ��
			*current_item = static_cast<Ty>(current_value);
			return true;
		}
		return false;
	}

}// namespace ImGui
