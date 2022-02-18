#pragma once

#include "Utility/inc/myfunc_vector.h"

namespace TKGEngine
{
	// ==============================================
	// ���C�g�̌`��
	// ==============================================
	enum class LightType
	{
		Directional = 0,
		Spot,
		Point,
	};

	// ==============================================
	// ���C�g���e�����Ƃ��̏��
	// ==============================================
	struct LightShadowData
	{
		VECTOR3 light_direction = VECTOR3::Forward;	// ���C�g����
		VECTOR3 light_up = VECTOR3::Up;	// ���C�g�̃A�b�v�x�N�g��
		float near_plane = 0.1f;	// �e��`�悷��ߕ���
		float far_plane = 100.0f;	// �e��`�悷�鉓����(���s�����ł̓t���X�^���̉�����)
		float spot_angle = 30.0f;	// �X�|�b�g���C�g�̍L����p�x
	};

	
}
