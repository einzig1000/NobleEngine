#pragma once
#include <memory>

class WindowManager;

class DirectXManager;
class RenderTexturePreview;

class DrawSystem;

class IOManager;

class CameraManager;

class AssetManager;
class ModelPreview;
class TexturePreview;

class TimeManager;
class TimeEditor;

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
	std::unique_ptr<RenderTexturePreview> renderTexturePreview_;
	
	std::unique_ptr<ModelPreview> modelEditor_;
	std::unique_ptr<TexturePreview> textureEditor_;

	std::unique_ptr<TimeEditor> timeEditor_;
};

