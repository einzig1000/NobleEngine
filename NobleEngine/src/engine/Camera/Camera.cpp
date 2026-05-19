#include "Camera.h"
#include "Facade/Game.h"
#include "Window/WindowManager.h"
#include <algorithm>
#include <ImGuiManager/ImGuiManager.h>

Camera::Camera()
{
    enableControl_ = true;

    fovY_ = 0.65f;

    Resize();
}

Camera::~Camera()
{
}

void Camera::Update()
{
    switch (cameraMode_)
    {
    case CameraMode_ORBIT_FPS::ORBIT:
        Update_Orbit();
        break;
    case CameraMode_ORBIT_FPS::FPS:
        Update_FPS();
        break;
    default:
        break;
    }
}

// 移動(回転)するものをカメラ座標にして、center_を固定
void Camera::Update_Orbit()
{
#pragma region 入力取得

    // マウス移動量取得
    Vector2 mouseDelta = Game::IO::Mouse::GetPositionDelta();
    // マウスホイール取得
    int32_t mouseWheel = Game::IO::Mouse::GetWheel();
    // マウス中ボタンが押されているか
    bool isMiddleButtonDown = Game::IO::Mouse::IsHeld(2);
    // シフトキーが押されているか
    bool isShiftDown = Game::IO::Key::IsHeld(DIK_LSHIFT);
	// コントロールキーが押されているか
	bool isControlDown = Game::IO::Key::IsHeld(DIK_LCONTROL);

#pragma endregion

#pragma region カメラ回転

    if (isMiddleButtonDown && !isShiftDown)
    {
        Quaternion qx = Quaternion::MakeRotateAxisAngleQuaternion(Vector3(0, 1, 0), (mouseDelta.x * -0.01f));
        Quaternion qy = Quaternion::MakeRotateAxisAngleQuaternion(Vector3(1, 0, 0), (mouseDelta.y * -0.01f));
        rotate_ = qx * qy * rotate_;
    }

#pragma endregion

#pragma region カメラ中心移動

    if (isMiddleButtonDown && isShiftDown)
    {
        // スクリーン座標 → カメラ平面移動
        const float moveSpeed = distance_ * 0.002f;

        // カメラの右方向・上方向ベクトルを取得
        Vector3 right = Vector3(1, 0, 0).RotateByQuaternion(rotate_);
        Vector3 up = Vector3(0, 1, 0).RotateByQuaternion(rotate_);

        center_ += right * (mouseDelta.x * moveSpeed);
        center_ += up * (mouseDelta.y * moveSpeed);
    }

#pragma endregion

#pragma region カメラ距離移動

    if (isControlDown && mouseWheel != 0)
    {
		const float moveSpeed = distance_ * 0.05f;

		distance_ -= mouseWheel * moveSpeed * 0.01f;
    }

#pragma endregion

#pragma region カメラ行列計算

    // eye 計算
    Vector3 offset = Vector3(0, 0, distance_).RotateByQuaternion(rotate_);
    eye = center_ + offset;

    // view 行列
    Vector3 up = Vector3(0.0f, 1.0f, 0.0f).RotateByQuaternion(rotate_);
    viewMatrix_ = Matrix4x4::LookAtMatrix(eye, center_, up);

    //GetShakeOffset();

    // ビュー行列とプロジェクション行列を掛け合わせた行列を作成
    viewProjectionMatrix = viewMatrix_ * projectionMatrix_;


	Vector3 dir = center_ - eye;
	Vector3 angle = Game::Math::YawPitchFromDirection(dir);

    CreateFrustumPlanes();

#pragma endregion

}

// 移動(回転)するものをcenter_にして、カメラ座標を固定
void Camera::Update_FPS()
{

}

void Camera::Resize()
{
	aspect_ = float(WindowManager::winWidth_) / float(WindowManager::winHeight_);
    projectionMatrix_ = Matrix4x4::MakePerspectiveFovMatrix(fovY_, aspect_, nearZ_, farZ_);
    viewportMatrix = Matrix4x4::MakeViewPortMatrix(0.0f, 0.0f, float(WindowManager::winWidth_), float(WindowManager::winHeight_), 0.0f, 1.0f);
}

void Camera::Draw()
{
#ifdef DEBUG
    Game::DebugDraw::AddSphere({ center_, 0.5f }, 0xFF0000FF);
#endif // DEBUG
}

void Camera::DrawImGui()
{
    std::string tag = "##" + name_;

    ImGui::Text(name_.c_str());

	//std::string centerTag = tag + ".Center";
	//ImGui::DragFloat3(centerTag.c_str(), &center_.x, 0.01f);
	//ImGui::SameLine();
	//ImGui::Text("Center");

    // 緯度・経度
    
	std::string centerTag = ".Center" + tag;
	ImGui::DragFloat3(centerTag.c_str(), &center_.x, 0.1f);

	std::string distanceTag = ".Distance" + tag;
    ImGui::DragFloat(distanceTag.c_str(), &distance_, 0.1f);

    //Vector3 euler = rotate_.ToEuler();
    //if (ImGui::DragFloat3("Rotation (Euler)", &euler.x, 0.1f))
    //{
    //    rotate_ = Quaternion::MakeFromEuler(euler);
    //    rotate_ = rotate_.Normalize();
    //}

	ImGui::Separator();

	std::string fovYTag = tag + ".fovY";
    if (ImGui::DragFloat(fovYTag.c_str(), &fovY_, 0.01f))
    {
        projectionMatrix_ = Matrix4x4::MakePerspectiveFovMatrix(fovY_, aspect_, nearZ_, farZ_);
    }
    ImGui::SameLine();
	ImGui::Text("fovY");

	std::string aspectTag = tag + ".aspect";
    if (ImGui::DragFloat(aspectTag.c_str(), &aspect_, 0.01f))
    {
        projectionMatrix_ = Matrix4x4::MakePerspectiveFovMatrix(fovY_, aspect_, nearZ_, farZ_);
    }
    ImGui::SameLine();
	ImGui::Text("aspect");

	std::string nearZTag = tag + ".nearZ";
    if (ImGui::DragFloat(nearZTag.c_str(), &nearZ_, 0.01f, 0.01f, 10.0f))
    {
        projectionMatrix_ = Matrix4x4::MakePerspectiveFovMatrix(fovY_, aspect_, nearZ_, farZ_);
    }
    ImGui::SameLine();
	ImGui::Text("nearZ");

	std::string farZTag = tag + ".farZ";
    if (ImGui::DragFloat(farZTag.c_str(), &farZ_, 0.01f, 0.01f, 500.0f))
    {
        projectionMatrix_ = Matrix4x4::MakePerspectiveFovMatrix(fovY_, aspect_, nearZ_, farZ_);
    }
    ImGui::SameLine();
	ImGui::Text("farZ");

	std::string enableControlTag = tag + ".enableControl";
    ImGui::Checkbox(enableControlTag.c_str(), &enableControl_);
    ImGui::SameLine();
	ImGui::Text("enableControl");

	std::string cameraModeTag = tag + ".cameraMode";
	int currentMode = static_cast<int>(cameraMode_);
	static const char* items[] = { "ORBIT", "FPS" };
    if (ImGui::Combo(cameraModeTag.c_str(), &currentMode, items, IM_ARRAYSIZE(items)))
    {
        cameraMode_ = static_cast<CameraMode_ORBIT_FPS>(currentMode);
	}
    ImGui::SameLine();
	ImGui::Text("cameraMode");
}

void Camera::CreateFrustumPlanes()
{
    // Left Plane
    frustumPlanes_[0].normal.x = viewProjectionMatrix.m[0][3] + viewProjectionMatrix.m[0][0];
    frustumPlanes_[0].normal.y = viewProjectionMatrix.m[1][3] + viewProjectionMatrix.m[1][0];
    frustumPlanes_[0].normal.z = viewProjectionMatrix.m[2][3] + viewProjectionMatrix.m[2][0];
    frustumPlanes_[0].distance = viewProjectionMatrix.m[3][3] + viewProjectionMatrix.m[3][0];
    // Right Plane
    frustumPlanes_[1].normal.x = viewProjectionMatrix.m[0][3] - viewProjectionMatrix.m[0][0];
    frustumPlanes_[1].normal.y = viewProjectionMatrix.m[1][3] - viewProjectionMatrix.m[1][0];
    frustumPlanes_[1].normal.z = viewProjectionMatrix.m[2][3] - viewProjectionMatrix.m[2][0];
    frustumPlanes_[1].distance = viewProjectionMatrix.m[3][3] - viewProjectionMatrix.m[3][0];
    // Bottom Plane
    frustumPlanes_[2].normal.x = viewProjectionMatrix.m[0][3] + viewProjectionMatrix.m[0][1];
    frustumPlanes_[2].normal.y = viewProjectionMatrix.m[1][3] + viewProjectionMatrix.m[1][1];
    frustumPlanes_[2].normal.z = viewProjectionMatrix.m[2][3] + viewProjectionMatrix.m[2][1];
    frustumPlanes_[2].distance = viewProjectionMatrix.m[3][3] + viewProjectionMatrix.m[3][1];
    // Top Plane
    frustumPlanes_[3].normal.x = viewProjectionMatrix.m[0][3] - viewProjectionMatrix.m[0][1];
    frustumPlanes_[3].normal.y = viewProjectionMatrix.m[1][3] - viewProjectionMatrix.m[1][1];
    frustumPlanes_[3].normal.z = viewProjectionMatrix.m[2][3] - viewProjectionMatrix.m[2][1];
    frustumPlanes_[3].distance = viewProjectionMatrix.m[3][3] - viewProjectionMatrix.m[3][1];
    // Near Plane
    frustumPlanes_[4].normal.x = viewProjectionMatrix.m[0][2];
    frustumPlanes_[4].normal.y = viewProjectionMatrix.m[1][2];
    frustumPlanes_[4].normal.z = viewProjectionMatrix.m[2][2];
    frustumPlanes_[4].distance = viewProjectionMatrix.m[3][2];
    // Far Plane
    frustumPlanes_[5].normal.x = viewProjectionMatrix.m[0][3] - viewProjectionMatrix.m[0][2];
    frustumPlanes_[5].normal.y = viewProjectionMatrix.m[1][3] - viewProjectionMatrix.m[1][2];
    frustumPlanes_[5].normal.z = viewProjectionMatrix.m[2][3] - viewProjectionMatrix.m[2][2];
    frustumPlanes_[5].distance = viewProjectionMatrix.m[3][3] - viewProjectionMatrix.m[3][2];

    // 各平面を正規化
    for (int i = 0; i < 6; ++i)
    {
        float length = sqrt(frustumPlanes_[i].normal.x * frustumPlanes_[i].normal.x +
            frustumPlanes_[i].normal.y * frustumPlanes_[i].normal.y +
            frustumPlanes_[i].normal.z * frustumPlanes_[i].normal.z);
        frustumPlanes_[i].normal = frustumPlanes_[i].normal / length;
        frustumPlanes_[i].distance /= length;
    }


}

bool Camera::InFrustum(const AABB& aabb)
{
    Vector3 points[8];

    points[0] = Vector3{ aabb.min.x, aabb.min.y, aabb.min.z };
    points[1] = Vector3{ aabb.max.x, aabb.min.y, aabb.min.z };
    points[2] = Vector3{ aabb.max.x, aabb.max.y, aabb.min.z };
    points[3] = Vector3{ aabb.min.x, aabb.max.y, aabb.min.z };
    points[4] = Vector3{ aabb.min.x, aabb.min.y, aabb.max.z };
    points[5] = Vector3{ aabb.max.x, aabb.min.y, aabb.max.z };
    points[6] = Vector3{ aabb.max.x, aabb.max.y, aabb.max.z };
    points[7] = Vector3{ aabb.min.x, aabb.max.y, aabb.max.z };

    // 6つの各平面に対してテスト
    for (const auto& plane : frustumPlanes_)
    {
        int inCount = 0;
        // AABBのすべての頂点が平面の裏側にあるかチェック
        for (int i = 0; i < 8; ++i)
        {
            float dist = plane.normal.Dot(points[i]) + plane.distance;
            if (dist >= 0)
            {
                inCount++;
            }
        }
        // すべての頂点が平面の裏側にある場合は、AABBは視錐台の外
        if (inCount == 0)
        {
            return false;
        }
    }

    return true; // どの平面の外側にもない場合は、視錐台内にあると判定
}

// 実際に動かす
void Camera::MovingCenter()
{
}
void Camera::MovingRotate()
{
}
void Camera::MovingDistance()
{
}

// 動かす先の設定
void Camera::SetCenterTarget(Vector3 target, int spendFrame, EaseType easetype)
{
};
void Camera::SetRotateTarget(Vector3 target, int spendFrame, EaseType easetype)
{
};
void Camera::SetDistanceTarget(float target, int spendFrame, EaseType easetype)
{
}

// シェイク
void Camera::StartShake(float intensity, float duration, float frequency)
{
}

bool Camera::IsShaking() const
{
	return false;
}

void Camera::StopShake()
{
}

Vector3 Camera::GetShakeOffset() const
{
	return Vector3{ 0.0f, 0.0f, 0.0f };
}
