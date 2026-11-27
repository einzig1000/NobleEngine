#include "Camera.h"
#include <Camera/CameraState/OrbitCameraState/OrbitCameraState.h>
#include <Camera/CameraState/FpsCameraState/FpsCameraState.h>
#include <Engine.h>
#include <IO/IOManager.h>
#include <IO/Mouse/MouseController.h>
#include <IO/Keyboard/KeyboardController.h>
#include <TimeManager/TimeManager.h>
#include <Window/WindowManager.h>
#include <algorithm>
#include <numbers>
#include <ImGuiManager/ImGuiManager.h>
#include <Utilities/Easing/Easing.h>

namespace
{
	// center に近づける距離の下限
	constexpr float kMinDistance = 0.1f;
	// 真上・真下を向かないようにする余裕(ラジアン)
	constexpr float kPhiMargin = 0.001f;

	// 操作モードに合った状態クラスを作る(enum から型を選ぶのはここだけ)
	std::unique_ptr<ICameraState> CreateCameraState(CameraMode_ORBIT_FPS mode)
	{
		switch (mode)
		{
		case CameraMode_ORBIT_FPS::FPS:
			return std::make_unique<FpsCameraState>();
		case CameraMode_ORBIT_FPS::ORBIT:
		default:
			return std::make_unique<OrbitCameraState>();
		}
	}
}

Camera::Camera(IOManager* ioManager)
	: ioManager_(ioManager)
{
	enableDebugControl_ = true;
	isDrawEasingTestImGui_ = false;

	state_ = CreateCameraState(CameraMode_ORBIT_FPS::ORBIT);

	fovY_ = 0.65f;

	screenSize_ = Vector2{ float(Constexprs::kInitialWindowWidth) , float(Constexprs::kInitialWindowHeight) };

	pose_.spherical.radius = 10.0f;
	pose_.spherical.theta = std::numbers::pi_v<float> / 2.0f;
	pose_.spherical.phi = 1.0f;

	backToFrontMatrix_ = Matrix4x4::MakeRotateYMatrix(std::numbers::pi_v<float>);

	Resize();
}

Camera::~Camera()
{}

void Camera::Update()
{

	float dt = Engine::Instance().GetTimeManager()->GetScaledDeltaTimeMs();

	// 操作
	if (enableDebugControl_)
	{
		state_->HandleInput(pose_, ReadCameraInput());
	}

	// イージング
	MovingCenter(dt);
	MovingEye(dt);
	MovingPhi(dt);
	MovingTheta(dt);
	MovingDistance(dt);
	MovingScreenSize(dt);
	MovingFov(dt);

	// 距離と上下の向きを範囲に収める
	ClampSpherical();

	// eyeとcenterを決める
	state_->ResolvePose(pose_);

	// 行列と視錐台
	UpdateMatrices();
}

void Camera::SetCameraMode(CameraMode_ORBIT_FPS mode)
{
	// 同じモードなら作り直さない
	if (state_ && state_->GetMode() == mode) return;

	// 姿勢は Camera が持っているので、状態を差し替えても画面は飛ばない
	state_ = CreateCameraState(mode);
}

void Camera::ClampSpherical()
{
	// 距離をクランプ
	if (pose_.spherical.radius < kMinDistance) pose_.spherical.radius = kMinDistance;

	// ピッチのクランプ
	pose_.spherical.phi = std::clamp(
		pose_.spherical.phi,
		-std::numbers::pi_v<float> / 2 + kPhiMargin,
		+std::numbers::pi_v<float> / 2 - kPhiMargin
	);
}


void Camera::UpdateMatrices()
{
	// 視線ベクトル
	cameraDirection_ = (pose_.center - pose_.eye).Normalized();

	viewMatrix_ = Matrix4x4::LookAtMatrix(pose_.eye, pose_.center, { 0.0f, 1.0f, 0.0f });
	viewProjectionMatrix = viewMatrix_ * projectionMatrix_;

	billboardMatrix_ = backToFrontMatrix_ * viewMatrix_.Inverse();
	billboardMatrix_.m[3][0] = 0.0f; // 平行移動成分のリセット
	billboardMatrix_.m[3][1] = 0.0f;
	billboardMatrix_.m[3][2] = 0.0f;

	CreateFrustumPlanes();
}

CameraInput Camera::ReadCameraInput()
{
	// マウス中クリック
	const bool isMiddleHeld = ioManager_->GetMouseController()->IsHeld(2);
	// Shiftキー
	const bool isShiftHeld = ioManager_->GetKeyboardController()->IsHeld(VK_LSHIFT);

	CameraInput input;
	input.mouseDelta = ioManager_->GetMouseController()->Get2DPositionDelta();
	input.wheel = ioManager_->GetMouseController()->GetWheelDelta();
	// 中ボタンだけ: 回転、Shift+中ボタン: 平行移動
	input.isRotating = isMiddleHeld && !isShiftHeld;
	input.isPanning = isMiddleHeld && isShiftHeld;
	return input;
}


void Camera::Resize()
{
	aspect_ = screenSize_.x / screenSize_.y;
	projectionMatrix_ = Matrix4x4::MakePerspectiveFovMatrix(fovY_, aspect_, nearZ_, farZ_);
	viewportMatrix = Matrix4x4::MakeViewPortMatrix(0.0f, 0.0f, screenSize_.x, screenSize_.y, 0.0f, 1.0f);
	orthoProjectionMatrix_ = Matrix4x4::MakeOrthographicMatrix(0.0f, 0.0f, screenSize_.x, screenSize_.y, -300.0f, 300.0f);
}

void Camera::Draw()
{}

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
	ImGui::DragFloat(tag.c_str(), &pose_.spherical.radius, 0.1f);
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
	ImGui::DragFloat(tag.c_str(), &pose_.spherical.phi, 0.01f);
	tag = "durationMs" + phiTag + cameraTag;
	ImGui::DragFloat(tag.c_str(), &phiEasing_.durationMs, 1, 0, 1000);
	tag = "target" + phiTag + cameraTag;
	ImGui::DragFloat(tag.c_str(), &phiEasing_.target, 0.01f);
	tag = "easeType" + phiTag + cameraTag;
	int32_t easeType2 = static_cast<int32_t>(phiEasing_.easetype);
	if (ImGui::Combo(tag.c_str(), &easeType2, EaseTypeNames, IM_ARRAYSIZE(EaseTypeNames)))
	{
		phiEasing_.easetype = static_cast<EaseType>(easeType2);
	}
	tag = "start" + phiTag + cameraTag;
	if (ImGui::Button(tag.c_str()))
	{
		phiEasing_.durationMs *= 0.001f;
		SetPhiTarget(phiEasing_.target, phiEasing_.durationMs, phiEasing_.easetype);
	}

	std::string thetaTag = "##theta";
	tag = "theta" + thetaTag + cameraTag;
	ImGui::DragFloat(tag.c_str(), &pose_.spherical.theta, 0.01f);
	tag = "durationMs" + thetaTag + cameraTag;
	ImGui::DragFloat(tag.c_str(), &thetaEasing_.durationMs, 1, 0, 1000);
	tag = "target" + thetaTag + cameraTag;
	ImGui::DragFloat(tag.c_str(), &thetaEasing_.target, 0.01f);
	tag = "easeType" + thetaTag + cameraTag;
	int32_t easeType3 = static_cast<int32_t>(thetaEasing_.easetype);
	if (ImGui::Combo(tag.c_str(), &easeType3, EaseTypeNames, IM_ARRAYSIZE(EaseTypeNames)))
	{
		thetaEasing_.easetype = static_cast<EaseType>(easeType3);
	}
	tag = "start" + thetaTag + cameraTag;
	if (ImGui::Button(tag.c_str()))
	{
		thetaEasing_.durationMs *= 0.001f;
		SetThetaTarget(thetaEasing_.target, thetaEasing_.durationMs, thetaEasing_.easetype);
	}

	std::string centerTag = "##Center";
	tag = "center" + centerTag + cameraTag;
	ImGui::DragFloat3(tag.c_str(), &pose_.center.x, 0.1f);
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

	std::string eyeTag = "##Eye";
	tag = "eye" + eyeTag + cameraTag;
	ImGui::DragFloat3(tag.c_str(), &pose_.eye.x, 0.1f);
	tag = "durationMs" + eyeTag + cameraTag;
	ImGui::DragFloat(tag.c_str(), &eyeEasing_.durationMs, 1, 0, 1000);
	tag = "target" + eyeTag + cameraTag;
	ImGui::DragFloat3(tag.c_str(), &eyeEasing_.target.x, 0.1f);
	tag = "easeType" + eyeTag + cameraTag;
	int32_t easeType7 = static_cast<int32_t>(eyeEasing_.easetype);
	if (ImGui::Combo(tag.c_str(), &easeType7, EaseTypeNames, IM_ARRAYSIZE(EaseTypeNames)))
	{
		eyeEasing_.easetype = static_cast<EaseType>(easeType7);
	}
	tag = "start" + eyeTag + cameraTag;
	if (ImGui::Button(tag.c_str()))
	{
		eyeEasing_.durationMs *= 0.001f;
		SetEyeTarget(eyeEasing_.target, eyeEasing_.durationMs, eyeEasing_.easetype);
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
	ImGui::Checkbox(enableControlTag.c_str(), &enableDebugControl_);
	ImGui::SameLine();
	ImGui::Text("enableControl");

	std::string cameraModeTag = tag + ".cameraMode";
	int32_t currentMode = static_cast<int32_t>(GetCameraMode());
	static const char* items[] = { "ORBIT", "FPS" };
	if (ImGui::Combo(cameraModeTag.c_str(), &currentMode, items, IM_ARRAYSIZE(items)))
	{
		// 直接書き換えず、状態クラスの作り直しを通す
		SetCameraMode(static_cast<CameraMode_ORBIT_FPS>(currentMode));
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

// 実際に動かす
void Camera::MovingCenter(float dt)
{
	if (!centerEasing_.easing) return;

	centerEasing_.elapsedMs += dt;

	pose_.center = Easing::EasingValue(
		centerEasing_.start,
		centerEasing_.target,
		centerEasing_.easetype,
		(centerEasing_.elapsedMs / centerEasing_.durationMs)
	);

	if (centerEasing_.elapsedMs > centerEasing_.durationMs)
	{
		centerEasing_.easing = false;
	}
}
void Camera::MovingEye(float dt)
{
	if (!eyeEasing_.easing) return;

	eyeEasing_.elapsedMs += dt;

	pose_.eye = Easing::EasingValue(
		eyeEasing_.start,
		eyeEasing_.target,
		eyeEasing_.easetype,
		(eyeEasing_.elapsedMs / eyeEasing_.durationMs)
	);

	if (eyeEasing_.elapsedMs > eyeEasing_.durationMs)
	{
		eyeEasing_.easing = false;
	}
}
void Camera::MovingPhi(float dt)
{
	if (!phiEasing_.easing) return;

	phiEasing_.elapsedMs += dt;

	float spherical = Easing::EasingValue(
		phiEasing_.start,
		phiEasing_.target,
		phiEasing_.easetype,
		(phiEasing_.elapsedMs / phiEasing_.durationMs)
	);
	pose_.spherical.phi = spherical;

	if (phiEasing_.elapsedMs > phiEasing_.durationMs)
	{
		phiEasing_.easing = false;
	}
}
void Camera::MovingTheta(float dt)
{
	if (!thetaEasing_.easing) return;
	thetaEasing_.elapsedMs += dt;
	float spherical = Easing::EasingValue(
		thetaEasing_.start,
		thetaEasing_.target,
		thetaEasing_.easetype,
		(thetaEasing_.elapsedMs / thetaEasing_.durationMs)
	);
	pose_.spherical.theta = spherical;
	if (thetaEasing_.elapsedMs > thetaEasing_.durationMs)
	{
		thetaEasing_.easing = false;
	}
}
void Camera::MovingDistance(float dt)
{
	if (!distanceEasing_.easing) return;

	distanceEasing_.elapsedMs += dt;

	pose_.spherical.radius = Easing::EasingValue(
		distanceEasing_.start,
		distanceEasing_.target,
		distanceEasing_.easetype,
		(distanceEasing_.elapsedMs / distanceEasing_.durationMs)
	);

	if (distanceEasing_.elapsedMs > distanceEasing_.durationMs)
	{
		distanceEasing_.easing = false;
	}
}
void Camera::MovingScreenSize(float dt)
{
	if (!screenSizeEasing_.easing) return;

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
		screenSizeEasing_.easing = false;
	}

	Resize();
}
void Camera::MovingFov(float dt)
{
	if (!fovEasing_.easing) return;

	fovEasing_.elapsedMs += dt;

	fovY_ = Easing::EasingValue(
		fovEasing_.start,
		fovEasing_.target,
		fovEasing_.easetype,
		(fovEasing_.elapsedMs / fovEasing_.durationMs)
	);

	if (fovEasing_.elapsedMs > fovEasing_.durationMs)
	{
		fovEasing_.easing = false;
	}

	projectionMatrix_ = Matrix4x4::MakePerspectiveFovMatrix(fovY_, aspect_, nearZ_, farZ_);
}


// 動かす先の設定
void Camera::SetCenterTarget(Vector3 target, float durationSec, EaseType easetype)
{
	float durationMs = (durationSec > 0.0f) ? durationSec * 1000.0f : 0.001f;

	centerEasing_.start = pose_.center;
	centerEasing_.target = target;
	centerEasing_.durationMs = durationMs;
	centerEasing_.elapsedMs = 0.0f;
	centerEasing_.easing = true;
	centerEasing_.easetype = easetype;
}
void Camera::SetEyeTarget(Vector3 target, float durationSec, EaseType easetype)
{
	float durationMs = (durationSec > 0.0f) ? durationSec * 1000.0f : 0.001f;

	eyeEasing_.start = pose_.eye;
	eyeEasing_.target = target;
	eyeEasing_.durationMs = durationMs;
	eyeEasing_.elapsedMs = 0.0f;
	eyeEasing_.easing = true;
	eyeEasing_.easetype = easetype;
}
void Camera::SetPhiTarget(float target, float durationSec, EaseType easetype)
{
	float durationMs = (durationSec > 0.0f) ? durationSec * 1000.0f : 0.001f;

	phiEasing_.start = pose_.spherical.phi;
	phiEasing_.target = target;
	phiEasing_.durationMs = durationMs;
	phiEasing_.elapsedMs = 0.0f;
	phiEasing_.easing = true;
	phiEasing_.easetype = easetype;
}
void Camera::SetThetaTarget(float target, float durationSec, EaseType easetype)
{
	float durationMs = (durationSec > 0.0f) ? durationSec * 1000.0f : 0.001f;
	thetaEasing_.start = pose_.spherical.theta;
	thetaEasing_.target = target;
	thetaEasing_.durationMs = durationMs;
	thetaEasing_.elapsedMs = 0.0f;
	thetaEasing_.easing = true;
	thetaEasing_.easetype = easetype;
}
void Camera::SetDistanceTarget(float target, float durationSec, EaseType easetype)
{
	float durationMs = (durationSec > 0.0f) ? durationSec * 1000.0f : 0.001f;

	distanceEasing_.start = pose_.spherical.radius;
	distanceEasing_.target = target;
	distanceEasing_.durationMs = durationMs;
	distanceEasing_.elapsedMs = 0.0f;
	distanceEasing_.easing = true;
	distanceEasing_.easetype = easetype;
}
void Camera::SetScreenSizeTarget(Vector2 target, float durationSec, EaseType easetype)
{
	float durationMs = (durationSec > 0.0f) ? durationSec * 1000.0f : 0.001f;

	screenSizeEasing_.start = screenSize_;
	screenSizeEasing_.target = target;
	screenSizeEasing_.durationMs = durationMs;
	screenSizeEasing_.elapsedMs = 0.0f;
	screenSizeEasing_.easing = true;
	screenSizeEasing_.easetype = easetype;
}
void Camera::SetFovTarget(float target, float durationSec, EaseType easetype)
{
	float durationMs = (durationSec > 0.0f) ? durationSec * 1000.0f : 0.001f;

	fovEasing_.start = fovY_;
	fovEasing_.target = target;
	fovEasing_.durationMs = durationMs;
	fovEasing_.elapsedMs = 0.0f;
	fovEasing_.easing = true;
	fovEasing_.easetype = easetype;
}


// シェイク
void Camera::StartShake(float intensity, float duration, float frequency)
{}

bool Camera::IsShaking() const
{
	return false;
}

void Camera::StopShake()
{}

Vector3 Camera::GetShakeOffset() const
{
	return Vector3{ 0.0f, 0.0f, 0.0f };
}
