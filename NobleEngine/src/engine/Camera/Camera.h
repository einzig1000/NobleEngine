#pragma once
#include "definition/definition.h"
#include <array>


// カメラは常にcenter_を見る
// ↳オービットの時はcenterを中心に、カメラが回転する。
//  FPSの時はカメラを中心に、centerが回転する
class Camera
{
public:
    Camera();
    ~Camera();

    void Update();
    void Draw();
	void DrawImGui();
    void Resize();

	// 動かす先の設定
    void SetCenterTarget(Vector3 target, int spendFrame, EaseType easetype);
    void SetRotateTarget(Vector3 target, int spendFrame, EaseType easetype);
    void SetDistanceTarget(float target, int spendFrame, EaseType easetype);

	// シェイク
    void StartShake(float intensity, float duration, float frequency = 25.0f);
    bool IsShaking() const;
    void StopShake();

    // 視錐台内にAABBがあるか
    bool InFrustum(const AABB& aabb);
    
	// 操作可能か設定
	void SetEnableControl(bool enable) { enableControl_ = enable; }

	void SetCameraMode(CameraMode_ORBIT_FPS mode) { cameraMode_ = mode; }

	// カメラ名
	std::string name_;


public:
	// 情報取得
	Matrix4x4 GetViewProjectionMatrix() const { return viewProjectionMatrix; }
    Matrix4x4 GetViewMatrix() const { return viewMatrix_; }
	Matrix4x4 GetViewportMatrix() const { return viewportMatrix; }
	Matrix4x4 GetProjectionMatrix() const { return projectionMatrix_; }
	Matrix4x4 GetOrthoProjectionMatrix() const { return orthoProjectionMatrix_; }
	Vector3 GetCenter() const { return center_; }
    Vector3 GetTranslate() const { return eye; }
    Vector3 GetRotate() const { return Vector3{}; }
	float GetDistance() const { return distance_; }

private:
    Vector3 GetShakeOffset() const;

	void Update_Orbit();
	void Update_FPS();

private:

    CameraMode_ORBIT_FPS cameraMode_ = CameraMode_ORBIT_FPS::ORBIT;

    // 操作可能か
    bool enableControl_;

	// 注視点
	Vector3 center_ = { 0.0f, 0.0f, 0.0f };
    // 注視点までの距離
	float distance_ = 30.0f;
    // カメラの向き
	Quaternion rotate_ = { 0.0f, 0.0f, 0.0f, 1.0f };
	// カメラの位置
	Vector3 eye = { 0.0f, 0.0f, -30.0f };

    /// カメラ回転
    void MovingCenter();


    /// 回転中心
    void MovingRotate();

    /// カメラ距離
    void MovingDistance();

    /// 視錐台判定用
    void CreateFrustumPlanes();
    std::array<Plane, 6> frustumPlanes_;// 視錐台を構成する6つの平面

	// ビューポート行列
    Matrix4x4 viewportMatrix;

	// ビュー行列
    Matrix4x4 viewMatrix_;

	// 正射影行列
	Matrix4x4 orthoProjectionMatrix_;

    /// プロジェクション行列関連データ
    Matrix4x4 projectionMatrix_;
	float fovY_ = 0.45f;
    float aspect_;
    float nearZ_ = 0.01f;
	float farZ_ = 1000.0f;

	// ビュープロジェクション行列
    Matrix4x4 viewProjectionMatrix;
};