#pragma once

#include "SystemAccessor.h"
#include "Systems/inc/TKGEngine_Defined.h"

#ifdef USE_IMGUI
#include "Application/Objects/inc/IGameObject.h"
#include "Application/Resource/inc/AnimatorController.h"
#include "Application/Resource/inc/Avatar.h"
#include "Utility/inc/myfunc_vector.h"

#include <vector>


namespace TKGEngine
{
	class IGUI
		: public SystemAccessor<IGUI>
	{
	public:
		// ヒエラルキーでの選択時に使用する関数群
		// 左クリック
		virtual void SetSelectionObject(GameObjectID goid) = 0;
		virtual void AddSelectionObject(GameObjectID goid) = 0;
		virtual void RemoveSelectionObject(GameObjectID goid) = 0;
		virtual void ClearSelectionObject() = 0;
		virtual const std::vector<GameObjectID>& GetSelectionObjects() const = 0; 
		virtual bool IsSelecting(GameObjectID goid) const = 0;
		// 右クリック
		virtual void SetRightClickObject(GameObjectID goid) = 0;
		virtual void ClearRightClickObject() = 0;
		virtual GameObjectID GetRightClickObject() const = 0;

		// Draw Shape
		virtual void DrawLine(const VECTOR3& from, const VECTOR3& to, const VECTOR3& from_color, const VECTOR3& to_color) = 0;
		virtual void DrawSphere(const VECTOR3& pos, const float radius, const VECTOR3& color) = 0;
		virtual void DrawBox(const VECTOR3& pos, const Quaternion& rot, const VECTOR3& half_extents, const VECTOR3& color) = 0;
		virtual void DrawCapsule(const VECTOR3& pos, const Quaternion& rot, const float radius, const float height, const VECTOR3& color) = 0;
		virtual void DrawCone(const VECTOR3& pos, const Quaternion& rot, const float radius, const float height, const VECTOR3& color) = 0;
		virtual void DrawContactPoint(const VECTOR3& pos, const VECTOR3& normal, const float distance, const VECTOR3& color) = 0;
		// ~Draw Shape

		// Gizmo
		virtual void EnterControlTransform() = 0;
		virtual void ExitControlTransform() = 0;
		// ~Gizmo

		// Scene
		// ゲームシーンを実行中か
		virtual bool IsPlaying() const = 0;
		// ゲームシーンをちょうど実行された時か
		virtual bool IsPlayedOnFrame() const = 0;
		// ゲームシーンをちょうど停止された時か
		virtual bool IsStoppedOnFrame() const = 0;

		// シーンビューを描画しているか
		virtual bool IsSceneView() const = 0;
		// 範囲のデバッグ描画をするか
		virtual bool DoDrawDebugExtent() const = 0;
		// UIを描画するか
		virtual bool DoDrawUI() const = 0;

		// Animator Controllerを描画するようにする
		virtual void SetDrawController(const GameObjectID goid, const AnimatorController& controller, const Avatar& avatar) = 0;

		// IInputの入力が有効であるか
		virtual bool IsActiveInputScene() const = 0;
		// IInputの入力の有効、無効をセットする
		virtual void SetActiveInputScene(bool is_active) = 0;

		// Global SettingのSaveボタンが押されたか
		virtual bool IsPushSaveGlobalSetting() const = 0;
		// ~Scene
	};

}// namespace TKGEngine
#endif// #ifdef USE_IMGUI