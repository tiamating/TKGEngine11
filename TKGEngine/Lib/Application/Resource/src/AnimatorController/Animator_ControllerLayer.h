#pragma once

#include "Animation_Defined.h"
#include "Animator_AvatarMask.h"
#include "Animator_State.h"
#include "Animator_StateMachine.h"
#include "Animator_BlendTree.h"

namespace TKGEngine::Animations
{
	/// <summary>
	/// モデルに適用するアニメーションのマスクとステートマシンをもつレイヤー
	/// </summary>
	class AnimatorControllerLayer
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		AnimatorControllerLayer();
		AnimatorControllerLayer(const std::string& name);
		virtual ~AnimatorControllerLayer() = default;
		AnimatorControllerLayer(const AnimatorControllerLayer&) = delete;
		AnimatorControllerLayer& operator=(const AnimatorControllerLayer&) = delete;

#ifdef USE_IMGUI
		void OnGUIStateMachine(const GameObjectID goid, std::unordered_map<std::string, std::shared_ptr<Animations::AnimatorControllerParameter>>& parameters);
		void OnGUIMask(const std::vector<std::string>* bone_names, std::unordered_map<std::string, std::shared_ptr<Animations::AnimatorControllerParameter>>& parameters);
#endif // USE_IMGUI

		// レイヤーの持つステートマシンを返す
		std::shared_ptr<AnimatorStateMachine> GetStateMachine();

		// ステートマシンにStateを追加する
		std::shared_ptr<AnimatorState> AddMotion(const std::string& motion_filepath);
		std::shared_ptr<AnimatorState> AddState(const std::string& name);
		std::shared_ptr<AnimatorState> AddState(int sm_index, const std::string& name);
		// ステートマシンにBlendTreeを追加する
		std::shared_ptr<BlendTree> AddBlendTree(const std::string& name);
		std::shared_ptr<BlendTree> AddBlendTree(int sm_index, const std::string& name);
		// ステートマシンにStateMachineを追加する
		//std::shared_ptr<AnimatorStateMachine> AddStateMachine(const std::string& name);
		//std::shared_ptr<AnimatorStateMachine> AddStateMachine(int sm_index, const std::string& name);
		// ブロックの削除
		void RemoveBlock(const int index);

		// アバターマスク
		void SetAvatarMask(const AvatarMask& mask);
		// レイヤーの重ね方を設定する
		void SetBlendingMode(BlendingMode mode);
		// レイヤー名
		const char* GetName() const;
		void SetName(const std::string& name);
		// 現在のキーフレームデータを計算して返す
		void ApplyAnimation(
			const GameObjectID goid,
			const float elapsed_time,
			const bool is_root,
			const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters,
			const std::unordered_map<std::string, int>* node_index,
			const std::vector<int>& enable_nodes,
			std::vector<KeyData>& data
		);

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
					CEREAL_NVP(m_use_weight_parameter),
					CEREAL_NVP(m_weight_parameter),
					CEREAL_NVP(m_layer_name),
					CEREAL_NVP(m_avatar_mask),
					CEREAL_NVP(m_blending_mode),
					CEREAL_NVP(m_blocks)
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
#ifdef USE_IMGUI
		// GUI専用、現在選択中のステートマシンindex
		int m_gui_current_select_state_machine_index = ROOT_STATEMACHINE_BLOCK_INDEX;
#endif// #ifdef USE_IMGUI

		// Weightにパラメータを乗算するか
		bool m_use_weight_parameter = false;
		// Weightに乗算するパラメータ名
		std::string m_weight_parameter;
		// レイヤー名
		std::string m_layer_name;
		// レイヤー合成時のマスクデータ
		AvatarMask m_avatar_mask;
		// ブレンディングモードは先頭のレイヤーでは無視される
		BlendingMode m_blending_mode = BlendingMode::Override;

		// レイヤー内に存在する全てのアニメーションブロック
		std::vector<std::shared_ptr<AnimatorBlockBase>> m_blocks;
	};


}// namespace TKGEngine::Animations

CEREAL_CLASS_VERSION(TKGEngine::Animations::AnimatorControllerLayer, 1);