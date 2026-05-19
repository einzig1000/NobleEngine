#include "ResourceLoader/ResourceID.h"
#include "Game.h"

// 全てのアイテムのテクスチャID配列（インベントリとかに表示するアイコン）
std::vector<int> ResourceID::ItemIconTextureIDs_;
// 全てのアイテムのテクスチャID配列（モデルとして描画する用）
std::vector<int> ResourceID::ItemModelTextureIDs_;
// 全てのアイテムのモデルID配列
std::vector<int> ResourceID::ItemModelIDs_;


// UIテクスチャID配列
std::vector<int> ResourceID::UITextureIDs_;

// その他のテクスチャID配列
std::vector<int> ResourceID::TextureIDs_;
// その他のモデルID配列
std::vector<int> ResourceID::ModelIDs_;

// 数字
std::vector<int> ResourceID::NumberTextureIDs_;

void ResourceID::reload()
{
	int tmp = -1;

#pragma region 全てのアイテムのテクスチャID配列（インベントリとかに表示するアイコン）

	ItemIconTextureIDs_.resize(static_cast<size_t>(ItemID::MAX));

	// 一旦モデル用のテクスチャを流用

	tmp = -1;
	ItemIconTextureIDs_[size_t(ItemID::None)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/tool/sword/woodSwordIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::木の剣)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/tool/sword/ironSwordIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::石の剣)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/tool/sword/ironSwordIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::鉄の剣)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/tool/sword/diamondSwordIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::ダイヤの剣)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/tool/pickel/woodPickelIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::木のツルハシ)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/tool/pickel/stonePickelIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::石のツルハシ)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/tool/pickel/ironPickelIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::鉄のツルハシ)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/tool/pickel/diamondPickelIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::ダイヤのツルハシ)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/tool/axe/woodAxeIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::木の斧)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/tool/axe/stoneAxeIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::石の斧)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/tool/axe/ironAxeIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::鉄の斧)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/tool/axe/diamondAxeIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::ダイヤの斧)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/armor/texture/ironHeadIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::鉄の頭)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/armor/texture/ironBodyIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::鉄の胴)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/armor/texture/ironLegIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::鉄の脚)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/armor/texture/ironFootIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::鉄の靴)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/armor/texture/diamondHeadIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::ダイヤの頭)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/armor/texture/diamondBodyIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::ダイヤの胴)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/armor/texture/diamondLegIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::ダイヤの脚)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/armor/texture/diamondFootIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::ダイヤの靴)] = tmp;


	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/glassIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::ガラスブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/LeafIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::葉ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/LogIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::原木ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/PlankIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::木材ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/LawnIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::芝ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/dirtIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::土ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/stoneIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::石ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/ironIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::鉄ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/goldIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::金ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/diamondIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::ダイヤブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/bedrockIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::岩盤ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/craftTableIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::作業台ブロック)] = tmp;


	tmp = Game::Resource::LoadTexture("resources/Minecraft/item/stick/stickIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::棒)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/item/ingot/ironIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::鉄インゴット)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/item/ingot/goldIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::金インゴット)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/item/diamond/diamondIcon.png");
	ItemIconTextureIDs_[size_t(ItemID::ダイヤモンド)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Prototypes/texture/cube.png");
	ItemIconTextureIDs_[size_t(ItemID::ビーコン)] = tmp;



#pragma endregion

#pragma region 全てのアイテムのモデルID配列

	ItemModelIDs_.resize(static_cast<size_t>(ItemID::MAX));

	tmp = -1;
	ItemModelIDs_[size_t(ItemID::None)] = tmp;

	tmp = Game::Resource::LoadModel("resources/Minecraft/Item/tool/sword/", "sword.obj");
	ItemModelIDs_[size_t(ItemID::木の剣)] = tmp;
	ItemModelIDs_[size_t(ItemID::石の剣)] = tmp;
	ItemModelIDs_[size_t(ItemID::鉄の剣)] = tmp;
	ItemModelIDs_[size_t(ItemID::ダイヤの剣)] = tmp;

	tmp = Game::Resource::LoadModel("resources/Minecraft/Item/tool/pickel/", "pickel.obj");
	ItemModelIDs_[size_t(ItemID::木のツルハシ)] = tmp;
	ItemModelIDs_[size_t(ItemID::石のツルハシ)] = tmp;
	ItemModelIDs_[size_t(ItemID::鉄のツルハシ)] = tmp;
	ItemModelIDs_[size_t(ItemID::ダイヤのツルハシ)] = tmp;

	tmp = Game::Resource::LoadModel("resources/Minecraft/Item/tool/axe/", "axe.obj");
	ItemModelIDs_[size_t(ItemID::木の斧)] = tmp;
	ItemModelIDs_[size_t(ItemID::石の斧)] = tmp;
	ItemModelIDs_[size_t(ItemID::鉄の斧)] = tmp;
	ItemModelIDs_[size_t(ItemID::ダイヤの斧)] = tmp;

	tmp = Game::Resource::LoadModel("resources/Prototypes/model/", "cube.obj");
	ItemModelIDs_[size_t(ItemID::ガラスブロック)] = tmp;
	ItemModelIDs_[size_t(ItemID::葉ブロック)] = tmp;
	ItemModelIDs_[size_t(ItemID::原木ブロック)] = tmp;
	ItemModelIDs_[size_t(ItemID::木材ブロック)] = tmp;
	ItemModelIDs_[size_t(ItemID::芝ブロック)] = tmp;
	ItemModelIDs_[size_t(ItemID::土ブロック)] = tmp;
	ItemModelIDs_[size_t(ItemID::石ブロック)] = tmp;
	ItemModelIDs_[size_t(ItemID::鉄ブロック)] = tmp;
	ItemModelIDs_[size_t(ItemID::金ブロック)] = tmp;
	ItemModelIDs_[size_t(ItemID::ダイヤブロック)] = tmp;
	ItemModelIDs_[size_t(ItemID::岩盤ブロック)] = tmp;
	ItemModelIDs_[size_t(ItemID::作業台ブロック)] = tmp;

	tmp = Game::Resource::LoadModel("resources/Minecraft/item/ingot/", "ingot.obj");
	ItemModelIDs_[size_t(ItemID::鉄インゴット)] = tmp;
	ItemModelIDs_[size_t(ItemID::金インゴット)] = tmp;

	tmp = Game::Resource::LoadModel("resources/Minecraft/item/diamond/", "diamond.obj");
	ItemModelIDs_[size_t(ItemID::ダイヤモンド)] = tmp;

	//tmp = Game::Resource::LoadModel("resources/Minecraft/block/beacon/", "beacon.obj");
	tmp = Game::Resource::LoadModel("resources/Prototypes/model/", "cube.obj");
	ItemModelIDs_[size_t(ItemID::ビーコン)] = tmp;

#pragma endregion

#pragma region 全てのアイテムのテクスチャID配列（モデルとして描画する用）

	ItemModelTextureIDs_.resize(static_cast<size_t>(ItemID::MAX));

	tmp = -1;
	ItemModelTextureIDs_[size_t(ItemID::None)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/tool/sword/sword.png");
	ItemModelTextureIDs_[size_t(ItemID::木の剣)] = tmp;
	ItemModelTextureIDs_[size_t(ItemID::石の剣)] = tmp;
	ItemModelTextureIDs_[size_t(ItemID::鉄の剣)] = tmp;
	ItemModelTextureIDs_[size_t(ItemID::ダイヤの剣)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/tool/pickel/pickel.png");
	ItemModelTextureIDs_[size_t(ItemID::木のツルハシ)] = tmp;
	ItemModelTextureIDs_[size_t(ItemID::石のツルハシ)] = tmp;
	ItemModelTextureIDs_[size_t(ItemID::鉄のツルハシ)] = tmp;
	ItemModelTextureIDs_[size_t(ItemID::ダイヤのツルハシ)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/Item/tool/axe/axe.png");
	ItemModelTextureIDs_[size_t(ItemID::木の斧)] = tmp;
	ItemModelTextureIDs_[size_t(ItemID::石の斧)] = tmp;
	ItemModelTextureIDs_[size_t(ItemID::鉄の斧)] = tmp;
	ItemModelTextureIDs_[size_t(ItemID::ダイヤの斧)] = tmp;


	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/craftTable.png");
	ItemModelTextureIDs_[size_t(ItemID::作業台ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/glass.png");
	ItemModelTextureIDs_[size_t(ItemID::ガラスブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/Leaf.png");
	ItemModelTextureIDs_[size_t(ItemID::葉ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/Log.png");
	ItemModelTextureIDs_[size_t(ItemID::原木ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/Plank.png");
	ItemModelTextureIDs_[size_t(ItemID::木材ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/Lawn.png");
	ItemModelTextureIDs_[size_t(ItemID::芝ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/dirt.png");
	ItemModelTextureIDs_[size_t(ItemID::土ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/stone.png");
	ItemModelTextureIDs_[size_t(ItemID::石ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/iron.png");
	ItemModelTextureIDs_[size_t(ItemID::鉄ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/gold.png");
	ItemModelTextureIDs_[size_t(ItemID::金ブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/diamond.png");
	ItemModelTextureIDs_[size_t(ItemID::ダイヤブロック)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/Blocks/bedrock.png");
	ItemModelTextureIDs_[size_t(ItemID::岩盤ブロック)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/item/stick/stick.png");
	ItemModelTextureIDs_[size_t(ItemID::棒)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/item/ingot/iron.png");
	ItemModelTextureIDs_[size_t(ItemID::鉄インゴット)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/item/ingot/gold.png");
	ItemModelTextureIDs_[size_t(ItemID::金インゴット)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/item/diamond/diamond.png");
	ItemModelTextureIDs_[size_t(ItemID::ダイヤモンド)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/blocks/beacon.png");
	ItemModelTextureIDs_[size_t(ItemID::ビーコン)] = tmp;

#pragma endregion

#pragma region テクスチャ読み込み

	TextureIDs_.resize(static_cast<size_t>(TextureID::MAX));

	tmp = -1;
	TextureIDs_[size_t(TextureID::None)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Prototypes/texture/uvChecker.png");
	TextureIDs_[size_t(TextureID::UVChecker)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Prototypes/texture/monsterBall.png");
	TextureIDs_[size_t(TextureID::monsterBall)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Prototypes/texture/white1x1.png");
	TextureIDs_[size_t(TextureID::white1x1)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Prototypes/texture/particle/circle.png");
	TextureIDs_[size_t(TextureID::Circle)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Prototypes/texture/font/font_0.png");
	TextureIDs_[size_t(TextureID::font_0)] = tmp;

#pragma endregion

#pragma region モデル読み込み

	ModelIDs_.resize(static_cast<size_t>(ModelID::MAX));

	tmp = -1;
	ModelIDs_[size_t(ModelID::None)] = tmp;

	tmp = Game::Resource::LoadModel("resources/Prototypes/model/", "cube.obj");
	ModelIDs_[size_t(ModelID::Cube)] = tmp;
	tmp = Game::Resource::LoadModel("resources/Prototypes/model/", "plane.obj");
	ModelIDs_[size_t(ModelID::Plane)] = tmp;
	tmp = Game::Resource::LoadModel("resources/Prototypes/model/", "corn.obj");
	ModelIDs_[size_t(ModelID::Corn)] = tmp;
	tmp = Game::Resource::LoadModel("resources/Prototypes/model/", "sphere.obj");
	ModelIDs_[size_t(ModelID::Sphere)] = tmp;

	tmp = Game::Resource::LoadModel("resources/Prototypes/model/", "pig.obj");
	ModelIDs_[size_t(ModelID::Pig)] = tmp;

#pragma endregion

#pragma region UIテクスチャ読み込み

	UITextureIDs_.resize(static_cast<size_t>(UITextureID::MAX));
	tmp = -1;
	UITextureIDs_[size_t(UITextureID::None)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/UI/Inventory/Inventory2x2.png");
	UITextureIDs_[size_t(UITextureID::Inventory2x2)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/UI/Inventory/Inventory3x3.png");
	UITextureIDs_[size_t(UITextureID::Inventory3x3)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/UI/Inventory/Hotbar.png");
	UITextureIDs_[size_t(UITextureID::Hotbar)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/UI/Inventory/Hotbar_Selected.png");
	UITextureIDs_[size_t(UITextureID::Hotbar_Selected)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/UI/Title/title.png");
	UITextureIDs_[size_t(UITextureID::TITLE_logo)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/UI/Title/slot.png");
	UITextureIDs_[size_t(UITextureID::TITLE_slot)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/UI/Title/NewWorldUI.png");
	UITextureIDs_[size_t(UITextureID::TITLE_newWorldUI)] = tmp;

	tmp = Game::Resource::LoadTexture("resources/Minecraft/UI/Pause/pause_menu.png");
	UITextureIDs_[size_t(UITextureID::PAUSE_menu)] = tmp;
	tmp = Game::Resource::LoadTexture("resources/Minecraft/UI/Pause/SaveButton.png");
	UITextureIDs_[size_t(UITextureID::PAUSE_SaveButton)] = tmp;

#pragma endregion

#pragma region 数字テクスチャ読み込み

	NumberTextureIDs_.resize(10);
	for (int i = 0; i <= 9; ++i)
	{
		std::string path = "resources/Prototypes/texture/font/" + std::to_string(i) + ".png";
		tmp = Game::Resource::LoadTexture(path);
		NumberTextureIDs_[i] = tmp;
	}

#pragma endregion

	std::vector<std::string> breakBlockFilePaths =
	{
		"resources/Minecraft/Blocks/breakBlock/breakBlock_0.png",
		"resources/Minecraft/Blocks/breakBlock/breakBlock_1.png",
		"resources/Minecraft/Blocks/breakBlock/breakBlock_2.png",
		"resources/Minecraft/Blocks/breakBlock/breakBlock_3.png",
		"resources/Minecraft/Blocks/breakBlock/breakBlock_4.png",
		"resources/Minecraft/Blocks/breakBlock/breakBlock_5.png",
	};

	tmp = Game::Resource::LoadTextureArray(breakBlockFilePaths);
	TextureIDs_[size_t(TextureID::BreakBlock_Array)] = tmp;

}