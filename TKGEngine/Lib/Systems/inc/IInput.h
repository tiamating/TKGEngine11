#pragma once

#include "SystemAccessor.h"
#include "Input_Defined.h"

#include "Utility/inc/myfunc_vector.h"

namespace TKGEngine
{
	class IInput
		: public SystemAccessor<IInput>
	{
	public:
		// =================================
		// KeyBoard
		// =================================
		virtual bool GetKey(Input::Key key) const = 0;
		virtual bool GetKeyUp(Input::Key key) const = 0;
		virtual bool GetKeyDown(Input::Key key) const = 0;

		// =================================
		// GamePad
		// =================================
		virtual bool GetPad(int player, Input::GamePad button) const = 0;
		virtual bool GetPadUp(int player, Input::GamePad button) const = 0;
		virtual bool GetPadDown(int player, Input::GamePad button) const = 0;

		virtual float GetPadStickRX(int player) const = 0;
		virtual float GetPadStickRY(int player) const = 0;
		virtual float GetPadStickLX(int player) const = 0;
		virtual float GetPadStickLY(int player) const = 0;
		virtual float GetPadRTrigger(int player) const = 0;
		virtual float GetPadLTrigger(int player) const = 0;
		virtual bool SetVibration(
			int player, float seconds,
			float left_motor, float right_motor,
			float left_trigger = 0, float right_trigger = 0
		) = 0;
		virtual bool StopVibration(int player) = 0;
		virtual bool IsConnected(int player) const = 0;
		//virtual void SetPadDeadZone()

		// =================================
		// Mouse
		// =================================
		virtual VECTOR2 GetMousePos() const = 0;
		virtual int GetMousePosX() const = 0;
		virtual int GetMousePosY() const = 0;
		virtual int GetMouseWheel() const = 0;

		virtual void SetMousePos(const VECTOR2& point) = 0;
		virtual void SetMousePos(int x, int y) = 0;
		virtual void SetMousePosX(int x) = 0;
		virtual void SetMousePosY(int y) = 0;

		virtual bool GetMouseButton(Input::Mouse button) const = 0;
		virtual bool GetMouseButtonUp(Input::Mouse button) const = 0;
		virtual bool GetMouseButtonDown(Input::Mouse button) const = 0;

		virtual bool GetMouseVisible() const = 0;
		virtual void SetMouseVisible(bool is_visible) = 0;
	};

}// namespace TKGEngine