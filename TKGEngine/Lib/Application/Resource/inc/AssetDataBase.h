#pragma once

#include "Systems/inc/TKGEngine_Defined.h"
#include "Asset_Defined.h"

#include "Utility/inc/myfunc_file.h"

#include <string>
#include <memory>

#include <functional>
#include <mutex>


namespace TKGEngine
{
	/// <summary>
	/// Asset Data base module
	/// </summary>
	class AssetDataBase
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		AssetDataBase()
		{
			/* nothing */
		}
		virtual ~AssetDataBase() = default;
		AssetDataBase(const AssetDataBase&) = delete;
		AssetDataBase& operator=(const AssetDataBase&) = delete;

#ifdef USE_IMGUI
		virtual void OnGUI() = 0;
#endif // USE_IMGUI
		virtual void Release() = 0;

		[[nodiscard]] bool IsLoading() const;
		[[nodiscard]] bool IsLoaded() const;
		[[nodiscard]] bool HasResource() const;

		void SetName(const std::string& name);
		[[nodiscard]] const char* GetName() const;

		void SetFilePath(const std::string& filepath);
		[[nodiscard]] const char* GetFilePath() const;
		[[nodiscard]] const std::string& GetLoadedFilePath() const;

		[[nodiscard]] unsigned GetHash() const;


	protected:
		// ==============================================
		// protected variables
		// ==============================================
		bool m_has_resource = false;
		bool m_is_loading = false;
		bool m_is_loaded = false;

		ASSET_LOAD_ASYNC_STATE m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_NOTSET;
		std::mutex m_load_state_mutex;


	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			if (version == 3)
			{
				archive(
					CEREAL_NVP(m_has_name),
					CEREAL_NVP(m_name)
				);
			}
			else if (version == 2)
			{
				archive(
					CEREAL_NVP(m_has_name),
					CEREAL_NVP(m_name),
					CEREAL_NVP(m_file_path)
				);
			}
			else if (version == 1)
			{
				archive(
					CEREAL_NVP(m_name),
					CEREAL_NVP(m_file_path)
				);
			}
		}

		// ==============================================
		// private variables
		// ==============================================
		// アセット名、ファイル名が変更されたら、保存時に以前のファイルを消して別名で保存する
		std::string m_loaded_filepath;

		bool m_has_name = false;
		bool m_has_path = false;
		std::string m_name;
		std::string m_file_path;
		unsigned m_hash = 0;
	};

	// ------------------------------------------
	// inline
	// ------------------------------------------
	inline bool AssetDataBase::IsLoading() const
	{
		return m_is_loading;
	}

	inline bool AssetDataBase::IsLoaded() const
	{
		return m_is_loaded;
	}

	inline bool AssetDataBase::HasResource() const
	{
		return m_has_resource;
	}

	inline void AssetDataBase::SetName(const std::string& name)
	{
		m_has_name = true;

		if (name == m_name)
			return;

		m_name = name;
		// ファイルパスを持っていないときは名前からハッシュを作成
		if (!m_has_path)
		{
			m_hash = std::hash<std::string>()(m_name);
		}
	}

	inline const char* AssetDataBase::GetName() const
	{
		return m_has_name == true ? m_name.c_str() : m_file_path.c_str();
	}

	inline void AssetDataBase::SetFilePath(const std::string& filepath)
	{
		m_has_path = true;

		if (filepath == m_file_path)
			return;
		if (m_file_path.empty())
		{
			m_loaded_filepath = filepath;
		}
		// 名前を持っていないならファイル名から作成
		if (!m_has_name)
		{
			SetName(MyFunc::SplitFileName(filepath));
		}
		m_file_path = filepath;
		m_hash = std::hash<std::string>()(m_file_path);
	}

	inline const char* AssetDataBase::GetFilePath() const
	{
		return m_has_path ? m_file_path.c_str() : "";
	}

	inline const std::string& AssetDataBase::GetLoadedFilePath() const
	{
		return m_loaded_filepath;
	}

	inline unsigned AssetDataBase::GetHash() const
	{
		return m_hash;
	}

}// namespace TKGEngine

// AssetDataBase
CEREAL_REGISTER_TYPE(TKGEngine::AssetDataBase)
CEREAL_CLASS_VERSION(TKGEngine::AssetDataBase, 3)