
#include "Graphics_IQuery.h"

#include <d3d11.h>

#include <wrl.h>
#include <cassert>

namespace TKGEngine::Graphics
{
	class QueryDisjoint
		: public IQueryDisjoint
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		QueryDisjoint() = default;
		virtual ~QueryDisjoint() = default;
		QueryDisjoint(const QueryDisjoint&) = delete;
		QueryDisjoint& operator=(const QueryDisjoint&) = delete;

		// IQuery
		bool Init(ID3D11Device* p_device) override;
		void Release() override;

		ID3D11Query* GetQuery() const override;
		// ~IQuery

		// IQueryDisjoint
		void Begin(ID3D11DeviceContext * p_ic) override;
		void End(ID3D11DeviceContext * p_ic) override;

		bool WaitAndCheckJoint(ID3D11DeviceContext * p_ic) override;
		void WaitForIdle(ID3D11DeviceContext* p_ic) override;
		UINT64 GetFrequency() const override;
		// ~IQueryDisjoint

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================
		/* nothing */


		// ==============================================
		// private variables
		// ==============================================
		bool m_is_init = false;

		Microsoft::WRL::ComPtr<ID3D11Query> m_queries[2];
		int m_frame_buffer = 0;		// Current setting buffer
		int m_collect_buffer = 0;	// Last collect buffer

		UINT64 m_gpu_frequency = 0;

	};

	class QueryTimeStamp
		: public IQueryTimeStamp
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		QueryTimeStamp() = default;
		virtual ~QueryTimeStamp() = default;
		QueryTimeStamp(const QueryTimeStamp&) = delete;
		QueryTimeStamp& operator=(const QueryTimeStamp&) = delete;

		// IQuery
		bool Init(ID3D11Device* p_device) override;
		void Release() override;

		ID3D11Query* GetQuery() const override;
		// ~IQuery

		// IQueryTimeStamp
		void End(ID3D11DeviceContext* p_ic) override;
		void GetData(ID3D11DeviceContext* p_ic) override;

		UINT64 GetTimeStamp() const override;
		// ~IQueryTimeStamp

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================
		/* nothing */


		// ==============================================
		// private variables
		// ==============================================
		bool m_is_init = false;
		UINT64 m_time_stamp = 0;
		Microsoft::WRL::ComPtr<ID3D11Query> m_queries[2];
		int m_frame_buffer = 0;		// Current setting buffer
		//int m_collect_buffer = 0;	// Last collect buffer
	};

#pragma region QueryDisjoint
	IQueryDisjoint* IQueryDisjoint::CreateInterface()
	{
		return new QueryDisjoint();
	}

	bool QueryDisjoint::Init(ID3D11Device* p_device)
	{
		assert(p_device != nullptr);

		m_is_init = true;
		m_gpu_frequency = 0;
		m_frame_buffer = 0;
		m_collect_buffer = -1;

		HRESULT hr = S_OK;

		D3D11_QUERY_DESC desc = {};
		desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;

		for (int i = 0; i < 2; ++i)
		{
			hr = p_device->CreateQuery(&desc, &m_queries[i]);
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device->CreateQuery() QueryDisjoint::Init()");
				Release();
				return false;
			}
		}

		return true;
	}

	void QueryDisjoint::Release()
	{
		if (m_is_init == true)
		{
			for (int i = 0; i < 2; ++i)
			{
				m_queries[i].Reset();
			}

			m_is_init = false;
		}
	}

	ID3D11Query* QueryDisjoint::GetQuery() const
	{
		return m_queries[m_frame_buffer].Get();
	}

	void QueryDisjoint::Begin(ID3D11DeviceContext* p_ic)
	{
		assert(p_ic != nullptr);

		p_ic->Begin(m_queries[m_frame_buffer].Get());
	}

	void QueryDisjoint::End(ID3D11DeviceContext* p_ic)
	{
		assert(p_ic != nullptr);

		p_ic->End(m_queries[m_frame_buffer].Get());

		m_frame_buffer = 1 - m_frame_buffer;
	}

	bool QueryDisjoint::WaitAndCheckJoint(ID3D11DeviceContext* p_ic)
	{
		assert(p_ic != nullptr);

		if (m_collect_buffer < 0)
		{
			m_collect_buffer = 0;
			return false;
		}

		// 1フレーム前にセットされたクエリから情報を取得することで待機時間を削減する

		// Wait for a recording buffer finished
		while (p_ic->GetData(m_queries[m_collect_buffer].Get(), nullptr, 0, 0) == S_FALSE)
		{
			/* do nothing */
		}
		const int now_frame = m_collect_buffer;
		m_collect_buffer = 1 - m_collect_buffer;

		D3D11_QUERY_DATA_TIMESTAMP_DISJOINT data;
		auto hr = p_ic->GetData(m_queries[now_frame].Get(), &data, sizeof(data), 0);
		if (FAILED(hr))
		{
			return false;
		}

		if (data.Disjoint == TRUE)
		{
			return false;
		}

		m_gpu_frequency = data.Frequency;
		
		return true;
	}

	void QueryDisjoint::WaitForIdle(ID3D11DeviceContext* p_ic)
	{
		assert(p_ic != nullptr);
		
		// Wait for a recording buffer finished
		while (p_ic->GetData(m_queries[0].Get(), nullptr, 0, 0) == S_FALSE)
		{
			/* do nothing */
		}
		while(p_ic->GetData(m_queries[1].Get(), nullptr, 0, 0) == S_FALSE)
		{
			/* do nothing */
		}
	}

	inline UINT64 QueryDisjoint::GetFrequency() const
	{
		return m_gpu_frequency;
	}

#pragma endregion

#pragma region QueryTimeStamp
	IQueryTimeStamp* IQueryTimeStamp::CreateInterface()
	{
		return new QueryTimeStamp();
	}

	bool QueryTimeStamp::Init(ID3D11Device* p_device)
	{
		assert(p_device != nullptr);

		HRESULT hr = S_OK;

		D3D11_QUERY_DESC desc = {};
		desc.Query = D3D11_QUERY_TIMESTAMP;

		for (int i = 0; i < 2; ++i)
		{
			hr = p_device->CreateQuery(&desc, m_queries[i].GetAddressOf());
			if (FAILED(hr))
			{
				assert(0 && "failed ID3D11Device->CreateQuery() QueryTimeStamp::Init()");
				return false;
			}
		}

		m_is_init = true;
		m_frame_buffer = 0;
		//m_collect_buffer = 0;

		return true;
	}

	void QueryTimeStamp::Release()
	{
		if (m_is_init == true)
		{
			for (int i = 0; i < 2; ++i)
			{
				m_queries[i].Reset();
			}

			m_is_init = false;
		}
	}

	ID3D11Query* QueryTimeStamp::GetQuery() const
	{
		return m_queries[m_frame_buffer].Get();
	}

	void QueryTimeStamp::End(ID3D11DeviceContext* p_ic)
	{
		assert(p_ic != nullptr);

		p_ic->End(m_queries[m_frame_buffer].Get());

		m_frame_buffer = 1 - m_frame_buffer;
	}

	void QueryTimeStamp::GetData(ID3D11DeviceContext* p_ic)
	{
		assert(p_ic != nullptr);


		// 1フレーム前にセットされたクエリから情報を取得することで待機時間を削減する
		UINT64 ts;
		auto hr = p_ic->GetData(m_queries[m_frame_buffer].Get(), &ts, sizeof(ts), 0);
		if (FAILED(hr))
		{
			return;
		}

		m_time_stamp = ts;
	}

	UINT64 QueryTimeStamp::GetTimeStamp() const
	{

		return m_time_stamp;
	}


#pragma endregion

}// namespace TKGEngine::Graphics