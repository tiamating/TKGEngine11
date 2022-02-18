#pragma once

namespace TKGEngine
{
	/// <summary>
	/// Shared_ptrとWeak_ptrを1つで管理するポインタ管理クラス
	/// </summary>
	template <typename Ty>
	class SWPtr
	{
	public:
		SWPtr() = default;
		~SWPtr() = default;
		SWPtr(const SWPtr&) = delete;
		SWPtr(SWPtr&&) = default;

		/// <summary>
		/// ポインタの代入
		/// </summary>
		SWPtr& operator=(const SWPtr& ptr)
		{
			// ポインタをセット
			Set(ptr.m_weak_ptr);
			return *this;
		}
		SWPtr& operator=(const std::weak_ptr<Ty>& ptr)
		{
			// ポインタをセット
			Set(ptr);
			return *this;
		}

		/// <summary>
		/// Shared_Ptrのオブジェクトにアクセス
		/// </summary>
		Ty* operator->() const
		{
			return m_shared_ptr.get();
		}

		/// <summary>
		/// Shared_Ptrの実体の間接参照
		/// </summary>
		Ty& operator*() const
		{
			return *m_shared_ptr.get();
		}

		/// <summary>
		/// boolへのキャスト
		/// </summary>
		explicit operator bool() const
		{
			return m_shared_ptr != nullptr;
		}

		/// <summary>
		/// 管理するポインタを登録
		///	管理しているものがあれば破棄する
		/// </summary>
		void Set(const std::weak_ptr<Ty>& ptr)
		{
			m_weak_ptr = ptr;
			m_shared_ptr.reset();
		}

		/// <summary>
		/// Shared_Ptrを取得する
		/// </summary>
		const std::shared_ptr<Ty>& Lock()
		{
			m_shared_ptr = m_weak_ptr.lock();
			return m_shared_ptr;
		}

		/// <summary>
		/// Shared_Ptrの所有権を破棄
		/// </summary>
		void Unlock()
		{
			m_shared_ptr.reset();
		}

		const std::shared_ptr<Ty>& GetShared()
		{
			return m_shared_ptr;
		}

		const std::weak_ptr<Ty>& GetWeak()
		{
			return m_weak_ptr;
		}

	private:
		std::weak_ptr<Ty> m_weak_ptr;
		std::shared_ptr<Ty> m_shared_ptr = nullptr;
	};

}