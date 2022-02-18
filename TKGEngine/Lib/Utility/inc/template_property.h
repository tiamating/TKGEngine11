#pragma once

#include <functional>
#include <cassert>

namespace TKGEngine
{
	// ====================================================
	// Get Set プロパティ用テンプレートクラス
	// ----------------------------------------------------
	// getter用ラムダ式		:	[this](){ Impl }
	// setter用ラムダ式		:	[this](T v){ Impl }
	// ====================================================
	template<class T>
	class Property
	{
	public:
		Property(T g(), void s(T))
			: getter(g), setter(s)
		{
			/* nothing */
		}
		Property(Property&&) = default;
		Property(const Property&) = delete;
		Property& operator=(const Property&) = delete;

		T get() const
		{
			return this->getter();
		}

		operator T()
		{
			return this->getter();
		}

		T operator ->()
		{
			return this->getter();
		}

		void operator=(const T v)
		{
			this->setter(v);
		}

	private:
		std::function<T()> getter = nullptr;
		std::function<void(T)> setter = nullptr;
	};

	// ====================================================
	// GetOnly プロパティ用テンプレートクラス
	// ----------------------------------------------------
	// デフォルトgetter	:	nullptr
	// getter用ラムダ式	:	[this](T v){ Impl }
	// ====================================================
	template<class T>
	class PropertyGet
	{
	public:
		PropertyGet(T g())
			: getter(g)
		{
			/* nothing */
		}
		PropertyGet(PropertyGet&&) = default;
		PropertyGet(const PropertyGet&) = delete;
		PropertyGet& operator=(const PropertyGet&) = delete;

		T get() const
		{
			return this->getter();
		}

		operator T()
		{
			return this->getter();
		}

		T operator ->()
		{
			return this->getter();
		}

		void operator=(const T v) = delete;

	private:
		std::function<T()> getter = nullptr;
	};
}