#pragma once

#include "Systems/inc/TKGEngine_Defined.h"
#include "Application/Resource/inc/Shader_Defined.h"

#include <cassert>

namespace TKGEngine::Animations
{
	/// <summary>
	/// �A�j���[�V����������Transform�Ƀ}�X�N�������邱�Ƃ��ł���N���X
	/// </summary>
	class AvatarMask
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		AvatarMask();
		virtual ~AvatarMask() = default;
		AvatarMask(const AvatarMask&);
		AvatarMask& operator=(const AvatarMask&);

#ifdef USE_IMGUI
		void OnGUI(const std::vector<std::string>* bone_names);
#endif // USE_IMGUI

		void SetWeight(int index, float weight);
		float GetWeight(int index) const;
		const float(&GetWeights() const)[MAX_BONES]
		{
			return m_weights;
		}

		void AddMask(int index);	//!< �}�X�N��ǉ�����
		void RemoveMask(int index);	//!< �}�X�N���O��

		bool GetMask(int index) const
		{
			assert(index >= 0 && index < MAX_BONES);
			return m_mask_transform_indices[index];
		}
		const bool(&GetMasks() const)[MAX_BONES]
		{
			return m_mask_transform_indices;
		}

		// ==============================================
		// public variables
		// ==============================================
		/* nothing */

	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			if (version == 1)
			{
				archive(
					CEREAL_NVP(m_weight),
					CEREAL_NVP(m_weights),
					CEREAL_NVP(m_mask_transform_indices)
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
		// TODO : �d�݂͔z��ɂ������ߍ폜�\��
		// �}�X�N��K�p����d��(0 : �Ȃ�, 1 : �ő�)
		float m_weight = 1.0f;
		float m_weights[MAX_BONES] = {};
		// true : �K�p����, false : ���O����
		bool m_mask_transform_indices[MAX_BONES] = { false };

	};


}// namespace TKGEngine::Animations

CEREAL_CLASS_VERSION(TKGEngine::Animations::AvatarMask, 1);