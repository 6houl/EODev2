#include "..\stdafx.h"
#include "Handler.h"


CLIENT_F_FUNC(Item)
{	
	auto player = game->map->m_Players.find(World::WorldCharacterID);
	if (player == game->map->m_Players.end() || player->second == nullptr)
		return false;
	Map_Player* mainPlayer = player->second;
		switch(action)
		{		
			case PACKET_GET: 
				{
					short UID = reader.GetShort();
					game->map->RemoveItem(UID);
					Map_UI_Inventory::InventoryItem newitem;
					newitem.id= reader.GetShort();
					newitem.amount = reader.GetThree();
					int weight = reader.GetChar();
					int maxweight = reader.GetChar();
					mainPlayer->weight = weight;
					mainPlayer->maxweight = maxweight;
					game->Map_UserInterface->map_inventory->AddItem(newitem);
					EIF_Data m_item = World::EIF_File->Get(newitem.id );
					game->Map_UserInterface->DrawHelpMessage("Notification", "You picked up " + std::to_string(newitem.amount) + "x " + m_item.name);
					
					break;
				}
			case PACKET_DROP:
			{
				Map::Map_Item m_item;
				m_item.ItemID = reader.GetShort();
				m_item.amount = reader.GetThree();
				int remainingAmount = reader.GetInt();
				int ItemIndex = reader.GetShort();
				m_item.x = reader.GetChar();
				m_item.y = reader.GetChar();
				int weight = reader.GetChar();
				int maxweight = reader.GetChar();

				{
					Map_UI_Inventory::InventoryItem invitem;
					invitem.amount = m_item.amount;
					invitem.id = m_item.ItemID;
					game->Map_UserInterface->map_inventory->SetItem(invitem.id, remainingAmount);
					mainPlayer->weight = weight;
					mainPlayer->maxweight = maxweight;

					EIF_Data m_item = World::EIF_File->Get(invitem.id);
					game->Map_UserInterface->DrawHelpMessage("Notification", "You dropped " + std::to_string(invitem.amount) + "x " + m_item.name);
				}

				game->map->AddItem(ItemIndex, m_item);

				break;
			}
			case PACKET_JUNK:
			{
				Map::Map_Item m_item;
				m_item.ItemID = reader.GetShort();
				m_item.amount = reader.GetThree();
				int remainingAmount = reader.GetInt();
				int weight = reader.GetChar();
				int maxweight = reader.GetChar();

				{
					Map_UI_Inventory::InventoryItem invitem;
					invitem.amount = m_item.amount;
					invitem.id = m_item.ItemID;
					game->Map_UserInterface->map_inventory->SetItem(invitem.id, remainingAmount);
					mainPlayer->weight = weight;
					mainPlayer->maxweight = maxweight;
				}

				break;
			}
			default:
				return false;
		}
		return true;
}
