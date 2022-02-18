#pragma once

#include "Animator_BlockBase.h"

#include "Animator_Clip.h"
#include "Animator_Event.h"

namespace TKGEngine::Animations
{
	class BlendTree;

	/// <summary>
	/// 複数のアニメーションクリップを正規化時間でブレンドするビルディングブロック
	/// </summary>
	class BlendTree
		: public AnimatorBlockBase
	{
		struct BlendData;
	public:
		// ==============================================
		// public enum
		// ==============================================
		/// <summary>
		/// ブレンドツリーで使用するブレンディングの種類
		/// </summary>
		enum class BlendTreeType
		{
			Simple1D,	// 1D
			Simple2D,	// 同じ向きのモーションを持たない2D
			FreeformCartesian2D,	// 勾配バンド補間した2D (http://runevision.com/thesis/rune_skovbo_johansen_thesis.pdf)
			FreeformCartesian3D,	// 勾配バンド補間した3D

			Max_BlendTreeType
		};

		// ==============================================
		// public methods
		// ==============================================
		BlendTree();
		BlendTree(const char* name);
		virtual ~BlendTree() = default;
		BlendTree(const BlendTree&) = delete;
		BlendTree& operator=(const BlendTree&) = delete;

		// パラメータ名をセット
		void SetParameter(const std::string& parameter);
		void SetParameterY(const std::string& parameter_y);
		void SetParameterZ(const std::string& parameter_z);

		// 子モーションの追加
		void AddChild(const std::string& motion_filepath);
		void AddChild(const std::string& motion_filepath, float threshold);
		void AddChild(const std::string& motion_filepath, const VECTOR2& position);
		void AddChild(const std::string& motion_filepath, const VECTOR3& position);
		// 子モーションの削除
		void RemoveChild(int index);

		// ブレンドツリーの種類
		void SetBlendTreeType(BlendTreeType type);

		// 再生速度(0以上)
		void Speed(const float speed);
		float Speed() const;

		// 繰り返し方
		// BlendTreeではLoopとClampのみ有効
		void SetWrapMode(WrapMode mode);

		// イベント
		void AddEvent(const std::string& class_type_name, const std::string& func_name, const float normalize_time);
		void AddEvent(const std::string& class_type_name, const std::string& func_name, const float normalize_time, int parameter);
		void AddEvent(const std::string& class_type_name, const std::string& func_name, const float normalize_time, float parameter);
		void RemoveEvent(int index);


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */

	private:
		// ==============================================
		// private struct
		// ==============================================

		/// <summary>
		/// ブレンドに使用する子モーションのデータ
		/// </summary>
		struct BlendChildClip
		{
			// 閾値
			VECTOR3 threshold = VECTOR3::Zero;
			// アニメーションクリップ
			AnimationClip clip;

#ifdef USE_IMGUI
			void OnGUI(BlendTreeType blend_type, const std::vector<BlendData>& blend_data, const int self_index);
#endif// #ifdef USE_IMGUI

		private:
			friend class cereal::access;
			template <class Archive>
			void serialize(Archive& archive)
			{
				archive(CEREAL_NVP(threshold), CEREAL_NVP(clip));
			}
		};

		friend class cereal::access;
		template <class Archive>
		void load(Archive& archive, const std::uint32_t version)
		{
			if (version == 1)
			{
				archive(
					cereal::base_class<AnimatorBlockBase>(this),
					CEREAL_NVP(m_parameter),
					CEREAL_NVP(m_parameter_y),
					CEREAL_NVP(m_parameter_z),
					CEREAL_NVP(m_blend_tree_type),
					CEREAL_NVP(m_wrap_mode),
					CEREAL_NVP(m_speed),
					CEREAL_NVP(m_use_speed_parameter),
					CEREAL_NVP(m_speed_parameter),
					CEREAL_NVP(m_children),
					CEREAL_NVP(m_events)
				);
			}
			const auto child_num = m_children.size();
			// ブレンドデータ配列をリサイズ
			m_blend_data.resize(child_num);
			// 影響度バッファのサイズ変更
			m_influences.resize(child_num);
		}
		template <class Archive>
		void save(Archive& archive, const std::uint32_t version) const
		{
			if (version == 1)
			{
				archive(
					cereal::base_class<AnimatorBlockBase>(this),
					CEREAL_NVP(m_parameter),
					CEREAL_NVP(m_parameter_y),
					CEREAL_NVP(m_parameter_z),
					CEREAL_NVP(m_blend_tree_type),
					CEREAL_NVP(m_wrap_mode),
					CEREAL_NVP(m_speed),
					CEREAL_NVP(m_use_speed_parameter),
					CEREAL_NVP(m_speed_parameter),
					CEREAL_NVP(m_children),
					CEREAL_NVP(m_events)
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		int OnGUI(
			const GameObjectID goid,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			std::unordered_map<std::string, std::shared_ptr<Animations::AnimatorControllerParameter>>& parameters
		) override;
#endif// #ifdef USE_IMGUI

		bool UpdateBlock(
			const GameObjectID goid,
			const bool is_root_layer,
			const bool is_additive,
			const bool check_transition,
			const bool is_current,
			const bool state_transitioning,
			const float elapsed_time,
			const float normalize_time,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters,
			const std::unordered_map<std::string, int>* node_index
		) override;
		bool UpdateKeyFrame(
			const bool is_root_layer,
			const bool is_additive,
			const float normalize_time,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			const std::unordered_map<std::string, int>* node_index,
			std::vector<KeyData>& dst_keydata
		) override;
		void OnReady(const float offset) override;

		// そのブロックのspeedとlengthを考慮したフレーム正規化経過時間と、speedを考慮しない正規化経過時間を返す
		void GetNormalizeTime(
			const float elapsed_time,
			const std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks,
			const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters,
			float& weighted_normalize_time,
			float& normalize_time
		) override;

		// レイヤー内のいずれかのBlock削除時に番号の確認と修正をする
		void OnRemoveBlock(int removed_index, std::vector<std::shared_ptr<AnimatorBlockBase>>& blocks) override;

		// Simple 1Dのブレンド率計算
		void CalculateBlendRatioSimple1D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters);
		// Simple 2Dのブレンド率計算
		void CalculateBlendRatioSimple2D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters);
		// FreeformCartesian2Dのブレンド率計算
		void CalculateBlendRatioFreeformCartesian2D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters);
		// FreeformCartesian3Dのブレンド率計算
		void CalculateBlendRatioFreeformCartesian3D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters);
		// 勾配バンド補間による各クリップの影響を求める
		float CalculateClipInfluence(const VECTOR2& param, const int child_idx);
		float CalculateClipInfluence(const VECTOR3& param, const int child_idx);

		// パラメータデータ探索、取得用
		float GetParameter1D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters) const;
		VECTOR2 GetParameter2D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters) const;
		VECTOR3 GetParameter3D(const std::unordered_map<std::string, std::shared_ptr<AnimatorControllerParameter>>& parameters) const;


		// ==============================================
		// private variables
		// ==============================================
		// 不正なChildrenの参照インデックス
		static constexpr int INVALID_CHILD_INDEX = -1;

		// 参照するパラメータ名
		std::string m_parameter;
		std::string m_parameter_y;
		std::string m_parameter_z;
		// ブレンドツリーの種類
		BlendTreeType m_blend_tree_type = BlendTreeType::Simple1D;
		// 繰り返し方
		WrapMode m_wrap_mode = WrapMode::Loop;
		// 0.0f以上の値のMotionの再生速度
		float m_speed = 1.0f;
		// Speedにパラメータを乗算するか
		bool m_use_speed_parameter = false;
		// Speedに乗算するパラメータ名
		std::string m_speed_parameter;
		// アニメーションクリップを含むツリーの子情報
		std::vector<BlendChildClip> m_children;
		// イベント
		std::vector<AnimatorEvent> m_events;

		// イベントのwrapによる発火ミスをなくすために前フレームのnormalize_timeを保持する
		float m_prev_normalize_time = 0.0f;

		/// <summary>
		/// ブレンドするデータ
		/// </summary>
		struct BlendData
		{
			int child_index = INVALID_CHILD_INDEX;
			float weight = 0.0f;

			void Clear()
			{
				child_index = INVALID_CHILD_INDEX;
				weight = 0.0f;
			}
		};
		std::vector<BlendData> m_blend_data = std::vector<BlendData>();
		// ブレンドするデータ数
		int m_blend_data_num = 0;
		// 勾配バンド補間時に影響度を入れておくバッファ
		std::vector<float> m_influences = std::vector<float>();
		// 子モーションデータから取得したアニメーションをブレンドするためのバッファ
		std::vector<KeyData> m_calc_child_keyframes[2];
	};

}// namespace TKGEngine::Animations

// BlendTree
CEREAL_REGISTER_TYPE(TKGEngine::Animations::BlendTree)
CEREAL_CLASS_VERSION(TKGEngine::Animations::BlendTree, 1)
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::Animations::AnimatorBlockBase, TKGEngine::Animations::BlendTree)
// archive時にload saveを使用する曖昧さ回避
CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(TKGEngine::Animations::BlendTree, cereal::specialization::member_load_save)
