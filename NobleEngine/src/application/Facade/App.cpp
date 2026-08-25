#include "App.h"
#include <Application.h>
#include <ResourceLoader/Data/DataManager.h>

namespace App
{
	namespace Data
	{
		namespace Item
		{
			void Load()
			{
				Application::Instance().GetDataManager()->GetItemDataManager()->getLoader()->Load();
			}

			const ItemInfo* Get(ItemID id)
			{
				return Application::Instance().GetDataManager()->GetItemDataManager()->getBank()->GetItemInfo(id);
			}
			const BlockInfo* Get(BlockID id)
			{
				return Application::Instance().GetDataManager()->GetItemDataManager()->getBank()->GetBlockInfo(id);
			}
			const ToolInfo* Get(ToolID id)
			{
				return Application::Instance().GetDataManager()->GetItemDataManager()->getBank()->GetToolInfo(id);
			}
			const ObjectInfo* Get(ObjectID id)
			{
				return Application::Instance().GetDataManager()->GetItemDataManager()->getBank()->GetObjectInfo(id);
			}

			const int32_t GetBlockInfoTableHeapSlot()
			{
				return Application::Instance().GetDataManager()->GetItemDataManager()->getBank()->GetBlockInfoTableHeapSlot();
			}

			void CreateBlockInfoTable()
			{
				Application::Instance().GetDataManager()->GetItemDataManager()->getBank()->CreateBlockInfoTable();
			}


			void Save(ItemID id, const ItemInfo& info)
			{
				Application::Instance().GetDataManager()->GetItemDataManager()->getBank()->SetItemInfo(id, info);
			}
			void Save(BlockID id, const BlockInfo& info)
			{
				Application::Instance().GetDataManager()->GetItemDataManager()->getBank()->SetBlockInfo(id, info);
			}
			void Save(ToolID id, const ToolInfo& info)
			{
				Application::Instance().GetDataManager()->GetItemDataManager()->getBank()->SetToolInfo(id, info);
			}
		}
	}

}
