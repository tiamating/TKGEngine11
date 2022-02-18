#pragma once

#include "Systems/inc/TKGEngine_Defined.h"
#include "Utility/inc/myfunc_vector.h"

#include <string>
#include <list>
#include <vector>
#include <unordered_map>

#ifdef USE_IMGUI
#pragma comment(lib, "libfbxsdk-md.lib")
#pragma comment(lib, "libxml2-md.lib")
#pragma comment(lib, "zlib-md.lib")
#include <fbxsdk.h>

namespace TKGEngine
{
	class FBXLoader
	{
	public:
		// ==============================================
		// public methods
		// ==============================================
		static bool LoadFromFile();


		// ==============================================
		// public variables
		// ==============================================
		/* nothing */


	private:
		enum class LOAD_STATE
		{
			INACTIVE,
			READY,
			BEGIN,
			SELECT,
			LOAD,
			END
		};

		struct FBX_Data
		{
			std::string fbx_filename;
			std::string fbx_filedir;
			std::string fbx_name;
			FbxManager* fbx_manager = nullptr;
			FbxScene* fbx_scene = nullptr;
			bool has_avatar = false;
			bool load_avatar = true;
			bool load_mesh = true;
			bool load_motion = true;

			struct MeshData
			{
				FbxNode* mesh_node = nullptr;
				bool is_selecting = false;
				bool is_skinned = false;

				MeshData(FbxNode* node, bool _is_selecting, bool _is_skinned)
					: mesh_node(node), is_selecting(_is_selecting), is_skinned(_is_skinned)
				{
					/* nothing */
				}
			};

			// メッシュノード名の探索順
			std::vector<std::string> mesh_names;
			// メッシュノード名, メッシュノードのデータ
			std::unordered_map<std::string, MeshData> mesh_selection;
			// モーションデータ名, ファイル書き出しの選択状態
			std::unordered_map<std::string, bool> motion_selection;
		};


		// ==============================================
		// private methods
		// ==============================================
		static void OnReady();
		static void OnBegin();
		static void OnSelect();
		static void OnLoad();
		static void OnEnd();

		// ==============================================
		// private variables
		// ==============================================
		static LOAD_STATE m_state;
		static std::vector<FBX_Data> m_fbx_data;
	};

}// namespace TKGEngine
#endif// #ifdef USE_IMGUI