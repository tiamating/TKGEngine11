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
		// �q�G�����L�[�ł̑I�����Ɏg�p����֐��Q
		// ���N���b�N
		virtual void SetSelectionObject(GameObjectID goid) = 0;
		virtual void AddSelectionObject(GameObjectID goid) = 0;
		virtual void RemoveSelectionObject(GameObjectID goid) = 0;
		virtual void ClearSelectionObject() = 0;
		virtual const std::vector<GameObjectID>& GetSelectionObjects() const = 0; 
		virtual bool IsSelecting(GameObjectID goid) const = 0;
		// �E�N���b�N
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
		// �Q�[���V�[�������s����
		virtual bool IsPlaying() const = 0;
		// �Q�[���V�[�������傤�ǎ��s���ꂽ����
		virtual bool IsPlayedOnFrame() const = 0;
		// �Q�[���V�[�������傤�ǒ�~���ꂽ����
		virtual bool IsStoppedOnFrame() const = 0;

		// �V�[���r���[��`�悵�Ă��邩
		virtual bool IsSceneView() const = 0;
		// �͈͂̃f�o�b�O�`������邩
		virtual bool DoDrawDebugExtent() const = 0;
		// UI��`�悷�邩
		virtual bool DoDrawUI() const = 0;

		// Animator Controller��`�悷��悤�ɂ���
		virtual void SetDrawController(const GameObjectID goid, const AnimatorController& controller, const Avatar& avatar) = 0;

		// IInput�̓��͂��L���ł��邩
		virtual bool IsActiveInputScene() const = 0;
		// IInput�̓��̗͂L���A�������Z�b�g����
		virtual void SetActiveInputScene(bool is_active) = 0;

		// Global Setting��Save�{�^���������ꂽ��
		virtual bool IsPushSaveGlobalSetting() const = 0;
		// ~Scene
	};

}// namespace TKGEngine
#endif// #ifdef USE_IMGUI