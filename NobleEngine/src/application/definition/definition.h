#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <EngineDefinition/EngineConstexprs.h>
#include <definition/constexprs.h>
#include <string>
#include <string_view>
#include <stdexcept>


// ゲームのフェーズ
enum class PHASE
{
    Phase_None,
    Phase_Test,
    Phase_Test2,
    Phase_Title,
    Phase_GameScene,
    Phase_StageSelect,
    Phase_GameClear,
};


enum class CharacterID
{
    Player,
	Zombie,
    
    MAX,
};

enum class ItemGenre
{
	None,
	// 防具
	Armor,
	// ツール
	Tool,
	// ブロック
	Block,
	// 構造物(かまどとかの既製品)
	Structure,

	MAX,
};


// 全てのアイテムID　// いずれ廃止。Jsonに名前もstring型で保存する
enum class ItemID
{
	None,

	// ツール
	Tool_Sword_of_Wood,
	Tool_Sword_of_Stone,
	Tool_Sword_of_Iron,
	Tool_Sword_of_Gold,
	Tool_Sword_of_Diamond,

	Tool_Pickel_of_Wood,
	Tool_Pickel_of_Stone,
	Tool_Pickel_of_Iron,
	Tool_Pickel_of_Gold,
	Tool_Pickel_of_Diamond,

	Tool_Axe_of_Wood,
	Tool_Axe_of_Stone,
	Tool_Axe_of_Iron,
	Tool_Axe_of_Gold,
	Tool_Axe_of_Diamond,

	// 防具
	Armor_Head_of_Wood,
	Armor_Head_of_Stone,
	Armor_Head_of_Iron,
	Armor_Head_of_Gold,
	Armor_Head_of_Diamond,

	Armor_Leg_of_Wood,
	Armor_Leg_of_Stone,
	Armor_Leg_of_Iron,
	Armor_Leg_of_Gold,
	Armor_Leg_of_Diamond,

	Armor_Shoes_of_Wood,
	Armor_Shoes_of_Stone,
	Armor_Shoes_of_Iron,
	Armor_Shoes_of_Gold,
	Armor_Shoes_of_Diamond,

	Armor_Body_of_Wood,
	Armor_Body_of_Stone,
	Armor_Body_of_Iron,
	Armor_Body_of_Gold,
	Armor_Body_of_Diamond,

	// ブロック    
	Block_Air,
	Block_Stone,	// 石
	Block_Iron,		// 鉄
	Block_Gold,		// 金
	Block_Diamond,	// ダイヤ
	Block_Bedrock,	// 岩盤
	Block_Glass,	// ガラス
	Block_Dirt,		// 土
	Block_Grass,	// 芝
	Block_Wood,		// 木
	Block_Leaf,		// 葉

	// アイテム
	鉄インゴット,
	金インゴット,
	ダイヤモンド,

    ビーコン,

    MAX,
};

// 全てのブロックID
enum class BlockID
{
    Air,
    Stone,	// 石
    Iron,   // 鉄
    Gold,	// 金
    Diamond,// ダイヤ
    Bedrock,// 岩盤
    Glass,	// ガラス
    Dirt,	// 土
	Grass,	// 芝
	wood,	// 木
    Leaf,	// 葉

    MAX,
};

// BlockID → ItemID 変換
ItemID BlockIDtoItemID(BlockID id);

/////////////////////////////////////////////////
/// BlockInfo・ItemInfoのリストをJsonで管理
/// std::unordered_map<BlockID, BlockInfo> blockInfoList;
/// std::unordered_map<ItemID, ItemInfo> itemInfoList;
/////////////////////////////////////////////////

// ブロックごとの情報
struct BlockInfo
{
	// ブロックID
	BlockID ID = BlockID::Air;
	// 色
	uint32_t color = 0xFFFFFFFF;
	// 耐久値
	float durability = 1.0f;
	// 透過ブロックかどうか
	bool isTransparent = false;
};

struct ToolInfo
{
	// 耐久値
	float durability = 1.0f;
	// 攻撃力
	float attackPower = 1.0f;
	// 採掘速度
	float miningSpeed = 1.0f;
};

// アイテムごとの情報
struct ItemInfo
{
	// アイテムID
	ItemID id = ItemID::None;

	// アイテムジャンル
	ItemGenre genre = ItemGenre::None;

	// ブロックとして扱う時のデータ(ItemGenre::Blockの時のみ有効)
	BlockInfo blockInfo;

	// ツールとして扱う時のデータ(ItemGenre::Toolの時のみ有効)
	ToolInfo toolInfo;

	// バッグに入ってる時のテクスチャ
	std::string texturePath{};
};





// 宣言だけ。未対応のEnumで使うとコンパイルエラーになる
template <class Enum>
struct EnumStringTraits;

template <class Enum>
std::string EnumToString(Enum value)
{
	return EnumStringTraits<Enum>::ToString(value);
}

template <class Enum>
Enum StringToEnum(std::string_view str)
{
	return EnumStringTraits<Enum>::FromString(str);
}

template <>
struct EnumStringTraits<BlockID>
{
	static std::string ToString(BlockID id)
	{
		switch (id)
		{
		case BlockID::Air:		return "Air";	break;
		case BlockID::Stone:	return "Stone"; break;
		case BlockID::Iron:		return "Iron"; break;
		case BlockID::Gold:		return "Gold"; break;
		case BlockID::Diamond:	return "Diamond"; break;
		case BlockID::Bedrock:	return "Bedrock"; break;
		case BlockID::Glass:	return "Glass"; break;
		case BlockID::Dirt:		return "Dirt"; break;
		case BlockID::Grass:	return "Grass"; break;
		case BlockID::wood:		return "wood"; break;
		case BlockID::Leaf:		return "Leaf"; break;
		case BlockID::MAX:		return "unknown"; break;
		}

		return "unknown";
	}
	static BlockID FromString(std::string_view str)
	{
		if (str == "Air")		return BlockID::Air;
		if (str == "Stone")		return BlockID::Stone;
		if (str == "Iron")		return BlockID::Iron;
		if (str == "Gold")		return BlockID::Gold;
		if (str == "Diamond")	return BlockID::Diamond;
		if (str == "Bedrock")	return BlockID::Bedrock;
		if (str == "Glass")		return BlockID::Glass;
		if (str == "Dirt")		return BlockID::Dirt;
		if (str == "Grass")		return BlockID::Grass;
		if (str == "wood")		return BlockID::wood;
		if (str == "Leaf")		return BlockID::Leaf;

		return BlockID::MAX;
	}
};

template <>
struct EnumStringTraits<ItemID>
{
	static std::string ToString(ItemID id)
	{
		switch (id)
		{
		case ItemID::None:
		case ItemID::Tool_Sword_of_Wood:		return "Tool_Sword_of_Wood";	break;
		case ItemID::Tool_Sword_of_Stone:		return "Tool_Sword_of_Stone";	break;
		case ItemID::Tool_Sword_of_Iron:		return "Tool_Sword_of_Iron";	break;
		case ItemID::Tool_Sword_of_Gold:		return "Tool_Sword_of_Gold";	break;
		case ItemID::Tool_Sword_of_Diamond:		return "Tool_Sword_of_Diamond";	break;
		case ItemID::Tool_Pickel_of_Wood:		return "Tool_Pickel_of_Wood";	break;
		case ItemID::Tool_Pickel_of_Stone:		return "Tool_Pickel_of_Stone";	break;
		case ItemID::Tool_Pickel_of_Iron:		return "Tool_Pickel_of_Iron";	break;
		case ItemID::Tool_Pickel_of_Gold:		return "Tool_Pickel_of_Gold";	break;
		case ItemID::Tool_Pickel_of_Diamond:	return "Tool_Pickel_of_Diamond";	break;
		case ItemID::Tool_Axe_of_Wood:			return "Tool_Axe_of_Wood";	break;
		case ItemID::Tool_Axe_of_Stone:			return "Tool_Axe_of_Stone";	break;
		case ItemID::Tool_Axe_of_Iron:			return "Tool_Axe_of_Iron";	break;
		case ItemID::Tool_Axe_of_Gold:			return "Tool_Axe_of_Gold";	break;
		case ItemID::Tool_Axe_of_Diamond:		return "Tool_Axe_of_Diamond";	break;
		case ItemID::Armor_Head_of_Wood:		return "Armor_Head_of_Wood";	break;
		case ItemID::Armor_Head_of_Stone:		return "Armor_Head_of_Stone";	break;
		case ItemID::Armor_Head_of_Iron:		return "Armor_Head_of_Iron";	break;
		case ItemID::Armor_Head_of_Gold:		return "Armor_Head_of_Gold";	break;
		case ItemID::Armor_Head_of_Diamond:		return "Armor_Head_of_Diamond";	break;
		case ItemID::Armor_Leg_of_Wood:			return "Armor_Leg_of_Wood";	break;
		case ItemID::Armor_Leg_of_Stone:		return "Armor_Leg_of_Stone";	break;
		case ItemID::Armor_Leg_of_Iron:			return "Armor_Leg_of_Iron";	break;
		case ItemID::Armor_Leg_of_Gold:			return "Armor_Leg_of_Gold";	break;
		case ItemID::Armor_Leg_of_Diamond:		return "Armor_Leg_of_Diamond";	break;
		case ItemID::Armor_Shoes_of_Wood:		return "Armor_Shoes_of_Wood";	break;
		case ItemID::Armor_Shoes_of_Stone:		return "Armor_Shoes_of_Stone";	break;
		case ItemID::Armor_Shoes_of_Iron:		return "Armor_Shoes_of_Iron";	break;
		case ItemID::Armor_Shoes_of_Gold:		return "Armor_Shoes_of_Gold";	break;
		case ItemID::Armor_Shoes_of_Diamond:	return "Armor_Shoes_of_Diamond";	break;
		case ItemID::Armor_Body_of_Wood:		return "Armor_Body_of_Wood";	break;
		case ItemID::Armor_Body_of_Stone:		return "Armor_Body_of_Stone";	break;
		case ItemID::Armor_Body_of_Iron:		return "Armor_Body_of_Iron";	break;
		case ItemID::Armor_Body_of_Gold:		return "Armor_Body_of_Gold";	break;
		case ItemID::Armor_Body_of_Diamond:		return "Armor_Body_of_Diamond";	break;
		case ItemID::Block_Air:					return "Block_Air";	break;
		case ItemID::Block_Stone:				return "Block_Stone";	break;
		case ItemID::Block_Iron:				return "Block_Iron";	break;
		case ItemID::Block_Gold:				return "Block_Gold";	break;
		case ItemID::Block_Diamond:				return "Block_Diamond";	break;
		case ItemID::Block_Bedrock:				return "Block_Bedrock";	break;
		case ItemID::Block_Glass:				return "Block_Glass";	break;
		case ItemID::Block_Dirt:				return "Block_Dirt";	break;
		case ItemID::Block_Grass:				return "Block_Grass";	break;
		case ItemID::Block_Wood:				return "Block_Wood";	break;
		case ItemID::Block_Leaf:				return "Block_Leaf";	break;
		case ItemID::鉄インゴット:				return "鉄インゴット";	break;
		case ItemID::金インゴット:				return "金インゴット";	break;
		case ItemID::ダイヤモンド:				return "ダイヤモンド";	break;
		case ItemID::ビーコン:					return "ビーコン";	break;
		case ItemID::MAX:
		default:
			break;
		}

		return "unknown";
	}
	static ItemID FromString(std::string_view str)
	{
		if (str == "None")						return ItemID::None;
		if (str == "Tool_Sword_of_Wood")		return ItemID::Tool_Sword_of_Wood;
		if (str == "Tool_Sword_of_Stone")		return ItemID::Tool_Sword_of_Stone;
		if (str == "Tool_Sword_of_Iron")		return ItemID::Tool_Sword_of_Iron;
		if (str == "Tool_Sword_of_Gold")		return ItemID::Tool_Sword_of_Gold;
		if (str == "Tool_Sword_of_Diamond")		return ItemID::Tool_Sword_of_Diamond;

		if (str == "Tool_Pickel_of_Wood")		return ItemID::Tool_Pickel_of_Wood;
		if (str == "Tool_Pickel_of_Stone")		return ItemID::Tool_Pickel_of_Stone;
		if (str == "Tool_Pickel_of_Iron")		return ItemID::Tool_Pickel_of_Iron;
		if (str == "Tool_Pickel_of_Gold")		return ItemID::Tool_Pickel_of_Gold;
		if (str == "Tool_Pickel_of_Diamond")	return ItemID::Tool_Pickel_of_Diamond;

		if (str == "Tool_Axe_of_Wood")			return ItemID::Tool_Axe_of_Wood;
		if (str == "Tool_Axe_of_Stone")			return ItemID::Tool_Axe_of_Stone;
		if (str == "Tool_Axe_of_Iron")			return ItemID::Tool_Axe_of_Iron;
		if (str == "Tool_Axe_of_Gold")			return ItemID::Tool_Axe_of_Gold;
		if (str == "Tool_Axe_of_Diamond")		return ItemID::Tool_Axe_of_Diamond;

		if (str == "Armor_Head_of_Wood")		return ItemID::Armor_Head_of_Wood;
		if (str == "Armor_Head_of_Stone")		return ItemID::Armor_Head_of_Stone;
		if (str == "Armor_Head_of_Iron")		return ItemID::Armor_Head_of_Iron;
		if (str == "Armor_Head_of_Gold")		return ItemID::Armor_Head_of_Gold;
		if (str == "Armor_Head_of_Diamond")		return ItemID::Armor_Head_of_Diamond;

		if (str == "Armor_Leg_of_Wood")			return ItemID::Armor_Leg_of_Wood;
		if (str == "Armor_Leg_of_Stone")		return ItemID::Armor_Leg_of_Stone;
		if (str == "Armor_Leg_of_Iron")			return ItemID::Armor_Leg_of_Iron;
		if (str == "Armor_Leg_of_Gold")			return ItemID::Armor_Leg_of_Gold;
		if (str == "Armor_Leg_of_Diamond")		return ItemID::Armor_Leg_of_Diamond;

		if (str == "Armor_Shoes_of_Wood")		return ItemID::Armor_Shoes_of_Wood;
		if (str == "Armor_Shoes_of_Stone")		return ItemID::Armor_Shoes_of_Stone;
		if (str == "Armor_Shoes_of_Iron")		return ItemID::Armor_Shoes_of_Iron;
		if (str == "Armor_Shoes_of_Gold")		return ItemID::Armor_Shoes_of_Gold;
		if (str == "Armor_Shoes_of_Diamond")	return ItemID::Armor_Shoes_of_Diamond;

		if (str == "Armor_Body_of_Wood")		return ItemID::Armor_Body_of_Wood;
		if (str == "Armor_Body_of_Stone")		return ItemID::Armor_Body_of_Stone;
		if (str == "Armor_Body_of_Iron")		return ItemID::Armor_Body_of_Iron;
		if (str == "Armor_Body_of_Gold")		return ItemID::Armor_Body_of_Gold;
		if (str == "Armor_Body_of_Diamond")		return ItemID::Armor_Body_of_Diamond;

		if (str == "Block_Air")					return ItemID::Block_Air;
		if (str == "Block_Stone")				return ItemID::Block_Stone;
		if (str == "Block_Iron")				return ItemID::Block_Iron;
		if (str == "Block_Gold")				return ItemID::Block_Gold;
		if (str == "Block_Diamond")				return ItemID::Block_Diamond;
		if (str == "Block_Bedrock")				return ItemID::Block_Bedrock;
		if (str == "Block_Glass")				return ItemID::Block_Glass;
		if (str == "Block_Dirt")				return ItemID::Block_Dirt;
		if (str == "Block_Grass")				return ItemID::Block_Grass;
		if (str == "Block_Wood")				return ItemID::Block_Wood;
		if (str == "Block_Leaf")				return ItemID::Block_Leaf;

		if (str == "鉄インゴット")				return ItemID::鉄インゴット;
		if (str == "金インゴット")				return ItemID::金インゴット;
		if (str == "ダイヤモンド")				return ItemID::ダイヤモンド;
		if (str == "ビーコン")					return ItemID::ビーコン;

		return ItemID::MAX;
	}
};