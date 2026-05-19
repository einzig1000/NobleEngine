#include "Physics/MapWorldCollider.h"


MapWorldCollider::MapWorldCollider()
{}

void MapWorldCollider::AddCollider(IWorldCollider * collider)
{
	colliders_.push_back(collider);
}

bool MapWorldCollider::SweepAABB(std::span<const AABB> aabbs, const Vector3& delta, Vector3& outCorrectedDelta) const
{
	return false;
}

bool MapWorldCollider::isSolidAt(const Vector3& position) const
{
	for (const auto& collider : colliders_)
	{
		if (collider->IsSolidAt(position))
		{
			return true;
		}
	}
	return false;
}