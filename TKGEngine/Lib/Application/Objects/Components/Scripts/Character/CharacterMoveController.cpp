#include "CharacterMoveController.h"
#include "Components/inc/CRigidBody.h"
#include "CharacterGroundChecker.h"

REGISTERCOMPONENT(TKGEngine::CharacterMoveController);

namespace TKGEngine
{
	void CharacterMoveController::Awake()
	{
		// 更新はステート更新の後
		Priority(9);
	}
	
	void CharacterMoveController::Start()
	{
		// Forwardの取得
		const auto transform = GetTransform();
		const VECTOR3 forward = transform->Forward();
		m_current_velocity_direction =
			m_current_forward =
			VECTOR2(forward.x, forward.z).Normalized();

		//// コンポーネントの取得
		// RigidBody
		m_rigidbody = GetComponent<RigidBody>();
		// CharacterGroundChecker
		m_ground_checker = GetComponent<CharacterGroundChecker>();
	}
	
	void CharacterMoveController::LateUpdate()
	{
		const auto rigidbody = m_rigidbody.GetWeak().lock();
		const auto ground_checker = m_ground_checker.GetWeak().lock();

		const float delta_time = ITime::Get().DeltaTime();

		// 重力を更新
		float gravity_velocity = rigidbody->GetLinearVelocity().y;
		if (m_is_updated_gravity)
		{
			if (ground_checker->IsGround())
			{
				gravity_velocity = 0.0f;
			}
			else
			{
				gravity_velocity += m_gravity_acceleration * delta_time;
			}
		}
		else
		{
			gravity_velocity = 0.0f;
			m_is_updated_gravity = true;
		}

		// 方向を更新
		rigidbody->SetRotation(Quaternion::LookRotation(VECTOR3(m_current_forward.x, 0.0f, m_current_forward.y)));

		// 速度を更新
		const VECTOR2 next_velocity = m_current_speed * m_current_velocity_direction;
		rigidbody->SetLinearVelocity(VECTOR3(next_velocity.x, gravity_velocity, next_velocity.y));
	}


	/////////////////////////////////////////////////////////////////////////
	// GUI設定
	/////////////////////////////////////////////////////////////////////////
#ifdef USE_IMGUI
	void CharacterMoveController::OnGUI()
	{
	}
#endif
}
