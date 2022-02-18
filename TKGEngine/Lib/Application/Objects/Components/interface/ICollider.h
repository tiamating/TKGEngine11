#pragma once

namespace TKGEngine
{
	/// <summary>
	/// �R���C�_�[�p�C���^�[�t�F�[�X
	/// </summary>
	class ICollider
	{
	public:
		/// <summary>
		/// �Փˏ�Ԃ̃`�F�b�N���ɌĂ΂��
		/// </summary>
		/// <param name="other">�Փˑ����ID</param>
		virtual void AddTriggerHitID(int other) = 0;
		virtual void AddCollisionHitID(int other) = 0;

		// �Փˏ�Ԃ̍X�V
		virtual void UpdateHitState() = 0;

		// �Փˎ��̊֐����ĂԂ��ǂ���
		virtual bool IsCallOnFunction() const = 0;

		// Trigger���ǂ���
		virtual bool IsTrigger() const = 0;

		// Kinematic���ǂ���
		virtual bool IsKinematic() const = 0;

		// GameObjectID�̎擾
		virtual int GetGameObjectID() const = 0;

		// ComponentID�̎擾
		virtual int GetColliderID() const = 0;

		// Collider���L�����ǂ���
		virtual bool IsActiveCollider() = 0;

		// Collision�̃A�N�e�B�u��ω�������Ƃ��ɌĂ΂��
		virtual void OnSetCollisionActive(bool is_active) = 0;
	};
}