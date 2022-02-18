#pragma once

#include "SystemAccessor.h"
#include "Application/inc/AudioResourceList.h"

namespace TKGEngine
{
	class IAudio
		: public SystemAccessor<IAudio>
	{
	public:
		// =================================
		//				BGM
		// =================================
		virtual void PlayBGM(Audio::BGM_LIST bgm, float start_timer = 0.0f, float start_volume = 0.0f, bool is_loop = true) = 0;
		virtual void StopBGM(Audio::BGM_LIST bgm, float end_timer = 0.0f, float start_volume = 1.0f) = 0;
		virtual void PauseBGM(Audio::BGM_LIST bgm) = 0;
		virtual void ResumeBGM(Audio::BGM_LIST bgm) = 0;

		// Ä¶’†‚Ì‚·‚×‚Ä‚ÌBGM‚ğ’â~
		virtual void StopAllBGM() = 0;
		// Ä¶’†‚Ì‚·‚×‚Ä‚ÌBGM‚ğˆê’â~
		virtual void PauseAllBGM() = 0;
		// ’â~’†‚Ì‚·‚×‚Ä‚ÌBGM‚ğÄ¶
		virtual void ResumeAllBGM() = 0;

		virtual void SetBGMVolume(Audio::BGM_LIST bgm, float volume) = 0;
		virtual void SetBGMPitch(Audio::BGM_LIST bgm, float pitch) = 0;
		virtual void SetBGMPan(Audio::BGM_LIST bgm, float pan) = 0;

		virtual bool IsBGMPlaying(Audio::BGM_LIST bgm) = 0;
		virtual bool IsBGMPausing(Audio::BGM_LIST bgm) = 0;
		virtual bool IsBGMLooped(Audio::BGM_LIST bgm) = 0;
		virtual bool IsBGMInUsing(Audio::BGM_LIST bgm) = 0;

		// =================================
		//				SE
		// =================================
		/// <returns>int : Ä¶’†‚ÌSE‚Ì¯•Ê”Ô† (-1 : Ä¶¸”s)</returns>
		virtual int PlaySE(Audio::SE_LIST se, float pitch = 0.0f, float pan = 0.0f, bool is_loop = false) = 0;

		virtual void StopSEs(Audio::SE_LIST se) = 0;
		virtual void StopSE(Audio::SE_LIST se, int id) = 0;

		virtual void PauseSEs(Audio::SE_LIST se) = 0;
		virtual void PauseSE(Audio::SE_LIST se, int id) = 0;

		virtual void ResumeSEs(Audio::SE_LIST se) = 0;
		virtual void ResumeSE(Audio::SE_LIST se, int id) = 0;

		// Ä¶’†‚Ì‚·‚×‚Ä‚ÌSE‚ğ’â~
		virtual void StopAllSE() = 0;
		// Ä¶’†‚Ì‚·‚×‚Ä‚ÌSE‚ğˆê’â~
		virtual void PauseAllSE() = 0;
		// ’â~’†‚Ì‚·‚×‚Ä‚ÌSE‚ğÄ¶
		virtual void ResumeAllSE() = 0;

		virtual void SetSEVolume(Audio::SE_LIST se, int id, float volume) = 0;
		virtual void SetSEPitch(Audio::SE_LIST se, int id, float pitch) = 0;
		virtual void SetSEPan(Audio::SE_LIST se, int id, float pan) = 0;
	};

}// namespace TKGEngine