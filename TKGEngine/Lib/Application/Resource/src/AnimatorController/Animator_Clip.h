#pragma once

#include "Animation_Defined.h"
#include "Systems/inc/TKGEngine_Defined.h"

namespace TKGEngine::Animations
{
	/// <summary>
	/// Motion�f�[�^���b�v�N���X
	/// </summary>
	class AnimationClip
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		AnimationClip();
		virtual ~AnimationClip() = default;
		AnimationClip(const AnimationClip&);
		AnimationClip& operator=(const AnimationClip&);

#ifdef USE_IMGUI
		void OnGUI() const;
#endif// #ifdef USE_IMGUI

		void AddMotion(const std::string& motion_filepath);
		void AddMotion(const Motion& motion);	// ������make_shared�������̂ɃR�s�[����

		bool HasMotion() const;

		const char* GetMotionName() const;
		const char* GetMotionFilepath() const;

		float GetSampleRate() const;
		float GetLength() const;
		float GetRCPLength() const;

		// ����t���[���̃L�[�f�[�^���擾
		std::vector<KeyData>* GetKeyFrame(const int frame) const;
		// Motion�f�[�^���̐��񏇃}�b�v
		std::unordered_map<std::string, int>* GetKeyIndex() const;

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */

	private:
		friend class cereal::access;
		template <class Archive>
		void load(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(m_motion_filedata)
				);
				// �V���A���C�Y���Ɋ��Ƀ��[�h���Ă���Ȃ烍�[�h����
				if (m_motion_filedata.HasData())
				{
					AddMotion(m_motion_filedata.filepath);
				}
			}
		}
		template <class Archive>
		void save(Archive& archive, const std::uint32_t version) const
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(m_motion_filedata)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
		/* nothing */

		// ==============================================
		// private variables
		// ==============================================
		Motion m_motion;
		FileLoadStateData m_motion_filedata;

		float m_sample_rate = 0.0f;
		float m_length = 0.0f;
		// ���K�����g�p�plength�t��
		float m_rcp_length = 0.0f;

	};
}// namespace TKGEngine::Animations

CEREAL_CLASS_VERSION(TKGEngine::Animations::AnimationClip, 1);
// archive����load save���g�p����B�������
CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(TKGEngine::Animations::AnimationClip, cereal::specialization::member_load_save)