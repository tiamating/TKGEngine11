
#include "../../inc/Motion.h"

#include "../AnimatorController/Animation_Defined.h"
#include "IResMotion.h"


namespace TKGEngine
{
	// ==================================================
	// class methods
	// ==================================================
	Motion& Motion::operator=(const Motion& m)
	{
		m_res_motion = m.m_res_motion;
		return *this;
	}

	void Motion::RemoveUnused()
	{
		IResMotion::RemoveUnused();
	}

	void Motion::Load(const std::string& filename)
	{
		m_res_motion = IResMotion::Load(filename);
	}

	void Motion::LoadAsync(const std::string& filename)
	{
		m_res_motion = IResMotion::LoadAsync(filename);
	}

	void Motion::Release()
	{
		if (m_res_motion != nullptr)
		{
			m_res_motion->Release();
		}
		m_res_motion.reset();
	}

	float Motion::GetSampleRate() const
	{
		return m_res_motion == nullptr ? 0.0f : m_res_motion->GetSampleRate();
	}

	float Motion::GetMotionLength() const
	{
		return m_res_motion == nullptr ? 0.0f : m_res_motion->GetMotionLength();
	}

	bool Motion::IsLoaded() const
	{
		return m_res_motion != nullptr ? m_res_motion->IsLoaded() : false;
	}

	bool Motion::HasMotion() const
	{
		return m_res_motion == nullptr ? false : m_res_motion->HasResource();
	}

	const char* Motion::GetName() const
	{
		return m_res_motion == nullptr ? nullptr : m_res_motion->GetName();
	}

	const char* Motion::GetFilepath() const
	{
		return m_res_motion == nullptr ? nullptr : m_res_motion->GetFilePath();
	}

	unsigned Motion::GetHash() const
	{
		return m_res_motion == nullptr ? 0 : m_res_motion->GetHash();
	}

	std::vector<Animations::KeyData>* Motion::GetKeyFrame(const int frame) const
	{
		return m_res_motion == nullptr ? nullptr : m_res_motion->GetKeyFrame(frame);
	}

	std::unordered_map<std::string, int>* Motion::GetKeyIndex() const
	{
		return m_res_motion == nullptr ? nullptr : m_res_motion->GetKeyIndex();
	}

}// namespace TKGEngine