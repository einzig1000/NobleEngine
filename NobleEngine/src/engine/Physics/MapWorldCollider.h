#pragma once
#include "Physics/IWorldCollider.h"
#include "definition/definition.h"
#include <span>

class IWorldCollider;

/// <summary>
/// 全静的コライダー(IWorldCollider)管理 + AABBスイープ
/// </summary>
class MapWorldCollider
{
public:
    MapWorldCollider();

	/// <summary>
	/// コライダー追加
	/// </summary>
	void AddCollider(IWorldCollider* collider);

	/// <summary>
	/// AABBスイープ: delta を outCorrectedDelta に補正し、何かヒットしたら true
	/// </summary>
	/// <param name="aabb"> 移動するAABB </param>
	/// <param name="delta"> 移動量ベクトル </param>
	/// <param name="outCorrectedDelta"> 補正後の移動量ベクトル </param>
	/// <returns> 何かにヒットしたら true </returns>
	bool SweepAABB(std::span<const AABB> aabbs, const Vector3& delta, Vector3& outCorrectedDelta) const;

	// その座標に固体(当たり判定があるか)を返す
	bool isSolidAt(const Vector3& position) const;


private:
	std::vector<IWorldCollider*> colliders_;
};