#pragma once
#include <definition/definition.h>

class TextureBank;
class DirectXManager;

class TexturePreview
{
public:
	TexturePreview(DirectXManager* dxManager, TextureBank* bank);
	~TexturePreview();

	void Update();
	void Draw();
	void DrawImGui();

private:
	TextureBank* bank_;
	DirectXManager* dxManager_;

	int32_t textureID_ = 0;
	bool fullscreen_ = false;
	Vector2int windowSize_ = Vector2int( 512, 512 );
};

