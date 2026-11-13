#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <Editor/IEditorPanel/IEditorPanel.h>

class TextureBank;
class DirectXManager;

class TexturePreview : public IEditorPanel
{
public:
	TexturePreview(DirectXManager* dxManager, TextureBank* bank);
	~TexturePreview();

	void Update() override;
	void Draw() override;
	void DrawImGui() override;

private:
	TextureBank* bank_;
	DirectXManager* dxManager_;

	int32_t textureID_ = -1;
	bool fullscreen_ = false;
	Vector2int windowSize_ = Vector2int(512, 512);

	// 選択中テクスチャの表示サイズを再計算する
	void UpdateWindowSizeFromTexture();
};

