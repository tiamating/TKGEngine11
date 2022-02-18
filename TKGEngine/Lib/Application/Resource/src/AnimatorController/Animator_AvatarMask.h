#pragma once

#include "Systems/inc/TKGEngine_Defined.h"
#include "Application/Resource/inc/Shader_Defined.h"

#include <cassert>

namespace TKGEngine::Animations
{
	/// <summary>
	/// アニメーションさせるTransformにマスクをかけることができるクラス
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

		void AddMask(int index);	//!< マスクを追加する
		void RemoveMask(int index);	//!< マスクを外す

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
		// TODO : 重みは配列にしたため削除予定
		// マスクを適用する重み(0 : なし, 1 : 最大)
		float m_weight = 1.0f;
		float m_weights[MAX_BONES] = {};
		// true : 適用する, false : 除外する
		bool m_mask_transform_indices[MAX_BONES] = { false };

	};


}// namespace TKGEngine::Animations

CEREAL_CLASS_VERSION(TKGEngine::Animations::AvatarMask, 1);