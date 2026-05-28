#include "Engine.h"
#include "Utilities/functions.h"
#include <cstdint>

#include <IO/IOManager.h>
#include <Window/WindowManager.h>
#include <DirectX/DirectXManager.h>
#include <Facade/Game.h>
#include <ResourceManager/ResourceManager.h>
#include <DrawSystem/DrawSystem.h>
#include <Camera/CameraManager.h>
#include <imGuiManager/ImGuiManager.h>
#include <FixFPS/FixFPS.h>
//#include <Physics/PhysicsSystem.h>

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
	resourceManager_ = std::make_unique<ResourceManager>(dxManager_.get());
	drawSystem_ = std::make_unique<DrawSystem>(dxManager_.get(), resourceManager_.get());
	cameraManager_ = std::make_unique<CameraManager>();
	ioManager_ = std::make_unique<IOManager>(windowManager_->GetHwnd(), cameraManager_.get());
	imguiManager_ = std::make_unique<ImGuiManager>();
	imguiManager_->Initialize(dxManager_.get(), windowManager_.get());
	//physicsSystem_ = std::make_unique<PhysicsSystem>();
	fixFPS_ = std::make_unique<FixFPS>();

	windowManager_->AttachMouseController(ioManager_->GetMouseController());
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
	// GPU同期
	//dxManager_->GetSynchronizationManager()->WaitForGPU();

	// imguiを更新
	imguiManager_->BeginFrame();

	// デバッグ情報更新
	UpdateDebugInfo();

	// 描画関数初期化
	drawSystem_->Reset();

	// DirectXを更新
	dxManager_->BeginFrame();

	// インプット系を更新
	ioManager_->Update();

	// カメラを更新	
	cameraManager_->Update();
}
void Engine::EndFrame()
{
	// 入力終了処理
	ioManager_->EndFrame();

	// 描画処理
	drawSystem_->SceneDraw();
	drawSystem_->PostEffectDraw();
	dxManager_->BeginRenderPass(dxManager_->GetSwapChain()->GetCurrentRenderTarget(), true);
	imguiManager_->EndFrame();
	drawSystem_->ScreenDraw();
	imguiManager_->Draw();
	dxManager_->EndRenderPass(dxManager_->GetSwapChain()->GetCurrentRenderTarget(), true, D3D12_RESOURCE_STATE_PRESENT);

	// DirectX終了処理
	dxManager_->EndFrame();

	// FPS制限
	fixFPS_->UpdateFixFPS();
}
void Engine::UpdateDebugInfo()
{
	if (Game::IO::Key::IsJustPressed(DIK_F1))
	{
		imguiManager_->ToggleDraw();
	}
	if (Game::IO::Key::IsJustPressed(DIK_F3))
	{
		cameraManager_->ToggleCamera();
	}

	cameraManager_->Draw();

	ImGui::Begin("------debug info------");
	ImGui::Text("ESC : Quit Application");
	ImGui::Text("F1  : Hide Debug Info");
	ImGui::Text("F3  : Toggle Camera Release or Debug");
	ImGui::Text("F5  : Toggle Camera FirstPerson or ThirdPerson");
	ImGui::Text("F12 : Toggle Fullscreen");
	ImGui::Text("DeltaTime: %.3f ms", fixFPS_->GetDeltaTime() * 1000.0f);
	ImGui::Text("FPS: %.1f ", fixFPS_->GetAverageFPS());
	ImGui::Text("ImGui FPS: %.1f ", ImGui::GetIO().Framerate);
	ImGui::End();
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

	// フレームレート制御
	fixFPS_.reset();
	// 物理演算
	//physicsSystem_.reset();
	// ImGui
	imguiManager_.reset();
	// 入力関連
	ioManager_.reset();
	// カメラ
	cameraManager_.reset();
	// 描画関連
	drawSystem_.reset();
	// リソース管理
	resourceManager_.reset();
	// DirectX関連
	dxManager_.reset();
	// ウィンドウ関連
	windowManager_.reset();

	// COMの終了処理
	CoUninitialize();
}

