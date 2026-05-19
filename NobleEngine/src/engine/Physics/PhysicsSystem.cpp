#include "Physics/PhysicsSystem.h"
#include "Game.h"
#include "Physics/IWorldCollider.h"
#include "Physics/IPhysicsBody.h"
#include "Physics/MapWorldCollider.h"

PhysicsSystem::PhysicsSystem()
{
	world_ = std::make_unique<MapWorldCollider>();
}

PhysicsSystem::~PhysicsSystem()
{}

void PhysicsSystem::AddWorldCollider(IWorldCollider* collider)
{
	world_->AddCollider(collider);
}

void PhysicsSystem::UnregisterDynamic(IPhysicsBody* model)
{
	dynamics_.erase(std::remove(dynamics_.begin(), dynamics_.end(), model), dynamics_.end());
}

void PhysicsSystem::Step()
{
    if (!world_) return;

	float dt = Game::Time::GetDeltaTime() * 60.0f;

    for (IPhysicsBody* b : dynamics_)
    {
		// 衝突判定前にAABB同期
		b->SyncAABBForSweep();
		const std::span<const AABB> aabbs = b->GetAABBs();
        if (aabbs.empty()) continue;

		// 1) 速度に加速度を加算
        b->IntegrateVelocity(dt);
        
		// 2) 理想移動量計算
        const Vector3 delta = b->GetVelocity() * dt;

		// 3) 実際移動量計算
        Vector3 corrected = delta;
        world_->SweepAABB(aabbs, delta, corrected);

        // 4) 位置反映
        b->ApplyTranslationDelta(corrected);

        // 5) 当たった軸の速度を0に
        const bool hitX = std::abs(corrected.x - delta.x) > eps;
        const bool hitY = std::abs(corrected.y - delta.y) > eps;
        const bool hitZ = std::abs(corrected.z - delta.z) > eps;

        Vector3 v = b->GetVelocity();
        if (hitX) v.x = 0.0f;
        if (hitZ) v.z = 0.0f;

        if (hitY)
        {
            if (delta.y < 0.0f && corrected.y > delta.y) v.y = 0.0f; // 落下で床
            else if (delta.y > 0.0f && corrected.y < delta.y) v.y = 0.0f; // 上昇で天井
        }

        b->SetVelocity(v);

        // 6) 最終同期
        b->SyncAfterMove();
    }
}