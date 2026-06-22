#pragma once
#include <memory>

class WindowManager;

class DirectXManager;
class RenderTexturePreview;

class DrawSystem;

class IOManager;

class CameraManager;

class ResourceManager;
class ModelPreview;
class TexturePreview;

class EngineEditor
{
public:
	EngineEditor(
		WindowManager* windowManager, 
		DirectXManager* dxManager, 
		DrawSystem* drawSystem, 
		IOManager* ioManager, 
		CameraManager* cameraManager, 
		ResourceManager* resourceManager);
	~EngineEditor();

	void Initialize();
	void Update();
	void Draw();
	void DrawImGui();

private:
	DirectXManager* dxManager_ = nullptr;
	std::unique_ptr<RenderTexturePreview> renderTexturePreview_;
	
	WindowManager* windowManager_ = nullptr;
	
	DrawSystem* drawSystem_ = nullptr;
	
	IOManager* ioManager_ = nullptr;
	
	CameraManager* cameraManager_ = nullptr;

	ResourceManager* resourceManager_ = nullptr;
	std::unique_ptr<ModelPreview> modelEditor_;
	std::unique_ptr<TexturePreview> textureEditor_;

};

