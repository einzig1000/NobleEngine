#include "ICharacter.h"
#include <GameObjects/Map/MapManager.h>
#include <GameObjects/UI/UIManager.h>

// 見ているブロックをtargetBlock_にセットする
void ICharacter::SetTargetBlock()
{
	// 前フレームの見ているブロック保存
	preTarget_ = target_;
	// 見ているブロック取得
	target_ = mapManager_->GetTerrain()->GetFirstHitByRay(viewRay_, maxDistance, this);
}

void ICharacter::ComputeViewRay(int32_t cameraID)
{
	viewRay_.origin = translate_.value;
	viewRay_.diff = Game::Camera::Getter::GetCameraDirection(cameraID);
}

std::optional<LookAtBlock> ICharacter::GetLookedAtBlock() const
{
	return mapManager_->GetTerrain()->GetBlockByCrossedRay(viewRay_, maxDistance);
}

AABB ICharacter::GetBlockWorldAABB(const Vector3int& chunkIndex, const Vector3int& localIndex) const
{
	return mapManager_->GetTerrain()->GetAABB(chunkIndex, localIndex);
}

void ICharacter::PlayItemMotion(ItemMotion motion)
{
}

ItemMotion ICharacter::GetCurrentItemMotion() const
{
	return ItemMotion();
}

bool ICharacter::IsItemMotionFinished() const
{
	return false;
}

void ICharacter::UpdateHaveItem(int32_t cameraID)
{
	haveItem_.SetParentWorldMatrix(worldMatrix_);
	haveItem_.SetItem(GetHaveItem());
	haveItem_.Update(cameraID);
}

void ICharacter::DrawHaveItem(int32_t renderTextureID)
{
	haveItem_.Draw(renderTextureID);
}

void ICharacter::Jump()
{
	// 接地していないならジャンプ不可
	if (!isGrounded_) return;

	translate_.velocity.y = jumpPower_;
	translate_.acceleration.y = Constexprs::kGravity; // ジャンプ後は重力を戻す
	isGrounded_ = false;
}


void ICharacter::ApplyMove()
{
	AABB aabb = aabb_;									// ワールド座標系でのプレイヤーのあたり判定を計算
	aabb.max = translate_.value + aabb.max;
	Vector3 aabbMin = aabb.min;
	aabbMin.y += Constexprs::kBlockSize * 0.5f;			// 足元の判定を少し上げる
	aabb.min = translate_.value + aabbMin;

	const float dt = Game::Time::GetScaledDeltaTimeMs() * 0.001f;	// 秒単位のデルタタイム

	translate_.velocity += translate_.acceleration * dt;	// 加速度を速度に反映
	Vector3 movement = translate_.velocity * dt;			// 移動量を計算
	mapManager_->GetTerrain()->SweepAABB(aabb, movement);	// mapManager_に希望移動量を申請し修正してもらう
	if (movement.y == 0.0f)isGrounded_ = true;				// 移動後の接地判定
	else isGrounded_ = false;
	translate_.value += movement;			// 移動
	if (isGrounded_ && translate_.velocity.y < -12.0f)
	{
		TakeDamage(1);
	}

	translate_.velocity = movement / dt;							// 修正された移動量を速度(distance/sec)に戻す
	worldMatrix_ = Matrix4x4::MakeAffineMatrix(scale_.value, rotate_.value, translate_.value);
}

void ICharacter::TakeDamage(float damage)
{
	float actualDamage = damage - defense_;
	if (actualDamage < 1.0f) actualDamage = 1.0f;
	hp_ -= actualDamage;
	if (hp_ < 0.0f) hp_ = 0.0f;
}

void ICharacter::Move(const Vector3& direction, float speed)
{
	translate_.velocity.x = direction.x * speed;
	translate_.velocity.z = direction.z * speed;
}




void ICharacter::SetBlock(BlockID id)
{
	if (target_.type != RayHitResult::Type::Block) return;
	//
	//LookAtBlock lab = target_.blockHit;
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
void ICharacter::DestroyBlockInAABB(const AABB& aabb, float power)
{
	mapManager_->GetTerrain()->ReplaceBlockInAABB(aabb, BlockID::Air, power);
}
void ICharacter::DestroyBlockInOBB(const OBB& obb, float power)
{
	mapManager_->GetTerrain()->ReplaceBlockInOBB(obb, BlockID::Air, power);
}
void ICharacter::DestroyBlockInSphere(const Sphere& sphere, float power)
{
	mapManager_->GetTerrain()->ReplaceBlockInSphere(sphere, BlockID::Air, power);
}

void ICharacter::RegisterToMap()
{
	mapManager_->GetTerrain()->RegisterCharacter(this);
}
