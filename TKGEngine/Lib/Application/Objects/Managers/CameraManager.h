#pragma once

#include "../Components/interface/ICamera.h"

#include "Application/Resource/inc/Shader.h"
#include "Systems/inc/TKGEngine_Defined.h"
#include "Utility/inc/myfunc_vector.h"

#include <unordered_map>
#include <memory>
#include <mutex>


namespace TKGEngine
{
	class ICamera;
	class DebugCamera;

	/// <summary>
	/// Management Camera components.
	/// </summary>
	class CameraManager
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static bool Initialize();
		static void Terminate();
		static void Run();

		static CameraID RegisterManager(const std::shared_ptr<ICamera>& p_camera);
		static void UnregisterManager(CameraID id);

		static void OnResizeWindow();

#ifdef USE_IMGUI
		static MATRIX GetSceneView();
		static void SetSceneView(const MATRIX& view);
		static void SetSceneViewPosition(const VECTOR3& pos);
		static MATRIX GetSceneProjection();
		static std::shared_ptr<ICamera> GetDebugCamera();
#endif// #ifdef USE_IMGUI


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		// ==============================================
		// private methods
		// ==============================================
		static void SetVector();
		static void SortCameras();
		// Sort Func
		static bool SortFunc_Depth(const std::shared_ptr<ICamera>& l, const std::shared_ptr<ICamera>& r);
		// ~Sort Func

		static void PresentAndClear();
		static void RenderAndUpdateCameras();
#ifdef USE_IMGUI
		static void RenderAndUpdateDebugCamera();
#endif// #ifdef USE_IMGUI

		static void CleanUpVector();
		static void Clear();

		// FullscreenQuadÇ…èoóÕÇ∑ÇÈ
		static void OutputFullscreenQuad();


		// ==============================================
		// private variables
		// ==============================================
#ifdef USE_IMGUI
		static ImFont* m_assertion_font;
		static std::shared_ptr<DebugCamera> m_debug_camera;
#endif// #ifdef USE_IMGUI
		static std::unordered_map<CameraID, std::shared_ptr<ICamera>> m_camera_index;
		static int m_management_camera_cnt;
		static int m_current_id;
		static std::vector<std::shared_ptr<ICamera>> m_sorted_list;
		static int m_current_count;
		static int m_prev_count;
		static int m_current_size;

		// FullscreenQuadóp
		static Shader m_fullscreen_quad_shader;

		static std::mutex m_mutex;
	};
}// namespace TKGEngine