
#include "../inc/myfunc_file.h"

#include "../inc/myfunc_string.h"

#include <filesystem>
#include <algorithm>
#include <commdlg.h>

namespace TKGEngine::MyFunc
{
#ifdef UNICODE
	bool FetchOpenFileNames(std::list<std::string>& filenames, const wchar_t* filters, const wchar_t* title)
#else
	bool FetchOpenFileNames(std::list<std::string>& filenames, const char* filters, const char* title)
#endif // UNICODE
	{
		filenames.clear();
		// バッファ宣言
		constexpr unsigned MAX_PATH_SIZE = MAX_PATH * 10;
		static OPENFILENAME ofn;
		static TCHAR current_dir[MAX_PATH_SIZE];
		static TCHAR opn_file[MAX_PATH_SIZE + 1];

		if (current_dir[0] == TEXT('\0'))
		{
			// 現在のディレクトリを取得
			GetCurrentDirectory(MAX_PATH_SIZE, current_dir);
		}
		// エクスプローラーを開く
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = nullptr;
		ofn.lpstrInitialDir = current_dir;
		ofn.lpstrFile = opn_file;
		ofn.nMaxFile = MAX_PATH_SIZE;
		ofn.lpstrFilter = filters;
		ofn.lpstrTitle = title;
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_NOCHANGEDIR;
		if (GetOpenFileName(&ofn))
		{
#ifdef UNICODE
			std::wstring wfilename = std::wstring(opn_file);
			const std::string directory = ConvertWstringToString(std::wstring(current_dir));
			const std::string file_path = ConvertWstringToString(wfilename);

			const std::filesystem::path full_path(file_path.c_str());
			const std::filesystem::path full_dir(directory.c_str());
			std::string relative_path = full_path.lexically_relative(full_dir).generic_string();
			relative_path = "./" + relative_path;

			size_t offset = file_path.size() + 1;
			wfilename = opn_file + offset;

			// Open one file
			if (wfilename.size() == 0)
			{
				filenames.emplace_back(relative_path);
			}
			// Open files over one
			else
			{
				relative_path += '/';
				while (wfilename.size() != 0)
				{
					// wstring -> string
					filenames.emplace_back(relative_path + ConvertWstringToString(wfilename));
					// Get next filename
					offset += wfilename.size() + 1;
					wfilename = opn_file + offset;
				}
			}
#else
			std::string filename = opn_file;
			std::string directory = current_dir;

			std::filesystem::path full_path(filename.c_str());
			std::filesystem::path full_dir(directory.c_str());
			std::string relative_path = full_path.lexically_relative(full_dir).generic_string(); // '\\' -> '/'

			size_t offset = filename.size() + 1;
			filename = opn_file + offset;

			// Open one file
			if (filename.size() == 0)
			{
				filenames.emplace_back(relative_path);
			}
			// Open files over one
			else
			{
				relative_path += '/';
				while (filename.size() != 0)
				{
					// wstring -> string
					filenames.emplace_back(relative_path + filename);
					// Get next filename
					offset += filename.size() + 1;
					filename = opn_file + offset;
				}
			}
#endif // UNICODE

			for (auto&& c : opn_file)
			{
#ifdef UNICODE
				c = wchar_t('\0');
#else
				c = char('\0');
#endif // UNICODE
			}

			return true;
		}
		return false;
	}

#ifdef UNICODE
	bool FetchOpenFileName(std::string& _filename, const wchar_t* filters, const wchar_t* title)
#else
	bool FetchOpenFileName(std::string& _filename, const char* filters, const char* title)
#endif // UNICODE
	{
		_filename.clear();
		// バッファ宣言
		static OPENFILENAME ofn;
		static TCHAR current_dir[MAX_PATH];
		static TCHAR opn_file[MAX_PATH + 1];

		if (current_dir[0] == TEXT('\0'))
		{
			// 現在のディレクトリを取得
			GetCurrentDirectory(MAX_PATH, current_dir);
		}
		// エクスプローラーを開く
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = nullptr;
		ofn.lpstrInitialDir = current_dir;
		ofn.lpstrFile = opn_file;
		ofn.nMaxFile = MAX_PATH;
		ofn.nMaxFileTitle = lstrlenW(title) + 1;
		ofn.lpstrFilter = filters;
		ofn.lpstrTitle = title;
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
		if (GetOpenFileName(&ofn))
		{
#ifdef UNICODE
			const std::wstring wfilename = std::wstring(opn_file);
			const std::string directory = ConvertWstringToString(std::wstring(current_dir));
			const std::string file_path = ConvertWstringToString(wfilename);

			const std::filesystem::path full_path(file_path.c_str());
			const std::filesystem::path full_dir(directory.c_str());
			const std::string relative_path = full_path.lexically_relative(full_dir).generic_string();
			_filename = "./" + relative_path;

#else
			std::string filename = opn_file;
			std::string directory = current_dir;

			std::filesystem::path full_path(filename.c_str());
			std::filesystem::path full_dir(directory.c_str());
			_filename = full_path.lexically_relative(full_dir).generic_string(); // '\\' -> '/'
#endif // UNICODE

			for (auto&& c : opn_file)
			{
#ifdef UNICODE
				c = wchar_t('\0');
#else
				c = char('\0');
#endif // UNICODE
			}

			return true;
		}
		return false;
	}
#ifdef UNICODE
	bool FetchOpenFileName(std::string& _filename, const wchar_t* filters, const wchar_t* title, const wchar_t* init_dir)
#else
	bool FetchOpenFileName(std::string& _filename, const char* filters, const char* title, const wchar_t* init_dir)
#endif // UNICODE
	{
		_filename.clear();
		// バッファ宣言
		static OPENFILENAME ofn;
		static TCHAR opn_file[MAX_PATH + 1];

		// エクスプローラーを開く
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = nullptr;
		ofn.lpstrInitialDir = init_dir;
		ofn.lpstrFile = opn_file;
		ofn.nMaxFile = MAX_PATH;
		ofn.nMaxFileTitle = lstrlenW(title) + 1;
		ofn.lpstrFilter = filters;
		ofn.lpstrTitle = title;
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_NOCHANGEDIR;
		if (GetOpenFileName(&ofn))
		{
			const std::filesystem::path current_path = std::filesystem::current_path();
#ifdef UNICODE
			const std::wstring wfilename = std::wstring(opn_file);
			const std::string file_path = ConvertWstringToString(wfilename);

			const std::filesystem::path full_path(file_path.c_str());
			const std::string relative_path = full_path.lexically_relative(current_path).generic_string();
			_filename = "./" + relative_path;

#else
			std::string filename = opn_file;

			std::filesystem::path full_path(filename.c_str());
			_filename = full_path.lexically_relative(current_path).generic_string(); // '\\' -> '/'
#endif // UNICODE

			for (auto&& c : opn_file)
			{
#ifdef UNICODE
				c = wchar_t('\0');
#else
				c = char('\0');
#endif // UNICODE
			}

			return true;
		}
		return false;
	}

#ifdef UNICODE
	bool FetchSaveFileName(std::string& _filename, const wchar_t* filters, const wchar_t* title)
#else
	bool FetchSaveFileName(std::string& _filename, const char* filters, const char* title)
#endif // UNICODE
	{
		_filename.clear();

		static OPENFILENAME ofn;
		static TCHAR current_dir[MAX_PATH];
		static TCHAR opn_file[MAX_PATH + 1];

		if (current_dir[0] == TEXT('\0'))
		{
			// 現在のディレクトリを取得
			GetCurrentDirectory(MAX_PATH, current_dir);
		}
		// エクスプローラーを開く
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = nullptr;
		ofn.lpstrInitialDir = current_dir;
		ofn.lpstrFile = opn_file;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = filters;
		ofn.lpstrTitle = title;
		ofn.Flags = OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
		if (GetSaveFileName(&ofn))
		{
#ifdef UNICODE
			const std::wstring wfilename = std::wstring(opn_file);
			const std::string directory = ConvertWstringToString(std::wstring(current_dir));
			const std::string file_path = ConvertWstringToString(wfilename);

			const std::filesystem::path full_path(file_path.c_str());
			const std::filesystem::path full_dir(directory.c_str());
			const std::string relative_path = full_path.lexically_relative(full_dir).generic_string();
			_filename = "./" + relative_path;
#else
			std::string filename = opn_file;
			std::string directory = current_dir;

			std::filesystem::path full_path(filename.c_str());
			std::filesystem::path full_dir(directory.c_str());
			std::string relative_path = full_path.lexically_relative(full_dir).generic_string(); // '\\' -> '/'
			_filename = "./" + relative_path;
#endif // UNICODE

			for (auto&& c : opn_file)
			{
#ifdef UNICODE
				c = wchar_t('\0');
#else
				c = char('\0');
#endif // UNICODE
			}

			return true;
		}
		return false;
	}
#ifdef UNICODE
	bool FetchSaveFileName(std::string& _filename, const wchar_t* filters, const wchar_t* title, const wchar_t* init_dir)
#else
	bool FetchSaveFileName(std::string& _filename, const char* filters, const char* title, const wchar_t* init_dir)
#endif // UNICODE
	{
		_filename.clear();

		static OPENFILENAME ofn;
		static TCHAR opn_file[MAX_PATH + 1];

		// エクスプローラーを開く
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = nullptr;
		ofn.lpstrInitialDir = init_dir;	// 区切り文字は'\\'、相対パスの先頭は'.'
		ofn.lpstrFile = opn_file;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = filters;
		ofn.lpstrTitle = title;
		ofn.Flags = OFN_EXPLORER | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
		if (GetSaveFileName(&ofn))
		{
			const std::filesystem::path current_path = std::filesystem::current_path();
#ifdef UNICODE
			const std::wstring wfilename = std::wstring(opn_file);
			const std::string file_path = ConvertWstringToString(wfilename);

			const std::filesystem::path full_path(file_path.c_str());
			const std::string relative_path = full_path.lexically_relative(current_path).generic_string();
			_filename = "./" + relative_path;
#else
			std::string filename = opn_file;

			std::filesystem::path full_path(filename.c_str());
			std::string relative_path = full_path.lexically_relative(current_path).generic_string(); // '\\' -> '/'
			_filename = "./" + relative_path;
#endif // UNICODE

			for (auto&& c : opn_file)
			{
#ifdef UNICODE
				c = wchar_t('\0');
#else
				c = char('\0');
#endif // UNICODE
			}

			return true;
		}
		return false;
	}

	std::string SplitFileName(const std::string& filepath)
	{
		namespace fs = std::filesystem;

		const fs::path p(filepath);
#ifdef UNICODE
		const std::wstring str = p.stem().c_str();
		return ConvertWstringToString(str);
#else
		return p.stem().c_str();
#endif // UNICODE
	}

	std::string SplitFilePathNoExtension(const std::string& filepath)
	{
		namespace fs = std::filesystem;

		const fs::path p(filepath);
#ifdef UNICODE
		const std::wstring parent_str = p.parent_path();
		const std::wstring stem_str = p.stem();
		const std::wstring ret_str = parent_str + L"/" + stem_str;
		return ConvertWstringToString(ret_str);
#else
		std::string parent_str = p.parent_path();
		std::string stem_str = p.stem();
		std::string ret_str = parent_str + "/" + stem_str;
		return ret_str.c_str();
#endif // UNICODE
	}

	std::string GetExtension(const std::string& path)
	{
		const size_t idx = path.rfind('.');
		// 見つからないか、'/''\\'が先に見つかるときは拡張子がないとき
		if (idx == std::string::npos)
		{
			return std::string();
		}
		const size_t slash_idx = path.find_last_of("/\\");
		if (slash_idx != std::string::npos && slash_idx > idx)
		{
			return std::string();
		}
		return path.substr(idx + 1, path.size() - idx - 1);
	}

	std::wstring GetExtension(const std::wstring& path)
	{
		const size_t idx = path.rfind(L'.');
		// 見つからないか、'/''\\'が先に見つかるときは拡張子がないとき
		if (idx == std::wstring::npos)
		{
			return std::wstring();
		}
		const size_t slash_idx = path.find_last_of(L"/\\");
		if (slash_idx != std::wstring::npos && slash_idx > idx)
		{
			return std::wstring();
		}
		return path.substr(idx + 1, path.size() - idx - 1);
		}

	std::string AlignedPathSlash(const std::string& path)
	{
		namespace fs = std::filesystem;

		const fs::path p(path);
		return p.generic_string();
	}

	std::wstring AlignedPathSlash(const std::wstring& path)
	{
		namespace fs = std::filesystem;

		const fs::path p(path);
		return p.generic_string<wchar_t>();
	}


	}// namespace TKGEngine::MyFunc