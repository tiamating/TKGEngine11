#pragma once

#include <string>
#include <vector>
#include <utility>
#include <Windows.h>

namespace TKGEngine::MyFunc
{
	/// ------------------------------------------
	/// @fn		convertStringToWstring
	/// @brief	string convert to the wstring
	/// @param[in] str	string to convert
	/// @return	converted wstring from the string
	/// 
	/// ------------------------------------------
	inline std::wstring ConvertStringToWstring(const std::string& str);
	/// ------------------------------------------
	/// @fn		convertWstringToString
	/// @brief	wstring convert to the string
	/// @param[in] str	wstring to convert
	/// @return	converted string from the wstring
	/// 
	/// ------------------------------------------
	inline std::string ConvertWstringToString(const std::wstring& wstr);

	/// ------------------------------------------
	/// @fn		splitFileName
	/// @brief	separate file name with set character from front
	/// @param[in] path	file name to separate
	/// @param[in] splitter	character to separate
	/// @return	before and after the splitter
	/// 
	/// ------------------------------------------
	std::pair<std::string, std::string> SplitFileName(const std::string& path, const char* splitter);

	/// ------------------------------------------
	/// @fn		SplitString
	/// @brief	split string use separator
	/// 
	/// ------------------------------------------
	std::vector<std::string> SplitString(const std::string& str, char sep);

	/// <summary>
	/// 文字列を全て小文字に変換する
	/// </summary>
	/// <param name="str">変換する文字列</param>
	void ToLower(std::string& str);

	/// <summary>
	/// 文字列を全て大文字に変換する
	/// </summary>
	/// <param name="str">変換する文字列</param>
	void ToUpper(std::string& str);


	/// <summary>
	/// 0から順に並んでいる列挙子をMAX_SIZEまで文字列に変換する
	/// </summary>
	/// <typeparam name="Ty"></typeparam>
	template <typename Ty, Ty MAX_ELEMENT>
	class EnumNameList
	{
	public:
		/// <summary>
		/// 文字列リスト取得
		/// </summary>
		/// <returns>文字列リスト</returns>
		static const std::vector<std::string>& Get();

	private:
		static std::vector<std::string> m_name_list;	// 管理している文字列リスト
		static bool m_has_list;	// 文字列リストを保持しているか
	};


	///////////////////////////////////////////////////////////////////
	//
	//					 inline
	//
	///////////////////////////////////////////////////////////////////
	std::wstring ConvertStringToWstring(const std::string& str)
	{
		const int in_size = static_cast<int>(str.size());
		const int out_size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), in_size, 0, 0);
		std::wstring wstr(out_size, L'\0');
		if (out_size > 0)
		{
			MultiByteToWideChar(CP_ACP, 0, str.c_str(), in_size, &wstr[0], out_size);
		}
		return wstr;
	}

	std::string ConvertWstringToString(const std::wstring& wstr)
	{
		const int in_size = static_cast<int>(wstr.size());
		const int out_size = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), in_size, 0, 0, 0, 0);
		std::string str(out_size, '\0');
		if (out_size > 0)
		{
			WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), in_size, &str[0], out_size, 0, 0);
		}
		return str;
	}


	////////////////////////////////////////////////////////
	// Template definition
	////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////
	// EnumNameList
	////////////////////////////////////////////////////////
	template<typename Ty, Ty MAX_ELEMENT>
	std::vector<std::string> EnumNameList<Ty, MAX_ELEMENT>::m_name_list(static_cast<int>(MAX_ELEMENT));
	template<typename Ty, Ty MAX_ELEMENT>
	bool EnumNameList<Ty, MAX_ELEMENT>::m_has_list = false;

	template<typename Ty, Ty MAX_ELEMENT>
	inline const std::vector<std::string>& EnumNameList<Ty, MAX_ELEMENT>::Get()
	{
		// 名前取得済みか
		if (!m_has_list)
		{
			constexpr int max_num = static_cast<int>(MAX_ELEMENT);
			for (int i = 0; i < max_num; ++i)
			{
				// enumを文字列に変換してリストに追加
				m_name_list.at(i) = (NAMEOF_ENUM(static_cast<Ty>(i)));
			}
			// 取得済み
			m_has_list = true;
		}
		return m_name_list;
	}
	////////////////////////////////////////////////////////

}	// namespace TKGEngine::MyFunc