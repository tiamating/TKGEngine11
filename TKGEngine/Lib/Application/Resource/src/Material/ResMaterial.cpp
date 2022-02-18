
#include "IResMaterial.h"

#include "Application/Resource/inc/Texture.h"
#include "Application/Resource/inc/Shader.h"
#include "Application/Resource/inc/ConstantBuffer.h"
#include "Application/Resource/inc/Shader_Defined.h"
#include "Utility/inc/myfunc_vector.h"
#include "Utility/inc/myfunc_file.h"
#include "Systems/inc/AssetSystem.h"

#include <vector>
#include <string>

#include <d3d11.h>
#include <wrl.h>
#include <cassert>
#include <cstddef>

namespace TKGEngine
{
	class ResMaterial
		: public IResMaterial
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		ResMaterial() = default;
		virtual ~ResMaterial() = default;
		ResMaterial(const ResMaterial&) = delete;
		ResMaterial& operator=(const ResMaterial&) = delete;

		// AssetDataBase
#ifdef USE_IMGUI
		void OnGUI() override {};
		void OnGUI(bool is_shadow_material) override;
#endif // USE_IMGUI
		// ~AssetDataBase

#ifdef USE_IMGUI
		// セーブされたフレームのみtrue
		bool IsSavedOnFrame() const override;
#endif // USE_IMGUI

		bool Activate(ID3D11DeviceContext* p_context, bool write_depth, bool set_depth_ps) override;

		int GetRenderQueue() const override;
		void SetRenderQueue(int queue) override;
		void UseTarget(bool use_target) override;
		bool IsUsedTarget() const override;
		void SetStencilMask(Stencil::Mask mask) override;
		Stencil::Mask GetStencilMask() const override;

		void SetTopology(PRIMITIVE_TOPOLOGY topology) override;
		void SetDepthState(StateManager::DS state) override;
		void SetRasterizerState(StateManager::RS state) override;
		void SetBlendState(StateManager::BS state) override;

		// Texture
		void AddTexture(int slot, const std::string& filepath, bool force_srgb = false) override;
		void RemoveTexture() override;
		// ~Texture

		// Shader
		int GetInputSlotFromType(VERTEX_ELEMENT_TYPE type) override;
		void SetVertexShader(const std::string& filepath) override;
		void SetPixelShader(const std::string& filepath) override;
		void SetDepthPixelShader(const std::string& filepath) override;
		void SetGeometryShader(const std::string& filepath) override;
		void SetDomainShader(const std::string& filepath) override;
		void SetHullShader(const std::string& filepath) override;
		void RemoveVertexShader() override;
		void RemovePixelShader() override;
		void RemoveDepthPixelShader() override;
		void RemoveGeometryShader() override;
		void RemoveDomainShader() override;
		void RemoveHullShader() override;
		// ~Shader

		bool HasVertexShader() override;
		bool HasPixelShader() override;
		bool HasDepthPixelShader() override;
		bool HasGeometryShader() override;
		bool HasDomainShader() override;
		bool HasHullShader() override;
		bool HasTextures() const override;

		void CreateCBMaterial(bool force_clear = true) override;
		void SetParam(const std::string& param_name, const void* p_src, size_t size) override;
		void GetParam(const std::string& param_name, void* p_dst, size_t size) const override;

		void CreateCBTexture() override;
		void SetTextureOffset(const VECTOR2& offset) override;
		void SetTextureTilling(const VECTOR2& tilling) override;
		void SetTextureCutout(const float cutout) override;


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		friend class cereal::access;
		template <class Archive>
		void serialize(Archive& archive, const std::uint32_t version)
		{
			//if (version > 0)
			{
				archive(
					cereal::base_class<IResMaterial>(this),
					CEREAL_NVP(m_render_queue),
					CEREAL_NVP(m_use_target),
					CEREAL_NVP(m_depth_only),
					CEREAL_NVP(m_stencil_mask),
					CEREAL_NVP(m_primitive_topology),
					CEREAL_NVP(m_depth_state),
					CEREAL_NVP(m_rasterizer_state),
					CEREAL_NVP(m_blend_state),
					CEREAL_NVP(m_cb_material),
					CEREAL_NVP(m_vertex_shader_filedata),
					CEREAL_NVP(m_pixel_shader_filedata),
					CEREAL_NVP(m_depth_pixel_shader_filedata),
					CEREAL_NVP(m_geometry_shader_filedata),
					CEREAL_NVP(m_domain_shader_filedata),
					CEREAL_NVP(m_hull_shader_filedata),
					CEREAL_NVP(m_cb_texture_param),
					CEREAL_NVP(m_texture_list)
				);
			}
		}

		struct TextureData
		{
			int slot = -1;
			FileLoadStateData texture_filedata;
			Texture texture;
			bool force_srgb = false;
			bool use_white = true;

		private:
			friend class cereal::access;
			template <class Archive>
			void serialize(Archive& archive, const std::uint32_t version)
			{
				if (version == 1)
				{
					archive(
						CEREAL_NVP(slot),
						CEREAL_NVP(texture_filedata),
						CEREAL_NVP(force_srgb),
						CEREAL_NVP(use_white)
					);
				}
				else if (version > 0)
				{
					archive(
						CEREAL_NVP(slot),
						CEREAL_NVP(texture_filedata),
						CEREAL_NVP(force_srgb)
					);
				}
			}
		};
		friend struct cereal::detail::Version<TextureData>;

		struct CB_TEXTURE
		{
			VECTOR2 offset = VECTOR2::Zero;
			VECTOR2 tilling = VECTOR2::One;
			float alpha_cutout = 0.0f;
			CB_TEXTURE()
			{/* nothing */
			}
			CB_TEXTURE(VECTOR2 offset, VECTOR2 tiling, float cutoff)
				: offset(offset), tilling(tiling), alpha_cutout(cutoff)
			{/* nothing */
			}
		private:
			friend class cereal::access;
			template <class Archive>
			void serialize(Archive& archive, const std::uint32_t version)
			{
				//if (version > 0)
				{
					archive(
						CEREAL_NVP(offset),
						CEREAL_NVP(tilling),
						CEREAL_NVP(alpha_cutout)
					);
				}
			}
		};

		// ==============================================
		// private methods
		// ==============================================
		void SetAsyncOnLoad() override;
		void OnLoad() override;

		// use GUI
		bool Save();
		void IncreaseTextureList();
		void ReduceTextureList();
		void SetTextureSlot(int idx, int slot);
		void SetTextureFilename(int idx, const std::string& filepath, bool force_srgb = false);
		// ~use GUI

		void SetPipelineTopology(ID3D11DeviceContext* p_context);


		// ==============================================
		// private variables
		// ==============================================
#ifdef USE_IMGUI
		// セーブされたフレームかどうか
		bool m_is_saved_on_frame = false;
#endif // USE_IMGUI

		int m_render_queue = RenderQueue::Geometry;
		bool m_use_target = false;
		bool m_depth_only = false;
		Stencil::Mask m_stencil_mask = Stencil::Mask::None;

		PRIMITIVE_TOPOLOGY m_primitive_topology = PRIMITIVE_TOPOLOGY::TriangleList;
		StateManager::DS m_depth_state = StateManager::DS::Geometry;
		StateManager::RS m_rasterizer_state = StateManager::RS::FillBack;
		StateManager::BS m_blend_state = StateManager::BS::Opaque;

		ConstantBuffer m_cb_texture;
		ShaderConstantBuffer m_cb_material;

		// Shader
		FileLoadStateData m_vertex_shader_filedata;
		FileLoadStateData m_pixel_shader_filedata;
		FileLoadStateData m_depth_pixel_shader_filedata;
		FileLoadStateData m_geometry_shader_filedata;
		FileLoadStateData m_domain_shader_filedata;
		FileLoadStateData m_hull_shader_filedata;
		Shader m_shader;
		PixelShader m_depth_ps;
		// ~Shader

		// Texture
		//CB_TEXTURE m_cb_texture_param;//s[8];
		CB_TEXTURE m_cb_texture_param;
		std::vector<TextureData> m_texture_list;
		// ~Texture
	};


	////////////////////////////////////////////////////////
	// Static member definition
	////////////////////////////////////////////////////////
	ResourceManager<IResMaterial> IResMaterial::m_caches;
	std::mutex IResMaterial::m_cache_mutex;

	static constexpr const char* DEFAULT_NAME = "default";

	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	// IResMaterial
	IResMaterial* IResMaterial::CreateInterface()
	{
		return new ResMaterial();
	}

	std::shared_ptr<IResMaterial> IResMaterial::Create()
	{
		static int idx = 0;

		std::shared_ptr<IResMaterial> s_ptr(CreateInterface());
		s_ptr->m_has_resource = true;
		s_ptr->SetName(std::string(DEFAULT_NAME + std::to_string(idx++)));

		s_ptr->CreateCBTexture();
		return s_ptr;
	}

	std::shared_ptr<IResMaterial> IResMaterial::Create(const std::string& name)
	{
		std::shared_ptr<IResMaterial> s_ptr(CreateInterface());
		s_ptr->m_has_resource = true;
		s_ptr->SetName(name);

		s_ptr->CreateCBTexture();
		return s_ptr;
	}

	std::shared_ptr<IResMaterial> IResMaterial::LoadAsync(const std::string& filename)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResMaterial> res_find = m_caches.Search(filename);
		if (res_find)
		{
			m_cache_mutex.unlock();
			return res_find;
		}
		std::shared_ptr<IResMaterial> res_new(CreateInterface());
		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Set async loader
		std::lock_guard<std::mutex> lock(res_new->m_load_state_mutex);
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		AssetSystem::GetInstance().Add(std::bind(&IResMaterial::SetAsyncOnLoad, res_new));

		return res_new;
	}

	std::shared_ptr<IResMaterial> IResMaterial::Load(const std::string& filename)
	{
		// Access resource map
		m_cache_mutex.lock();
		std::shared_ptr<IResMaterial> res_find = m_caches.Search(filename);
		if (res_find)
		{
			m_cache_mutex.unlock();
			return res_find;
		}
		std::shared_ptr<IResMaterial> res_new(CreateInterface());
		res_new->SetFilePath(filename);
		m_caches.Set(filename, res_new);
		m_cache_mutex.unlock();

		// Load Resource
		res_new->m_load_state_mutex.lock();
		res_new->m_is_loading = true;
		res_new->m_is_loaded = false;
		res_new->m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_WAITING;
		res_new->OnLoad();
		if (!res_new->HasResource())
		{
			res_new->Release();
			res_new->m_load_state_mutex.unlock();
			res_new.reset();
			return std::shared_ptr<IResMaterial>();
		}
		res_new->m_load_state_mutex.unlock();
		return res_new;
	}

	void IResMaterial::RemoveUnused()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveUnusedCache();
	}

	void IResMaterial::Release()
	{
		std::lock_guard<std::mutex> lock(m_cache_mutex);
		m_caches.RemoveCache(this->GetFilePath());
	}
	// ~IResMaterial

	// ResMaterial
#ifdef USE_IMGUI
	void ResMaterial::OnGUI(const bool is_shadow_material)
	{
		// Parameters
#pragma region Parameters
		// ファイルパス
		{
			ImGui::Text("File");
			ImGui::SameLine();
			ImGui::Text("\"%s\"", GetFilePath());
		}
		// Saveボタン
		{
			if (ImGui::Button("Save"))
			{
				Save();
				m_is_saved_on_frame = true;
			}
			else
			{
				m_is_saved_on_frame = false;
			}
		}
		// マテリアル名
		{
			ImGui::IDWrapped id("Set Material Name");
			ImGui::Text("Name:");
			ImGui::SameLine();
			static std::string str;
			str = GetName();
			ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
			if (ImGui::InputText("##Material name", &str, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				SetName(str);
			}
		}
		constexpr ImGuiTreeNodeFlags tree_flags =
			ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Selected;
		// フラグ、状態の設定
		if (ImGui::TreeNodeEx("Parameters", tree_flags))
		{
			ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
			// Render Queue
			ImGui::Text("Render Queue");
			ImGui::SameLine();
			ImGui::HelpMarker("Transparent is over 3000.");
			ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.4f);
			ImGui::DragInt("##Render Queue", &m_render_queue, 0.1f, 0, 5000, "%d", ImGuiSliderFlags_AlwaysClamp);
			ImGui::SameLine();
			if (ImGui::Button("Default Queue"))
			{
				ImGui::OpenPopup("Default Queue Popup");
			}
			if (ImGui::BeginPopup("Default Queue Popup"))
			{
				// Background
				if (ImGui::Selectable("Background"))
				{
					m_render_queue = RenderQueue::Background;
				}
				// Geometry
				if (ImGui::Selectable("Geometry"))
				{
					m_render_queue = RenderQueue::Geometry;
				}
				// AlphaTest
				if (ImGui::Selectable("AlphaTest"))
				{
					m_render_queue = RenderQueue::AlphaTest;
				}
				// Transparent
				if (ImGui::Selectable("Transparent"))
				{
					m_render_queue = RenderQueue::Transparent;
				}
				// Overlay
				if (ImGui::Selectable("Overlay"))
				{
					m_render_queue = RenderQueue::Overlay;
				}
				ImGui::EndPopup();
			}
			// 各種ステート、フラグ
			// Use target
			ImGui::Text("Use Target");
			ImGui::SameLine();
			ImGui::Checkbox("##Use Target", &m_use_target);
			// Topology
			ImGui::Text("Topology");
			ImGui::AlignedSameLine(0.5f);
			ImGui::ComboEnum<PRIMITIVE_TOPOLOGY, PRIMITIVE_TOPOLOGY::MAX_PRIMITIVE_TOPOLOGY>("##Topology", &m_primitive_topology);
			// Depth state
			ImGui::Text("DepthState");
			ImGui::AlignedSameLine(0.5f);
			ImGui::ComboEnum<StateManager::DS, StateManager::DS::MAX_DEPTHSTENCIL_STATE>("##DepthState", &m_depth_state);
			// Stencil mask
			ImGui::Text("Stencil Mask");
			ImGui::AlignedSameLine(0.5f);
			ImGui::ComboEnum<Stencil::Mask, Stencil::Mask::Max_Mask>("##Stencil Mask", &m_stencil_mask);
			// Rasterizer state
			ImGui::Text("Rasterizer");
			ImGui::AlignedSameLine(0.5f);
			ImGui::ComboEnum<StateManager::RS, StateManager::RS::MAX_RATERIZER_STATE>("##RasterizerState", &m_rasterizer_state);
			// Blend state
			ImGui::Text("BlendState");
			ImGui::AlignedSameLine(0.5f);
			ImGui::ComboEnum<StateManager::BS, StateManager::BS::MAX_BLEND_STATE>("##BlendState", &m_blend_state);

			ImGui::TreePop();
		}
#pragma endregion
		// ~Parameters

		// Texture
#pragma region Texture
		if (ImGui::TreeNodeEx("Texture", tree_flags))
		{
			// Texture CBufferデータ
			// Offset
			ImGui::Text("Offset");
			if (ImGui::DragFloat2("##Offset", &m_cb_texture_param.offset.x, 0.005f, 0.0f, 0.0f, "%.2f"))
			{
				SetTextureOffset(m_cb_texture_param.offset);
			}
			// Tilling
			ImGui::Text("Tilling");
			if (ImGui::DragFloat2("##Tilling", &m_cb_texture_param.tilling.x, 0.005f, 0.0f, 0.0f, "%.2f"))
			{
				SetTextureTilling(m_cb_texture_param.tilling);
			}
			// Alpha cutout
			ImGui::Text("Alpha cutout");
			if (ImGui::DragFloat("##Alpha cutout", &m_cb_texture_param.alpha_cutout, 0.005f, 0.0f, 1.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
			{
				SetTextureCutout(m_cb_texture_param.alpha_cutout);
			}

			// Texture管理タブバー表示
			constexpr ImGuiTabBarFlags tab_bar_flags =
				ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_TabListPopupButton | ImGuiTabBarFlags_FittingPolicyResizeDown;
			if (ImGui::BeginTabBar("TextureTagBar", tab_bar_flags))
			{
				// 追加用のボタン
				{
					if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Leading | ImGuiTabItemFlags_NoTooltip))
					{
						// Texture Dataの追加
						m_texture_list.emplace_back(TextureData());
					}
				}
				// タブ表示
				ImGuiTabItemFlags tab_item_flags = ImGuiTabItemFlags_None;
				for (size_t i = 0; i < m_texture_list.size(); ++i)
				{
					ImGui::IDWrapped id(i);
					bool is_open = true;
					auto& data = m_texture_list.at(i);
					const char* tex_name = data.texture.GetName() ? data.texture.GetName() : "No Name";
					if (ImGui::BeginTabItem(tex_name, &is_open, tab_item_flags))
					{
						// Slot
						ImGui::Text("Slot");
						ImGui::SameLine();
						// SlotのスタートがTEXTURE_RENDERER_SLOT_BEGIN_NUMのため0基準にする
						int input_slot = data.slot - TEXTURE_RENDERER_SLOT_BEGIN_NUM;
						if (ImGui::Combo("##Material TexSlot", &input_slot, TEXTURE_RENDERER_SLOT_NAMES, std::size(TEXTURE_RENDERER_SLOT_NAMES)))
						{
							data.slot = input_slot + TEXTURE_RENDERER_SLOT_BEGIN_NUM;
						}
						// sRGB
						ImGui::Text("Force sRGB");
						ImGui::SameLine();
						ImGui::Checkbox("##force sRGB", &data.force_srgb);
						// テクスチャがセットされていないときに白と黒どちらを使用するか
						if (!data.texture.HasTexture())
						{
							ImGui::Text("Use Dummy White");
							ImGui::SameLine();
							ImGui::Checkbox("##Use Dummy White", &data.use_white);
						}
						// Path
						ImGui::Text("Path");
						ImGui::Text("\"%s\"", data.texture_filedata.filepath.c_str());
						// Texture
						ImGui::Text("Texture");
						{
							ImGui::IndentWrapped indent(ImGui::INDENT_VALUE);
							data.texture.OnGUI();
						}
						if (ImGui::Button("Load"))
						{
							std::string filepath;
							if (MyFunc::FetchOpenFileName(
								filepath,
								TEXT("Texture(*.png,*.dds,*.jpg,*.jpeg,*.bmp,*.gif,*.tiff,*.tga,*.hdr)\0*.png;*.dds;*.jpg;*.jpeg;*.bmp;*.gif;*.tiff;*.tga;*.hdr\0"),
								TEXT("Load Texture"),
								TEXT(".\\Asset\\Textures")
							))
							{
								data.texture.Load(filepath.c_str(), data.force_srgb);
								data.texture_filedata.Set(filepath);
							}
						}

						ImGui::EndTabItem();
					}
					// タブを閉じたときは、要素を削除する
					if (!is_open)
					{
						m_texture_list.erase(m_texture_list.begin() + i);
					}
				}
				ImGui::EndTabBar();
			}
			ImGui::TreePop();
		}
#pragma endregion
		// ~Texture

		// Shader
#pragma region Shader
		ImGui::Separator();
		if (ImGui::TreeNodeEx("Shader", tree_flags))
		{
			ImGui::HelpMarker("When serialize hlsl file, file extension is changed cso.");
			// VS
			ImGui::Text("Vertex Shader");
			ImGui::Text("\"%s\"", m_shader.VS().GetFilePath());
			if (ImGui::Button("Load(.cso) ##VS"))
			{
				std::string filepath;
				if (MyFunc::FetchOpenFileName(filepath, TEXT("VertexShader(*vs.cso)\0*vs.cso\0"), TEXT("Load Vertex Shader"), TEXT(".\\Shader")))
				{
					SetVertexShader(filepath);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Load(.hlsl) ##VS"))
			{
				std::string filepath;
				if (MyFunc::FetchOpenFileName(filepath, TEXT("Vertex Shader(*vs.hlsl)\0*vs.hlsl\0"), TEXT("Load Vertex Shader"), TEXT(".\\Shader")))
				{
					m_shader.VS().Compile(filepath, "main", "vs_5_0", true);
					std::string cso_filepath = MyFunc::SplitFilePathNoExtension(filepath) + ".cso";
					m_vertex_shader_filedata.Set(cso_filepath);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Remove##VS"))
			{
				RemoveVertexShader();
			}
			ImGui::Separator();
			// PS
			ImGui::Text("Pixel Shader");
			ImGui::Text("\"%s\"", m_shader.PS().GetFilePath());
			static bool force_clear = true;
			ImGui::Checkbox("Force clear##PS", &force_clear);
			ImGui::SameLine();
			ImGui::HelpMarker("If you want to remain CB_Material data, force clear is false.");
			if (ImGui::Button("Load(.cso) ##PS"))
			{
				std::string filepath;
				if (MyFunc::FetchOpenFileName(filepath, TEXT("Pixel Shader(*ps.cso)\0*ps.cso\0"), TEXT("Load Pixel Shader"), TEXT(".\\Shader")))
				{
					m_shader.PS().Load(filepath);
					m_pixel_shader_filedata.Set(filepath);
					CreateCBMaterial(force_clear);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Load(.hlsl) ##PS"))
			{
				std::string filepath;
				if (MyFunc::FetchOpenFileName(filepath, TEXT("Pixel Shader(*ps.hlsl)\0*ps.hlsl\0"), TEXT("Load Pixel Shader"), TEXT(".\\Shader")))
				{
					m_shader.PS().Compile(filepath, "main", "ps_5_0", true);
					std::string cso_filepath = MyFunc::SplitFilePathNoExtension(filepath) + ".cso";
					m_pixel_shader_filedata.Set(cso_filepath);
					CreateCBMaterial(force_clear);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Remove##PS"))
			{
				RemovePixelShader();
			}
			ImGui::Separator();
			// Depth PS
			if (!is_shadow_material)
			{
				ImGui::Text("Depth Pixel Shader");
				ImGui::Text("\"%s\"", m_depth_ps.GetFilePath());
				if (ImGui::Button("Load(.cso) ##DepthPS"))
				{
					std::string filepath;
					if (MyFunc::FetchOpenFileName(filepath, TEXT("Depth Pixel Shader(*ps.cso)\0*ps.cso\0"), TEXT("Load Depth Pixel Shader"), TEXT(".\\Shader")))
					{
						SetDepthPixelShader(filepath);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Load(.hlsl) ##DepthPS"))
				{
					std::string filepath;
					if (MyFunc::FetchOpenFileName(filepath, TEXT("Depth Pixel Shader(*ps.hlsl)\0*ps.hlsl\0"), TEXT("Load Depth Pixel Shader"), TEXT(".\\Shader")))
					{
						m_depth_ps.Compile(filepath, "main", "ps_5_0", true);
						std::string cso_filepath = MyFunc::SplitFilePathNoExtension(filepath) + ".cso";
						m_depth_pixel_shader_filedata.Set(cso_filepath);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Remove##DepthPS"))
				{
					RemoveDepthPixelShader();
				}
				ImGui::Separator();
			}
			// GS
			ImGui::Text("Geometry Shader");
			ImGui::Text("\"%s\"", m_shader.GS().GetFilePath());
			if (ImGui::Button("Load(.cso) ##GS"))
			{
				std::string filepath;
				if (MyFunc::FetchOpenFileName(filepath, TEXT("Geometry Shader(*gs.cso)\0*gs.cso\0"), TEXT("Load Geometry Shader"), TEXT(".\\Shader")))
				{
					SetGeometryShader(filepath);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Load(.hlsl) ##GS"))
			{
				std::string filepath;
				if (MyFunc::FetchOpenFileName(filepath, TEXT("Geometry Shader(*gs.hlsl)\0*gs.hlsl\0"), TEXT("Load Geometry Shader"), TEXT(".\\Shader")))
				{
					m_shader.GS().Compile(filepath, "main", "gs_5_0", true);
					std::string cso_filepath = MyFunc::SplitFilePathNoExtension(filepath) + ".cso";
					m_geometry_shader_filedata.Set(cso_filepath);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Remove##GS"))
			{
				RemoveGeometryShader();
			}
			ImGui::Separator();
			// HS
			ImGui::Text("Hull Shader");
			ImGui::Text("\"%s\"", m_shader.HS().GetFilePath());
			if (ImGui::Button("Load(.cso) ##HS"))
			{
				std::string filepath;
				if (MyFunc::FetchOpenFileName(filepath, TEXT("Hull Shader(*hs.cso)\0*hs.cso\0"), TEXT("Load Hull Shader"), TEXT(".\\Shader")))
				{
					SetHullShader(filepath);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Load(.hlsl) ##HS"))
			{
				std::string filepath;
				if (MyFunc::FetchOpenFileName(filepath, TEXT("Hull Shader(*hs.hlsl)\0*hs.hlsl\0"), TEXT("Load Hull Shader"), TEXT(".\\Shader")))
				{
					m_shader.HS().Compile(filepath, "main", "hs_5_0", true);
					std::string cso_filepath = MyFunc::SplitFilePathNoExtension(filepath) + ".cso";
					m_hull_shader_filedata.Set(cso_filepath);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Remove##HS"))
			{
				RemoveHullShader();
			}
			ImGui::Separator();
			// DS
			ImGui::Text("Domain Shader");
			ImGui::Text("\"%s\"", m_shader.DS().GetFilePath());
			if (ImGui::Button("Load(.cso) ##DS"))
			{
				std::string filepath;
				if (MyFunc::FetchOpenFileName(filepath, TEXT("Domain Shader(*ds.cso)\0*ds.cso\0"), TEXT("Load Domain Shader"), TEXT(".\\Shader")))
				{
					SetDomainShader(filepath);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Load(.hlsl) ##DS"))
			{
				std::string filepath;
				if (MyFunc::FetchOpenFileName(filepath, TEXT("Domain Shader(*ds.hlsl)\0*ds.hlsl\0"), TEXT("Load Domain Shader"), TEXT(".\\Shader")))
				{
					m_shader.DS().Compile(filepath, "main", "ds_5_0", true);
					std::string cso_filepath = MyFunc::SplitFilePathNoExtension(filepath) + ".cso";
					m_domain_shader_filedata.Set(cso_filepath);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button("Remove##DS"))
			{
				RemoveDomainShader();
			}

			ImGui::TreePop();
		}
#pragma endregion
		// ~Shader

		// CBuffer
		if (ImGui::TreeNodeEx("CB_Material", tree_flags))
		{
			m_cb_material.OnGUI();

			ImGui::TreePop();
		}
	}
	bool ResMaterial::IsSavedOnFrame() const
	{
		return m_is_saved_on_frame;
	}
#endif // USE_IMGUI

	bool ResMaterial::Activate(ID3D11DeviceContext* p_context, const bool write_depth, const bool set_depth_ps)
	{
		// Shader
		if (!m_shader.Activate(p_context, set_depth_ps))
		{
			// 頂点シェーダーをセットできていないと描画に失敗するためfalseを返す
			return false;
		}
		if (set_depth_ps)
		{
			if (m_depth_ps.HasResource())
			{
				m_depth_ps.Activate(p_context);
			}
			else
			{
				m_depth_ps.Deactivate(p_context);
			}
		}

		// Texture
		for (const auto& texture_data : m_texture_list)
		{
			if (texture_data.slot < 0)
			{
				continue;
			}
			if (texture_data.texture.HasTexture())
			{
				texture_data.texture.SetSRV(p_context, texture_data.slot, ShaderVisibility::ALL);
			}
			else
			{
				if (texture_data.use_white)
					Texture::SetDummyWhiteSRV(p_context, texture_data.slot, ShaderVisibility::ALL);
				else
					Texture::SetDummyBlackSRV(p_context, texture_data.slot, ShaderVisibility::ALL);
			}
		}

		// Constant Buffers
		m_cb_material.SetVS(p_context, CBS_MATERIAL);
		m_cb_material.SetHS(p_context, CBS_MATERIAL);
		m_cb_material.SetDS(p_context, CBS_MATERIAL);
		m_cb_material.SetGS(p_context, CBS_MATERIAL);
		m_cb_material.SetPS(p_context, CBS_MATERIAL);

		m_cb_texture.SetVS(p_context, CBS_TEXTURE);
		m_cb_texture.SetHS(p_context, CBS_TEXTURE);
		m_cb_texture.SetDS(p_context, CBS_TEXTURE);
		m_cb_texture.SetGS(p_context, CBS_TEXTURE);
		m_cb_texture.SetPS(p_context, CBS_TEXTURE);

		// Primitive Topology
		SetPipelineTopology(p_context);

		// DepthStencil State
		StateManager::SetState(p_context, m_depth_state, m_stencil_mask, write_depth);
		// Blend State
		StateManager::SetState(p_context, m_blend_state);
		// Rasterizer State
		StateManager::SetState(p_context, m_rasterizer_state);

		return true;
	}

	int ResMaterial::GetRenderQueue() const
	{
		return m_render_queue;
	}

	void ResMaterial::SetRenderQueue(int queue)
	{
		m_render_queue = queue;
	}

	void ResMaterial::UseTarget(bool use_target)
	{
		m_use_target = use_target;
	}

	bool ResMaterial::IsUsedTarget() const
	{
		return m_use_target;
	}

	void ResMaterial::SetStencilMask(const Stencil::Mask mask)
	{
		// TODO : bit演算を使って複数フラグを含めることもできる
		m_stencil_mask = mask;
	}

	Stencil::Mask ResMaterial::GetStencilMask() const
	{
		return m_stencil_mask;
	}

	void ResMaterial::SetTopology(const PRIMITIVE_TOPOLOGY topology)
	{
		m_primitive_topology = topology;
	}

	void ResMaterial::SetDepthState(const StateManager::DS state)
	{
		m_depth_state = state;
	}

	void ResMaterial::SetRasterizerState(const StateManager::RS state)
	{
		m_rasterizer_state = state;
	}

	void ResMaterial::SetBlendState(const StateManager::BS state)
	{
		m_blend_state = state;
	}

	void ResMaterial::AddTexture(const int slot, const std::string& filepath, const bool force_srgb)
	{
		if (slot < 0)
			return;

		// 同一スロットに既にセットされている場合は上書きロードする
		for (auto&& data : m_texture_list)
		{
			if (data.slot == slot)
			{
				data.texture_filedata.Set(filepath);
				data.texture.Load(filepath, force_srgb);
				return;
			}
		}

		TextureData new_data;
		new_data.slot = slot;
		new_data.texture_filedata.Set(filepath);
		new_data.texture.Load(filepath, force_srgb);
		new_data.force_srgb = force_srgb;
		m_texture_list.emplace_back(new_data);
	}

	void ResMaterial::RemoveTexture()
	{
		if (m_texture_list.size() <= 0)
		{
			return;
		}

		m_texture_list.pop_back();
		m_texture_list.shrink_to_fit();
	}

	int ResMaterial::GetInputSlotFromType(const VERTEX_ELEMENT_TYPE type)
	{
		if (!m_shader.VS().HasResource())
			return -1;

		return m_shader.VS().GetSlotFromType(type);
	}

	void ResMaterial::SetVertexShader(const std::string& filepath)
	{
		m_shader.VS().Load(filepath);
		m_vertex_shader_filedata.Set(filepath);
	}

	void ResMaterial::SetPixelShader(const std::string& filepath)
	{
		m_shader.PS().Load(filepath);
		m_pixel_shader_filedata.Set(filepath);
		CreateCBMaterial();
	}

	void ResMaterial::SetDepthPixelShader(const std::string& filepath)
	{
		m_depth_ps.Load(filepath);
		m_depth_pixel_shader_filedata.Set(filepath);
	}

	void ResMaterial::SetGeometryShader(const std::string& filepath)
	{
		m_shader.GS().Load(filepath);
		m_geometry_shader_filedata.Set(filepath);
	}

	void ResMaterial::SetDomainShader(const std::string& filepath)
	{
		m_shader.DS().Load(filepath);
		m_domain_shader_filedata.Set(filepath);
	}

	void ResMaterial::SetHullShader(const std::string& filepath)
	{
		m_shader.HS().Load(filepath);
		m_hull_shader_filedata.Set(filepath);
	}

	void ResMaterial::RemoveVertexShader()
	{
		m_shader.VS().Remove();
		m_vertex_shader_filedata.Clear();
	}

	void ResMaterial::RemovePixelShader()
	{
		m_shader.PS().Remove();
		m_pixel_shader_filedata.Clear();
		m_cb_material.Release();
	}

	void ResMaterial::RemoveDepthPixelShader()
	{
		m_depth_ps.Remove();
		m_depth_pixel_shader_filedata.Clear();
	}

	void ResMaterial::RemoveGeometryShader()
	{
		m_shader.GS().Remove();
		m_geometry_shader_filedata.Clear();
	}

	void ResMaterial::RemoveDomainShader()
	{
		m_shader.DS().Remove();
		m_domain_shader_filedata.Clear();
	}

	void ResMaterial::RemoveHullShader()
	{
		m_shader.HS().Remove();
		m_hull_shader_filedata.Clear();
	}

	bool ResMaterial::HasVertexShader()
	{
		return m_shader.VS().HasResource();
	}

	bool ResMaterial::HasPixelShader()
	{
		return m_shader.PS().HasResource();
	}

	bool ResMaterial::HasDepthPixelShader()
	{
		return m_depth_ps.HasResource();
	}

	bool ResMaterial::HasGeometryShader()
	{
		return m_shader.GS().HasResource();
	}

	bool ResMaterial::HasDomainShader()
	{
		return m_shader.DS().HasResource();
	}

	bool ResMaterial::HasHullShader()
	{
		return m_shader.HS().HasResource();
	}

	bool ResMaterial::HasTextures() const
	{
		for (const auto& tex : m_texture_list)
		{
			if (!tex.texture.HasTexture())
				return false;
		}

		return true;
	}

	void ResMaterial::CreateCBMaterial(bool force_clear)
	{
		if (!m_shader.PS().HasResource())
		{
			m_cb_material.Release();
			return;
		}

		m_cb_material.Create(m_shader.PS().GetReflection()->GetConstantBufferByName(CB_MATERIAL_NAME), force_clear);
	}

	void ResMaterial::SetParam(const std::string& param_name, const void* p_src, size_t size)
	{
		m_cb_material.SetParam(param_name, p_src, size);
	}

	void ResMaterial::GetParam(const std::string& param_name, void* p_dst, size_t size) const
	{
		m_cb_material.GetParam(param_name, p_dst, size);
	}

	void ResMaterial::CreateCBTexture()
	{
		CB_TEXTURE cb = m_cb_texture_param;
		m_cb_texture.Create(static_cast<int>(sizeof(cb)), &cb, false);
	}

	void ResMaterial::SetTextureOffset(const VECTOR2& offset)
	{
		m_cb_texture_param.offset = offset;
		m_cb_texture.SetParam(&offset, sizeof(offset), offsetof(CB_TEXTURE, offset));
	}

	void ResMaterial::SetTextureTilling(const VECTOR2& tilling)
	{
		m_cb_texture_param.tilling = tilling;
		m_cb_texture.SetParam(&tilling, sizeof(tilling), offsetof(CB_TEXTURE, tilling));
	}

	void ResMaterial::SetTextureCutout(const float cutout)
	{
		m_cb_texture_param.alpha_cutout = cutout;
		m_cb_texture.SetParam(&cutout, sizeof(cutout), offsetof(CB_TEXTURE, alpha_cutout));
	}

	void ResMaterial::IncreaseTextureList()
	{
		m_texture_list.emplace_back(TextureData());
	}

	void ResMaterial::ReduceTextureList()
	{
		if (m_texture_list.size() <= 0)
			return;

		m_texture_list.pop_back();
	}

	void ResMaterial::SetTextureSlot(int idx, int slot)
	{
		if (slot < 0)
			return;

		if (m_texture_list.size() >= static_cast<size_t>(idx))
			return;

		auto&& tex_data = m_texture_list.at(idx);
		tex_data.slot = slot;
	}

	void ResMaterial::SetTextureFilename(int idx, const std::string& filepath, bool force_srgb)
	{
		if (m_texture_list.size() >= static_cast<size_t>(idx))
			return;

		auto&& tex_data = m_texture_list.at(idx);
		tex_data.texture_filedata.Set(filepath);
		tex_data.texture.LoadAsync(filepath, force_srgb);
	}

	void ResMaterial::SetPipelineTopology(ID3D11DeviceContext* p_context)
	{
		switch (m_primitive_topology)
		{
			case PRIMITIVE_TOPOLOGY::TriangleList:
				p_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				break;
			case PRIMITIVE_TOPOLOGY::TriangleStrip:
				p_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
				break;
			case PRIMITIVE_TOPOLOGY::PointList:
				p_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
				break;
			case PRIMITIVE_TOPOLOGY::LineList:
				p_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
				break;
			case PRIMITIVE_TOPOLOGY::LineStrip:
				p_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
				break;
			case PRIMITIVE_TOPOLOGY::ControlPoint3:
				p_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
				break;
			default:
				assert(0 && "Invalid enum primitive topoloty");
		}
	}

	void ResMaterial::SetAsyncOnLoad()
	{
		// ロード準備
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}
		// ロード開始
		bool b_result = false;
		{
			std::ifstream ifs(this->GetFilePath(), std::ios::in | std::ios::binary);

			if (!ifs.is_open())
			{
				b_result = false;
			}
			else
			{
				cereal::BinaryInputArchive ar(ifs);

				ar(*this);
				b_result = true;
			}
		}
		// ロード終了
		{
			std::lock_guard<std::mutex> lock(m_load_state_mutex);
			m_is_loading = false;
			m_is_loaded = true;
			if (b_result)
			{
				// 成功
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;

				// テクスチャ用CBuffer作成
				CreateCBTexture();
				// ファイルパスからリソースデータをロードする
				{
					// Texture
					for (auto& data : m_texture_list)
					{
						// シリアライズ時の状態をチェック
						if (!data.texture_filedata.HasData())
							continue;
						// テクスチャをロードする
						data.texture.Load(data.texture_filedata.filepath, data.force_srgb);
					}
					// Shader
					{
						// VS
						if (m_vertex_shader_filedata.HasData())
						{
							m_shader.VS().Load(m_vertex_shader_filedata.filepath);
						}
						// PS
						if (m_pixel_shader_filedata.HasData())
						{
							m_shader.PS().Load(m_pixel_shader_filedata.filepath);
							// CB_Materialのbufferを作成
							m_cb_material.Create();
						}
						// Depth PS
						if (m_depth_pixel_shader_filedata.HasData())
						{
							m_depth_ps.Load(m_depth_pixel_shader_filedata.filepath);
						}
						// GS
						if (m_geometry_shader_filedata.HasData())
						{
							m_shader.GS().Load(m_geometry_shader_filedata.filepath);
						}
						// HS
						if (m_hull_shader_filedata.HasData())
						{
							m_shader.HS().Load(m_hull_shader_filedata.filepath);
						}
						// DS
						if (m_domain_shader_filedata.HasData())
						{
							m_shader.DS().Load(m_domain_shader_filedata.filepath);
						}
					}
				}
			}
			else
			{
				// 失敗
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
		}
	}

	void ResMaterial::OnLoad()
	{
		// ロード準備
		{
			m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_LOADING;
			m_has_resource = false;
		}
		// ロード開始
		bool b_result = false;
		{
			std::ifstream ifs(this->GetFilePath(), std::ios::in | std::ios::binary);
			if (!ifs.is_open())
			{
				b_result = false;
			}
			else
			{
				cereal::BinaryInputArchive ar(ifs);

				ar(*this);
				b_result = true;
			}
		}
		// ロード終了
		{
			m_is_loading = false;
			m_is_loaded = true;
			if (b_result)
			{
				// 成功
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_SUCCESS;
				m_has_resource = true;

				// テクスチャ用CBuffer作成
				CreateCBTexture();
				// ファイルパスからリソースデータをロードする
				{
					// Texture
					for (auto& data : m_texture_list)
					{
						// シリアライズ時の状態をチェック
						if (!data.texture_filedata.HasData())
							continue;
						// テクスチャをロードする
						data.texture.Load(data.texture_filedata.filepath, data.force_srgb);
					}
					// Shader
					{
						// VS
						if (m_vertex_shader_filedata.HasData())
						{
							m_shader.VS().Load(m_vertex_shader_filedata.filepath);
						}
						// PS
						if (m_pixel_shader_filedata.HasData())
						{
							m_shader.PS().Load(m_pixel_shader_filedata.filepath);
							// CB_Materialのbufferを作成
							m_cb_material.Create();
						}
						// Depth PS
						if (m_depth_pixel_shader_filedata.HasData())
						{
							m_depth_ps.Load(m_depth_pixel_shader_filedata.filepath);
						}
						// GS
						if (m_geometry_shader_filedata.HasData())
						{
							m_shader.GS().Load(m_geometry_shader_filedata.filepath);
						}
						// HS
						if (m_hull_shader_filedata.HasData())
						{
							m_shader.HS().Load(m_hull_shader_filedata.filepath);
						}
						// DS
						if (m_domain_shader_filedata.HasData())
						{
							m_shader.DS().Load(m_domain_shader_filedata.filepath);
						}
					}
				}
			}
			else
			{
				// 失敗
				m_load_state = ASSET_LOAD_ASYNC_STATE::ASSET_LOAD_ASYNC_FAILED;
				m_has_resource = false;
			}
		}
	}

	bool ResMaterial::Save()
	{
		// ファイル名取得
		std::string filepath;
		if (!MyFunc::FetchSaveFileName(filepath, TEXT("Material(*.material)\0*.material\0"), TEXT("Save Material"), TEXT(".\\Asset\\Materials")))
		{
			return false;
		}
		// ファイルの上書き時は拡張子があるためチェック
		if (MyFunc::GetExtension(filepath).empty())
		{
			filepath += MATERIAL_EXTENSION;
		}
		// 新しいファイルパスをセット
		SetFilePath(filepath);
		// シリアライズ
		// Binary
		{
			std::ofstream ofs(filepath.c_str(), std::ios::out | std::ios::binary);
			if (!ofs.is_open())
			{
				assert(0 && "failed save file. ResMaterial::Save()");
				return false;
			}
			cereal::BinaryOutputArchive ar(ofs);
			ar(*this);
		}
		// Json
#ifdef SAVE_JSON
		{
			filepath += "J";
			std::ofstream ofs(filepath.c_str(), std::ios::out | std::ios::binary);
			if (!ofs.is_open())
			{
				assert(0 && "failed save file. ResMaterial::Save()");
				return false;
			}
			cereal::JSONOutputArchive ar(ofs);
			ar(*this);
		}
#endif// ~SAVE_JSON

		return true;
	}

	// ~ResMaterial


}// namespace TKGEngine

CEREAL_REGISTER_TYPE(TKGEngine::ResMaterial);
CEREAL_CLASS_VERSION(TKGEngine::ResMaterial, 1);
CEREAL_REGISTER_POLYMORPHIC_RELATION(TKGEngine::IResMaterial, TKGEngine::ResMaterial)

CEREAL_CLASS_VERSION(TKGEngine::ResMaterial::TextureData, 1);