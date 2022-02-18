#include "../inc/FBXLoader.h"

#include "Systems/inc/LogSystem.h"
#include "Utility/inc/myfunc_vector.h"
#include "Utility/inc/myfunc_file.h"

#include "Application/Objects/Managers/GameObjectManager.h"
#include "Application/Objects/Components/inc/CMeshRenderer.h"
#include "Application/Objects/Components/inc/CSkinnedMeshRenderer.h"
#include "Application/Objects/Components/inc/CAnimator.h"

#include "Application/Resource/src/Mesh/IResMesh.h"
#include "Application/Resource/src/Avatar/IResAvatar.h"
#include "Application/Resource/src/Motion/IResMotion.h"

#include <filesystem>
#include <functional>
#include <cassert>

#ifdef USE_IMGUI
namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Internal linkage Global Class
	////////////////////////////////////////////////////////



	////////////////////////////////////////////////////////
	// Static member definition
	////////////////////////////////////////////////////////
	FBXLoader::LOAD_STATE FBXLoader::m_state = LOAD_STATE::INACTIVE;
	std::vector<FBXLoader::FBX_Data> FBXLoader::m_fbx_data;

	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	bool FBXLoader::LoadFromFile()
	{
		switch (m_state)
		{
			case LOAD_STATE::INACTIVE:
				m_state = LOAD_STATE::READY;
				break;
			case LOAD_STATE::READY:
				OnReady();
				break;
			case LOAD_STATE::BEGIN:
				OnBegin();
				break;
			case LOAD_STATE::SELECT:
				OnSelect();
				break;
			case LOAD_STATE::LOAD:
				OnLoad();
				break;
			case LOAD_STATE::END:
				OnEnd();
				break;
		}
		// ロード終了したらtrue
		if (m_state == LOAD_STATE::INACTIVE)
		{
			return true;
		}
		// ロード中ならfalse
		return false;
	}

	void FBXLoader::OnReady()
	{
		std::list<std::string> fbx_filename_list;
		if (!MyFunc::FetchOpenFileNames(fbx_filename_list, TEXT("fbx / obj(*.fbx,*.obj)\0*.fbx;*.obj\0"), TEXT("Load FBX File")))
		{
			m_state = LOAD_STATE::END;
		}
		else
		{
			const int file_num = static_cast<int>(fbx_filename_list.size());
			auto itr = fbx_filename_list.begin();
			const auto itr_end = fbx_filename_list.end();
			m_fbx_data.resize(file_num);
			for (int i = 0; i < file_num; ++i)
			{
				if (itr == itr_end)
				{
					m_state = LOAD_STATE::END;
					return;
				}
				m_fbx_data.at(i).fbx_filename = *itr;
				// ファイル名から不正な文字を取り除く
				std::string mesh_path = MyFunc::SplitFilePathNoExtension(*itr);
				while (true)
				{
					unsigned find = mesh_path.find_last_of(":;*?,<>\"| ");
					if (find == std::string::npos)
					{
						break;
					}
					else
					{
						if (find <= 0)
						{
							assert(0 && "invalid string.");
						}
						mesh_path = mesh_path.substr(0, find) + mesh_path.substr(find + 1);
					}
				}
				m_fbx_data.at(i).fbx_filedir = mesh_path;
				m_fbx_data.at(i).fbx_name = MyFunc::SplitFileName(*itr);
				++itr;
			}
			// Render ImGUI
			{
				const int data_size = static_cast<int>(m_fbx_data.size());
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0.2f, 0.f, 1));
				ImGui::SetNextWindowPos(ImVec2(150.0f, 120.0f), ImGuiCond_Always);
				ImGui::SetNextWindowSize(ImVec2(600.0f, 24.0f * data_size + 60.0f), ImGuiCond_Always);
				ImGui::Begin("Loading", NULL, ImGuiWindowFlags_NoTitleBar);
				ImGui::Text("Loading FBX Files");
				ImGui::NewLine();
				for (int i = 0; i < data_size; ++i)
				{
					ImGui::BulletText("%s", m_fbx_data.at(i).fbx_filename.c_str());
					ImGui::Spacing();
				}
				ImGui::End();
				ImGui::PopStyleColor();

			}
			m_state = LOAD_STATE::BEGIN;
		}
	}

	void FBXLoader::OnBegin()
	{
		for (auto&& data : m_fbx_data)
		{
			auto& manager = data.fbx_manager;
			manager = FbxManager::Create();

			// FBXに対する入出力を定義
			FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
			manager->SetIOSettings(ios);

			// インポータを生成
			FbxImporter* importer = FbxImporter::Create(manager, "importer");
			bool result = importer->Initialize(data.fbx_filename.c_str(), -1, manager->GetIOSettings());
			if (!result)
			{
				LOG_ASSERT("Failed Load FBX File.(%s)\nFbxImporter::Initialize()\n(%s)",
					data.fbx_filename.c_str(), importer->GetStatus().GetErrorString());
				manager->Destroy();
				m_state = LOAD_STATE::END;
				return;
			}

			// SceneオブジェクトにFBXファイル内のデータを流し込む
			data.fbx_scene = FbxScene::Create(manager, "scene");
			result = importer->Import(data.fbx_scene);
			if (!result)
			{
				LOG_ASSERT("Failed Load FBX File.(%s)\nFbxImporter::Initialize()\n(%s)",
					data.fbx_filename.c_str(), importer->GetStatus().GetErrorString());
				importer->Destroy();
				manager->Destroy();
				m_state = LOAD_STATE::END;
				return;
			}
			importer->Destroy();	// シーンを流し込んだらImporterは解放してよい

			// ジオメトリを三角化しておく
			// TODO : 法線などがないモデルはあらかじめFBXSceneで計算するようにしたい
			FbxGeometryConverter geometry_converter(manager);
			geometry_converter.Triangulate(data.fbx_scene, true, true);
			geometry_converter.RemoveBadPolygonsFromMeshes(data.fbx_scene);
			FbxAxisSystem axis_system_ref = data.fbx_scene->GetGlobalSettings().GetAxisSystem();
			FbxAxisSystem::ECoordSystem coord_system = axis_system_ref.GetCoorSystem();
			//if (coord_system == FbxAxisSystem::eRightHanded)
			//	data.is_RH = true;

			// 座標単位を取得する
			float system_unit = 1.0f;
			FbxSystemUnit unit = data.fbx_scene->GetGlobalSettings().GetSystemUnit();
			if (unit == FbxSystemUnit::cm)
			{
				//system_unit = 0.01f;
			}

			//// DirectX座標系に変換する
			//MATRIX system_axis = MATRIX::Identity;
			FbxAxisSystem scene_axis_system = data.fbx_scene->GetGlobalSettings().GetAxisSystem();
			std::vector<std::string> names;
			// Node探索
			FbxNode* root_node = data.fbx_scene->GetRootNode();
			{
				std::function<void(FbxNode*)> traverse = [&](FbxNode* node) {
					if (!node)
						return;
					FbxNodeAttribute* node_attribute = node->GetNodeAttribute();
					// デバッグ用ノード名取得
					auto name = node->GetName();
					if (node_attribute)
					{
						const auto attribute_type = node_attribute->GetAttributeType();
						switch (attribute_type)
						{
							case FbxNodeAttribute::eMesh:
								if (static_cast<FbxMesh*>(node_attribute)->GetPolygonCount() > 0)
								{
									data.mesh_names.emplace_back(node->GetName());
									data.mesh_selection.emplace(node->GetName(), FBX_Data::MeshData(node, true, false));
								}
								break;
							case FbxNodeAttribute::eSkeleton:
								data.has_avatar = true;
								names.emplace_back(node->GetName());
								break;
						}
					}
					const int child_cnt = node->GetChildCount();
					for (int i = 0; i < child_cnt; ++i)
					{
						traverse(node->GetChild(i));
					}
				};
				traverse(root_node);
			}
			// Motion探索
			{
				FbxArray<FbxString*> motion_names;
				data.fbx_scene->FillAnimStackNameArray(motion_names);

				const int motion_cnt = motion_names.Size();
				for (int motion_idx = 0; motion_idx < motion_cnt; ++motion_idx)
				{
					data.motion_selection.emplace(motion_names[motion_idx]->Buffer(), true);
				}

				for (int i = 0; i < motion_cnt; ++i)
				{
					delete motion_names[i];
				}
			}
		}
		m_state = LOAD_STATE::SELECT;
	}

	void FBXLoader::OnSelect()
	{
		ImGui::SetNextWindowPos(ImVec2(200.0f, 100.0f), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(480.0f, 600.0f), ImGuiCond_Always);
		ImGui::Begin("Select FBX Data to Export.", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		{
			ImGui::BeginChild("Data name", ImVec2(460.0f, 520.0f), true);
			{
				// TODO : Collision Mesh用に選択できる用にする

				for (auto&& data : m_fbx_data)
				{
					ImGui::SetNextItemOpen(true, ImGuiCond_Once);
					ImGui::PushID(data.fbx_name.c_str());
					if (ImGui::CollapsingHeader(data.fbx_name.c_str()))
					{
						ImGui::Indent(20.0f);
						// Mesh
						{
							if (!data.mesh_selection.empty())
							{
								ImGui::Checkbox("##Select mesh all", &data.load_mesh);
								ImGui::SameLine();
								ImGui::SetNextItemOpen(true, ImGuiCond_Once);
								if (ImGui::CollapsingHeader("Mesh"))
								{
									ImGui::Indent(20.0f);
									for (auto&& mesh : data.mesh_selection)
									{
										ImGui::Checkbox(mesh.first.c_str(), &mesh.second.is_selecting);
									}
									ImGui::Unindent(20.0f);
								}
							}
						}
						// Avatar
						{
							if (data.has_avatar)
							{
								ImGui::SetNextItemOpen(true, ImGuiCond_Once);
								if (ImGui::CollapsingHeader("Avatar"))
								{
									ImGui::Indent(20.0f);
									ImGui::Checkbox("Avatar Data", &data.load_avatar);
									ImGui::Unindent(20.0f);
								}
							}
						}
						// Motion
						{
							if (!data.motion_selection.empty())
							{
								ImGui::Checkbox("##Select motion all", &data.load_motion);
								ImGui::SameLine();
								ImGui::SetNextItemOpen(true, ImGuiCond_Once);
								if (ImGui::CollapsingHeader("Motion"))
								{
									ImGui::Indent(20.0f);
									for (auto&& motion : data.motion_selection)
									{
										ImGui::Checkbox(motion.first.c_str(), &motion.second);
									}
									ImGui::Unindent(20.0f);
								}
							}
						}
						ImGui::Unindent(20.0f);
					}
					ImGui::PopID();
				}
			}
			ImGui::EndChild();

			ImGui::Separator();
			if (ImGui::Button("Export"))
			{
				m_state = LOAD_STATE::LOAD;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				m_state = LOAD_STATE::END;
			}
		}
		ImGui::End();
	}

	void FBXLoader::OnLoad()
	{
		for (auto&& data : m_fbx_data)
		{
			// FBXファイルからロードする要素がない場合は飛ばす
			bool is_skip = true;
			// .objectの書き出しを実行するか
			bool do_save_object_file = true;

			// Mesh
			if (data.load_mesh)
			{
				for (auto&& mesh_data : data.mesh_selection)
				{
					if (mesh_data.second.is_selecting)
					{
						is_skip = false;
						break;
					}
					do_save_object_file = false;
				}
			}
			else
			{
				do_save_object_file = false;
			}
			// Motion
			if (is_skip)
			{
				for (auto&& motion_data : data.motion_selection)
				{
					if (motion_data.second)
					{
						is_skip = false;
						break;
					}
				}
			}
			// Avatar
			if (is_skip)
			{
				if (data.load_avatar)
				{
					is_skip = false;
				}
				else
				{
					do_save_object_file = false;
				}
			}
			// 書き出すファイルが存在しないならスキップする
			if (is_skip)
				continue;

			// FBXのファイル名からフォルダを作成する
			std::filesystem::path p = data.fbx_filedir;
			std::filesystem::create_directory(p);

			// Avatar探索
			std::vector<FbxMesh*> fbx_meshes;
			FbxNode* root_node = data.fbx_scene->GetRootNode();
			{
				std::function<void(FbxNode*)> traverse = [&](FbxNode* node) {
					if (!node)
						return;
					FbxNodeAttribute* node_attribute = node->GetNodeAttribute();
					if (node_attribute)
					{
						switch (node_attribute->GetAttributeType())
						{
							case FbxNodeAttribute::eMesh:
								fbx_meshes.emplace_back(static_cast<FbxMesh*>(node_attribute));
								break;
						}
					}
					const int child_cnt = node->GetChildCount();
					for (int i = 0; i < child_cnt; ++i)
					{
						traverse(node->GetChild(i));
					}
				};
				traverse(root_node);
			}
			std::unordered_map<std::string, int> bone_name_index;
			// Avatarをファイル書き出しするかに関係なくAvatar作成は行う、load_avatarによってファイル書き出しするか決める
			if (data.has_avatar)
			{
				IResAvatar::CreateBinaryFromFBX(data.fbx_filedir + '/' + data.fbx_name + AVATAR_EXTENSION, data.fbx_scene, fbx_meshes, bone_name_index, data.load_avatar);
			}

			// Mesh探索
			int save_mesh_num = 0;
			if (data.load_mesh)
			{
				std::function<void(FbxNode*)> traverse = [&](FbxNode* node) {
					if (!node)
						return;
					FbxNodeAttribute* node_attribute = node->GetNodeAttribute();
					if (node_attribute)
					{
						switch (node_attribute->GetAttributeType())
						{
							case FbxNodeAttribute::eMesh:
								auto&& itr = data.mesh_selection.find(node->GetName());
								if (itr != data.mesh_selection.end())
								{
									if (itr->second.is_selecting)
									{
										IResMesh::CreateBinaryFromFBX(
											data.fbx_filedir + '/' + itr->first + MESH_EXTENSION,
											data.fbx_scene,
											node,
											static_cast<FbxMesh*>(node_attribute),
											bone_name_index,
											itr->second.is_skinned
										);
										++save_mesh_num;
									}
									else
									{
										// メッシュを全て書き出さない時は.objectを生成しない
										do_save_object_file = false;
									}
								}
								break;
						}
					}
					const int child_cnt = node->GetChildCount();
					for (int i = 0; i < child_cnt; ++i)
					{
						traverse(node->GetChild(i));
					}
				};
				traverse(root_node);
			}

			// Motion
			if (data.load_motion)
			{
				int load_motion_cnt = 0;
				for (auto&& motion : data.motion_selection)
				{
					if (motion.second)
					{
						++load_motion_cnt;
					}
				}

				std::vector<std::string> motion_filepathes;
				std::vector<std::string> motion_names;
				motion_filepathes.reserve(load_motion_cnt);
				motion_names.reserve(load_motion_cnt);
				for (auto&& motion : data.motion_selection)
				{
					if (motion.second)
					{
						motion_filepathes.emplace_back(data.fbx_filedir + '/' + motion.first + MOTION_EXTENSION);
						motion_names.emplace_back(motion.first);
					}
				}
				IResMotion::CreateBinaryFromFBX(motion_filepathes, data.fbx_scene, motion_names, bone_name_index);
			}

			// .object生成
			if (do_save_object_file && save_mesh_num > 0)
			{
				// RootのGameObject作成
				auto gameobject = GameObjectManager::CreateGameObject(data.fbx_name.c_str());
				// RendererコンポーネントのアタッチとAvatarをつける
				// 書き出したMesh数が1つならRootオブジェクトにコンポーネントをつける
				if (save_mesh_num == 1)
				{
					// .meshファイルパス
					std::string mesh_filepath = data.fbx_filedir + '/';
					auto itr_mesh = data.mesh_selection.begin();
					if (itr_mesh == data.mesh_selection.end())
					{
						assert(0 && "failed process. When save_mesh_num == 1, mesh_selection size is over 1.");
						return;
					}
					mesh_filepath += itr_mesh->first + MESH_EXTENSION;
					// Avatarをもつ
					if (data.has_avatar)
					{
						// Animator
						auto animator = gameobject->AddComponent<Animator>();
						animator->SetAvatar(data.fbx_filedir + '/' + data.fbx_name + AVATAR_EXTENSION);
						// SkinnedMeshRenderer
						auto renderer = gameobject->AddComponent<SkinnedMeshRenderer>();
						renderer->AddMesh(mesh_filepath);
						renderer->AddAnimator(animator);
					}
					// Avatarがない
					else
					{
						// MeshRenderer
						auto renderer = gameobject->AddComponent<MeshRenderer>();
						renderer->AddMesh(mesh_filepath);
					}
				}
				// 書き出したMesh数が複数の場合は子オブジェクトを作成する
				else if (save_mesh_num > 1)
				{
					// AvatarをもつならRootにAnimatorをアタッチする
					std::shared_ptr<Animator> animator;
					if (data.has_avatar)
					{
						// Animator
						animator = gameobject->AddComponent<Animator>();
						animator->SetAvatar(data.fbx_filedir + '/' + data.fbx_name + AVATAR_EXTENSION);
					}

					// Mesh数分の子オブジェクトを作成してRendererをアタッチする
					std::string mesh_filepath;
					for (auto& mesh_name : data.mesh_names)
					{
						auto& mesh_index = data.mesh_selection.at(mesh_name);

						// .meshファイルパス
						mesh_filepath = data.fbx_filedir + '/';
						mesh_filepath += mesh_name + MESH_EXTENSION;
						// 対応するRendererのアタッチ
						if (data.has_avatar)
						{
							// Meshがボーンの影響を受けるものはSkinnedMeshRenderer
							if (mesh_index.is_skinned)
							{
								// Childオブジェクトを作成
								auto child = GameObjectManager::CreateChild(gameobject, mesh_name.c_str());
								// SkinnedMeshRenderer
								auto renderer = child->AddComponent<SkinnedMeshRenderer>();
								renderer->AddMesh(mesh_filepath);
								renderer->AddAnimator(animator);
							}
							// Meshがボーンの影響を受けないものは親ノードを探索してChildオブジェクトを作成
							else
							{
								std::string parent_name = mesh_index.mesh_node->GetParent()->GetName();
								std::function<bool(const std::shared_ptr<IGameObject>&)> recurse = [&](const std::shared_ptr<IGameObject>& parent)
								{
									const auto transform = parent->GetTransform();
									const int child_num = transform->GetChildCount();
									for (int i = 0; i < child_num; ++i)
									{
										const auto child_transform = transform->GetChild(i);
										const auto child = child_transform->GetGameObject();
										if (child != nullptr)
										{
											// 名前が一致したら子に設定する
											if (child->GetName() == parent_name)
											{
												// 一致したらChildオブジェクトを作成
												const auto new_go = GameObjectManager::CreateGameObject(mesh_name.c_str());
												new_go->GetTransform()->SetParent(child_transform, false);
												const auto renderer = new_go->AddComponent<MeshRenderer>();
												renderer->AddMesh(mesh_filepath);
												return true;
											}
											// 探索の再帰する、すでに発見していたら再帰を抜けていく
											if (recurse(child))
											{
												return true;
											}
										}
									}
									return false;
								};
								// 再帰的に名前を探索して発見したら子オブジェクトを作成
								recurse(gameobject);
							}
						}
						else
						{
							// Childオブジェクトを作成
							auto child = GameObjectManager::CreateChild(gameobject, mesh_name.c_str());
							// MeshRenderer
							auto renderer = child->AddComponent<MeshRenderer>();
							renderer->AddMesh(mesh_filepath);
						}
					}
				}

				// 座標単位を取得する
				FbxSystemUnit unit = data.fbx_scene->GetGlobalSettings().GetSystemUnit();
				if (unit == FbxSystemUnit::cm)
				{
					gameobject->GetTransform()->LocalScale(0.01f, 0.01f, 0.01f);
				}

				// File書き出し
				GameObjectManager::SaveGameObject(gameobject->GetGameObjectID(), data.fbx_filedir + '/' + data.fbx_name + GAMEOBJECT_EXTENSION);

				// GameObject破棄
				gameobject->Destroy();
			}
		}// ~for

		m_state = LOAD_STATE::END;
	}

	void FBXLoader::OnEnd()
	{
		for (auto&& data : m_fbx_data)
		{
			data.fbx_manager->Destroy();
		}
		m_fbx_data.clear();
		m_fbx_data.shrink_to_fit();

		m_state = LOAD_STATE::INACTIVE;
	}

}// namespace TKGEngine::FBXLoader
#endif// #ifdef USE_IMGUI