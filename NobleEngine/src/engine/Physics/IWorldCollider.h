#pragma once
#include "definition/definition.h"

/// <summary>
/// 静的オブジェクト側：インターフェース
/// </summary>
/// マップや建物など「動かない側」の当たり判定を、種類問わず統一して扱うための共通窓口。
/// MapWorldCollider が複数の静的コライダーをまとめるために使う。
class IWorldCollider
{
public:
	virtual ~IWorldCollider() = default;

	// その座標に固体(当たり判定があるか)を返す
    virtual bool IsSolidAt(const Vector3& position) const = 0;
};

