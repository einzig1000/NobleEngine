#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <Camera/CameraPose.h>
#include <Camera/CameraState/ICameraState.h>
#include <array>
#include <memory>

class IOManager;

/// <summary>
/// カメラ機能そのもの
/// </summary>
class Camera
{
public:
    Camera(IOManager* ioManager);
    ~Camera();

    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera(Camera&&) noexcept = default;
    Camera& operator=(Camera&&) noexcept = default;

    void Update();
    void Draw();
    void DrawImGui();
    void Resize();

    void SetName(const std::string& name) { name_ = name; }
    std::string GetName() const { return name_; }

    // 動かす先の設定
    void SetCenterTarget(Vector3 target, float durationSec, EaseType easetype);
    void SetEyeTarget(Vector3 target, float durationSec, EaseType easetype);
    void SetPhiTarget(float target, float durationSec, EaseType easetype);
    void SetThetaTarget(float target, float durationSec, EaseType easetype);
    void SetDistanceTarget(float target, float durationSec, EaseType easetype);
    void SetScreenSizeTarget(Vector2 target, float durationSec, EaseType easetype);
    void SetFovTarget(float target, float durationSec, EaseType easetype);

    // シェイク
    void StartShake(float intensity, float duration, float frequency = 25.0f);
    bool IsShaking() const;
    void StopShake();

    // 視錐台内にAABBがあるか
    bool InCamera(const AABB& aabb);

    // 操作可能か設定
    void SetEnableControl(bool enable) { enableDebugControl_ = enable; }

    // カメラモードの切り替え
    void SetCameraMode(CameraMode_ORBIT_FPS mode);
    CameraMode_ORBIT_FPS GetCameraMode() const { return state_->GetMode(); }

public:
    // 情報取得
    Matrix4x4 GetViewProjectionMatrix() const { return viewProjectionMatrix; }
    Matrix4x4 GetViewMatrix() const { return viewMatrix_; }
    Matrix4x4 GetViewportMatrix() const { return viewportMatrix; }
    Matrix4x4 GetProjectionMatrix() const { return projectionMatrix_; }
    Matrix4x4 GetOrthoProjectionMatrix() const { return orthoProjectionMatrix_; }
    Matrix4x4 GetBillboardMatrix() const { return billboardMatrix_; }
    Vector3 GetCenter() const { return pose_.center; }
    Vector3 GetTranslate() const { return pose_.eye; }
    Vector3 GetCameraDirection() const { return cameraDirection_; }
    float GetDistance() const { return pose_.spherical.radius; }
    float GetPhi() const { return pose_.spherical.phi; }
    float GetTheta() const { return pose_.spherical.theta; }

private:
    Vector3 GetShakeOffset() const;

    // 球面座標を使える範囲に収める
    void ClampSpherical();
    // eye と center から行列と視錐台を作る
    void UpdateMatrices();

    // 1フレーム分の操作入力を集める
    CameraInput ReadCameraInput();

private:
	IOManager* ioManager_ = nullptr;


	// カメラモード
    std::unique_ptr<ICameraState> state_;

    // 名前
    std::string name_ = "Camera";

    // マウスでデバッグ操作可能か
    bool enableDebugControl_ = true;
	// イージングテストのImGuiを表示するか
	bool isDrawEasingTestImGui_ = false;


    // 姿勢(球面座標・カメラ位置・注視点)。状態クラスに渡すのはこれだけ
    CameraPose pose_;
    // カメラ方向
    Vector3 cameraDirection_ = { 0.0f, 1.0f, 0.0f };
    // スクリーンサイズ
    Vector2 screenSize_;


    /// 注視点
    void MovingCenter(float dt);
    EasingSet<Vector3> centerEasing_;
    /// カメラ位置
    void MovingEye(float dt);
    EasingSet<Vector3> eyeEasing_;
    /// 回転
    void MovingPhi(float dt);
    EasingSet<float> phiEasing_;
    void MovingTheta(float dt);
    EasingSet<float> thetaEasing_;
    /// カメラ距離
    void MovingDistance(float dt);
    EasingSet<float> distanceEasing_;
    /// 画面サイズ
    void MovingScreenSize(float dt);
    EasingSet<Vector2> screenSizeEasing_;
    /// FOV
    void MovingFov(float dt);
    EasingSet<float> fovEasing_;



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
    float farZ_ = 500.0f;

    // ビュープロジェクション行列
    Matrix4x4 viewProjectionMatrix;

    // ビルボード行列
    Matrix4x4 billboardMatrix_;
    Matrix4x4 backToFrontMatrix_;
};