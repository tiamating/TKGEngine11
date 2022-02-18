#pragma once

#include <memory>
#include <unordered_map>
#include <string>

#include <mutex>

namespace TKGEngine
{
	template <class T>
	class ResourceManager
	{
	public:
		// =================================
		// public methods
		// =================================
		/// ------------------------------------------
		/// @brief	set Resource Data to cache
		/// @param[in]	name	name
		/// @param[in]	res		shared_ptr Resource Data
		/// 
		/// ------------------------------------------
		void Set(const std::string& name, const std::shared_ptr<T>& res);

		/// ------------------------------------------
		/// @brief	search from Resource cache
		/// @param[in] name
		/// @retval non empty	successful find Resource from cache
		/// @retval empty		failed find Resource from cache
		/// 
		/// ------------------------------------------
		std::shared_ptr<T> Search(const std::string& name);

		/// ------------------------------------------
		/// @brief				remove a resource by filename from cache
		/// @param[in] tex_name	File Name to search for cache
		/// 
		/// ------------------------------------------
		void RemoveCache(const std::string& name);

		/// ------------------------------------------
		/// @brief				remove a unused resource from cache
		/// ------------------------------------------
		void RemoveUnusedCache();


		// =================================
		// public variables
		// =================================
		/* nothing */

	private:
		// =================================
		// private methods
		// =================================
		/* nothing */

		// =================================
		// private variables
		// =================================
		std::unordered_map<std::string, std::shared_ptr<T>> m_caches;	//!< cache
	};
	///////////////////////////////////////////////////////////////////
	//
	//					 inline
	//
	///////////////////////////////////////////////////////////////////
	template <class T>
	void ResourceManager<T>::Set(const std::string& name, const std::shared_ptr<T>& res)
	{
		m_caches.emplace(name, res);
	}

	template <class T>
	std::shared_ptr<T> ResourceManager<T>::Search(const std::string& name)
	{
		const auto itr_find = m_caches.find(name);
		if (itr_find != m_caches.end())
		{
			return itr_find->second;
		}
		return std::shared_ptr<T>();
	}

	template <class T>
	void ResourceManager<T>::RemoveCache(const std::string& name)
	{
		m_caches.erase(name);
	}

	template<class T>
	inline void ResourceManager<T>::RemoveUnusedCache()
	{
		const auto itr_end = m_caches.end();
		for (auto itr = m_caches.begin(); itr != itr_end;)
		{
			if (itr->second.use_count() <= 1)
			{
				itr = m_caches.erase(itr);
				continue;
			}
			++itr;
		}
	}


}// namespace TKGEngine