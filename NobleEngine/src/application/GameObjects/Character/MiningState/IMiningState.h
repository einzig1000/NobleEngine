#pragma once
#include <definition/definition.h>

/// <summary>
/// 採掘モードのインターフェース
/// </summary>
class IMiningState
{
public:
	virtual ~IMiningState() = default;

	// このモードに切り替わったとき(切り替えのたびに1回だけ呼ばれる)
	virtual void Enter() {}
	// 毎フレームの採掘処理
	virtual void Update() = 0;
	// ImGui表示
	virtual void DrawImGui() {}
	// どの採掘モードか
	virtual MiningPattern GetPattern() const = 0;
};
