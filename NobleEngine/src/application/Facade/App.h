#pragma once
#include <definition/definition.h>


namespace App
{
	namespace Data
	{
		namespace Item
		{
			// 一括読み込み
			void Load();

			// ToolIDを指定して個別読み込み
			void Load(ToolID id);
			// ItemIDを指定して個別読み込み
			void Load(ItemID id);
			// ObjectIDを指定して個別読み込み
			void Load(ObjectID id);

			// ItemIDを指定して個別保存
			void Save(ItemID id, const ItemInfo& info);
			// BlockIDを指定して個別保存
			void Save(BlockID id, const BlockInfo& info);
			// ToolIDを指定して個別保存
			void Save(ToolID id, const ToolInfo& info);
			// ObjectIDを指定して個別保存
			void Save(ObjectID id, const ObjectInfo& info);

			// ItemIDを指定してデータ取得
			const ItemInfo* Get(ItemID id);
			// BlockIDを指定してデータ取得
			const BlockInfo* Get(BlockID id);
			// ToolIDを指定してデータ取得
			const ToolInfo* Get(ToolID id);
			// ObjectIDを指定してデータ取得
			const ObjectInfo* Get(ObjectID id);
			// BlockInfoTableのHeapSlotを取得
			const int32_t GetBlockInfoTableHeapSlot();
			// BlockInfoTableを作成
			void CreateBlockInfoTable();
		}
	}
}