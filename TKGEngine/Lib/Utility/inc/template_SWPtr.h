#pragma once

namespace TKGEngine
{
	/// <summary>
	/// Shared_ptr��Weak_ptr��1�ŊǗ�����|�C���^�Ǘ��N���X
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
		/// �|�C���^�̑��
		/// </summary>
		SWPtr& operator=(const SWPtr& ptr)
		{
			// �|�C���^���Z�b�g
			Set(ptr.m_weak_ptr);
			return *this;
		}
		SWPtr& operator=(const std::weak_ptr<Ty>& ptr)
		{
			// �|�C���^���Z�b�g
			Set(ptr);
			return *this;
		}

		/// <summary>
		/// Shared_Ptr�̃I�u�W�F�N�g�ɃA�N�Z�X
		/// </summary>
		Ty* operator->() const
		{
			return m_shared_ptr.get();
		}

		/// <summary>
		/// Shared_Ptr�̎��̂̊ԐڎQ��
		/// </summary>
		Ty& operator*() const
		{
			return *m_shared_ptr.get();
		}

		/// <summary>
		/// bool�ւ̃L���X�g
		/// </summary>
		explicit operator bool() const
		{
			return m_shared_ptr != nullptr;
		}

		/// <summary>
		/// �Ǘ�����|�C���^��o�^
		///	�Ǘ����Ă�����̂�����Δj������
		/// </summary>
		void Set(const std::weak_ptr<Ty>& ptr)
		{
			m_weak_ptr = ptr;
			m_shared_ptr.reset();
		}

		/// <summary>
		/// Shared_Ptr���擾����
		/// </summary>
		const std::shared_ptr<Ty>& Lock()
		{
			m_shared_ptr = m_weak_ptr.lock();
			return m_shared_ptr;
		}

		/// <summary>
		/// Shared_Ptr�̏��L����j��
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