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

Vector3 ICharacter::GetSpawnPosition(int32_t x, int32_t z) const
{
	//mapManager_->GetTerrain()->
	return Vector3{};
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
	const float dt = Game::Time::GetScaledDeltaTimeMs() * 0.001f;
	if (dt <= 0.0f) return;

	AABB aabb = aabb_ * scale_.value;	// ワールド座標でのプレイヤーのあたり判定を計算
	aabb = aabb + translate_.value;

	translate_.velocity += translate_.acceleration * dt;	// 加速度を速度に反映
	Vector3 movement = translate_.velocity * dt;			// 移動量を計算
	const float requestedY = movement.y;					// 修正前のY軸移動量
	const float stepUpHeight = stepUpBlockCount_ * Constexprs::kBlockSize;		// 自動で登れる段差の高さ
	const float stepDownHeight = stepDownBlockCount_ * Constexprs::kBlockSize;	// 吸着して降りられる段差の高さ
	mapManager_->GetTerrain()->SweepAABB(aabb, movement, stepUpHeight, stepDownHeight);	// mapManager_に希望移動量を申請し修正してもらう
	const bool hitY = (movement.y != requestedY);			// 床か天井に当たった(段差を登った・降りたときも含む)
	isGrounded_ = hitY && requestedY < 0.0f;				// 下へ動こうとして止められた = 接地
	translate_.value += movement;			// 移動
	if (isGrounded_ && translate_.velocity.y < -fallDamageSpeed_)
	{
		TakeDamage(1);
	}

	translate_.velocity = movement / dt;	// 移動量を速度に戻す
	if (hitY) translate_.velocity.y = 0.0f;
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
	//	uiManager_->ChangeScreen(UIScreenType::Crafting);
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
