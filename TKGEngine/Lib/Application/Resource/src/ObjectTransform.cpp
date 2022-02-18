
#include "../inc/ObjectTransform.h"

#include <fstream>

namespace TKGEngine
{
	void ObjectTransform::Load(const std::string& filepath)
	{
		std::ifstream ifs(filepath.c_str(), std::ios::out | std::ios::binary);
		if (ifs.is_open() == false)
		{
			assert(0 && "failed open file. ObjectTransform::Load()");
			return;
		}
		cereal::BinaryInputArchive ar(ifs);
		ar(m_data);
	}

	std::vector<ObjectTransform::ObjData>* ObjectTransform::Get()
	{
		return &m_data;
	}

}// namespace TKGEngine