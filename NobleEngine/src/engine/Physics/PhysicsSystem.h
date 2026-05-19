#pragma once
#include <memory>
#include <vector>
#include "definition/definition.h"

class IWorldCollider;
class IPhysicsBody;
class MapWorldCollider;

/// <summary>
/// 物理演算システム
/// </summary>
///	毎フレームの全物理更新を行う最高指令部。
/// 動的オブジェクト（IPhysicsBody）をまとめて持ち、静的ワールド（MapWorldCollider）に問い合わせながら移動を確定する。
class PhysicsSystem
{
public:
    PhysicsSystem();
	~PhysicsSystem();

	// コライダーの追加
	void AddWorldCollider(IWorldCollider* collider);

    /// IPhysicsBodyの物理演算有効化
    void RegisterDynamic(IPhysicsBody* model) { dynamics_.push_back(model); }

    /// IPhysicsBodyの物理演算無効化
    void UnregisterDynamic(IPhysicsBody* model);

    /// 登録されている全てのIPhysicsBodyの物理演算無効化
    void ClearDynamics() { dynamics_.clear(); }

    void Step();

private:
	std::unique_ptr<MapWorldCollider> world_;
    std::vector<IPhysicsBody*> dynamics_;
};

 