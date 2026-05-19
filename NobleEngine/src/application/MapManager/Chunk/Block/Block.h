#pragma once
#include "definition/definition.h"
#include "DrawSystem/RenderData/RenderData.h"
#include <memory>

class BlockDurability;

class Block
{
public:
	Block();
	~Block();
	void Initialize();
	// チャンク内の更新
	void Update();

	// RenderData_Blockのslot
	int32_t dataSlot_ = 0;

	// ブロックの種類設定
	void SetBlockType(Blockinfo info);
	BlockID GetBlockID() const { return blockInfo_.type; };

	// ブロックの位置設定
	void SetBlockPosition(const Vector3& position);

	// プレイヤーに見られている時、輝度に応じて色を更新
	void UpdateColor();


	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };
	Vector3 position_;
	AABB aabb_;

	// 輝度
	int32_t lightEmission_ = 14;

	// 耐久値
	std::unique_ptr<BlockDurability> durability_;

	// 表面に露出しているか
	bool isExposed_ = false;

	Blockinfo blockInfo_;
};