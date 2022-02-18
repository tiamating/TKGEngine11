
#include "Utility/inc/myfunc_collision.h"

namespace TKGEngine::MyFunc
{
	// =============================================================
	// ヘルパー関数
	// =============================================================
	/// <summary>
	/// AABB vs Frustumのスイープテスト
	/// </summary>
	/// <param name="aabb">AABB</param>
	/// <param name="frustum_points">Frustum</param>
	/// <param name="face_norm_dir">面法線</param>
	/// <param name="sweep_dir">スイープ方向</param>
	/// <param name="dist_min">交差位置までの最短</param>
	/// <param name="dist_max">交差位置までの最長</param>
	/// <returns></returns>
	bool ProjectedSweepIntersection(
		const Bounds& aabb, const VECTOR3 (&frustum_points)[8],
		const VECTOR3& face_norm_dir, const VECTOR3& sweep_dir,
		float& dist_min, float& dist_max
	)
	{
		const VECTOR3 center_pos = aabb.GetCenter();
		const VECTOR3 half_extents = aabb.GetExtents();
		// スイープの法線方向成分
		const float sweep = VECTOR3::Dot(sweep_dir, face_norm_dir);
		// AABBの中心点をベクトルに投影
		const float center = VECTOR3::Dot(center_pos, face_norm_dir);
		// AABBの半範囲をベクトルに投影
		const float half_size = 
			half_extents.x * fabs(face_norm_dir.x) +
			half_extents.y * fabs(face_norm_dir.y) +
			half_extents.z * fabs(face_norm_dir.z);
		// AABBの最大、最小
		const float min_1 = center - half_size;
		const float max_1 = center + half_size;

		// フラスタムの点を投影
		float project = VECTOR3::Dot(frustum_points[0], face_norm_dir);
		float min_2 = project;
		float max_2 = project;
		for (int i = 1; i < 8; i++)
		{
			project = VECTOR3::Dot(frustum_points[i], face_norm_dir);
			min_2 = (std::min)(project, min_2);
			max_2 = (std::max)(project, max_2);
		}

		// スイープが交差に影響する
		if (sweep != 0)
		{
			// max_1 + sweep * t >= min_2のとき交差が開始
			float intersection_start = (min_2 - max_1) / sweep;
			// min_1 + sweep * t >= max_2のとき交差が終了
			float intersection_end = (max_2 - min_1) / sweep;

			// 範囲の順序をチェック
			if (intersection_start > intersection_end)
			{
				const auto buf = intersection_start;
				intersection_start = intersection_end;
				intersection_end = buf;
			}

			// 距離の範囲が重ならない
			if (dist_min > intersection_end || intersection_start > dist_max)
			{
				return false;
			}

			// 範囲を更新する
			dist_min = (std::max)(dist_min, intersection_start);
			dist_max = (std::min)(dist_max, intersection_end);
		}
		// スイープが交差に影響しない
		else
		{
			// 交差しない
			if (min_1 > max_2 || min_2 > max_1)
			{
				return false;
			}
		}

		return true;
	}

	// =============================================================
	// スイープテスト
	// =============================================================
	bool SweepIntersectionTest(const Bounds& object_aabb, const Bounds& frustum_aabb, const VECTOR3& sweep_dir)
	{
		const VECTOR3 frustum_min = frustum_aabb.GetCenter() - frustum_aabb.GetExtents();
		const VECTOR3 frustum_max = frustum_aabb.GetCenter() + frustum_aabb.GetExtents();
		const VECTOR3 object_min = object_aabb.GetCenter() - object_aabb.GetExtents();
		const VECTOR3 object_max = object_aabb.GetCenter() + object_aabb.GetExtents();

		// スイープ方向に沿って計算する

		// Frustum
		// AABBの中心点を投影する
		const VECTOR3 frustum_center = (frustum_min + frustum_max) * 0.5f;
		const VECTOR3 frustum_half_size = (frustum_max - frustum_min) * 0.5f;
		const float frustum_center_proj = VECTOR3::Dot(frustum_center, sweep_dir);
		// AABBの半範囲を投影する
		const float frustum_half_size_proj = frustum_half_size.x * fabs(sweep_dir.x) +
			frustum_half_size.y * fabs(sweep_dir.y) +
			frustum_half_size.z * fabs(sweep_dir.z);
		const float frustum_proj_min = frustum_center_proj - frustum_half_size_proj;
		const float frustum_proj_max = frustum_center_proj + frustum_half_size_proj;

		// Object
		// AABBの中心点を投影する
		const VECTOR3 object_center = (object_min + object_max) * 0.5f;
		const VECTOR3 object_half_size = (object_max - object_min) * 0.5f;
		const float object_center_proj = VECTOR3::Dot(object_center, sweep_dir);
		// AABBの半範囲を投影する
		const float object_half_size_proj = object_half_size.x * fabs(sweep_dir.x) +
			object_half_size.y * fabs(sweep_dir.y) +
			object_half_size.z * fabs(sweep_dir.z);
		const float object_proj_min = object_center_proj - object_half_size_proj;
		const float object_proj_max = object_center_proj + object_half_size_proj;

		// スイープ方向の距離を求める
		// すべての軸で交点が発生する場所を探索

		// スイープ方向の交差点
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
		// スイープ方向の反対側でのみ交わるなら交差なし
		if (dist_max < 0) return false;

		// 軸上の交差点
		// starts: object_max.x + dist*sweep_dir.x = frustum_min.x
		//   ends: object_min.x + dist*sweep_dir.x = frustum_max.x

		// X軸テスト
		if(MyMath::Approximately(sweep_dir.x, 0.0f))
		{
			// この軸上には決して交点はない
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
			// 距離の範囲が重ならない
			if (dist_min > dist_max_new || dist_min_new > dist_max) return false;
			// 範囲を更新する
			dist_min = MyMath::Max(dist_min, dist_min_new);
			dist_max = MyMath::Min(dist_max, dist_max_new);
		}

		// Y軸テスト
		if (MyMath::Approximately(sweep_dir.y, 0.0f))
		{
			// この軸上には決して交点はない
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
			// 距離の範囲が重ならない
			if (dist_min > dist_max_new || dist_min_new > dist_max) return false;
			// 範囲を更新する
			dist_min = MyMath::Max(dist_min, dist_min_new);
			dist_max = MyMath::Min(dist_max, dist_max_new);
		}

		// Z軸テスト
		if (MyMath::Approximately(sweep_dir.z, 0.0f))
		{
			// この軸上には決して交点はない
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
			// 距離の範囲が重ならない
			if (dist_min > dist_max_new || dist_min_new > dist_max) return false;
		}

		// すべてのテストに合格なので交差点が発生
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

		// スイープ方向の交点候補範囲を見つける
		if (!ProjectedSweepIntersection(object_aabb, frustum_points, sweep_dir, sweep_dir, dist_min, dist_max)) return false;

		// AABBの面法線
		if (!ProjectedSweepIntersection(object_aabb, frustum_points, VECTOR3(1, 0, 0), sweep_dir, dist_min, dist_max)) return false;
		if (!ProjectedSweepIntersection(object_aabb, frustum_points, VECTOR3(0, 1, 0), sweep_dir, dist_min, dist_max)) return false;
		if (!ProjectedSweepIntersection(object_aabb, frustum_points, VECTOR3(0, 0, 1), sweep_dir, dist_min, dist_max)) return false;

		// frustumの面法線

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


		// フラスタムの辺との交差
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

		// すべてのテストに合格なので交差点が発生
		return true;
	}

}