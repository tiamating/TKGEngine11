#pragma once

#include "SystemAccessor.h"
#include "Systems/inc/Graphics_Defined.h"

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace TKGEngine
{
	class IGraphics
		: public SystemAccessor<IGraphics>
	{
	public:
		virtual ID3D11Device* Device() const = 0;
		virtual ID3D11DeviceContext* IC() const = 0;
		virtual ID3D11DeviceContext* DC(int thread_idx, Graphics::DC_RENDER_PATH path_idx) const = 0;
		virtual ID3D11DeviceContext* DC(int thread_idx, Graphics::DC_COPY_PATH path_idx) const = 0;
		virtual ID3D11DeviceContext* DC(int thread_idx, Graphics::DC_COMPUTE_PATH path_idx) const = 0;

		virtual void Present(int syncinterval) = 0;
		virtual void SetViewPort(ID3D11DeviceContext* p_context, float width, float height, float top_left_x = 0.0f, float top_left_y = 0.0f) = 0;

		virtual void SetRTBackBuffer(ID3D11DeviceContext* p_context, bool set_depth) = 0;
		virtual void ClearBackBuffer() = 0;
	};

}// namespace TKGEngine