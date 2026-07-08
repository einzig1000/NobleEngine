#pragma once
#include "definition/definition.h"
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

	// チャンク描画オブジェクトのインスタンススロット
	int32_t dataSlot_ = -1;

	// ブロックの種類設定
	void SetBlockType(Blockinfo info);
	BlockID GetBlockID() const { return blockInfo_.type; };

	// ブロックの位置設定
	void SetBlockPosition(const Vector3& position);

	// プレイヤーに見られている時、輝度に応じて色を更新
	void UpdateColor();

	Vector3 position_;
	AABB aabb_;

	// 輝度
	int32_t lightEmission_ = 14;

	int32_t GetExposedFace() const { return exposedFace_; }
	bool IsExposed();
	bool IsExposed(AABBFace face) const;

	void SetExposedFace(AABBFace face, bool isExposed);

	Blockinfo blockInfo_;

private:
	// どの面が露出しているか
	// ビットフラグを使用
	// 0,0,前+z,後-z,左+x,右-x,上+y,下-y
	// 0b00000000(全て非露出), 0b00111111(全て露出)
	int32_t exposedFace_ = 0;
};