#include "TitlePhase.h"
#include <GameObjects/Map/MapManager.h>
#include <GameObjects/Map/SkyBox/SkyBox.h>
#include <GameObjects/Camera/CameraController.h>
#include <GameObjects/ScreenDrawer/ScreenDrawer.h>
#include <Utilities/Json/JsonManager.h>
#include <Utilities/functions.h>
#include <externals/MagicEnum/magic_enum.hpp>

namespace
{
	Vector4 IndexToColor(size_t id)
	{
		Vector4 color;
		color.x = static_cast<float>((id & 0x000000FF) >> 0) / 255.0f;
		color.y = static_cast<float>((id & 0x0000FF00) >> 8) / 255.0f;
		color.z = static_cast<float>((id & 0x00FF0000) >> 16) / 255.0f;
		color.w = 1.0f;
		return color;
	}
}

TitlePhase::TitlePhase()
{
	// カメラコントローラー生成
	cameraController_ = std::make_unique<CameraController>();
	// スクリーンドロワー生成
	screenDrawer_ = std::make_unique<ScreenDrawer>();
	// マップマネージャー生成
	map_ = std::make_unique<MapManager>();
	// スカイボックス生成
	skyBox_ = std::make_unique<SkyBox>();

	// カメラ作成
	c_title_ = cameraController_->AddCamera("TitlePhaseCamera");

	//buttonModelID_ = Game::Asset::Model::Load("assets/application/model/buttonPlane/buttonPlane.obj");
	buttonColliderModelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	buttonModelID_ = buttonColliderModelID_;
	buttonColliderModel_ = Game::Asset::Model::GetData(buttonColliderModelID_);
}

TitlePhase::~TitlePhase()
{}

void TitlePhase::Initialize()
{
	// フェーズ初期化
	nextPhase_ = PHASE::Phase_None;

	map_->Initialize();
	map_->GetTerrain()->SetDrawRadius(Vector3int{8,1,12});
	//map_->GetTerrain()->SetDrawRadius(Vector3int{ 3,1,3 });
	map_->GetTerrain()->SetSeed(12345);


	Game::IO::Mouse::ShowCursor(true);
	Game::Camera::Setter::SetCenter(Vector3{ 0.0f, 18.0f, 30.0f }, 0.0f, EaseType::LINEAR, c_title_);
	Game::Camera::Setter::SetRotate(Vector3{ 0.7f, 1.37f, 0.0f }, 0.0f, EaseType::LINEAR, c_title_);
	Game::Camera::Setter::SetDistance(25.0f, 0.0f, EaseType::LINEAR, c_title_);
	cameraController_->Update(c_title_);

	LoadButtonData();

	Vector3 rotate = Game::Math::YawPitchFromDirection(buttons_[0].transforms.translate - Game::Camera::Getter::GetTranslate(c_title_));
	buttons_[0].transforms.rotate = rotate;

	Vector3 pos = Vector3{ 0.0f, 25.0f, 0.0f };
	map_->GetTerrain()->GenerateChunks(pos);
}


void TitlePhase::Update()
{
	map_->Update(c_title_, Vector3{ 0.0f, 25.0f, 3.0f });
	skyBox_->Update(c_title_);
	screenDrawer_->Update(c_title_);
	cameraController_->Update(c_title_);

	if (Game::IO::Key::IsJustPressed(VK_SPACE))
	{
		MoveCamera(Stage::Menu, 1.5f);
	}


	Matrix4x4 viewProjection = Game::Camera::Getter::GetViewProjectionMatrix(c_title_);
	Ray mouseRay = Game::IO::Mouse::GetRay(viewProjection);

	for (int32_t i = 0; i < buttons_.size(); ++i)
	{
		Matrix4x4 worldMatrix = buttons_[i].worldMatrix;

#ifdef _DEBUG

		worldMatrix = Matrix4x4::MakeAffineMatrix(buttons_[i].transforms.scale, buttons_[i].transforms.rotate, buttons_[i].transforms.translate);

#endif // 

		
		if (IsCollision(mouseRay, *buttonColliderModel_, worldMatrix) || i == 0)
		{
			selectedButtonIndex_ = i;
			if (Game::IO::Mouse::IsJustPressed(0) && i != 0)
			{
				buttons_[i].onClick();
			}
			Vector3 scale = buttons_[i].transforms.scale;
			uint32_t elapsedTime = Game::Time::GetElapsedTime();
			scale *= 1.1f + std::sin(static_cast<float>(elapsedTime) * 0.1f) * 0.1f;
			worldMatrix = Matrix4x4::MakeAffineMatrix(scale, buttons_[i].transforms.rotate, buttons_[i].transforms.translate);
		}

		Matrix4x4 wvp = worldMatrix * viewProjection;
		buttons_[i].render_->SetCBufferData(0, ShaderType::VertexShader, &wvp);
		buttons_[i].render_->SetCBufferData(1, ShaderType::VertexShader, &worldMatrix);
		Vector4 color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
		buttons_[i].render_->SetCBufferData(0, ShaderType::PixelShader, &color);
		buttons_[i].render_->SetCBufferData(1, ShaderType::PixelShader, &buttons_[i].textureID);
	}
}


void TitlePhase::Draw()
{
	int32_t rt_3D = screenDrawer_->Get3DRenderTexture();
	int32_t rt_UI = screenDrawer_->GetUIRenderTexture();
	int32_t rt_Background = screenDrawer_->GetBackgroundRenderTexture();

	map_->Draw(rt_Background, rt_3D);

	screenDrawer_->Draw();

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
	//
	//	// ワールド選択ボタン群
	//	for (size_t worldIndex = 0; worldIndex < EnterWorldButtons.size(); worldIndex++)
	//	{
	//		EnterWorldButtons[worldIndex]->Draw();
	//		for (auto& strPart : EnterWorldStrs[worldIndex])
	//		{
	//			strPart->Draw();
	//		}
	//	}
	//
	//	break;
	//}
	//case TitlePhaseState::CreateNewWorld:
	//{
	//	NewWorldNameInputBox->Draw();
	//	nameInputBox->Draw();
	//	seedInputBox->Draw();
	//	tentenLine->Draw();
	//	CreateWorldDecideButton->Draw();
	//
	//	CreateWorldDecideButton->DrawImGui();
	//
	//	for (int32_t i = 0; i < 8; i++)
	//	{
	//		seedInputStr[i]->Draw();
	//	}
	//
	//	for (int32_t i = 0; i < 16; i++)
	//	{
	//		nameInputStr[i]->Draw();
	//	}
	//
	//	break;
	//}
	//default:
	//	break;
	//}

	for (auto& button : buttons_)
	{
		button.render_->Draw(rt_3D);
	}
}


void TitlePhase::DrawImGui()
{
	map_->GetTerrain()->DrawImGui();

	ImGui::Begin("Button");
	for (auto& button : buttons_)
	{
		if (ImGui::TreeNode(button.label.c_str()))
		{
			ImGui::InputText("Label", &button.label);
			ImGui::InputText("Text", &button.text);
			ImGui::DragInt("CharSize", &button.charSize);
			ImGui::DragFloat("Text Space", &button.textSpace, 0.1f);
			ImGui::DragFloat3("Text Pos", &button.textPos.x, 0.1f);
			ImGui::ColorEdit4("Text Color", &button.textColor.x);
			ImGui::DragFloat3("translate", &button.transforms.translate.x, 0.1f);
			ImGui::DragFloat3("rotate", &button.transforms.rotate.x, 0.1f);
			ImGui::DragFloat3("scale", &button.transforms.scale.x, 0.1f);
			ImGui::ColorEdit4("textureColor", &button.textureColor.x);
			
			if (ImGui::Button("Click"))
			{
				button.onClick();
			}
			ImGui::TreePop();
		}
	}

	static ButtonInfo newButton;
	if (ImGui::TreeNode("New Button"))
	{
		ImGui::InputText("Label", &newButton.label);
		ImGui::InputText("Text", &newButton.text);
		ImGui::DragInt("Char Size", &newButton.charSize);
		ImGui::DragFloat("Text Space", &newButton.textSpace, 0.1f);
		ImGui::DragFloat3("Text Pos", &newButton.textPos.x, 0.1f);
		ImGui::ColorEdit4("Text Color", &newButton.textColor.x);
		ImGui::DragFloat3("translate", &newButton.transforms.translate.x, 0.1f);
		ImGui::DragFloat3("rotate", &newButton.transforms.rotate.x, 0.1f);
		ImGui::DragFloat3("scale", &newButton.transforms.scale.x, 0.1f);
		ImGui::ColorEdit4("textureColor", &newButton.textureColor.x);



		ImGui::TreePop();
	}
	if (ImGui::Button("Add"))
	{
		buttons_.emplace_back();
		auto& button = buttons_.back();
		button.label = newButton.label;
		button.text = newButton.text;
		button.charSize = newButton.charSize;
		button.textSpace = newButton.textSpace;
		button.textPos = newButton.textPos;
		button.textColor = newButton.textColor;
		button.transforms = newButton.transforms;
		button.textureColor = newButton.textureColor;

		button.worldMatrix = Matrix4x4::MakeAffineMatrix(button.transforms.scale, button.transforms.rotate, button.transforms.translate);

		button.onClick = [this, label = button.label]() { OnClickButton(label); };

		button.render_ = std::make_unique<RenderObject>();
		button.render_->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
		button.render_->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
		button.render_->modelID_ = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");;
		button.render_->SetupFromShaders();

		Vector2 size = Game::Asset::Font::MeasureJustTextureSize(button.text, button.charSize, button.textPos, button.textSpace);
		button.textureID = Game::Asset::RenderTexture::CreateRenderTexture(static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y), button.label, button.textureColor);
		Game::Asset::Font::DrawString(button.textureID, button.text, button.charSize, button.textPos, button.textColor, button.textSpace);

	}

	if (ImGui::Button("Save"))
	{
		JsonManager::AddParam(buttonJsonPath_, "/buttonCount", static_cast<int32_t>(buttons_.size()));
		for (size_t i = 0; i < buttons_.size(); i++)
		{
			auto& button = buttons_[i];
			std::string key = "/" + std::to_string(i);
			JsonManager::AddParam(buttonJsonPath_, key + "/label", button.label);
			JsonManager::AddParam(buttonJsonPath_, key + "/text", button.text);
			JsonManager::AddParam(buttonJsonPath_, key + "/charSize", button.charSize);
			JsonManager::AddParam(buttonJsonPath_, key + "/textSpace", button.textSpace);
			JsonManager::AddParam(buttonJsonPath_, key + "/textPos", button.textPos);
			JsonManager::AddParam(buttonJsonPath_, key + "/textColor", button.textColor);
			JsonManager::AddParam(buttonJsonPath_, key + "/translate", button.transforms.translate);
			JsonManager::AddParam(buttonJsonPath_, key + "/rotate", button.transforms.rotate);
			JsonManager::AddParam(buttonJsonPath_, key + "/scale", button.transforms.scale);
			JsonManager::AddParam(buttonJsonPath_, key + "/textureColor", button.textureColor);

			JsonManager::Save(buttonJsonPath_);
		}
	}

	ImGui::End();
}

void TitlePhase::OnClickButton(std::string buttonLabel)
{
	if (buttonLabel == "TitleLogo")
	{
		MoveCamera(Stage::Title, 1.5f);
	}
	else if (buttonLabel == "WorldSelect")
	{
		MoveCamera(Stage::WorldSelect, 1.5f);
	}
	else if (buttonLabel == "Menu")
	{
		MoveCamera(Stage::Menu, 1.5f);
	}
	else if (buttonLabel == "CreateNewWorld")
	{
		nextPhase_ = PHASE::Phase_GameScene;
	}

}

void TitlePhase::MoveCamera(Stage stage, float duration)
{
	switch (stage)
	{
	case Stage::None:
		break;
	case Stage::Title:
		Game::Camera::Setter::SetCenter(Vector3{ 0.0f, 18.0f, 30.0f }, duration, EaseType::LINEAR, c_title_);
		Game::Camera::Setter::SetRotate(Vector3{ 0.7f, 1.37f, 0.0f }, duration, EaseType::LINEAR, c_title_);
		Game::Camera::Setter::SetDistance(25.0f, duration, EaseType::LINEAR, c_title_);
		break;
	case Stage::Menu:
		Game::Camera::Setter::SetCenter(Vector3{ 0.0f, 17.0f, 3.0f }, duration, EaseType::OUT_CUBIC, c_title_);
		Game::Camera::Setter::SetRotate(Vector3{ 0.1f, -1.5f, 0.0f }, duration, EaseType::OUT_CUBIC, c_title_);
		Game::Camera::Setter::SetDistance(10.0f, duration, EaseType::OUT_CUBIC, c_title_);
		break;
	case Stage::WorldSelect:
		Game::Camera::Setter::SetCenter(Vector3{ -1.0f, 17.0f, 2.0f }, duration, EaseType::OUT_CUBIC, c_title_);
		Game::Camera::Setter::SetRotate(Vector3{ 0.1f, -2.0f, 0.0f }, duration, EaseType::OUT_CUBIC, c_title_);
		Game::Camera::Setter::SetDistance(15.0f, duration, EaseType::OUT_CUBIC, c_title_);
		break;
	case Stage::CreateNewWorld:
		//Game::Camera::Setter::SetCenter(Vector3{ -1.0f, 17.0f, 2.0f }, duration, EaseType::OUT_CUBIC, c_title_);
		//Game::Camera::Setter::SetRotate(Vector3{ 0.1f, -2.0f, 0.0f }, duration, EaseType::OUT_CUBIC, c_title_);
		//Game::Camera::Setter::SetDistance(15.0f, duration, EaseType::OUT_CUBIC, c_title_);
		break;
	default:
		break;
	}
}

void TitlePhase::LoadButtonData()
{
	int32_t buttonCount = 0;
	JsonManager::Load(buttonJsonPath_, "/buttonCount", buttonCount);
	for (int32_t i = 0; i < buttonCount; ++i)
	{
		buttons_.emplace_back();
		std::string key = "/" + std::to_string(i);

		JsonManager::Load(buttonJsonPath_, key + "/label", buttons_[i].label);
		JsonManager::Load(buttonJsonPath_, key + "/text", buttons_[i].text);
		JsonManager::Load(buttonJsonPath_, key + "/charSize", buttons_[i].charSize);
		JsonManager::Load(buttonJsonPath_, key + "/textSpace", buttons_[i].textSpace);
		JsonManager::Load(buttonJsonPath_, key + "/textPos", buttons_[i].textPos);
		JsonManager::Load(buttonJsonPath_, key + "/textColor", buttons_[i].textColor);
		JsonManager::Load(buttonJsonPath_, key + "/translate", buttons_[i].transforms.translate);
		JsonManager::Load(buttonJsonPath_, key + "/rotate", buttons_[i].transforms.rotate);
		JsonManager::Load(buttonJsonPath_, key + "/scale", buttons_[i].transforms.scale);
		JsonManager::Load(buttonJsonPath_, key + "/textureColor", buttons_[i].textureColor);

		buttons_[i].worldMatrix = Matrix4x4::MakeAffineMatrix(buttons_[i].transforms.scale, buttons_[i].transforms.rotate, buttons_[i].transforms.translate);

		Vector2 size = Game::Asset::Font::MeasureJustTextureSize(buttons_[i].text, buttons_[i].charSize, buttons_[i].textPos, buttons_[i].textSpace);
		buttons_[i].textureID = Game::Asset::RenderTexture::CreateRenderTexture(static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y), buttons_[i].label, buttons_[i].textureColor);
		Game::Asset::Font::DrawString(buttons_[i].textureID, buttons_[i].text, buttons_[i].charSize, buttons_[i].textPos, buttons_[i].textColor, buttons_[i].textSpace);

		buttons_[i].onClick = [this, label = buttons_[i].label]() { OnClickButton(label); };

		buttons_[i].render_ = std::make_unique<RenderObject>();
		buttons_[i].render_->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
		buttons_[i].render_->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
		buttons_[i].render_->modelID_ = buttonModelID_;
		buttons_[i].render_->SetupFromShaders();
	}
}
