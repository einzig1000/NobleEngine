#include "GameScenePhase.h"
#include <ResourceLoader/ResourceID.h>
#include <GameObjects/Map/MapManager.h>
#include <GameObjects/Map/SkyBox/SkyBox.h>
#include <GameObjects/Character/Player/Player.h>
#include <GameObjects/Character/Enemy/EnemyManager.h>
#include <GameObjects/Camera/CameraController.h>
#include <GameObjects/ScreenDrawer/ScreenDrawer.h>
#include <GameObjects/UI/UIManager.h>

GameScenePhase::GameScenePhase()
{
	c_debug_ = Game::Camera::AddCamera("DebugCamera");

	// UIマネージャー生成
	uiManager_ = std::make_unique<UIManager>();
	// プレイヤー生成
	player_ = std::make_unique<Player>();
	// カメラコントローラー生成
	cameraController_ = std::make_unique<CameraController>();
	// マップマネージャー生成
	map_ = std::make_unique<MapManager>();
	// スカイボックス生成
	skyBox_ = std::make_unique<SkyBox>();

	// 敵マネージャー生成
	//enemyManager_ = std::make_unique<EnemyManager>();

	// カメラコントローラーにプレイヤーとマップマネージャーをセット
	//cameraController_->SetPlayer(player_.get());
	//cameraController_->SetMapManager(map_.get());
	//cameraController_->SetUIManager(uiManager_.get());

	// プレイヤーにマップマネージャーをセット
	player_->SetMapManager(map_.get());

	// 敵マネージャーにプレイヤーをセット
	//enemyManager_->SetPlayer(player_.get());
	//enemyManager_->SetMapManager(map_.get());
	//enemyManager_->SetUIManager(uiManager_.get());

	screenDrawer_ = std::make_unique<ScreenDrawer>();


	ResourceID::reload();
}

GameScenePhase::~GameScenePhase() {}

void GameScenePhase::Initialize()
{
	nextPhase_ = PHASE::Phase_None;

	map_->Initialize();
	player_->Initialize();
	uiManager_->Initialize();
	//enemyManager_->Initialize();

	if (context_->isNewGame) map_->CreateNewMap(context_->mapName, context_->seed);
	else map_->LoadMap(context_->mapName);
}

void GameScenePhase::Update()
{
	//int32_t targetCameraID = c_debug_;
	int32_t targetCameraID = player_->GetCameraID();
	Game::Camera::Update(targetCameraID);

	if (uiManager_->IsGameplayActive())
	{
		// プレイヤー更新
		player_->Update(targetCameraID);
		// 敵マネージャー更新
		//enemyManager_->Update(targetCameraID);
	}
	// マップ更新
	map_->Update(targetCameraID);
	skyBox_->Update(targetCameraID);
	// UI更新
	uiManager_->Update(targetCameraID);
	// カメラ更新
	//cameraController_->Update();

	if (player_->JustDamaged())
	{
 		screenDrawer_->TakeDamage();
	}
	screenDrawer_->Update(targetCameraID);
}


void GameScenePhase::Draw()
{
	int32_t rt_3D = screenDrawer_->Get3DRenderTexture();
	int32_t rt_UI = screenDrawer_->GetUIRenderTexture();
	int32_t rt_Background = screenDrawer_->GetBackgroundRenderTexture();

	// マップ描画
	map_->Draw(rt_3D);
	skyBox_->Draw(rt_Background);
	// プレイヤー描画
	player_->Draw(rt_3D);
	// 敵描画
	//enemyManager_->Draw(rt_3D);
	// UI描画
	uiManager_->Draw(rt_UI);

	screenDrawer_->Draw();
}

void GameScenePhase::DrawImGui()
{
	// マップImGui描画
	map_->DrawImGui();
	// プレイヤーImGui描画
	player_->DrawImGui();
	// 敵ImGui描画
	//enemyManager_->DrawImGui();
	// UIImGui描画
	uiManager_->DrawImGui();

	screenDrawer_->DrawImGui();
}

