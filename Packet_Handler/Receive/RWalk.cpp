#include "..\stdafx.h"
#include "Handler.h"


CLIENT_F_FUNC(Walk)
{	
		switch(action)
		{		
			case PACKET_PLAYER: 
				{
					
					short PlayerID = reader.GetShort();
					int Direction = reader.GetChar();
					int FromX = reader.GetChar();
					int FromY = reader.GetChar();
					game->map->WalkPlayer(PlayerID, Direction, FromX, FromY);
					break;
				}
			case PACKET_REPLY:
				{
					if (reader.Remaining() < 2)
						return false;
					reader.Getbyte();
					reader.Getbyte();
					while (reader.Remaining() >= 9)
					{
						Map::Map_Item item;
						const int itemIndex = reader.GetShort();
						item.ItemID = reader.GetShort();
						item.x = reader.GetChar();
						item.y = reader.GetChar();
						item.amount = reader.GetThree();
						game->map->AddItem(itemIndex, item);
					}
					break;
				}
			default:
				return false;
		}
		return true;
}
