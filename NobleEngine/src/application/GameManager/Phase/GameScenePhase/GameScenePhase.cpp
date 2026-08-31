#include "GameScenePhase.h"
#include <ResourceLoader/ResourceID.h>
#include <GameObjects/EventBus/EventBus.h>
#include <GameObjects/Map/MapManager.h>
#include <GameObjects/Character/CharacterManager.h>
#include <GameObjects/UI/UIManager.h>
#include <GameObjects/Camera/CameraController.h>
#include <GameObjects/ScreenDrawer/ScreenDrawer.h>

GameScenePhase::GameScenePhase()
{
	// イベントバス生成
	eventBus_ = std::make_unique<EventBus>();

	// カメラコントローラー生成
	cameraController_ = std::make_unique<CameraController>();
	cameraController_->SetEventBus(eventBus_.get());
	// スクリーンドロワー生成
	screenDrawer_ = std::make_unique<ScreenDrawer>();
	screenDrawer_->SetEventBus(eventBus_.get());

	// マップマネージャー生成
	map_ = std::make_unique<MapManager>();
	map_->SetEventBus(eventBus_.get());
	// キャラクターマネージャー生成
	charctorManager_ = std::make_unique<CharacterManager>(map_.get());
	charctorManager_->SetEventBus(eventBus_.get());
	// UIマネージャー生成
	uiManager_ = std::make_unique<UIManager>();
	uiManager_->SetEventBus(eventBus_.get());
	uiManager_->SetInventory(charctorManager_->GetPlayer()->GetInventory());



	c_debug_ = cameraController_->AddCamera("DebugCamera");
	c_player_ = cameraController_->AddCamera("PlayerCamera");

	charctorManager_->SetViewCamera(c_player_);
}

GameScenePhase::~GameScenePhase() {}

void GameScenePhase::Initialize()
{
	nextPhase_ = PHASE::Phase_None;

	map_->Initialize();
	uiManager_->Initialize();
	charctorManager_->Initialize();

	if (context_->isNewGame) map_->GetTerrain()->CreateNewMap(context_->mapName, context_->seed);
	else map_->GetTerrain()->LoadMap(context_->mapName);
}

void GameScenePhase::Update()
{
	//int32_t targetCameraID = c_debug_;
	int32_t targetCameraID = c_player_;
	Vector3 cameraPos = Game::Camera::Getter::GetCenter(targetCameraID);

	// UI更新
	uiManager_->Update(targetCameraID);
	// キャラクターマネージャー更新
	charctorManager_->Update(targetCameraID);
	// マップ更新
	map_->Update(targetCameraID, cameraPos);

	screenDrawer_->Update(targetCameraID);

	// カメラ更新
	//Game::Camera::Setter::SetCenter(charctorManager_->GetPlayer()->GetPosition(), 0.0f, EaseType::IN_BACK, targetCameraID);
	cameraController_->Update(targetCameraID);

}


void GameScenePhase::Draw()
{
	int32_t rt_3D = screenDrawer_->Get3DRenderTexture();
	int32_t rt_UI = screenDrawer_->GetUIRenderTexture();
	int32_t rt_Background = screenDrawer_->GetBackgroundRenderTexture();

	// キャラクター描画
	charctorManager_->Draw(rt_3D);
	// マップ描画
	map_->Draw(rt_Background, rt_3D);
	// UI描画
	uiManager_->Draw(rt_UI);

	screenDrawer_->Draw();
}

void GameScenePhase::DrawImGui()
{
	// マップImGui描画
	map_->DrawImGui();
	// キャラクターImGui描画
	charctorManager_->DrawImGui();
	// UIImGui描画
	uiManager_->DrawImGui();

	screenDrawer_->DrawImGui();
}

