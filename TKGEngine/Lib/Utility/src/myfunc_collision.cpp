
#include "Utility/inc/myfunc_collision.h"

namespace TKGEngine::MyFunc
{
	// =============================================================
	// �w���p�[�֐�
	// =============================================================
	/// <summary>
	/// AABB vs Frustum�̃X�C�[�v�e�X�g
	/// </summary>
	/// <param name="aabb">AABB</param>
	/// <param name="frustum_points">Frustum</param>
	/// <param name="face_norm_dir">�ʖ@��</param>
	/// <param name="sweep_dir">�X�C�[�v����</param>
	/// <param name="dist_min">�����ʒu�܂ł̍ŒZ</param>
	/// <param name="dist_max">�����ʒu�܂ł̍Œ�</param>
	/// <returns></returns>
	bool ProjectedSweepIntersection(
		const Bounds& aabb, const VECTOR3 (&frustum_points)[8],
		const VECTOR3& face_norm_dir, const VECTOR3& sweep_dir,
		float& dist_min, float& dist_max
	)
	{
		const VECTOR3 center_pos = aabb.GetCenter();
		const VECTOR3 half_extents = aabb.GetExtents();
		// �X�C�[�v�̖@����������
		const float sweep = VECTOR3::Dot(sweep_dir, face_norm_dir);
		// AABB�̒��S�_���x�N�g���ɓ��e
		const float center = VECTOR3::Dot(center_pos, face_norm_dir);
		// AABB�̔��͈͂��x�N�g���ɓ��e
		const float half_size = 
			half_extents.x * fabs(face_norm_dir.x) +
			half_extents.y * fabs(face_norm_dir.y) +
			half_extents.z * fabs(face_norm_dir.z);
		// AABB�̍ő�A�ŏ�
		const float min_1 = center - half_size;
		const float max_1 = center + half_size;

		// �t���X�^���̓_�𓊉e
		float project = VECTOR3::Dot(frustum_points[0], face_norm_dir);
		float min_2 = project;
		float max_2 = project;
		for (int i = 1; i < 8; i++)
		{
			project = VECTOR3::Dot(frustum_points[i], face_norm_dir);
			min_2 = (std::min)(project, min_2);
			max_2 = (std::max)(project, max_2);
		}

		// �X�C�[�v�������ɉe������
		if (sweep != 0)
		{
			// max_1 + sweep * t >= min_2�̂Ƃ��������J�n
			float intersection_start = (min_2 - max_1) / sweep;
			// min_1 + sweep * t >= max_2�̂Ƃ��������I��
			float intersection_end = (max_2 - min_1) / sweep;

			// �͈͂̏������`�F�b�N
			if (intersection_start > intersection_end)
			{
				const auto buf = intersection_start;
				intersection_start = intersection_end;
				intersection_end = buf;
			}

			// �����͈̔͂��d�Ȃ�Ȃ�
			if (dist_min > intersection_end || intersection_start > dist_max)
			{
				return false;
			}

			// �͈͂��X�V����
			dist_min = (std::max)(dist_min, intersection_start);
			dist_max = (std::min)(dist_max, intersection_end);
		}
		// �X�C�[�v�������ɉe�����Ȃ�
		else
		{
			// �������Ȃ�
			if (min_1 > max_2 || min_2 > max_1)
			{
				return false;
			}
		}

		return true;
	}

	// =============================================================
	// �X�C�[�v�e�X�g
	// =============================================================
	bool SweepIntersectionTest(const Bounds& object_aabb, const Bounds& frustum_aabb, const VECTOR3& sweep_dir)
	{
		const VECTOR3 frustum_min = frustum_aabb.GetCenter() - frustum_aabb.GetExtents();
		const VECTOR3 frustum_max = frustum_aabb.GetCenter() + frustum_aabb.GetExtents();
		const VECTOR3 object_min = object_aabb.GetCenter() - object_aabb.GetExtents();
		const VECTOR3 object_max = object_aabb.GetCenter() + object_aabb.GetExtents();

		// �X�C�[�v�����ɉ����Čv�Z����

		// Frustum
		// AABB�̒��S�_�𓊉e����
		const VECTOR3 frustum_center = (frustum_min + frustum_max) * 0.5f;
		const VECTOR3 frustum_half_size = (frustum_max - frustum_min) * 0.5f;
		const float frustum_center_proj = VECTOR3::Dot(frustum_center, sweep_dir);
		// AABB�̔��͈͂𓊉e����
		const float frustum_half_size_proj = frustum_half_size.x * fabs(sweep_dir.x) +
			frustum_half_size.y * fabs(sweep_dir.y) +
			frustum_half_size.z * fabs(sweep_dir.z);
		const float frustum_proj_min = frustum_center_proj - frustum_half_size_proj;
		const float frustum_proj_max = frustum_center_proj + frustum_half_size_proj;

		// Object
		// AABB�̒��S�_�𓊉e����
		const VECTOR3 object_center = (object_min + object_max) * 0.5f;
		const VECTOR3 object_half_size = (object_max - object_min) * 0.5f;
		const float object_center_proj = VECTOR3::Dot(object_center, sweep_dir);
		// AABB�̔��͈͂𓊉e����
		const float object_half_size_proj = object_half_size.x * fabs(sweep_dir.x) +
			object_half_size.y * fabs(sweep_dir.y) +
			object_half_size.z * fabs(sweep_dir.z);
		const float object_proj_min = object_center_proj - object_half_size_proj;
		const float object_proj_max = object_center_proj + object_half_size_proj;

		// �X�C�[�v�����̋��������߂�
		// ���ׂĂ̎��Ō�_����������ꏊ��T��

		// �X�C�[�v�����̌����_
		// starts: object_proj_max + dist = frustum_proj_min
		//   ends: object_proj_min + dist = frustum_proj_max
		float dist_min = frustum_proj_min - object_proj_max;
		float dist_max = frustum_proj_max - object_proj_min;
		if (dist_min > dist_max)
		{
			const auto buf = dist_min;
			dist_min = dist_max;
			dist_max = buf;
		}
		// �X�C�[�v�����̔��Α��ł̂݌����Ȃ�����Ȃ�
		if (dist_max < 0) return false;

		// ����̌����_
		// starts: object_max.x + dist*sweep_dir.x = frustum_min.x
		//   ends: object_min.x + dist*sweep_dir.x = frustum_max.x

		// X���e�X�g
		if(MyMath::Approximately(sweep_dir.x, 0.0f))
		{
			// ���̎���ɂ͌����Č�_�͂Ȃ�
			if (frustum_min.x > object_max.x || object_min.x > frustum_max.x) return false;
		}
		else
		{
			float dist_min_new = (frustum_min.x - object_max.x) / sweep_dir.x;
			float dist_max_new = (frustum_max.x - object_min.x) / sweep_dir.x;
			if (dist_min_new > dist_max_new)
			{
				const auto buf = dist_min_new;
				dist_min_new = dist_max_new;
				dist_max_new = buf;
			}
			// �����͈̔͂��d�Ȃ�Ȃ�
			if (dist_min > dist_max_new || dist_min_new > dist_max) return false;
			// �͈͂��X�V����
			dist_min = MyMath::Max(dist_min, dist_min_new);
			dist_max = MyMath::Min(dist_max, dist_max_new);
		}

		// Y���e�X�g
		if (MyMath::Approximately(sweep_dir.y, 0.0f))
		{
			// ���̎���ɂ͌����Č�_�͂Ȃ�
			if (frustum_min.y > object_max.y || object_min.y > frustum_max.y) return false;
		}
		else
		{
			float dist_min_new = (frustum_min.y - object_max.y) / sweep_dir.y;
			float dist_max_new = (frustum_max.y - object_min.y) / sweep_dir.y;
			if (dist_min_new > dist_max_new) 
			{
				const auto buf = dist_min_new;
				dist_min_new = dist_max_new;
				dist_max_new = buf;
			}
			// �����͈̔͂��d�Ȃ�Ȃ�
			if (dist_min > dist_max_new || dist_min_new > dist_max) return false;
			// �͈͂��X�V����
			dist_min = MyMath::Max(dist_min, dist_min_new);
			dist_max = MyMath::Min(dist_max, dist_max_new);
		}

		// Z���e�X�g
		if (MyMath::Approximately(sweep_dir.z, 0.0f))
		{
			// ���̎���ɂ͌����Č�_�͂Ȃ�
			if (frustum_min.z > object_max.z || object_min.z > frustum_max.z) return false;
		}
		else
		{
			float dist_min_new = (frustum_min.z - object_max.z) / sweep_dir.z;
			float dist_max_new = (frustum_max.z - object_min.z) / sweep_dir.z;
			if (dist_min_new > dist_max_new)
			{
				const auto buf = dist_min_new;
				dist_min_new = dist_max_new;
				dist_max_new = buf;
			}
			// �����͈̔͂��d�Ȃ�Ȃ�
			if (dist_min > dist_max_new || dist_min_new > dist_max) return false;
		}

		// ���ׂẴe�X�g�ɍ��i�Ȃ̂Ō����_������
		return true;
	}

	bool SweepIntersectionTest(const Bounds& object_aabb, const Frustum& frustum, const VECTOR3& sweep_dir)
	{
		VECTOR3 frustum_points[8];
		frustum.GetCorners(frustum_points);
		return SweepIntersectionTest(object_aabb, frustum_points, sweep_dir);
	}

	bool SweepIntersectionTest(const Bounds& object_aabb, const VECTOR3(&frustum_points)[8], const VECTOR3& sweep_dir)
	{
		float dist_min = 0.0f;
		float dist_max = FLT_MAX;

		// �X�C�[�v�����̌�_���͈͂�������
		if (!ProjectedSweepIntersection(object_aabb, frustum_points, sweep_dir, sweep_dir, dist_min, dist_max)) return false;

		// AABB�̖ʖ@��
		if (!ProjectedSweepIntersection(object_aabb, frustum_points, VECTOR3(1, 0, 0), sweep_dir, dist_min, dist_max)) return false;
		if (!ProjectedSweepIntersection(object_aabb, frustum_points, VECTOR3(0, 1, 0), sweep_dir, dist_min, dist_max)) return false;
		if (!ProjectedSweepIntersection(object_aabb, frustum_points, VECTOR3(0, 0, 1), sweep_dir, dist_min, dist_max)) return false;

		// frustum�̖ʖ@��

		// front and back faces
		const VECTOR3 face_norm1 = (VECTOR3::Cross(frustum_points[1] - frustum_points[0], frustum_points[3] - frustum_points[0])).Normalized();
		if (!ProjectedSweepIntersection(object_aabb, frustum_points, face_norm1, sweep_dir, dist_min, dist_max)) return false;

		// left face
		const VECTOR3 face_norm2 = (VECTOR3::Cross(frustum_points[3] - frustum_points[0], frustum_points[4] - frustum_points[0])).Normalized();
		if (!ProjectedSweepIntersection(object_aabb, frustum_points, face_norm2, sweep_dir, dist_min, dist_max)) return false;

		// right face
		const VECTOR3 face_norm3 = (VECTOR3::Cross(frustum_points[2] - frustum_points[1], frustum_points[5] - frustum_points[1])).Normalized();
		if (!ProjectedSweepIntersection(object_aabb, frustum_points, face_norm3, sweep_dir, dist_min, dist_max)) return false;

		// top face
		const VECTOR3 face_norm4 = (VECTOR3::Cross(frustum_points[1] - frustum_points[0], frustum_points[4] - frustum_points[0])).Normalized();
		if (!ProjectedSweepIntersection(object_aabb, frustum_points, face_norm4, sweep_dir, dist_min, dist_max)) return false;

		// bottom face
		const VECTOR3 face_norm5 = (VECTOR3::Cross(frustum_points[3] - frustum_points[2], frustum_points[7] - frustum_points[2])).Normalized();
		if (!ProjectedSweepIntersection(object_aabb, frustum_points, face_norm5, sweep_dir, dist_min, dist_max)) return false;


		// �t���X�^���̕ӂƂ̌���
		constexpr VECTOR3 box_edges[3] = { VECTOR3(1,0,0), VECTOR3(0,1,0), VECTOR3(0,0,1) };
		for (int i = 0; i < 3; i++)
		{
			// edge up-down
			VECTOR3 norm1 = (VECTOR3::Cross(frustum_points[3] - frustum_points[0], box_edges[i])).Normalized();
			if (!ProjectedSweepIntersection(object_aabb, frustum_points, norm1, sweep_dir, dist_min, dist_max)) return false;

			// edge left-right
			VECTOR3 norm2 = (VECTOR3::Cross(frustum_points[1] - frustum_points[0], box_edges[i])).Normalized();
			if (!ProjectedSweepIntersection(object_aabb, frustum_points, norm2, sweep_dir, dist_min, dist_max)) return false;

			// edge top left
			VECTOR3 norm4 = (VECTOR3::Cross(frustum_points[4] - frustum_points[0], box_edges[i])).Normalized();
			if (!ProjectedSweepIntersection(object_aabb, frustum_points, norm4, sweep_dir, dist_min, dist_max)) return false;

			// edge top right
			VECTOR3 norm5 = (VECTOR3::Cross(frustum_points[5] - frustum_points[1], box_edges[i])).Normalized();
			if (!ProjectedSweepIntersection(object_aabb, frustum_points, norm5, sweep_dir, dist_min, dist_max)) return false;

			// edge bottom right
			VECTOR3 norm6 = (VECTOR3::Cross(frustum_points[6] - frustum_points[2], box_edges[i])).Normalized();
			if (!ProjectedSweepIntersection(object_aabb, frustum_points, norm6, sweep_dir, dist_min, dist_max)) return false;

			// edge bottom left
			VECTOR3 norm3 = (VECTOR3::Cross(frustum_points[7] - frustum_points[3], box_edges[i])).Normalized();
			if (!ProjectedSweepIntersection(object_aabb, frustum_points, norm3, sweep_dir, dist_min, dist_max)) return false;
		}

		// ���ׂẴe�X�g�ɍ��i�Ȃ̂Ō����_������
		return true;
	}

}