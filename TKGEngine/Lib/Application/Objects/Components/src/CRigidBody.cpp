#include "../inc/CRigidBody.h"

#include "../inc/CTransform.h"
#include "../../inc/IGameObject.h"
#include "Systems/inc/PhysicsSystem.h"
#include "Systems/inc/Physics_Shape.h"
#include "Systems/inc/IGUI.h"
#include "Utility/inc/myfunc_math.h"

REGISTERCOMPONENT(TKGEngine::RigidBody)

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// RigidBody class Methods
	////////////////////////////////////////////////////////
#pragma region RigidBody
	RigidBody::RigidBody()
	{
		/* nothing */
	}

	RigidBody::~RigidBody()
	{
		/* nothing */
	}

#ifdef USE_IMGUI
	void RigidBody::OnGUI()
	{
		ImGui::IDWrapped id(this);

		// Collider::OnGUI
		Collider::OnGUI();
		// Is Static
		if (ImGui::Checkbox("Is Static", &m_is_static))
		{
			SetStatic(m_is_static);
		}
		// Is Kinematic
		if (ImGui::Checkbox("Is Kinematic", &m_is_kinematic))
		{
			SetKinematic(m_is_kinematic);
		}
		ImGui::Separator();
		// Gravity
		if (ImGui::Checkbox("Use Gravity", &m_use_gravity))
		{
			UseGravity(m_use_gravity);
		}
		if (ImGui::Checkbox("Applied world gravity", &m_applied_world_gravity))
		{
			ApplyGravitySetting();
		}
		if (!m_applied_world_gravity)
		{
			ImGui::Text("Gravity");
			ImGui::AlignedSameLine(0.5f);
			ImGui::DragFloat3("##Gravity", &m_gravity.x, 0.02f, 0.0f, 0.0f, "%.2f");
			if (ImGui::IsItemDeactivated())
			{
				ApplyGravitySetting();
			}
		}
		ImGui::Separator();
		// Damping, Friction and Restitution
		{
			// Restitution
			ImGui::Text("Restitution");
			ImGui::AlignedSameLine(0.5f);
			ImGui::DragFloat("##Restitution", &m_restitution, 0.02f, 0.0f, 10.0f, "(0 ~ 10)%.3f", ImGuiSliderFlags_AlwaysClamp);
			if (ImGui::IsItemActive() || ImGui::IsItemDeactivated())
			{
				SetRestitution(m_restitution);
			}
			// Damping
			ImGui::Text("Damping");
			ImGui::SameLine();
			ImGui::HelpMarker("0 : Outer Space");
			{
				ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
				ImGui::Text("Linear");
				ImGui::AlignedSameLine(0.5f);
				ImGui::SliderFloat("##LinearDamping", &m_linear_damp, 0.0f, 1.0f, "(0 ~ 1) %.3f", ImGuiSliderFlags_AlwaysClamp);
				if (ImGui::IsItemActive() || ImGui::IsItemDeactivated())
				{
					SetLinearDamping(m_linear_damp);
				}
				ImGui::Text("Angular");
				ImGui::AlignedSameLine(0.5f);
				ImGui::SliderFloat("##AngularDamping", &m_angular_damp, 0.0f, 1.0f, "(0 ~ 1) %.3f", ImGuiSliderFlags_AlwaysClamp);
				if (ImGui::IsItemActive() || ImGui::IsItemDeactivated())
				{
					SetAngularDamping(m_angular_damp);
				}
			}
			// Friction
			ImGui::Text("Friction");
			{
				ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
				ImGui::Text("Linear");
				ImGui::AlignedSameLine(0.5f);
				ImGui::DragFloat("##LinearFriction", &m_friction, 0.01f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
				if (ImGui::IsItemActive() || ImGui::IsItemDeactivated())
				{
					SetLinearFriction(m_friction);
				}
				ImGui::Text("Roll");
				ImGui::AlignedSameLine(0.5f);
				ImGui::DragFloat("##RollFriction", &m_rolling_friction, 0.01f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
				if (ImGui::IsItemActive() || ImGui::IsItemDeactivated())
				{
					SetRollingFriction(m_rolling_friction);
				}
				ImGui::Text("Spin");
				ImGui::AlignedSameLine(0.5f);
				ImGui::DragFloat("##SpinFriction", &m_spinning_friction, 0.01f, 0.0f, FLT_MAX, "%.2f", ImGuiSliderFlags_AlwaysClamp);
				if (ImGui::IsItemActive() || ImGui::IsItemDeactivated())
				{
					SetSpinningFriction(m_spinning_friction);
				}
			}
		}
		// Constraint
		{
			ImGui::Text("Constraint");
			ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
			{
				// Position
				ImGui::Text("Position");
				ImGui::AlignedSameLine(0.5f);
				ImGui::Text("X");
				ImGui::SameLine();
				if(ImGui::Checkbox("##Freeze Position X", &m_freeze_position_x))
				{
					SetConstraintPosition(m_freeze_position_x, m_freeze_position_y, m_freeze_position_z);
				}
				ImGui::SameLine();
				ImGui::Text("Y");
				ImGui::SameLine();
				if (ImGui::Checkbox("##Freeze Position Y", &m_freeze_position_y))
				{
					SetConstraintPosition(m_freeze_position_x, m_freeze_position_y, m_freeze_position_z);
				}
				ImGui::SameLine();
				ImGui::Text("Z");
				ImGui::SameLine();
				if (ImGui::Checkbox("##Freeze Position Z", &m_freeze_position_z))
				{
					SetConstraintPosition(m_freeze_position_x, m_freeze_position_y, m_freeze_position_z);
				}
				// Rotation
				ImGui::Text("Rotation");
				ImGui::AlignedSameLine(0.5f);
				ImGui::Text("X");
				ImGui::SameLine();
				if (ImGui::Checkbox("##Freeze Rotation X", &m_freeze_rotation_x))
				{
					SetConstraintRotation(m_freeze_rotation_x, m_freeze_rotation_y, m_freeze_rotation_z);
				}
				ImGui::SameLine();
				ImGui::Text("Y");
				ImGui::SameLine();
				if (ImGui::Checkbox("##Freeze Rotation Y", &m_freeze_rotation_y))
				{
					SetConstraintRotation(m_freeze_rotation_x, m_freeze_rotation_y, m_freeze_rotation_z);
				}
				ImGui::SameLine();
				ImGui::Text("Z");
				ImGui::SameLine();
				if (ImGui::Checkbox("##Freeze Rotation Z", &m_freeze_rotation_z))
				{
					SetConstraintRotation(m_freeze_rotation_x, m_freeze_rotation_y, m_freeze_rotation_z);
				}
			}
		}

		// RigidBodyを作り直す
		ImVec2 button_size;
		{
			const float width = ImGui::GetWindowWidth();
			constexpr float button_space = 0.2f;
			button_size.x = width * (1.0f - button_space * 2.0f);
			button_size.y = ImGui::DEFAULT_SPACE_SIZE * 1.5f;
			// ボタンの上にスペースを開ける
			ImGui::Separator();
			// ボタンを中央に置く
			ImGui::SetCursorPosX(width * button_space);
		}
		if (ImGui::Button("Recreate RigidBody", button_size))
		{
			RecreateCollisionObject();
		}
		ImGui::HelpText("If you want to change any of the parameters\nbelow from here, press this button");

		// Shapes
		Collider::ShapeGUI();
		ImGui::Separator();
		// Sleep threshold
		{
			ImGui::Text("Sleeping Threshold");
			ImGui::SameLine();
			ImGui::HelpMarker("The speed is judged to be stopped.");
			ImGui::Text("Linear");
			ImGui::AlignedSameLine(0.5f);
			ImGui::SliderFloat("##Linear Threshold", &m_linear_sleeping_threshold, 0.0f, 2.0f, "(0 ~ 2) %.2f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::Text("Angular");
			ImGui::AlignedSameLine(0.5f);
			ImGui::SliderFloat("##Angular Threshold", &m_angular_sleeping_threshold, 0.0f, 2.0f, "(0 ~ 2) %.2f", ImGuiSliderFlags_AlwaysClamp);
		}
		// Display parameter
		if (m_rigidbody)
		{
			const float label_width = ImGui::GetWindowWidth() * 0.3f;
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Text("RigidBody parameters");
			ImGui::Spacing();
			// 総質量
			ImGui::Text("Total mass :");
			ImGui::SameLine();
			ImGui::SetCursorPosX(label_width);
			ImGui::Text("%.2f kg", m_total_mass);
			// 速度
			{
				ImGui::Spacing();
				ImGui::Text("Velocity");
				ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
				// Linear
				const btVector3 linear_velocity = m_rigidbody->getLinearVelocity();
				ImGui::Text("Linear :");
				ImGui::SameLine();
				ImGui::SetCursorPosX(label_width);
				ImGui::Text("x(%.1f) y(%.1f) z(%.1f)", linear_velocity.x(), linear_velocity.y(), linear_velocity.z());
				// Angular
				const btVector3 angular_velocity = m_rigidbody->getAngularVelocity();
				ImGui::Text("Angular :");
				ImGui::SameLine();
				ImGui::SetCursorPosX(label_width);
				ImGui::Text("x(%.1f) y(%.1f) z(%.1f)", angular_velocity.x(), angular_velocity.y(), angular_velocity.z());
			}
			// 力
			{
				ImGui::Spacing();
				// Force
				const btVector3 total_force = m_rigidbody->getTotalForce();
				ImGui::Text("Force :");
				ImGui::SameLine();
				ImGui::SetCursorPosX(label_width);
				ImGui::Text("x(%.1f) y(%.1f) z(%.1f)", total_force.x(), total_force.y(), total_force.z());
				// Torque
				const btVector3 total_torque = m_rigidbody->getTotalTorque();
				ImGui::Text("Torque :");
				ImGui::SameLine();
				ImGui::SetCursorPosX(label_width);
				ImGui::Text("x(%.1f) y(%.1f) z(%.1f)", total_torque.x(), total_torque.y(), total_torque.z());
			}
			// 慣性
			ImGui::Spacing();
			ImGui::Text("Inertia :");
			ImGui::SameLine();
			ImGui::SetCursorPosX(label_width);
			ImGui::Text("x(%.1f) y(%.1f) z(%.1f)", m_inertia.x, m_inertia.y, m_inertia.z);
			// 重心
			ImGui::Spacing();
			ImGui::Text("Principal :");
			ImGui::SameLine();
			ImGui::SetCursorPosX(label_width);
			ImGui::Text("x(%.1f) y(%.1f) z(%.1f)", m_principal.x, m_principal.y, m_principal.z);

			ImGui::Separator();
		}
	}

	void RigidBody::OnGizmoEnter()
	{
		if (m_is_kinematic || !m_rigidbody)
			return;
		m_rigidbody->setCollisionFlags(m_rigidbody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		m_rigidbody->forceActivationState(DISABLE_DEACTIVATION);
	}

	void RigidBody::OnGizmoExit()
	{
		m_has_controlled_gizmo = true;
	}
#endif // USE_IMGUI

	void RigidBody::SetPosition(const VECTOR3& pos)
	{
		// 静的でないとき早期リターン
		if (!m_rigidbody || IsStatic())
			return;

		// RigidBodyの位置を現在のTransform位置に移動する
		const auto trans = GetTransform();
		if (IsKinematic())
		{
			trans->Position(pos);
		}
		else
		{
			Activate();
			m_rigidbody->getWorldTransform().
				setOrigin(ConvertVectorTobtVector(pos + VECTOR3::TransformRotate(m_principal, trans->Rotation())));
		}
	}

	void RigidBody::SetRotation(const Quaternion& rot)
	{
		// 静的でないとき早期リターン
		if (!m_rigidbody || IsStatic())
			return;

		// RigidBodyの位置を現在のTransform位置に移動する
		const auto trans = GetTransform();
		if (IsKinematic())
		{
			trans->Rotation(rot);
		}
		else
		{
			Activate();
			m_rigidbody->getWorldTransform().setRotation(ConvertQuaternionTobtQuaternion(rot));
		}
	}

	void RigidBody::RecreateCollisionObject()
	{
		// RigidBodyをWorldから外す
		PhysicsSystem::GetInstance()->GetWorld()->removeRigidBody(m_rigidbody.get());
		const int id = m_rigidbody->getUserIndex();
		m_rigidbody.reset();

		// AABB、慣性、質量の再計算
		RecalculateBody();

		// 新しくRigidBodyを作る
		{
			CreateRigidBody();

			// Scene,GameObject,Behaviourがアクティブならセット
			if (IsActiveCollider())
			{
				// レイヤーグループとマスクレイヤーの取得
				int group = 0;
				int mask = 0;
				CreateLayerGroupAndMask(group, mask);
				// レイヤーを設定してワールドに登録
				PhysicsSystem::GetInstance()->GetWorld()->addRigidBody(m_rigidbody.get(), group, mask);
			}

			// 前と同じIDを振る
			m_rigidbody->setUserIndex(id);
		}
		// 常時更新にするか
		if (m_is_kinematic || m_disable_deactivation)
		{
			m_rigidbody->setActivationState(DISABLE_DEACTIVATION);
		}
	}

	void RigidBody::UseGravity(const bool use_gravity)
	{
		m_use_gravity = use_gravity;

		// 重力を適用する
		ApplyGravitySetting();
	}

	bool RigidBody::UseGravity() const
	{
		return m_use_gravity;
	}

	void RigidBody::SetStatic(const bool is_static)
	{
		// 状態の代入
		m_is_static = is_static;

		// 早期リターン
		if (!m_rigidbody)
		{
			return;
		}

		// RigidBodyを作りなおす
		RecreateCollisionObject();
	}

	void RigidBody::SetKinematic(const bool is_kinematic)
	{
		// 状態の代入
		m_is_kinematic = is_kinematic;
		m_disable_deactivation = m_is_kinematic;

		// 物理更新状態の設定
		SetActivation();

		// 早期リターン
		if (!m_rigidbody)
		{
			return;
		}

		// RigidBodyを作りなおす
		RecreateCollisionObject();
	}

	void RigidBody::SetActivation()
	{
		// http://westplain.sakuraweb.com/translate/libGDX/Articles/xoppa/Using%2520the%2520libGDX%25203D%2520physics%2520Bullet%2520wrapper%2520-%2520part2.cgi
		// https://ousttrue.hatenadiary.org/entry/20100424/1272126923
		// DISABLE_DEACTIVATION: body は決して(自動的に)非アクティブ化されることはありません。
		// DISABLE_SIMULATION : body は決して(自動的に)アクティブ化されることはありません。
		// setWorldTransformの毎ステップ呼び出しが無効になる
		if (m_disable_deactivation)
		{
			m_rigidbody->forceActivationState(DISABLE_DEACTIVATION);
		}
		else
		{
			m_rigidbody->forceActivationState(ACTIVE_TAG);
		}
	}

	void RigidBody::Activate()
	{
		if (m_rigidbody)
		{
			// 常時更新のものはDISABLE_DEACTIVATION
			SetActivation();
		}
	}

	void RigidBody::ForceDisableDeactivation(bool force)
	{
		m_disable_deactivation = force;

		// 早期リターン
		if (!m_rigidbody)
		{
			return;
		}

		if (m_disable_deactivation)
		{
			m_rigidbody->forceActivationState(DISABLE_DEACTIVATION);
		}
		else
		{
			if (IsStatic())
			{
				m_rigidbody->forceActivationState(ISLAND_SLEEPING);
			}
			else
			{
				m_rigidbody->activate(true);
			}
		}
	}

	ShapeID RigidBody::AddShape(ShapeType shape_type, const VECTOR3& translate, const Quaternion& quat)
	{
		// Children Shapeの作成
		const ShapeID ret_id = Collider::AddShape(shape_type, translate, quat);

		return ret_id;
	}

	void RigidBody::RemoveShape(ShapeID id)
	{
		// Children Shapeの削除
		Collider::RemoveShape(id);
	}

	void RigidBody::SetMass(ShapeID id, float mass)
	{
		// IDのチェック
		const int idx = FindShapeDataIndex(id);
		// IDが存在しなかったら早期リターン
		if (idx < 0)
		{
			return;
		}

		m_shapes.at(idx)->SetMass(mass);
	}

	void RigidBody::AddForce(const VECTOR3& force)
	{
		if (!m_rigidbody)
			return;

		Activate();
		m_rigidbody->applyCentralForce(ConvertVectorTobtVector(force));
	}

	void RigidBody::AddForce(const VECTOR3& force, const VECTOR3& position)
	{
		if (!m_rigidbody)
			return;

		Activate();
		m_rigidbody->applyForce(
			ConvertVectorTobtVector(force),
			ConvertVectorTobtVector(position - m_principal)
		);
	}

	void RigidBody::AddImpulse(const VECTOR3& impulse)
	{
		if (!m_rigidbody)
			return;

		Activate();
		m_rigidbody->applyCentralImpulse(ConvertVectorTobtVector(impulse));
	}

	void RigidBody::AddImpulse(const VECTOR3& impulse, const VECTOR3& position)
	{
		if (!m_rigidbody)
			return;

		Activate();
		m_rigidbody->applyImpulse(
			ConvertVectorTobtVector(impulse),
			ConvertVectorTobtVector(position - m_principal)
		);
	}

	void RigidBody::AddPushImpulse(const VECTOR3& impulse)
	{
		if (!m_rigidbody)
			return;

		Activate();
		m_rigidbody->applyCentralPushImpulse(ConvertVectorTobtVector(impulse));
	}

	void RigidBody::AddPushImpulse(const VECTOR3& impulse, const VECTOR3& position)
	{
		if (!m_rigidbody)
			return;

		Activate();
		m_rigidbody->applyPushImpulse(
			ConvertVectorTobtVector(impulse),
			ConvertVectorTobtVector(position - m_principal)
		);
	}

	void RigidBody::AddTorque(const VECTOR3& torque)
	{
		if (!m_rigidbody)
			return;

		Activate();
		m_rigidbody->applyTorque(ConvertVectorTobtVector(torque));
	}

	void RigidBody::AddTorqueImpulse(const VECTOR3& torque_imp)
	{
		if (!m_rigidbody)
			return;

		Activate();
		m_rigidbody->applyTorqueImpulse(ConvertVectorTobtVector(torque_imp));
	}

	void RigidBody::AddTorqueTurnImpulse(const VECTOR3& torque_imp)
	{
		if (!m_rigidbody)
			return;

		Activate();
		m_rigidbody->applyTorqueTurnImpulse(ConvertVectorTobtVector(torque_imp));
	}

	void RigidBody::ResetForces()
	{
		if (!m_rigidbody)
			return;
		m_rigidbody->clearForces();
	}

	void RigidBody::SetGravity(const VECTOR3& gravity)
	{
		m_gravity = gravity;

		//　早期リターン
		if (!m_rigidbody)
			return;

		// 重力を適用する
		ApplyGravitySetting();
	}

	void RigidBody::SetLinearVelocity(const VECTOR3& velocity)
	{
		if (!m_rigidbody)
			return;

		Activate();
		m_rigidbody->setLinearVelocity(ConvertVectorTobtVector(velocity));
	}

	VECTOR3 RigidBody::GetLinearVelocity() const
	{
		return (m_rigidbody) ?
			ConvertbtVectorToVector(m_rigidbody->getLinearVelocity()) :
			VECTOR3::Zero;
	}

	void RigidBody::SetAngularVelocity(const VECTOR3& velocity)
	{
		if (!m_rigidbody)
			return;

		Activate();
		m_rigidbody->setAngularVelocity(ConvertVectorTobtVector(velocity));
	}

	VECTOR3 RigidBody::GetAngularVelocity() const
	{
		return (m_rigidbody) ?
			ConvertbtVectorToVector(m_rigidbody->getAngularVelocity()) :
			VECTOR3::Zero;
	}

	void RigidBody::SetTurnVelocity(const VECTOR3& velocity)
	{
		if (!m_rigidbody)
			return;

		Activate();
		m_rigidbody->setTurnVelocity(ConvertVectorTobtVector(velocity));
	}

	VECTOR3 RigidBody::GetTurnVelocity() const
	{
		return (m_rigidbody) ?
			ConvertbtVectorToVector(m_rigidbody->getTurnVelocity()) :
			VECTOR3::Zero;
	}

	void RigidBody::SetLinearDamping(float damp)
	{
		m_linear_damp = MyMath::Max(damp, 0.0f);
		if (m_rigidbody)
		{
			m_rigidbody->setDamping(m_linear_damp, m_angular_damp);
		}
	}

	void RigidBody::SetAngularDamping(float damp)
	{
		m_angular_damp = MyMath::Max(damp, 0.0f);
		if (m_rigidbody)
		{
			m_rigidbody->setDamping(m_linear_damp, m_angular_damp);
		}
	}

	void RigidBody::SetRestitution(float restitution)
	{
		m_restitution = MyMath::Max(restitution, 0.0f);
		if (m_rigidbody)
		{
			m_rigidbody->setRestitution(m_restitution);
		}
	}

	void RigidBody::SetLinearFriction(float friction)
	{
		m_friction = MyMath::Max(friction, 0.0f);
		if (m_rigidbody)
		{
			m_rigidbody->setFriction(m_friction);
		}
	}

	void RigidBody::SetRollingFriction(float friction)
	{
		m_rolling_friction = MyMath::Max(friction, 0.0f);
		if (m_rigidbody)
		{
			m_rigidbody->setRollingFriction(m_rolling_friction);
		}
	}

	void RigidBody::SetSpinningFriction(float friction)
	{
		m_spinning_friction = MyMath::Max(friction, 0.0f);
		if (m_rigidbody)
		{
			m_rigidbody->setSpinningFriction(m_spinning_friction);
		}
	}

	void RigidBody::SetConstraintPosition(bool x, bool y, bool z)
	{
		m_freeze_position_x = x;
		m_freeze_position_y = y;
		m_freeze_position_z = z;
		m_rigidbody->activate(true);
		btVector3 linear_factor;
		linear_factor.setX(x ? 0.0f : 1.0f);
		linear_factor.setY(y ? 0.0f : 1.0f);
		linear_factor.setZ(z ? 0.0f : 1.0f);
		m_rigidbody->setLinearFactor(linear_factor);
	}

	void RigidBody::SetConstraintRotation(bool x, bool y, bool z)
	{
		m_freeze_rotation_x = x;
		m_freeze_rotation_y = y;
		m_freeze_rotation_z = z;
		m_rigidbody->activate(true);
		btVector3 angular_factor;
		angular_factor.setX(x ? 0.0f : 1.0f);
		angular_factor.setY(y ? 0.0f : 1.0f);
		angular_factor.setZ(z ? 0.0f : 1.0f);
		m_rigidbody->setAngularFactor(angular_factor);
	}

	void RigidBody::OnCreated(const std::shared_ptr<Object>& p_self)
	{
		// モーションステートの作成
		{
			m_motion_state = std::make_unique<ColliderMotionState>(GetTransform(), this);
		}
		// ロード時にShapeがあれば作成
		if (!m_shapes.empty())
		{
			for (const auto& shape : m_shapes)
			{
				// 実体作成
				shape->CreateShape();
				// CompoundShapeに登録
				m_compound_shape->addChildShape(shape->GetbtTransform(), shape->GetPtr());
			}

			// AABB、慣性、質量の再計算
			RecalculateBody();
		}
		// リジッドボディの作成
		CreateRigidBody();

		// ロード時にKinematicなら適用する
		if (IsKinematic())
		{
			SetKinematic(m_is_kinematic);
		}

		// マネージャーに登録
		{
			// ObjectをColliderにキャスト
			std::shared_ptr<Collider> collider = std::static_pointer_cast<Collider>(p_self);

			// Managerに登録
			PhysicsSystem::GetInstance()->RegisterRigidBody(collider, m_rigidbody.get());
		}
	}

	void RigidBody::OnDestroyed()
	{
		// Managerから削除
		PhysicsSystem::GetInstance()->UnregisterRigidBody(m_rigidbody.get());

		// Motion Stateの破棄
		m_motion_state.reset();

		// リジッドボディの破棄
		m_rigidbody.reset();
	}

	void RigidBody::OnEnabled()
	{
		// 現在のTransformに更新する
		{
			const auto transform = GetTransform();
			const VECTOR3 pos = transform->Position();
			const Quaternion rot = transform->Rotation();
			m_rigidbody->getWorldTransform().setOrigin(ConvertVectorTobtVector(pos + VECTOR3::TransformRotate(m_principal, rot)));
			m_rigidbody->getWorldTransform().setRotation(ConvertQuaternionTobtQuaternion(rot));
		}

		// RigidBodyをWorldにセット
		{
			// レイヤーグループとマスクレイヤーの取得
			int group = 0;
			int mask = 0;
			CreateLayerGroupAndMask(group, mask);

			PhysicsSystem::GetInstance()->GetWorld()->addRigidBody(m_rigidbody.get(), group, mask);

			// 周辺の変化に合わせるために更新可能にする
			if (!IsStatic())
			{
				m_rigidbody->activate();
			}
		}
	}

	void RigidBody::OnDisabled()
	{
		// RigidBodyをWorldから外す
		PhysicsSystem::GetInstance()->GetWorld()->removeRigidBody(m_rigidbody.get());
	}

	void RigidBody::OnSetCollisionActive(const bool is_active)
	{
		if (is_active)
		{
			OnEnabled();
		}
		else
		{
			OnDisabled();
		}
	}

	void RigidBody::CreateRigidBody()
	{
		// Staticかどうかで質量を変える
		float mass = 0.0f;
		if (!m_is_static && !m_is_kinematic)
		{
			mass = GetTotalMass();
		}
		m_total_mass = mass;

		// 現在のパラメータを代入する
		btRigidBody::btRigidBodyConstructionInfo rb_info(static_cast<btScalar>(mass), m_motion_state.get(), m_compound_shape.get(), ConvertVectorTobtVector(m_inertia));
		{
			// 摩擦
			rb_info.m_friction = m_friction;
			rb_info.m_rollingFriction = m_rolling_friction;
			rb_info.m_spinningFriction = m_spinning_friction;
			// 速度減衰度
			rb_info.m_linearDamping = m_linear_damp;
			rb_info.m_angularDamping = m_angular_damp;
			// 反発係数
			rb_info.m_restitution = m_restitution;
			// 最小速度
			rb_info.m_linearSleepingThreshold = m_linear_sleeping_threshold;
			rb_info.m_angularSleepingThreshold = m_angular_sleeping_threshold;
		}
		// RigidBodyの作成
		m_rigidbody = std::make_unique<btRigidBody>(rb_info);

		m_rigidbody->updateInertiaTensor();

		// コリジョンフラグの設定
		if (m_is_static)
		{
			m_rigidbody->setCollisionFlags(m_rigidbody->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
		}
		else
		{
			m_rigidbody->setCollisionFlags(m_rigidbody->getCollisionFlags() & ~btCollisionObject::CF_STATIC_OBJECT);
		}
		if (m_is_kinematic)
		{
			m_rigidbody->setCollisionFlags(m_rigidbody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		}
		else
		{
			m_rigidbody->setCollisionFlags(m_rigidbody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
		}

		// 重力を適用する
		ApplyGravitySetting();
		// 拘束を適用する
		SetConstraintPosition(m_freeze_position_x, m_freeze_position_y, m_freeze_position_z);
		SetConstraintRotation(m_freeze_rotation_x, m_freeze_rotation_y, m_freeze_rotation_z);
	}

	void RigidBody::RecalculateBody()
	{
		// 子形状の再設定、結合AABBの再計算
		RecalculateShapeAABB();

		// 子形状がないときは早期リターン
		const size_t data_size = m_shapes.size();
		if (data_size <= 0)
			return;

		// 質量、子形状、トランスフォームの配列を作る
		const int child_num = m_compound_shape->getNumChildShapes();
		struct ChildShapeData
		{
			btCollisionShape* shape;
			btTransform transform;
		};
		std::vector<ChildShapeData> children_data(child_num);
		std::vector<btScalar> masses(child_num);
		for (int i = 0; i < child_num; ++i)
		{
			// 子形状データのIDを使って検索する
			const auto* shape = m_compound_shape->getChildShape(i);
			const int id = shape->getUserIndex();

			for (size_t idx = 0; idx < data_size; ++idx)
			{
				// ShapeIDが一致したら質量を入れる
				if (m_shapes.at(idx)->GetShapeID() == id)
				{
					masses.at(i) = static_cast<btScalar>(m_shapes.at(idx)->GetMass());
					children_data.at(idx).shape = m_shapes.at(idx)->GetPtr();
					children_data.at(idx).transform = m_shapes.at(idx)->GetbtTransform();
					break;
				}
			}
		}

		// スタティックならゼロ代入
		if (IsStatic())
		{
			m_inertia = VECTOR3::Zero;
			m_principal = VECTOR3::Zero;
		}
		else
		{
			// 慣性の計算
			btTransform principal;
			btVector3 inertia;
			m_compound_shape->calculatePrincipalAxisTransform(masses.data(), principal, inertia);
			m_inertia = ConvertbtVectorToVector(inertia);
			m_principal = ConvertbtVectorToVector(principal.getOrigin());

		}
		// 子形状を重心位置で補正
		for (int i = 0; i < child_num; ++i)
		{
			children_data.at(i).transform.setOrigin(children_data.at(i).transform.getOrigin() - ConvertVectorTobtVector(m_principal));
			m_compound_shape->updateChildTransform(i, children_data.at(i).transform);
		}
	}

	void RigidBody::ApplyGravitySetting()
	{
		if (m_rigidbody)
		{
			// 重力を適用する
			if (m_use_gravity && !IsKinematic() && !IsStatic())
			{
				// World共通の重力を適用する
				if (m_applied_world_gravity)
				{
					// ワールド重力を適用する
					m_rigidbody->setFlags(m_rigidbody->getFlags() & ~btRigidBodyFlags::BT_DISABLE_WORLD_GRAVITY);
					m_rigidbody->setGravity(ConvertVectorTobtVector(PhysicsSystem::GetInstance()->GetWorldGravity()));
				}
				// 個別に重力を適用する
				else
				{
					m_rigidbody->setFlags(m_rigidbody->getFlags() | btRigidBodyFlags::BT_DISABLE_WORLD_GRAVITY);
					m_rigidbody->setGravity(ConvertVectorTobtVector(m_gravity));
				}
			}
			// 重力を適用しない
			else
			{
				// 個別に重力を設定できるようにする
				m_rigidbody->setFlags(m_rigidbody->getFlags() | btRigidBodyFlags::BT_DISABLE_WORLD_GRAVITY);
				m_rigidbody->setGravity(btVector3(static_cast<btScalar>(0.0f), static_cast<btScalar>(0.0f), static_cast<btScalar>(0.0f)));
			}
		}
	}

	float RigidBody::GetTotalMass() const
	{
		float total_mass = 0.0f;

		// 形状データを探索して質量を取得
		const size_t num_shapes = m_shapes.size();
		for (size_t i = 0; i < num_shapes; ++i)
		{
			total_mass += m_shapes.at(i)->GetMass();
		}

		return total_mass;
	}

	void RigidBody::CreateLayerGroupAndMask(int& group, int& mask)
	{
		Layer layer = GetGameObject()->GetLayer();
		int current_layer = (1 << static_cast<int>(layer));
		int current_layer_mask = PhysicsSystem::GetInstance()->GetLayerMask(layer);

		// 衝突しないレイヤーの設定
		int ignore_layer = 0;
		{
			// Static同士は衝突しない
			if (IsStatic())
			{
				constexpr int static_layer = static_cast<int>(Layer::Static);
				current_layer |= (1 << static_layer);
				ignore_layer |= (1 << static_layer);
			}
		}

		group = current_layer;
		mask = current_layer_mask & ~ignore_layer;
	}

#pragma endregion
	// ~RigidBody class


	// ==============================================
	// Collider MotionState class methods
	// ==============================================
#pragma region ColliderMotionState
	RigidBody::ColliderMotionState::ColliderMotionState(
		const std::shared_ptr<Transform>& transform,
		RigidBody* rigidbody
	)
		: btMotionState()
	{
		// 自身のTransform,rigidbodyへの参照をキャッシュしておく
		m_transform = transform;
		m_rigidbody = rigidbody;
	}

	RigidBody::ColliderMotionState::~ColliderMotionState()
	{
		m_transform.reset();
	}

	void RigidBody::ColliderMotionState::getWorldTransform(btTransform& world_trans) const
	{
#ifdef USE_IMGUI
		// 操作した場合、キネマティックになっているので後始末
		if (m_rigidbody->m_has_controlled_gizmo)
		{
			m_rigidbody->m_has_controlled_gizmo = false;

			// RigidBodyが未作成なら早期リターン
			if (m_rigidbody->m_rigidbody == nullptr)
				return;

			m_rigidbody->m_rigidbody->setLinearVelocity(btVector3(static_cast<btScalar>(0.0f), static_cast<btScalar>(0.0f), static_cast<btScalar>(0.0f)));
			m_rigidbody->m_rigidbody->setAngularVelocity(btVector3(static_cast<btScalar>(0.0f), static_cast<btScalar>(0.0f), static_cast<btScalar>(0.0f)));

			// もともとキネマティックならリターン
			if (m_rigidbody->IsKinematic())
			{
				return;
			}

			// キネマティックのフラグを解除したタイミングで速度や座標を外から触ることが出来なくなる
			m_rigidbody->m_rigidbody->setCollisionFlags(m_rigidbody->m_rigidbody->getCollisionFlags() ^ btCollisionObject::CF_KINEMATIC_OBJECT);
			m_rigidbody->m_rigidbody->setDeactivationTime(static_cast<btScalar>(0.0));

			// Activation Stateの更新
			if (m_rigidbody->m_disable_deactivation)
			{
				return;
			}
			if (m_rigidbody->IsStatic())
			{
				m_rigidbody->m_rigidbody->forceActivationState(ISLAND_SLEEPING);
			}
			else
			{
				m_rigidbody->m_rigidbody->forceActivationState(ACTIVE_TAG);
			}

			return;
		}
#endif // USE_IMGUI

		// 物理世界の座標の更新
		const VECTOR3 pos = m_transform->Position();
		const Quaternion rot = m_transform->Rotation();

		// 動的なものは重心分移動しているため、重心を考慮して位置を設定する
		if (m_rigidbody->IsStatic())
		{
			world_trans.setOrigin(ConvertVectorTobtVector(pos));
		}
		else
		{
			// 回転原点を重心にする
			world_trans.setOrigin(
				ConvertVectorTobtVector(pos + VECTOR3::TransformRotate(m_rigidbody->m_principal, rot))
			);
		}
		world_trans.setRotation(ConvertQuaternionTobtQuaternion(rot));
	}

	void RigidBody::ColliderMotionState::setWorldTransform(const btTransform& world_trans)
	{
#ifdef USE_IMGUI
		// ゲームシーンを再生中以外は更新をしない
		if (!IGUI::Get().IsPlaying())
		{
			m_rigidbody->m_rigidbody->setCollisionFlags(m_rigidbody->m_rigidbody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			m_rigidbody->m_rigidbody->forceActivationState(DISABLE_DEACTIVATION);
			return;
		}
#endif // USE_IMGUI

		// 描画世界の座標の更新
		const Quaternion rot = ConvertbtQuaternionToQuaternion(world_trans.getRotation());

		// 動的なものは重心分移動しているため、重心を考慮して位置を設定する
		if (m_rigidbody->IsStatic())
		{
			m_transform->Position(ConvertbtVectorToVector(world_trans.getOrigin()));
		}
		else
		{
			m_transform->Position(
				ConvertbtVectorToVector(world_trans.getOrigin()) - VECTOR3::TransformRotate(m_rigidbody->m_principal, rot)
			);
		}
		m_transform->Rotation(rot);
	}
#pragma endregion
	// ~ColliderMotionState class


}// ~namespace TKGEngine