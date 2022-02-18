
#include "../../inc/IAudioSystem.h"
#include "../../inc/IAudio.h"

#include "../../DirectXTK/Inc/Audio.h"

#pragma comment(lib, "DirectXTKAudioWin8.lib")

#include <memory>

namespace TKGEngine::Audio
{
	/// ========================================================
	/// @class	SceneSystem
	/// @brief	Scene system
	/// 
	/// ========================================================
	class AudioSystem
		: public IAudioSystem,
		public IAudio
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		AudioSystem();
		virtual ~AudioSystem();
		AudioSystem(AudioSystem&&) = default;
		AudioSystem(const AudioSystem&) = delete;
		AudioSystem& operator=(const AudioSystem&) = delete;

		// ISystem
		bool OnInit() override;
		void OnTerm() override;
		void OnFrameBegin(const FrameEventArgs& args) override;
		void OnFrameRender(const FrameEventArgs& args) override;
		void OnFrameEnd(const FrameEventArgs& args) override;
		void OnResize(const ResizeEventArgs& args) override;
		// ~ISystem


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================
		// IAudio
		// BGM
		void PlayBGM(BGM_LIST bgm, float start_timer = 0.0f, float start_volume = 0.0f, bool is_loop = true) override;
		void StopBGM(BGM_LIST bgm, float end_timer = 0.0f, float start_volume = 1.0f) override;
		void PauseBGM(BGM_LIST bgm) override;
		void ResumeBGM(BGM_LIST bgm) override;

		// 再生中のすべてのBGMを停止
		void StopAllBGM() override;
		// 再生中のすべてのBGMを一時停止
		void PauseAllBGM() override;
		// 停止中のすべてのBGMを再生
		void ResumeAllBGM() override;

		void SetBGMVolume(BGM_LIST bgm, float volume) override;
		void SetBGMPitch(BGM_LIST bgm, float pitch) override;
		void SetBGMPan(BGM_LIST bgm, float pan) override;

		bool IsBGMPlaying(BGM_LIST bgm) override;
		bool IsBGMPausing(BGM_LIST bgm) override;
		bool IsBGMLooped(BGM_LIST bgm) override;
		bool IsBGMInUsing(BGM_LIST bgm) override;
		// ~BGM

		// SE
		/// <returns>int : 再生中のSEの識別番号</returns>
		int PlaySE(SE_LIST se, float pitch = 0.0f, float pan = 0.0f, bool is_loop = false) override;

		void StopSEs(SE_LIST se) override;
		void StopSE(SE_LIST se, int id) override;

		void PauseSEs(SE_LIST se) override;
		void PauseSE(SE_LIST se, int id) override;

		void ResumeSEs(SE_LIST se) override;
		void ResumeSE(SE_LIST se, int id) override;

		// 再生中のすべてのSEを停止
		void StopAllSE() override;
		// 再生中のすべてのSEを一時停止
		void PauseAllSE() override;
		// 停止中のすべてのSEを再生
		void ResumeAllSE() override;

		void SetSEVolume(SE_LIST se, int id, float volume) override;
		void SetSEPitch(SE_LIST se, int id, float pitch) override;
		void SetSEPan(SE_LIST se, int id, float pan) override;
		// ~SE
		// ~IAudio

		// ==============================================
		// private variables
		// ==============================================
		// Audio Engine
		std::unique_ptr<DirectX::AudioEngine> m_audio_engine = nullptr;

		// BGM
		struct BGMData
		{
			std::unique_ptr<DirectX::SoundEffect> m_bgm[MAX_BGM_LIST];
			std::unique_ptr<DirectX::SoundEffectInstance> m_bgm_inst[MAX_BGM_LIST];
		};
		BGMData m_bgm_data;

		struct BGMTimerParam
		{
			int bgm_id = -1;
			float start_volume = 1.0f;
			float end_volume = 0.0f;
			float length = 0.0f;		// [s]
			float remain_time = 0.0f;	// [s]

			void Add(int id, float length_, float start_volume_, float end_volume_)
			{
				bgm_id = id;
				length = length_;
				remain_time = length_;
				start_volume = start_volume;
				end_volume = end_volume;
			}
			void Remove()
			{
				bgm_id = -1;
				length = 0.0f;
				remain_time = 0.0f;
			}
		};
		BGMTimerParam m_bgm_play_data;
		BGMTimerParam m_bgm_stop_data;

		// SE
		struct SEData
		{
			std::unique_ptr<DirectX::WaveBank> m_wave_bank;
			std::unique_ptr<DirectX::SoundEffectInstance> m_se_inst[MAX_SE_LIST][WAVE_SOUND_MAX];
		};
		SEData m_se_data;
	};

	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	std::unique_ptr<IAudioSystem> IAudioSystem::CreateInterface()
	{
		return std::make_unique<AudioSystem>();
	}

	AudioSystem::AudioSystem()
	{
		/* nothing */
	}

	AudioSystem::~AudioSystem()
	{
		/* nothing */
	}


	bool AudioSystem::OnInit()
	{
		// Create DirectXTK for Audio objects.
		DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_Default;
#ifdef _DEBUG
		eflags = eflags | DirectX::AudioEngine_Debug;
#endif
		m_audio_engine = std::make_unique<DirectX::AudioEngine>(eflags);

		// Load BGM data.
		if (LOAD_BGM_FILE == true)
		{
			for (int i = 0; i < MAX_BGM_LIST; ++i)
			{
				m_bgm_data.m_bgm[i] =
					std::make_unique<DirectX::SoundEffect>(m_audio_engine.get(), BGM_FILENAMES[i]);
				if (m_bgm_data.m_bgm[i] == nullptr)
				{
					assert(0 && "failed load BGM from file");
					return false;
				}
			}
		}

		// Load SE data.
		if (LOAD_SE_FILE == true)
		{
			m_se_data.m_wave_bank =
				std::make_unique<DirectX::WaveBank>(m_audio_engine.get(), XWB_FILENAME);
			if (m_se_data.m_wave_bank == nullptr)
			{
				assert(0 && "failed load SE from file");
				return false;
			}
		}

		// Init IAudio.
		IAudio::Init(this);

		return true;
	}

	void AudioSystem::OnTerm()
	{
		// Release IAudio.
		IAudio::Release();

		// Release AudioEngine.
		m_audio_engine.reset();
	}

	void AudioSystem::OnFrameBegin(const FrameEventArgs& args)
	{
		// Play
		{
			// Playデータが存在するかチェック
			if (m_bgm_play_data.bgm_id != -1)
			{
				// Playデータの更新
				m_bgm_play_data.remain_time -= args.unscaled_delta_time;

				// 設定した時間が経過していたらPlayデータをリセット
				if (m_bgm_play_data.remain_time <= 0.0f)
				{
					m_bgm_play_data.Remove();
				}
				// 経過していなければVolumeを調整
				else
				{
					float volume = 
						(m_bgm_play_data.end_volume - m_bgm_play_data.start_volume) * 
						(m_bgm_play_data.remain_time / m_bgm_play_data.length) + 
						m_bgm_play_data.start_volume;
					m_bgm_data.m_bgm_inst[m_bgm_play_data.bgm_id]->SetVolume(volume);
				}
			}
		}

		// Stop
		{
			// Stopデータが存在するかチェック
			if (m_bgm_stop_data.bgm_id != -1)
			{
				// Stopデータの更新
				m_bgm_stop_data.remain_time -= args.unscaled_delta_time;

				// 設定した時間が経過していたらStopしてリセット
				if (m_bgm_stop_data.remain_time <= 0.0f)
				{
					m_bgm_data.m_bgm_inst[m_bgm_stop_data.bgm_id]->Stop();
					m_bgm_stop_data.Remove();
				}
				// 経過していなければVolumeを調整
				else
				{
					float volume = m_bgm_stop_data.start_volume * (m_bgm_stop_data.remain_time / m_bgm_stop_data.length);
					m_bgm_data.m_bgm_inst[m_bgm_stop_data.bgm_id]->SetVolume(volume);
				}
			}
		}
	}

	void AudioSystem::OnFrameRender(const FrameEventArgs& args)
	{
		/* nothing */
	}

	void AudioSystem::OnFrameEnd(const FrameEventArgs& args)
	{
		/* nothing*/
	}

	void AudioSystem::OnResize(const ResizeEventArgs& args)
	{
		/* nothing */
	}

	// IAudio
	// -----------------------------------------
	// BGM
	// -----------------------------------------
#pragma region BGM
	void AudioSystem::PlayBGM(BGM_LIST bgm, float start_timer, float start_volume, bool is_loop)
	{
		auto& bgm_data = m_bgm_data.m_bgm[static_cast<int>(bgm)];
		auto& bgm_inst = m_bgm_data.m_bgm_inst[static_cast<int>(bgm)];
		if (bgm_data == nullptr)
			return;
		bgm_inst = bgm_data->CreateInstance();
		bgm_inst->Play(is_loop);
		// 徐々に変化させない
		if (start_timer <= 0.0f)
		{
			bgm_inst->SetVolume(BGM_VOLUMES[static_cast<int>(bgm)]);
			return;
		}
		// 徐々に変化させる
		bgm_inst->SetVolume(start_volume);
		// Playデータにセットする
		m_bgm_play_data.Add(static_cast<int>(bgm), start_timer, start_volume, BGM_VOLUMES[static_cast<int>(bgm)]);
	}

	void AudioSystem::StopBGM(BGM_LIST bgm, float end_timer, float start_volume)
	{
		const int new_stop_id = static_cast<int>(bgm);
		if (m_bgm_data.m_bgm_inst[new_stop_id] == nullptr)
			return;

		// 既に停止しているかチェック
		DirectX::SoundState state = m_bgm_data.m_bgm_inst[new_stop_id]->GetState();
		if (state == DirectX::SoundState::STOPPED)
			return;

		if (end_timer <= 0.0f)
		{
			m_bgm_data.m_bgm_inst[new_stop_id]->Stop();
		}
		else
		{
			const int current_stop_id = m_bgm_stop_data.bgm_id;

			// Stopデータが残っている
			if (current_stop_id != -1)
			{
				//	StopデータのIDが有効なら即時停止する
				if (m_bgm_data.m_bgm[current_stop_id]->IsInUse() == true)
				{
					m_bgm_data.m_bgm_inst[current_stop_id]->Stop();
				}
				m_bgm_stop_data.Remove();
			}

			// 新しい停止データを入れる
			m_bgm_stop_data.Add(new_stop_id, end_timer, start_volume, 0.0f);
		}
	}

	void AudioSystem::PauseBGM(BGM_LIST bgm)
	{
		m_bgm_data.m_bgm_inst[static_cast<int>(bgm)]->Pause();
	}

	void AudioSystem::ResumeBGM(BGM_LIST bgm)
	{
		m_bgm_data.m_bgm_inst[static_cast<int>(bgm)]->Resume();
	}

	void AudioSystem::StopAllBGM()
	{
		for (auto&& inst : m_bgm_data.m_bgm_inst)
		{
			inst->Stop();
		}
	}

	void AudioSystem::PauseAllBGM()
	{
		for (auto&& inst : m_bgm_data.m_bgm_inst)
		{
			inst->Pause();
		}
	}

	void AudioSystem::ResumeAllBGM()
	{
		for (auto&& inst : m_bgm_data.m_bgm_inst)
		{
			inst->Resume();
		}
	}

	void AudioSystem::SetBGMVolume(BGM_LIST bgm, float volume)
	{
		return m_bgm_data.m_bgm_inst[static_cast<int>(bgm)]->SetVolume(volume);
	}

	void AudioSystem::SetBGMPitch(BGM_LIST bgm, float pitch)
	{
		return m_bgm_data.m_bgm_inst[static_cast<int>(bgm)]->SetPitch(pitch);
	}

	void AudioSystem::SetBGMPan(BGM_LIST bgm, float pan)
	{
		return m_bgm_data.m_bgm_inst[static_cast<int>(bgm)]->SetPan(pan);
	}

	bool AudioSystem::IsBGMPlaying(BGM_LIST bgm)
	{
		return m_bgm_data.m_bgm_inst[static_cast<int>(bgm)]->GetState() ==
			DirectX::SoundState::PLAYING;
	}

	bool AudioSystem::IsBGMPausing(BGM_LIST bgm)
	{
		return m_bgm_data.m_bgm_inst[static_cast<int>(bgm)]->GetState() ==
			DirectX::SoundState::PAUSED;
	}

	bool AudioSystem::IsBGMLooped(BGM_LIST bgm)
	{
		return m_bgm_data.m_bgm_inst[static_cast<int>(bgm)]->IsLooped();
	}

	bool AudioSystem::IsBGMInUsing(BGM_LIST bgm)
	{
		return m_bgm_data.m_bgm[static_cast<int>(bgm)]->IsInUse();
	}
#pragma endregion


	// -----------------------------------------
	// SE
	// -----------------------------------------
#pragma region SE
	int AudioSystem::PlaySE(SE_LIST se, float pitch, float pan, bool is_loop)
	{
		const int se_id = static_cast<int>(se);

		for (int i = 0; i < WAVE_SOUND_MAX; ++i)
		{
			auto& instance = m_se_data.m_se_inst[se_id][i];
			if (instance != nullptr)
			{
				const auto state = instance->GetState();
				if (state != DirectX::SoundState::STOPPED)
					continue;
			}

			instance = m_se_data.m_wave_bank->CreateInstance(se_id);
			if (instance != nullptr)
			{
				instance->SetVolume(SE_VOLUMES[se_id]);
				instance->SetPitch(pitch);
				instance->SetPan(pan);
				instance->Play(is_loop);
				return i;
			}
		}
		return -1;
	}

	void AudioSystem::StopSEs(SE_LIST se)
	{
		auto& se_instances = m_se_data.m_se_inst[static_cast<int>(se)];
		for (int inst_id = 0; inst_id < WAVE_SOUND_MAX; ++inst_id)
		{
			const auto state = se_instances[inst_id]->GetState();
			if (state == DirectX::SoundState::STOPPED)
				continue;
			se_instances[inst_id]->Stop();
		}
	}

	void AudioSystem::StopSE(SE_LIST se, int id)
	{
		if (id < 0)
			return;
		auto& se_inst = m_se_data.m_se_inst[static_cast<int>(se)][id];
		const auto state = se_inst->GetState();
		if (state == DirectX::SoundState::STOPPED)
			return;
		se_inst->Stop();
	}

	void AudioSystem::PauseSEs(SE_LIST se)
	{
		auto& se_instances = m_se_data.m_se_inst[static_cast<int>(se)];
		for (int inst_id = 0; inst_id < WAVE_SOUND_MAX; ++inst_id)
		{
			const auto state = se_instances[inst_id]->GetState();
			if (state != DirectX::SoundState::PLAYING)
				continue;
			se_instances[inst_id]->Pause();
		}
	}

	void AudioSystem::PauseSE(SE_LIST se, int id)
	{
		if (id < 0)
			return;
		auto& se_inst = m_se_data.m_se_inst[static_cast<int>(se)][id];
		const auto state = se_inst->GetState();
		if (state != DirectX::SoundState::PLAYING)
			return;
		se_inst->Pause();
	}

	void AudioSystem::ResumeSEs(SE_LIST se)
	{
		auto& se_instances = m_se_data.m_se_inst[static_cast<int>(se)];
		for (int inst_id = 0; inst_id < WAVE_SOUND_MAX; ++inst_id)
		{
			const auto state = se_instances[inst_id]->GetState();
			if (state != DirectX::SoundState::PAUSED)
				continue;
			se_instances[inst_id]->Resume();
		}
	}

	void AudioSystem::ResumeSE(SE_LIST se, int id)
	{
		if (id < 0)
			return;
		auto& se_inst = m_se_data.m_se_inst[static_cast<int>(se)][id];
		const auto state = se_inst->GetState();
		if (state != DirectX::SoundState::PAUSED)
			return;
		se_inst->Resume();
	}

	void AudioSystem::StopAllSE()
	{
		for (int se_id = 0; se_id < MAX_SE_LIST; ++se_id)
		{
			auto& se_instances = m_se_data.m_se_inst[se_id];
			for (int inst_id = 0; inst_id < WAVE_SOUND_MAX; ++inst_id)
			{
				const auto state = se_instances[inst_id]->GetState();
				if (state == DirectX::SoundState::STOPPED)
					continue;
				se_instances[inst_id]->Stop();
			}
		}
	}

	void AudioSystem::PauseAllSE()
	{
		for (int se_id = 0; se_id < MAX_SE_LIST; ++se_id)
		{
			auto& se_instances = m_se_data.m_se_inst[se_id];
			for (int inst_id = 0; inst_id < WAVE_SOUND_MAX; ++inst_id)
			{
				const auto state = se_instances[inst_id]->GetState();
				if (state != DirectX::SoundState::PLAYING)
					continue;
				se_instances[inst_id]->Pause();
			}
		}
	}

	void AudioSystem::ResumeAllSE()
	{
		for (int se_id = 0; se_id < MAX_SE_LIST; ++se_id)
		{
			auto& se_instances = m_se_data.m_se_inst[se_id];
			for (int inst_id = 0; inst_id < WAVE_SOUND_MAX; ++inst_id)
			{
				const auto state = se_instances[inst_id]->GetState();
				if (state != DirectX::SoundState::PAUSED)
					continue;
				se_instances[inst_id]->Resume();
			}
		}
	}

	void AudioSystem::SetSEVolume(SE_LIST se, int id, float volume)
	{
		if (id < 0)
			return;
		auto& se_instance = m_se_data.m_se_inst[static_cast<int>(se)][id];
		if (se_instance == nullptr)
			return;
		se_instance->SetVolume(volume);
	}

	void AudioSystem::SetSEPitch(SE_LIST se, int id, float pitch)
	{
		if (id < 0)
			return;
		auto& se_instance = m_se_data.m_se_inst[static_cast<int>(se)][id];
		if (se_instance == nullptr)
			return;
		se_instance->SetPitch(pitch);
	}

	void AudioSystem::SetSEPan(SE_LIST se, int id, float pan)
	{
		if (id < 0)
			return;
		auto& se_instance = m_se_data.m_se_inst[static_cast<int>(se)][id];
		if (se_instance == nullptr)
			return;
		se_instance->SetPan(pan);
	}
#pragma endregion
	// ~IAudio

}// namespace TKGEngine::Audio