#pragma once
#include "DrawSystem/RenderData/RenderData.h"

class BlockDurability
{
public:
	BlockDurability();
	~BlockDurability();

	void Update();
	void DecreaseDurability(float power);
	void SetIsCollisionRay(bool* isCollisionRay) { isCollisionRay_ = isCollisionRay; }

	void SetMaxDurability(float maxDurability);
	bool GetIsDestroy() const { return isDestroy_; }
	uint32_t GetBreakStage() const;

private:
	float nowDurability_ = 0;
	float maxDurability_ = 60;
	float destroyFrame_ = 0;

	
	bool isBeingDestroyed_ = false;	// 破壊中か
	bool isDestroy_ = false;		// 破壊済みか
	bool isJustDestroyed_ = false;	// 破壊された瞬間か

	
	bool* isCollisionRay_ = nullptr;// 視線レイと衝突しているかポインタ
};

