#pragma once
#include <memory>
#include <cstdint>
#include <EngineDefinition/EngineDefinition.h>
#include <DrawSystem/RenderObject/RenderObject.h>

// UIモード
enum class UIScreenType
{
	Playing,	// プレイ中　(手持ちアイテムのみ表示)
	Inventory,	// インベントリ表示中
	Crafting,	// クラフト画面
	Pause,		// ポーズ画面
	MiningMode,	// 採掘モード選択画面
	EmptyScreen,	// なにもない画面

	MAX
};

enum class UIElementType
{
	Inventory,
	Hotbar,
	Craft,
	Pause,
	MiningMode,
	MiningPointGauge,

	MAX
};


struct ElementData
{
	std::unique_ptr<RenderObject> render;

	// テクスチャID
	int32_t textureID = -1;

	EulerTransforms transforms;
};