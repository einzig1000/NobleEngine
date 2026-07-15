#pragma once
#include <vector>
#include <Game.h>
#include <GameObjects/UI/UIData.h>

class IUIElement;
class Player;
class MapManager;

class IUIScreen
{
public:
	IUIScreen() = default;
	virtual ~IUIScreen() = default;
	// 画面の初期化
	virtual void Initialize() = 0;
	// 画面の更新
	virtual void Update(int32_t cameraID) = 0;
	// 画面の描画
	virtual void Draw(int32_t renderTargetID) = 0;

	virtual UIMode GetNextUIMode() const { return nextUIMode_; }

	// エレメントの追加
	virtual void AddElement(IUIElement* element) { uiElements_.push_back(element); }

	// プレイヤーセット
	virtual void SetPlayer(Player* player) { player_ = player; }
	// マップマネージャーセット
	virtual void SetMapManager(MapManager* mapManager) { mapManager_ = mapManager; }

	// 必要なエレメントの取得
	virtual const std::vector<UIElementType>& GetRequiredElements() const { return elementTypes_; }


protected:
	UIMode nextUIMode_ = UIMode::None;
	std::vector<UIElementType> elementTypes_;	// この画面で使用するエレメントの種類
	std::vector<IUIElement*> uiElements_;

	Player* player_ = nullptr;
	MapManager* mapManager_ = nullptr;
};

