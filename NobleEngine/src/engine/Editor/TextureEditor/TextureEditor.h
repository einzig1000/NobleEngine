#pragma once
#include <definition/definition.h>

class TextureBank;
class DirectXManager;

class TextureEditor
{
public:
	TextureEditor(DirectXManager* dxManager, TextureBank* bank);
	~TextureEditor();

	void Update();
	void Draw();
	void DrawImGui();

private:
	TextureBank* bank_;
	DirectXManager* dxManager_;

	int32_t textureID_;
	bool fullscreen_ = false;
};

