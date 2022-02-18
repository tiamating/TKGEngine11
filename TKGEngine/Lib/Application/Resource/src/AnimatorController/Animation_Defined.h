#pragma once

#include <Systems/inc/TKGEngine_Defined.h>
#include <Application/Resource/inc/Motion.h>
#include <Utility/inc/myfunc_vector.h>

#include <string>
#include <vector>
#include <unordered_map>

namespace TKGEngine::Animations
{
	class AnimatorStateMachine;

	// RootステートマシンのBlockインデックス
	constexpr int ROOT_STATEMACHINE_BLOCK_INDEX = 0;
	// ブロックを指さない時のブロックindex
	constexpr int INVALID_BLOCK_INDEX = -1;

	// あるボーンのデータ1つ
	struct KeyData
	{
		VECTOR3 translate = VECTOR3::Zero;
		Quaternion rotate = Quaternion::Identity;
		VECTOR3 scale = VECTOR3::One;

	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(translate),
					CEREAL_NVP(rotate),
					CEREAL_NVP(scale)
				);
			}
		}
	};
	// ある時間のポーズデータ
	struct KeyFrame
	{
		float seconds = 0.f;
		std::vector<KeyData> keys;
	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(seconds),
					CEREAL_NVP(keys)
				);
			}
		}
	};
	// アニメーションデータ
	struct Animation
	{
		float seconds_length = 0.f;
		float sampling_rate = 0.0f;
		std::vector<KeyFrame> keyframes;
	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(seconds_length),
					CEREAL_NVP(sampling_rate),
					CEREAL_NVP(keyframes)
				);
			}
		}
	};

	/// <summary>
	/// スクリプトとコントローラ間の連携に使用するデータ構造体
	/// </summary>
	struct AnimatorControllerParameter
	{
		enum class Type
		{
			Float,
			Int,
			Bool,
			Trigger
		};

		Type type = Type::Bool;
		bool default_bool = false;
		float default_float = 0.0f;
		int default_int = 0;

#ifdef USE_IMGUI
		void OnGUI()
		{
			ImGui::PushID(this);
			// Typeごとの表示
			switch (type)
			{
				case Type::Float:
				{
					ImGui::DragFloat("##Float", &default_float, 0.01f);
				}
				break;
				case Type::Int:
				{
					ImGui::DragInt("##Int", &default_int, 0.03f);
				}
				break;
				case Type::Bool:
				{
					ImGui::Checkbox("##Bool", &default_bool);
				}
				break;
				case Type::Trigger:
				{
					if (ImGui::RadioButton("##Trigger", default_bool))
					{
						default_bool = !default_bool;
					}
				}
				break;
			}
			// Type変更
			if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
			{
				ImGui::OpenPopup("Type select popup");
			}
			if (ImGui::BeginPopup("Type select popup"))
			{
				// Float
				if (ImGui::Selectable("Float", type == Type::Float ? true : false))
				{
					type = Type::Float;
				}
				// Int
				if (ImGui::Selectable("Int", type == Type::Int ? true : false))
				{
					type = Type::Int;
				}
				// Bool
				if (ImGui::Selectable("Bool", type == Type::Bool ? true : false))
				{
					type = Type::Bool;
				}
				// Trigger
				if (ImGui::Selectable("Trigger", type == Type::Trigger ? true : false))
				{
					type = Type::Trigger;
				}

				ImGui::EndPopup();
			}
			ImGui::PopID();
		}
#endif // USE_IMGUI

	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(type),
					CEREAL_NVP(default_bool),
					CEREAL_NVP(default_float),
					CEREAL_NVP(default_int)
				);
			}
		}
	};

	/// <summary>
	/// Animationを遷移させる条件指定用構造体
	/// </summary>
	struct AnimatorCondition
	{
		/// <summary>
		/// 遷移条件のモード
		/// Triggerはモードにかかわらずtrueになったら遷移する
		/// </summary>
		enum class Mode
		{
			// For Parameter::Bool
			If,			// パラメータがtrueの時true
			// ~For Parameter::Bool

			// For Parameter::Int or Float
			Greater,	// threshold > パラメータの時true
			Less,		// threshold < パラメータの時true
			// ~For Parameter::Int or Float

			// For Parameter::Int
			Equal,		// threshold == パラメータの時true
			NotEqual,	// threshold != パラメータの時true
			// ~For Parameter::Int
		};

		Mode mode = Mode::If;		//!< 条件のモード
		float threshold = 0.0f;		//!< 条件がtrueになるためのAnimatorParameterの閾値
		std::string parameter_name;	//!< 遷移条件で使用されるパラメータ名

		AnimatorCondition() = default;
		AnimatorCondition(Mode mode_, float threshold_, const std::string& name_)
			: mode(mode_), threshold(threshold_), parameter_name(name_)
		{
			/* noting */
		}

	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					CEREAL_NVP(mode),
					CEREAL_NVP(threshold),
					CEREAL_NVP(parameter_name)
				);
			}
		}
	};

	/// <summary>
	/// アニメーションレイヤーのブレンド方法の種類
	/// </summary>
	enum class BlendingMode
	{
		Override,	//!< アニメーションを上書きする
		Additive,	//!< アニメーションを加算する
		Max_BlendingMode
	};

	/// <summary>
	/// Clipのキーフレーム範囲外の時間の扱い方の種類
	/// </summary>
	enum class WrapMode
	{
		Once,		// 終わるとClipの始めで止まる
		Loop,		// 終わると初めから繰り返す
		Clamp,		// 終わると最後のフレームを繰り返す
		Pingpong,	// 終わると逆方向に繰り返す
		RevLoop,
		RevClamp,
		Max_WrapMode
	};



}// namespace TKGEngine::Animations

CEREAL_CLASS_VERSION(TKGEngine::Animations::KeyData, 1)

CEREAL_CLASS_VERSION(TKGEngine::Animations::KeyFrame, 1)

CEREAL_CLASS_VERSION(TKGEngine::Animations::Animation, 1)

CEREAL_CLASS_VERSION(TKGEngine::Animations::AnimatorControllerParameter, 1)

CEREAL_CLASS_VERSION(TKGEngine::Animations::AnimatorCondition, 1)