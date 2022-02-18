#pragma once

#include "Systems/inc/TKGEngine_Defined.h"

#include "Application/Objects/Components/interface/IRenderer.h"
#include "Application/Resource/inc/VertexBuffer.h"
#include "Utility/inc/template_thread.h"

#include <list>
#include <vector>
#include <memory>
#include <mutex>

#include <Windows.h>


namespace TKGEngine
{
	class ICamera;
	class IRenderer;

	class RendererManager
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static void Initialize();
		static void Run(const std::shared_ptr<ICamera>& camera);

		static std::list<std::shared_ptr<IRenderer>>::iterator RegisterManager(const std::shared_ptr<IRenderer>& p_renderer, bool is_ui);
		static void UnregisterManager(std::list<std::shared_ptr<IRenderer>>::iterator itr, bool is_ui);

#ifdef USE_IMGUI
		static void OnGUI();

		// シーンビューのクリック時レイピック用
		static const std::list<std::shared_ptr<IRenderer>> GetRendererList();
#endif// #ifdef USE_IMGUI

		// ==============================================
		// public variables
		// ==============================================


	private:
		// ==============================================
		// private struct
		// ==============================================
		struct DrawData
		{
			int start_idx;
			int instance_cnt;
			bool use_target;
			int queue;
			int subset_idx;
			std::shared_ptr<IRenderer> renderer;

			void operator=(const MainData& data)
			{
				use_target = data.use_target;
				queue = data.queue;
				subset_idx = data.subset_idx;
				renderer = data.renderer;
			}
			void operator=(const UIData& data)
			{
				use_target = data.use_target;
				renderer = data.renderer;
			}
		};

		enum class RenderPath
		{
			Shadow = 0,
			Main,
			UI,

			MAX_RENDER_PATH
		};

		class UIPath
		{
		public:
			static std::list<std::shared_ptr<IRenderer>>::iterator RegisterManager(const std::shared_ptr<IRenderer>& p_renderer);
			static void UnregisterManager(std::list<std::shared_ptr<IRenderer>>::iterator itr);

			static void Initialize();
			static void FrameBegin(const std::shared_ptr<ICamera>& camera);
			static void Update(const std::shared_ptr<ICamera>& camera);
			static void FrameEnd();

		private:
			static void SetDataList(const std::shared_ptr<ICamera>& camera);
			static void CleanUpVector();
			static void ClearAndStore();

			static std::mutex m_mutex;
			// 存在するRenderer一括管理用リスト
			static std::list<std::shared_ptr<IRenderer>> m_renderer_list;

			// 描画時に使用するデータ情報
			static std::vector<DrawData> m_draw_data_list;
			static int m_current_draw_data_count;
			static int m_prev_draw_data_count;

			// インスタンスバッファ
			static VertexBuffer m_instance_buffer;
		};


		// ==============================================
		// private methods
		// ==============================================
		static void SetDataList(const std::shared_ptr<ICamera>& camera);
		static void AddShadowData(int subset_count, float distance, const std::shared_ptr<IRenderer>& renderer);
		static void AddMainData(int subset_count, float distance, const std::shared_ptr<IRenderer>& renderer);

		// Update Part
		static void UpdateMain(const std::shared_ptr<ICamera>& camera);
		static void UpdateShadow(const std::shared_ptr<ICamera>& camera);

		// --------------------------------------------------------------------------
		// Sort Func
	// --------------------------------------------------------------------------
		static bool SortMain_DoRender(const MainData& left, const MainData& right);
		static bool SortMain_Queue(const MainData& left, const MainData& right);
		static bool SortMain_Distance(const MainData& left, const MainData& right);
		static bool SortMain_Material(const MainData& left, const MainData& right);
		static bool SortMain_Mesh(const MainData& left, const MainData& right);
		static bool SortMain_Subset(const MainData& left, const MainData& right);
		static bool SortMain_Batching(const MainData& left, const MainData& right);

		static bool SortUI_Depth(const UIData& left, const UIData& right);
		static bool SortUI_Material(const UIData& left, const UIData& right);
		static bool SortUI_Texture(const UIData& left, const UIData& right);
		// --------------------------------------------------------------------------
		// ~Sort Func
		// --------------------------------------------------------------------------

		// ~Update Part

		static void CleanUpVector();
		static void ClearAndStore();

		// ==============================================
		// private variables
		// ==============================================
		static std::mutex m_mutex;

		// 存在するRenderer一括管理用リスト
		static std::list<std::shared_ptr<IRenderer>> m_renderer_list;

		// それぞれのレンダーパスごとにカリング、ソート、バッファとコマンドのセットをする
		static std::vector<MainData> m_shadow_path_list;
		static std::vector<MainData> m_main_path_list;
		static std::vector<UIData> m_UI_path_list;

		static int m_path_current_data_sizes[static_cast<int>(RenderPath::MAX_RENDER_PATH)];
		static int m_path_current_data_counts[static_cast<int>(RenderPath::MAX_RENDER_PATH)];
		static int m_path_prev_data_counts[static_cast<int>(RenderPath::MAX_RENDER_PATH)];

		// 描画時に使用するデータ情報
		static std::vector<DrawData> m_draw_data_list;
		static int m_current_draw_data_count;
		static int m_prev_draw_data_count;

		// インスタンスバッファ
		static VertexBuffer m_instance_buffer;

		// Viewport param
		static float m_current_vp_width;
		static float m_current_vp_height;
	};


}// namespace TKGEngine