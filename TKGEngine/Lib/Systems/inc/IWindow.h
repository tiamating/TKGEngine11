#pragma once

#include "SystemAccessor.h"

#include <Windows.h>

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace TKGEngine
{
	class IWindow
		: public SystemAccessor<IWindow>
	{
	public:
		virtual RECT GetScreenRect() const = 0;
		virtual int GetScreenWidth() const = 0;
		virtual float GetScreenWidthf() const = 0;
		virtual int GetScreenHeight() const = 0;
		virtual float GetScreenHeightf() const = 0;
		virtual int GetScreenFrequency() const = 0;
		virtual HWND GetHwnd() const = 0;
		virtual DWORD GetDisplayFrequency() const = 0;
		virtual DWORD GetScreenResolutionWidth() const = 0;
		virtual DWORD GetScreenResolutionHeight() const = 0;
	};

}// namespace TKGEngine