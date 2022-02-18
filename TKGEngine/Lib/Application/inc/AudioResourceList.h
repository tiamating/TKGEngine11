#pragma once

namespace TKGEngine::Audio
{
	///////////////////////////////////////////////////////////////
	/// 
	///							BGM
	/// 
	///////////////////////////////////////////////////////////////
	enum class BGM_LIST
	{
		///////////////////////////////////////////////////////
		/// �g�p����BGM�̗񋓎q���L�q
		///////////////////////////////////////////////////////
		Dummy,



		///////////////////////////////////////////////////////
		MAX_NUM_BGM_LIST
	};
	static constexpr const int MAX_BGM_LIST = static_cast<int>(BGM_LIST::MAX_NUM_BGM_LIST);
	static constexpr const bool LOAD_BGM_FILE = false;
	static constexpr const wchar_t* BGM_FILENAMES[MAX_BGM_LIST] =
	{
		L"./Asset/Audio/BGM/test.wav"
	};
	static constexpr float BGM_VOLUMES[MAX_BGM_LIST] =
	{
		1.0f,

	};


	///////////////////////////////////////////////////////////////
	/// 
	///							SE
	/// 
	///////////////////////////////////////////////////////////////
	enum class SE_LIST
	{
		///////////////////////////////////////////////////////
		/// �g�p����SE�̗񋓎q���L�q
		///////////////////////////////////////////////////////
		Dummy,



		///////////////////////////////////////////////////////
		MAX_NUM_SE_LIST
	};
	static constexpr const int MAX_SE_LIST = static_cast<int>(SE_LIST::MAX_NUM_SE_LIST);
	static constexpr const bool LOAD_SE_FILE = false;
	static constexpr const wchar_t* XWB_FILENAME = L"./Asset/Audio/SE/test.xwb";
	static constexpr const int WAVE_SOUND_MAX = 16;
	static constexpr float SE_VOLUMES[MAX_SE_LIST] =
	{
		1.0f,

	};


}// namespace TKGEngine