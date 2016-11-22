#include "pch.h"
#include "MultiMeshDemo.h"

using namespace Library;
using namespace MultiMesh;
using namespace std;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showCommand)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	UNREFERENCED_PARAMETER(previousInstance);
	UNREFERENCED_PARAMETER(commandLine);

	SetCurrentDirectory(UtilityWin32::ExecutableDirectory().c_str());

	ThrowIfFailed(CoInitializeEx(nullptr, COINITBASE_MULTITHREADED), "Error initializing COM.");

	const wstring windowClassName = L"Box2DSpriteDemoClass";
	const wstring windowTitle = L"Box2D Sprite Demo";
	const SIZE RenderTargetSize = { 1024, 768 };

	HWND windowHandle;
	WNDCLASSEX window;
	UtilityWin32::InitializeWindow(window, windowHandle, instance, windowClassName, windowTitle, RenderTargetSize, showCommand);

	auto getRenderTargetSize = [&](SIZE& renderTargetSize)
	{
		renderTargetSize = RenderTargetSize;
	};

	auto getWindow = [&]() -> void*
	{
		return reinterpret_cast<void*>(windowHandle);
	};

	MultiMeshDemo game(getWindow, getRenderTargetSize);
	game.UpdateRenderTargetSize();
	game.Initialize();

	MSG message = { 0 };

	try
	{
		while (message.message != WM_QUIT)
		{
			if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
			else
			{
				game.Run();
			}
		}
	}
	catch (GameException ex)
	{
		MessageBox(windowHandle, ex.whatw().c_str(), windowTitle.c_str(), MB_ABORTRETRYIGNORE);
	}

	game.Shutdown();
	UnregisterClass(windowClassName.c_str(), window.hInstance);

	CoUninitialize();

	return static_cast<int>(message.wParam);
}