#pragma once
#include "definition/definition.h"
#include <array>
#include <vector>
#include <string>
#include <wrl/client.h>
#include <memory>

class WindowManager;
class DirectXManager;
class DrawSystem;
class IOManager;
class CameraManager;
class ImGuiManager;
class AssetManager;
class FixFPS;
class RenderObject;
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
	void Initialize(int width, int height, const std::wstring& title);
	bool ProcessMessage();
	void BeginFrame();
	void EndFrame();
	void Quit();
	void Finalize();

public:

	DirectXManager* GetDirectXManager() { return dxManager_.get(); }
	WindowManager* GetWindowManager() { return windowManager_.get(); }
	DrawSystem* GetDrawSystem() { return drawSystem_.get(); }
	IOManager* GetIOManager() { return ioManager_.get(); }
	AssetManager* GetAssetManager() { return assetManager_.get(); }
	CameraManager* GetCameraManager() { return cameraManager_.get(); }
	FixFPS* GetFixFPS() { return fixFPS_.get(); }

private:
	Engine() = default;
	~Engine() = default;

	// ウィンドウ関連
	std::unique_ptr<WindowManager> windowManager_;
	// DirectX関連
	std::unique_ptr<DirectXManager> dxManager_;
	// 描画関連
	std::unique_ptr<DrawSystem> drawSystem_;
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