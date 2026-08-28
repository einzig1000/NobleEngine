#pragma once
#include <GameObjects/Character/ICharacter.h>
#include <GameObjects/Character/Player/Player.h>
#include <vector>
#include <memory>

class MapManager;
class EventBus;

class CharacterManager
{
public:
	CharacterManager(MapManager* mapManager);
	~CharacterManager();

	void Initialize();
	void Update(int32_t cameraID);
	void Draw(int32_t renderTextureID);
	void DrawImGui();

	void SetViewCamera(int32_t cameraID);

	// Enemyクラス作ったら置き換え
	void AddEnemy(std::unique_ptr<ICharacter> character);
	void RemoveEnemy(int32_t enemyID);

	void SetEventBus(EventBus* eventBus);

	Player* GetPlayer() { return player_.get(); }

private:
	MapManager* mapManager_ = nullptr;
	EventBus* eventBus_ = nullptr;

	std::unique_ptr<Player> player_;
	std::vector<std::unique_ptr<ICharacter>> enemies_;
};

