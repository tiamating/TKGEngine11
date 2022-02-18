
#include "../inc/IGameObject.h"
#include "Components/inc/CTransform.h"

#ifdef USE_IMGUI
#include "Systems/inc/IGUI.h"
#endif // USE_IMGUI
#include "Systems/inc/LogSystem.h"
#include "Managers/GameObjectManager.h"
#include "Managers/ComponentManager.h"
#include "Managers/MonoBehaviourManager.h"
#include "Managers/SceneManager.h"
#include "Systems/inc/PhysicsSystem.h"

#include <memory>
#include <cassert>
#include <functional>

namespace TKGEngine
{
	/// <summary>
	/// GameObject module
	/// </summary>
	class GameObject
		: public IGameObject
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;

		GameObject();
		virtual ~GameObject();

		// IGameObject
		void Destroy(float delay = 0.0f) override;

		GameObjectID GetGameObjectID() const override;
		std::shared_ptr<Transform> GetTransform() override;
		void SetActive(bool is_active) override;
		bool GetActiveSelf() const override;
		bool GetActiveHierarchy() const override;

		void SetTag(Tag tag) override;
		Tag GetTag() const override;
		Layer GetLayer() const override;
		void SetLayer(Layer layer) override;
		SceneID GetScene() const override;
		void SetScene(SceneID scene_id) override;
		// ~IGameObject


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */

	private:
		friend class cereal::access;
		template <class Archive>
		void load(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				// GameObject�̃����o�ǂݍ���
				archive(
					cereal::base_class<IGameObject>(this),
					CEREAL_NVP(m_active_self),
					CEREAL_NVP(m_active_hierarchy),
					CEREAL_NVP(m_is_active),
					CEREAL_NVP(m_transform),
					CEREAL_NVP(m_tag),
					CEREAL_NVP(m_layer)
				);
				// �A�^�b�`���ꂽComponent�����X�g��Manager�ɓo�^����
				archive(
					cereal::make_nvp("ComponentNameList", ComponentManager::GetComponentNameList(GetGameObjectID()))
				);
			}
		}
		template <class Archive>
		void save(Archive& archive, const std::uint32_t version) const
		{
			//if (version > 0)
			{
				archive(
					cereal::base_class<IGameObject>(this),
					CEREAL_NVP(m_active_self),
					CEREAL_NVP(m_active_hierarchy),
					CEREAL_NVP(m_is_active),
					CEREAL_NVP(m_transform),
					CEREAL_NVP(m_tag),
					CEREAL_NVP(m_layer)
				);
				// �A�^�b�`���ꂽComponent�����X�g�������o��
				archive(
					cereal::make_nvp("ComponentNameList", ComponentManager::GetComponentNameList(GetGameObjectID()))
				);
			}
		}

		// ==============================================
		// private methods
		// ==============================================
#ifdef USE_IMGUI
		void OnGUI() override;	// For inspector
		void OnGUIHierarchy() override;
#endif // USE_IMGUI

		// Object
		void OnCreate(const std::shared_ptr<Object>& p_self) override;
		void OnDestroy() override;
		// ~Object

		// �ċA�I�Ɏ��g�ȉ��̏����V�[����ύX����
		void RecurseOnSetScene(const std::shared_ptr<Transform>& transform, const SceneID scene_id);
		// �ċA�����ŃV�[���ԍ����Z�b�g����ۂɂ̂ݎg�p
		void RecurseSetScene(SceneID scene_id) override;
		// �B��ActiveHierarchy�𑀍삷��
		void ApplyActiveInHierarchy(const bool is_hierarchy_active, const bool is_scene_active) override;


		// ==============================================
		// private variables
		// ==============================================
		bool m_active_self = true;
		bool m_active_hierarchy = true;
		bool m_is_active = true;

		GameObjectID m_gameObject_id = 0;
		std::weak_ptr<Transform> m_transform;

		Tag m_tag = Tag::Default;
		Layer m_layer = Layer::Default;
		SceneID m_scene_id = SceneManager::INVALID_SCENE_INDEX;
	};


	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	// ---------------
	// IGameObject
	// ---------------
	IGameObject::IGameObject()
		: Object(OBJECT_TYPE::OBJECT_GAMEOBJECT)
	{
		/* nothing */
	}

	IGameObject* IGameObject::CreateInterface()
	{
		return new GameObject();
	}

	std::shared_ptr<IGameObject> IGameObject::Create()
	{
		return GameObjectManager::CreateGameObject();
	}

	std::shared_ptr<IGameObject> IGameObject::Create(const char* name)
	{
		return GameObjectManager::CreateGameObject(name);
	}

	std::shared_ptr<IGameObject> IGameObject::Duplicate(const std::shared_ptr<IGameObject>& gameobject)
	{
		return GameObjectManager::DuplicateGameObject(gameobject);
	}

	std::shared_ptr<IGameObject> IGameObject::Find(const std::string& name)
	{
		return GameObjectManager::FindGameObject(name);
	}

	template<>
	void IGameObject::RemoveComponent<Transform>()
	{
		LOG_ASSERT("Invalid process. Transform can not Removed.");
	}

	// ---------------
	// GameObject
	// ---------------
	GameObject::GameObject()
	{
		m_gameObject_id = this->GetInstanceID();
	}

	GameObject::~GameObject()
	{
		/* nothing */
	}

#ifdef USE_IMGUI
	void GameObject::OnGUI()
	{
		// GameObject��GUI�\��
		if (ImGui::Begin("Inspector"))
		{
			ImGui::IDWrapped id(this);

			// Active self
			{
				bool active = m_active_self;
				ImGui::IDWrapped self_id("ActiveSelf");
				if (ImGui::Checkbox("", &active))
				{
					if (active != m_active_self)
					{
						SetActive(active);
					}
				}
			}
			ImGui::SameLine();
			// GameObject name
			{
				constexpr size_t str_len = 256;
				static char str[str_len] = "\0";
				strncpy_s(str, GetName(), str_len);
				ImGui::IDWrapped go_name_id("GOName");
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				if (ImGui::InputText("", str, str_len, ImGuiInputTextFlags_EnterReturnsTrue))
				{
					SetName(str);
				}
			}
			// Tag
			{
				ImGui::IDWrapped tag_id("Tag");
				ImGui::Text("Tag");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(-ImGui::GetContentRegionAvail().x * 0.6f);
				if (ImGui::BeginCombo("##Tag", NAMEOF_ENUM(m_tag).data(), ImGuiComboFlags_HeightLargest))
				{
					for (int i = 0; i < static_cast<int>(Tag::MAX_TAG_NUM); ++i)
					{
						const bool is_selected = (static_cast<int>(m_tag) == i);
						if (ImGui::Selectable(NAMEOF_ENUM(static_cast<Tag>(i)).data(), is_selected))
						{
							m_tag = static_cast<Tag>(i);
						}

						if (is_selected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}
			ImGui::SameLine();
			// Layer
			{
				ImGui::IDWrapped layer_id("Layer");
				ImGui::Text("Layer");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				if (ImGui::BeginCombo("##Layer", NAMEOF_ENUM(m_layer).data(), ImGuiComboFlags_HeightLargest))
				{
					for (int i = 0; i < static_cast<int>(Layer::MAX_USER_LAYER_NUM); ++i)
					{
						const bool is_selected = (static_cast<int>(m_layer) == i);
						if (ImGui::Selectable(NAMEOF_ENUM(static_cast<Layer>(i)).data(), is_selected))
						{
							m_layer = static_cast<Layer>(i);
						}

						if (is_selected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}
		}
		ImGui::End();

		// Attach���ꂽ�R���|�[�l���g��GUI�\��
		ComponentManager::OnGUI(m_gameObject_id);
		// Add Component�{�^��
		ComponentManager::OnGUIAddComponent(m_gameObject_id);
		// Remove Component�{�^��
		ComponentManager::OnGUIRemoveComponent(m_gameObject_id);
	}

	void GameObject::OnGUIHierarchy()
	{
		static std::function<void(std::shared_ptr<Transform>&)> recurse = [&](const std::shared_ptr<Transform>& tr)
		{
			// �g�p����p�����[�^�̎��O�擾
			const int child_cnt = tr->GetChildCount();
			const auto& go = tr->GetGameObject();
			const GameObjectID goid = go->GetGameObjectID();
			const char* name = go->GetName();
			const bool active = go->GetActiveHierarchy();
			const bool selecting = IGUI::Get().IsSelecting(goid);


			ImGuiTreeNodeFlags flags =
				ImGuiTreeNodeFlags_OpenOnArrow |
				ImGuiTreeNodeFlags_SpanAvailWidth |
				ImGuiTreeNodeFlags_OpenOnDoubleClick;

			ImGui::PushID(go.get());
			// �I�𒆂̃I�u�W�F�N�g�Ή�
			if (selecting)
			{
				flags |= ImGuiTreeNodeFlags_Selected;
			}

			// �q�G�����L�[�̖��[��
			if (child_cnt == 0)
			{
				flags |= (ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);

				if (active)
				{
					ImGui::TreeNodeEx(name, flags, name);
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
					ImGui::TreeNodeEx(name, flags, name);
					ImGui::PopStyleColor();
				}

				// �I�����Ή�
				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					if (ImGui::GetIO().KeyCtrl)
					{
						if (selecting)
							IGUI::Get().RemoveSelectionObject(goid);
						else
							IGUI::Get().AddSelectionObject(goid);
					}
					else
					{
						IGUI::Get().SetSelectionObject(goid);
					}
				}
				// �E�N���b�N�����A�C�e����ۑ�����
				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					IGUI::Get().SetRightClickObject(goid);
				}
			}
			// �q�����ꍇ
			else
			{
				bool is_open = false;
				if (active)
				{
					is_open = ImGui::TreeNodeEx(name, flags, name);
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
					is_open = ImGui::TreeNodeEx(name, flags, name);
					ImGui::PopStyleColor();
				}

				// �I�����Ή�
				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					if (ImGui::GetIO().KeyCtrl)
					{
						if (selecting)
							IGUI::Get().RemoveSelectionObject(goid);
						else
							IGUI::Get().AddSelectionObject(goid);
					}
					else
					{
						IGUI::Get().SetSelectionObject(goid);
					}
				}
				// �E�N���b�N�����A�C�e����ۑ�����
				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					IGUI::Get().SetRightClickObject(goid);
				}

				// Tree node�W�J��
				if (is_open)
				{
					ImGui::Indent(ImGui::INDENT_VALUE);
					for (int i = 0; i < child_cnt; ++i)
					{
						auto child = tr->GetChild(i);
						recurse(child);
					}
					ImGui::Unindent(ImGui::INDENT_VALUE);
					ImGui::TreePop();
				}
			}
			ImGui::PopID();
		};

		auto s_tr = GetTransform();
		recurse(s_tr);
	}
#endif // USE_IMGUI

	void GameObject::OnCreate(const std::shared_ptr<Object>& p_self)
	{
		m_transform = AddComponent<Transform>();
	}

	void GameObject::OnDestroy()
	{
		// OnDestroyed�֐��̌Ăяo��
		MonoBehaviourManager::OnDestroyed(m_gameObject_id);
		// ����GameObject�ɃA�^�b�`���ꂽ���ׂẴR���|�[�l���g��j��
		ComponentManager::RemoveAllComponent(m_gameObject_id);
		// GameObject�̍폜
		GameObjectManager::EraseGameObject(m_gameObject_id);
	}

	void GameObject::Destroy(float delay)
	{
		Object::Destroy(m_gameObject_id, delay);
	}

	GameObjectID GameObject::GetGameObjectID() const
	{
		return m_gameObject_id;
	}

	std::shared_ptr<Transform> GameObject::GetTransform()
	{
		return m_transform.lock();
	}

	void GameObject::SetActive(const bool is_active)
	{
		// ���݂�active_self�ƕω����Ȃ���Α������^�[��
		if (is_active == m_active_self)
			return;

		// �V����active_self���Z�b�g
		m_active_self = is_active;

		// ���g�ȉ���active_hierarchy���X�V����
		ApplyActiveInHierarchy(m_active_hierarchy, SceneManager::GetActive(GetScene()));
	}

	void GameObject::ApplyActiveInHierarchy(const bool is_hierarchy_active, const bool is_scene_active)
	{
		// �V����active_hierarchy���Z�b�g����
		m_active_hierarchy = is_hierarchy_active;

		// �V����is_active�����߂�
		const bool new_active = is_hierarchy_active && m_active_self;

		// m_is_active���ω����Ă��邩�ǂ���
		if (m_is_active == new_active)
		{
			// �ω����Ă��Ȃ��Ȃ瑁�����^�[��
			return;
		}
		// �V����is_active���Z�b�g����
		m_is_active = new_active;
		
		// �V�[�����A�N�e�B�u�Ȃ�OnEnable,OnDisable���Ă�
		if(is_scene_active)
		{
			if (m_is_active)
			{
				// MonoBehaviour::OnEnable
				MonoBehaviourManager::OnEnable(m_gameObject_id);
			}
			else
			{
				// MonoBehaviour::OnDisable
				MonoBehaviourManager::OnDisable(m_gameObject_id);
			}
			// Collider�̏�Ԃ�ω�������
			PhysicsSystem::GetInstance()->SetCollisionActive(m_gameObject_id, m_is_active);
		}

		// ���g�̎q�I�u�W�F�N�g�ɂ����݂�active_hierarchy��K�p����
		{
			// ���g��Transform���擾����
			const auto transform = m_transform.lock();
			if (transform == nullptr)
			{
				LOG_ASSERT("failed get self transform. ApplyActiveInHierarchy()");
				return;
			}

			// �q�̐����擾����
			const int child_cnt = transform->GetChildCount();
			for (int i = 0; i < child_cnt; ++i)
			{
				auto s_ptr = transform->GetChild(i);
				if (s_ptr == nullptr)
					continue;
				auto child_go = s_ptr->GetGameObject();
				if (child_go == nullptr)
					continue;

				// ���݂�active_hierarchy��K�p����
				child_go->ApplyActiveInHierarchy(m_is_active, is_scene_active);
			}
		}
	}

	bool GameObject::GetActiveSelf() const
	{
		return m_active_self;
	}

	bool GameObject::GetActiveHierarchy() const
	{
		// �q�G�����L�[�ƃZ���t������True�Ȃ�ActiveHierarchy��True
		return m_is_active;
	}

	void GameObject::SetTag(Tag tag)
	{
		m_tag = tag;
	}

	Tag GameObject::GetTag() const
	{
		return m_tag;
	}

	Layer GameObject::GetLayer() const
	{
		return m_layer;
	}

	void GameObject::SetLayer(const Layer layer)
	{
		m_layer = layer;
	}

	int GameObject::GetScene() const
	{
		return m_scene_id;
	}

	void GameObject::SetScene(const SceneID scene_id)
	{
		// ���݂̏����V�[������ω����Ȃ���Α������^�[��
		if (m_scene_id == scene_id)
			return;
		// ���g�̏����V�[����ύX����
		m_scene_id = scene_id;
		// ���g�ȉ��̏����V�[����ύX����
		const auto p_tr = GetTransform();
		RecurseOnSetScene(p_tr, scene_id);
	}

	void GameObject::RecurseOnSetScene(const std::shared_ptr<Transform>& transform, const SceneID scene_id)
	{
		// �q�I�u�W�F�N�g��T��
		const int child_cnt = transform->GetChildCount();
		for (int i = 0; i < child_cnt; ++i)
		{
			auto child = transform->GetChild(i);
			if (child == nullptr)
				continue;
			RecurseOnSetScene(child, scene_id);
		}
		// RecurseSetScene�ŃV�[����ύX����
		const auto s_go = transform->GetGameObject();
		s_go->RecurseSetScene(scene_id);
	}

	void GameObject::RecurseSetScene(SceneID scene_id)
	{
		m_scene_id = scene_id;
	}

}// namespace TKGEngine

CEREAL_CLASS_VERSION(TKGEngine::GameObject, 1)
CEREAL_REGISTER_TYPE_WITH_NAME(TKGEngine::GameObject, "TKGEngine::GameObject")
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::IGameObject, TKGEngine::GameObject)
// archive����load save���g�p����B�������
CEREAL_SPECIALIZE_FOR_ALL_ARCHIVES(TKGEngine::GameObject, cereal::specialization::member_load_save)