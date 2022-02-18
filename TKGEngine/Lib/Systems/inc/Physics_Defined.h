#pragma once

#include <btBulletCollisionCommon.h>
#include "../../Utility/inc/myfunc_vector.h"

namespace TKGEngine
{
	// ==============================================
	// Utility function
	// ==============================================
	// btVector3 --> VECTOR3
	inline void ConvertbtVectorToVector(const btVector3& bt, VECTOR3& vec)
	{
		vec.x = static_cast<float>(bt.x());
		vec.y = static_cast<float>(bt.y());
		vec.z = static_cast<float>(bt.z());
	}
	inline VECTOR3 ConvertbtVectorToVector(const btVector3& bt)
	{
		VECTOR3 ret_val;
		ret_val.x = static_cast<float>(bt.x());
		ret_val.y = static_cast<float>(bt.y());
		ret_val.z = static_cast<float>(bt.z());
		return ret_val;
	}
	// VECTOR3 --> btVector3
	inline void ConvertVectorTobtVector(const VECTOR3& vec, btVector3& bt)
	{
		bt.setValue(btScalar(vec.x), btScalar(vec.y), btScalar(vec.z));
	}
	inline btVector3 ConvertVectorTobtVector(const VECTOR3& vec)
	{
		btVector3 ret_val(vec.x, vec.y, vec.z);
		return ret_val;
	}

	// btQuaternion --> Quaternion
	inline void ConvertbtQuaternionToQuaternion(const btQuaternion& bt_quat, Quaternion& quat)
	{
		quat.x = static_cast<float>(bt_quat.x());
		quat.y = static_cast<float>(bt_quat.y());
		quat.z = static_cast<float>(bt_quat.z());
		quat.w = static_cast<float>(bt_quat.w());
	}
	inline Quaternion ConvertbtQuaternionToQuaternion(const btQuaternion& bt_quat)
	{
		Quaternion ret_val;
		ret_val.x = static_cast<float>(bt_quat.x());
		ret_val.y = static_cast<float>(bt_quat.y());
		ret_val.z = static_cast<float>(bt_quat.z());
		ret_val.w = static_cast<float>(bt_quat.w());
		return ret_val;
	}
	// Quaternion --> btQuaternion
	inline void ConvertQuaternionTobtQuaternion(const Quaternion& quat, btQuaternion& bt_quat)
	{
		bt_quat.setX(quat.x);
		bt_quat.setY(quat.y);
		bt_quat.setZ(quat.z);
		bt_quat.setW(quat.w);
	}
	inline btQuaternion ConvertQuaternionTobtQuaternion(const Quaternion& quat)
	{
		btQuaternion ret_val(quat.x, quat.y, quat.z, quat.w);
		return ret_val;
	}

	// Origin & Rotation --> btTransform
	inline btTransform CreatebtTransform(const VECTOR3& translation, const Quaternion& rotation)
	{
		btTransform ret_val;
		ret_val.setOrigin(ConvertVectorTobtVector(translation));
		ret_val.setRotation(ConvertQuaternionTobtQuaternion(rotation));
		return ret_val;
	}


	// ==============================================
	// Definition
	// ==============================================
	// �`��f�[�^��ID
	typedef int ShapeID;

	/// <summary>
	/// �R���W�����`��̎��
	/// </summary>
	enum class ShapeType
	{
		Box = 0,		// �{�b�N�X
		Sphere,		// ��
		Capsule,	// �J�v�Z��
		Cylinder,	// �~��
		Cone,		// �~��

		StaticMesh,		// ���b�V�� TODO : ���b�V���R���C�_�[�Ή�
		ConvexPolytope,	// TODO : Mesh���ނ��́ADynamic�Ɏg�p�\

		MAX_ShapeType
	};



}// namespace TKGEngine