#pragma once
#include <EngineDefinition/EngineDefinition.h>

/// <summary>
/// カメラの姿勢。ICameraStateが読み書きするのはこれだけ
/// </summary>
struct CameraPose
{
	// center から見た eye の向きと距離
	Coordinate_spherical spherical;
	// カメラ位置
	Vector3 eye = { 0.0f, 0.0f, 0.0f };
	// 注視点
	Vector3 center = { 0.0f, 0.0f, 0.0f };

	// centerからeyeへのずれ(ORBITはeye = center + GetOffset()、FPSは center = eye - GetOffset())
	Vector3 GetOffset() const;
	// 視線の向き(eyeからcenterへ長さ1)
	Vector3 GetForward() const;
	// 画面の右方向・上方向(平行移動に使う)
	Vector3 GetRight() const;
	Vector3 GetUp() const;
};
