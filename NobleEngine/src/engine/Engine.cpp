#include "Engine.h"
#include <Utilities/functions.h>
#include <IO/IOManager.h>
#include <Window/WindowManager.h>
#include <DirectX/DirectXManager.h>
#include <AssetManager/AssetManager.h>
#include <DrawSystem/DrawSystem.h>
#include <Camera/CameraManager.h>
#include <imGuiManager/ImGuiManager.h>
#include <FixFPS/FixFPS.h>
#include <Editor/EngineEditor.h>

using namespace DirectX;

Engine& Engine::Instance()
{
	static Engine instance; 
	return instance;
}

// 初期化用
void Engine::Initialize(int width, int height, const std::wstring& title)
{
	// COM の初期化
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));
	// 例外ハンドラの設定
	SetUnhandledExceptionFilter(ExportDump);

	windowManager_ = std::make_unique<WindowManager>(width, height, title);
	dxManager_ = std::make_unique<DirectXManager>(windowManager_->GetHwnd());
	cameraManager_ = std::make_unique<CameraManager>();
	imguiManager_ = std::make_unique<ImGuiManager>(dxManager_.get(), windowManager_.get());
	assetManager_ = std::make_unique<AssetManager>(dxManager_.get());
	ioManager_ = std::make_unique<IOManager>(windowManager_->GetHwnd(), cameraManager_.get());
	drawSystem_ = std::make_unique<DrawSystem>(dxManager_.get(), assetManager_.get());
	fixFPS_ = std::make_unique<FixFPS>();

	windowManager_->AttachMouseController(ioManager_->GetMouseController());

#ifdef _DEBUG
	engineEditor_ = std::make_unique<EngineEditor>(
		windowManager_.get(),
		dxManager_.get(),
		drawSystem_.get(),
		ioManager_.get(),
		cameraManager_.get(),
		assetManager_.get(),
		fixFPS_.get());
#endif
}

// メインループ用
bool Engine::ProcessMessage()
{
	MSG msg = {};
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}
void Engine::BeginFrame()
{
	// imguiを更新
	imguiManager_->BeginFrame();

	// 描画関数初期化
	drawSystem_->Reset();

	// DirectXを更新
	dxManager_->BeginFrame();

	// インプット系を更新
	ioManager_->Update();

	// デバッグモードの時のみ呼び出す
#ifdef _DEBUG
	engineEditor_->Update();
#endif
}

void Engine::EndFrame()
{
	// 入力終了処理
	ioManager_->EndFrame();

	// デバッグモードの時のみ呼び出す
#ifdef _DEBUG
	engineEditor_->Draw();
	engineEditor_->DrawImGui();
#endif

	// カメラのImGui描画
	cameraManager_->DrawImGui();

	// 描画処理
	drawSystem_->SceneDraw();
	drawSystem_->PostEffectDraw();
	drawSystem_->PreScreenDraw();

	dxManager_->BeginRenderPass(dxManager_->GetSwapChain()->GetCurrentRenderTarget(), true);
	drawSystem_->ScreenDraw();
	imguiManager_->EndFrame();
	imguiManager_->Draw();
	dxManager_->EndRenderPass(dxManager_->GetSwapChain()->GetCurrentRenderTarget(), true, D3D12_RESOURCE_STATE_PRESENT);

	// DirectX終了処理
	dxManager_->EndFrame();

	// FPS制限
	fixFPS_->UpdateFixFPS();
}
void Engine::Quit()
{
	windowManager_->Quit();
}

// 終了処理
void Engine::Finalize()
{
	dxManager_->GetSynchronizationManager()->WaitForGPU();

	// ImGuiの終了処理
	imguiManager_->Finalize();

	// エディタ
	engineEditor_.reset();
	// フレームレート制御
	fixFPS_.reset();
	// 描画関連
	drawSystem_.reset();
	// 入力関連
	ioManager_.reset();
	// アセット管理
	assetManager_.reset();
	// ImGui
	imguiManager_.reset();
	// カメラ
	cameraManager_.reset();
	// DirectX関連
	dxManager_.reset();
	// ウィンドウ関連
	windowManager_.reset();

	// COMの終了処理
	CoUninitialize();
}

