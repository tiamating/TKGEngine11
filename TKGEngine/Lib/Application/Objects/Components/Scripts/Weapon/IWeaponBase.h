#pragma once
#include "Components/inc/CMonoBehaviour.h"

namespace TKGEngine
{
	class ParticleSystem;

	/// <summary>
	/// �e�̃x�[�X�N���X
	/// </summary>
	class IWeaponBase
		: public MonoBehaviour
	{
	public:
		IWeaponBase() = default;
		virtual ~IWeaponBase() = default;
		IWeaponBase(const IWeaponBase&) = delete;
		IWeaponBase& operator=(const IWeaponBase&) = delete;
		IWeaponBase& operator=(IWeaponBase&&) = default;

		// ����
		virtual void Shot(const VECTOR3& origin, const VECTOR3& direction) = 0;
		// ���ˉ\���`�F�b�N
		virtual bool CheckCanShot() = 0;

		// �����[�h�J�n
		virtual void BeginReload() = 0;
		// �����[�h����
		virtual void CompleteReload() = 0;
		// �����[�h�I��
		virtual void EndReload() = 0;
		// �����[�h�L�����Z��
		virtual void CancelReload() = 0;
		// �����[�h�\���`�F�b�N
		virtual bool CheckCanReload() = 0;
		// �����[�h��
		bool OnReloading() const { return m_on_reloading; }

		
		// �}�K�W�����c�e�����擾
		virtual int GetRemainBulletNum() const = 0;

	protected:
		// �}�Y���t���b�V���p�p�[�e�B�N��
		SWPtr<ParticleSystem> m_muzzle_flush;
		// �q�b�g�p�p�[�e�B�N��
		SWPtr<ParticleSystem> m_hit_effect;
		
		// �����[�h���t���O
		bool m_on_reloading = false;

	private:
		/////////////////////////////////////////////////////////////////////////
		// Serialize�ݒ�
		/////////////////////////////////////////////////////////////////////////
		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			if (version == 1)
			{
				archive(
					cereal::base_class<TKGEngine::MonoBehaviour>(this)
				);
			}
		}
	};


}
/////////////////////////////////////////////////////////////////////////
// Serialize�o�^
/////////////////////////////////////////////////////////////////////////
CEREAL_CLASS_VERSION(TKGEngine::IWeaponBase, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::IWeaponBase, "TKGEngine::IWeaponBase")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::IWeaponBase)
