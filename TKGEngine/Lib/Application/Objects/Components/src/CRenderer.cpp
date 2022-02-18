
#include "../inc/CRenderer.h"

#include "../interface/ICamera.h"
#include "../inc/CTransform.h"
#include "../../../Objects/inc/IGameObject.h"
#include "../../Managers/RendererManager.h"
#include "../../Managers/SceneManager.h"


namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Static variable declaration
	////////////////////////////////////////////////////////
	std::mutex Renderer::m_mutex;
	bool Renderer::m_do_init = false;
	Mesh Renderer::m_quad;


	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	Renderer::Renderer()
		: Component(COMPONENT_TYPE::COMPONENT_GENERAL)
	{
		/* nothing */
	}

	Renderer::Renderer(RendererType type)
		: Component(COMPONENT_TYPE::COMPONENT_GENERAL), m_renderer_type(type)
	{
		/* nothing */
	}

	Renderer::~Renderer()
	{
		/* nothing */
	}

#ifdef USE_IMGUI
	void Renderer::OnGUI()
	{
		// 派生先でPushIDして使用

		// Enable
		ImGui::Checkbox("Enable", &m_is_enabled);
		ImGui::Separator();
		// Shadow Casting Mode(UIとパーティクルはシャドウ非対応)
		if (m_renderer_type != RendererType::UI && m_renderer_type != RendererType::Particle)
		{
			ImGui::Text("Shadow Mode");
			ImGui::AlignedSameLine(0.5f);
			ImGui::ComboEnum<ShadowCastingMode, ShadowCastingMode::MAX_ShadowCastingMode>("##Test Enum", &m_shadow_casting_mode);
		}
		// Color
		ImGui::Text("Instance Color");
		ImGui::ColorEdit4("##Renderer Instance Color", &m_instance_color.x, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf);
	}
	void Renderer::OnGUIMesh()
	{
		constexpr ImGuiTreeNodeFlags tree_flags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
		if (ImGui::TreeNodeEx("Mesh", tree_flags))
		{
			// Mesh用タブ表示
			if (ImGui::BeginTabBar("Mesh Tab Bar"))
			{
				if (m_shadow_casting_mode != ShadowCastingMode::ShadowsOnly)
				{
					// Main Mesh
					ImGui::IDWrapped main_mesh_id("Main Mesh");
					if (ImGui::BeginTabItem("Main Mesh"))
					{
						// ファイルパス
						{
							ImGui::Text("File");
							ImGui::Text("\"%s\"", m_mesh.GetFilePath());
						}
						// Loadボタン
						if (ImGui::Button("Load##MainMesh"))
						{
							// ファイルパス取得
							std::string filepath;
							if (MyFunc::FetchOpenFileName(filepath, TEXT("Mesh(*.mesh)\0*.mesh\0"), TEXT("Load Mesh"), TEXT(".\\Asset\\Models")))
							{
								// ファイルオープン
								const std::ifstream ifs(filepath.c_str(), std::ios::in | std::ios::binary);
								if (ifs.is_open())
								{
									AddMesh(filepath, true);
								}
							}
						}
						ImGui::EndTabItem();
					}
				}
				if (m_shadow_casting_mode != ShadowCastingMode::OFF)
				{
					// Shadow Mesh
					ImGui::IDWrapped shadow_mesh_id("Shadow Mesh");
					if (ImGui::BeginTabItem("Shadow Mesh"))
					{
						// ファイルパス
						{
							ImGui::Text("File");
							ImGui::Text("\"%s\"", m_shadow_mesh.GetFilePath());
						}
						// Loadボタン
						if (ImGui::Button("Load##ShadowMesh"))
						{
							// ファイルパス取得
							std::string filepath;
							if (MyFunc::FetchOpenFileName(filepath, TEXT("Mesh(*.mesh)\0*.mesh\0"), TEXT("Load Mesh"), TEXT(".\\Asset\\Models")))
							{
								// ファイルオープン
								const std::ifstream ifs(filepath.c_str(), std::ios::in | std::ios::binary);
								if (ifs.is_open())
								{
									AddShadowMesh(filepath, false);
								}
							}
						}
						ImGui::EndTabItem();
					}
				}
				ImGui::EndTabBar();
			}
			ImGui::TreePop();
		}
		ImGui::Separator();
	}
	void Renderer::OnGUIMaterial()
	{
		constexpr ImGuiTreeNodeFlags tree_flags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
		if (ImGui::TreeNodeEx("Materials", tree_flags))
		{
			// Material用タブ表示
			if (ImGui::BeginTabBar("Material Tab Bar"))
			{
				// Main Materials
				if (m_shadow_casting_mode != ShadowCastingMode::ShadowsOnly)
				{
					ImGui::IDWrapped main_material_id("Main Materials");
					if (ImGui::BeginTabItem("Main Materials"))
					{
						if (ImGui::BeginTabBar("Main Material Tab Bar"))
						{
							const size_t mat_num = m_materials.size();
							for (size_t i = 0; i < mat_num; ++i)
							{
								ImGui::IDWrapped material_id(&m_materials.at(i));
								std::string tab_item_label = "No." + std::to_string(i) + "##Main Material";
								if (ImGui::BeginTabItem(tab_item_label.c_str()))
								{
									// Create Load
									{
										// Createボタン
										if (ImGui::Button("Create##Material"))
											OnButtonCreateMaterial(static_cast<int>(i));
										ImGui::SameLine();
										// Loadボタン
										if (ImGui::Button("Load##Material"))
											OnButtonLoadMaterial(static_cast<int>(i));
									}
									// マテリアルを持っていない
									if (!m_materials.at(i).HasMaterial())
									{
										ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "No Material!");
									}
									else
									{
										// マテリアルが存在する
										ImGui::IndentWrapped indent(ImGui::INDENT_VALUE * 0.5f);
										if (ImGui::TreeNodeEx(m_materials.at(i).GetName(), tree_flags))
										{
											// マテリアルごとのGUI
											if (m_materials.at(i).OnGUI(false))
											{
												// セーブされたらファイルパスを上書きする
												m_material_filedata_list.at(i).Set(m_materials.at(i).GetFilePath());
											}
											ImGui::TreePop();
										}
									}
									ImGui::EndTabItem();
								}
							}
							ImGui::EndTabBar();
						}
						ImGui::EndTabItem();
					}
				}
				// Shadow Material
				if (m_shadow_casting_mode != ShadowCastingMode::OFF)
				{
					// Shadow Materials
					ImGui::IDWrapped shadow_material_id("Shadow Materials");
					if (ImGui::BeginTabItem("Shadow Materials"))
					{
						if (ImGui::BeginTabBar("Shadow Material Tab Bar"))
						{
							const size_t mat_num = m_shadow_materials.size();
							for (size_t i = 0; i < mat_num; ++i)
							{
								ImGui::IDWrapped material_id(&m_shadow_materials.at(i));
								std::string tab_item_label = "No." + std::to_string(i) + "##Shadow Material";
								if (ImGui::BeginTabItem(tab_item_label.c_str()))
								{
									// Create Load
									{
										// Createボタン
										if (ImGui::Button("Create##Shadow Material"))
											OnButtonCreateShadowMaterial(static_cast<int>(i));
										ImGui::SameLine();
										// Loadボタン
										if (ImGui::Button("Load##Shadow Material"))
											OnButtonLoadShadowMaterial(static_cast<int>(i));
									}
									// マテリアルを持っていない
									if (!m_shadow_materials.at(i).HasMaterial())
									{
										ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "No Material!");
									}
									else
									{
										// マテリアルが存在する
										ImGui::IndentWrapped indent(ImGui::INDENT_VALUE * 0.5f);
										if (ImGui::TreeNodeEx(m_shadow_materials.at(i).GetName(), tree_flags))
										{
											// マテリアルごとのGUI
											if (m_shadow_materials.at(i).OnGUI(true))
											{
												// セーブされたらファイルパスを上書きする
												m_shadow_material_filedata_list.at(i).Set(m_shadow_materials.at(i).GetFilePath());
											}
											ImGui::TreePop();
										}
									}
									ImGui::EndTabItem();
								}
							}
							ImGui::EndTabBar();
						}
						ImGui::EndTabItem();
					}
				}
				ImGui::EndTabBar();
			}
			ImGui::TreePop();
		}
		ImGui::Separator();
	}
	void Renderer::OnButtonCreateMaterial(const int mat_index)
	{
		m_materials.at(mat_index).Create();
	}
	void Renderer::OnButtonLoadMaterial(const int mat_index)
	{
		// ロードするマテリアルのファイル名を取得
		std::string filepath;
		if (MyFunc::FetchOpenFileName(filepath, TEXT("Material(*.material)\0*.material\0"), TEXT("Load Material"), TEXT(".\\Asset\\Materials")))
		{
			AddMaterial(mat_index, filepath);
		}
	}
	void Renderer::OnButtonCreateShadowMaterial(const int mat_index)
	{
		m_shadow_materials.at(mat_index).Create();
	}
	void Renderer::OnButtonLoadShadowMaterial(const int mat_index)
	{
		// ロードするマテリアルのファイル名を取得
		std::string filepath;
		if (MyFunc::FetchOpenFileName(filepath, TEXT("Material(*.material)\0*.material\0"), TEXT("Load Material"), TEXT(".\\Asset\\Materials")))
		{
			AddShadowMaterial(mat_index, filepath);
		}
	}
#endif // USE_IMGUI

	void Renderer::OnCreate(const std::shared_ptr<Object>& p_self)
	{
		// Create Quad.
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (!m_do_init)
			{
				CreateQuad();
				m_do_init = true;
			}
		}
		// マネージャに登録する
		const std::shared_ptr<Renderer> renderer = std::static_pointer_cast<Renderer>(p_self);
		m_manager_iterator =
			RendererManager::RegisterManager(renderer, m_renderer_type == RendererType::UI ? true : false);
		// 派生先ごとの処理
		OnCreated();

		// デシリアライズ時にMeshとMaterialを持っていたらロードする
		// Main
		{
			// Mesh
			if (m_mesh_filedata.HasData())
			{
				AddMesh(m_mesh_filedata.filepath, false);
			}
			// Shadow
			int mat_index = 0;
			for (auto& mat_data : m_material_filedata_list)
			{
				if (mat_data.HasData())
				{
					AddMaterial(mat_index, mat_data.filepath);
				}
				else
				{
					AddMaterial(mat_index, Material::GetDefaultMaterial());
				}
				++mat_index;
			}
		}
		// Shadow
		{
			// Mesh
			if (m_shadow_mesh_filedata.HasData())
			{
				AddShadowMesh(m_shadow_mesh_filedata.filepath, false);
			}
			// Shadow
			int mat_index = 0;
			for (auto& mat_data : m_shadow_material_filedata_list)
			{
				if (mat_data.HasData())
				{
					AddShadowMaterial(mat_index, mat_data.filepath);
				}
				++mat_index;
			}
		}
	}

	void Renderer::OnDestroy()
	{
		OnDestroyed();
		RendererManager::UnregisterManager(m_manager_iterator, m_renderer_type == RendererType::UI ? true : false);
	}

	Material* Renderer::GetMaterial(const int index)
	{
		if (index >= static_cast<int>(m_materials.size()))
		{
			return nullptr;
		}
		return &(m_materials.at(index));
	}

	void Renderer::AddMaterial(const int index, const std::string& filepath)
	{
		if (index >= static_cast<int>(m_materials.size()))
			return;

		m_materials.at(index).Load(filepath);
		// リソース状態チェック
		if (m_materials.at(index).HasMaterial())
		{
			m_material_filedata_list.at(index).Set(filepath);
		}
		else
		{
			m_material_filedata_list.at(index).Clear();
		}
	}

	void Renderer::AddMaterial(const int index, const Material& material)
	{
		if (index >= static_cast<int>(m_materials.size()))
			return;

		m_materials.at(index) = material;
	}

	Mesh* Renderer::GetMesh()
	{
		return &m_mesh;
	}

	void Renderer::AddMesh(const std::string& filepath, const bool set_global)
	{
		m_mesh.Load(filepath);
		// リソース状態チェック
		if (m_mesh.HasMesh())
		{
			m_mesh_filedata.Set(filepath);
		}
		else
		{
			m_mesh_filedata.Clear();
			return;
		}
		// マテリアル数を変更する
		ResizeMaterial(m_mesh.GetSubsetCount());
		// メッシュの持つトランスフォームを適用するか
		if (set_global)
		{
			SetGlobalTransform(m_mesh);
		}

		// Shadow用のメッシュがなければMain用メッシュのSubset数でResizeする
		if (!m_shadow_mesh.HasMesh())
		{
			ResizeShadowMaterial(m_mesh.GetSubsetCount());
		}
	}

	void Renderer::AddMesh(const Mesh& mesh, const bool set_global)
	{
		m_mesh = mesh;
		const int subset_cnt = m_mesh.GetSubsetCount();
		// マテリアル数を変更する
		ResizeMaterial(subset_cnt > 0 ? subset_cnt : 1);
		// メッシュの持つトランスフォームを適用するか
		if (set_global)
		{
			SetGlobalTransform(m_mesh);
		}

		// Shadow用のメッシュがなければMain用メッシュのSubset数でResizeする
		if (!m_shadow_mesh.HasMesh())
		{
			ResizeShadowMaterial(subset_cnt > 0 ? subset_cnt : 1);
		}
	}

	Material* Renderer::GetShadowMaterial(const int index)
	{
		if (index >= static_cast<int>(m_shadow_materials.size()))
		{
			return nullptr;
		}
		return &(m_shadow_materials.at(index));
	}

	void Renderer::AddShadowMaterial(const int index, const std::string& filepath)
	{
		if (index >= static_cast<int>(m_shadow_materials.size()))
			return;

		m_shadow_materials.at(index).Load(filepath);
		// リソース状態チェック
		if (m_shadow_materials.at(index).HasMaterial())
		{
			m_shadow_material_filedata_list.at(index).Set(filepath);
		}
		else
		{
			m_shadow_material_filedata_list.at(index).Clear();
		}
	}

	void Renderer::AddShadowMaterial(const int index, const Material& material)
	{
		if (index >= static_cast<int>(m_shadow_materials.size()))
			return;

		m_shadow_materials.at(index) = material;
	}

	Mesh* Renderer::GetShadowMesh()
	{
		return &m_shadow_mesh;
	}

	void Renderer::AddShadowMesh(const std::string& filepath, const bool set_global)
	{
		m_shadow_mesh.Load(filepath);
		// リソース状態チェック
		if (m_shadow_mesh.HasMesh())
		{
			m_shadow_mesh_filedata.Set(filepath);
		}
		else
		{
			m_shadow_mesh_filedata.Clear();
			return;
		}
		// マテリアル数を変更する
		ResizeShadowMaterial(m_shadow_mesh.GetSubsetCount());
		// メッシュの持つトランスフォームを適用するか
		if (set_global)
		{
			SetGlobalTransform(m_shadow_mesh);
		}
	}

	void Renderer::AddShadowMesh(const Mesh& mesh, const bool set_global)
	{
		m_shadow_mesh = mesh;
		const int subset_cnt = m_shadow_mesh.GetSubsetCount();
		// マテリアル数を変更する
		ResizeShadowMaterial(subset_cnt > 0 ? subset_cnt : 1);
		// メッシュの持つトランスフォームを適用するか
		if (set_global)
		{
			SetGlobalTransform(m_shadow_mesh);
		}
	}

	void Renderer::ResizeMaterial(const int size)
	{
		if (size < 1)
			return;

		int current_size = static_cast<int>(m_materials.size());

		if (current_size < size)
		{
			m_materials.resize(size);
			m_material_filedata_list.resize(size);
		}
		else if (current_size > size)
		{
			while (current_size > size)
			{
				m_materials.pop_back();
				m_material_filedata_list.pop_back();
				--current_size;
			}
		}
	}

	void Renderer::ResizeShadowMaterial(const int size)
	{
		if (size < 1)
			return;

		int current_size = static_cast<int>(m_shadow_materials.size());

		if (current_size < size)
		{
			m_shadow_materials.resize(size);
			m_shadow_material_filedata_list.resize(size);
		}
		else if (current_size > size)
		{
			while (current_size > size)
			{
				m_shadow_materials.pop_back();
				m_shadow_material_filedata_list.pop_back();
				--current_size;
			}
		}
	}

	void Renderer::RenderShadow(ID3D11DeviceContext* p_context, int index, int start_index, int instance_count, VertexBuffer& instance_buffer)
	{
		// 影を描画するものはオーバーロードして実装するのでここには来ない
		assert(0);
	}

	void Renderer::SetInstance(MainInstance* p_instance)
	{
		p_instance->transform_matrix = m_current_transform_matrix;
		p_instance->instance_color = m_instance_color;
	}

	void Renderer::SetInstance(UIInstance* p_instance)
	{
		// UIRendererのものが呼ばれるのでここには来ない
		assert(0);
	}

	int Renderer::GetSubsetCount() const
	{
		return m_mesh.GetSubsetCount();
	}

	unsigned Renderer::GetHashMesh() const
	{
		return m_mesh.GetHash();
	}

	unsigned Renderer::GetHashMaterial(const int index) const
	{
		return m_materials.at(index).GetHash();
	}

	int Renderer::GetRenderQueue(const int index) const
	{
		return m_materials.at(index).GetRenderQueue();
	}

	int Renderer::GetShadowSubsetCount() const
	{
		if (m_shadow_mesh.HasMesh())
			return m_shadow_mesh.GetSubsetCount();
		return GetSubsetCount();
	}

	unsigned Renderer::GetShadowHashMesh() const
	{
		if (m_shadow_mesh.HasMesh())
			return m_shadow_mesh.GetHash();
		return GetHashMesh();
	}

	unsigned Renderer::GetShadowHashMaterial(const int index) const
	{
		return m_shadow_materials.at(index).GetHash();
	}

	int Renderer::GetShadowRenderQueue(const int index) const
	{
		return m_shadow_materials.at(index).GetRenderQueue();
	}

	bool Renderer::IsActiveAndEnabled()
	{
		const auto s_go = GetGameObject();
		if (!s_go)
		{
			LOG_ASSERT("invalid shared pointer. Renderer::IsActiveAndEnabled()");
			return false;
		}

		return m_is_enabled && s_go->GetActiveHierarchy() && SceneManager::GetActive(s_go->GetScene());
	}

	void Renderer::CalculateRenderParameter(const std::shared_ptr<ICamera>& camera)
	{
		// Transformを参照を取得
		const auto s_transform = GetTransform();

		// WorldPosition, WorldMatrixの取得
		m_current_world_position = s_transform->Position();
		m_current_transform_matrix = s_transform->GetLocalToWorldMatrix();

		// Boundsの更新
		m_bounds = m_mesh.GetBounds()->Transform(m_current_transform_matrix);
	}

	Layer Renderer::GetLayer()
	{
		const auto s_go = GetGameObject();
		if (!s_go)
		{
			LOG_ASSERT("invalid shared pointer. Renderer::IsActiveAndEnabled()");
			return Layer::Default;
		}
		return s_go->GetLayer();
	}

	void Renderer::SetGlobalTransform(const Mesh& mesh) const
	{
		const auto& transform = GetTransform();

		transform->LocalPosition(mesh.GetGlobalTranslate());
		transform->LocalRotation(mesh.GetGlobalRotate());
		transform->LocalScale(mesh.GetGlobalScale());
	}

	void Renderer::CreateQuad()
	{
		m_quad.Create();

		{
			std::vector<VECTOR3> vec;
			vec.resize(4);

			// Position
			vec.at(0) = VECTOR3(+1.0f, +1.0f, 0.0f);	// Right Top
			vec.at(1) = VECTOR3(-1.0f, +1.0f, 0.0f);	// Left Top
			vec.at(2) = VECTOR3(+1.0f, -1.0f, 0.0f);	// Right Bottom
			vec.at(3) = VECTOR3(-1.0f, -1.0f, 0.0f);	// Left Bottom
			m_quad.SetPositions(vec);

			// Normal
			vec.at(0)
				= vec.at(1)
				= vec.at(2)
				= vec.at(3)
				= VECTOR3::Forward;
			m_quad.SetNormals(vec);
		}
		{
			std::vector<VECTOR2> vec;
			vec.resize(4);

			// UV
			vec.at(0) = VECTOR2(+1.0f, +0.0f);	// Right Top
			vec.at(1) = VECTOR2(+0.0f, +0.0f);	// Left Top
			vec.at(2) = VECTOR2(+1.0f, +1.0f);	// Right Bottom
			vec.at(3) = VECTOR2(+0.0f, +1.0f);	// Left Bottom
			m_quad.SetUVs(vec);
		}
		{
			std::vector<unsigned> vec;
			vec.resize(4);

			// Index
			vec.at(0) = 0;
			vec.at(1) = 1;
			vec.at(2) = 2;
			vec.at(3) = 3;

			m_quad.SetIndices(vec);
		}

		// Set Bounds.
		m_quad.SetBounds(Bounds(VECTOR3(0.0f, 0.0f, 0.0f), VECTOR3(1.0f, 1.0f, 0.0f)));
	}


}// namespace TKGEngine