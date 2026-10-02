#include "ItemEditor.h"
#include <System/ResourceLoader/Data/DataManager.h>
#include <GameObjects/UI/ItemIcon/ItemIconManager.h>
#include <Utilities/Json/JsonManager.h>
#include <App.h>

ItemEditor::ItemEditor(DataManager* dataManager)
{
	renderObject_ = std::make_unique<RenderObject>();
	renderObject_->psoConfig_.ps = "assets/shaders/SimpleModel/SimpleModel.PS.hlsl";
	renderObject_->psoConfig_.vs = "assets/shaders/SimpleModel/SimpleModel.VS.hlsl";
	renderObject_->SetupFromShaders();

	renderTextureID_ = Game::Asset::RenderTexture::CreateRenderTexture(512, 512, "ItemEditorTexture");
	cameraID_ = Game::Camera::AddCamera("ItemEditorCamera");
	Game::Camera::Setter::ScreenSizeTarget(Vector2(512, 512), 0, EaseType::IN_BACK, cameraID_);

	renderObject_->modelID_ = Game::Asset::Model::Load("assets/engine/model/cube/cube.obj");
	textureID_ = Game::Asset::Texture::Load("assets/engine/texture/uvChecker.png");

	iconPreviewTextureID_ = Game::Asset::RenderTexture::CreateRenderTexture(256, 256, "ItemIconPreview");
	iconRenderObject_ = ItemIconManager::CreateRenderObject();
}

ItemEditor::~ItemEditor()
{

}

void ItemEditor::Update()
{
	Game::Camera::Update(cameraID_);
}

void ItemEditor::Draw()
{
	if (renderObject_->modelID_ >= 0 && textureID_ > 0)
	{
		Matrix4x4 world = Matrix4x4::MakeAffineMatrix(transforms_.scale, transforms_.rotate, transforms_.translate);
		Matrix4x4 wpv = world * Game::Camera::Getter::GetViewProjectionMatrix(cameraID_);

		renderObject_->SetBRegisterData(0, ShaderType::PixelShader, &color_);
		renderObject_->SetBRegisterData(1, ShaderType::PixelShader, &textureID_);
		renderObject_->SetBRegisterData(0, ShaderType::VertexShader, &wpv);
		renderObject_->SetBRegisterData(1, ShaderType::VertexShader, &world);
		renderObject_->Draw(renderTextureID_);
	}

	// アイコンのプレビュー
	if (iconItemID_ != ItemID::MAX)
	{
		ItemIconManager::DrawItem(*iconRenderObject_, iconItemInfo_, iconPreviewTextureID_);
		iconPreviewDrawn_ = true;
	}
}

void ItemEditor::DrawImGui()
{
	DrawIconImGui();

	ImGui::Begin("Item Editor");

	// enum配列
	auto itemGenreValues = magic_enum::enum_values<ItemGenre>();
	auto itemGenreNames = magic_enum::enum_names<ItemGenre>();
	auto toolIDValues = magic_enum::enum_values<ToolID>();
	auto toolIDNames = magic_enum::enum_names<ToolID>();
	auto blockIDValues = magic_enum::enum_values<BlockID>();
	auto blockIDNames = magic_enum::enum_names<BlockID>();
	auto objectIDValues = magic_enum::enum_values<ObjectID>();
	auto objectIDNames = magic_enum::enum_names<ObjectID>();
	auto itemIDValues = magic_enum::enum_values<ItemID>();
	auto itemIDNames = magic_enum::enum_names<ItemID>();

	if (ImGui::TreeNodeEx("子データ作成", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// アイテムジャンル
		{
			if (ImGui::BeginCombo("ItemGenre", magic_enum::enum_name(genre_).data()))
			{
				for (std::size_t i = 0; i < itemGenreValues.size(); i++)
				{
					ItemGenre value = itemGenreValues[i];
					bool selected = (genre_ == value);

					if (ImGui::Selectable(itemGenreNames[i].data(), selected))
					{
						genre_ = value;
					}
					if (selected) ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
		}

		// パラメータ編集
		switch (genre_)
		{
		case ItemGenre::MAX:
		{
			break;
		}
		case ItemGenre::Armor:
		{
			break;
		}
		case ItemGenre::Tool:
		{
			if (ImGui::BeginCombo("ToolID", magic_enum::enum_name(toolID_).data()))
			{
				for (std::size_t i = 0; i < toolIDValues.size(); i++)
				{
					ToolID value = toolIDValues[i];
					bool selected = (toolID_ == value);

					if (ImGui::Selectable(toolIDNames[i].data(), selected))
					{
						toolID_ = value;
					}
					if (selected) ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}


			ImGui::DragFloat("toolInfo.miningPower", &toolInfo.miningPower, 0.1f);
			ImGui::DragFloat("toolInfo.miningSpeed", &toolInfo.miningSpeed, 0.1f);

			// 読み込み
			if (ImGui::Button("Load"))
			{
				App::Data::Item::Load(toolID_);
				toolInfo = *App::Data::Item::Get(toolID_);
			}
			// 保存
			if (ImGui::Button("Save"))
			{
				App::Data::Item::Save(toolID_, toolInfo);
			}

			break;
		}
		case ItemGenre::Block:
		{
			if (ImGui::BeginCombo("BlockID", magic_enum::enum_name(blockID_).data()))
			{
				for (std::size_t i = 0; i < blockIDValues.size(); i++)
				{
					BlockID value = blockIDValues[i];
					bool selected = (blockID_ == value);

					if (ImGui::Selectable(blockIDNames[i].data(), selected))
					{
						blockID_ = value;
					}
					if (selected) ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			ImGui::ColorEdit4("Color", &color_.x);
			ImGui::DragFloat("blockInfo.miningPoint", &blockInfo.miningPoint, 0.1f);
			ImGui::DragFloat("blockInfo.durability", &blockInfo.durability, 0.1f);

			// 読み込み
			if (ImGui::Button("Load"))
			{
				App::Data::Item::Load(blockID_);
				blockInfo = *App::Data::Item::Get(blockID_);
				color_ = Game::Math::Converter::UintToVector4(blockInfo.color);
			}
			// 保存
			if (ImGui::Button("Save"))
			{
				blockInfo.color = Game::Math::Converter::Vector4ToUint(color_);
				if (color_.w < 1.0f) blockInfo.isTransparent = true;
				else blockInfo.isTransparent = false;

				App::Data::Item::Save(blockID_, blockInfo);
			}

			break;
		}
		case ItemGenre::Object:
		{
			if (ImGui::BeginCombo("ObjectID", magic_enum::enum_name(objectID_).data()))
			{
				for (std::size_t i = 0; i < objectIDValues.size(); i++)
				{
					ObjectID value = objectIDValues[i];
					bool selected = (objectID_ == value);

					if (ImGui::Selectable(objectIDNames[i].data(), selected))
					{
						objectID_ = value;
					}
					if (selected) ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			// 読み込み
			if (ImGui::Button("Load"))
			{
				App::Data::Item::Load(objectID_);
				objectInfo = *App::Data::Item::Get(objectID_);
			}
			// 保存
			if (ImGui::Button("Save"))
			{
				App::Data::Item::Save(objectID_, objectInfo);
			}

			break;
		}
		default:
			break;
		}

		ImGui::TreePop();
	}

	ImGui::Separator();

	if (ImGui::TreeNodeEx("親データ作成", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// ミニプレビュー画面 兼 フルスクボタン
		if (ImGui::ImageButton("##ss", ImTextureID(Game::Asset::RenderTexture::GetRenderTextureGPUPtr(renderTextureID_)), ImVec2(128, 128)))
		{
			fullscreen_ = !fullscreen_;
		}

		// テクスチャ・モデルの選択
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DAD_TEXTURE_ID"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int32_t));
				textureID_ = *reinterpret_cast<const int32_t*>(payload->Data);
			}
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DAD_MODEL_ID"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int32_t));
				renderObject_->modelID_ = *reinterpret_cast<const int32_t*>(payload->Data);
			}
			ImGui::EndDragDropTarget();
		}

		// アイテムIDを選ぶ
		if (ImGui::BeginCombo("ItemList", magic_enum::enum_name(itemID_).data()))
		{
			for (std::size_t i = 0; i < itemIDValues.size(); i++)
			{
				ItemID value = itemIDValues[i];
				bool selected = (itemID_ == value);

				if (ImGui::Selectable(itemIDNames[i].data(), selected))
				{
					itemID_ = value;
				}
				if (selected) ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		// アイテム情報の編集
		if (itemID_ != ItemID::MAX)
		{
			// モデルパス
			const ModelData* modelData = Game::Asset::Model::GetData(renderObject_->modelID_);
			std::string modelName = modelData ? modelData->filePath : "None";
			ImGui::Text("ModelPath	: %s", modelName.c_str());
			// テクスチャパス
			const TextureData* textureData = Game::Asset::Texture::GetData(textureID_);
			std::string textureName = textureData ? textureData->filePath : "None";
			ImGui::Text("TexturePath: %s", textureName.c_str());

			if (ImGui::BeginCombo("BlockID", magic_enum::enum_name(itemInfo.blockID).data()))
				{
					for (std::size_t i = 0; i < blockIDValues.size(); i++)
					{
						BlockID value = blockIDValues[i];
						bool selected = (itemInfo.blockID == value);

						if (ImGui::Selectable(blockIDNames[i].data(), selected))
						{
							itemInfo.blockID = value;
						}
						if (selected) ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
			if (ImGui::BeginCombo("ToolID", magic_enum::enum_name(itemInfo.toolID).data()))
				{
					for (std::size_t i = 0; i < toolIDValues.size(); i++)
					{
						ToolID value = toolIDValues[i];
						bool selected = (itemInfo.toolID == value);
						if (ImGui::Selectable(toolIDNames[i].data(), selected))
						{
							itemInfo.toolID = value;
						}
						if (selected) ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			if (ImGui::BeginCombo("ObjectID", magic_enum::enum_name(itemInfo.objectID).data()))
				{
					for (std::size_t i = 0; i < objectIDValues.size(); i++)
					{
						ObjectID value = objectIDValues[i];
						bool selected = (itemInfo.objectID == value);
						if (ImGui::Selectable(objectIDNames[i].data(), selected))
						{
							itemInfo.objectID = value;
						}
						if (selected) ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

			ImGui::DragInt("MaxStackCount", &itemInfo.maxStackCount, 1.0f, 1, 99999);

			// 読み込み
			if (ImGui::Button("Load"))
			{
				App::Data::Item::Load(itemID_);
				itemInfo = *App::Data::Item::Get(itemID_);
				Game::Camera::Setter::DistanceTarget(itemInfo.iconCamera.radius, 0.0f, EaseType::IN_BACK, cameraID_);
				Game::Camera::Setter::ThetaTarget(itemInfo.iconCamera.theta, 0.0f, EaseType::IN_BACK, cameraID_);
				Game::Camera::Setter::PhiTarget(itemInfo.iconCamera.phi, 0.0f, EaseType::IN_BACK, cameraID_);
				Game::Camera::Setter::CenterTarget(itemInfo.cameraPos, 0.0f, EaseType::IN_BACK, cameraID_);
				textureID_ = itemInfo.textureID;
				renderObject_->modelID_ = itemInfo.modelID;
			}
			ImGui::SameLine();
			// 保存
			if (ImGui::Button("Save"))
			{
				itemInfo.textureID = textureID_;
				itemInfo.modelID = renderObject_->modelID_;
				itemInfo.iconCamera.radius = Game::Camera::Getter::GetDistance(cameraID_);
				itemInfo.iconCamera.theta = Game::Camera::Getter::GetTheta(cameraID_);
				itemInfo.iconCamera.phi = Game::Camera::Getter::GetPhi(cameraID_);
				itemInfo.cameraPos = Game::Camera::Getter::GetCenter(cameraID_);

				App::Data::Item::Save(itemID_, itemInfo);
			}
		}

		ImGui::TreePop();
	}

	// フルスクリーン表示
	if (fullscreen_)
	{
		ImGui::Begin("Item Preview");

		ImGui::Image(ImTextureID(Game::Asset::RenderTexture::GetRenderTextureGPUPtr(renderTextureID_)), ImVec2(512, 512));

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DAD_TEXTURE_ID"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int32_t));
				textureID_ = *reinterpret_cast<const int32_t*>(payload->Data);
			}
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DAD_MODEL_ID"))
			{
				IM_ASSERT(payload->DataSize == sizeof(int32_t));
				renderObject_->modelID_ = *reinterpret_cast<const int32_t*>(payload->Data);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::End();
	}

	ImGui::End();
}

void ItemEditor::DrawIconImGui()
{
	//ImGui::Begin("Item Icon");

	//// 調整するアイテムを選ぶ
	//auto itemIDValues = magic_enum::enum_values<ItemID>();
	//auto itemIDNames = magic_enum::enum_names<ItemID>();
	//const char* currentName = (iconItemID_ == ItemID::MAX) ? "None" : magic_enum::enum_name(iconItemID_).data();
	//if (ImGui::BeginCombo("ItemID", currentName))
	//{
	//	for (std::size_t i = 0; i < itemIDValues.size(); i++)
	//	{
	//		ItemID value = itemIDValues[i];
	//		if (value == ItemID::MAX) continue;
	//		const ItemInfo* info = App::Data::Item::Get(value);
	//		if (!info) continue;

	//		bool selected = (iconItemID_ == value);
	//		if (ImGui::Selectable(itemIDNames[i].data(), selected))
	//		{
	//			iconItemID_ = value;
	//			iconItemInfo_ = *info;
	//		}
	//		if (selected) ImGui::SetItemDefaultFocus();
	//	}
	//	ImGui::EndCombo();
	//}

	//if (iconItemID_ != ItemID::MAX)
	//{
	//	// プレビュー(ゲーム内のアイコンと同じ写り方)
	//	if (iconPreviewDrawn_)
	//	{
	//		ImGui::Image(ImTextureID(Game::Asset::RenderTexture::GetRenderTextureGPUPtr(iconPreviewTextureID_)), ImVec2(256, 256));
	//	}

	//	// カメラ。表示は度、中身はラジアン
	//	ImGui::SliderAngle("Theta", &iconItemInfo_.iconCamera.theta, -180.0f, 180.0f);
	//	ImGui::SliderAngle("Phi", &iconItemInfo_.iconCamera.phi, -89.0f, 89.0f);
	//	ImGui::DragFloat("Distance", &iconItemInfo_.iconCamera.radius, 0.01f, 0.0f, 100.0f);

	//	// 保存するとゲーム内のアイコンも描き直される
	//	if (ImGui::Button("Save"))
	//	{
	//		App::Data::Item::Save(iconItemID_, iconItemInfo_);
	//	}
	//	ImGui::SameLine();
	//	// 保存してある値に戻す
	//	if (ImGui::Button("Reset"))
	//	{
	//		if (const ItemInfo* info = App::Data::Item::Get(iconItemID_))
	//		{
	//			iconItemInfo_ = *info;
	//		}
	//	}
	//}

	//ImGui::End();
}