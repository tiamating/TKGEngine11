#pragma once
#include "Player/StickInputData.h"
#include "Components/inc/CMonoBehaviour.h"

namespace TKGEngine
{
	class RigidBody;
	class CharacterGroundChecker;

	class CharacterMoveController
		: public TKGEngine::MonoBehaviour
	{
	public:
		CharacterMoveController() = default;
		virtual ~CharacterMoveController() override = default;
		CharacterMoveController(const CharacterMoveController&) = delete;
		CharacterMoveController& operator=(const CharacterMoveController&) = delete;

	private:
		virtual void Awake() override;
		virtual void Start() override;
		virtual void LateUpdate() override;


		//////////////////////////////////
		// functions
		//////////////////////////////////



		//////////////////////////////////
		// variables
		//////////////////////////////////
		// 参照
		SWPtr<RigidBody> m_rigidbody;
		SWPtr<CharacterGroundChecker> m_ground_checker;

		// 	// X-Z平面での正面方向
		VECTOR2 m_current_forward = VECTOR2::Zero;
		// 現在の速度のデータ
		VECTOR2 m_current_velocity_direction = VECTOR2::Zero;	// X-Z平面での速度方向
		float m_current_speed = 0.0f;

		// 重力加速度
		float m_gravity_acceleration = -9.8f;

		// セットを許すか
		bool m_allow_set_parameter = true;

	public:
		// 現在の速度方向
		const VECTOR2& GetForward() const { return m_current_forward; }
		void SetForward(const VECTOR2& forward)
		{
			if(m_allow_set_parameter)
				m_current_forward = forward;
		}
		// 現在の速度方向
		const VECTOR2& GetVelocityDirection() const { return m_current_velocity_direction; }
		void SetVelocityDirection(const VECTOR2& dir)
		{
			if (m_allow_set_parameter)
				m_current_velocity_direction = dir;
		}
		// 現在の速度
		float GetSpeed() const { return m_current_speed; }
		void SetSpeed(float speed)
		{
			if (m_allow_set_parameter)
				m_current_speed = speed;
		}

		// セットを有効にするか
		void AllowSetParameter(const bool allow) { m_allow_set_parameter = allow; }


	private:
		/////////////////////////////////////////////////////////////////////////
		// Serialize設定
		/////////////////////////////////////////////////////////////////////////
		friend class cereal::access;

		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			if (version == 1)
			{
				archive(
					cereal::base_class<TKGEngine::MonoBehaviour>(this)
				);
			}
		}

		/////////////////////////////////////////////////////////////////////////
		// GUI設定
		/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
		void OnGUI() override;
#endif // USE_IMGUI
	};
}

/////////////////////////////////////////////////////////////////////////
// Serialize登録
/////////////////////////////////////////////////////////////////////////
CEREAL_CLASS_VERSION(TKGEngine::CharacterMoveController, 1)

CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::CharacterMoveController, "TKGEngine::CharacterMoveController")

CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::MonoBehaviour, TKGEngine::CharacterMoveController)
