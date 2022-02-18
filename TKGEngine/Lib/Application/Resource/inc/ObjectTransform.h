#pragma once

#include "../../../Systems/inc/TKGEngine_Defined.h"
#include "../../../Utility/inc/myfunc_vector.h"

#include <string>
#include <vector>

namespace TKGEngine
{
	class ObjectTransform
	{
	public:
		struct ObjData
		{
			VECTOR3 position;
			Quaternion rotation;
			VECTOR3 scale;

		private:
			friend class cereal::access;
			template <class Archive>
			void serialize(Archive& archive, const std::uint32_t version)
			{
				//if (version > 0)
				{
					archive(
						CEREAL_NVP(position),
						CEREAL_NVP(rotation),
						CEREAL_NVP(scale)
					);
				}
			}
		};

		ObjectTransform() = default;
		virtual ~ObjectTransform() = default;
		ObjectTransform(ObjectTransform&&) = default;
		ObjectTransform(const ObjectTransform&) = delete;
		ObjectTransform& operator=(const ObjectTransform&) = delete;

		void Load(const std::string& filepath);
		std::vector<ObjData>* Get();

	private:
		std::vector<ObjData> m_data;

	};


}// namespace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::ObjectTransform::ObjData, 1)