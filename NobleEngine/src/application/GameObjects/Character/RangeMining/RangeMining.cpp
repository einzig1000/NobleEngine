#include "RangeMining.h"
#include <GameObjects/Character/Player/Player.h>
#include <algorithm>
#include <optional>

RangeMining::RangeMining(Player* owner)
	: owner_(owner)
{}

void RangeMining::Update()
{
	if (!Game::IO::Mouse::IsJustPressed(0)) return;

	std::optional<lookAtBlock> hit = owner_->GetLookedAtBlock();
	if (!hit.has_value()) return;

	if (state_ == State::WaitingForStart)
	{
		startChunk_ = hit->chunkIndex;
		startLocal_ = hit->localIndex;
		state_ = State::WaitingForEnd;
		return;
	}

	// 2点目が確定したので範囲を確定して一括破壊する
	AABB startAABB = owner_->GetBlockWorldAABB(startChunk_, startLocal_);
	AABB endAABB = owner_->GetBlockWorldAABB(hit->chunkIndex, hit->localIndex);

	// どちらを先にクリックしても範囲になるよう軸ごとにmin/maxを取る
	Vector3 min(
		std::min(startAABB.min.x, endAABB.min.x),
		std::min(startAABB.min.y, endAABB.min.y),
		std::min(startAABB.min.z, endAABB.min.z));
	Vector3 max(
		std::max(startAABB.max.x, endAABB.max.x),
		std::max(startAABB.max.y, endAABB.max.y),
		std::max(startAABB.max.z, endAABB.max.z));

	owner_->DestroyBlockInAABB(AABB(min, max));

	state_ = State::WaitingForStart;
}
