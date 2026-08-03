#pragma once

// UIモード
enum class UIMode
{
	None,		// 非表示
	Playing,	// プレイ中　(手持ちアイテムのみ表示)
	Inventory,	// インベントリ表示中
	Crafting,	// クラフト画面
	Pause,		// ポーズ画面
	MiningMode,	// 採掘モード選択画面

	Max
};

enum class UIElementType
{
	None,
	Inventory,
	Hotbar,
	Craft,
	Pause,
	MiningMode,
	Max
};