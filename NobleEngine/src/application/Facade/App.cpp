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
				Application::Instance().GetDataManager()->GetItemDataManager()->GetLoader()->Load();
			}
			void Load(ToolID id)
			{
				Application::Instance().GetDataManager()->GetItemDataManager()->GetLoader()->Load(id);
			}
			void Load(ItemID id)
			{
				Application::Instance().GetDataManager()->GetItemDataManager()->GetLoader()->Load(id);
			}
			void Load(ObjectID id)
			{
				Application::Instance().GetDataManager()->GetItemDataManager()->GetLoader()->Load(id);
			}

			const ItemInfo* Get(ItemID id)
			{
				return Application::Instance().GetDataManager()->GetItemDataManager()->GetBank()->GetItemInfo(id);
			}
			const BlockInfo* Get(BlockID id)
			{
				return Application::Instance().GetDataManager()->GetItemDataManager()->GetBank()->GetBlockInfo(id);
			}
			const ToolInfo* Get(ToolID id)
			{
				return Application::Instance().GetDataManager()->GetItemDataManager()->GetBank()->GetToolInfo(id);
			}
			const ObjectInfo* Get(ObjectID id)
			{
				return Application::Instance().GetDataManager()->GetItemDataManager()->GetBank()->GetObjectInfo(id);
			}

			const int32_t GetBlockInfoTableHeapSlot()
			{
				return Application::Instance().GetDataManager()->GetItemDataManager()->GetBank()->GetBlockInfoTableHeapSlot();
			}

			void CreateBlockInfoTable()
			{
				Application::Instance().GetDataManager()->GetItemDataManager()->GetBank()->CreateBlockInfoTable();
			}


			void Save(ItemID id, const ItemInfo& info)
			{
				Application::Instance().GetDataManager()->GetItemDataManager()->GetSaver()->Save(id, info);
			}
			void Save(BlockID id, const BlockInfo& info)
			{
				Application::Instance().GetDataManager()->GetItemDataManager()->GetSaver()->Save(id, info);
			}
			void Save(ToolID id, const ToolInfo& info)
			{
				Application::Instance().GetDataManager()->GetItemDataManager()->GetSaver()->Save(id, info);
			}
			void Save(ObjectID id, const ObjectInfo& info)
			{
				Application::Instance().GetDataManager()->GetItemDataManager()->GetSaver()->Save(id, info);
			}
		}
	}
}