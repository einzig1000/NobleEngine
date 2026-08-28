#include "ItemEditor.h"
#include <ResourceLoader/Data/DataManager.h>
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
	Game::Camera::Setter::SetScreenSize(Vector2(512, 512), 0, EaseType::IN_BACK, cameraID_);
}

ItemEditor::~ItemEditor()
{

}

void ItemEditor::Update()
{
	Game::Camera::Update(cameraID_);

	Matrix4x4 world = Matrix4x4::MakeAffineMatrix(transforms_.scale, transforms_.rotate, transforms_.translate);
	Matrix4x4 wpv = world * Game::Camera::Getter::GetViewProjectionMatrix(cameraID_);
	renderObject_->SetCBufferData(0, ShaderType::PixelShader, &color_);
	renderObject_->SetCBufferData(1, ShaderType::PixelShader, &textureID_);
	renderObject_->SetCBufferData(0, ShaderType::VertexShader, &wpv);
	renderObject_->SetCBufferData(1, ShaderType::VertexShader, &world);
}

void ItemEditor::Draw()
{
	if (renderObject_->modelID_ > 0 && textureID_ > 0)
	{
		renderObject_->Draw(renderTextureID_);
	}
}

void ItemEditor::DrawImGui()
{
	ImGui::Begin("Item Editor");

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
		const ModelData* modelData = Game::Asset::Model::GetData(renderObject_->modelID_);
		std::string modelName = modelData ? modelData->filePath : "None";
		ImGui::Text("ModelPath	: %s", modelName.c_str());

		const TextureData* textureData = Game::Asset::Texture::GetData(textureID_);
		std::string textureName = textureData ? textureData->filePath : "None";
		ImGui::Text("TexturePath: %s", textureName.c_str());

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

		const ModelData* modelData = Game::Asset::Model::GetData(renderObject_->modelID_);
		std::string modelName = modelData ? modelData->filePath : "None";
		ImGui::Text("ModelPath	: %s", modelName.c_str());

		const TextureData* textureData = Game::Asset::Texture::GetData(textureID_);
		std::string textureName = textureData ? textureData->filePath : "None";
		ImGui::Text("TexturePath: %s", textureName.c_str());

		ImGui::DragFloat("toolInfo.durability", &toolInfo.durability);
		ImGui::DragFloat("toolInfo.attackPower", &toolInfo.attackPower);
		ImGui::DragFloat("toolInfo.miningSpeed", &toolInfo.miningSpeed);

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
		ImGui::DragFloat("blockInfo.durability", &blockInfo.durability);

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

		const ModelData* modelData = Game::Asset::Model::GetData(renderObject_->modelID_);
		std::string modelName = modelData ? modelData->filePath : "None";
		ImGui::Text("ModelPath	: %s", modelName.c_str());

		const TextureData* textureData = Game::Asset::Texture::GetData(textureID_);
		std::string textureName = textureData ? textureData->filePath : "None";
		ImGui::Text("TexturePath: %s", textureName.c_str());

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


	// 全表示
	ImGui::BeginChild("ItemListChild", ImVec2(250, 200), true, ImGuiWindowFlags_HorizontalScrollbar);
	for (int32_t i = 0; i < static_cast<int32_t>(ItemID::MAX); i++)
	{
		ItemID itemID = static_cast<ItemID>(i);
		const ItemInfo* info = App::Data::Item::Get(itemID);
		ItemInfo tempInfo = *info;

		if (info && ImGui::TreeNode(itemIDNames[i].data()))
		{
			if (ImGui::BeginCombo("BlockID", magic_enum::enum_name(tempInfo.blockID).data()))
			{
				for (std::size_t i = 0; i < blockIDValues.size(); i++)
				{
					BlockID value = blockIDValues[i];
					bool selected = (tempInfo.blockID == value);

					if (ImGui::Selectable(blockIDNames[i].data(), selected))
					{
						tempInfo.blockID = value;
						App::Data::Item::Save(itemID, tempInfo);
					}
					if (selected) ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
			if (ImGui::BeginCombo("ToolID", magic_enum::enum_name(tempInfo.toolID).data()))
			{
				for (std::size_t i = 0; i < toolIDValues.size(); i++)
				{
					ToolID value = toolIDValues[i];
					bool selected = (tempInfo.toolID == value);
					if (ImGui::Selectable(toolIDNames[i].data(), selected))
					{
						tempInfo.toolID = value;
						App::Data::Item::Save(itemID, tempInfo);
					}
					if (selected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			if (ImGui::BeginCombo("ObjectID", magic_enum::enum_name(tempInfo.objectID).data()))
			{
				for (std::size_t i = 0; i < objectIDValues.size(); i++)
				{
					ObjectID value = objectIDValues[i];
					bool selected = (tempInfo.objectID == value);
					if (ImGui::Selectable(objectIDNames[i].data(), selected))
					{
						tempInfo.objectID = value;
						App::Data::Item::Save(itemID, tempInfo);
					}
					if (selected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::Text("BlockID: %s", blockIDNames[static_cast<size_t>(tempInfo.blockID)].data());
			ImGui::Text("ToolID: %s", toolIDNames[static_cast<size_t>(tempInfo.toolID)].data());
			ImGui::Text("ObjectID: %s", objectIDNames[static_cast<size_t>(tempInfo.objectID)].data());


			ImGui::TreePop();
		}
	}
	ImGui::EndChild();


	if (ImGui::Button("AllSave"))
	{
		for (BlockID id : magic_enum::enum_values<BlockID>())
		{
			if (id == BlockID::MAX) continue;
			const BlockInfo* info = App::Data::Item::Get(id);
			if (info) App::Data::Item::Save(id, *info);
		}

		for (ToolID id : magic_enum::enum_values<ToolID>())
		{
			if (id == ToolID::MAX) continue;
			const ToolInfo* info = App::Data::Item::Get(id);
			if (info) App::Data::Item::Save(id, *info);
		}

		for (ObjectID id : magic_enum::enum_values<ObjectID>())
		{
			if (id == ObjectID::MAX) continue;
			const ObjectInfo* info = App::Data::Item::Get(id);
			if (info) App::Data::Item::Save(id, *info);
		}

		for (ItemID id : magic_enum::enum_values<ItemID>())
		{
			if (id == ItemID::MAX) continue;
			const ItemInfo* info = App::Data::Item::Get(id);
			if (info) App::Data::Item::Save(id, *info);
		}
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
