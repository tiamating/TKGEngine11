#pragma once

#include "Systems/inc/TKGEngine_Defined.h"

namespace TKGEngine
{
	static constexpr int g_max_num_system_deque_size = 2000;
	static constexpr int g_max_num_asset_load_threads = 1;

	/// <summary>
	/// Enumurate state of async loading asset data
	/// </summary>
	enum class ASSET_LOAD_ASYNC_STATE
	{
		ASSET_LOAD_ASYNC_NOTSET = 0,
		ASSET_LOAD_ASYNC_WAITING,
		ASSET_LOAD_ASYNC_LOADING,
		ASSET_LOAD_ASYNC_SUCCESS,
		ASSET_LOAD_ASYNC_FAILED
	};

	// ファイルデータの状態管理用構造体
	struct FileLoadStateData
	{
	public:
		// ファイルパス
		std::string filepath;
	private:
		// データを持っているか
		bool has_data = false;

	public:
		// データを持っているか
		bool HasData() const
		{
			return has_data;
		}
		// 初期化
		void Clear()
		{
			filepath.clear();
			has_data = false;
		}
		// データセット
		void Set(const std::string& filepath_)
		{
			filepath = filepath_;
			has_data = true;
		}
	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(filepath),
					CEREAL_NVP(has_data)
				);
			}
		}
	};


}// namespace TKGEngine

// FileLoadStateData
CEREAL_CLASS_VERSION(TKGEngine::FileLoadStateData, 1)
