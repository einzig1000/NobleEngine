#include "GameScenePhase.h"
#include <System/EventBus/EventBus.h>
#include <GameObjects/Map/MapManager.h>
#include <GameObjects/Character/CharacterManager.h>
#include <GameObjects/UI/UIManager.h>
#include <GameObjects/Camera/CameraController.h>
#include <GameObjects/ScreenDrawer/ScreenDrawer.h>
#include <GameObjects/Effect/EffectManager.h>

GameScenePhase::GameScenePhase()
{
	// イベントバス生成
	eventBus_ = std::make_unique<EventBus>();

	// カメラコントローラー生成
	cameraController_ = std::make_unique<CameraController>();
	cameraController_->SetEventBus(eventBus_.get());
	c_player_ = cameraController_->AddCamera("PlayerCamera");
	// スクリーンドロワー生成
	screenDrawer_ = std::make_unique<ScreenDrawer>();
	screenDrawer_->SetEventBus(eventBus_.get());
	screenDrawer_->SetFogParams(FogParams{ Vector3{ 0.5f, 0.5f, 0.5 }, 0.06f, 20.0f, 50.0f });
	// マップマネージャー生成
	map_ = std::make_unique<MapManager>();
	map_->SetEventBus(eventBus_.get());
	// キャラクターマネージャー生成
	charcterManager_ = std::make_unique<CharacterManager>(map_.get());
	charcterManager_->SetEventBus(eventBus_.get());
	charcterManager_->SetViewCamera(c_player_);
	// UIマネージャー生成
	uiManager_ = std::make_unique<UIManager>();
	uiManager_->ChangeScreen(UIMode::Playing);
	uiManager_->SetEventBus(eventBus_.get());
	uiManager_->SetInventory(charcterManager_->GetPlayer()->GetInventory());
	uiManager_->SetMiningGauge(charcterManager_->GetPlayer()->GetMiningPointGauge());
	// エフェクトマネージャー生成
	effectManager_ = std::make_unique<EffectManager>();
	effectManager_->SetEventBus(eventBus_.get());
}

GameScenePhase::~GameScenePhase() {}

void GameScenePhase::Initialize()
{
	nextPhase_ = Phase::Phase_None;

	map_->Initialize();
	uiManager_->Initialize();
	charcterManager_->Initialize();

	if (context_->isNewGame) map_->GetTerrain()->CreateNewMap(context_->mapName, context_->seed);
	else map_->GetTerrain()->LoadMap(context_->mapName);
}

void GameScenePhase::Update()
{
	eventBus_->Update();

	int32_t targetCameraID = c_player_;
	Vector3 cameraPos = Game::Camera::Getter::GetCenter(targetCameraID);

	// キャラクターマネージャー更新
	charcterManager_->Update(targetCameraID);
	// マップ更新
	map_->Update(targetCameraID, cameraPos);
	// UI更新
	uiManager_->Update(targetCameraID);

	// エフェクト更新
	effectManager_->Update(targetCameraID);

	// ポストエフェクト更新
	screenDrawer_->Update(targetCameraID);

	// カメラ更新
	cameraController_->Update(targetCameraID);
}


void GameScenePhase::Draw()
{
	int32_t rt_3D = screenDrawer_->Get3DRenderTexture();
	int32_t rt_UI = screenDrawer_->GetUIRenderTexture();
	int32_t rt_Background = screenDrawer_->GetBackgroundRenderTexture();

	// キャラクター描画
	charcterManager_->Draw(rt_3D);
	// マップ描画
	map_->Draw(rt_Background, rt_3D);
	// エフェクト描画
	effectManager_->Draw(rt_3D);

	// UI描画
	uiManager_->Draw(rt_UI);

	screenDrawer_->Draw();
}

void GameScenePhase::DrawImGui()
{
	// マップImGui描画
	map_->DrawImGui();
	// キャラクターImGui描画
	charcterManager_->DrawImGui();
	// UIImGui描画
	uiManager_->DrawImGui();

	screenDrawer_->DrawImGui();
}

