#pragma once

#include "Utility/inc/myfunc_vector.h"
#include "Utility/inc/myfunc_math.h"

#include "Application/inc/ProjectSetting.h"

#include <vector>
#include <memory>


// ==============================================
// �O���錾
// ==============================================
class btConvexShape;
class btCollisionObject;

namespace TKGEngine
{
	class Collider;
	class RigidBody;
}

namespace TKGEngine
{
	/// <summary>
	/// �Փ˔���p�֐��Q
	/// </summary>
	class Physics
	{
	public:
		// ���C�L���X�g�ɂ����𓾂邽�߂̍\����
		struct RaycastHit
		{
			// �Փ˂������[���h���W
			VECTOR3 position = VECTOR3::Zero;
			// �Փ˂������[���h�@��
			VECTOR3 normal = VECTOR3::Zero;
			// �Փˈʒu�܂ł̋���
			float distance = 0.0f;
			float hit_fraction = 0.0f;
			// �Փ˂�������
			std::shared_ptr<Collider> collider = nullptr;
			// ���̂Ȃ�擾�\
			std::shared_ptr<RigidBody> rigidbody = nullptr;
		};

#pragma region Raycast
		/// <summary>
		/// �n�߂Ƀ��C�ƏՓ˂����I�u�W�F�N�g�𓾂�
		/// </summary>
		/// <param name="origin">���[���h���W�ł̃��C�̎n�_</param>
		/// <param name="direction">���C�̕���</param>
		/// <param name="max_distance">���C�̏Փ˂����m����ő勗��</param>
		/// <param name="is_hit_trigger">�g���K�[���������邩</param>
		/// <param name="layer_mask">�Փ˂��郌�C���[</param>
		/// <returns>collide : true</returns>
		static bool Raycast(
			const VECTOR3& origin,
			const VECTOR3& direction,
			const float max_distance,
			const bool is_hit_trigger = false,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
		/// <summary>
		/// �n�߂Ƀ��C�ƏՓ˂����I�u�W�F�N�g�𓾂�
		/// </summary>
		/// <param name="origin">���[���h���W�ł̃��C�̎n�_</param>
		/// <param name="direction">���C�̕���</param>
		/// <param name="hit_info">�߂�ltrue�̎��ɁA�Փ˂����R���C�_�[�̏��</param>
		/// <param name="max_distance">���C�̏Փ˂����m����ő勗��</param>
		/// <param name="is_hit_trigger">�g���K�[���������邩</param>
		/// <param name="layer_mask">�Փ˂��郌�C���[</param>
		/// <returns>collide : true</returns>
		static bool Raycast(
			const VECTOR3& origin,
			const VECTOR3& direction,
			RaycastHit& hit_info,
			const float max_distance,
			const bool is_hit_trigger = false,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
		/// <summary>
		/// ���C�ƏՓ˂������ׂẴI�u�W�F�N�g�������\�[�g�������̔z��𓾂�
		/// </summary>
		/// <param name="origin">���[���h���W�ł̃��C�̎n�_</param>
		/// <param name="direction">���C�̕���</param>
		/// <param name="hit_infos">�߂�ltrue�̎��ɁA�Փ˂����R���C�_�[�̏��</param>
		/// <param name="max_distance">���C�̏Փ˂����m����ő勗��</param>
		/// <param name="is_hit_trigger">�g���K�[���������邩</param>
		/// <param name="layer_mask">�Փ˂��郌�C���[</param>
		/// <returns>collide : true</returns>
		static bool RaycastAll(
			const VECTOR3& origin,
			const VECTOR3& direction,
			std::vector<RaycastHit>& hit_infos,
			const float max_distance,
			const bool is_hit_trigger = false,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
#pragma endregion
		// ~Raycast

#pragma region Line
		/// <summary>
		/// �����ƏՓ˂��邩
		/// </summary>
		/// <param name="start">���[���h���W�ł̃��C�̎n�_</param>
		/// <param name="end">���[���h���W�ł̃��C�̏I�_</param>
		/// <param name="is_hit_trigger">�g���K�[���������邩</param>
		/// <param name="layer_mask">�Փ˂��郌�C���[</param>
		/// <returns>collide : true</returns>
		static bool Linecast(
			const VECTOR3& start,
			const VECTOR3& end,
			const bool is_hit_trigger = false,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
		/// <summary>
		/// �����ƏՓ˂��邩
		/// </summary>
		/// <param name="start">���[���h���W�ł̃��C�̎n�_</param>
		/// <param name="end">���[���h���W�ł̃��C�̏I�_</param>
		/// <param name="hit_info">�߂�ltrue�̎��ɁA�Փ˂����R���C�_�[�̏��</param>
		/// <param name="is_hit_trigger">�g���K�[���������邩</param>
		/// <param name="layer_mask">�Փ˂��郌�C���[</param>
		/// <returns>collide : true</returns>
		static bool Linecast(
			const VECTOR3& start,
			const VECTOR3& end,
			RaycastHit& hit_info,
			const bool is_hit_trigger = false,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
#pragma endregion
		// ~Line

#pragma region Sphere
		/// <summary>
		/// �����ړ����������ɏՓ˂��邩���ׂ�
		/// </summary>
		/// <param name="origin">���[���h���W�ł̋��̎n�_</param>
		/// <param name="radius">���̔��a</param>
		/// <param name="direction">���̕���</param>
		/// <param name="max_distance">�X�C�[�v�̍ő�̒���</param>
		/// <param name="is_hit_trigger">�g���K�[���������邩</param>
		/// <param name="ccd_penetration">�Փ˂������ƂɂȂ�Ȃ��߂荞�ݗ�</param>
		/// <param name="layer_mask">�Փ˂��郌�C���[</param>
		/// <returns>collide : true</returns>
		static bool SphereCast(
			const VECTOR3& origin,
			const float radius,
			const VECTOR3& direction,
			const float max_distance,
			const bool is_hit_trigger = false,
			const float ccd_penetration = 0.f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
		/// <summary>
		/// �����ړ����������ɏՓ˂��邩���ׂ�
		/// </summary>
		/// <param name="origin">���[���h���W�ł̋��̎n�_</param>
		/// <param name="radius">���̔��a</param>
		/// <param name="direction">���̕���</param>
		/// <param name="max_distance">�X�C�[�v�̍ő�̒���</param>
		/// <param name="hit_info">�߂�ltrue�̎��ɁA�Փ˂����R���C�_�[�̏��</param>
		/// <param name="is_hit_trigger">�g���K�[���������邩</param>
		/// <param name="ccd_penetration">�Փ˂������ƂɂȂ�Ȃ��߂荞�ݗ�</param>
		/// <param name="layer_mask">�Փ˂��郌�C���[</param>
		/// <returns>collide : true</returns>
		static bool SphereCast(
			const VECTOR3& origin,
			const float radius,
			const VECTOR3& direction,
			const float max_distance,
			RaycastHit& hit_info,
			const bool is_hit_trigger = false,
			const float ccd_penetration = 0.f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);

		/// <summary>
		/// �w�肵���ʒu�ɋ��𐶐����ăq�b�g���邩���ׂ�
		/// </summary>
		/// <param name="origin">���[���h���W�ł̋��̎n�_</param>
		/// <param name="radius">���̔��a</param>
		/// <param name="is_hit_trigger">�g���K�[���������邩</param>
		/// <param name="threshold">�Փ˂���������Ƃ鋗����臒l</param>
		/// <param name="layer_mask">�Փ˂��郌�C���[</param>
		/// <returns>collide : true</returns>
		static bool CheckSphere(
			const VECTOR3& origin,
			const float radius,
			const bool is_hit_trigger = false,
			const float threshold = 0.0f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);

		/// <summary>
		/// ���Əd�Ȃ��Ă���R���C�_�[���擾����
		/// </summary>
		/// <param name="origin">���[���h���W�ł̋��̎n�_</param>
		/// <param name="radius">���̔��a</param>
		/// <param name="hit_infos">�߂�ltrue�̎��ɁA�Փ˂����R���C�_�[�̏��</param>
		/// <param name="is_hit_trigger">�g���K�[���������邩</param>
		/// <param name="threshold">�Փ˂���������Ƃ鋗����臒l</param>
		/// <param name="layer_mask">�Փ˂��郌�C���[</param>
		/// <returns>collide : true</returns>
		static bool OverlapSphere(
			const VECTOR3& origin,
			const float radius,
			std::vector<RaycastHit>& hit_infos,
			const bool is_hit_trigger = false,
			const float threshold = 0.0f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
#pragma endregion
		// ~Sphere

#pragma region Box
		/// <summary>
		/// Box���ړ����������ɏՓ˂��邩���ׂ�
		/// </summary>
		/// <param name="start_center">�{�b�N�X�̎n�_���S</param>
		/// <param name="start_rot">�{�b�N�X�̎n�_�p��</param>
		/// <param name="end_center">�{�b�N�X�̏I�_���S</param>
		/// <param name="end_rot">�{�b�N�X�̏I�_�p��</param>
		/// <param name="half_extents">�{�b�N�X�̔����͈�</param>
		/// <param name="is_hit_trigger">�g���K�[���������邩</param>
		/// <param name="ccd_penetration">�Փ˂������ƂɂȂ�Ȃ��߂荞�ݗ�</param>
		/// <param name="layer_mask">�Փ˂��郌�C���[</param>
		/// <returns>collide : true</returns>
		static bool BoxCast(
			const VECTOR3& start_center,
			const Quaternion& start_rot,
			const VECTOR3& end_center,
			const Quaternion& end_rot,
			const VECTOR3& half_extents,
			const bool is_hit_trigger = false,
			const float ccd_penetration = 0.f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
		/// <summary>
		/// Box���ړ����������ɏՓ˂��邩���ׂ�
		/// </summary>
		/// <param name="start_center">�{�b�N�X�̎n�_���S</param>
		/// <param name="start_rot">�{�b�N�X�̎n�_�p��</param>
		/// <param name="end_center">�{�b�N�X�̏I�_���S</param>
		/// <param name="end_rot">�{�b�N�X�̏I�_�p��</param>
		/// <param name="half_extents">�{�b�N�X�̔����͈�</param>
		/// <param name="hit_info">�߂�ltrue�̎��ɁA�Փ˂����R���C�_�[�̏��</param>
		/// <param name="is_hit_trigger">�g���K�[���������邩</param>
		/// <param name="ccd_penetration">�Փ˂������ƂɂȂ�Ȃ��߂荞�ݗ�</param>
		/// <param name="layer_mask">�Փ˂��郌�C���[</param>
		/// <returns>collide : true</returns>
		static bool BoxCast(
			const VECTOR3& start_center,
			const Quaternion& start_rot,
			const VECTOR3& end_center,
			const Quaternion& end_rot,
			const VECTOR3& half_extents,
			RaycastHit& hit_info,
			const bool is_hit_trigger = false,
			const float ccd_penetration = 0.f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);

		/// <summary>
		/// �w�肵���ʒu��Box�𐶐����ăq�b�g���邩���ׂ�
		/// </summary>
		/// <param name="center">�{�b�N�X�̒��S</param>
		/// <param name="rot">�{�b�N�X�̎p��</param>
		/// <param name="half_extents">�{�b�N�X�̔����͈�</param>
		/// <param name="is_hit_trigger">�g���K�[���������邩</param>
		/// <param name="threshold">�Փ˂���������Ƃ鋗����臒l</param>
		/// <param name="layer_mask">�Փ˂��郌�C���[</param>
		/// <returns>collide : true</returns>
		static bool CheckBox(
			const VECTOR3& center,
			const Quaternion& rot,
			const VECTOR3& half_extents,
			const bool is_hit_trigger = false,
			const float threshold = 0.0f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);

		/// <summary>
		/// Box�Əd�Ȃ��Ă���R���C�_�[���擾����
		/// </summary>
		/// <param name="center">�{�b�N�X�̒��S</param>
		/// <param name="rot">�{�b�N�X�̎p��</param>
		/// <param name="half_extents">�{�b�N�X�̔����͈�</param>
		/// <param name="hit_infos">�߂�ltrue�̎��ɁA�Փ˂����R���C�_�[�̏��</param>
		/// <param name="is_hit_trigger">�g���K�[���������邩</param>
		/// <param name="threshold">�Փ˂���������Ƃ鋗����臒l</param>
		/// <param name="layer_mask">�Փ˂��郌�C���[</param>
		/// <returns>collide : true</returns>
		static bool OverlapBox(
			const VECTOR3& center,
			const Quaternion& rot,
			const VECTOR3& half_extents,
			std::vector<RaycastHit>& hit_infos,
			const bool is_hit_trigger = false,
			const float threshold = 0.0f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
#pragma endregion
		// ~Box

#pragma region Capsule
		/// <summary>
		/// Capsule���ړ����������ɏՓ˂��邩���ׂ�
		/// </summary>
		/// <param name="point_1">�J�v�Z���ɂ��鋅�̂̒��S</param>
		/// <param name="point_2">�J�v�Z���ɂ��鋅�̂̒��S</param>
		/// <param name="radius">�J�v�Z���̔��a</param>
		/// <param name="direction">�J�v�Z���̒ʉߕ���</param>
		/// <param name="max_distance">�X�C�[�v�̍ő�̒���</param>
		/// <param name="is_hit_trigger">�g���K�[���������邩</param>
		/// <param name="ccd_penetration">�Փ˂������ƂɂȂ�Ȃ��߂荞�ݗ�</param>
		/// <param name="layer_mask">�Փ˂��郌�C���[</param>
		/// <returns>collide : true</returns>
		static bool CapsuleCast(
			const VECTOR3& point_1,
			const VECTOR3& point_2,
			const float radius,
			const VECTOR3& direction,
			const float max_distance,
			const bool is_hit_trigger = false,
			const float ccd_penetration = 0.f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
		/// <summary>
		/// Capsule���ړ����������ɏՓ˂��邩���ׂ�
		/// </summary>
		/// <param name="point_1">�J�v�Z���ɂ��鋅�̂̒��S</param>
		/// <param name="point_2">�J�v�Z���ɂ��鋅�̂̒��S</param>
		/// <param name="radius">�J�v�Z���̔��a</param>
		/// <param name="direction">�J�v�Z���̒ʉߕ���</param>
		/// <param name="max_distance">�X�C�[�v�̍ő�̒���</param>
		/// <param name="hit_info">�߂�ltrue�̎��ɁA�Փ˂����R���C�_�[�̏��</param>
		/// <param name="is_hit_trigger">�g���K�[���������邩</param>
		/// <param name="ccd_penetration">�Փ˂������ƂɂȂ�Ȃ��߂荞�ݗ�</param>
		/// <param name="layer_mask">�Փ˂��郌�C���[</param>
		/// <returns>collide : true</returns>
		static bool CapsuleCast(
			const VECTOR3& point_1,
			const VECTOR3& point_2,
			const float radius,
			const VECTOR3& direction,
			const float max_distance,
			RaycastHit& hit_info,
			const bool is_hit_trigger = false,
			const float ccd_penetration = 0.f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);

		/// <summary>
		/// �w�肵���ʒu��Capsule�𐶐����ăq�b�g���邩���ׂ�
		/// </summary>
		/// <param name="point_1">�J�v�Z���ɂ��鋅�̂̒��S</param>
		/// <param name="point_2">�J�v�Z���ɂ��鋅�̂̒��S</param>
		/// <param name="radius">�J�v�Z���̔��a</param>
		/// <param name="is_hit_trigger">�g���K�[���������邩</param>
		/// <param name="threshold">�Փ˂���������Ƃ鋗����臒l</param>
		/// <param name="layer_mask">�Փ˂��郌�C���[</param>
		/// <returns>collide : true</returns>
		static bool CheckCapsule(
			const VECTOR3& point_1,
			const VECTOR3& point_2,
			const float radius,
			const bool is_hit_trigger = false,
			const float threshold = 0.0f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);

		/// <summary>
		/// Capsule�Əd�Ȃ��Ă���R���C�_�[���擾����
		/// </summary>
		/// <param name="point_1">�J�v�Z���ɂ��鋅�̂̒��S</param>
		/// <param name="point_2">�J�v�Z���ɂ��鋅�̂̒��S</param>
		/// <param name="radius">�J�v�Z���̔��a</param>
		/// <param name="hit_infos">�߂�ltrue�̎��ɁA�Փ˂����R���C�_�[�̏��</param>
		/// <param name="is_hit_trigger">�g���K�[���������邩</param>
		/// <param name="threshold">�Փ˂���������Ƃ鋗����臒l</param>
		/// <param name="layer_mask">�Փ˂��郌�C���[</param>
		/// <returns>collide : true</returns>
		static bool OverlapCapsule(
			const VECTOR3& point_1,
			const VECTOR3& point_2,
			const float radius,
			std::vector<RaycastHit>& hit_infos,
			const bool is_hit_trigger = false,
			const float threshold = 0.0f,
			const int layer_mask = static_cast<int>(Layer::AllFilter)
		);
#pragma endregion
		// ~Capsule

		private:
			// �ŋߖT�̓ʕ�Փˏ����擾����
			static bool ClosestConvexCast(
				const btConvexShape* shape,
				const VECTOR3& start_pos,
				const Quaternion& start_rot,
				const VECTOR3& end_pos,
				const Quaternion& end_rot,
				const bool is_hit_trigger,
				const float ccd_penetration,
				const int layer_mask
			);
			// �ŋߖT�̓ʕ�Փˏ����擾����
			static bool ClosestConvexCast(
				RaycastHit& hit_info,
				const btConvexShape* shape,
				const VECTOR3& start_pos,
				const Quaternion& start_rot,
				const VECTOR3& end_pos,
				const Quaternion& end_rot, 
				const bool is_hit_trigger,
				const float ccd_penetration,
				const int layer_mask
			);

			// �ʕ�Փ˂��邩�ǂ���
			static bool CheckConvex(
				btCollisionObject* col_obj,
				const bool is_hit_trigger,
				const float threshold,
				const int layer_mask
			);

			// �d�Ȃ�I�u�W�F�N�g�̏���Ԃ�
			static bool OverlapConvex(
				std::vector<RaycastHit>& hit_infos,
				btCollisionObject* col_obj,
				const bool is_hit_trigger,
				const float threshold,
				const int layer_mask
			);

			// �Փˈʒu�Ƀf�o�b�O�`����s���Ƃ��ɌĂԊ֐�
			static void RenderHitShape(btConvexShape* shape, const VECTOR3& pos, const Quaternion& rot);

			// Debug�`��F
			static const VECTOR3 DEBUG_COLOR;
			// �Փˎ�Debug�`��F
			static const VECTOR3 DEBUG_HIT_COLOR;
			// �Փ˓_Debug�`��F
			static const VECTOR3 DEBUG_CONTACT_POINT_COLOR;
	};


}// namespace TKGEngine