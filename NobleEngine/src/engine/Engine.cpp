#include "Engine.h"
#include <Window/WindowManager.h>
#include <DirectX/DirectXManager.h>
#include <DrawSystem/DrawSystem.h>
#include <ComputeSystem/ComputeSystem.h>
#include <RootBinding/StructuredBufferManager/StructuredBufferManager.h>
#include <IO/IOManager.h>
#include <Camera/CameraManager.h>
#include <imGuiManager/ImGuiManager.h>
#include <AssetManager/AssetManager.h>
#include <FixFPS/FixFPS.h>
#include <Editor/EngineEditor.h>

#include <windows.h>
#include <DbgHelp.h>
#include <strsafe.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "Dbghelp.lib")


LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception)
{
	//時刻を取得して、時刻を名前に入れたファイルを作成。Dumpsディレクトリ以下に出力
	SYSTEMTIME time;
	GetLocalTime(&time);
	wchar_t filePath[MAX_PATH] = { 0 };
	CreateDirectory(L"./Dumps", nullptr);
	StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
	HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	// processId (このexeのId) とクラッシュ (例外)の発生したthreadIdを取得
	DWORD processId = GetCurrentProcessId();
	DWORD threadId = GetCurrentThreadId();
	// 設定情報を入力
	MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
	minidumpInformation.ThreadId = threadId;
	minidumpInformation.ExceptionPointers = exception;
	minidumpInformation.ClientPointers = TRUE;
	// Dumpを出力。MiniDumpNormalは最低限の情報を出力するフラグ
	MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);
	// 他に関連づけられているSEH例外ハンドラがあれば実行。通常はプロセスを終了する
	return EXCEPTION_EXECUTE_HANDLER;
}


Engine& Engine::Instance()
{
	static Engine instance; 
	return instance;
}

// 初期化用
void Engine::Initialize(int32_t width, int32_t height, const std::wstring& title)
{
	// COM の初期化
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));
	// 例外ハンドラの設定
	SetUnhandledExceptionFilter(ExportDump);

	windowManager_ = std::make_unique<WindowManager>(width, height, title);
	dxManager_ = std::make_unique<DirectXManager>(windowManager_->GetHwnd());

	// DirectXを更新
	dxManager_->BeginFrame();

	structuredBufferManager_ = std::make_unique<StructuredBufferManager>(dxManager_.get());
	imguiManager_ = std::make_unique<ImGuiManager>(dxManager_.get(), windowManager_.get());
	assetManager_ = std::make_unique<AssetManager>(dxManager_.get());
	cameraManager_ = std::make_unique<CameraManager>();
	ioManager_ = std::make_unique<IOManager>(windowManager_->GetHwnd());
	drawSystem_ = std::make_unique<DrawSystem>(dxManager_.get(), assetManager_.get());
	computeSystem_ = std::make_unique<ComputeSystem>(dxManager_.get(), structuredBufferManager_.get());
	fixFPS_ = std::make_unique<FixFPS>();

	windowManager_->AttachMouseController(ioManager_->GetMouseController());

//#ifdef _DEBUG
	engineEditor_ = std::make_unique<EngineEditor>(
		windowManager_.get(),
		dxManager_.get(),
		drawSystem_.get(),
		ioManager_.get(),
		cameraManager_.get(),
		assetManager_.get(),
		fixFPS_.get());
//#endif

	// DirectX終了処理
	dxManager_->EndFrame();
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

	// GPU計算システム初期化
	computeSystem_->Reset();

	// 描画システム初期化
	drawSystem_->Reset();

	// DirectXを更新
	dxManager_->BeginFrame();

	// 入出力系を更新
	ioManager_->Update();

	// オーディオの再生完了ボイスを回収
	assetManager_->Update();

	// デバッグモードの時のみ呼び出す
//#ifdef _DEBUG
	engineEditor_->Update();
//#endif
}

void Engine::EndFrame()
{
	// 入力終了処理
	ioManager_->EndFrame();

	// デバッグモードの時のみ呼び出す
//#ifdef _DEBUG
	engineEditor_->Draw();
	engineEditor_->DrawImGui();
//#endif

	// カメラのImGui描画
	cameraManager_->DrawImGui();

	// gpu計算処理
	computeSystem_->DispatchComputeObjects();

	// ReadBack処理
	structuredBufferManager_->FlushPendingReadbackRequests();


	// 描画処理
	drawSystem_->Execute();

	dxManager_->BeginRenderPass(dxManager_->GetSwapChain()->GetCurrentRenderTarget(), true);
	drawSystem_->ScreenDraw();
	imguiManager_->EndFrame();
	imguiManager_->Draw();
	dxManager_->EndRenderPass(dxManager_->GetSwapChain()->GetCurrentRenderTarget(), true, D3D12_RESOURCE_STATE_PRESENT);

	// DirectX終了処理
	dxManager_->EndFrame();

	// FPS制限
	fixFPS_->Update();
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
	// GPU計算関連
	computeSystem_.reset();
	// 描画関連
	drawSystem_.reset();
	// 入力関連
	ioManager_.reset();
	// カメラ
	cameraManager_.reset();
	// アセット管理
	assetManager_.reset();
	// ImGui
	imguiManager_.reset();
	// 
	structuredBufferManager_.reset();
	// DirectX関連
	dxManager_.reset();
	// ウィンドウ関連
	windowManager_.reset();

	// COMの終了処理
	CoUninitialize();
}

