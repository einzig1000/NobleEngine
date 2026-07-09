#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <DrawSystem/RenderData/RenderObject.h>
#include <memory>

class DirectXManager;

class RenderTexturePreview
{
public:
	RenderTexturePreview(DirectXManager* dxManager);
	~RenderTexturePreview();
	void Update();
	void Draw();
	void DrawImGui();

private:
	DirectXManager* dxManager_ = nullptr;
	std::unique_ptr<RenderObject> renderObject_;
	UINT64 gpuDescriptorHandlePtr_ = 0;
	size_t renderTargetIndex_ = 22193211;
	bool fullscreen_ = false;
	Vector2int windowSize_ = Vector2int(512, 512);
};

