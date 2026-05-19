#pragma once
#include "definition/definition.h"
#include <vector>
#include <unordered_map>


enum class ModelID
{
	None,
	Cube,
	Plane,
	Corn,
	Sphere,

	Pig,

	MAX,
};

enum class TextureID
{
	None,
	UVChecker,
	monsterBall,
	white1x1,
	empty1x1,
	Circle,


	BreakBlock_Array,

	// 文字テクスチャ
	font_0,


	MAX,
};

enum class UITextureID
{
	None,

	Inventory2x2,
	Inventory3x3,

	Hotbar,
	Hotbar_Selected,

	TITLE_logo,
	TITLE_slot,
	TITLE_newWorldUI,

	PAUSE_menu,
	PAUSE_SaveButton,

	MAX,
};

class ResourceID
{
public:

	static void reload();


	// UIテクスチャID取得
	static int32_t GetUITextureID(UITextureID id)
	{
		return UITextureIDs_[static_cast<size_t>(id)];
	}

	// 数字
	static int32_t GetNumberTextureID(int number)
	{
		if (number < 0 || number > 9)
		{
			return -1;
		}
		return NumberTextureIDs_[static_cast<size_t>(number)];
	}

	// アイテム
	static int32_t Get2DTextureID(BlockID id)
	{
		ItemID ID = BlockIDToItemID(id);
		return Get2DTextureID(ID);
	}
	static int32_t Get3DTextureID(BlockID id)
	{
		ItemID ID = BlockIDToItemID(id);
		return Get3DTextureID(ID);
	}
	static int32_t GetModelID(BlockID id)
	{
		ItemID ID = BlockIDToItemID(id);
		return GetModelID(ID);
	}
	static int32_t Get3DTextureID(ItemID id)
	{
		return ItemModelTextureIDs_[static_cast<size_t>(id)];
	}
	static int32_t Get2DTextureID(ItemID id)
	{
		return ItemIconTextureIDs_[static_cast<size_t>(id)];
	}
	static int32_t GetModelID(ItemID id)
	{
		return ItemModelIDs_[static_cast<size_t>(id)];
	}

	// その他
	static int32_t GetTextureID(TextureID id)
	{
		return TextureIDs_[static_cast<size_t>(id)];
	}
	static int32_t GetModelID(ModelID id)
	{
		return ModelIDs_[static_cast<size_t>(id)];
	}



private:

	// 全てのアイテムのテクスチャID配列（インベントリとかに表示するアイコン）
	static std::vector<int> ItemIconTextureIDs_;
	// 全てのアイテムのテクスチャID配列（モデルとして描画する用）
	static std::vector<int> ItemModelTextureIDs_;
	// 全てのアイテムのモデルID配列
	static std::vector<int> ItemModelIDs_;
	

	// UIテクスチャID配列
	static std::vector<int> UITextureIDs_;

	// その他のテクスチャID配列
	static std::vector<int> TextureIDs_;
	// その他のモデルID配列
	static std::vector<int> ModelIDs_;

	// 数字
	static std::vector<int> NumberTextureIDs_;

	static inline const std::unordered_map<TextureID, std::string> textureFilePaths_ =
	{
	{ TextureID::UVChecker,    "resources/Prototypes/texture/uvChecker.png" },
	{ TextureID::monsterBall,  "resources/Prototypes/texture/monsterBall.png" },
	{ TextureID::white1x1,     "resources/Prototypes/texture/white1x1.png" },
	{ TextureID::empty1x1,     "resources/Prototypes/texture/empty1x1.png" },
	{ TextureID::Circle,       "resources/Prototypes/texture/circle.png" },
	};

	ResourceID() = delete;
	~ResourceID() = delete;

};