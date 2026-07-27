#pragma once
#include "definition/definition.h"
#include <memory>

class BlockDurability;

class Block
{
public:
	// ブロックの種類管理
	void SetBlockID(BlockID info);
	BlockID GetBlockID() const { return blockID_; };

	// 露出面管理
	int32_t GetExposedFace() const { return exposedFace_; }
	bool IsExposed();
	bool IsExposed(AABBFace face) const;
	void SetExposedFace(AABBFace face, bool isExposed);

private:

	// ブロックの種類
	BlockID blockID_ = BlockID::Air;

	// 輝度
	int32_t lightEmission_ = 14;

	// どの面が露出しているか
	// ビットフラグを使用
	// 0,0,前+z,後-z,左+x,右-x,上+y,下-y
	// 0b00000000(全て非露出), 0b00111111(全て露出)
	int32_t exposedFace_ = 0;
};