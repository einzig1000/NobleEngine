#include "CameraController.h"

CameraController::CameraController()
{}

void CameraController::Update(int32_t cameraID)
{
	Game::Camera::Update(cameraID);
}

int32_t CameraController::AddCamera(const std::string & cameraName)
{
	int32_t cameraID = Game::Camera::AddCamera(cameraName);
	cameraIDs_.push_back(cameraID);
	return cameraID;
}

