// ==============================================================================
// 
// DirectX 11 Library
// 
// TKGEngine 1st
// 
// USE Library
// ImGUI, cereal, Effekseer, etc
// 
// ==============================================================================
#include "Systems/inc/Application.h"

#include <Windows.h>
#include <crtdbg.h>

int WINAPI wWinMain
(
	HINSTANCE instance,
	HINSTANCE prev_instance,
	LPWSTR cmd_line,
	INT cmd_show
)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	TKGEngine::Application app(L"TKGEngine", nullptr, nullptr);
	return app.Run();
}
