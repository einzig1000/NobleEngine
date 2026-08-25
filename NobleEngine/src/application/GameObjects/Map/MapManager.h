#pragma once
#include <GameObjects/Map/SkyBox/SkyBox.h>
#include <GameObjects/Map/Terrain/Terrain.h>
#include <memory>

class EventBus;

class MapManager
{
public:
	MapManager();
	~MapManager();

	void Initialize();
	void Update(int32_t cameraID, Vector3 cameraPos);
	void Draw(int32_t rt_Background, int32_t rt_Terrain);
	void DrawImGui();

	void SetEventBus(EventBus* eventBus);

	SkyBox* GetSkyBox() { return skyBox_.get(); }
	Terrain* GetTerrain() { return terrain_.get(); }

private:
	std::unique_ptr<SkyBox> skyBox_;
	std::unique_ptr<Terrain> terrain_;
};

