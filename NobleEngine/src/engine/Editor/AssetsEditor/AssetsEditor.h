#pragma once
#include <Editor/IEditorPanel/IEditorPanel.h>
#include <memory>
#include <vector>

class DirectXManager;
class CameraManager;
class AssetManager;

class AssetsEditor : public IEditorPanel
{
public:
	AssetsEditor(DirectXManager* dxManager, CameraManager* cameraManager, AssetManager* assetManager);
	~AssetsEditor();

	void Update() override;
	void Draw() override;
	void DrawImGui() override;

private:
	std::vector<std::unique_ptr<IEditorPanel>> tabs_;

};
