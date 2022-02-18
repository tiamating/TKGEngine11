#pragma once

#include "SystemAccessor.h"

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace TKGEngine
{
	class ITime
		: public SystemAccessor<ITime>
	{
	public:
		virtual void TimeScale(float scale) = 0;
		virtual float TimeScale() const = 0;
		virtual double TotalTime() const = 0;
		virtual double UnscaledTotalTime() const = 0;
		virtual float DeltaTime() const = 0;
		virtual float UnscaledDeltaTime() const = 0;
	};

}// namespace TKGEngine