#include "BlockConfig.h"

BlockConfig::BlockConfig()
{
	BlockInfo info;
	info.ID = BlockID::Air;		// ID:空気
	info.durability = -1;			// 耐久値:壊せない
	info.isTransparent = true;		// 透過ブロック:true
	info.color = 0x00000000;		// 色:透明

	blockInfoMap_[BlockID::Air] = info;

	info.ID = BlockID::Stone;		// ID:石
	info.durability = 120.0f;		// 耐久値:60
	info.isTransparent = false;		// 透過ブロック:false
	info.color = 0x808080FF;		// 色:灰色
	blockInfoMap_[BlockID::Stone] = info;

	info.ID = BlockID::Iron;		// ID:鉄
	info.durability = 160.0f;		// 耐久値:800
	info.isTransparent = false;		// 透過ブロック:false
	info.color = 0xC0C0C0FF;		// 色:薄灰色
	blockInfoMap_[BlockID::Iron] = info;

	info.ID = BlockID::Diamond;	// ID:ダイヤ
	info.durability = 240.0f;		// 耐久値:1200
	info.isTransparent = false;		// 透過ブロック:false
	info.color = 0xFF00FFFF;		// 色:水色
	blockInfoMap_[BlockID::Diamond] = info;

	info.ID = BlockID::Bedrock;	// ID:岩盤
	info.durability = std::numeric_limits<float>::infinity(); // 耐久値:壊せない
	info.isTransparent = false;		// 透過ブロック:false
	info.color = 0x404040FF;		// 色:暗灰色
	blockInfoMap_[BlockID::Bedrock] = info;

	info.ID = BlockID::Dirt;		// ID:土
	info.durability = 30.0f;		// 耐久値:300
	info.isTransparent = false;		// 透過ブロック:false
	info.color = 0x8B4513FF;		// 色:茶色
	blockInfoMap_[BlockID::Dirt] = info;

	info.ID = BlockID::Grass;		// ID:草付き土
	info.durability = 30.0f;		// 耐久値:300
	info.isTransparent = false;		// 透過ブロック:false
	info.color = 0x228B22FF;		// 色:緑色
	blockInfoMap_[BlockID::Grass] = info;

	info.ID = BlockID::Glass;		// ID:ガラス
	info.durability = 10.0f;		// 耐久値:100
	info.isTransparent = true;		// 透過ブロック:true
	info.color = 0x00000000;		// 色:透明
	blockInfoMap_[BlockID::Glass] = info;

	info.ID = BlockID::wood;		// ID:木材
	info.durability = 40.0f;		// 耐久値:400
	info.isTransparent = false;		// 透過ブロック:false
	info.color = 0x8B4513FF;		// 色:茶色
	blockInfoMap_[BlockID::wood] = info;

	info.ID = BlockID::Leaf;		// ID:葉っぱ
	info.durability = 20.0f;		// 耐久値:200
	info.isTransparent = true;		// 透過ブロック:true
	info.color = 0x228B22FF;		// 色:緑色
	blockInfoMap_[BlockID::Leaf] = info;

	//info.ID = BlockID::craftTable;// ID:作業台
	//info.durability = 40.0f;		// 耐久値:400
	//info.isTransparent = false;		// 透過ブロック:false
	//info.color = 0x8B4513FF;		// 色:茶色
	//blockInfoMap_[BlockID::craftTable] = info;
}

BlockConfig::~BlockConfig()
{}

BlockInfo BlockConfig::GetBlockInfo(BlockID id) const
{
	return blockInfoMap_.at(id);
}
