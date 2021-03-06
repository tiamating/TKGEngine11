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

		// 再生中のすべてのBGMを停止
		virtual void StopAllBGM() = 0;
		// 再生中のすべてのBGMを一時停止
		virtual void PauseAllBGM() = 0;
		// 停止中のすべてのBGMを再生
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
		/// <returns>int : 再生中のSEの識別番号 (-1 : 再生失敗)</returns>
		virtual int PlaySE(Audio::SE_LIST se, float pitch = 0.0f, float pan = 0.0f, bool is_loop = false) = 0;

		virtual void StopSEs(Audio::SE_LIST se) = 0;
		virtual void StopSE(Audio::SE_LIST se, int id) = 0;

		virtual void PauseSEs(Audio::SE_LIST se) = 0;
		virtual void PauseSE(Audio::SE_LIST se, int id) = 0;

		virtual void ResumeSEs(Audio::SE_LIST se) = 0;
		virtual void ResumeSE(Audio::SE_LIST se, int id) = 0;

		// 再生中のすべてのSEを停止
		virtual void StopAllSE() = 0;
		// 再生中のすべてのSEを一時停止
		virtual void PauseAllSE() = 0;
		// 停止中のすべてのSEを再生
		virtual void ResumeAllSE() = 0;

		virtual void SetSEVolume(Audio::SE_LIST se, int id, float volume) = 0;
		virtual void SetSEPitch(Audio::SE_LIST se, int id, float pitch) = 0;
		virtual void SetSEPan(Audio::SE_LIST se, int id, float pan) = 0;
	};

}// namespace TKGEngine