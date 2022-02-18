
#include "../../inc/AssetSystem.h"

#include "Application/Resource/src/Texture/IResTexture.h"
#include "Application/Resource/inc/Asset_Defined.h"
#include "Systems/inc/IGraphics.h"
#include "Systems/inc/Graphics_Defined.h"

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Static definition
	////////////////////////////////////////////////////////
	AssetSystem* AssetSystem::m_instance = nullptr;


	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	void AssetSystem::Create()
	{
		if (m_instance == nullptr)
		{
			m_instance = new AssetSystem();
			m_instance->Init();
		}
	}

	void AssetSystem::Destroy()
	{
		if (m_instance)
		{
			m_instance->Term();
			delete m_instance;
			m_instance = nullptr;
		}
	}

	AssetSystem& AssetSystem::GetInstance()
	{
		assert(m_instance);
		return *m_instance;
	}

	bool AssetSystem::Add(const std::function<void()>& func)
	{
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			if (g_max_num_system_deque_size <= m_func_deque.size())
			{
				return false;
			}
			m_func_deque.emplace_back(func);
		}
		m_cv.notify_all();

		return true;
	}

	bool AssetSystem::Add(std::function<void()>&& func)
	{
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			if (g_max_num_system_deque_size <= m_func_deque.size())
			{
				return false;
			}
			m_func_deque.emplace_back(std::move(func));
		}
		m_cv.notify_all();

		return true;
	}

	ID3D11Device* AssetSystem::GetDevice() const
	{
		return IGraphics::Get().Device();
	}

	ID3D11DeviceContext* AssetSystem::GetCopyDC() const
	{
		return IGraphics::Get().DC(0, Graphics::DC_COPY_PATH::DC_CP_MAIN);
	}

	void AssetSystem::Init()
	{
		// Create threads
		m_is_termination_requested = false;
		m_threads.reserve(g_max_num_asset_load_threads);

		for (int i = 0; i < g_max_num_asset_load_threads; ++i)
		{
			m_threads.emplace_back(std::thread(m_load_func));
		}

		// Create Dummy texture
		if (!IResTexture::CreateDummyTexture())
		{
			assert(0 && "failed IResTexture::CreateDummyTexture() AssetSystem::Init()");
			return;
		}
	}

	void AssetSystem::Term()
	{
		// スレッドを抜けるようにフラグを立てる
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_is_termination_requested = true;
		}
		// Dequeを空にしてスレッドを終了する
		m_cv.notify_all();
		for (auto& th : m_threads)
		{
			th.join();
		}
	}

}// namespace TKGEngine