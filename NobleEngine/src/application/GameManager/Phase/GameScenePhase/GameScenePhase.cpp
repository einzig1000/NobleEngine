#include "GameScenePhase.h"
#include "MapManager/MapManager.h"
#include "Character/Player/Player.h"
#include "Character/Enemy/EnemyManager.h"
#include "Camera/CameraController.h"
#include "UIManager/UIManager.h"
#include "Physics/IWorldCollider.h"
#include "Item/CraftRecipe/CraftRecipe.h"
#include <fstream>

GameScenePhase::GameScenePhase()
{
	// プレイヤー生成
	player_ = std::make_unique<Player>();
	// カメラコントローラー生成
	cameraController_ = std::make_unique<CameraController>();
	// マップマネージャー生成
	map_ = std::make_unique<MapManager>();
	// UIマネージャー生成
	uiManager_ = std::make_unique<UIManager>();
	// 敵マネージャー生成
	enemyManager_ = std::make_unique<EnemyManager>();

	// マップマネージャーにプレイヤーをセット
	map_->SetPlayer(player_.get());
	// マップ名とパスのマップ読み込み
	map_->LoadNameAndPathMap("resources/Minecraft/Maps/MapNameAndPath.csv");

	// カメラコントローラーにプレイヤーとマップマネージャーをセット
	cameraController_->SetPlayer(player_.get());
	cameraController_->SetMapManager(map_.get());
	cameraController_->SetUIManager(uiManager_.get());

	// プレイヤーにマップマネージャーをセット
	player_->SetMapManager(map_.get());
	player_->SetUIManager(uiManager_.get());

	// 敵マネージャーにプレイヤーをセット
	enemyManager_->SetPlayer(player_.get());
	enemyManager_->SetMapManager(map_.get());
	enemyManager_->SetUIManager(uiManager_.get());

	// UIマネージャーにプレイヤーとマップマネージャーをセット
	uiManager_->SetPlayer(player_.get());
	uiManager_->SetMapManager(map_.get());

	// 物理システムにワールドコライダーをセット
	//Game::Physics::AddWorldCollider(map_.get());
	//Game::Physics::ClearDynamicAll();

	// プレイヤーの物理演算有効化
	//Game::Physics::RegisterDynamic(&player_->data_);

	CraftRecipeList::InitializeRecipes();
}

GameScenePhase::~GameScenePhase() {}

void GameScenePhase::Initialize()
{
	nextPhase_ = PHASE::Phase_None;

	map_->Initialize();
	player_->Initialize();
	uiManager_->Initialize();
	enemyManager_->Initialize();

	if (context_->isNewGame) map_->CreateNewMap(context_->mapName, context_->seed);
	else map_->LoadMap(context_->mapName);

	Game::Camera::SetCameraMode(CameraMode_ORBIT_FPS::FPS);
	Game::IO::Mouse::ShowCursor(false);
}


void GameScenePhase::Update()
{
	//if (Game::IO::Key::IsJustPressed(DIK_F))
	//{
	//	enemyManager_->AddNewEnemy(player_->data_.GetWorldPosition() + Vector3{ 0.0f,20.0f,0.0f });
	//}

	if (Game::IO::Key::IsJustPressed(DIK_L))
	{
		map_->SaveMap();
	}

	// プレイヤー更新
	player_->Update();
	// 敵マネージャー更新
	enemyManager_->Update();
	// マップ更新
	map_->Update();
	// UI更新
	uiManager_->Update();
	// カメラ更新
	cameraController_->Update();

}


void GameScenePhase::Draw()
{
	// マップ描画
	map_->Draw();
	// プレイヤー描画
	player_->Draw();
	// 敵描画
	enemyManager_->Draw();
	// UI描画
	uiManager_->Draw();
}

void GameScenePhase::DrawImGui()
{
	// マップImGui描画
	map_->DrawImGui();
	// プレイヤーImGui描画
	player_->DrawImGui();
	// 敵ImGui描画
	enemyManager_->DrawImGui();
	// UIImGui描画
	uiManager_->DrawImGui();
}




