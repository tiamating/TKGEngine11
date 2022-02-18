#pragma once

#include <mutex>
#include <cassert>

namespace TKGEngine
{
	/// <summary>
	/// Systemクラスのアクセス用クラステンプレート
	/// </summary>
	template<class Ty>
	class SystemAccessor
	{
	public:
		static Ty& Get();

	protected:
		static void Init(Ty* ref);
		static void Release();

	private:
		static Ty* p_ref;
		static std::mutex mtx;
	};

	// --------------------------------------
	// 定義
	// --------------------------------------
	template <class Ty>
	Ty& SystemAccessor<Ty>::Get()
	{
		assert(p_ref != nullptr);
		std::lock_guard<std::mutex> lock(mtx);
		return *p_ref;
	}

	template <class Ty>
	void SystemAccessor<Ty>::Init(Ty* ref)
	{
		std::lock_guard<std::mutex> lock(mtx);
		p_ref = ref;
	}

	template <class Ty>
	void SystemAccessor<Ty>::Release()
	{
		std::lock_guard<std::mutex> lock(mtx);
		p_ref = nullptr;
	}

	template <class Ty>
	Ty* SystemAccessor<Ty>::p_ref = nullptr;

	template <class Ty>
	std::mutex SystemAccessor<Ty>::mtx;

}
