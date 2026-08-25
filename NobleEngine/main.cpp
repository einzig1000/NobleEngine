#include "Engine.h"
#include "Application.h"
#include <definition/definition.h>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#ifdef _DEBUG
	D3DResourceLeakChecker checker;
#endif

	// ウィンドウ、DirectX初期化
	Engine::Instance().Initialize(1280, 720, L"MyCraft Builders");
	Engine::Instance().BeginFrame();
	Application::Instance().Initialize();
	Engine::Instance().EndFrame();
	while (Engine::Instance().ProcessMessage())
	{
		//// フレームの開始
		Engine::Instance().BeginFrame();
		// ↓更新処理ここから
		Application::Instance().Update();
		// ↓描画処理ここから
		Application::Instance().Draw();
		// ↓ImGui描画処理ここから
		Application::Instance().DrawImGui();
		// フレームの終了
		Engine::Instance().EndFrame();
	}
	Application::Instance().Finalize();
	Engine::Instance().Finalize();
	return 0;
}