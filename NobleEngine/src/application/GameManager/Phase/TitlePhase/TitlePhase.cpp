#include "TitlePhase.h"
//#include "MapManager/MapManager.h"
//#include "Character/Player/Player.h"


TitlePhase::TitlePhase()
{
	//startButton = std::make_unique<RenderData_Sprite>();
	//for (auto& strPart : startStr)
	//{
	//	strPart = std::make_unique<RenderData_Sprite>();
	//}
	//CreateNewWorldButton = std::make_unique<RenderData_Sprite>();
	//for (auto& strPart : CreateNewWorldStr)
	//{
	//	strPart = std::make_unique<RenderData_Sprite>();
	//}

	//titleLogo = std::make_unique<RenderData_Sprite>();


	//// プレイヤー生成
	//player_ = std::make_unique<Player>();

	//// マップマネージャー生成
	//map_ = std::make_unique<MapManager>();
	//map_->SetPlayer(player_.get());
	//map_->SetSeed(123456);
	//map_->SetDrawRadius(20);
	//map_->SetUpdateRadius(0);
}

TitlePhase::~TitlePhase()
{}

void TitlePhase::Initialize()
{
	// フェーズ初期化
	nextPhase_ = PHASE::Phase_None;

	//// マップ初期化
	//map_->Initialize();
	//// マップ名とパスのマップ読み込み
	//map_->LoadNameAndPathMap("resources/Minecraft/Maps/MapNameAndPath.csv");

	//// タイトルフェーズ初期化
	//currentState = TitlePhaseState::None;
	//nextState = TitlePhaseState::Title;

	//// ロゴ
	//{
	//	titleLogo->texture = ResourceID::GetUITextureID(UITextureID::TITLE_logo);
	//	titleLogo->anchor = Anchor::Center;
	//	titleLogo->name = "TitleLogo";
	//	titleLogo->transforms.translate = { 640.0f,160.0f,0.0f };
	//}

	//// スタートボタン
	//{
	//	startButton->texture = ResourceID::GetUITextureID(UITextureID::TITLE_slot);
	//	startButton->anchor = Anchor::Center;
	//	startButton->name = "StartButton";
	//	startButton->transforms.translate = { 640.0f,500.0f,0.0f };
	//	startButton->color = 0x777777FF;

	//	char startText[] = { 'S','T','A','R','T' };
	//	for (int i = 0; i < 5; i++)
	//	{
	//		startStr[i]->texture = ResourceID::GetTextureID(TextureID::font_0);
	//		startStr[i]->transforms.scale = { 0.15f, 0.3f, 1.0f };
	//		startStr[i]->transforms.translate = startButton->transforms.translate;
	//		float monospacedWidth = 54.0f;
	//		startStr[i]->transforms.translate.x += monospacedWidth * i - (monospacedWidth * 5.0f) / 2.0f + monospacedWidth / 2.0f;
	//		startStr[i]->anchor = Anchor::Center;
	//		startStr[i]->cutImageSize = Vector2int(32, 32);
	//		startStr[i]->cutImageLeftTop = fontCutImagePos(startText[i]);
	//	}
	//}
	//// 新しい世界を作るボタン
	//{
	//	CreateNewWorldButton->texture = ResourceID::GetUITextureID(UITextureID::TITLE_slot);
	//	CreateNewWorldButton->anchor = Anchor::Center;
	//	CreateNewWorldButton->name = "CreateNewWorldButton";
	//	CreateNewWorldButton->transforms.translate = { 640.0f,120.0f,0.0f };
	//	CreateNewWorldButton->transforms.scale = { 1.0f,1.0f,1.0f };
	//	CreateNewWorldButton->color = 0x777777FF;

	//	char createNewWorldText[] = { 'C','R','E','A','T','E',' ','N','E','W',' ','W','O','R','L','D' };

	//	for (int i = 0; i < 16; i++)
	//	{
	//		CreateNewWorldStr[i]->texture = ResourceID::GetTextureID(TextureID::font_0);
	//		CreateNewWorldStr[i]->transforms.scale = { 0.1f, 0.2f, 1.0f };
	//		CreateNewWorldStr[i]->transforms.translate = CreateNewWorldButton->transforms.translate;
	//		float monospacedWidth = 25.0f;
	//		CreateNewWorldStr[i]->transforms.translate.x += monospacedWidth * i - (monospacedWidth * 16.0f) / 2.0f + monospacedWidth / 2.0f;
	//		CreateNewWorldStr[i]->anchor = Anchor::Center;
	//		CreateNewWorldStr[i]->cutImageSize = Vector2int(32, 32);
	//		CreateNewWorldStr[i]->cutImageLeftTop = fontCutImagePos(createNewWorldText[i]);
	//	}
	//}
	//// すべての世界の名前を設定
	//{
	//	// map_->mapNameToFilePath_のキー一覧を取得
	//	for (const auto& [mapName, filePath] : map_->mapNameToFilePath_)
	//	{
	//		allWorldNames.push_back(mapName);
	//	}
	//	EnterWorldButtons.resize(allWorldNames.size());
	//	EnterWorldStrs.resize(allWorldNames.size());

	//	for (size_t worldIndex = 0; worldIndex < allWorldNames.size(); worldIndex++)
	//	{
	//		const std::string& worldName = allWorldNames[worldIndex];

	//		EnterWorldButtons[worldIndex] = std::make_unique<RenderData_Sprite>();
	//		EnterWorldButtons[worldIndex]->texture = ResourceID::GetUITextureID(UITextureID::TITLE_slot);
	//		EnterWorldButtons[worldIndex]->anchor = Anchor::Center;
	//		EnterWorldButtons[worldIndex]->name = "EnterWorldButton_" + worldName;
	//		EnterWorldButtons[worldIndex]->transforms.translate = { 640.0f, 250.0f + static_cast<float>(worldIndex) * 50.0f, 0.0f };
	//		EnterWorldButtons[worldIndex]->transforms.scale = { 1.0f,0.5f,1.0f };
	//		EnterWorldButtons[worldIndex]->color = 0x777777FF;

	//		for (size_t charIndex = 0; charIndex < worldName.size(); charIndex++)
	//		{
	//			EnterWorldStrs[worldIndex].push_back(std::make_unique<RenderData_Sprite>());
	//			EnterWorldStrs[worldIndex][charIndex]->texture = ResourceID::GetTextureID(TextureID::font_0);
	//			EnterWorldStrs[worldIndex][charIndex]->transforms.scale = { 0.1f, 0.2f, 1.0f };
	//			EnterWorldStrs[worldIndex][charIndex]->transforms.translate = EnterWorldButtons[worldIndex]->transforms.translate;
	//			float monospacedWidth = 40.0f;
	//			EnterWorldStrs[worldIndex][charIndex]->transforms.translate.x += monospacedWidth * charIndex - (monospacedWidth * static_cast<float>(worldName.size())) / 2.0f + monospacedWidth / 2.0f;
	//			EnterWorldStrs[worldIndex][charIndex]->anchor = Anchor::Center;
	//			EnterWorldStrs[worldIndex][charIndex]->cutImageSize = Vector2int(32, 32);
	//			EnterWorldStrs[worldIndex][charIndex]->cutImageLeftTop = fontCutImagePos(worldName[charIndex]);
	//		}
	//	}
	//}
	//// 新規ワールド情報入力ボックス
	//{
	//	tentenPos[0].y = 200.0f;
	//	tentenPos[0].x = 400.0f;
	//	tentenPos[1].y = 356.0f;
	//	tentenPos[1].x = 475.0f;

	//	// 入力ボックス
	//	nameInputBox = std::make_unique<RenderData_Sprite>();
	//	nameInputBox->texture = ResourceID::GetUITextureID(UITextureID::TITLE_slot);
	//	nameInputBox->anchor = Anchor::Center;
	//	nameInputBox->name = "NameInputBox";
	//	nameInputBox->transforms.translate = { 640.0f,tentenPos[0].y,0.0f };
	//	nameInputBox->transforms.scale = { 0.5f,0.7f,1.0f };
	//	seedInputBox = std::make_unique<RenderData_Sprite>();
	//	seedInputBox->texture = ResourceID::GetUITextureID(UITextureID::TITLE_slot);
	//	seedInputBox->anchor = Anchor::Center;
	//	seedInputBox->name = "SeedInputBox";
	//	seedInputBox->transforms.translate = { 640.0f,tentenPos[1].y,0.0f };
	//	seedInputBox->transforms.scale = { 0.5f,0.7f,1.0f };

	//	// UI
	//	NewWorldNameInputBox = std::make_unique<RenderData_Sprite>();
	//	NewWorldNameInputBox->texture = ResourceID::GetUITextureID(UITextureID::TITLE_newWorldUI);
	//	NewWorldNameInputBox->anchor = Anchor::Center;
	//	NewWorldNameInputBox->name = "NewWorldNameInputBox";
	//	NewWorldNameInputBox->transforms.translate = { 640.0f,360.0f,0.0f };
	//	NewWorldNameInputBox->transforms.scale = { 1.0f,1.0f,1.0f };

	//	// 入力点滅カーソル
	//	tentenLine = std::make_unique<RenderData_Sprite>();
	//	tentenLine->texture = ResourceID::GetTextureID(TextureID::white1x1);
	//	tentenLine->anchor = Anchor::Center;
	//	tentenLine->name = "TentenLine";
	//	tentenLine->transforms.translate = { tentenPos[0].x,tentenPos[0].y,0.0f };
	//	tentenLine->transforms.scale = { 3.0f,30.0f,1.0f };
	//	tentenLine->color = 0x000000FF;

	//	// 決定ボタン
	//	CreateWorldDecideButton = std::make_unique<RenderData_Sprite>();
	//	CreateWorldDecideButton->texture = ResourceID::GetUITextureID(UITextureID::TITLE_slot);
	//	CreateWorldDecideButton->anchor = Anchor::Center;
	//	CreateWorldDecideButton->name = "CreateWorldDecideButton";
	//	CreateWorldDecideButton->transforms.translate = { 640.0f,540.0f,0.0f };
	//	CreateWorldDecideButton->transforms.scale = { 0.2f,0.5f,1.0f };
	//	CreateWorldDecideButton->color = 0x777777FF;

	//	// シードスプライト群
	//	for (int i = 0; i < 8; i++)
	//	{
	//		seedInputStr[i] = std::make_unique<RenderData_Sprite>();
	//		seedInputStr[i]->texture = ResourceID::GetTextureID(TextureID::font_0);
	//		seedInputStr[i]->transforms.scale = { 0.1f, 0.2f, 1.0f };
	//		seedInputStr[i]->transforms.translate = seedInputBox->transforms.translate;
	//		float monospacedWidth = 30.0f;
	//		seedInputStr[i]->transforms.translate.x += monospacedWidth * i - (monospacedWidth * 8.0f) / 2.0f + monospacedWidth / 2.0f;
	//		seedInputStr[i]->anchor = Anchor::Center;
	//		seedInputStr[i]->cutImageSize = Vector2int(32, 32);
	//		int32_t rand = Game::Math::RandInt(1, 9);
	//		newWorldSeed = newWorldSeed * 10 + rand;
	//		char randChar = '0' + rand;
	//		seedInputStr[i]->cutImageLeftTop = fontCutImagePos(randChar);
	//	}
	//	// 名前スプライト群
	//	for (int i = 0; i < 16; i++)
	//	{
	//		nameInputStr[i] = std::make_unique<RenderData_Sprite>();
	//		nameInputStr[i]->texture = ResourceID::GetTextureID(TextureID::font_0);
	//		nameInputStr[i]->transforms.scale = { 0.1f, 0.2f, 1.0f };
	//		nameInputStr[i]->transforms.translate = nameInputBox->transforms.translate;
	//		float monospacedWidth = 30.0f;
	//		nameInputStr[i]->transforms.translate.x += monospacedWidth * i - (monospacedWidth * 16.0f) / 2.0f + monospacedWidth / 2.0f;
	//		nameInputStr[i]->anchor = Anchor::Center;
	//		nameInputStr[i]->cutImageSize = Vector2int(32, 32);
	//		nameInputStr[i]->cutImageLeftTop = fontCutImagePos(' ');
	//	}
	//}

	Game::IO::Mouse::ShowCursor(true);
	Game::Camera::MoveCameraCenter({ 0.0f, 15.0f, 0.0f }, 0, EaseType::LINEAR);
	Game::Camera::MoveCameraDistance(0.1f, 0, EaseType::LINEAR);

	frameCount = 0;
}


void TitlePhase::Update()
{
	if (nextState != TitlePhaseState::None)
	{
		currentState = nextState;
		switch (currentState)
		{
		case TitlePhaseState::None:
			break;
		case TitlePhaseState::Title:
			break;
		case TitlePhaseState::WorldSelect:
			break;
		case TitlePhaseState::CreateNewWorld:

			break;
		default:
			break;
		}
		nextState = TitlePhaseState::None;
	}

	//switch (currentState)
	//{
	//case TitlePhaseState::None:
	//{
	//	break;
	//}
	//case TitlePhaseState::Title:
	//{
	//	if (startButton->isCollisionMouseRay)
	//	{
	//		startButton->color = 0xFFFFFFFF;
	//		if (Game::IO::Mouse::IsJustPressed(0))
	//		{
	//			nextState = TitlePhaseState::WorldSelect;
	//		}
	//	}
	//	else
	//	{
	//		startButton->color = 0x777777FF;
	//	}
	//
	//	startButton->transforms.scale = { 1.0f + 0.05f * std::sin(frameCount * 0.1f), 1.0f + 0.05f * std::sin(frameCount * 0.1f), 1.0f };
	//
	//	break;
	//}
	//case TitlePhaseState::WorldSelect:
	//{
	//	if (CreateNewWorldButton->isCollisionMouseRay)
	//	{
	//		CreateNewWorldButton->color = 0xFFFFFFFF;
	//		if (Game::IO::Mouse::IsJustPressed(0))
	//		{
	//			nextState = TitlePhaseState::CreateNewWorld;
	//		}
	//	}
	//	else
	//	{
	//		CreateNewWorldButton->color = 0x777777FF;
	//	}
	//
	//	for (size_t worldIndex = 0; worldIndex < EnterWorldButtons.size(); worldIndex++)
	//	{
	//		if (EnterWorldButtons[worldIndex]->isCollisionMouseRay)
	//		{
	//			EnterWorldButtons[worldIndex]->color = 0xFFFFFFFF;
	//			if (Game::IO::Mouse::IsJustPressed(0))
	//			{
	//				// ワールド選択処理
	//				context_->isNewGame = false;
	//				context_->mapName = allWorldNames[worldIndex];
	//				nextPhase_ = PHASE::Phase_GameScene;
	//			}
	//		}
	//		else
	//		{
	//			EnterWorldButtons[worldIndex]->color = 0x777777FF;
	//		}
	//	}
	//
	//	break;
	//}
	//case TitlePhaseState::CreateNewWorld:
	//{
	//	if (nameInputBox->isCollisionMouseRay)
	//	{
	//		nameInputBox->color = 0xFFFFFFFF;
	//		if (Game::IO::Mouse::IsJustPressed(0))
	//		{
	//			tentenLine->transforms.translate.y = tentenPos[0].y;
	//		}
	//	}
	//	else
	//	{
	//		nameInputBox->color = 0x777777FF;
	//	}
	//
	//	if (seedInputBox->isCollisionMouseRay)
	//	{
	//		seedInputBox->color = 0xFFFFFFFF;
	//		if (Game::IO::Mouse::IsJustPressed(0))
	//		{
	//			tentenLine->transforms.translate.y = tentenPos[1].y;
	//		}
	//	}
	//	else
	//	{
	//		seedInputBox->color = 0x777777FF;
	//	}
	//
	//	if (CreateWorldDecideButton->isCollisionMouseRay)
	//	{
	//		CreateWorldDecideButton->color = 0xFFFFFFFF;
	//		if (Game::IO::Mouse::IsJustPressed(0))
	//		{
	//			// ワールド作成処理
	//			if (newWorldName.length() > 0)
	//			{
	//				context_->isNewGame = true;
	//				context_->mapName = newWorldName;
	//				context_->seed = newWorldSeed;
	//
	//				nextPhase_ = PHASE::Phase_GameScene;
	//			}
	//		}
	//	}
	//	else
	//	{
	//		CreateWorldDecideButton->color = 0x777777FF;
	//	}
	//
	//	// 点滅処理
	//	if ((frameCount / 30) % 2 == 0)
	//	{
	//		tentenLine->color = 0x000000FF;
	//	}
	//	else
	//	{
	//		tentenLine->color = 0x00000000;
	//	}
	//
	//	if (std::abs(tentenLine->transforms.translate.y - tentenPos[0].y) < 0.1f)
	//	{
	//		// 名前入力中
	//		int32_t nameDigitCount = int32_t(newWorldName.length());
	//
	//		tentenLine->transforms.translate.x = nameInputBox->transforms.translate.x - (30.0f * 16.0f) / 2.0f + 30.0f * (nameDigitCount)+15.0f;
	//
	//		if (Game::IO::Key::IsJustPressed(DIK_BACKSPACE))
	//		{
	//			if (nameDigitCount > 0)
	//			{
	//				newWorldName.pop_back();
	//				nameInputStr[nameDigitCount - 1]->cutImageLeftTop = fontCutImagePos(' ');
	//			}
	//		}
	//
	//		if (nameDigitCount < 16)
	//		{
	//			if (Game::IO::Key::IsJustPressed(DIK_0))
	//			{
	//				newWorldName.push_back('0');
	//				nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('0');
	//			}
	//			else if (Game::IO::Key::IsJustPressed(DIK_1))
	//			{
	//				newWorldName.push_back('1');
	//				nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('1');
	//			}
	//			else if (Game::IO::Key::IsJustPressed(DIK_2))
	//			{
	//				newWorldName.push_back('2');
	//				nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('2');
	//			}
	//			else if (Game::IO::Key::IsJustPressed(DIK_3))
	//			{
	//				newWorldName.push_back('3');
	//				nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('3');
	//			}
	//			else if (Game::IO::Key::IsJustPressed(DIK_4))
	//			{
	//				newWorldName.push_back('4');
	//				nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('4');
	//			}
	//			else if (Game::IO::Key::IsJustPressed(DIK_5))
	//			{
	//				newWorldName.push_back('5');
	//				nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('5');
	//			}
	//			else if (Game::IO::Key::IsJustPressed(DIK_6))
	//			{
	//				newWorldName.push_back('6');
	//				nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('6');
	//			}
	//			else if (Game::IO::Key::IsJustPressed(DIK_7))
	//			{
	//				newWorldName.push_back('7');
	//				nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('7');
	//			}
	//			else if (Game::IO::Key::IsJustPressed(DIK_8))
	//			{
	//				newWorldName.push_back('8');
	//				nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('8');
	//			}
	//			else if (Game::IO::Key::IsJustPressed(DIK_9))
	//			{
	//				newWorldName.push_back('9');
	//				nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('9');
	//			}
	//
	//			if (Game::IO::Key::IsHeld(DIK_LSHIFT))
	//			{
	//				if (Game::IO::Key::IsJustPressed(DIK_A))
	//				{
	//					newWorldName.push_back('A');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('A');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_B))
	//				{
	//					newWorldName.push_back('B');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('B');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_C))
	//				{
	//					newWorldName.push_back('C');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('C');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_D))
	//				{
	//					newWorldName.push_back('D');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('D');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_E))
	//				{
	//					newWorldName.push_back('E');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('E');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_F))
	//				{
	//					newWorldName.push_back('F');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('F');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_G))
	//				{
	//					newWorldName.push_back('G');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('G');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_H))
	//				{
	//					newWorldName.push_back('H');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('H');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_I))
	//				{
	//					newWorldName.push_back('I');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('I');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_J))
	//				{
	//					newWorldName.push_back('J');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('J');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_K))
	//				{
	//					newWorldName.push_back('K');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('K');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_L))
	//				{
	//					newWorldName.push_back('L');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('L');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_M))
	//				{
	//					newWorldName.push_back('M');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('M');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_N))
	//				{
	//					newWorldName.push_back('N');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('N');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_O))
	//				{
	//					newWorldName.push_back('O');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('O');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_P))
	//				{
	//					newWorldName.push_back('P');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('P');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_Q))
	//				{
	//					newWorldName.push_back('Q');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('Q');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_R))
	//				{
	//					newWorldName.push_back('R');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('R');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_S))
	//				{
	//					newWorldName.push_back('S');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('S');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_T))
	//				{
	//					newWorldName.push_back('T');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('T');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_U))
	//				{
	//					newWorldName.push_back('U');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('U');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_V))
	//				{
	//					newWorldName.push_back('V');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('V');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_W))
	//				{
	//					newWorldName.push_back('W');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('W');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_X))
	//				{
	//					newWorldName.push_back('X');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('X');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_Y))
	//				{
	//					newWorldName.push_back('Y');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('Y');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_Z))
	//				{
	//					newWorldName.push_back('Z');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('Z');
	//				}
	//			}
	//			else
	//			{
	//				if (Game::IO::Key::IsJustPressed(DIK_A))
	//				{
	//					newWorldName.push_back('a');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('a');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_B))
	//				{
	//					newWorldName.push_back('b');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('b');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_C))
	//				{
	//					newWorldName.push_back('c');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('c');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_D))
	//				{
	//					newWorldName.push_back('d');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('d');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_E))
	//				{
	//					newWorldName.push_back('e');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('e');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_F))
	//				{
	//					newWorldName.push_back('f');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('f');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_G))
	//				{
	//					newWorldName.push_back('g');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('g');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_H))
	//				{
	//					newWorldName.push_back('h');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('h');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_I))
	//				{
	//					newWorldName.push_back('i');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('i');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_J))
	//				{
	//					newWorldName.push_back('j');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('j');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_K))
	//				{
	//					newWorldName.push_back('k');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('k');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_L))
	//				{
	//					newWorldName.push_back('l');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('l');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_M))
	//				{
	//					newWorldName.push_back('m');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('m');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_N))
	//				{
	//					newWorldName.push_back('n');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('n');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_O))
	//				{
	//					newWorldName.push_back('o');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('o');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_P))
	//				{
	//					newWorldName.push_back('p');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('p');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_Q))
	//				{
	//					newWorldName.push_back('q');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('q');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_R))
	//				{
	//					newWorldName.push_back('r');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('r');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_S))
	//				{
	//					newWorldName.push_back('s');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('s');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_T))
	//				{
	//					newWorldName.push_back('t');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('t');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_U))
	//				{
	//					newWorldName.push_back('u');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('u');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_V))
	//				{
	//					newWorldName.push_back('v');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('v');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_W))
	//				{
	//					newWorldName.push_back('w');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('w');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_X))
	//				{
	//					newWorldName.push_back('x');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('x');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_Y))
	//				{
	//					newWorldName.push_back('y');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('y');
	//				}
	//				else if (Game::IO::Key::IsJustPressed(DIK_Z))
	//				{
	//					newWorldName.push_back('z');
	//					nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos('z');
	//				}
	//			}
	//
	//			if (Game::IO::Key::IsJustPressed(DIK_SPACE))
	//			{
	//				newWorldName.push_back(' ');
	//				nameInputStr[nameDigitCount]->cutImageLeftTop = fontCutImagePos(' ');
	//			}
	//		}
	//	}
	//	else if (std::abs(tentenLine->transforms.translate.y - tentenPos[1].y) < 0.1f)
	//	{
	//		// newWorldSeed の桁数
	//		int32_t seedDigitCount = int32_t(std::to_string(newWorldSeed).length());
	//
	//		tentenLine->transforms.translate.x = seedInputBox->transforms.translate.x - (30.0f * 8.0f) / 2.0f + 30.0f * (seedDigitCount)+10.0f;
	//
	//		if (Game::IO::Key::IsJustPressed(DIK_BACKSPACE))
	//		{
	//			newWorldSeed = newWorldSeed / 10;
	//			seedInputStr[seedDigitCount - 1]->cutImageLeftTop = fontCutImagePos(' ');
	//		}
	//
	//		if (seedDigitCount < 8)
	//		{
	//			if (Game::IO::Key::IsJustPressed(DIK_1))
	//			{
	//				newWorldSeed = newWorldSeed * 10 + 1;
	//				seedInputStr[seedDigitCount]->cutImageLeftTop = fontCutImagePos('1');
	//			}
	//			else if (Game::IO::Key::IsJustPressed(DIK_2))
	//			{
	//				newWorldSeed = newWorldSeed * 10 + 2;
	//				seedInputStr[seedDigitCount]->cutImageLeftTop = fontCutImagePos('2');
	//			}
	//			else if (Game::IO::Key::IsJustPressed(DIK_3))
	//			{
	//				newWorldSeed = newWorldSeed * 10 + 3;
	//				seedInputStr[seedDigitCount]->cutImageLeftTop = fontCutImagePos('3');
	//			}
	//			else if (Game::IO::Key::IsJustPressed(DIK_4))
	//			{
	//				newWorldSeed = newWorldSeed * 10 + 4;
	//				seedInputStr[seedDigitCount]->cutImageLeftTop = fontCutImagePos('4');
	//			}
	//			else if (Game::IO::Key::IsJustPressed(DIK_5))
	//			{
	//				newWorldSeed = newWorldSeed * 10 + 5;
	//				seedInputStr[seedDigitCount]->cutImageLeftTop = fontCutImagePos('5');
	//			}
	//			else if (Game::IO::Key::IsJustPressed(DIK_6))
	//			{
	//				newWorldSeed = newWorldSeed * 10 + 6;
	//				seedInputStr[seedDigitCount]->cutImageLeftTop = fontCutImagePos('6');
	//			}
	//			else if (Game::IO::Key::IsJustPressed(DIK_7))
	//			{
	//				newWorldSeed = newWorldSeed * 10 + 7;
	//				seedInputStr[seedDigitCount]->cutImageLeftTop = fontCutImagePos('7');
	//			}
	//			else if (Game::IO::Key::IsJustPressed(DIK_8))
	//			{
	//				newWorldSeed = newWorldSeed * 10 + 8;
	//				seedInputStr[seedDigitCount]->cutImageLeftTop = fontCutImagePos('8');
	//			}
	//			else if (Game::IO::Key::IsJustPressed(DIK_9))
	//			{
	//				newWorldSeed = newWorldSeed * 10 + 9;
	//				seedInputStr[seedDigitCount]->cutImageLeftTop = fontCutImagePos('9');
	//			}
	//			else if (Game::IO::Key::IsJustPressed(DIK_0))
	//			{
	//				newWorldSeed = newWorldSeed * 10 + 0;
	//				seedInputStr[seedDigitCount]->cutImageLeftTop = fontCutImagePos('0');
	//			}
	//		}
	//	}
	//
	//	break;
	//}
	//default:
	//{
	//	break;
	//}
	//}
	//
	//Vector3 cameraRot = { 0.1f, static_cast<float>(frameCount) * 0.005f, 0.0f };
	//Game::Camera::MoveCameraRotate(cameraRot, 0, EaseType::LINEAR);
	//
	//map_->Update();

	frameCount++;
}


void TitlePhase::Draw()
{
	//map_->Draw();

	//switch (currentState)
	//{
	//case TitlePhaseState::None:
	//{
	//	break;
	//}
	//case TitlePhaseState::Title:
	//{
	//	titleLogo->Draw();
	//	titleLogo->DrawImGui();
	//	startButton->Draw();
	//	startButton->DrawImGui();
	//	for (auto& strPart : startStr)
	//	{
	//		strPart->Draw();
	//	}
	//	break;
	//}
	//case TitlePhaseState::WorldSelect:
	//{
	//	CreateNewWorldButton->Draw();
	//	for (auto& strPart : CreateNewWorldStr)
	//	{
	//		strPart->Draw();
	//	}

	//	// ワールド選択ボタン群
	//	for (size_t worldIndex = 0; worldIndex < EnterWorldButtons.size(); worldIndex++)
	//	{
	//		EnterWorldButtons[worldIndex]->Draw();
	//		for (auto& strPart : EnterWorldStrs[worldIndex])
	//		{
	//			strPart->Draw();
	//		}
	//	}

	//	break;
	//}
	//case TitlePhaseState::CreateNewWorld:
	//{
	//	NewWorldNameInputBox->Draw();
	//	nameInputBox->Draw();
	//	seedInputBox->Draw();
	//	tentenLine->Draw();
	//	CreateWorldDecideButton->Draw();

	//	CreateWorldDecideButton->DrawImGui();

	//	for (int i = 0; i < 8; i++)
	//	{
	//		seedInputStr[i]->Draw();
	//	}

	//	for (int i = 0; i < 16; i++)
	//	{
	//		nameInputStr[i]->Draw();
	//	}

	//	break;
	//}
	//default:
	//	break;
	//}

}


void TitlePhase::DrawImGui()
{}
