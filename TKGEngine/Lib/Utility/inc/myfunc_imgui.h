#pragma once

#include <imgui.h>
#include <imgui_stdlib.h>

#include "Utility/inc/myfunc_string.h"

namespace ImGui
{
	// 段落の幅
	constexpr float INDENT_VALUE = 10.0f;
	// ウィンドウ内で有効な下からの位置
	constexpr float ENABLE_SIZE_FROM_BOTTOM = 20.0f;
	// 基本で使用するスペースのサイズ
	constexpr float DEFAULT_SPACE_SIZE = 20.0f;
	// エラー用フォントカラー
	static const ::ImVec4 ERROR_FONT_COLOR(0.8f, 0.2f, 0.2f, 1.0f);

	/// <summary>
	/// PushIDとPopIDのヘルパークラス
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
	/// GUIのインデントヘルパークラス
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
	/// EnumをComboとして表示する
	/// </summary>
	template <typename Ty, Ty MAX_ELEMENT>
	bool ComboEnum(const char* label, Ty* current_item);

	/// <summary>
	/// 文字列配列をComboとして表示する
	/// </summary>
	bool ComboString(const char* label, int* current_item, const std::vector<std::string>& items);

	/// <summary>
	/// マウスを画面の端で止まることなく動かせるようにしたDragFloat
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
	/// ボタンの色を設定可能なButton
	/// </summary>
	/// <param name="label">ボタンラベル</param>
	/// <param name="color">表示色</param>
	/// <param name="hovered_color">マウスオーバー時の色</param>
	/// <param name="active_color">マウスクリック時の色</param>
	/// <param name="size">ボタンサイズ</param>
	/// <returns>ボタンを押したときtrueを返す</returns>
	bool ButtonColorSettable(
		const char* label,
		const ImVec4& color,
		const ImVec4& hovered_color,
		const ImVec4& active_color,
		const ImVec2& size = ImVec2(0, 0)
	);

	/// <summary>
	/// ヘルプ用文字列表示
	/// </summary>
	/// <param name="desc">ヘルプ詳細文</param>
	void HelpText(const char* desc);
	void HelpMarker(const char* desc);

	/// <summary>
	/// マウスのいずれかのボタンを押したか
	/// </summary>
	bool IsAnyMouseClicked();

	/// <summary>
	/// マウスのいずれかのボタンを離したか
	/// </summary>
	bool IsAnyMouseReleased();

	/// <summary>
	/// 次のアイテムを同じ行に右詰めで配置する
	/// </summary>
	/// <param name="occupancy_ratio">右端からのウィンドウ占有割合</param>
	void AlignedSameLine(float occupancy_ratio);

	/// <summary>
	/// テクスチャを表示するウィンドウとそのボタン
	/// </summary>
	/// <param name="user_texture_id">テクスチャID</param>
	/// <param name="unique_id">識別ID</param>
	/// <param name="texture_size">テクスチャサイズ</param>
	/// <param name="button_size">ボタンサイズ</param>
	/// <param name="uv0">表示する左上UV</param>
	/// <param name="uv1">表示する右下UV</param>
	void ImageWindowButton(ImTextureID user_texture_id, void* unique_id, const ImVec2& texture_size, const ImVec2& button_size, const ImVec2& uv0 = ImVec2(0.0f, 0.0f), const ImVec2& uv1 = ImVec2(1.0f, 1.0f));


	////////////////////////////////////////////////////////
	// Template definition
	////////////////////////////////////////////////////////
	template<typename Ty, Ty MAX_ELEMENT>
	bool ComboEnum(const char* label, Ty* current_item)
	{
		// 現在のアイテムをintに変換する
		int current_value = static_cast<int>(*current_item);
		if (ComboString(label, &current_value, TKGEngine::MyFunc::EnumNameList<Ty, MAX_ELEMENT>::Get()))
		{
			// 選択されたintを変換してアイテムに代入
			*current_item = static_cast<Ty>(current_value);
			return true;
		}
		return false;
	}

}// namespace ImGui
