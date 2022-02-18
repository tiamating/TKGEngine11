

#include "RendererManager.h"

#include "Components/interface/ICamera.h"

#include "Application/Resource/inc/Shader_Defined.h"
#include "Application/Resource/inc/Material_Defined.h"

#include "Managers/SceneManager.h"
#include "Managers/LightManager.h"
#include "Systems/inc/IGraphics.h"
#include "Systems/inc/IGUI.h"
#include "Systems/inc/Graphics_Defined.h"
#include "Systems/inc/StateManager.h"

#include <iterator>
#include <algorithm>
#include <cassert>

#include <d3d11.h>

namespace TKGEngine
{
	////////////////////////////////////////////////////////
	// Static member definition
	////////////////////////////////////////////////////////
	// 配列サイズ
	static constexpr int START_SHADOW_LIST_SIZE = 1000;
	static constexpr int ADD_SHADOW_LIST_SIZE = 500;

	static constexpr int START_MAIN_LIST_SIZE = 1000;
	static constexpr int ADD_MAIN_LIST_SIZE = 500;

	static constexpr int START_UI_LIST_SIZE = 50;
	static constexpr int ADD_UI_LIST_SIZE = 50;

	// UI path
	std::mutex RendererManager::UIPath::m_mutex;
	std::list<std::shared_ptr<IRenderer>> RendererManager::UIPath::m_renderer_list;
	std::vector<RendererManager::DrawData> RendererManager::UIPath::m_draw_data_list;
	int RendererManager::UIPath::m_current_draw_data_count;
	int RendererManager::UIPath::m_prev_draw_data_count;
	VertexBuffer RendererManager::UIPath::m_instance_buffer;
	// ~UI path

	std::mutex RendererManager::m_mutex;
	std::list<std::shared_ptr<IRenderer>> RendererManager::m_renderer_list;

	std::vector<MainData> RendererManager::m_shadow_path_list;
	std::vector<MainData> RendererManager::m_main_path_list;
	std::vector<UIData> RendererManager::m_UI_path_list;

	int RendererManager::m_path_current_data_sizes[static_cast<int>(RenderPath::MAX_RENDER_PATH)] = {};
	int RendererManager::m_path_current_data_counts[static_cast<int>(RenderPath::MAX_RENDER_PATH)] = {};
	int RendererManager::m_path_prev_data_counts[static_cast<int>(RenderPath::MAX_RENDER_PATH)] = {};

	std::vector<RendererManager::DrawData> RendererManager::m_draw_data_list;
	int RendererManager::m_current_draw_data_count;
	int RendererManager::m_prev_draw_data_count;
	VertexBuffer RendererManager::m_instance_buffer;

	float RendererManager::m_current_vp_width = 0.0f;
	float RendererManager::m_current_vp_height = 0.0f;

	////////////////////////////////////////////////////////
	// Class Methods
	////////////////////////////////////////////////////////
	void RendererManager::Initialize()
	{
		// Shadow
		{
			constexpr int path_idx = static_cast<int>(RenderPath::Shadow);
			m_shadow_path_list.resize(START_SHADOW_LIST_SIZE);
			m_path_current_data_sizes[path_idx] = START_SHADOW_LIST_SIZE;
		}
		// Main
		{
			constexpr int path_idx = static_cast<int>(RenderPath::Main);
			m_main_path_list.resize(START_MAIN_LIST_SIZE);
			m_path_current_data_sizes[path_idx] = START_MAIN_LIST_SIZE;
			if (!m_instance_buffer.Create(
				nullptr,
				static_cast<int>(sizeof(MainInstance)),
				START_MAIN_LIST_SIZE,
				VERTEX_ELEMENT_TYPE::MAIN_INSTANCE,
				BUFFER_HEAP_TYPE::BUFFER_HEAP_DYNAMIC))
			{
				assert(0 && "failed VertexBuffer::Create() RendererManager::Initialize(RenderPath::Main)");
				return;
			}
			m_draw_data_list.resize(START_MAIN_LIST_SIZE);
		}
		// UI
		UIPath::Initialize();
	}

	void RendererManager::Run(const std::shared_ptr<ICamera>& camera)
	{
#ifdef USE_IMGUI
		// シーンビューかつUI描画が無効ならUIは描画しない
		const bool draw_ui = !IGUI::Get().IsSceneView() || IGUI::Get().DoDrawUI();
#endif // USE_IMGUI

		// vectorにセット
		SetDataList(camera);
#ifdef USE_IMGUI
		if (draw_ui)
#endif // USE_IMGUI
			UIPath::FrameBegin(camera);

		// ライトの更新
		LightManager::CalculateSceneLight(m_shadow_path_list, m_path_current_data_counts[static_cast<int>(RenderPath::Shadow)], camera);
		// 描画パスを実行
		{
			UpdateShadow(camera);
			UpdateMain(camera);
#ifdef USE_IMGUI
			if (draw_ui)
#endif // USE_IMGUI
				UIPath::Update(camera);
		}

		// 後始末
		CleanUpVector();
#ifdef USE_IMGUI
		if (draw_ui)
#endif // USE_IMGUI
			UIPath::FrameEnd();

		// データ数のクリアと保持
		ClearAndStore();
	}

	std::list<std::shared_ptr<IRenderer>>::iterator RendererManager::RegisterManager(const std::shared_ptr<IRenderer>& p_renderer, const bool is_ui)
	{
		if (is_ui)
			return UIPath::RegisterManager(p_renderer);

		std::lock_guard<std::mutex> lock(m_mutex);
		m_renderer_list.emplace_back(p_renderer);
		auto itr_end = m_renderer_list.end();
		return --itr_end;
	}

	void RendererManager::UnregisterManager(std::list<std::shared_ptr<IRenderer>>::iterator itr, const bool is_ui)
	{
		if (is_ui)
		{
			UIPath::UnregisterManager(itr);
			return;
		}

		std::lock_guard<std::mutex> lock(m_mutex);
		m_renderer_list.erase(itr);
	}

	void RendererManager::SetDataList(const std::shared_ptr<ICamera>& camera)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		const int culling_layer = camera->GetCullingLayer();
		const VECTOR3 camera_pos = camera->GetWorldPosition();
		const auto end_itr = m_renderer_list.end();

		for (auto itr = m_renderer_list.begin(); itr != end_itr; ++itr)
		{
			auto& renderer = *itr;

			// TODO : カメラごとに状態を持っているわけではないので上書きされてしまう
			// 映っているかのフラグを初期化
			renderer->SetVisible(false);

			// GameObject、シーン、レンダラー全てがアクティブかチェック
			if (!renderer->IsActiveAndEnabled())
			{
				continue;
			}

			// Check Culling layer
			if ((culling_layer & (1 << static_cast<int>(renderer->GetLayer()))) > 0)
			{
				continue;
			}

			// Set data list
			{
				const int subset_cnt = renderer->GetSubsetCount();

				if (subset_cnt <= 0)
				{
					continue;
				}

				// レンダラーごとの行列、AABBなどの計算、更新
				renderer->CalculateRenderParameter(camera);

				// TODO : 距離を使用してLODの区分分けなどを行う
				const float sq_dist = VECTOR3::DistanceSq(camera_pos, renderer->GetWorldPosition());

				// 描画の有無でリストを振り分ける
				// Shadow
				if (renderer->GetShadowCastMode() != ShadowCastingMode::OFF)
				{
					AddShadowData(subset_cnt, sq_dist, renderer);
				}
				// Main
				if (renderer->GetShadowCastMode() != ShadowCastingMode::ShadowsOnly)
				{
					AddMainData(subset_cnt, sq_dist, renderer);
				}
			}
		}
	}

	void RendererManager::AddShadowData(int subset_count, float distance, const std::shared_ptr<IRenderer>& renderer)
	{
		constexpr int path_idx = static_cast<int>(RenderPath::Shadow);
		const unsigned mesh_hash = renderer->GetHashMesh();

		int& path_size = m_path_current_data_sizes[path_idx];
		int& path_cnt = m_path_current_data_counts[path_idx];

		// 現在のリストサイズを超えるならリストとインスタンスバッファを拡張する
		if (path_cnt + subset_count > path_size)
		{
			// リスト拡張
			path_size += ADD_SHADOW_LIST_SIZE;
			m_shadow_path_list.resize(path_size);
		}

		// Set Data
		for (int i = 0; i < subset_count; ++i)
		{
			// Renderキューが0未満なら描画しない
			const auto queue = renderer->GetRenderQueue(i);
			if (queue < 0)
				continue;
			// ソート、描画に使用するSubsetデータを保持する
			auto& data = m_shadow_path_list.at(path_cnt);
			data.queue = queue;
			data.can_batching = renderer->CanBatching();
			data.distance = distance;
			data.mesh_hash = mesh_hash;
			data.material_hash = renderer->GetHashMaterial(i);
			data.subset_idx = i;
			data.use_target = renderer->IsUsedTarget(i);
			// shared_ptrが前情報と同一なら上書きしない
			if (data.renderer != renderer)
			{
				data.renderer = renderer;
			}
			// 現在のデータ数をインクリメント
			++path_cnt;
		}
		// 映っているフラグを立てる
		renderer->SetVisible(true);
	}

	void RendererManager::AddMainData(int subset_count, float distance, const std::shared_ptr<IRenderer>& renderer)
	{
		constexpr int path_idx = static_cast<int>(RenderPath::Main);
		const unsigned mesh_hash = renderer->GetHashMesh();

		int& path_size = m_path_current_data_sizes[path_idx];
		int& path_cnt = m_path_current_data_counts[path_idx];

		// 現在のリストサイズを超えるならリストとインスタンスバッファを拡張する
		if (path_cnt + subset_count > path_size)
		{
			// リスト拡張
			path_size += ADD_MAIN_LIST_SIZE;
			m_main_path_list.resize(path_size);
			// インスタンスバッファ拡張
			if (!m_instance_buffer.Resize(static_cast<int>(sizeof(MainInstance)), path_size))
			{
				assert(0 && "failed InstanceBuffer::Resize(). RendererManager::AddMainData()");
				return;
			}
			// Drawリスト拡張
			m_draw_data_list.resize(path_size);
		}
		// Set Data
		for (int i = 0; i < subset_count; ++i)
		{
			// Renderキューが0未満なら描画しない
			const auto queue = renderer->GetRenderQueue(i);
			if (queue < 0)
				continue;
			// ソート、描画に使用するSubsetデータを保持する
			auto& data = m_main_path_list.at(path_cnt);
			data.queue = queue;
			data.can_batching = renderer->CanBatching();
			data.distance = distance;
			data.mesh_hash = mesh_hash;
			data.material_hash = renderer->GetHashMaterial(i);
			data.subset_idx = i;
			data.use_target = renderer->IsUsedTarget(i);
			// shared_ptrが前情報と同一なら上書きしない
			if (data.renderer != renderer)
			{
				data.renderer = renderer;
			}
			// 現在のデータ数をインクリメント
			++path_cnt;
		}
	}

	// /////////////////////////////////////////////////////////////////////
	// ~Divide Part
	// /////////////////////////////////////////////////////////////////////

	// /////////////////////////////////////////////////////////////////////
	// Update Part
	// /////////////////////////////////////////////////////////////////////
	void RendererManager::UpdateMain(const std::shared_ptr<ICamera>& camera)
	{
		constexpr int path_idx = static_cast<int>(RenderPath::Main);
		const int count = m_path_current_data_counts[path_idx];

		ID3D11DeviceContext* dc_z = IGraphics::Get().DC(0, Graphics::DC_RENDER_PATH::DC_RP_Z_PREPATH);
		ID3D11DeviceContext* dc_main = IGraphics::Get().DC(0, Graphics::DC_RENDER_PATH::DC_RP_MAIN);
		// Set Camera parameter
		{
			// Viewport
			camera->SetViewport(dc_z);
			// CBuffers
			camera->SetCBCamera(dc_z);
			camera->SetCBViewProjection(dc_z);

			// Viewport
			camera->SetViewport(dc_main);
			// CBuffers
			camera->SetCBCamera(dc_main);
			camera->SetCBViewProjection(dc_main);

			// RTV and DSV
#if 1
			camera->SetRTVs(dc_z, false, true);
			camera->SetRTVs(dc_main, true, true);
#else
			// TODO : 仮置き BackBuffer
			IGraphics::Get().SetRTBackBuffer(dc_z, true);
			IGraphics::Get().SetRTBackBuffer(dc_main, true);
#endif
		}
		// Set Light parameter
		{
			LightManager::GetInstance()->SetPipeline(dc_main);
		}

		if (count <= 0)
		{
			return;
		}

		const auto itr_begin = m_main_path_list.begin();
		auto itr_end = itr_begin;
		std::advance(itr_end, count);
		auto itr_current = itr_begin;

		auto& draw_list = m_draw_data_list;
		int& draw_cnt = m_current_draw_data_count;

		// フラスタムカリング
		// OPTIMIZE : フラスタムをAABBとしてカリングに使用することもできる
		//auto frustum_aabb = camera->GetFrustum().GetAABB();
		for (auto itr = itr_begin; itr != itr_end; ++itr)
		{
			const auto& ref_renderer = itr->renderer;
			// パーティクルは範囲が不明のため視錐台カリングはなし
			if (ref_renderer->IsThroughFrustumCulling())
			{
				itr->do_render = true;
				continue;
			}
			itr->do_render = ref_renderer->GetRendererBounds().ContainsBy(camera->GetFrustum());
		}

		// Sort DoRender
		std::sort(itr_begin, itr_end, SortMain_DoRender);
		while (itr_current != itr_end)
		{
			if (itr_current->do_render)
				break;
			++itr_current;
		}
		if (itr_current == itr_end)
		{
			return;
		}

		auto itr_same_queue_end = itr_current;		// 同一Queueの終端を持つ
		auto itr_same_material_end = itr_current;	// 同一Materialの終端を持つ
		auto itr_same_mesh_end = itr_current;		// 同一Meshの終端を持つ
		auto itr_same_subset_end = itr_current;		// 同一Subsetの終端を持つ

		// InstanceBufferをMapする
		MainInstance* instance = nullptr;
		int instance_count = 0;
		if (!m_instance_buffer.Map(dc_z, reinterpret_cast<void**>(&instance)))
		{
			assert(0 && "failed InstanceBuffer::Map(). RendererManager::UpdateMain()");
			return;
		}

		// Queueソート
		std::sort(itr_current, itr_end, SortMain_Queue);

		// 終端までループ
		while (itr_current != itr_end)
		{
			// 同一Queueのend取得
			while (true)
			{
				if (itr_same_queue_end == itr_end)
				{
					break;
				}
				if (itr_same_queue_end->queue != itr_current->queue)
				{
					break;
				}
				++itr_same_queue_end;
			}

			// 不透明
			if (itr_begin->queue < RenderQueue::Transparent)
			{
				// バッチ可能ソート
				std::sort(itr_current, itr_same_queue_end, SortMain_Batching);

				while (itr_current != itr_same_queue_end)
				{
					// バッチ可能がくるまで1つずつセットしていく
					if (!itr_current->can_batching)
					{
						auto&& draw_data = draw_list.at(draw_cnt++);
						draw_data = *itr_current;
						draw_data.start_idx = instance_count++;
						draw_data.instance_cnt = 1;
						itr_current->renderer->SetInstance(instance++);

						++itr_current;
					}
					// バッチ可能になったためbreak
					else
					{
						itr_same_material_end = itr_current;
						itr_same_mesh_end = itr_current;
						itr_same_subset_end = itr_current;
						break;
					}
				}

				// Materialソート
				std::sort(itr_current, itr_same_queue_end, SortMain_Material);

				while (itr_current != itr_same_queue_end)
				{
					// 同一マテリアルの範囲を取得
					while (itr_same_material_end != itr_same_queue_end)
					{
						if (itr_current->material_hash != itr_same_material_end->material_hash)
						{
							break;
						}
						++itr_same_material_end;
					}

					// Meshソート
					std::sort(itr_current, itr_same_material_end, SortMain_Mesh);

					while (itr_current != itr_same_material_end)
					{
						// 同一メッシュの範囲を取得
						while (itr_same_mesh_end != itr_same_material_end)
						{
							if (itr_current->mesh_hash != itr_same_mesh_end->mesh_hash)
							{
								break;
							}
							++itr_same_mesh_end;
						}

						// Subset_Idxソート
						std::sort(itr_current, itr_same_mesh_end, SortMain_Subset);

						while (itr_current != itr_same_mesh_end)
						{
							// 同一サブセットIdxの範囲を取得
							while (itr_same_subset_end != itr_same_mesh_end)
							{
								if (itr_current->subset_idx != itr_same_subset_end->subset_idx)
								{
									break;
								}
								++itr_same_subset_end;
							}

							// DrawDataに詰め込む
							if (itr_current != itr_same_subset_end)
							{
								// 共通データを詰め込む
								auto&& draw_data = draw_list.at(draw_cnt);
								draw_data = *itr_current;
								draw_data.start_idx = instance_count;
								draw_data.instance_cnt = 0;

								// インスタンス毎のデータを詰め込む
								while (itr_current != itr_same_subset_end)
								{
									++instance_count;
									++draw_data.instance_cnt;
									itr_current->renderer->SetInstance(instance++);

									++itr_current;
								}
							}

							++draw_cnt;
						}

					}// while same material.
				}// while same queue.
			}// if opaque.
			// 半透明
			else
			{
				// Transparentの距離ソート
				std::sort(itr_current, itr_same_queue_end, SortMain_Distance);

				while (itr_current != itr_same_queue_end)
				{
					auto&& draw_data = draw_list.at(draw_cnt++);
					draw_data = *itr_current;
					draw_data.start_idx = instance_count++;
					draw_data.instance_cnt = 1;
					itr_current->renderer->SetInstance(instance++);

					++itr_current;
				}
			}
		}// 終端までループ

		// Unmap
		m_instance_buffer.Unmap(dc_z);

		// Set Command
		{
			// Draw
			for (int i = 0; i < draw_cnt; ++i)
			{
				auto&& draw_data = draw_list.at(i);

				// Pre-Z
				if (draw_data.queue < RenderQueue::AlphaTest && !draw_data.renderer->IsParticle())
				{
					draw_data.renderer->Render(
						dc_z,
						draw_data.subset_idx, draw_data.start_idx, draw_data.instance_cnt,
						m_instance_buffer,
						camera,
						true
					);
				}

				// Main
				if (!draw_data.renderer->IsParticle() && draw_data.use_target)
				{
					camera->CopyAndBindTargets(dc_main);
				}
				draw_data.renderer->Render(
					dc_main,
					draw_data.subset_idx, draw_data.start_idx, draw_data.instance_cnt,
					m_instance_buffer,
					camera,
					false
				);
			}
		}
	}

	void RendererManager::UpdateShadow(const std::shared_ptr<ICamera>& camera)
	{
		constexpr int path_idx = static_cast<int>(RenderPath::Shadow);
		const int count = m_path_current_data_counts[path_idx];

		ID3D11DeviceContext* dc_shadow = IGraphics::Get().DC(0, Graphics::DC_RENDER_PATH::DC_RP_SHADOW);
		// 描画されるライトの影を描画する
		LightManager::RenderShadow(dc_shadow);
	}

	// --------------------------------------------------------------------------
	// Sort Func
	// --------------------------------------------------------------------------
	// https://wp.kazto.dev/2018/05/08/stdsort%E3%81%AB%E6%B8%A1%E3%81%99%E6%AF%94%E8%BC%83%E9%96%A2%E6%95%B0%E3%81%AE%E3%80%8Cstrict-weak-ordering%E3%80%8D%E3%83%AB%E3%83%BC%E3%83%AB%E3%81%AB%E3%81%A4%E3%81%84%E3%81%A6/
#pragma region Sort func
	bool RendererManager::SortMain_DoRender(const MainData& left, const MainData& right)
	{
		return left.do_render == false && right.do_render == true;
	}
	bool RendererManager::SortMain_Queue(const MainData& left, const MainData& right)
	{
		return left.queue < right.queue;
	}
	bool RendererManager::SortMain_Distance(const MainData& left, const MainData& right)
	{
		return left.distance > right.distance;
	}
	bool RendererManager::SortMain_Material(const MainData& left, const MainData& right)
	{
		return left.material_hash < right.material_hash;
	}
	bool RendererManager::SortMain_Mesh(const MainData& left, const MainData& right)
	{
		return left.mesh_hash < right.mesh_hash;
	}
	bool RendererManager::SortMain_Subset(const MainData& left, const MainData& right)
	{
		return left.subset_idx < right.subset_idx;
	}
	bool RendererManager::SortMain_Batching(const MainData& left, const MainData& right)
	{
		return left.can_batching == false && right.can_batching == true;
	}

	bool RendererManager::SortUI_Depth(const UIData& left, const UIData& right)
	{
		return left.depth < right.depth;
	}
	bool RendererManager::SortUI_Material(const UIData& left, const UIData& right)
	{
		return left.material_hash < right.material_hash;
	}
	bool RendererManager::SortUI_Texture(const UIData& left, const UIData& right)
	{
		return left.texture_hash < right.texture_hash;
	}
	// --------------------------------------------------------------------------
	// ~Sort Func
	// --------------------------------------------------------------------------
#pragma endregion


	// /////////////////////////////////////////////////////////////////////
	// ~Update Part
	// /////////////////////////////////////////////////////////////////////

	void RendererManager::CleanUpVector()
	{
		// Shadow List
		{
			constexpr int path_idx = static_cast<int>(RenderPath::Shadow);
			// Data list
			{
				const int current = m_path_current_data_counts[path_idx];
				const int prev = m_path_prev_data_counts[path_idx];
				if (prev > current)
				{
					for (int i = current; i < prev; ++i)
					{
						m_shadow_path_list.at(i).renderer = nullptr;
					}
				}
			}
		}
		// Main List
		{
			constexpr int path_idx = static_cast<int>(RenderPath::Main);
			// Data list
			{
				const int current = m_path_current_data_counts[path_idx];
				const int prev = m_path_prev_data_counts[path_idx];
				if (prev > current)
				{
					for (int i = current; i < prev; ++i)
					{
						m_main_path_list.at(i).renderer = nullptr;
					}
				}
			}
			// Draw list
			{
				const int current = m_current_draw_data_count;
				const int prev = m_prev_draw_data_count;
				if (prev > current)
				{
					for (int i = current; i < prev; ++i)
					{
						m_draw_data_list.at(i).renderer = nullptr;
					}
				}
			}
		}
	}

	void RendererManager::ClearAndStore()
	{
		// 現在のデータ数の保持とクリア
		for (int i = 0; i < static_cast<int>(RenderPath::MAX_RENDER_PATH); ++i)
		{
			m_path_prev_data_counts[i] = m_path_current_data_counts[i];
			m_path_current_data_counts[i] = 0;
		}
		// Main
		m_prev_draw_data_count = m_current_draw_data_count;
		m_current_draw_data_count = 0;
	}

	// /////////////////////////////////////////////////////////////////////
	// UI Path
	// /////////////////////////////////////////////////////////////////////
	std::list<std::shared_ptr<IRenderer>>::iterator RendererManager::UIPath::RegisterManager(const std::shared_ptr<IRenderer>& p_renderer)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		UIPath::m_renderer_list.emplace_back(p_renderer);
		auto itr_end = UIPath::m_renderer_list.end();
		return --itr_end;
	}

	void RendererManager::UIPath::UnregisterManager(std::list<std::shared_ptr<IRenderer>>::iterator itr)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		UIPath::m_renderer_list.erase(itr);
	}

	void RendererManager::UIPath::Initialize()
	{
		constexpr int path_idx = static_cast<int>(RenderPath::UI);
		m_UI_path_list.resize(START_UI_LIST_SIZE);
		m_path_current_data_sizes[path_idx] = START_UI_LIST_SIZE;
		if (!m_instance_buffer.Create(
			nullptr,
			static_cast<int>(sizeof(UIInstance)),
			START_UI_LIST_SIZE,
			VERTEX_ELEMENT_TYPE::UI_INSTANCE,
			BUFFER_HEAP_TYPE::BUFFER_HEAP_DYNAMIC))
		{
			assert(0 && "failed VertexBuffer::Create() RendererManager::UIPath::Initialize(RenderPath::UI)");
			return;
		}
		m_draw_data_list.resize(START_UI_LIST_SIZE);
	}

	void RendererManager::UIPath::FrameBegin(const std::shared_ptr<ICamera>& camera)
	{
		SetDataList(camera);
	}

	void RendererManager::UIPath::FrameEnd()
	{
		CleanUpVector();
		ClearAndStore();
	}

	void RendererManager::UIPath::SetDataList(const std::shared_ptr<ICamera>& camera)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		constexpr int path_idx = static_cast<int>(RenderPath::UI);
		const unsigned culling_layer = camera->GetCullingLayer();
		auto& path_size = m_path_current_data_sizes[path_idx];
		bool is_resized = false;
		int list_cnt = 0;
		const auto end_itr = m_renderer_list.end();

		for (auto itr = m_renderer_list.begin(); itr != end_itr; ++itr)
		{
			auto& renderer = *itr;
			// Check Active and Enabled
			if (!renderer->IsActiveAndEnabled())
			{
				continue;
			}

			// Check Culling layer
			if ((culling_layer & (1 << static_cast<int>(renderer->GetLayer()))) > 0)
			{
				continue;
			}

			// Check has texture.
			const auto tex_hash = renderer->GetHashTexture();
			if (tex_hash == 0)
			{
				continue;
			}

			// Set data list
			{
				// NDC変換行列を計算する
				renderer->CalculateRenderParameter(camera);

				// Check buffer size
				if (list_cnt + 1 > path_size)
				{
					// リサイズ
					is_resized = true;
					path_size += ADD_UI_LIST_SIZE;
					m_UI_path_list.resize(path_size);
				}

				// Set temp list
				auto&& data = m_UI_path_list.at(list_cnt);
				data.depth = renderer->GetDepth();
				data.material_hash = renderer->GetHashMaterial(0);
				data.use_target = renderer->IsUsedTarget(0);
				data.texture_hash = tex_hash;
				if (data.renderer != renderer)
				{
					data.renderer = renderer;
				}

				// データ数をインクリメント
				++list_cnt;
			}
		}
		// 現在のデータ数を保持
		m_path_current_data_counts[path_idx] = list_cnt;

		// 現在のリストサイズを変更したならバッファを拡張する
		if (is_resized)
		{
			// インスタンスバッファ拡張
			if (!m_instance_buffer.Resize(static_cast<int>(sizeof(UIInstance)), path_size))
			{
				assert(0 && "failed InstanceBuffer::Resize(). RendererManager::SetDataList()");
				return;
			}
			// Drawリスト拡張
			m_draw_data_list.resize(path_size);
		}
	}

	void RendererManager::UIPath::Update(const std::shared_ptr<ICamera>& camera)
	{
		constexpr int path_idx = static_cast<int>(RenderPath::UI);
		const int count = m_path_current_data_counts[path_idx];

		ID3D11DeviceContext* dc_ui = IGraphics::Get().DC(0, Graphics::DC_RENDER_PATH::DC_RP_UI);
		// Set Camera parameter.
		{
			// Viewport
			camera->SetViewport(dc_ui);
			// CBuffers
			camera->SetCBCamera(dc_ui);
			camera->SetCBViewProjection(dc_ui);
		}

		// RTV and DSV
		// TODO : PostProcess後のフレームバッファにUIを重ねるようにする
#if 0
		camera->SetRTVs(dc_ui, true, false);
#else
		IGraphics::Get().SetRTBackBuffer(dc_ui, false);
#endif

		if (count <= 0)
		{
			return;
		}

		const auto itr_begin = m_UI_path_list.begin();
		auto itr_end = itr_begin;
		std::advance(itr_end, count);
		auto itr_current = itr_begin;

		auto& draw_list = m_draw_data_list;
		int& draw_cnt = m_current_draw_data_count;

		auto itr_same_depth_end = itr_current;		// 同一Depthの終端を持つ
		auto itr_same_material_end = itr_current;	// 同一Materialの終端を持つ
		auto itr_same_texture_end = itr_current;	// 同一Textureの終端を持つ

		// InstanceBufferをMapする
		UIInstance* instance = nullptr;
		int instance_count = 0;
		if (!m_instance_buffer.Map(dc_ui, reinterpret_cast<void**>(&instance)))
		{
			assert(0 && "failed InstanceBuffer::Map(). RendererManager::UIPath::Update()");
			return;
		}

		// Depthソート
		std::sort(itr_current, itr_end, SortUI_Depth);

		// 終端までループ
		while (itr_current != itr_end)
		{
			while (true)
			{
				if (itr_same_depth_end == itr_end)
				{
					break;
				}
				if (itr_same_depth_end->depth != itr_current->depth)
				{
					break;
				}
				++itr_same_depth_end;
			}

			// Materialソート
			std::sort(itr_current, itr_same_depth_end, SortUI_Material);

			while (itr_current != itr_same_depth_end)
			{
				// 同一マテリアルの範囲を取得
				while (itr_same_material_end != itr_same_depth_end)
				{
					if (itr_current->material_hash != itr_same_material_end->material_hash)
					{
						break;
					}
					++itr_same_material_end;
				}

				// Textureソート
				std::sort(itr_current, itr_same_material_end, SortUI_Texture);

				while (itr_current != itr_same_material_end)
				{
					// 同一テクスチャの範囲を取得
					while (itr_same_texture_end != itr_same_material_end)
					{
						if (itr_current->texture_hash != itr_same_texture_end->texture_hash)
						{
							break;
						}
						++itr_same_texture_end;
					}

					// DrawDataに詰め込む
					if (itr_current != itr_same_texture_end)
					{
						// 共通データを詰め込む
						auto&& draw_data = draw_list.at(draw_cnt);
						draw_data = *itr_current;
						draw_data.start_idx = instance_count;
						draw_data.instance_cnt = 0;

						// インスタンス毎のデータを詰め込む
						while (itr_current != itr_same_texture_end)
						{
							++instance_count;
							++draw_data.instance_cnt;
							itr_current->renderer->SetInstance(instance++);

							++itr_current;
						}
					}// ~DrawDataに詰め込む

					++draw_cnt;
				}// while same texture.
			}// while same material.
		}// 終端までループ

		// Unmap
		m_instance_buffer.Unmap(dc_ui);

		// Set Command
		{
			// Draw
			for (int i = 0; i < draw_cnt; ++i)
			{
				auto&& draw_data = draw_list.at(i);

				draw_data.renderer->Render(
					dc_ui,
					draw_data.subset_idx, draw_data.start_idx, draw_data.instance_cnt,
					m_instance_buffer,
					camera,
					false
				);
			}
		}
	}

	void RendererManager::UIPath::CleanUpVector()
	{
		constexpr int path_idx = static_cast<int>(RenderPath::UI);
		// Data list
		{
			const int current = m_path_current_data_counts[path_idx];
			const int prev = m_path_prev_data_counts[path_idx];
			if (prev > current)
			{
				for (int i = current; i < prev; ++i)
				{
					m_UI_path_list.at(i).renderer = nullptr;
				}
			}
		}
		// Draw list
		{
			const int current = m_current_draw_data_count;
			const int prev = m_prev_draw_data_count;
			if (prev > current)
			{
				for (int i = current; i < prev; ++i)
				{
					m_draw_data_list.at(i).renderer = nullptr;
				}
			}
		}
	}

	void RendererManager::UIPath::ClearAndStore()
	{
		m_prev_draw_data_count = m_current_draw_data_count;
		m_current_draw_data_count = 0;
	}

	// /////////////////////////////////////////////////////////////////////
	// ~UI Path
	// /////////////////////////////////////////////////////////////////////

}// namespace TKGEngine