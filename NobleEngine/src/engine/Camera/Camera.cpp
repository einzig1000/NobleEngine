#include "Camera.h"
#include <Game.h>
#include <Engine.h>
#include <IO/IOManager.h>
#include <IO/Mouse/MouseController.h>
#include <IO/Keyboard/KeyboardController.h>
#include <TimeManager/TimeManager.h>
#include <Window/WindowManager.h>
#include <algorithm>
#include <numbers>
#include <ImGuiManager/ImGuiManager.h>
#include <Utilities/Converter/CoordinateConverter/CoordinateConverter.h>
#include <Utilities/Easing/Easing.h>

Camera::Camera()
{
	enableControl_ = true;

	fovY_ = 0.65f;

	screenSize_ = Vector2{ float(WindowManager::winWidth_) , float(WindowManager::winHeight_) };

	sphericalEye_.radius = 20.0f;
	sphericalEye_.theta = std::numbers::pi_v<float> / 2.0f;
	sphericalEye_.phi = 0;

	//backToFrontMatrix_.m[0][0] = -1.00000000f;
	//backToFrontMatrix_.m[0][1] = 0.00000000f;
	//backToFrontMatrix_.m[0][2] = 8.74227766e-08f;
	//backToFrontMatrix_.m[0][3] = 0.00000000f;

	//backToFrontMatrix_.m[1][0] = 0.00000000f;
	//backToFrontMatrix_.m[1][1] = 1.00000000f;
	//backToFrontMatrix_.m[1][2] = 0.00000000f;
	//backToFrontMatrix_.m[1][3] = 0.00000000f;

	//backToFrontMatrix_.m[2][0] = -8.74227766e-08f;
	//backToFrontMatrix_.m[2][1] = 0.00000000f;
	//backToFrontMatrix_.m[2][2] = -1.00000000f;
	//backToFrontMatrix_.m[2][3] = 0.00000000f;

	//backToFrontMatrix_.m[2][0] = 0.00000000f;
	//backToFrontMatrix_.m[2][1] = 0.00000000f;
	//backToFrontMatrix_.m[2][2] = 0.00000000f;
	//backToFrontMatrix_.m[2][3] = 0.00000000f;
	backToFrontMatrix_ = Matrix4x4::MakeRotateYMatrix(3.14159265358979323846f);

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

void Camera::Update_Orbit()
{
	float dt = Engine::Instance().GetTimeManager()->GetScaledDeltaTimeMs();

	if (enableControl_)
	{
		// マウス移動量
		Vector2 mouseDelta = Game::IO::Mouse::Get2DPositionDelta();
		// マウスホイール
		int32_t mouseWheel = Game::IO::Mouse::GetWheel();
		// マウス中クリック
		bool isMiddleHeld = Game::IO::Mouse::IsHeld(2);
		// Shiftキー
		bool isShiftHeld = Game::IO::Key::IsHeld(VK_LSHIFT);


		// パン
		if (isMiddleHeld && isShiftHeld)
		{
			Matrix4x4 rot = Matrix4x4::MakeRotateXMatrix(sphericalEye_.phi) * Matrix4x4::MakeRotateYMatrix(-sphericalEye_.theta);
			Vector3 right = { rot.m[0][0], rot.m[1][0], rot.m[2][0] };
			Vector3 up = { rot.m[0][1], rot.m[1][1], rot.m[2][1] };

			float speed = sphericalEye_.radius * 0.02f;
			center_ += (right * mouseDelta.x + up * mouseDelta.y) * 0.1f * speed;
		}
		// 回転
		if (isMiddleHeld && !isShiftHeld)
		{
			sphericalEye_.phi -= mouseDelta.y * -0.01f;
			sphericalEye_.theta -= mouseDelta.x * 0.01f;
		}
		sphericalEye_.radius -= mouseWheel * sphericalEye_.radius * 0.001f;
	}

	// イージング
	MovingCenter(dt);
	MovingRotate(dt);
	MovingDistance(dt);
	MovingScreenSize(dt);
	MovingFov(dt);

	// 距離をクランプ
	if (sphericalEye_.radius < 0.1f) sphericalEye_.radius = 0.1f;

	// ピッチのクランプ
	sphericalEye_.phi = std::clamp(
		sphericalEye_.phi,
		-std::numbers::pi_v<float> / 2 + 0.001f,
		+std::numbers::pi_v<float> / 2 - 0.001f
	);

	// カメラ位置
	Vector3 localPos = CoordinateConverter::ToCartesian(sphericalEye_);
	eye_ = center_ + localPos;

	// 視線ベクトル
	cameraDirection_ = (center_ - eye_).Normalized();

	viewMatrix_ = Matrix4x4::LookAtMatrix(eye_, center_, { 0.0f, 1.0f, 0.0f });
	viewProjectionMatrix = viewMatrix_ * projectionMatrix_;

	billboardMatrix_ = backToFrontMatrix_ * viewMatrix_.Inverse();
	billboardMatrix_.m[3][0] = 0.0f; // 平行移動成分のリセット
	billboardMatrix_.m[3][1] = 0.0f;
	billboardMatrix_.m[3][2] = 0.0f;

	CreateFrustumPlanes();
}

// 移動(回転)するものをcenter_にして、カメラ座標を固定
void Camera::Update_FPS()
{

}

void Camera::Resize()
{
	aspect_ = screenSize_.x / screenSize_.y;
	projectionMatrix_ = Matrix4x4::MakePerspectiveFovMatrix(fovY_, aspect_, nearZ_, farZ_);
	viewportMatrix = Matrix4x4::MakeViewPortMatrix(0.0f, 0.0f, screenSize_.x, screenSize_.y, 0.0f, 1.0f);
	orthoProjectionMatrix_ = Matrix4x4::MakeOrthographicMatrix(0.0f, 0.0f, screenSize_.x, screenSize_.y, nearZ_, farZ_);
}

void Camera::Draw()
{
}

void Camera::DrawImGui()
{
	static const char* EaseTypeNames[] = {
	"LINEAR",
	"IN_SINE", "OUT_SINE", "IN_OUT_SINE",
	"IN_QUAD", "OUT_QUAD", "IN_OUT_QUAD",
	"IN_CUBIC", "OUT_CUBIC", "IN_OUT_CUBIC",
	"IN_QUART", "OUT_QUART", "IN_OUT_QUART",
	"IN_QUINT", "OUT_QUINT", "IN_OUT_QUINT",
	"IN_EXPO", "OUT_EXPO", "IN_OUT_EXPO",
	"IN_CIRC", "OUT_CIRC", "IN_OUT_CIRC",
	"IN_BACK", "OUT_BACK", "IN_OUT_BACK",
	"IN_ELASTIC", "OUT_ELASTIC", "IN_OUT_ELASTIC",
	"IN_BOUNCE", "OUT_BOUNCE"
	};

	std::string cameraTag = "##";
	std::string tag;

	std::string radiusTag = "##radius";
	tag = "radius" + radiusTag + cameraTag;
	ImGui::DragFloat(tag.c_str(), &sphericalEye_.radius, 0.1f);
	tag = "duration" + radiusTag + cameraTag;
	ImGui::DragFloat(tag.c_str(), &distanceEasing_.durationMs, 1, 0, 1000);
	tag = "target" + radiusTag + cameraTag;
	ImGui::DragFloat(tag.c_str(), &distanceEasing_.target, 0.1f);
	tag = "easeType" + radiusTag + cameraTag;
	int32_t easeType = static_cast<int32_t>(distanceEasing_.easetype);
	if (ImGui::Combo(tag.c_str(), &easeType, EaseTypeNames, IM_ARRAYSIZE(EaseTypeNames)))
	{
		distanceEasing_.easetype = static_cast<EaseType>(easeType);
	}
	tag = "start" + radiusTag + cameraTag;
	if (ImGui::Button(tag.c_str()))
	{
		distanceEasing_.durationMs *= 0.001f;
		SetDistanceTarget(distanceEasing_.target, distanceEasing_.durationMs, distanceEasing_.easetype);
	}

	std::string phiTag = "##phi";
	tag = "phi" + phiTag + cameraTag;
	ImGui::DragFloat(tag.c_str(), &sphericalEye_.phi, 0.01f);
	tag = "durationMs" + phiTag + cameraTag;
	ImGui::DragFloat(tag.c_str(), &rotateEasing_.durationMs, 1, 0, 1000);
	tag = "target" + phiTag + cameraTag;
	ImGui::DragFloat(tag.c_str(), &rotateEasing_.target.x, 0.01f);
	tag = "easeType" + phiTag + cameraTag;
	int32_t easeType2 = static_cast<int32_t>(rotateEasing_.easetype);
	if (ImGui::Combo(tag.c_str(), &easeType2, EaseTypeNames, IM_ARRAYSIZE(EaseTypeNames)))
	{
		rotateEasing_.easetype = static_cast<EaseType>(easeType2);
	}
	tag = "start" + phiTag + cameraTag;
	if (ImGui::Button(tag.c_str()))
	{
		rotateEasing_.durationMs *= 0.001f;
		SetRotateTarget(rotateEasing_.target, rotateEasing_.durationMs, rotateEasing_.easetype);
	}

	std::string thetaTag = "##theta";
	tag = "theta" + thetaTag + cameraTag;
	ImGui::DragFloat(tag.c_str(), &sphericalEye_.theta, 0.01f);
	tag = "durationMs" + thetaTag + cameraTag;
	ImGui::DragFloat(tag.c_str(), &rotateEasing_.durationMs, 1, 0, 1000);
	tag = "target" + thetaTag + cameraTag;
	ImGui::DragFloat(tag.c_str(), &rotateEasing_.target.y, 0.01f);
	tag = "easeType" + thetaTag + cameraTag;
	int32_t easeType3 = static_cast<int32_t>(rotateEasing_.easetype);
	if (ImGui::Combo(tag.c_str(), &easeType3, EaseTypeNames, IM_ARRAYSIZE(EaseTypeNames)))
	{
		rotateEasing_.easetype = static_cast<EaseType>(easeType3);
	}
	tag = "start" + thetaTag + cameraTag;
	if (ImGui::Button(tag.c_str()))
	{
		rotateEasing_.durationMs *= 0.001f;
		SetRotateTarget(rotateEasing_.target, rotateEasing_.durationMs, rotateEasing_.easetype);
	}

	std::string centerTag = "##Center";
	tag = "center" + centerTag + cameraTag;
	ImGui::DragFloat3(tag.c_str(), &center_.x, 0.1f);
	tag = "durationMs" + centerTag + cameraTag;
	ImGui::DragFloat(tag.c_str(), &centerEasing_.durationMs, 1, 0, 1000);
	tag = "target" + centerTag + cameraTag;
	ImGui::DragFloat3(tag.c_str(), &centerEasing_.target.x, 0.1f);
	tag = "easeType" + centerTag + cameraTag;
	int32_t easeType4 = static_cast<int32_t>(centerEasing_.easetype);
	if (ImGui::Combo(tag.c_str(), &easeType4, EaseTypeNames, IM_ARRAYSIZE(EaseTypeNames)))
	{
		centerEasing_.easetype = static_cast<EaseType>(easeType4);
	}
	tag = "start" + centerTag + cameraTag;
	if (ImGui::Button(tag.c_str()))
	{
		centerEasing_.durationMs *= 0.001f;
		SetCenterTarget(centerEasing_.target, centerEasing_.durationMs, centerEasing_.easetype);
	}

	ImGui::Separator();

	std::string screenSizeTag = "##screenSize";
	tag = "screenSize" + screenSizeTag + cameraTag;
	if (ImGui::DragFloat2(tag.c_str(), &screenSize_.x, 1.0f))
	{
		Resize();
	}
	tag = "durationMs" + screenSizeTag + cameraTag;
	ImGui::DragFloat(tag.c_str(), &screenSizeEasing_.durationMs, 1, 0, 1000);
	tag = "target" + screenSizeTag + cameraTag;
	ImGui::DragFloat2(tag.c_str(), &screenSizeEasing_.target.x, 1.0f);
	tag = "easeType" + screenSizeTag + cameraTag;
	int32_t easeType6 = static_cast<int32_t>(screenSizeEasing_.easetype);
	if (ImGui::Combo(tag.c_str(), &easeType6, EaseTypeNames, IM_ARRAYSIZE(EaseTypeNames)))
	{
		screenSizeEasing_.easetype = static_cast<EaseType>(easeType6);
	}
	tag = "start" + screenSizeTag + cameraTag;
	if (ImGui::Button(tag.c_str()))
	{
		screenSizeEasing_.durationMs *= 0.001f;
		SetScreenSizeTarget(screenSizeEasing_.target, screenSizeEasing_.durationMs, screenSizeEasing_.easetype);
	}

	std::string fovYTag = "##fovY";
	tag = "fovY" + fovYTag + cameraTag;
	if (ImGui::DragFloat(tag.c_str(), &fovY_, 0.01f))
	{
		projectionMatrix_ = Matrix4x4::MakePerspectiveFovMatrix(fovY_, aspect_, nearZ_, farZ_);
	}
	tag = "durationMs" + fovYTag + cameraTag;
	ImGui::DragFloat(tag.c_str(), &fovEasing_.durationMs, 1, 0, 1000);
	tag = "target" + fovYTag + cameraTag;
	ImGui::DragFloat(tag.c_str(), &fovEasing_.target, 0.01f);
	tag = "easeType" + fovYTag + cameraTag;
	int32_t easeType5 = static_cast<int32_t>(fovEasing_.easetype);
	if (ImGui::Combo(tag.c_str(), &easeType5, EaseTypeNames, IM_ARRAYSIZE(EaseTypeNames)))
	{
		fovEasing_.easetype = static_cast<EaseType>(easeType5);
	}
	tag = "start" + fovYTag + cameraTag;
	if (ImGui::Button(tag.c_str()))
	{
		fovEasing_.durationMs *= 0.001f;
		SetFovTarget(fovEasing_.target, fovEasing_.durationMs, fovEasing_.easetype);
	}



	std::string enableControlTag = tag + ".enableControl";
	ImGui::Checkbox(enableControlTag.c_str(), &enableControl_);
	ImGui::SameLine();
	ImGui::Text("enableControl");

	std::string cameraModeTag = tag + ".cameraMode";
	int32_t currentMode = static_cast<int32_t>(cameraMode_);
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
	for (int32_t i = 0; i < 6; ++i)
	{
		float length = sqrt(frustumPlanes_[i].normal.x * frustumPlanes_[i].normal.x +
			frustumPlanes_[i].normal.y * frustumPlanes_[i].normal.y +
			frustumPlanes_[i].normal.z * frustumPlanes_[i].normal.z);
		frustumPlanes_[i].normal = frustumPlanes_[i].normal / length;
		frustumPlanes_[i].distance /= length;
	}


}

bool Camera::InCamera(const AABB& aabb)
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
		int32_t inCount = 0;
		// AABBのすべての頂点が平面の裏側にあるかチェック
		for (int32_t i = 0; i < 8; ++i)
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

//Matrix4x4 Camera::GetViewProjectionMatrix()
//{
//	Vector3 localPos = CoordinateConverter::ToCartesian(sphericalEye_);
//	eye_ = center_ + localPos;
//
//	viewMatrix_ = Matrix4x4::LookAtMatrix(eye_, center_, { 0.0f, 1.0f, 0.0f });
//	viewProjectionMatrix = viewMatrix_ * projectionMatrix_;
//
//	return viewProjectionMatrix;
//}

// 実際に動かす
void Camera::MovingCenter(float dt)
{
	if (!centerEasing_.easeing) return;

	centerEasing_.elapsedMs += dt;

	center_ = Easing::EasingValue(
		centerEasing_.start,
		centerEasing_.target,
		centerEasing_.easetype,
		(centerEasing_.elapsedMs / centerEasing_.durationMs)
	);

	if (centerEasing_.elapsedMs > centerEasing_.durationMs)
	{
		centerEasing_.easeing = false;
	}
}
void Camera::MovingRotate(float dt)
{
	if (!rotateEasing_.easeing) return;

	rotateEasing_.elapsedMs += dt;

	Vector3 spherical = Easing::EasingValue(
		rotateEasing_.start,
		rotateEasing_.target,
		rotateEasing_.easetype,
		(rotateEasing_.elapsedMs / rotateEasing_.durationMs)
	);
	sphericalEye_.phi = spherical.x;
	sphericalEye_.theta = spherical.y;

	if (rotateEasing_.elapsedMs > rotateEasing_.durationMs)
	{
		rotateEasing_.easeing = false;
	}
}
void Camera::MovingDistance(float dt)
{
	if (!distanceEasing_.easeing) return;

	distanceEasing_.elapsedMs += dt;

	sphericalEye_.radius = Easing::EasingValue(
		distanceEasing_.start,
		distanceEasing_.target,
		distanceEasing_.easetype,
		(distanceEasing_.elapsedMs / distanceEasing_.durationMs)
	);

	if (distanceEasing_.elapsedMs > distanceEasing_.durationMs)
	{
		distanceEasing_.easeing = false;
	}
}
void Camera::MovingScreenSize(float dt)
{
	if (!screenSizeEasing_.easeing) return;

	screenSizeEasing_.elapsedMs += dt;

	screenSize_.x = Easing::EasingValue(
		screenSizeEasing_.start.x,
		screenSizeEasing_.target.x,
		screenSizeEasing_.easetype,
		(screenSizeEasing_.elapsedMs / screenSizeEasing_.durationMs)
	);
	screenSize_.y = Easing::EasingValue(
		screenSizeEasing_.start.y,
		screenSizeEasing_.target.y,
		screenSizeEasing_.easetype,
		(screenSizeEasing_.elapsedMs / screenSizeEasing_.durationMs)
	);

	if (screenSizeEasing_.elapsedMs > screenSizeEasing_.durationMs)
	{
		screenSizeEasing_.easeing = false;
	}

	Resize();
}
void Camera::MovingFov(float dt)
{
	if (!fovEasing_.easeing) return;

	fovEasing_.elapsedMs += dt;

	fovY_ = Easing::EasingValue(
		fovEasing_.start,
		fovEasing_.target,
		fovEasing_.easetype,
		(fovEasing_.elapsedMs / fovEasing_.durationMs)
	);

	if (fovEasing_.elapsedMs > fovEasing_.durationMs)
	{
		fovEasing_.easeing = false;
	}

	projectionMatrix_ = Matrix4x4::MakePerspectiveFovMatrix(fovY_, aspect_, nearZ_, farZ_);
}

// 動かす先の設定
void Camera::SetCenterTarget(Vector3 target, float durationSec, EaseType easetype)
{
	float durationMs = (durationSec > 0.0f) ? durationSec * 1000.0f : 0.001f;

	centerEasing_.start = center_;
	centerEasing_.target = target;
	centerEasing_.durationMs = durationMs;
	centerEasing_.elapsedMs = 0.0f;
	centerEasing_.easeing = true;
	centerEasing_.easetype = easetype;
}
void Camera::SetRotateTarget(Vector3 target, float durationSec, EaseType easetype)
{
	float durationMs = (durationSec > 0.0f) ? durationSec * 1000.0f : 0.001f;

	rotateEasing_.start = Vector3{ sphericalEye_.phi, sphericalEye_.theta, 0.0f };
	rotateEasing_.target = target;
	rotateEasing_.durationMs = durationMs;
	rotateEasing_.elapsedMs = 0.0f;
	rotateEasing_.easeing = true;
	rotateEasing_.easetype = easetype;
}
void Camera::SetDistanceTarget(float target, float durationSec, EaseType easetype)
{
	float durationMs = (durationSec > 0.0f) ? durationSec * 1000.0f : 0.001f;

	distanceEasing_.start = sphericalEye_.radius;
	distanceEasing_.target = target;
	distanceEasing_.durationMs = durationMs;
	distanceEasing_.elapsedMs = 0.0f;
	distanceEasing_.easeing = true;
	distanceEasing_.easetype = easetype;
}
void Camera::SetScreenSizeTarget(Vector2 target, float durationSec, EaseType easetype)
{
	float durationMs = (durationSec > 0.0f) ? durationSec * 1000.0f : 0.001f;

	screenSizeEasing_.start = screenSize_;
	screenSizeEasing_.target = target;
	screenSizeEasing_.durationMs = durationMs;
	screenSizeEasing_.elapsedMs = 0.0f;
	screenSizeEasing_.easeing = true;
	screenSizeEasing_.easetype = easetype;
}
void Camera::SetFovTarget(float target, float durationSec, EaseType easetype)
{
	float durationMs = (durationSec > 0.0f) ? durationSec * 1000.0f : 0.001f;

	fovEasing_.start = fovY_;
	fovEasing_.target = target;
	fovEasing_.durationMs = durationMs;
	fovEasing_.elapsedMs = 0.0f;
	fovEasing_.easeing = true;
	fovEasing_.easetype = easetype;
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
