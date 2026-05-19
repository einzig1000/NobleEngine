#pragma once
#include "GameManager/Phase/PhaseParent/PhaseParent.h"

class MapManager;
class Player;

enum class TitlePhaseState
{
	None,
	Title,
	WorldSelect,
	CreateNewWorld,
};

class TitlePhase :
	public PhaseParent
{
public:
	TitlePhase();
	~TitlePhase() override;

	std::string GetStageFilePath() const { return stageFilePath_; }

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void DrawImGui() override;
	void ChangePhase(PHASE phase) override { nextPhase_ = phase; }


private:

	std::string stageFilePath_;

	//// ロゴ
	//std::unique_ptr<RenderData_Sprite> titleLogo;

	//// スタートボタン
	//std::unique_ptr<RenderData_Sprite> startButton;
	//std::array<std::unique_ptr<RenderData_Sprite>, 5> startStr;

	//// 新規ワールド作成ボタン
	//std::unique_ptr<RenderData_Sprite> CreateNewWorldButton;
	//std::array<std::unique_ptr<RenderData_Sprite>, 16> CreateNewWorldStr;

	//// 新規ワールド名前シード入力
	//std::unique_ptr<RenderData_Sprite> NewWorldNameInputBox;
	//std::unique_ptr<RenderData_Sprite> nameInputBox;
	//std::array<std::unique_ptr<RenderData_Sprite>, 16> nameInputStr;
	//std::unique_ptr<RenderData_Sprite> seedInputBox;
	//std::string newWorldName;
	//std::array<std::unique_ptr<RenderData_Sprite>, 8> seedInputStr;
	//uint32_t newWorldSeed = 0;
	//std::unique_ptr<RenderData_Sprite> tentenLine;
	//Vector2 tentenPos[2];
	//std::unique_ptr<RenderData_Sprite> CreateWorldDecideButton;

	//// ワールド選択ボタン群
	//std::vector<std::string> allWorldNames;
	//std::vector<std::unique_ptr<RenderData_Sprite>> EnterWorldButtons;
	//std::vector<std::vector<std::unique_ptr<RenderData_Sprite>>> EnterWorldStrs;



	// タイトルフェーズ状態
	TitlePhaseState currentState = TitlePhaseState::Title;
	TitlePhaseState nextState = TitlePhaseState::Title;

	// フレームカウント
	uint32_t frameCount = 0;


	//// マップ
	//std::unique_ptr<MapManager> map_;
	//// プレイヤー
	//std::unique_ptr<Player> player_;
};