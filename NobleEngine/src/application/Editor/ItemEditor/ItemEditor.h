#pragma once
#include <Game.h>
#include <memory>

class DataManager;

class ItemEditor
{
public:
	ItemEditor(DataManager* dataManager);
	~ItemEditor();

	void Update();
	void Draw();
	void DrawImGui();

private:
	DataManager* dataManager_;

	int32_t renderTextureID_ = -1;
	int32_t cameraID_ = -1;

	int32_t textureID_ = -1;

	Vector4 color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	
	bool fullscreen_ = false;

	EulerTransforms transforms_;

	std::unique_ptr<RenderObject> renderObject_;
};

