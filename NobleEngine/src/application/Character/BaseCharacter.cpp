#include "BaseCharacter.h"
#include "MapManager/MapManager.h"
#include "MapManager/Chunk/Chunk.h"
#include "MapManager/Chunk/Block/Block.h"
#include "MapManager/Chunk/Block/BlockDurability.h"
#include "UIManager/UIManager.h"

// 見ているブロックをtargetBlock_にセットする
void BaseCharacter::SetTargetBlock()
{
	// 前フレームの見ているブロック保存
	preTarget_ = target_;
	// 見ているブロック取得
	target_ = mapManager_->GetFirstHitByRay(viewRay_, maxDistance, this);
}


void BaseCharacter::AddItem(ItemID id)
{
	haveItem_->AddItem(id);
}

void BaseCharacter::UpdateAttackPower()
{
	ItemID currentItemID = haveItem_->GetCurrentSelectedItemID();
	if (currentItemID == ItemID::木の剣)attackPower_ = 2.0f;
	if (currentItemID == ItemID::石の剣)attackPower_ = 3.0f;
	if (currentItemID == ItemID::鉄の剣)attackPower_ = 4.0f;
	if (currentItemID == ItemID::ダイヤの剣)attackPower_ = 15.0f;
}

void BaseCharacter::UpdateGrounded()
{
	if (!mapManager_)
	{
		isGrounded_ = false;
		return;
	}

	const Vector3 center = data_.aabbs[0].center();
	const float halfHeight = data_.scale.value.y * 0.5f;

	// 足元の少し下をサンプル
	const float probe = 0.06f;
	Vector3 foot = center;
	foot.y = center.y - halfHeight - probe;

	// 左右(4点)も見ると段差で安定する
	const float r = data_.scale.value.x * 0.45f;
	const Vector3 probes[5] =
	{
		foot,
		foot + Vector3{ +r, 0, +r },
		foot + Vector3{ -r, 0, +r },
		foot + Vector3{ +r, 0, -r },
		foot + Vector3{ -r, 0, -r },
	};

	bool grounded = false;
	for (auto& p : probes)
	{
		if (mapManager_->isSolidAt(p))
		{
			grounded = true;
			break;
		}
	}

	isGrounded_ = grounded;

	// 接地中は落下速度を止める（沈み防止）
	if (isGrounded_ && data_.translate.velocity.y < 0.0f)
	{
		data_.translate.velocity.y = 0.0f;
	}
}

void BaseCharacter::SetMapManager(MapManager* mapManager)
{
	mapManager_ = mapManager;
}

void BaseCharacter::SetUIManager(UIManager* uiManager)
{
	uiManager_ = uiManager;
}

void BaseCharacter::Jump()
{
	// 接地していないならジャンプ不可
	if (!isGrounded_) return;

	data_.translate.velocity.y = jumpPower_;
	data_.translate.acceleration.y = GRAVITY; // ジャンプ後は重力を戻す
	isGrounded_ = false;
}

void BaseCharacter::TakeDamage(int32_t damage)
{
	int32_t actualDamage = damage - defense_;
	if (actualDamage < 1) actualDamage = 1;
	HP_ -= actualDamage;
	if (HP_ < 0) HP_ = 0;

	data_.color = { 1.0f, 0.0f, 0.0f, 1.0f }; // ダメージ受けたら赤くする
}

void BaseCharacter::Move(const Vector3& direction, float speed)
{
	data_.translate.velocity.x = direction.x * speed;
	data_.translate.velocity.z = direction.z * speed;
}


void BaseCharacter::UpdateLeftClick()
{
	if (target_.type == RayHitResult::Type::Block)
	{
		lookAtBlock lab = target_.blockHit;
		Block* block = lab.block;
		if (!block) return;

		ItemID currentItemID = haveItem_->GetCurrentSelectedItemID();

		breakPower_ = 1.0f;
		// ツールのジャンルとブロックのジャンルが合っている場合
		if (block->blockInfo_.genre == ItemGenre::Wood)
		{
			if (currentItemID == ItemID::木の斧)breakPower_ = 2.0f;
			if (currentItemID == ItemID::石の斧)breakPower_ = 3.0f;
			if (currentItemID == ItemID::鉄の斧)breakPower_ = 4.0f;
			if (currentItemID == ItemID::ダイヤの斧)breakPower_ = 15.0f;
		}
		else if (block->blockInfo_.genre == ItemGenre::Stone)
		{
			if (currentItemID == ItemID::木のツルハシ)breakPower_ = 2.0f;
			if (currentItemID == ItemID::石のツルハシ)breakPower_ = 3.0f;
			if (currentItemID == ItemID::鉄のツルハシ)breakPower_ = 4.0f;
			if (currentItemID == ItemID::ダイヤのツルハシ)breakPower_ = 15.0f;
		}

		block->durability_->DecreaseDurability(breakPower_);

		// 破壊されていたら非アクティブ化
		if (block->durability_->GetIsDestroy())
		{
			mapManager_->DestroyBlockAt(lab.chunkIndex, lab.localIndex);
		}
	}
	else if (target_.type == RayHitResult::Type::Character)
	{
		BaseCharacter* targetCharacter = target_.Character;
		if (!targetCharacter) return;
		targetCharacter->TakeDamage(int32_t(attackPower_));
	}
}


void BaseCharacter::SetNewBlock(BlockID id)
{
	if (target_.type != RayHitResult::Type::Block) return;

	lookAtBlock lab = target_.blockHit;

	if (lab.block->blockInfo_.isExtraAction)
	{
		uiManager_->ChangeScreen(UIMode::Crafting);
		// 特殊ブロックの上に置けない
		return;
	}

	// 当たった面情報が無いなら置けない
	if (lab.face == AABBFace::NONE) return;


	// MapManagerから設置処理を呼び出す
	mapManager_->SetBlockAt(lab, id);

	// アイテムを1つ消費
	haveItem_->RemoveCurrentSelectedItem(1);
}
