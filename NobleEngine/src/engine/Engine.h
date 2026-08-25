#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <memory>

class WindowManager;
class DirectXManager;
class StructuredBufferManager;
class DrawSystem;
class ComputeSystem;
class IOManager;
class CameraManager;
class ImGuiManager;
class AssetManager;
class FixFPS;
class EngineEditor;

class Engine
{
public:
	// 唯一のインスタンス取得
	static Engine& Instance();

	// コピー・ムーブ禁止
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;
	Engine(Engine&&) = delete;
	Engine& operator=(Engine&&) = delete;

	// メインループ用
	void Initialize(int32_t width, int32_t height, const std::wstring& title);
	bool ProcessMessage();
	void BeginFrame();
	void EndFrame();
	void Quit();
	void Finalize();

public:

	DirectXManager* GetDirectXManager() { return dxManager_.get(); }
	WindowManager* GetWindowManager() { return windowManager_.get(); }
	DrawSystem* GetDrawSystem() { return drawSystem_.get(); }
	ComputeSystem* GetComputeSystem() { return computeSystem_.get(); }
	IOManager* GetIOManager() { return ioManager_.get(); }
	AssetManager* GetAssetManager() { return assetManager_.get(); }
	CameraManager* GetCameraManager() { return cameraManager_.get(); }
	StructuredBufferManager* GetStructuredBufferManager() { return structuredBufferManager_.get(); }
	FixFPS* GetFixFPS() { return fixFPS_.get(); }

private:
	Engine() = default;
	~Engine() = default;

	// ウィンドウ関連
	std::unique_ptr<WindowManager> windowManager_;
	// DirectX関連
	std::unique_ptr<DirectXManager> dxManager_;
	// 
	std::unique_ptr<StructuredBufferManager> structuredBufferManager_;
	// 描画関連
	std::unique_ptr<DrawSystem> drawSystem_;
	// GPU計算関連
	std::unique_ptr<ComputeSystem> computeSystem_;
	// 入力関連
	std::unique_ptr<IOManager> ioManager_;
	// カメラ
	std::unique_ptr<CameraManager> cameraManager_;
	// ImGui
	std::unique_ptr<ImGuiManager> imguiManager_;
	// リソース管理
	std::unique_ptr<AssetManager> assetManager_;
	// フレームレート制御
	std::unique_ptr<FixFPS> fixFPS_;

	// エディタ
	std::unique_ptr<EngineEditor> engineEditor_;
};


/// <summary>
/// クラッシュ時にミニダンプを生成する関数
/// </summary>
/// <param name="exception">例外情報</param>
/// <returns>例外ハンドラの実行結果</returns>
LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception);
