#pragma once
#include "Game.h"

class Player;
class MapManager;
class UIManager;

class CameraController
{
public:
	CameraController();
	void SetPlayer(Player* player) { player_ = player; }
	void SetMapManager(MapManager* mapManager) { mapManager_ = mapManager; }
	void SetUIManager(UIManager* uiManager) { uiManager_ = uiManager; }
	void Update();

	CameraMode_FirstPerson_ThirdPerson cameraMode_;

	// カメラ関係
	Vector3 cameraPos = { 0.0f,0.0f,0.0f };
	Vector3 cameraRot = { 0.0f,0.0f,0.0f };

	// マウス関連
	Vector2 mousedelta = { 0.0f,0.0f };
	float mouseSensitivity_ = 0.02f;

	// プレイヤー
	Player* player_ = nullptr;
	// マップマネージャー
	MapManager* mapManager_ = nullptr;
	// UIマネージャー
	UIManager* uiManager_ = nullptr;


	// カメラがブロック表面に当たった時、めり込まないよう少し手前に出す量
	float cameraPadding_ = 0.15f;

private:

	// カメラ操作可能か
	bool enableControl;

};

