
#include "Graphics_IProfiler.h"

#include "Systems/inc/ITime.h"

#include <d3d11.h>
#include <wrl.h>

#include <vector>
#include <deque>
#include <cassert>


namespace TKGEngine::Graphics
{
	/// <summary>
	/// Graphics profiler module
	/// </summary>
	class Profiler
		: public IProfiler
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		Profiler();
		virtual ~Profiler() = default;
		Profiler(const Profiler&) = delete;
		Profiler& operator=(const Profiler&) = delete;

		bool Init(ID3D11Device* p_device) override;
		void Release() override;

		void BeginFrame(ID3D11DeviceContext* p_ic) override;
		void TimeStamp(ID3D11DeviceContext* p_ic, TS_TYPE type, int thread_idx) override;
		void EndFrame(ID3D11DeviceContext* p_ic) override;

		void WaitAndUpdate(ID3D11DeviceContext* p_ic) override;
		void WaitForIdle(ID3D11DeviceContext* p_ic) override;

		float GetElapsedTime(TS_TYPE type, int thread_idx) override;
		float GetAverageTime(TS_TYPE type, int thread_idx) override;


		// ==============================================
		// public variables
		// ==============================================


	private:
		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		// プロファイラのデバッグ表示
		void OnGUI() override;
#endif// #ifdef USE_IMGUI


		// ==============================================
		// private variables
		// ==============================================
		static constexpr int m_total_num_ts =
			1 +
			g_max_num_compute_threads * static_cast<int>(DC_COMPUTE_PATH::DC_CP_MAX_NUM) +
			g_max_num_copy_threads * static_cast<int>(DC_COPY_PATH::DC_CP_MAX_NUM) +
			g_max_num_render_threads * static_cast<int>(DC_RENDER_PATH::DC_RP_MAX_NUM) +
			1;

		int m_ts_type_head[static_cast<int>(TS_TYPE::TS_MAX_NUM)];// {0,1,?,?,max_elements}

		int m_frame_query = 0;
		int m_frame_collect = -1;
		Microsoft::WRL::ComPtr<ID3D11Query> m_query_disjoints[2] = {};
		Microsoft::WRL::ComPtr<ID3D11Query> m_query_TSs[m_total_num_ts][2] = {};

		float m_elapsed_times[m_total_num_ts] = {};
		float m_average_times[m_total_num_ts] = {};	// 0.5s ave

		float m_ave_total_times[m_total_num_ts] = {};
		LONGLONG m_ave_frame_count = 0;
		float m_ave_begin_time = 0.f;
		float m_total_time = 0.f;
	};


	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	// IProfiler
	IProfiler* IProfiler::CreateInterface()
	{
		return new Profiler();
	}
	// ~IProfiler


	// Profiler
	Profiler::Profiler()
	{
		m_ts_type_head[static_cast<int>(TS_TYPE::TS_BEGIN)] = 0;

		m_ts_type_head[static_cast<int>(TS_TYPE::TS_COPY)] =
			m_ts_type_head[static_cast<int>(TS_TYPE::TS_BEGIN)] + 1;

		m_ts_type_head[static_cast<int>(TS_TYPE::TS_COMPUTE)] =
			m_ts_type_head[static_cast<int>(TS_TYPE::TS_COPY)] + g_max_num_copy_threads * static_cast<int>(DC_COPY_PATH::DC_CP_MAX_NUM);

		m_ts_type_head[static_cast<int>(TS_TYPE::TS_RENDER)] =
			m_ts_type_head[static_cast<int>(TS_TYPE::TS_COMPUTE)] + g_max_num_compute_threads * static_cast<int>(DC_COMPUTE_PATH::DC_CP_MAX_NUM);

		m_ts_type_head[static_cast<int>(TS_TYPE::TS_END)] =
			m_ts_type_head[static_cast<int>(TS_TYPE::TS_RENDER)] + g_max_num_render_threads * static_cast<int>(DC_RENDER_PATH::DC_RP_MAX_NUM);
	}

	bool Profiler::Init(ID3D11Device* p_device)
	{
		assert(p_device);

		HRESULT hr = S_OK;

		// Create Query Disjoint
		{
			D3D11_QUERY_DESC desc = {};
			desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;

			for (int i = 0; i < 2; ++i)
			{
				hr = p_device->CreateQuery(&desc, m_query_disjoints[i].GetAddressOf());
				if (FAILED(hr))
				{
					assert(0 && "failed ID3D11Device::CreateQuery() Profiler::Init()");
					return false;
				}
			}
		}

		// Create Query TimeStamp
		{
			D3D11_QUERY_DESC desc = {};
			desc.Query = D3D11_QUERY_TIMESTAMP;

			for (int type = 0; type < m_total_num_ts; ++type)
			{
				for (int i = 0; i < 2; ++i)
				{
					hr = p_device->CreateQuery(&desc, m_query_TSs[type][i].GetAddressOf());
					if (FAILED(hr))
					{
						assert(0 && "failed ID3D11Device::CreateQuery() Profiler::Init()");
						return false;
					}
				}
			}
		}

		return true;
	}

	void Profiler::Release()
	{
		/* nothing */
	}

	void Profiler::BeginFrame(ID3D11DeviceContext* p_ic)
	{
		p_ic->Begin(m_query_disjoints[m_frame_query].Get());
#ifdef USE_IMGUI
		TimeStamp(p_ic, TS_TYPE::TS_BEGIN, 0);
#endif// #ifdef USE_IMGUI
	}

	void Profiler::TimeStamp(ID3D11DeviceContext* p_ic, TS_TYPE type, int thread_idx)
	{
		p_ic->End(
			m_query_TSs[m_ts_type_head[static_cast<int>(type)] + thread_idx][m_frame_query].Get());
	}

	void Profiler::EndFrame(ID3D11DeviceContext* p_ic)
	{
#ifdef USE_IMGUI
		TimeStamp(p_ic, TS_TYPE::TS_END, 0);
#endif// #ifdef USE_IMGUI
		p_ic->End(m_query_disjoints[m_frame_query].Get());

		m_frame_query = 1 - m_frame_query;
	}

	void Profiler::WaitAndUpdate(ID3D11DeviceContext* p_ic)
	{
		if (m_frame_collect < 0)
		{
			// まだ実行されておらずデータを持っていないため早期リターン
			m_frame_collect = 0;
			return;
		}

		// Wait for data
		while (p_ic->GetData(m_query_disjoints[m_frame_collect].Get(), nullptr, 0, 0) == S_FALSE)
		{
			/* nothing */
		}

		m_frame_collect = 1 - m_frame_collect;

#ifdef USE_IMGUI
		const int frame_idx = 1 - m_frame_collect;
		D3D11_QUERY_DATA_TIMESTAMP_DISJOINT ts_disjoint = {};
		auto hr = p_ic->GetData(m_query_disjoints[frame_idx].Get(), &ts_disjoint, sizeof(ts_disjoint), 0);
		if (FAILED(hr))
		{
			assert(0 && "failed ID3D11DeviceContext::GetData() Profiler::WaitAndUpdate()");
			return;
		}
		// Check disjoint
		if (ts_disjoint.Disjoint)
		{
			// このフレームのデータを破棄する
			return;
		}
		// フレーム開始時のフレーム数を取得
		UINT64 ts_prev = 0;
		hr = p_ic->GetData(m_query_TSs[m_ts_type_head[static_cast<int>(TS_TYPE::TS_BEGIN)]][frame_idx].Get(), &ts_prev, sizeof(UINT64), 0);
		if (FAILED(hr))
		{
			assert(0 && "failed ID3D11DeviceContext::GetData() Profiler::WaitAndUpdate()");
			return;
		}
		// それぞれのタイムスタンプの実行による経過時間を取得
		for (int i = m_ts_type_head[static_cast<int>(TS_TYPE::TS_BEGIN)] + 1; i < m_total_num_ts; ++i)
		{
			UINT64 ts = 0;
			hr = p_ic->GetData(m_query_TSs[i][frame_idx].Get(), &ts, sizeof(UINT64), 0);
			// 有効なタイムスタンプかチェック
			if (FAILED(hr))
			{
				continue;
			}
			// 経過時間を保持する
			m_elapsed_times[i] = static_cast<float>(static_cast<long double>(ts - ts_prev) / static_cast<long double>(ts_disjoint.Frequency));
			ts_prev = ts;

			m_ave_total_times[i] += m_elapsed_times[i];
		}
		// 0.1秒ごとに平均の計算を行う
		++m_ave_frame_count;
		m_total_time += ITime::Get().UnscaledDeltaTime();
		if (m_total_time > m_ave_begin_time + 0.1f)
		{
			for (int i = m_ts_type_head[static_cast<int>(TS_TYPE::TS_BEGIN)]; i < m_total_num_ts; ++i)
			{
				m_average_times[i] = m_ave_total_times[i] / m_ave_frame_count;
				m_ave_total_times[i] = 0.f;
			}

			m_ave_frame_count = 0;
			m_ave_begin_time = m_total_time;
		}
#endif// #ifdef USE_IMGUI
	}

	void Profiler::WaitForIdle(ID3D11DeviceContext* p_ic)
	{
		if (m_frame_collect < 0)
		{
			m_frame_collect = 0;
		}

		// GPUの処理終了待ち
		while (p_ic->GetData(m_query_disjoints[m_frame_collect].Get(), nullptr, 0, 0) == S_FALSE)
		{
			/* nothing */
		}
		m_frame_collect = 1 - m_frame_collect;
		while (p_ic->GetData(m_query_disjoints[m_frame_collect].Get(), nullptr, 0, 0) == S_FALSE)
		{
			/* nothing */
		}

		for (int i = 0; i < m_total_num_ts; ++i)
		{
			m_elapsed_times[i] = 0.f;
			m_average_times[i] = 0.f;
			m_ave_total_times[i] = 0.f;
		}

		m_frame_query = 0;
		m_frame_collect = -1;
		m_ave_frame_count = 0;
		m_ave_begin_time = 0.f;
		m_total_time = 0.f;
	}

	float Profiler::GetElapsedTime(TS_TYPE type, int thread_idx)
	{
		return m_elapsed_times[m_ts_type_head[static_cast<int>(type)] + thread_idx];
	}

	float Profiler::GetAverageTime(TS_TYPE type, int thread_idx)
	{
		return m_average_times[m_ts_type_head[static_cast<int>(type)] + thread_idx];
	}

#ifdef USE_IMGUI
	void Profiler::OnGUI()
	{
		constexpr int time_stamp_type_num = m_total_num_ts - 2;
		// 値を保持する配列
		static std::deque<float> average_time_buffer[time_stamp_type_num];
		static bool is_deque_init = false;
		// 要素確保する
		constexpr size_t buffer_size = 240;
		if (!is_deque_init)
		{
			is_deque_init = true;
			for (auto& buf : average_time_buffer)
			{
				buf.resize(buffer_size);
			}
		}
		// プロット時値取得用関数オブジェクト
		struct RegisterFunction
		{
			static float GetValue(void* deque, int i)
			{
				return static_cast<std::deque<float>*>(deque)->at(i);
			}
		};
		// Window表示
		ImGui::PushID(this);
		ImGui::SetNextWindowSize(ImVec2(200.0f, 90.0f), ImGuiCond_FirstUseEver);
		static int current_item = static_cast<int>(TS_TYPE::TS_RENDER) + 1;// 初期はMain
		constexpr const char* names[time_stamp_type_num] =
		{
			"Copy",
			"Compute",
			"Shadow",
			"Pre-Z",
			"Main",
			"Post After Main",
			"UI",
			"Post After All"
		};

		if (ImGui::Begin("GPU Profiler"))
		{
			// 表示するものの選択
			ImGui::Combo("##Time Stamp Type", &current_item, names, time_stamp_type_num);
			//表示しているものだけを更新
			average_time_buffer[current_item].pop_front();
			// timesの0番目にはBeginがあるためCompute分ずらす
			average_time_buffer[current_item].push_back(m_elapsed_times[current_item + m_ts_type_head[static_cast<int>(TS_TYPE::TS_COPY)]]);

			constexpr float spacing = 0.05f;
			std::string label = "##" + std::string(names[current_item]);
			ImGui::SetNextItemWidth(ImGui::GetWindowSize().x * (1.0f - spacing));
			ImGui::PlotLines(
				label.c_str(), RegisterFunction::GetValue, &average_time_buffer[current_item], buffer_size,
				0, nullptr, 0.0f, 2e-3f/*2[ms]*/, ImVec2(0.0f, 60.0f)
			);
		}
		ImGui::End();
		ImGui::PopID();
	}
#endif// #ifdef USE_IMGUI

	// ~Profiler


}// namespace TKGEngine::Graphics