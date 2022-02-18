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

	// Root�X�e�[�g�}�V����Block�C���f�b�N�X
	constexpr int ROOT_STATEMACHINE_BLOCK_INDEX = 0;
	// �u���b�N���w���Ȃ����̃u���b�Nindex
	constexpr int INVALID_BLOCK_INDEX = -1;

	// ����{�[���̃f�[�^1��
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
	// ���鎞�Ԃ̃|�[�Y�f�[�^
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
	// �A�j���[�V�����f�[�^
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
	/// �X�N���v�g�ƃR���g���[���Ԃ̘A�g�Ɏg�p����f�[�^�\����
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
			// Type���Ƃ̕\��
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
			// Type�ύX
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
	/// Animation��J�ڂ���������w��p�\����
	/// </summary>
	struct AnimatorCondition
	{
		/// <summary>
		/// �J�ڏ����̃��[�h
		/// Trigger�̓��[�h�ɂ�����炸true�ɂȂ�����J�ڂ���
		/// </summary>
		enum class Mode
		{
			// For Parameter::Bool
			If,			// �p�����[�^��true�̎�true
			// ~For Parameter::Bool

			// For Parameter::Int or Float
			Greater,	// threshold > �p�����[�^�̎�true
			Less,		// threshold < �p�����[�^�̎�true
			// ~For Parameter::Int or Float

			// For Parameter::Int
			Equal,		// threshold == �p�����[�^�̎�true
			NotEqual,	// threshold != �p�����[�^�̎�true
			// ~For Parameter::Int
		};

		Mode mode = Mode::If;		//!< �����̃��[�h
		float threshold = 0.0f;		//!< ������true�ɂȂ邽�߂�AnimatorParameter��臒l
		std::string parameter_name;	//!< �J�ڏ����Ŏg�p�����p�����[�^��

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
	/// �A�j���[�V�������C���[�̃u�����h���@�̎��
	/// </summary>
	enum class BlendingMode
	{
		Override,	//!< �A�j���[�V�������㏑������
		Additive,	//!< �A�j���[�V���������Z����
		Max_BlendingMode
	};

	/// <summary>
	/// Clip�̃L�[�t���[���͈͊O�̎��Ԃ̈������̎��
	/// </summary>
	enum class WrapMode
	{
		Once,		// �I����Clip�̎n�߂Ŏ~�܂�
		Loop,		// �I���Ə��߂���J��Ԃ�
		Clamp,		// �I���ƍŌ�̃t���[�����J��Ԃ�
		Pingpong,	// �I���Ƌt�����ɌJ��Ԃ�
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