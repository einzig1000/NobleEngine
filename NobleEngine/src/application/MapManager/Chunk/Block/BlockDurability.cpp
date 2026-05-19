#include "MapManager/Chunk/Block/BlockDurability.h"
#include "Game.h"

BlockDurability::BlockDurability(){}
BlockDurability::~BlockDurability(){}

void BlockDurability::Update()
{
	// 破壊済みなら何もしない
	if (isDestroy_ == true) return;

	// 破壊中でないなら耐久値回復
	if (isBeingDestroyed_ == false)
	{
		nowDurability_ = maxDurability_;
	}

	// 破壊中フラグ初期化
	isBeingDestroyed_ = false;
}

void BlockDurability::DecreaseDurability(float power)
{
	// 掘る速度がFPSに依存しないようにする
	float deltaTime = Game::Time::GetDeltaTime();
	deltaTime *= 60.0f; // 60FPS基準に補正
	// 耐久値減少
	nowDurability_ -= power * deltaTime;
	// 破壊中
	isBeingDestroyed_ = true;
	// 破壊フレーム数増加
	destroyFrame_++;
	// 耐久値が0以下なら
	if (nowDurability_ <= 0.0f)
	{
		// 耐久値0
		nowDurability_ = 0.0f;
		// 破壊済みフラグ
		isDestroy_ = true;
	}
}

void BlockDurability::SetMaxDurability(float maxDurability)
{
	maxDurability_ = maxDurability;
	nowDurability_ = maxDurability_;

	if (nowDurability_ <= 0.0f)
	{
		isDestroy_ = true;
	}
	else
	{
		isDestroy_ = false;
	}
}

uint32_t BlockDurability::GetBreakStage() const
{
	// 0~5段階で返す
	if (isDestroy_) return 5;
	float ratio = static_cast<float>(nowDurability_) / static_cast<float>(maxDurability_);
	uint32_t result = static_cast<uint32_t>(6 - std::ceil(ratio * 6.0f));
	result = std::clamp(result, 0u, 5u);
	return result;
}
