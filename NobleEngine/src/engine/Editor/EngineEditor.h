#pragma once
#include <memory>

class WindowManager;

class DirectXManager;

class DrawSystem;

class IOManager;

class CameraManager;

class ResourceManager;
class ModelEditor;
class TextureEditor;

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
	
	WindowManager* windowManager_ = nullptr;
	
	DrawSystem* drawSystem_ = nullptr;
	
	IOManager* ioManager_ = nullptr;
	
	CameraManager* cameraManager_ = nullptr;

	ResourceManager* resourceManager_ = nullptr;
	std::unique_ptr<ModelEditor> modelEditor_;
	std::unique_ptr<TextureEditor> textureEditor_;

};

