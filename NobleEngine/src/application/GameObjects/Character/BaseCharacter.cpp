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

void BaseCharacter::ComputeViewRay()
{
	Vector3 cameraPos = Game::Camera::Getter::GetTranslate(c_viewCameraID_);
	Vector3 cameraCenter = Game::Camera::Getter::GetCenter(c_viewCameraID_);
	Vector3 cameraDir = (cameraCenter - cameraPos).Normalized();

	viewRay_.origin = translate_.value;
	viewRay_.diff = cameraDir;
}

std::optional<lookAtBlock> BaseCharacter::GetLookedAtBlock() const
{
	return mapManager_->GetBlockByCrossedRay(viewRay_, maxDistance);
}

AABB BaseCharacter::GetBlockWorldAABB(const Vector3int& chunkIndex, const Vector3int& localIndex) const
{
	return mapManager_->GetAABB(chunkIndex, localIndex);
}

void BaseCharacter::UpdateHaveItem(int32_t cameraID)
{
	haveItem_.SetParentWorldMatrix(worldMatrix_);
	haveItem_.SetItem(GetHaveItem());
	haveItem_.Update(cameraID);
}

void BaseCharacter::DrawHaveItem(int32_t renderTextureID)
{
	haveItem_.Draw(renderTextureID);
}

void BaseCharacter::Jump()
{
	// 接地していないならジャンプ不可
	if (!isGrounded_) return;

	translate_.velocity.y = jumpPower_;
	translate_.acceleration.y = Constexprs::GRAVITY; // ジャンプ後は重力を戻す
	isGrounded_ = false;
}

void BaseCharacter::ApplyMove()
{
	AABB aabb = aabb_;									// ワールド座標系でのプレイヤーのあたり判定を計算
	aabb.max = translate_.value + aabb.max;
	Vector3 aabbMin = aabb.min;
	aabbMin.y += Constexprs::kBlockSize * 0.5f;			// 足元の判定を少し上げる
	aabb.min = translate_.value + aabbMin;
	float dt = Game::Time::GetDeltaTime();
	translate_.velocity += translate_.acceleration;		// 加速度を速度に反映
	Vector3 movement = translate_.velocity;				// 移動量を計算
	mapManager_->SweepAABB(aabb, movement);				// mapManager_に希望移動量を申請し修正してもらう
	if (movement.y == 0.0f)isGrounded_ = true;			// 移動後の接地判定
	else isGrounded_ = false;
	translate_.value += movement;			// 移動
	if (isGrounded_ && translate_.velocity.y < -0.2f)
	{
		TakeDamage(1);
	}

	translate_.velocity = movement;						// 修正された移動量を速度に反映
	worldMatrix_ = Matrix4x4::MakeAffineMatrix(scale_.value, rotate_.value, translate_.value);
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




void BaseCharacter::SetBlock(BlockID id)
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

void BaseCharacter::DestroyBlockInAABB(const AABB& aabb)
{
	mapManager_->ReplaceBlockInAABB(aabb, BlockID::Air);
}

void BaseCharacter::DestroyBlockInOBB(const OBB& obb)
{
	mapManager_->ReplaceBlockInOBB(obb, BlockID::Air);
}

void BaseCharacter::RegisterToMap()
{
	mapManager_->RegisterCharacter(this);
}
