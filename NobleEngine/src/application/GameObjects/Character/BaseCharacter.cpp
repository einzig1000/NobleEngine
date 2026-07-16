#include "BaseCharacter.h"
#include <GameObjects/Map/MapManager.h>
#include <GameObjects/UI/UIManager.h>

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
	//haveItem_->AddItem(id);
}

void BaseCharacter::Jump()
{
	// 接地していないならジャンプ不可
	if (!isGrounded_) return;

	translate_.velocity.y = jumpPower_;
	translate_.acceleration.y = Constexprs::GRAVITY; // ジャンプ後は重力を戻す
	isGrounded_ = false;
}

void BaseCharacter::TakeDamage(int32_t damage)
{
	int32_t actualDamage = damage - defense_;
	if (actualDamage < 1) actualDamage = 1;
	HP_ -= actualDamage;
	if (HP_ < 0) HP_ = 0;
}

void BaseCharacter::Move(const Vector3& direction, float speed)
{
	translate_.velocity.x = direction.x * speed;
	translate_.velocity.z = direction.z * speed;
}


void BaseCharacter::UpdateLeftClick()
{
	//if (target_.type == RayHitResult::Type::Block)
	//{
	//	lookAtBlock lab = target_.blockHit;
	//	Block* block = lab.block;
	//	if (!block) return;
	//
	//	ItemID currentItemID = haveItem_->GetCurrentSelectedItemID();
	//
	//	breakPower_ = 1.0f;
	//	// ツールのジャンルとブロックのジャンルが合っている場合
	//	if (block->blockInfo_.genre == ItemGenre::Wood)
	//	{
	//		if (currentItemID == ItemID::木の斧)breakPower_ = 2.0f;
	//		if (currentItemID == ItemID::石の斧)breakPower_ = 3.0f;
	//		if (currentItemID == ItemID::鉄の斧)breakPower_ = 4.0f;
	//		if (currentItemID == ItemID::ダイヤの斧)breakPower_ = 15.0f;
	//	}
	//	else if (block->blockInfo_.genre == ItemGenre::Stone)
	//	{
	//		if (currentItemID == ItemID::木のツルハシ)breakPower_ = 2.0f;
	//		if (currentItemID == ItemID::石のツルハシ)breakPower_ = 3.0f;
	//		if (currentItemID == ItemID::鉄のツルハシ)breakPower_ = 4.0f;
	//		if (currentItemID == ItemID::ダイヤのツルハシ)breakPower_ = 15.0f;
	//	}
	//
	//	block->durability_->DecreaseDurability(breakPower_);
	//
	//	// 破壊されていたら非アクティブ化
	//	if (block->durability_->GetIsDestroy())
	//	{
	//		mapManager_->DestroyBlockAt(lab.chunkIndex, lab.localIndex);
	//	}
	//}
	//else if (target_.type == RayHitResult::Type::Character)
	//{
	//	BaseCharacter* targetCharacter = target_.Character;
	//	if (!targetCharacter) return;
	//	targetCharacter->TakeDamage(int32_t(attackPower_));
	//}
}

void BaseCharacter::UpdateRightClick()
{}


void BaseCharacter::SetNewBlock(BlockID id)
{
	//if (target_.type != RayHitResult::Type::Block) return;
	//
	//lookAtBlock lab = target_.blockHit;
	//
	//if (lab.block->blockInfo_.isExtraAction)
	//{
	//	uiManager_->ChangeScreen(UIMode::Crafting);
	//	// 特殊ブロックの上に置けない
	//	return;
	//}
	//
	//// 当たった面情報が無いなら置けない
	//if (lab.face == AABBFace::NONE) return;
	//
	//
	//// MapManagerから設置処理を呼び出す
	//mapManager_->SetBlockAt(lab, id);
	//
	//// アイテムを1つ消費
	//haveItem_->RemoveCurrentSelectedItem(1);
}
