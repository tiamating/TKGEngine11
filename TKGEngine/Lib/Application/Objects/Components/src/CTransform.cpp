
#include "../inc/CTransform.h"

#include "Managers/GameObjectManager.h"
#include "Managers/SceneManager.h"
#include "Utility/inc/myfunc_math.h"

#ifdef USE_IMGUI
#include "Systems/inc/IGUI.h"
#endif // USE_IMGUI

#include <iterator>
#include <cassert>

REGISTERCOMPONENT(TKGEngine::Transform);

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	Transform::Transform()
		: Component(COMPONENT_TYPE::COMPONENT_GENERAL)
	{
		/* nothing */
	}

	Transform::~Transform()
	{
		/* nothing */
	}

	std::shared_ptr<Transform> Transform::Find(const std::string& obj_name) const
	{
		return OnFindRecurse(obj_name, GetTransform());
	}

#ifdef USE_IMGUI
	void Transform::OnGUI()
	{
		// 変更されたことを検知するフラグ
		bool has_changed = false;

		// 位置
		ImGui::Text("Pos");
		ImGui::AlignedSameLine(0.6f);
		ImGui::DragFloat3("##Pos", &(m_local_position.x), 0.02f, 0.f, 0.f, "%.3f", ImGuiSliderFlags_NoRoundToFormat);
		if (ImGui::IsItemActive())
		{
			has_changed = true;
		}
		// オイラー角
		ImGui::Text("Rot");
		ImGui::AlignedSameLine(0.6f);
		const auto prev_euler = m_local_euler_angle;
		ImGui::DragFloat3("##Rot", &(m_local_euler_angle.x), 0.2f, 0.f, 0.f, "%.2f", ImGuiSliderFlags_NoRoundToFormat);
		if (ImGui::IsItemActive())
		{
			// NaN回避
			if(std::isnan(m_local_euler_angle.x) || std::isnan(m_local_euler_angle.y) || std::isnan(m_local_euler_angle.z))
			{
				m_local_euler_angle = prev_euler;
			}
			else
			{
				m_local_rotation = Quaternion::EulerAngles(m_local_euler_angle);
				has_changed = true;
			}
		}
		// スケール
		ImGui::Text("Sca");
		ImGui::AlignedSameLine(0.6f);
		ImGui::DragFloat3("##Sca", &(m_local_scale.x), 0.02f, 0.0f, 0.0f, "%.3f", ImGuiSliderFlags_NoRoundToFormat);
		if (ImGui::IsItemActive())
		{
			has_changed = true;
		}

		// リセットボタン
		const float indent = ImGui::GetContentRegionAvail().x * 0.1f;
		ImGui::Indent(indent);
		if (ImGui::Button("Reset", ImVec2(ImGui::GetContentRegionAvail().x * 0.8f, ImGui::GetFontSize() * 1.2f)))
		{
			m_local_position = VECTOR3::Zero;
			m_local_rotation = Quaternion::Identity;
			m_local_euler_angle = m_local_rotation.ToEulerAngles();
			m_local_scale = VECTOR3::One;
			has_changed = true;
		}
		ImGui::Unindent(indent);

		// 変更があった場合、Transform自身とGizmoに知らせる
		if (has_changed)
		{
			m_is_changed = true;
			IGUI::Get().EnterControlTransform();
		}
		else
		{
			IGUI::Get().ExitControlTransform();
		}
	}
#endif // USE_IMGUI

	void Transform::OnCreate(const std::shared_ptr<Object>& p_self)
	{
		const auto p_comp = std::static_pointer_cast<Component>(p_self);
		m_gameObject = GameObjectManager::GetGameObject(p_comp->GetOwnerID());
		const auto p_transform = std::static_pointer_cast<Transform>(p_comp);
		p_transform->SetTransform(p_transform);
	}

	void Transform::OnDestroy()
	{
		if (!m_is_destroying)
		{
			m_is_root_deleter = true;
			// 親が存在したらそのchildrenリストから消す
			if (m_parent)
			{
				m_parent->EraseFromParent(GetInstanceID());
			}
			// ルートオブジェクトならシーンから削除する
			else
			{
				const int goid = GetGameObject()->GetGameObjectID();
				SceneManager::EraseGameObjectFromScene(goid);
			}

			// 自身とその子TransformのGameObjectも削除する
			RecurseDestroy();
		}
	}

	void Transform::RecurseOnRotation(const std::shared_ptr<Transform>& parent, Quaternion& rot) const
	{
		// ローカルの回転を掛ける
		rot *= parent->LocalRotation();
		// 親があれば再帰的に呼び出す
		if (parent->GetParent())
		{
			RecurseOnRotation(parent->GetParent(), rot);
		}
	}

	void Transform::RecurseOnEulerAngles(const std::shared_ptr<Transform>& parent, VECTOR3& angles) const
	{
		// ローカルのオイラー角を加算
		angles += parent->LocalEulerAngles();
		// 親があれば再帰的に呼び出す
		if (parent->GetParent())
		{
			RecurseOnEulerAngles(parent->GetParent(), angles);
		}
	}

	void Transform::RecurseOnScale(const std::shared_ptr<Transform>& parent, VECTOR3& scale) const
	{
		// ローカルのスケールを掛ける
		scale *= parent->LocalScale();
		// 親があれば再帰的に呼び出す
		if (parent->GetParent())
		{
			RecurseOnScale(parent->GetParent(), scale);
		}
	}

	void Transform::RecurseOnAxis(const std::shared_ptr<Transform>& parent, Quaternion& rot) const
	{
		// ローカルの回転を掛ける
		rot *= parent->LocalRotation();
		// 親があれば再帰的に呼び出す
		if (parent->GetParent())
		{
			RecurseOnAxis(parent->GetParent(), rot);
		}
	}

	void Transform::RecurseOnSetScene(const std::shared_ptr<Transform>& child, const int scene_id) const
	{
		const int child_cnt = child->GetChildCount();
		// 全ての子オブジェクトを再帰的に呼び出す
		for (int i = 0; i < child_cnt; ++i)
		{
			auto s_ptr = child->GetChild(i);
			if (s_ptr == nullptr)
				continue;
			RecurseOnSetScene(s_ptr, scene_id);
		}
		// 所属シーンを変更する
		child->GetGameObject()->SetScene(scene_id);
	}

	void Transform::RecurseOnCalculateWorldMatrix(const std::shared_ptr<Transform>& parent, MATRIX& matrix) const
	{
		matrix *= parent->GetAffineTransform();
		// 親があれば再帰的に計算する
		if (parent->GetParent())
		{
			RecurseOnCalculateWorldMatrix(parent->GetParent(), matrix);
		}
	}

	void Transform::RecurseDestroy()
	{
		m_is_destroying = true;
		m_parent.reset();

		if (m_child_count > 0)
		{
			for (auto&& child : m_children)
			{
				auto s_ptr = child.lock();
				if (s_ptr == nullptr)
					continue;
				s_ptr->RecurseDestroy();
			}
		}

		if (!m_is_root_deleter)
		{
			// 自身がアタッチされたGOを削除する
			const auto s_ptr = m_gameObject.lock();
			if (s_ptr == nullptr)
				return;
			ObjectManager::Destroy(s_ptr->GetInstanceID(), -1.0f);
		}
	}

	void Transform::EraseFromParent(InstanceID id)
	{
		const auto itr_end = m_children.end();
		for (auto itr = m_children.begin(); itr != itr_end;)
		{
			const auto s_ptr = itr->lock();
			if (!s_ptr)
			{
				itr = m_children.erase(itr);
				--m_child_count;
				continue;
			}

			if (s_ptr->GetInstanceID() == id)
			{
				m_children.erase(itr);
				--m_child_count;
				break;
			}

			++itr;
		}
	}

	void Transform::OnDeserialize()
	{
		// GameObjectの登録をサポート
		const auto gameobject = m_gameObject.lock();
		GameObjectManager::RegisterGameObjectOnDeserialize(gameobject);
		// 自身をManagerに登録
		const auto transform = gameobject->GetTransform();
		auto s_component = std::static_pointer_cast<Component>(transform);
		ComponentManager::AddComponentOnSerialize(gameobject->GetGameObjectID(), s_component);
		// Childrenが存在するならば子のParentに登録する
		if (transform->m_child_count <= 0)
			return;
		for (auto& child : m_children)
		{
			const auto child_transform = child.lock();
			child_transform->m_parent = transform;
		}
	}

	void Transform::Translate(const VECTOR3& translation, bool self_space)
	{
		if (self_space/* Local */)
		{
			m_local_position += translation;
		}
		else/* World */
		{
			//Position(Position() + translation);
			//VECTOR3 pos = Position();
			OnTranslateSetPosition(translation);
		}

		m_is_changed = true;
	}

	void Transform::Translate(float x, float y, float z, bool self_space)
	{
		Translate(VECTOR3(x, y, z), self_space);
	}

	void Transform::Rotate(const Quaternion& quat, bool self_space)
	{
		if (self_space/* Local */)
		{
			m_local_rotation *= quat;// *m_local_rotation;
		}
		else/* World */
		{
			Rotation(Rotation() * quat);
		}
		m_local_rotation = m_local_rotation.Normalize();
		m_local_euler_angle = m_local_rotation.ToEulerAngles();

		m_is_changed = true;
	}

	void Transform::Rotate(const VECTOR3& eulers, bool self_space)
	{
		Rotate(Quaternion::EulerAngles(eulers), self_space);
	}

	void Transform::Rotate(float x_angle, float y_angle, float z_angle, bool self_space)
	{
		Rotate(Quaternion::EulerAngles(x_angle, y_angle, z_angle), self_space);
	}

	void Transform::Rotate(const VECTOR3& axis, float angle, bool self_space)
	{
		Rotate(Quaternion::AxisAngle(axis, angle), self_space);
	}

	void Transform::LookAt(const std::shared_ptr<Transform>& target, const VECTOR3& world_up)
	{
		LookAt(target->Position(), world_up);
	}

	void Transform::LookAt(const VECTOR3& point, const VECTOR3& world_up)
	{
		const VECTOR3 forward = point - Position();
		Rotation(Quaternion::LookRotation(forward, world_up));
	}

	VECTOR3 Transform::Position() const
	{
		// 親がなければそのまま返す
		if (!m_parent)
			return m_local_position;
		// 親があれば親のワールド行列を使って求める
		return VECTOR3::TransformMatrix(m_local_position, m_parent->GetLocalToWorldMatrix());
	}

	void Transform::Position(const VECTOR3& pos)
	{
		// 親がなければそのまま代入
		if (!m_parent)
		{
			m_local_position = pos;
		}
		else
		{
			// Scale == 0.0fなら0代入でエラー回避
			const VECTOR3 lossy_scale = m_parent->LossyScale();
			const VECTOR3 world_pos = Position();
			VECTOR3 translate = pos - world_pos;
			if (MyMath::Approximately(lossy_scale.x, 0.0f))
			{
				translate.x = 0.0f;
			}
			else
			{
				translate.x = translate.x / lossy_scale.x;
			}

			if (MyMath::Approximately(lossy_scale.y, 0.0f))
			{
				translate.y = 0.0f;
			}
			else
			{
				translate.y = translate.y / lossy_scale.y;
			}

			if (MyMath::Approximately(lossy_scale.z, 0.0f))
			{
				translate.z = 0.0f;
			}
			else
			{
				translate.z = translate.z / lossy_scale.z;
			}

			m_local_position += VECTOR3::TransformRotate(translate, m_parent->Rotation().Inverse());
		}

		m_is_changed = true;
	}

	std::shared_ptr<Transform> Transform::OnFindRecurse(const std::string& obj_name, const std::shared_ptr<Transform>& transform)
	{
		const size_t child_num = transform->m_children.size();
		std::list<std::shared_ptr<Transform>> child_have_list;

		// 自身の子オブジェクトから検索する
		for (size_t i = 0; i < child_num; ++i)
		{
			auto child = transform->GetChild(i);
			// nullチェック
			if (child == nullptr)
				continue;

			// 名前比較
			if (child->GetGameObject()->GetName() == obj_name)
				return child;

			// 検索にかからず子を持つなら探索予定のリストに入れる
			if (child->GetChildCount() > 0)
				child_have_list.emplace_back(child);
		}
		// 最後まで見つからなかったらリスト内の子オブジェクトを再帰処理する
		std::shared_ptr<Transform> find_obj = nullptr;
		for (const auto& child : child_have_list)
		{
			find_obj = OnFindRecurse(obj_name, child);
			if (find_obj != nullptr)
				break;
		}
		return find_obj;
	}

	void Transform::OnTranslateSetPosition(const VECTOR3& translate)
	{
		//const VECTOR3 world_pos = Position();
		VECTOR3 translation = translate;

		// Scale == 0.0fなら0代入でエラー回避
		if (m_parent)
		{
			const VECTOR3 lossy_scale = m_parent->LossyScale();

			if (MyMath::Approximately(lossy_scale.x, 0.0f))
			{
				translation.x = 0.0f;
			}
			else
			{
				translation.x = translation.x / lossy_scale.x;
			}

			if (MyMath::Approximately(lossy_scale.y, 0.0f))
			{
				translation.y = 0.0f;
			}
			else
			{
				translation.y = translation.y / lossy_scale.y;
			}

			if (MyMath::Approximately(lossy_scale.z, 0.0f))
			{
				translation.z = 0.0f;
			}
			else
			{
				translation.z = translation.z / lossy_scale.z;
			}
		}

		if (m_parent)
		{
			m_local_position += VECTOR3::TransformRotate(translation, m_parent->Rotation().Inverse());
		}
		else
		{
			m_local_position += translation;
		}

		m_is_changed = true;
	}

	Quaternion Transform::Rotation() const
	{
		if (m_parent == nullptr)
			return m_local_rotation;

		// 自身より上の階層の回転を再帰的に掛ける
		Quaternion rot = m_local_rotation;
		RecurseOnRotation(m_parent, rot);

		return rot;
	}

	void Transform::Rotation(const Quaternion& quat)
	{
		// 親を持つオブジェクト
		if (m_parent)
		{
			m_local_rotation = (quat / m_parent->Rotation());
		}
		// 親を持たないオブジェクト
		else
		{
			m_local_rotation = quat;
		}
		m_local_rotation = m_local_rotation.Normalize();
		m_local_euler_angle = m_local_rotation.ToEulerAngles();

		m_is_changed = true;
	}

	VECTOR3 Transform::EulerAngles() const
	{
		if (m_parent == nullptr)
			return m_local_euler_angle;

		// 自身より上の階層のオイラー角を再帰的に加算
		VECTOR3 angles = m_local_euler_angle;
		RecurseOnEulerAngles(m_parent, angles);

		angles.x = MyMath::Mod(angles.x, 360.0f);
		angles.y = MyMath::Mod(angles.y, 360.0f);
		angles.z = MyMath::Mod(angles.z, 360.0f);
		return angles;
	}

	void Transform::EulerAngles(const VECTOR3& angles)
	{
		m_local_euler_angle += (angles - EulerAngles());
		m_local_euler_angle.x = MyMath::Mod(m_local_euler_angle.x, 360.0f);
		m_local_euler_angle.y = MyMath::Mod(m_local_euler_angle.y, 360.0f);
		m_local_euler_angle.z = MyMath::Mod(m_local_euler_angle.z, 360.0f);
		m_local_rotation = Quaternion::EulerAngles(m_local_euler_angle);

		m_is_changed = true;
	}

	VECTOR3 Transform::LossyScale() const
	{
		if (m_parent == nullptr)
			return m_local_scale;

		// 自身より上の階層のスケールを再帰的に掛ける
		VECTOR3 scale = m_local_scale;
		RecurseOnScale(m_parent, scale);

		return scale;
	}

	VECTOR3 Transform::Right() const
	{
		if (m_parent == nullptr)
			return m_local_rotation.ToRotationMatrix().Right();

		// 自身より上の階層の回転を再帰的に掛ける
		Quaternion rot = m_local_rotation;
		RecurseOnAxis(m_parent, rot);

		return rot.ToRotationMatrix().Right();
	}

	void Transform::Right(const VECTOR3& right)
	{
		Rotation(Quaternion::FromToRotation(VECTOR3::Right, right));

		//const Quaternion q_from_to = Quaternion::FromToRotation(Right(), right);
		//m_local_rotation *= q_from_to;
		//m_local_euler_angle = m_local_rotation.ToEulerAngles();

		//m_is_changed = true;
	}

	VECTOR3 Transform::Up() const
	{
		if (m_parent == nullptr)
			return m_local_rotation.ToRotationMatrix().Up();

		// 自身より上の階層の回転を再帰的に掛ける
		Quaternion rot = m_local_rotation;
		RecurseOnAxis(m_parent, rot);

		return rot.ToRotationMatrix().Up();
	}

	void Transform::Up(const VECTOR3& up)
	{
		Rotation(Quaternion::FromToRotation(VECTOR3::Up, up));

		//const Quaternion q_from_to = Quaternion::FromToRotation(Up(), up);
		//m_local_rotation *= q_from_to;
		//m_local_euler_angle = m_local_rotation.ToEulerAngles();

		//m_is_changed = true;
	}

	VECTOR3 Transform::Forward() const
	{
		if (m_parent == nullptr)
			return m_local_rotation.ToRotationMatrix().Forward();

		// 自身より上の階層の回転を再帰的に掛ける
		Quaternion rot = m_local_rotation;
		RecurseOnAxis(m_parent, rot);

		return rot.ToRotationMatrix().Forward();
	}

	void Transform::Forward(const VECTOR3& forward)
	{
		Rotation(Quaternion::FromToRotation(VECTOR3::Forward, forward));

		//const Quaternion q_from_to = Quaternion::FromToRotation(Forward(), forward);
		//m_local_rotation *= q_from_to;
		//m_local_euler_angle = m_local_rotation.ToEulerAngles();

		//m_is_changed = true;
	}

	void Transform::SetParent(bool keep_world_pos)
	{
		// 親を持っていなければ早期リターン
		if (m_parent == nullptr)
			return;

		// ワールド座標を維持するなら
		if (keep_world_pos == true)
		{
			// 新しい相対変換行列をセットする
			SetRelativeMatrix(GetLocalToWorldMatrix());
		}

		// 親の子リストを探索して自身のIDを見つけたらリストから消去
		const int id = GetInstanceID();
		auto& children = m_parent->m_children;
		const auto itr_end = children.end();
		for (auto itr = children.begin(); itr != itr_end; ++itr)
		{
			auto s_ptr = itr->lock();
			if (s_ptr == nullptr)
				continue;

			// IDが一致したら
			if (s_ptr->GetInstanceID() == id)
			{
				// 子リストから消去
				children.erase(itr);

				// 子のカウントをデクリメント
				--(m_parent->m_child_count);
				break;
			}
		}
		m_parent.reset();

		// シーンのルートにセットする
		{
			const std::shared_ptr<IGameObject> s_go = GetGameObject();
			const auto scene_id = s_go->GetScene();
			SceneManager::MoveGameObjectToScene(s_go->GetGameObjectID(), scene_id);
			s_go->ApplyActiveInHierarchy(true, SceneManager::GetActive(scene_id));
		}
	}

	void Transform::SetParent(const std::shared_ptr<Transform>& parent, bool keep_world_pos)
	{
		// parentがなければRootになるようにする
		if (parent == nullptr)
		{
			SetParent(keep_world_pos);
			return;
		}

		// 既に親子になっていたら早期リターン
		if (m_parent == parent)
		{
			return;
		}

		const std::shared_ptr<Transform>& p_transform = GetTransform();
		const std::shared_ptr<IGameObject> s_go = GetGameObject();

		// 新しいシーンをセットする。もし、prevがルートならシーンルートから削除、prevが子で所属シーンが異なれば自身の子を含めて変更する
		{
			if (p_transform->GetParent() == nullptr)
			{
				// prevがルートなのでルートから削除
				SceneManager::EraseGameObjectFromScene(s_go->GetGameObjectID());
			}
			const int parent_scene_id = parent->GetGameObject()->GetScene();
			const int prev_self_scene_id = s_go->GetScene();
			// 親のシーンと自身のシーンが異なるときは自分以下のオブジェクトの所属シーンを変更する
			if (parent_scene_id != prev_self_scene_id)
			{
				// 再帰処理で自身以下の全ての所属シーンを変更する
				RecurseOnSetScene(p_transform, parent_scene_id);
			}
		}

		// ワールド座標を維持するなら
		if (keep_world_pos)
		{
			// 親のInvWorld行列を取得
			const MATRIX parent_inv_world_matrix = parent->GetWorldToLocalMatrix();

			// 自身の現在のWorld行列を取得
			const MATRIX self_world_matrix = GetLocalToWorldMatrix();

			// 新しい親から見た相対World行列を求める
			const MATRIX new_local_matrix = self_world_matrix * parent_inv_world_matrix;

			// 新しい相対変換行列をセットする
			SetRelativeMatrix(new_local_matrix);
		}
		// 現在の親がいるなら
		if (m_parent)
		{
			// 現在の自身の親から自身を削除
			const int id = GetInstanceID();
			auto& children = m_parent->m_children;
			const auto itr_end = children.end();
			for (auto itr = children.begin(); itr != itr_end; ++itr)
			{
				auto s_ptr = itr->lock();
				if (s_ptr == nullptr)
					continue;

				// IDが一致したら
				if (s_ptr->GetInstanceID() == id)
				{
					// 子リストから消去
					children.erase(itr);

					// 子のカウントをデクリメント
					--(m_parent->m_child_count);
					break;
				}
			}
		}
		// 親の子に自身をセット
		parent->m_children.emplace_back(p_transform);
		++parent->m_child_count;
		// 自身の親にparentをセット
		p_transform->m_parent = parent;

		// active状態の更新
		s_go->ApplyActiveInHierarchy(parent->GetGameObject()->GetActiveHierarchy(), SceneManager::GetActive(s_go->GetScene()));
	}

	std::weak_ptr<Transform> Transform::FindFromChild(const std::string& name)
	{
		if (m_child_count <= 0)
		{
			return std::weak_ptr<Transform>();
		}

		const auto itr_end = m_children.end();
		for (auto itr = m_children.begin(); itr != itr_end; ++itr)
		{
			const auto ref = itr->lock();
			if (ref == nullptr) continue;

			if (ref->GetName() == name)
			{
				return *itr;
			}
		}
		// Not find.
		LOG_ASSERT("error. Transform::FindFromChild() can not find child.");
		return std::weak_ptr<Transform>();
	}

	std::shared_ptr<Transform> Transform::GetChild(const int index)
	{
		if (m_child_count < index)
		{
			// Out of range
			return nullptr;
		}

		auto itr = m_children.begin();
		std::advance(itr, index);

		auto s_ptr = itr->lock();
		return s_ptr;
	}

	int Transform::GetChildCount() const
	{
		return m_child_count;
	}

	const MATRIX& Transform::GetAffineTransform()
	{
		// パラメータに変更があった時のみ行列を作成する
		if (!m_is_changed)
		{
			return m_affine_transform;
		}

		m_affine_transform = MATRIX::CreateWorld(m_local_position, m_local_rotation, m_local_scale);

		// 行列の更新完了
		m_is_changed = false;
		return m_affine_transform;
	}

	MATRIX Transform::GetLocalToWorldMatrix()
	{
		MATRIX matrix = this->GetAffineTransform();
		// 親がなければそのまま返す
		if (!m_parent)
			return matrix;
		// 再帰的に計算する
		RecurseOnCalculateWorldMatrix(m_parent, matrix);
		return matrix;
	}

	MATRIX Transform::GetWorldToLocalMatrix()
	{
		return GetLocalToWorldMatrix().Invert();
	}

	void Transform::SetRelativeMatrix(const MATRIX& m)
	{
		m.Decompose(m_local_scale, m_local_rotation, m_local_position);
		m_local_euler_angle = m_local_rotation.ToEulerAngles();

		m_is_changed = true;
	}

	void Transform::SetWorldMatrix(const MATRIX& m)
	{
		// 親オブジェクトがないならば相対行列をセットする処理と同じ
		if (m_parent == nullptr)
		{
			SetRelativeMatrix(m);
			return;
		}

		// 親オブジェクトの逆ワールド行列を取得
		const MATRIX parent_inv_world_matrix = m_parent->GetWorldToLocalMatrix();

		// 相対行列を作成
		const MATRIX relative_matrix = m * parent_inv_world_matrix;

		// 相対行列をセットする
		SetRelativeMatrix(relative_matrix);
	}

}// namespace TKGEngine