#include "ResourceID.h"
#include <Game.h>

// 全てのアイテムのテクスチャID配列（インベントリとかに表示するアイコン）
std::vector<int32_t> ResourceID::ItemIconTextureIDs_;
// 全てのアイテムのテクスチャID配列（モデルとして描画する用）
std::vector<int32_t> ResourceID::ItemModelTextureIDs_;
// 全てのアイテムのモデルID配列
std::vector<int32_t> ResourceID::ItemModelIDs_;


// UIテクスチャID配列
std::vector<int32_t> ResourceID::UITextureIDs_;

// その他のテクスチャID配列
std::vector<int32_t> ResourceID::TextureIDs_;
// その他のモデルID配列
std::vector<int32_t> ResourceID::ModelIDs_;

// 数字
std::vector<int32_t> ResourceID::NumberTextureIDs_;

void ResourceID::reload()
{
	int32_t tmp = -1;

#pragma region //全てのアイテムのテクスチャID配列（インベントリとかに表示するアイコン）

	ItemIconTextureIDs_.resize(static_cast<size_t>(ItemID::MAX));

	// 一旦モデル用のテクスチャを流用

	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Item/tool/sword/woodSwordIcon.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Item/tool/sword/ironSwordIcon.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Item/tool/sword/ironSwordIcon.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Item/tool/sword/diamondSwordIcon.png");

	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Item/tool/pickel/woodPickelIcon.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Item/tool/pickel/stonePickelIcon.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Item/tool/pickel/ironPickelIcon.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Item/tool/pickel/diamondPickelIcon.png");

	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Item/tool/axe/woodAxeIcon.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Item/tool/axe/stoneAxeIcon.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Item/tool/axe/ironAxeIcon.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Item/tool/axe/diamondAxeIcon.png");

	//tmp = Game::Asset::Texture::Load("assets/Application/Minecraft/Item/armor/texture/ironHeadIcon.png");
	//tmp = Game::Asset::Texture::Load("assets/Application/Minecraft/Item/armor/texture/ironBodyIcon.png");
	//tmp = Game::Asset::Texture::Load("assets/Application/Minecraft/Item/armor/texture/ironLegIcon.png");
	//tmp = Game::Asset::Texture::Load("assets/Application/Minecraft/Item/armor/texture/ironFootIcon.png");

	//tmp = Game::Asset::Texture::Load("assets/Application/Minecraft/Item/armor/texture/diamondHeadIcon.png");
	//tmp = Game::Asset::Texture::Load("assets/Application/Minecraft/Item/armor/texture/diamondBodyIcon.png");
	//tmp = Game::Asset::Texture::Load("assets/Application/Minecraft/Item/armor/texture/diamondLegIcon.png");
	//tmp = Game::Asset::Texture::Load("assets/Application/Minecraft/Item/armor/texture/diamondFootIcon.png");


	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/glassIcon.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/LeafIcon.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/LogIcon.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/LawnIcon.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/dirtIcon.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/stoneIcon.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/ironIcon.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/goldIcon.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/diamondIcon.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/bedrockIcon.png");
	//tmp = Game::Asset::Texture::Load("assets/Application/Minecraft/Blocks/craftTableIcon.png");
	//ItemIconTextureIDs_[size_t(ItemID::Block_Crafting_Table)] = tmp;


	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/item/ingot/ironIcon.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/item/ingot/goldIcon.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/item/diamond/diamondIcon.png");



#pragma endregion

#pragma region 全てのアイテムのモデルID配列


	tmp = Game::Asset::Model::Load("assets/application/Minecraft/Item/tool/pickel/pickel.obj");
	tmp = Game::Asset::Model::Load("assets/application/Minecraft/Item/tool/sword/sword.obj");
	tmp = Game::Asset::Model::Load("assets/application/Minecraft/Item/tool/axe/axe.obj");
	tmp = Game::Asset::Model::Load("assets/engine/model/cube/cube.obj");
	tmp = Game::Asset::Model::Load("assets/application/Minecraft/item/ingot/ingot.obj");
	tmp = Game::Asset::Model::Load("assets/application/Minecraft/item/diamond/diamond.obj");
	tmp = Game::Asset::Model::Load("assets/engine/model/cube/cube.obj");

#pragma endregion

#pragma region //全てのアイテムのテクスチャID配列（モデルとして描画する用）


	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Item/tool/sword/sword.png");

	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Item/tool/pickel/pickel.png");

	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Item/tool/axe/axe.png");


	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/craftTable.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/glass.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/Leaf.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/Log.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/Plank.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/Lawn.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/dirt.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/stone.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/iron.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/gold.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/diamond.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/Blocks/bedrock.png");

	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/item/stick/stick.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/item/ingot/iron.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/item/ingot/gold.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/item/diamond/diamond.png");

	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/blocks/beacon.png");

#pragma endregion

#pragma region テクスチャ読み込み

	tmp = Game::Asset::Texture::Load("assets/engine/texture/uvChecker.png");
	tmp = Game::Asset::Texture::Load("assets/engine/texture/monsterBall.png");
	tmp = Game::Asset::Texture::Load("assets/engine/texture/white1x1.png");
	tmp = Game::Asset::Texture::Load("assets/engine/texture/particle/circle.png");


#pragma endregion

#pragma region モデル読み込み

	tmp = Game::Asset::Model::Load("assets/engine/model/cube/cube.obj");
	tmp = Game::Asset::Model::Load("assets/engine/model/plane/plane.obj");
	tmp = Game::Asset::Model::Load("assets/engine/model/corn/corn.obj");
	tmp = Game::Asset::Model::Load("assets/engine/model/sphere/sphere.obj");
	tmp = Game::Asset::Model::Load("assets/engine/model/pig.obj");

#pragma endregion

#pragma region UIテクスチャ読み込み

	UITextureIDs_.resize(static_cast<size_t>(UITextureID::MAX));
	tmp = -1;
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/UI/Inventory/Inventory2x2.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/UI/Inventory/Inventory3x3.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/UI/Inventory/Hotbar.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/UI/Inventory/Hotbar_Selected.png");

	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/UI/Title/title.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/UI/Title/slot.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/UI/Title/NewWorldUI.png");

	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/UI/Pause/pause_menu.png");
	tmp = Game::Asset::Texture::Load("assets/application/Minecraft/UI/Pause/SaveButton.png");

#pragma endregion

#pragma region 数字テクスチャ読み込み

}