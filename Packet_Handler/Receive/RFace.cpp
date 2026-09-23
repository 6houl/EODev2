#include "..\stdafx.h"
#include "Handler.h"


CLIENT_F_FUNC(Face)
{	
		switch(action)
		{		
			case PACKET_PLAYER: 
				{
					short PlayerID = reader.GetShort();
					int Direction = reader.GetChar();
					int findid = (PlayerID);
					auto player = game->map->m_Players.find(findid);
					if (findid >= 0 && player != game->map->m_Players.end() && player->second)
					{
						int procDirection = Direction;
						if (Direction == 1)
						{
							procDirection = 3;
						}
						if (Direction == 2)
						{
							procDirection = 1;
						}
						if (Direction == 3)
						{
							procDirection = 2;
						}
						game->map->ThreadLock.lock();
						player->second->direction = procDirection;
						game->map->ThreadLock.unlock();
					}
					
					break;
				}
			default:
				return false;
		}
		return true;
}
