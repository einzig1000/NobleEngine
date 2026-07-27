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

			const ItemInfo& Get(ItemID id)
			{
				return Application::Instance().GetDataManager()->GetItemDataManager()->getBank()->GetItemInfo(id);
			}
			const BlockInfo& Get(BlockID id)
			{
				return Application::Instance().GetDataManager()->GetItemDataManager()->getBank()->GetBlockInfo(id);
			}
			const ToolInfo& Get(ToolID id)
			{
				return Application::Instance().GetDataManager()->GetItemDataManager()->getBank()->GetToolInfo(id);
			}

			const int32_t GetBlockInfoTableSRVIndex()
			{
				return Application::Instance().GetDataManager()->GetItemDataManager()->getBank()->GetBlockInfoTableSRVIndex();
			}


			void Save(ItemID id, const ItemInfo& info)
			{}
			void Save(BlockID id, const BlockInfo& info)
			{}
			void Save(ToolID id, const ToolInfo& info)
			{}
		}
	}

	void Update()
	{
		Application::Instance().Update();
	}
	void Draw()
	{
		Application::Instance().Draw();
	}
	void DrawImGui()
	{
		Application::Instance().DrawImGui();
	}
}
