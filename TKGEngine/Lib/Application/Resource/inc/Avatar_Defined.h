#pragma once

#include "../../../Systems/inc/TKGEngine_Defined.h"

#include "../../../Utility/inc/myfunc_vector.h"

#include <string>
#include <vector>

namespace TKGEngine
{
	struct Node
	{
		std::string bone_name;
		int bone_idx = -1;	// CB�Z�b�g�p�v�f�ԍ�

		std::vector<int> children_idx;
		int parent_idx = -1;	// RootBone��-1

		//std::weak_ptr<Transform> transform;	// �{�[�����g��GO�ɃA�^�b�`���ꂽTransform Component

		MATRIX inverse_transform = MATRIX::Identity;

		VECTOR3 init_translate = VECTOR3::Zero;
		Quaternion init_rotate = Quaternion::Identity;
		VECTOR3 init_scale = VECTOR3::One;

	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(bone_name),
					CEREAL_NVP(bone_idx),
					CEREAL_NVP(children_idx),
					CEREAL_NVP(parent_idx),
					CEREAL_NVP(inverse_transform),
					CEREAL_NVP(init_translate),
					CEREAL_NVP(init_rotate),
					CEREAL_NVP(init_scale)
				);
			}
		}
	};


}// namespace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::Node, 1);