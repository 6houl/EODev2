#include "..\stdafx.h"
#include "Handler.h"
#include "..\Send\SRefresh.h"

CLIENT_F_FUNC(NPC)
{	
		switch(action)
		{		
			case PACKET_REPLY: 
				{
					int FromID = reader.GetShort();
					int Direction = reader.GetChar();
					int Index = reader.GetShort();
					int Damage = reader.GetThree();
					double HpLeft = reader.GetShort(); //As a percent?
					game->map->ThreadLock.lock();
					auto npc = game->map->m_NPCs.find(Index);
					if (npc != game->map->m_NPCs.end() && npc->second)
						npc->second->DealDamage(HpLeft,Damage);
					game->map->ThreadLock.unlock();
					break;
				}
			case PACKET_PLAYER:
			{
				int Index = reader.GetChar();
				if (Index == 254) ///Attacking
				{
					int Index = reader.GetChar();
					int Hp = reader.GetChar();
					int NpcDir = reader.GetChar();
					int PlayerID = reader.GetShort();
					int Damage = reader.GetThree();
					int procDirection = NpcDir;
					game->map->ThreadLock.lock();
					//game->map->m_Players[PlayerID]->hp -= Damage;
					auto player = game->map->m_Players.find(PlayerID);
					if (player != game->map->m_Players.end() && player->second)
						player->second->DealDamage(Damage);
					if (NpcDir == 1)
					{
						procDirection = 3;
					}
					if (NpcDir == 2)
					{
						procDirection = 1;
					}
					if (NpcDir == 3)
					{
						procDirection = 2;
					}
					int npcid = (Index);

					map<int, Map_NPC*>::iterator it = game->map->m_NPCs.find(npcid);
					if (it != game->map->m_NPCs.end())
					{
						it->second->direction = procDirection;
						it->second->SetStance(Map_NPC::NPC_Stance::Attacking);
					}
					game->map->ThreadLock.unlock();
				}
				else ///Walking
				{
					//int ID = reader.GetChar();
 					int x = reader.GetChar();
					int y = reader.GetChar();
					int Dir = reader.GetChar();
					//if (game->map->FindNPCInList(ID) != -1)
					{
						game->map->WalkNPC(Index, Dir, x, y);
						//game->map->
						//Stage.Map.NPCs[ID].AnimIndex = 0;
						//Stage.Map.NPCs[ID].Walk(x, y, Dir);
					}
					//else
					{
						//SRefresh::RequestRefresh(game->world->connection->ClientStream, game);
					}
				}
				break;
								
			}
			case PACKET_SPEC:
			{
				if (reader.Remaining() < 5)
					return false;
				int KillerID = reader.GetShort();
				int KillerDirection = reader.GetChar();
				int NPCIndex = reader.GetShort();
				{
					std::lock_guard<std::mutex> lock(game->map->ThreadLock);
					auto killer = game->map->m_Players.find(KillerID);
					if (killer != game->map->m_Players.end() && killer->second != nullptr)
						killer->second->direction = KillerDirection;
				}

				if (reader.Remaining() == 0)
				{
					game->map->RemoveNPC(NPCIndex);
				}
				else if (reader.Remaining() >= 13)
				{	
					int NPCDropUID = reader.GetShort();
					int NPCDropItemID = reader.GetShort();
					int x =	reader.GetChar();
			        int y =	reader.GetChar();
					int ItemAmount = reader.GetInt();
					int damage = reader.GetThree();
					if (NPCDropUID > 0)
					{
						game->map->AddItem(NPCDropUID, NPCDropItemID, x, y, ItemAmount);
					}
					{
						std::lock_guard<std::mutex> lock(game->map->ThreadLock);
						auto npc = game->map->m_NPCs.find(NPCIndex);
						if (npc != game->map->m_NPCs.end() && npc->second)
							npc->second->DealDamage(0, damage);
					}
					game->map->KillNPC(NPCIndex);
				}
				else
				{
					return false;
				}
				break;
			}
			default:
				return false;
		}
		return true;
}
