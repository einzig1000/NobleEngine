#include "CameraController.h"
#include "Character/Player/Player.h"
#include "MapManager/MapManager.h"
#include "UiManager/UiManager.h"

namespace
{
	static Vector3 FaceToNormal(AABBFace face)
	{
		switch (face)
		{
		case AABBFace::LEFT:   return { -1, 0, 0 };
		case AABBFace::RIGHT:  return { 1, 0, 0 };
		case AABBFace::BOTTOM: return { 0,-1, 0 };
		case AABBFace::TOP:    return { 0, 1, 0 };
		case AABBFace::BACK:   return { 0, 0,-1 };
		case AABBFace::FRONT:  return { 0, 0, 1 };
		default:              return { 0, 0, 0 };
		}
	}
}

CameraController::CameraController()
{
	// カメラ操作可能に設定
	enableControl = true;
	
	// 初期カメラモードは三人称後方視点
	cameraMode_ = CameraMode_FirstPerson_ThirdPerson::FirstPerson;
}

void CameraController::Update()
{
	UIMode uiMode = uiManager_->GetCurrentUIMode();


	mousedelta = Game::IO::Mouse::GetPositionDelta();

	// カメラモード切り替え
	if (Game::IO::Key::IsJustPressed(DIK_F5))
	{
		switch (cameraMode_)
		{
		case CameraMode_FirstPerson_ThirdPerson::FirstPerson:
			cameraMode_ = CameraMode_FirstPerson_ThirdPerson::ThirdPerson_Back;
			break;
		case CameraMode_FirstPerson_ThirdPerson::ThirdPerson_Back:
			cameraMode_ = CameraMode_FirstPerson_ThirdPerson::ThirdPerson_Front;
			break;
		case CameraMode_FirstPerson_ThirdPerson::ThirdPerson_Front:
			cameraMode_ = CameraMode_FirstPerson_ThirdPerson::FirstPerson;
			break;
		default:
			break;
		}
	}

	if (enableControl)
	{
		// マウス移動量に応じてカメラ回転
		if (uiMode == UIMode::Playing ||
			uiMode == UIMode::Hidden)
		{
			cameraRot.x += mousedelta.y * mouseSensitivity_;
			cameraRot.y += mousedelta.x * mouseSensitivity_;
			float harfPi = 3.14159f / 2.0f;
			cameraRot.x = std::clamp<float>(cameraRot.x, -harfPi, harfPi);
		}

		// カメラ位置計算
		const Vector3 target = player_->viewRay_.origin;
		Vector3 desirePos = target;

		switch (cameraMode_)
		{
		case CameraMode_FirstPerson_ThirdPerson::FirstPerson:
			// カメラ位置はプレイヤーの目の位置
			desirePos = target;
			break;

		case CameraMode_FirstPerson_ThirdPerson::ThirdPerson_Back:
		{
			// カメラ位置はプレイヤーの後方
			Vector3 backOffset;
			backOffset.x = -std::sin(cameraRot.y) * 4.0f;
			backOffset.y = 2.0f + std::sin(cameraRot.x) * 2.0f;
			backOffset.z = -std::cos(cameraRot.y) * 4.0f;
			desirePos = target + backOffset;
			break;
		}
		case CameraMode_FirstPerson_ThirdPerson::ThirdPerson_Front:
		{
			// カメラ位置はプレイヤーの前方
			Vector3 frontOffset;
			frontOffset.x = std::sin(cameraRot.y) * 4.0f;
			frontOffset.y = 2.0f + std::sin(cameraRot.x) * 2.0f;
			frontOffset.z = std::cos(cameraRot.y) * 4.0f;
			desirePos = target + frontOffset;
			break;
		}
		default:
			break;
		}

		// ===== ここが「めり込み防止」本体 =====
		cameraPos = desirePos;

		// 一人称は通常めり込み防止不要（目の位置を壁に入れないのは別問題）
		if (cameraMode_ == CameraMode_FirstPerson_ThirdPerson::ThirdPerson_Back ||
			cameraMode_ == CameraMode_FirstPerson_ThirdPerson::ThirdPerson_Front)
		{
			Vector3 toCam = desirePos - target;
			const float desiredDist = toCam.Length();
			if (desiredDist > 1e-6f)
			{
				Ray camRay;
				camRay.origin = target;
				camRay.diff = toCam / desiredDist; // normalize

				auto hit = mapManager_->GetBlockByCrossedRay(camRay, player_->maxDistance);
				if (hit.has_value())
				{
					const lookAtBlock& lab = hit.value();

					// GetBlockByCrossedRay は max 20.0f 前提なので、念のため距離で絞る
					// ※lab.distance は LengthSq()なのでsqrtするか比較をSqで統一する
					const float hitDist = std::sqrt(lab.distance);

					// カメラより手前で当たったなら、当たり面から少し押し出した位置へ寄せる
					if (hitDist < desiredDist)
					{
						const Vector3 n = FaceToNormal(lab.face);

						// 当たったブロックのAABBを取り、面の座標を取る
						AABB aabb = mapManager_->GetAABB(lab.chunkIndex, lab.localIndex);

						// 面の外側へ少しだけ出す（cameraPadding_）
						Vector3 clamped = desirePos;

						switch (lab.face)
						{
						case AABBFace::LEFT:   clamped.x = aabb.min.x - cameraPadding_; break;
						case AABBFace::RIGHT:  clamped.x = aabb.max.x + cameraPadding_; break;
						case AABBFace::BOTTOM: clamped.y = aabb.min.y - cameraPadding_; break;
						case AABBFace::TOP:    clamped.y = aabb.max.y + cameraPadding_; break;
						case AABBFace::BACK:   clamped.z = aabb.min.z - cameraPadding_; break;
						case AABBFace::FRONT:  clamped.z = aabb.max.z + cameraPadding_; break;
						default: break;
						}

						// 注視点から見て同一直線上に近づける（視線ズレを減らす）
						// ここでは「押し出し後の位置」を採用
						cameraPos = clamped;
					}
				}
			}
		}
		else if (cameraMode_ == CameraMode_FirstPerson_ThirdPerson::FirstPerson)
		{
			if (mapManager_->isSolidAt(cameraPos))
			{
				int i = 0;
			}
		}
	}
	
	Game::Camera::MoveCameraDistance(0.0f, 0, EaseType::LINEAR);
	Game::Camera::MoveCameraCenter(cameraPos, 0, EaseType::LINEAR);
	Game::Camera::MoveCameraRotate(cameraRot, 3, EaseType::LINEAR);
}