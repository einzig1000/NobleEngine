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
	resourceManager_ = std::make_unique<ResourceManager>(dxManager_->GetCommandContextManager()->GetCommandList(), dxManager_->GetDescriptorHeapManager(), dxManager_->GetDevice());
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
	dxManager_->GetSynchronizationManager()->WaitForGPU();

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

	// オフスクリーン描画用のRTV･DSVをセット
	dxManager_->PreSceneDraw();

	// シーン描画
	drawSystem_->SceneDraw();

	// オフスクリーン描画終了
	dxManager_->PostSceneDraw();

	// スクリーン描画用のRTVをセット
	dxManager_->PreScreenDraw();

	// スクリーン描画
	drawSystem_->ScreenDraw();

	// ImGui描画
	imguiManager_->EndFrame();
	imguiManager_->Draw();

	// スクリーン描画終了
	dxManager_->PostScreenDraw();

	// DirectX終了処理
	dxManager_->EndFrame();

	// FPS制限
	fixFPS_->UpdateFixFPS();
}
void Engine::UpdateTransforms()
{
	//if (RenderData_Model::renderModels.size() == 0)return;

	//if (Game::IO::Key::IsHeld(DIK_RSHIFT))
	//{
	//	if (!Game::IO::Key::IsJustPressed(DIK_RIGHTARROW))
	//	{
	//		return;
	//	}
	//}

#pragma region モデルリスト取得

	//const auto& modelList = RenderData_Model::renderModels;

#pragma endregion

#pragma region 座標更新 & 描画範囲内判定

	// オブジェクト更新
	//std::vector<ModelData> objects = resourceManager_->GetModelManager()->GetModelList();

	//for (auto& rd : modelList)
	//{
	//	//rd->Update1();
	//}
	//for (auto& rd : modelList)
	//{
	//	//rd->Update2();
	//}
	//for (auto& rd : modelList)
	//{
	//	// 座標更新,衝突ペア・深度の保存
	//	//rd->Update3();
	//}
	//for (auto& rd : modelList)
	//{
	//	// 衝突時の更新、それに伴う座標修正など
	//	//rd->Update4();
	//}
	//for (auto& rd : modelList)
	//{
	//	//rd->Update5();
	//}

#pragma endregion

#pragma region マウスレイ衝突判定

	//// マウスレイ取得
	//Ray mouseRay = ioManager_->GetMouseController()->GetRay();

	//// モデルと衝突までの距離セット構造体
	//struct HitInfo { RenderData_Model* rdm; float distance; };
	//// のリスト
	//std::vector<HitInfo> hits;
	//// のリサイズ(リサイズではない)
	//hits.reserve(modelList.size());

	//for (auto& rd : modelList)
	//{
	//	rd->isCollisionMouseRay = -1;
	//	// 描画範囲内なら判定
	//	if (rd->inPicture && rd->isCheckMouseRay)
	//	{
	//		// 最近接衝突点を取得
	//		std::optional<Vector3> colPos = IntersectRayModel(
	//			mouseRay,
	//			objects[rd->GetModel()].modelData.vertices, rd
	//		);
	//		// 衝突していたらリストに登録
	//		if (colPos)
	//		{
	//			float minDistance = (colPos.value() - mouseRay.origin).Length();
	//			hits.push_back({ rd, minDistance });
	//		}
	//	}
	//}

	//// 距離の昇順でソート
	//std::sort(hits.begin(), hits.end(),
	//	[](auto& a, auto& b) { return a.distance < b.distance; });

	//// ソート後に順序を割り当て
	//for (int order = 0; order < (int)hits.size(); ++order)
	//{
	//	hits[order].rdm->isCollisionMouseRay = order;
	//}

#pragma endregion

}
void Engine::UpdateParticles()
{
	// パーティクル更新
	//RenderData_Particle::UpdateAllParticles();
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
	if (Game::IO::Key::IsJustPressed(DIK_F12))
	{
		ToggleFullscreen();
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

// ウィンドウ操作
void Engine::ToggleFullscreen()
{
	windowManager_->ToggleFullscreen();

	// DirectXのリサイズ処理
	dxManager_->Resize();

	// カメラのアスペクト比を更新
	cameraManager_->Resize();
}
