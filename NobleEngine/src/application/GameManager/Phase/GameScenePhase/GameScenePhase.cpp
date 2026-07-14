#include "GameScenePhase.h"
#include <GameObjects/Map/MapManager.h>
#include <ResourceLoader/ResourceID.h>
#include <GameObjects/Character/Player/Player.h>
#include <GameObjects/Character/Enemy/EnemyManager.h>
#include <Camera/CameraController.h>
#include <GameObjects/UI/UIManager.h>
#include <GameObjects/Item/CraftRecipe/CraftRecipe.h>
#include <fstream>

GameScenePhase::GameScenePhase()
{
	rt_main_ = Game::Asset::RenderTexture::CreateRenderTexture(Game::Window::GetWidth(), Game::Window::GetHeight(), "Main");
	c_debug_ = Game::Camera::AddCamera("DebugCamera");

	// プレイヤー生成
	player_ = std::make_unique<Player>();
	// カメラコントローラー生成
	//cameraController_ = std::make_unique<CameraController>();
	// マップマネージャー生成
	map_ = std::make_unique<MapManager>();
	// UIマネージャー生成
	uiManager_ = std::make_unique<UIManager>();
	// 敵マネージャー生成
	//enemyManager_ = std::make_unique<EnemyManager>();

	// マップ名とパスのマップ読み込み
	map_->LoadNameAndPathMap("resources/Minecraft/Maps/MapNameAndPath.csv");

	// カメラコントローラーにプレイヤーとマップマネージャーをセット
	//cameraController_->SetPlayer(player_.get());
	//cameraController_->SetMapManager(map_.get());
	//cameraController_->SetUIManager(uiManager_.get());

	// プレイヤーにマップマネージャーをセット
	player_->SetMapManager(map_.get());
	player_->SetUIManager(uiManager_.get());

	// 敵マネージャーにプレイヤーをセット
	//enemyManager_->SetPlayer(player_.get());
	//enemyManager_->SetMapManager(map_.get());
	//enemyManager_->SetUIManager(uiManager_.get());

	// UIマネージャーにプレイヤーとマップマネージャーをセット
	uiManager_->SetPlayer(player_.get());
	uiManager_->SetMapManager(map_.get());

	// 物理システムにワールドコライダーをセット
	//Game::Physics::AddWorldCollider(map_.get());
	//Game::Physics::ClearDynamicAll();


	//CraftRecipeList::InitializeRecipes();
	//ResourceID::reload();

	render_ = std::make_unique<RenderObject>();
	render_->psoConfig_.vs = "resources/shaders/FullScreen/FullScreen.VS.hlsl";
	render_->psoConfig_.ps = "resources/shaders/FullScreen/CopyImage.PS.hlsl";
	render_->modelID_ = Game::Asset::Model::Load("resources/prototypes/model/plane/plane.obj");
	render_->SetupFromShaders();
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

	//Game::Camera::SetCameraMode(CameraMode_ORBIT_FPS::FPS);
	//Game::IO::Mouse::ShowCursor(false);
}


void GameScenePhase::Update()
{
	//int32_t targetCameraID = c_debug_;
	int32_t targetCameraID = player_->GetCameraID();
	Game::Camera::Update(targetCameraID);

	// プレイヤー更新
	player_->Update(targetCameraID);
	// 敵マネージャー更新
	//enemyManager_->Update();
	// マップ更新
	map_->Update(targetCameraID);
	// UI更新
	uiManager_->Update(targetCameraID);
	// カメラ更新
	//cameraController_->Update();


	render_->SetCBufferData(0, ShaderType::PixelShader, &rt_main_);
}


void GameScenePhase::Draw()
{
	// マップ描画
	map_->Draw(rt_main_);
	// プレイヤー描画
	player_->Draw(rt_main_);
	// 敵描画
	//enemyManager_->Draw();
	// UI描画
	uiManager_->Draw();

	render_->ScreenDraw();
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

	//ImGui::Begin("------debug info------");
	//ImGui::Text("ESC : Quit Application");
	//ImGui::Text("F1  : Hide Debug Info");
	//ImGui::Text("F3  : Toggle Camera Release or Debug");
	//ImGui::Text("F5  : Toggle Camera FirstPerson or ThirdPerson");
	//ImGui::Text("F12 : Toggle Fullscreen");
	//ImGui::Text("DeltaTime: %.3f ms", Game::Time::GetDeltaTime() * 1000.0f);
	//ImGui::Text("FPS: %.1f ", Game::Time::GetFrameRate());
	//ImGui::End();
}




