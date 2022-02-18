#include "DirectionalLightShadow.h"

#include "Utility/inc/myfunc_collision.h"

#include "Components/interface/IRenderer.h"

namespace TKGEngine
{
	void DirectionalLightShadow::FindCasters(
		const std::vector<MainData>& scene_objects,
		const int object_num,
		const std::shared_ptr<ICamera>& camera,
		const LightShadowData& data,
		const Frustum& frustum,
		bool test_accurate,
		std::vector<MainData>& casters
	)
	{
		// 情報を入れる先のクリアとリサイズ
		casters.clear();
		casters.reserve(object_num);

		// 大雑把な判定時に使用するフラスタムのAABB
		Bounds frustum_aabb;
		if (!test_accurate)
		{
			frustum_aabb = frustum.GetAABB();
		}

		for (int i = 0; i < object_num; ++i)
		{
			const auto& obj = scene_objects.at(i);

			// nullptrチェック
			if (!obj.renderer)
				continue;

			// 交差テスト
			if (test_accurate)
			{
				// AABB vs Frustum
				if (!MyFunc::SweepIntersectionTest(obj.renderer->GetRendererBounds(), frustum, data.light_direction))
					continue;
			}
			else
			{
				// AABB vs Frustum.AABB
				if (!MyFunc::SweepIntersectionTest(obj.renderer->GetRendererBounds(), frustum_aabb, data.light_direction))
					continue;
			}
			// 交差するものを詰める
			casters.emplace_back(obj);
		}
	}

}
