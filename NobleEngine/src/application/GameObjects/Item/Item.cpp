#include "Item.h"

void Item::Initialize(ItemID id)
{
	id_ = id;
	// インベントリに表示する時のテクスチャ
	textureHandleForInventory_ = ResourceID::Get2DTextureID(id);
	// モデルとして描画する時のテクスチャ
	textureHandleForModel_ = ResourceID::Get3DTextureID(id);
	// モデルデータ
	modelHandle_ = ResourceID::GetModelID(id);
	// 積載可能数

	switch (id)
	{
	case ItemID::None:
		break;
	case ItemID::木の剣:			ableStackCount_ = 1; break;
	case ItemID::石の剣:			ableStackCount_ = 1; break;
	case ItemID::鉄の剣:			ableStackCount_ = 1; break;
	case ItemID::ダイヤの剣:		ableStackCount_ = 1; break;
	case ItemID::木のツルハシ:		ableStackCount_ = 1; break;
	case ItemID::石のツルハシ:		ableStackCount_ = 1; break;
	case ItemID::鉄のツルハシ:		ableStackCount_ = 1; break;
	case ItemID::ダイヤのツルハシ:	ableStackCount_ = 1; break;
	case ItemID::木の斧:			ableStackCount_ = 1; break;
	case ItemID::石の斧:			ableStackCount_ = 1; break;
	case ItemID::鉄の斧:			ableStackCount_ = 1; break;
	case ItemID::ダイヤの斧:		ableStackCount_ = 1; break;

	case ItemID::鉄の頭:			ableStackCount_ = 1; break;
	case ItemID::鉄の胴:			ableStackCount_ = 1; break;
	case ItemID::鉄の脚:			ableStackCount_ = 1; break;
	case ItemID::鉄の靴:			ableStackCount_ = 1; break;
	case ItemID::ダイヤの頭:		ableStackCount_ = 1; break;
	case ItemID::ダイヤの胴:		ableStackCount_ = 1; break;
	case ItemID::ダイヤの脚:		ableStackCount_ = 1; break;
	case ItemID::ダイヤの靴:		ableStackCount_ = 1; break;

	case ItemID::ガラスブロック:	ableStackCount_ = 64; break;
	case ItemID::葉ブロック:		ableStackCount_ = 64; break;
	case ItemID::原木ブロック:		ableStackCount_ = 64; break;
	case ItemID::芝ブロック:		ableStackCount_ = 64; break;
	case ItemID::土ブロック:		ableStackCount_ = 64; break;
	case ItemID::石ブロック:		ableStackCount_ = 64; break;
	case ItemID::鉄ブロック:		ableStackCount_ = 64; break;
	case ItemID::金ブロック:		ableStackCount_ = 64; break;
	case ItemID::ダイヤブロック:	ableStackCount_ = 64; break;
	case ItemID::岩盤ブロック:		ableStackCount_ = 64; break;
	case ItemID::鉄インゴット:		ableStackCount_ = 64; break;
	case ItemID::金インゴット:		ableStackCount_ = 64; break;
	case ItemID::ダイヤモンド:		ableStackCount_ = 64; break;
	case ItemID::ビーコン:			ableStackCount_ = 64; break;
	case ItemID::MAX:
		break;
	default:
		break;
	}
}