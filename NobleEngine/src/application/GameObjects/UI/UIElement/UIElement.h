#pragma once
#include <Game.h>

struct ElementData
{
	std::unique_ptr<RenderObject> render;

	// テクスチャID
	int32_t textureID = -1;

	EulerTransforms transforms;
};

class UIElement
{
public:
	UIElement() = default;
	virtual ~UIElement() = default;
	// UI要素の初期化
	virtual void Initialize() = 0;
	// UI要素の更新
	virtual void Update(int32_t cameraID) = 0;
	// UI要素の描画
	virtual void Draw(int32_t rt_ID) = 0;

protected:
	std::vector<ElementData> sprites_;
};

