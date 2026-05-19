#include "Engine.h"
#include "GameManager/GameManager.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#ifdef _DEBUG
	D3DResourceLeakChecker checker;
#endif

	// ウィンドウ、DirectX初期化
	Engine::Instance().Initialize(WIDTH, HEIGHT, L"MyCraft Builders");
	Engine::Instance().BeginFrame();
	std::unique_ptr<GameManager> gm = std::make_unique<GameManager>();
	Engine::Instance().EndFrame();
	while (Engine::Instance().ProcessMessage())
	{
		// フレームの開始
		Engine::Instance().BeginFrame();
		// ↓更新処理ここから
		gm->Update();
		// ↓描画処理ここから
		gm->Draw();
		// ↓ImGui描画処理ここから
		gm->DrawImGui();
		// フレームの終了
		Engine::Instance().EndFrame();
	}
	Engine::Instance().Finalize();
	return 0;
}