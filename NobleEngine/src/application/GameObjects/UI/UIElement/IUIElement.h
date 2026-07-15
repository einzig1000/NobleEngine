#pragma once
#include <Game.h>
#include <GameObjects/UI/UIData.h>

struct ElementData
{
	std::unique_ptr<RenderObject> render;

	// テクスチャID
	int32_t textureID = -1;

	EulerTransforms transforms;
};

class IUIElement
{
public:
	IUIElement() = default;
	virtual ~IUIElement() = default;
	// UI要素の初期化
	virtual void Initialize() = 0;
	// UI要素の更新
	virtual void Update(int32_t cameraID) = 0;
	// UI要素の描画
	virtual void Draw(int32_t rt_ID) = 0;

protected:
	std::vector<ElementData> sprites_;

	// このUIが表示されている時プレイヤーは操作可能か
	bool isPlayerControllable_ = true;
};

