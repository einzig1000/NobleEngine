#pragma once
#include <vector>
#include "definition/definition.h"

class Camera;

class CameraManager
{
public:
	CameraManager();
	~CameraManager();

	void AddCamera(const std::string name, bool enableControl);

	void Update();
	void Draw();
    void Resize();

    void SetCenterTarget(Vector3 Center, int spendFrame, EaseType easetype);
    void SetRotateTarget(Vector3 Center, int spendFrame, EaseType easetype);
    void SetDistanceTarget(float Center, int spendFrame, EaseType easetype);

    void StartShake(float intensity, float duration, float frequency = 25.0f);
    bool IsShaking();
    void StopShake();


	Vector3 GetCenter(const std::string name) const;			// カメラ回転中心
	Vector3 GetTranslate(const std::string name) const;			// カメラ位置
	Matrix4x4 GetViewProjectionMatrix(const std::string name) const; // ビュープロジェクション行列
	float GetDistance(const std::string name) const;			// カメラ距離

	Vector3 GetCurrentCenter() const;			// カメラ回転中心
	Vector3 GetCurrentTranslate() const;		// カメラ位置
	Vector3 GetCurrentRotate() const;
	Matrix4x4 GetCurrentViewProjectionMatrix() const; // ビュープロジェクション行列
	Matrix4x4 GetCurrentViewMatrix() const; // ビュー行列
	Matrix4x4 GetCurrentProjectionMatrix() const; // プロジェクション行列
	float GetCurrentDistance() const;			// カメラ距離
	void SetCameraMode(CameraMode_ORBIT_FPS mode);

	void SetEnableControl(bool enable);

	// 視錐台内にAABBがあるか
	bool InCamera(const AABB& aabb);

	void ToggleCamera();
	void ToggleCamera(const std::string name);

private:
	int currentCameraID_ = 0;

	std::vector<Camera> camera_;



};
