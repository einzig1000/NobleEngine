#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <Editor/IEditorPanel/IEditorPanel.h>
#include <DrawSystem/RenderObject/RenderObject.h>
#include <memory>
#include <optional>

class DirectXManager;

class RenderTexturePreview : public IEditorPanel
{
public:
	RenderTexturePreview(DirectXManager* dxManager);
	~RenderTexturePreview();
	void Update() override;
	void Draw() override;
	void DrawImGui() override;

private:
	DirectXManager* dxManager_ = nullptr;
	std::unique_ptr<RenderObject> renderObject_;
	UINT64 gpuDescriptorHandlePtr_ = 0;
	std::optional<size_t> renderTargetIndex_ = std::nullopt;
	//size_t renderTargetIndex_ = 22193211;
	bool fullscreen_ = false;
	Vector2int windowSize_ = Vector2int(512, 512);
};

