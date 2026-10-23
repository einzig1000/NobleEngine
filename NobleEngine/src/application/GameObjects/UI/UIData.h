#pragma once
#include <memory>
#include <cstdint>
#include <string>
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





// ゲーム中不変のデータ。Jsonで保存するのはこいつ(textureID/textureSizeはLoad時にいれる)
struct ElementData
{
	// コードがパーツを探すための名前(同じ種類のパーツは同じ名前)。飾りなら空でよい
	std::string label;
	// 同じlabelの中での番号(0から)
	int32_t labelIndex = 0;

	// テクスチャ(パスが空なら描かない。文字の位置などの目印に使える)
	std::string texturePath;
	int32_t textureID = -1;
	Vector2 textureSize;

	// 位置。画面左上が0%、右下が100%
	Vector2 translateRate;
	// 絵のどこをtranslateRateに合わせるか(0,0 = 左上、0.5,0.5 = 中心、1,1 = 右下)
	Vector2 pivot = { 0.5f, 0.5f };
	// 1.0で絵と同じ大きさ
	Vector2 scale = { 1.0f, 1.0f };
	// 回転(ラジアン)。zが画面内の回転、x・yは奥に倒す回転
	Vector3 rotate;
};

// 実際の描画に使うデータ
struct ElementInstanceData
{
	ElementData data;

	std::unique_ptr<RenderObject> render;
	Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Matrix4x4 world;
	Matrix4x4 wvp;
};
