#pragma once
#include <memory>
#include <vector>

class WindowManager;
class DirectXManager;
class DrawSystem;
class IOManager;
class CameraManager;
class AssetManager;
class TimeManager;

class IEditorPanel;

class EngineEditor
{
public:
	EngineEditor(
		WindowManager* windowManager,
		DirectXManager* dxManager,
		DrawSystem* drawSystem,
		IOManager* ioManager,
		CameraManager* cameraManager,
		AssetManager* assetManager,
		TimeManager* timeManager);
	~EngineEditor();

	void Initialize();
	void Update();
	void Draw();
	void DrawImGui();

private:
	std::vector<std::unique_ptr<IEditorPanel>> panels_;
};
