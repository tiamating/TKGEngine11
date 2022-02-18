#pragma once

#if defined(DEBUG) || (_DEBUG)
#endif// (DEBUG) || (_DEBUG)

// ---------------------------
// ImGUI
// ---------------------------
// If use ImGUI
#define USE_IMGUI

#ifdef USE_IMGUI
#include <imgui.h>
#include <ImGuizmo.h>
#include "Utility/inc/myfunc_imgui.h"
#endif// #ifdef USE_IMGUI
// ---------------------------

// ---------------------------
// cereal
// ---------------------------
// Cereal type.
//#define SAVE_JSON

#include <cereal/cereal.hpp>
#include <cereal/access.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/list.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/unordered_set.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

#include <iostream>
#include <sstream>
#include <fstream>
// ---------------------------

// ---------------------------
// cereal file extensions
// ---------------------------
namespace TKGEngine
{
	constexpr const char* GAMEOBJECT_EXTENSION = ".object";
	constexpr const char* MESH_EXTENSION = ".mesh";
	constexpr const char* MATERIAL_EXTENSION = ".material";
	constexpr const char* AVATAR_EXTENSION = ".avatar";
	constexpr const char* MOTION_EXTENSION = ".motion";
	constexpr const char* ANIMATOR_CONTROLLER_EXTENSION = ".controller";
	constexpr const char* SCENE_EXTENSION = ".scene";
}
// ---------------------------

// ---------------------------
// World Setting
// ---------------------------
// Axis system.
#define AXIS_RH



// ---------------------------
// Library ID
// ---------------------------
namespace TKGEngine
{
	/// <summary>
	/// Starting at 1. 0 is initial value.
	/// </summary>
	///	Unique Instance ID
	typedef unsigned int InstanceID;

	// ïsê≥Ç»IDÇÃíl
	static constexpr InstanceID INVALID_ID = 0;

	/// <summary>
	///	Unique GameObject ID
	/// Starting at 1. 0 is initial value.
	/// </summary>
	typedef InstanceID GameObjectID;

	/// <summary>
	///	Unique ComponentType ID
	/// Starting at 1. 0 is initial value.
	/// </summary>
	typedef InstanceID ComponentTypeID;

	/// <summary>
	///	Unique Scene ID
	/// -2 : Invalid ID. -1 : Don't Destroy. Starting at 0.
	/// </summary>
	typedef int SceneID;
}
// ---------------------------