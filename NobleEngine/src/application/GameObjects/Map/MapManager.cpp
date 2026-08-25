#include "MapManager.h"

MapManager::MapManager()
{
	skyBox_ = std::make_unique<SkyBox>();
	terrain_ = std::make_unique<Terrain>();
}

MapManager::~MapManager()
{
	skyBox_.reset();
	terrain_.reset();
}

void MapManager::Initialize()
{
	skyBox_->Initialize();
	terrain_->Initialize();
}

void MapManager::Update(int32_t cameraID, Vector3 cameraPos)
{
	skyBox_->Update(cameraID);
	terrain_->Update(cameraID, cameraPos);
}

void MapManager::Draw(int32_t rt_Background, int32_t rt_Terrain)
{
	skyBox_->Draw(rt_Background);
	terrain_->Draw(rt_Terrain);
}

void MapManager::DrawImGui()
{
	terrain_->DrawImGui();
}

void MapManager::SetEventBus(EventBus* eventBus)
{
	skyBox_->SetEventBus(eventBus);
	terrain_->SetEventBus(eventBus);
}

