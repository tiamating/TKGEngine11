

#include "ComponentManager.h"

#include "Components/Component.h"
#include "Components/inc/CMonoBehaviour.h"

#include <functional>

// Preprocess::RegisterModule
std::unique_ptr<std::unordered_map<std::string, Preprocess::RegisterModule::ComponentRegisteredData>> Preprocess::RegisterModule::m_component_function_index = nullptr;
// ~Preprocess::RegisterModule

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Static variable declaration
	////////////////////////////////////////////////////////
	std::unordered_map<std::string, ComponentTypeID> ComponentManager::m_typeName_id_index;
	std::unordered_map<std::string, ComponentTypeID> ComponentManager::m_componentName_id_index;
	std::unordered_map<ComponentTypeID, std::string> ComponentManager::m_componentTypeID_typeName_index;
	std::unordered_map<ComponentTypeID, std::function<std::shared_ptr<Component>(GameObjectID)>> ComponentManager::m_add_functions;
	std::unordered_map<GameObjectID, std::unordered_map<std::string, std::weak_ptr<Component>>> ComponentManager::m_gameObject_components_typename_index;
	std::unordered_map<GameObjectID, std::list<std::string>> ComponentManager::m_gameObject_attached_components_list;

	const std::string ComponentManager::m_transform_class_name = "TKGEngine::Transform";
	const std::string ComponentManager::m_transform_type_name = "class TKGEngine::Transform";
	std::mutex ComponentManager::m_mutex;


	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	void ComponentManager::SetUpComponentTypeID()
	{
		// 現在のプロジェクト内に存在するコンポーネントとTypeIDを紐づける
		RegisterComponentType();
	}

#ifdef USE_IMGUI
	void ComponentManager::OnGUI(GameObjectID goid)
	{
		// アタッチされたComponentマップの取得
		const auto itr_map_find = m_gameObject_components_typename_index.find(goid);
		if (itr_map_find == m_gameObject_components_typename_index.end())
		{
			return;
		}
		auto& components_map = itr_map_find->second;
		const auto itr_map_end = components_map.end();
		// アタッチされたComponent名リストの取得
		const auto itr_name_list_find = m_gameObject_attached_components_list.find(goid);
		if (itr_name_list_find == m_gameObject_attached_components_list.end())
		{
			return;
		}
		auto& name_list = itr_name_list_find->second;
		const auto itr_name_end = name_list.end();
		// Inspector
		ImGui::IDWrapped object_id(static_cast<int>(goid));
		if (ImGui::Begin("Inspector"))
		{
			// 追加された順にGUI表示していく
			auto itr_name_find = name_list.begin();
			for (; itr_name_find != itr_name_end; ++itr_name_find)
			{
				// 名前から一致するComponentを検索
				auto itr_component_find = components_map.find(*itr_name_find);
				// 早期リターン
				if (itr_component_find == itr_map_end)
					continue;
				// Componentの実体取得
				auto component_ptr = itr_component_find->second.lock();
				// class名の抜き出し
				size_t start_pos = itr_name_find->find_last_of(':');
				if (start_pos == std::string::npos)
				{
					start_pos = itr_name_find->find_first_of(' ');
				}
				// TreeNodeの表示開始
				constexpr auto flags = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth;
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
				// 表示部分
				ImGui::IDWrapped component_id(component_ptr.get());
				if (ImGui::TreeNodeEx(itr_name_find->c_str() + (start_pos + 1), flags))
				{
					// MonoBehaviour継承の場合はMonoBehaviour::OnGUIを先に呼び出す
					if (component_ptr->GetComponentType() == COMPONENT_TYPE::COMPONENT_SCRIPT)
					{
						std::static_pointer_cast<MonoBehaviour>(component_ptr)->MonoBehaviour::OnGUI();
					}
					component_ptr->OnGUI();
					ImGui::TreePop();
				}
			}
		}
		ImGui::End();
	}

	void ComponentManager::OnGUIAddComponent(GameObjectID goid)
	{
		ImGui::Begin("Inspector");
		{
			static bool window_activate = false;
			ImGui::IDWrapped object_id(static_cast<int>(goid));

			// Add Component用ボタン
			ImGui::Dummy(ImVec2(0.0f, 15.0f));
			ImGui::Separator();
			{
				const float indent_value = ImGui::GetContentRegionAvail().x * 0.15f;
				ImGui::IndentWrapped indent(indent_value);
				if (ImGui::Button("Add Component", ImVec2(ImGui::GetContentRegionAvail().x * 0.7f, ImGui::GetFontSize() * 1.5f)))
				{
					window_activate = true;
				}
			}

			// Add Component用ウィンドウ
			if (window_activate)
			{
				const ImVec2 window_size(300.0f, 500.0f);
				const ImVec2 mouse_pos = ImGui::GetMousePos();
				const ImVec2 window_pos = ImVec2(mouse_pos.x - window_size.x * 0.5f, mouse_pos.y - window_size.y * 0.1f);
				ImGui::SetNextWindowPos(window_pos, ImGuiCond_Appearing);
				ImGui::SetNextWindowSize(window_size, ImGuiCond_Appearing);
				const ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse;
				ImGui::Begin("Add Component Window", &window_activate, flags);
				{
					// ImGuiのフィルター機能
					static ImGuiTextFilter im_filter;
					im_filter.Draw("##component filter");

					// component mapを全てループさせる
					const auto itr_end = m_componentName_id_index.end();
					auto itr = m_componentName_id_index.begin();
					ImGuiTreeNodeFlags flags = 0;
					flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
					flags |= ImGuiTreeNodeFlags_Leaf;
					flags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;
					for (; itr != itr_end; ++itr)
					{
						// 表示する要素の準備
						const char* component_name = itr->first.c_str();

						// Transformは表示しない
						if (component_name == m_transform_class_name)
						{
							continue;
						}
						// フィルターを通過できるか
						if (im_filter.PassFilter(component_name))
						{
							ImGui::IDWrapped component_id(component_name);
							// 表示
							ImGui::TreeNodeEx(component_name, flags, component_name);
							// 選択されたらAddする
							if (ImGui::IsItemClicked(0))
							{
								m_add_functions.at(itr->second)(goid);
								window_activate = false;
							}
						}
					}
					// Add Componentウィンドウ以外でクリックしたら閉じる
					if (!ImGui::IsWindowAppearing() &&
						!ImGui::IsWindowHovered() &&
						ImGui::IsAnyMouseReleased()
						)
					{
						window_activate = false;
					}
				}
				ImGui::End();
			}
		}
		ImGui::End();
	}

	void ComponentManager::OnGUIRemoveComponent(GameObjectID goid)
	{
		ImGui::Begin("Inspector");
		{
			static bool window_activate = false;
			static std::string selecting_component_type_name;
			static bool assert_window_activate = false;

			// Remove Component用ボタン
			ImGui::Dummy(ImVec2(0.0f, 0.0f));
			ImGui::IDWrapped object_id(static_cast<int>(goid));
			{
				const float indent_value = ImGui::GetContentRegionAvail().x * 0.1f;
				ImGui::IndentWrapped indent(indent_value);
				if (ImGui::Button("Remove Component", ImVec2(ImGui::GetContentRegionAvail().x * 0.8f, ImGui::GetFontSize() * 1.5f)))
				{
					window_activate = true;
				}
			}
			// Remove Component用ウィンドウ
			if (window_activate)
			{
				const ImVec2 window_size(300.0f, 300.0f);
				const ImVec2 mouse_pos = ImGui::GetMousePos();
				const ImVec2 window_pos = ImVec2(mouse_pos.x - window_size.x * 0.5f, mouse_pos.y - window_size.y * 0.1f);
				ImGui::SetNextWindowPos(window_pos, ImGuiCond_Appearing);
				ImGui::SetNextWindowSize(window_size, ImGuiCond_Appearing);
				constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse;
				ImGui::Begin("Remove Component Window", &window_activate, flags);
				{
					// ImGuiのフィルター機能
					static ImGuiTextFilter im_filter;
					im_filter.Draw("##component filter");

					// owner_idからGameObjectの持つコンポーネントリストを取得
					const auto component_list = m_gameObject_components_typename_index.at(goid);
					const auto itr_end = component_list.end();
					auto itr = component_list.begin();
					ImGuiTreeNodeFlags flags = 0;
					flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
					flags |= ImGuiTreeNodeFlags_Leaf;
					flags |= ImGuiTreeNodeFlags_NoTreePushOnOpen;
					for (; itr != itr_end; ++itr)
					{
						// 表示する要素の準備
						const char* component_name = itr->first.c_str();
						// Transformは削除できない
						if (component_name == m_transform_type_name)
						{
							continue;
						}

						// フィルターを通過できるか
						if (im_filter.PassFilter(component_name))
						{
							ImGui::IDWrapped component_id(component_name);
							// 表示
							ImGui::TreeNodeEx(component_name, flags, component_name);
							// 選択されたら確認画面の表示に移る
							if (ImGui::IsItemClicked(0))
							{
								window_activate = false;
								selecting_component_type_name = component_name;
								assert_window_activate = true;
							}
						}
					}

					// Remove Componentウィンドウ以外で左クリックしたら閉じる
					if (!ImGui::IsWindowAppearing() &&
						!ImGui::IsWindowHovered() &&
						ImGui::IsAnyMouseReleased()
						)
					{
						window_activate = false;
					}
				}
				ImGui::End();
			}
			// Remove確認画面
			else if (assert_window_activate)
			{
				const ImVec2 window_size(400.0f, 120.0f);
				const ImVec2 mouse_pos = ImGui::GetMousePos();
				const ImVec2 window_pos = ImVec2(mouse_pos.x - window_size.x * 0.5f, mouse_pos.y - window_size.y * 0.4f);
				ImGui::SetNextWindowPos(window_pos, ImGuiCond_Appearing);
				ImGui::SetNextWindowSize(window_size, ImGuiCond_Appearing);
				constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
				ImGui::Begin("Remove Component confirmation Window", &assert_window_activate, flags);
				{

					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					static std::string draw_text = "Remove " + selecting_component_type_name;
					const float window_width = window_size.x;

					// 中央揃えで表示
					{
						const float string_width = ImGui::CalcTextSize(draw_text.c_str()).x;
						ImGui::SetCursorPosX((window_width - string_width) * 0.5f);
						ImGui::Text(draw_text.c_str());
						ImGui::Dummy(ImVec2(0.0f, 20.0f));
					}

					// ボタンの表示
					{
						ImVec2 button_size;
						button_size.x = window_size.x / 3.0f;
						button_size.y = window_size.y / 4.0f;
						const float button_space = (window_width * 0.5f - button_size.x) * 0.5f;
						ImGui::SetCursorPosX(button_space);
						// Removeを押したらgoidから検索してコンポーネントを消す
						if (ImGui::Button("Remove##RemoveComponent", button_size))
						{
							RemoveComponent(selecting_component_type_name, goid);
							assert_window_activate = false;
						}
						ImGui::SameLine(0.0f, button_space * 2.0f);
						// キャンセルボタンを押したらウィンドウを消す
						if (ImGui::Button("Cancel##RemoveComponent", button_size))
						{
							assert_window_activate = false;
						}
					}

					// Remove Componentウィンドウ以外で左クリックしたら閉じる
					if (!ImGui::IsWindowAppearing() &&
						!ImGui::IsWindowHovered() &&
						ImGui::IsAnyMouseReleased()
						)
					{
						assert_window_activate = false;
					}
				}
				ImGui::End();
			}
		}
		ImGui::End();
	}
#endif // USE_IMGUI

	std::shared_ptr<Component> ComponentManager::AddComponent(ComponentTypeID type_id, GameObjectID owner_id)
	{
		const auto itr_find = m_componentTypeID_typeName_index.find(type_id);
		if (itr_find == m_componentTypeID_typeName_index.end())
		{
			LOG_ASSERT("Can not find registered component ComponentManager::AddComponent()");
			return std::shared_ptr<Component>();
		}
		// AddComponent<T>を呼んでOnCreateまで済ませた参照をもらう
		return m_add_functions.at(type_id)(owner_id);
	}

	std::shared_ptr<Component> ComponentManager::AddComponent(const std::string& component_name, GameObjectID owner_id)
	{
		const auto itr_find = m_componentName_id_index.find(component_name);
		if (itr_find == m_componentName_id_index.end())
		{
			LOG_ASSERT("Can not find registered component ComponentManager::AddComponent()");
			return std::shared_ptr<Component>();
		}
		return AddComponent(itr_find->second, owner_id);
	}

	void ComponentManager::AddComponentOnSerialize(GameObjectID owner_id, std::shared_ptr<Component>& component)
	{
		m_mutex.lock();

		// コンポーネント名の取得
		const std::string type_name(typeid(*component.get()).name());
		auto&& go_components_name_index = m_gameObject_components_typename_index[owner_id];
		{
			// 同一コンポーネントがすでに存在するするなら登録しない
			const auto itr = go_components_name_index.find(type_name);
			if (itr != go_components_name_index.end())
			{
				return;
			}
		}
		// コンポーネントの管理情報をセット
		const auto itr = m_typeName_id_index.find(type_name);
		assert(itr != m_typeName_id_index.end());
		component->SetComponentTypeID(itr->second);
		component->SetOwnerIDAndTransform(owner_id);
		// マネージャにセット
		ObjectManager::Set(component);
		go_components_name_index.emplace(type_name, component);
		m_mutex.unlock();
		// 作成時の関数
		std::static_pointer_cast<Object>(component)->OnCreate(component);
	}

	void ComponentManager::RegisterComponentType()
	{
		auto&& registered_components = *Preprocess::RegisterModule::m_component_function_index;

		// 実行開始時に作成したコンポーネントリストがendまで繰り返す
		const auto&& itr_end = registered_components.end();
		for (auto itr = registered_components.begin(); itr != itr_end; ++itr)
		{
			ComponentTypeID id = std::hash<std::string>()(itr->first);
			// Register TypeName, ComponentName and AddFunction
			m_componentName_id_index.emplace(itr->first, id);
			m_typeName_id_index.emplace(itr->second.type_name, id);
			m_add_functions.emplace(id, itr->second.add_func);
			m_componentTypeID_typeName_index.emplace(id, itr->second.type_name);
		}
	}

	std::vector<std::weak_ptr<Component>> ComponentManager::GetComponents(GameObjectID owner_id)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		// GameObjectの所持するComponentリストを取得する
		auto&& component_index = m_gameObject_components_typename_index.at(owner_id);

		// 返す配列を宣言する
		const size_t index_size = component_index.size();
		std::vector<std::weak_ptr<Component>> component_vec(index_size);

		// Componentリストの先頭から順に配列に詰め込む
		auto index_itr = component_index.begin();
		for (size_t i = 0; i < index_size; ++i)
		{
			component_vec.at(i) = index_itr->second;
			++index_itr;
		}

		return component_vec;
	}

	void ComponentManager::GetComponents(GameObjectID owner_id, std::vector<std::weak_ptr<Component>>& vec)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		// GameObjectの所持するComponentリストを取得する
		auto&& component_index = m_gameObject_components_typename_index.at(owner_id);

		// 詰め込む配列を初期化
		const size_t index_size = component_index.size();
		vec.clear();
		vec.resize(index_size);

		// Componentリストの先頭から順に配列に詰め込む
		auto index_itr = component_index.begin();
		for (size_t i = 0; i < index_size; ++i)
		{
			vec.at(i) = index_itr->second;
			++index_itr;
		}
	}

	void ComponentManager::RemoveComponent(const std::string& component_type_name, GameObjectID owner_id)
	{
		// owner_idからGameObjectのもつコンポーネントリストを取得
		m_mutex.lock();
		auto&& component_index = m_gameObject_components_typename_index.at(owner_id);
		// TypeNameがTransformの時は消去しない
		if (component_type_name == m_transform_type_name)
		{
			m_mutex.unlock();
			LOG_ASSERT("Transform component can not remove!");
			return;
		}

		const auto itr = component_index.find(component_type_name);

		// アタッチされていなければ早期リターン
		if (itr == component_index.end())
		{
			m_mutex.unlock();
			assert(0 && "Invalid Component Type. This type is not attached this GameObject");
			return;
		}

		const std::shared_ptr<Component> s_ptr = itr->second.lock();
		if (!s_ptr)
		{
			component_index.erase(itr);
		}
		// Component名をOwnerから削除
		{
			auto& name_list = m_gameObject_attached_components_list.at(owner_id);
			const auto itr_name_find = std::find(name_list.begin(), name_list.end(), component_type_name);
			if (itr_name_find != name_list.end())
			{
				name_list.erase(itr_name_find);
			}
		}
		m_mutex.unlock();
		// DEBUG : RemoveComponentで消されるのはImmediateではない？
		ObjectManager::Destroy(s_ptr->GetInstanceID(), -1.0f);

		m_mutex.lock();
		component_index.erase(itr);
		m_mutex.unlock();
	}

	void ComponentManager::RemoveAllComponent(GameObjectID owner_id)
	{
		auto&& components = m_gameObject_components_typename_index.at(owner_id);
		const auto itr_end = components.end();
		for (auto itr = components.begin(); itr != itr_end;)
		{
			const auto s_ptr = itr->second.lock();
			if (!s_ptr)
			{
				itr = components.erase(itr);
				continue;
			}
			ObjectManager::Destroy(s_ptr->GetInstanceID(), -1.0f);
			itr = components.erase(itr);
		}
		// GameObjectIDで登録されたリストを消去する
		m_gameObject_components_typename_index.erase(owner_id);
		m_gameObject_attached_components_list.erase(owner_id);
	}

	std::list<std::string>& ComponentManager::GetComponentNameList(GameObjectID owner_id)
	{
		return m_gameObject_attached_components_list[owner_id];
	}

}//namespace TKGEngine 
