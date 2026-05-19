#pragma once
#include "Game.h"

/// <summary>
/// UI要素基底クラス
/// 複数のスプライトで構成されたUI
/// 例：アイテムホットバー、
/// </summary>
class UIElement
{
public:
	UIElement() = default;
	virtual ~UIElement() = default;
	// UI要素の初期化
	virtual void Initialize() = 0;
	// UI要素の更新
	virtual void Update() = 0;
	// UI要素の描画
	virtual void Draw();

protected:
	std::vector<std::unique_ptr<RenderData_Sprite>> sprites_;
};

