
#include "Animator_Clip.h"
#include "Utility/inc/myfunc_math.h"

namespace TKGEngine::Animations
{
	AnimationClip::AnimationClip()
	{
		/* nothing */
	}
	AnimationClip::AnimationClip(const AnimationClip& clip)
	{
		m_motion = clip.m_motion;
		m_motion_filedata = clip.m_motion_filedata;
		m_sample_rate = clip.m_sample_rate;
		m_length = clip.m_length;
		m_rcp_length = clip.m_rcp_length;
	}

	AnimationClip& AnimationClip::operator=(const AnimationClip& clip)
	{
		m_motion = clip.m_motion;
		m_motion_filedata = clip.m_motion_filedata;
		m_sample_rate = clip.m_sample_rate;
		m_length = clip.m_length;
		m_rcp_length = clip.m_rcp_length;
		return *this;
	}

#ifdef USE_IMGUI
	void AnimationClip::OnGUI() const
	{
		// Name
		ImGui::Text("Name:");
		ImGui::SameLine();
		ImGui::Text(GetMotionName());
		// Path
		ImGui::Text("Path:");
		ImGui::SameLine();
		ImGui::Text(GetMotionFilepath());
		// Rate
		ImGui::Text("Sample Rate:");
		ImGui::SameLine();
		const std::string str_rate = std::to_string(m_sample_rate);
		ImGui::Text(str_rate.c_str());
		// Length
		ImGui::Text("Length:");
		ImGui::SameLine();
		const std::string str_length = std::to_string(m_length);
		ImGui::Text(str_length.c_str());
	}
#endif// #ifdef USE_IMGUI

	void AnimationClip::AddMotion(const std::string& motion_filepath)
	{
		m_motion.Load(motion_filepath);
		if (m_motion.HasMotion())
		{
			m_motion_filedata.Set(motion_filepath);
			m_sample_rate = m_motion.GetSampleRate();
			m_length = m_motion.GetMotionLength();
			// ポーズデータなら逆数は0.0f
			m_rcp_length = MyMath::Approximately(m_length, 0.0f) ? 0.0f : static_cast<float>(1.0 / m_length);
		}
		else
		{
			m_motion_filedata.Clear();
			m_sample_rate = 0.0f;
			m_length = 0.0f;
			m_rcp_length = 0.0f;
		}
	}

	void AnimationClip::AddMotion(const Motion& motion)
	{
		m_motion = motion;
		if (m_motion.HasMotion())
		{
			m_motion_filedata.Set(m_motion.GetFilepath());
			m_sample_rate = m_motion.GetSampleRate();
			m_length = m_motion.GetMotionLength();
			// ポーズデータなら逆数は0.0f
			m_rcp_length = MyMath::Approximately(m_length, 0.0f) ? 0.0f : static_cast<float>(1.0 / m_length);
		}
		else
		{
			m_motion_filedata.Clear();
			m_sample_rate = 0.0f;
			m_length = 0.0f;
			m_rcp_length = 0.0f;
		}
	}

	bool AnimationClip::HasMotion() const
	{
		return m_motion_filedata.HasData();
	}

	const char* AnimationClip::GetMotionName() const
	{
		return m_motion.GetName();
	}

	const char* AnimationClip::GetMotionFilepath() const
	{
		return m_motion_filedata.filepath.c_str();
	}

	float AnimationClip::GetSampleRate() const
	{
		return m_sample_rate;
	}

	float AnimationClip::GetLength() const
	{
		return m_length;
	}

	float AnimationClip::GetRCPLength() const
	{
		return m_rcp_length;
	}

	std::vector<KeyData>* AnimationClip::GetKeyFrame(const int frame) const
	{
		return m_motion.GetKeyFrame(frame);
	}

	std::unordered_map<std::string, int>* AnimationClip::GetKeyIndex() const
	{
		return m_motion.GetKeyIndex();
	}

}// namespace TKGEngine::Animations
