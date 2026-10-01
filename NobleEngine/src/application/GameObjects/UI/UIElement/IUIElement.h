#pragma once
#include <Game.h>
#include <GameObjects/UI/UIData.h>

class EventBus;
class ItemInventory;
class MiningPointGauge;
class ItemIconManager;

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

	virtual void DrawImGui() = 0;

	virtual void SetEventBus(EventBus* eventBus) { eventBus_ = eventBus; }
	virtual void SetInventory(const ItemInventory* inventory) { inventory_ = inventory; }
	virtual void SetMiningGauge(const MiningPointGauge* miningGauge) { miningGauge_ = miningGauge; }
	virtual void SetIconManager(ItemIconManager* iconManager) { iconManager_ = iconManager; }
	virtual void SetNextUIMode(UIMode* nextUIMode) { nextUIMode_ = nextUIMode; }

protected:
	EventBus* eventBus_ = nullptr;
	UIMode* nextUIMode_ = nullptr;

	const MiningPointGauge* miningGauge_ = nullptr;
	const ItemInventory* inventory_ = nullptr;
	ItemIconManager* iconManager_ = nullptr;

	std::vector<ElementData> sprites_;
};