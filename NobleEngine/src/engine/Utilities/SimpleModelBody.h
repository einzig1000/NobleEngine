#pragma once
#include <vector>
#include <span>
#include "definition/definition.h"
#include "Physics/IPhysicsBody.h"

/// <summary>
/// シンプル3Dモデル物理ボディ
/// </summary>
///「モデル・テクスチャ・座標」だけを持つ、最小の動的オブジェクト実装。
/// IPhysicsBody を実装し、PhysicsSystem に登録できる。
class SimpleModelBody : public IPhysicsBody
{
public:
    // 3Dモデル
    int32_t model = -1;
    // テクスチャ
    int32_t texture = -1;


    // 座標（value）と速度・加速度
    VectorDynamics translate;

    // ローカルAABB群（原点基準）
    std::vector<AABB> localAabbs;



    std::span<const AABB> GetAABBs() const override
    {
        return std::span<const AABB>(worldAabbs_);
    }

    void SyncAABBForSweep() override
    {
        worldAabbs_.clear();
        worldAabbs_.reserve(localAabbs.size());

        for (const auto& a : localAabbs)
        {
            AABB w = a + translate.value;
            w.Fix();
            worldAabbs_.push_back(w);
        }
    }

    void IntegrateVelocity(float dt) override
    {
        translate.velocity += translate.acceleration * dt;
    }

    Vector3 GetVelocity() const override
    {
        return translate.velocity;
    }

    void SetVelocity(const Vector3& v) override
    {
        translate.velocity = v;
    }

    void ApplyTranslationDelta(const Vector3& delta) override
    {
        translate.value += delta;

        // ワールドAABBも追従（次の処理で使われる可能性があるため）
        for (auto& a : worldAabbs_)
        {
            a.Move(delta);
        }
    }

    void SyncAfterMove() override
    {
        // 移動後も確実に整合させる
        SyncAABBForSweep();
    }

private:
    std::vector<AABB> worldAabbs_;
};

