#pragma once

#include <string>
#include <list>


namespace TKGEngine::MyFunc
{
	/// ------------------------------------------
	/// @fn		FetchOpenFileNames
	/// @brief	get file names with dialog box
	/// @return	true : get file name. false : cancel.
	/// 
	/// ------------------------------------------
#ifdef UNICODE
	bool FetchOpenFileNames(std::list<std::string>& filenames, const wchar_t* filters, const wchar_t* title);
#else
	bool FetchOpenFileNames(std::list<std::string>& filenames, const char* filters, const char* title);
#endif // UNICODE
	/// ------------------------------------------
	/// @fn		FetchOpenFileName
	/// @brief	get file name with dialog box
	/// @return	true : get file name. false : cancel.
	/// 
	/// ------------------------------------------
#ifdef UNICODE
	bool FetchOpenFileName(std::string& filename, const wchar_t* filters, const wchar_t* title);
	bool FetchOpenFileName(std::string& filename, const wchar_t* filters, const wchar_t* title, const wchar_t* init_dir);
#else
	bool FetchOpenFileName(std::string& filename, const char* filters, const char* title);
	bool FetchOpenFileName(std::string& filename, const char* filters, const char* title, const wchar_t* init_dir);
#endif // UNICODE

	/// ------------------------------------------
	/// @fn		FetchSaveFileNames
	/// @brief	get file name with dialog box
	/// @return	true : get file name. false : cancel.
	/// 
	/// ------------------------------------------
#ifdef UNICODE
	bool FetchSaveFileName(std::string& filename, const wchar_t* filters, const wchar_t* title);
	bool FetchSaveFileName(std::string& filename, const wchar_t* filters, const wchar_t* title, const wchar_t* init_dir);
#else
	bool FetchSaveFileName(std::string& filename, const char* filters, const char* title);
	bool FetchSaveFileName(std::string& filename, const char* filters, const char* title, const wchar_t* init_dir);
#endif // UNICODE


	/// ------------------------------------------
	/// @fn		SplitFileName
	/// @brief	get file names without extension
	/// @return	file name 
	/// 
	/// ------------------------------------------
	std::string SplitFileName(const std::string& filepath);


	/// ------------------------------------------
	/// @fn		SplitFilePathNoExtension
	/// @brief	get file path no extension
	/// @return	folder path no extension
	/// 
	/// ------------------------------------------
	std::string SplitFilePathNoExtension(const std::string& filepath);


	/// ------------------------------------------
	/// @fn		GetExtension
	/// @brief	return file extension
	/// @param[in] path	file path
	/// @return	extension string
	/// 
	/// ------------------------------------------
	std::string GetExtension(const std::string& path);
	std::wstring GetExtension(const std::wstring& path);


	/// ------------------------------------------
	/// @fn		AlignedPathSlash
	/// @brief	return aligned path
	/// @param[in] path	file path
	/// @return	aligned path
	/// 
	/// ------------------------------------------
	std::string AlignedPathSlash(const std::string& path);
	std::wstring AlignedPathSlash(const std::wstring& path);


}// namespace TKGEngine::MyFunc