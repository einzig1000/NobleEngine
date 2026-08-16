#pragma once
#include <definition/definition.h>

namespace App
{
	void Update();
	void Draw();
	void DrawImGui();

	namespace Data
	{
		namespace Item
		{
			void Load();

			const ItemInfo* Get(ItemID id);
			const BlockInfo* Get(BlockID id);
			const ToolInfo* Get(ToolID id);
			const ObjectInfo* Get(ObjectID id);
			const int32_t GetBlockInfoTableHeapSlot();
			void CreateBlockInfoTable();

			void Save(ItemID id, const ItemInfo& info);
			void Save(BlockID id, const BlockInfo& info);
			void Save(ToolID id, const ToolInfo& info);
		}
	}


}
