#include "MapManager/Chunk/Block/BlockConfig.h"

BlockConfig::BlockConfig()
{
	Blockinfo info;
	info.type = BlockID::Air;		// ID:空気
	info.durability = -1;			// 耐久値:壊せない
	info.isTransparent = true;		// 透過ブロック:true
	info.isExtraAction = false;		// 右クリック特殊動作:false
	info.genre = ItemGenre::None;	// アイテムジャンル:なし

	blockInfoMap_[BlockID::Air] = info;

	info.type = BlockID::Stone;		// ID:石
	info.durability = 120.0f;		// 耐久値:60
	info.isTransparent = false;		// 透過ブロック:false
	info.isExtraAction = false;		// 右クリック特殊動作:false
	info.genre = ItemGenre::Stone;	// アイテムジャンル:鉱石系
	blockInfoMap_[BlockID::Stone] = info;

	info.type = BlockID::Iron;		// ID:鉄
	info.durability = 160.0f;		// 耐久値:800
	info.isTransparent = false;		// 透過ブロック:false
	info.isExtraAction = false;		// 右クリック特殊動作:false
	info.genre = ItemGenre::Stone;	// アイテムジャンル:鉱石系
	blockInfoMap_[BlockID::Iron] = info;

	info.type = BlockID::Diamond;	// ID:ダイヤ
	info.durability = 240.0f;		// 耐久値:1200
	info.isTransparent = false;		// 透過ブロック:false
	info.isExtraAction = false;		// 右クリック特殊動作:false
	info.genre = ItemGenre::Stone;	// アイテムジャンル:鉱石系
	blockInfoMap_[BlockID::Diamond] = info;

	info.type = BlockID::Bedrock;	// ID:岩盤
	info.durability = std::numeric_limits<float>::infinity(); // 耐久値:壊せない
	info.isTransparent = false;		// 透過ブロック:false
	info.isExtraAction = false;		// 右クリック特殊動作:false
	info.genre = ItemGenre::None;	// アイテムジャンル:なし
	blockInfoMap_[BlockID::Bedrock] = info;

	info.type = BlockID::Dirt;		// ID:土
	info.durability = 30.0f;		// 耐久値:300
	info.isTransparent = false;		// 透過ブロック:false
	info.isExtraAction = false;		// 右クリック特殊動作:false
	info.genre = ItemGenre::Dirt;	// アイテムジャンル:土系
	blockInfoMap_[BlockID::Dirt] = info;

	info.type = BlockID::Lawn;		// ID:草付き土
	info.durability = 30.0f;		// 耐久値:300
	info.isTransparent = false;		// 透過ブロック:false
	info.isExtraAction = false;		// 右クリック特殊動作:false
	info.genre = ItemGenre::Dirt;	// アイテムジャンル:土系
	blockInfoMap_[BlockID::Lawn] = info;

	info.type = BlockID::Glass;		// ID:ガラス
	info.durability = 10.0f;		// 耐久値:100
	info.isTransparent = true;		// 透過ブロック:true
	info.isExtraAction = false;		// 右クリック特殊動作:false
	info.genre = ItemGenre::Stone;	// アイテムジャンル:鉱石系
	blockInfoMap_[BlockID::Glass] = info;

	info.type = BlockID::Log;		// ID:木材
	info.durability = 40.0f;		// 耐久値:400
	info.isTransparent = false;		// 透過ブロック:false
	info.isExtraAction = false;		// 右クリック特殊動作:false
	info.genre = ItemGenre::Wood;	// アイテムジャンル:木材系
	blockInfoMap_[BlockID::Log] = info;

	info.type = BlockID::Planks;	// ID:木材
	info.durability = 30.0f;		// 耐久値:300
	info.isTransparent = false;		// 透過ブロック:false
	info.isExtraAction = false;		// 右クリック特殊動作:false
	info.genre = ItemGenre::Wood;	// アイテムジャンル:木材系
	blockInfoMap_[BlockID::Planks] = info;

	info.type = BlockID::Leaf;		// ID:葉っぱ
	info.durability = 20.0f;		// 耐久値:200
	info.isTransparent = true;		// 透過ブロック:true
	info.isExtraAction = false;		// 右クリック特殊動作:false
	info.genre = ItemGenre::Wood;	// アイテムジャンル:木材系
	blockInfoMap_[BlockID::Leaf] = info;

	info.type = BlockID::craftTable;// ID:作業台
	info.durability = 40.0f;		// 耐久値:400
	info.isTransparent = false;		// 透過ブロック:false
	info.isExtraAction = true;		// 右クリック特殊動作:false
	info.genre = ItemGenre::Wood;	// アイテムジャンル:木材系
	blockInfoMap_[BlockID::craftTable] = info;
}

BlockConfig::~BlockConfig()
{}

Blockinfo BlockConfig::GetBlockInfo(BlockID id) const
{
	return blockInfoMap_.at(id);
}
