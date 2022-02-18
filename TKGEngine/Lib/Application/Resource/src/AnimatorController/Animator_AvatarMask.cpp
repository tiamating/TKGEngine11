
#include "Animator_AvatarMask.h"

#include "Utility/inc/myfunc_math.h"

namespace TKGEngine::Animations
{
	AvatarMask::AvatarMask()
	{
		// 初期状態では全て適用する
		for (int i = 0; i < MAX_BONES; ++i)
		{
			m_weights[i] = 1.0f;
		}
	}

	AvatarMask::AvatarMask(const AvatarMask& mask)
	{
		memcpy(&m_weights, mask.m_weights, sizeof(mask.m_weights));
		memcpy(&m_mask_transform_indices, mask.m_mask_transform_indices, sizeof(mask.m_mask_transform_indices));
	}

	AvatarMask& AvatarMask::operator=(const AvatarMask& mask)
	{
		memcpy(&m_weights, mask.m_weights, sizeof(mask.m_weights));
		memcpy(&m_mask_transform_indices, mask.m_mask_transform_indices, sizeof(mask.m_mask_transform_indices));
		return *this;
	}

#ifdef USE_IMGUI
	void AvatarMask::OnGUI(const std::vector<std::string>* bone_names)
	{
		ImGui::IDWrapped id(this);
		// Mask
		ImGui::Text("Select Active Bone");
		// ImGuiのフィルター機能
		static ImGuiTextFilter im_filter;
		ImGui::Text("Filter");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		im_filter.Draw("##parameter filter");

		ImGui::BeginChild("##SelectBone", ImVec2(0.0f, 0.0f), true);
		const size_t bone_num = bone_names->size();

		// まとめて設定する
		if(ImGui::Button("Select All"))
		{
			for(size_t i = 0;i < bone_num;++i)
			{
				m_mask_transform_indices[i] = true;
			}
		}
		ImGui::SameLine();
		if(ImGui::Button("Deselect All"))
		{
			for (size_t i = 0; i < bone_num; ++i)
			{
				m_mask_transform_indices[i] = false;
			}
		}
		// BoneNamesの順に表示する
		for (size_t i = 0; i < bone_num; ++i)
		{
			// フィルターを通過するかチェック
			if (im_filter.PassFilter(bone_names->at(i).c_str()))
			{
				ImGui::IDWrapped bone_id(static_cast<int>(i));
				// CheckBox
				ImGui::Checkbox("##select active checkbox", &m_mask_transform_indices[i]);
				// Selectable
				ImGui::SameLine();
				const float selectable_item_width = ImGui::GetContentRegionAvailWidth() * 0.6f;
				ImGui::SetNextItemWidth(selectable_item_width);
				if (ImGui::Selectable(bone_names->at(i).c_str(), false, 0, ImVec2(selectable_item_width, 0.0f)))
				{
					m_mask_transform_indices[i] = !m_mask_transform_indices[i];
				}
				// Weight
				ImGui::AlignedSameLine(0.3f);
				ImGui::SliderFloat("##Bone Weight", &m_weights[i], 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
				ImGui::Separator();
			}
		}

		ImGui::EndChild();
	}
#endif // USE_IMGUI

	void AvatarMask::SetWeight(const int index, const float weight)
	{
		if (index < 0 || index >= MAX_BONES)
			return;
		m_weights[index] = MyMath::Clamp(weight, 0.0f, 1.0f);
	}

	float AvatarMask::GetWeight(const int index) const
	{
		if (index < 0 || index >= MAX_BONES)
			return 0.0f;
		return m_weights[index];
	}

	void AvatarMask::AddMask(const int index)
	{
		m_mask_transform_indices[index] = true;
	}

	void AvatarMask::RemoveMask(const int index)
	{
		m_mask_transform_indices[index] = false;
	}


}// namespace TKGEngine::Animations
