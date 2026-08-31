#pragma once
#include <EngineDefinition/EngineDefinition.h>
#include <EngineDefinition/EngineConstexprs.h>
#include <definition/constexprs.h>
#include <string>
#include <string_view>
#include <stdexcept>
#include <externals/MagicEnum/magic_enum.hpp>


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
	// 防具
	Armor,
	// ツール
	Tool,
	// ブロック
	Block,
	// オブジェクト(かまどとかチェスト)
	Object,

	MAX
};


// 全てのアイテムID
enum class ItemID
{
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
	Wood,	// 木
    Leaf,	// 葉

	MAX,
};

// 全てのツールID
enum class ToolID
{
	// ツール
	Sword_of_Wood,
	Sword_of_Stone,
	Sword_of_Iron,
	Sword_of_Gold,
	Sword_of_Diamond,
	Pickel_of_Wood,
	Pickel_of_Stone,
	Pickel_of_Iron,
	Pickel_of_Gold,
	Pickel_of_Diamond,
	Axe_of_Wood,
	Axe_of_Stone,
	Axe_of_Iron,
	Axe_of_Gold,
	Axe_of_Diamond,
	MAX,
};

enum class ObjectID
{
	Chest,
	kamado,

	MAX,
};

// 採掘モードの種類
enum class MiningPattern
{
	Swing,	// スイング採掘
	Range,	// 範囲採掘

	MAX,
};

// BlockID → ItemID 変換
ItemID BlockIDtoItemID(BlockID id);
// ToolID → ItemID 変換
ItemID ToolIDtoItemID(ToolID id);

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
	// ツールID
	ToolID ID = ToolID::MAX;
	// 耐久値
	float durability = 1.0f;
	// 攻撃力
	float attackPower = 1.0f;
	// 採掘速度
	float miningSpeed = 1.0f;
};

struct ObjectInfo
{
	// オブジェクトID
	ObjectID ID = ObjectID::MAX;
};

// アイテムごとの情報
struct ItemInfo
{
	// アイテムID
	ItemID id = ItemID::MAX;
	// アイテムジャンル
	ItemGenre genre = ItemGenre::MAX;

	// ブロックとして扱う時のデータ
	BlockID blockID = BlockID::MAX;
	// ツールとして扱う時のデータ
	ToolID toolID = ToolID::MAX;
	// オブジェクトとして扱う時のデータ
	ObjectID objectID = ObjectID::MAX;

	// テクスチャ
	int32_t textureID = -1;
	// モデル
	int32_t modelID = -1;

	// アイコン
	int32_t iconID = -1;
};


struct lookAtBlock
{
	BlockID* blockID = nullptr;
	Vector3int chunkIndex = { 0,0,0 };
	Vector3int localIndex = { 0,0,0 };
	AABBFace face = AABBFace::NONE;
	float distance = 0.0f;
};

class ICharacter;
struct RayHitResult
{
	enum class Type
	{
		None,
		Block,      // マップ上のブロック
		Character,  // キャラクター
		Object,     // マップ上のオブジェクト(かまど、チェストなど)
	};

	Type type = Type::None;

	// type == Block のとき有効
	lookAtBlock blockHit{};

	// type == Character のとき有効
	ICharacter* Character = nullptr;

	// 共通：レイ原点からの距離
	float distance = 0.0f;
};

struct Rect
{
	int32_t minX, minY;
	int32_t maxX, maxY;
	bool empty = true;
};
