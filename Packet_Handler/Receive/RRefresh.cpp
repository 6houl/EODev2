#include "..\stdafx.h"
#include "Handler.h"


CLIENT_F_FUNC(Refresh)
{	
		switch(action)
		{
			case PACKET_REPLY:
			{
				int numberofplayers = reader.GetChar();
				reader.Getbyte();
				game->map->ThreadLock.lock();
				auto mainPlayerEntry = game->map->m_Players.find(World::WorldCharacterID);
				if (mainPlayerEntry == game->map->m_Players.end() || mainPlayerEntry->second == nullptr)
				{
					game->map->ThreadLock.unlock();
					return false;
				}
				game->map->ThreadLock.unlock();
				std::vector<int> preserveIDList;
				//				game->map->ClearMap();
				for (int i = 0; i < numberofplayers; i++)
				{
					Map_Player* newplayer = new Map_Player();
					newplayer->name = reader.GetBreakString();
					newplayer->ID = reader.GetShort();
					preserveIDList.push_back(newplayer->ID);
					newplayer->mapid = reader.GetShort();
					newplayer->x = reader.GetShort();
					newplayer->y = reader.GetShort();
					newplayer->direction = reader.GetChar();

					int procDirection = newplayer->direction;
					if (newplayer->direction == 1)
					{
						procDirection = 3;
					}
					if (newplayer->direction == 2)
					{
						procDirection = 1;
					}
					if (newplayer->direction == 3)
					{
						procDirection = 2;
					}
					newplayer->direction = procDirection;

					reader.GetChar(); // Unknown
					newplayer->guildtag = reader.GetFixedString(3);//PaddedGuildTag
					newplayer->level = reader.GetChar();
					newplayer->Gender = reader.GetChar();
					newplayer->HairStyle = reader.GetChar() - 1;
					newplayer->HairCol = reader.GetChar();
					newplayer->SkinCol = reader.GetChar();
					newplayer->maxhp = reader.GetShort();
					newplayer->hp = reader.GetShort();
					newplayer->maxtp = reader.GetShort();
					newplayer->tp = reader.GetShort();

					newplayer->ShoeID = reader.GetShort() - 1;
					reader.GetShort();
					reader.GetShort();
					reader.GetShort();
					newplayer->ArmorID = reader.GetShort() - 1;
					reader.GetShort();
					newplayer->HatID = reader.GetShort() - 1;
					newplayer->ShieldID = reader.GetShort() - 1;
					newplayer->WeaponID = reader.GetShort() - 1;

					unsigned char is_sitting = reader.GetChar();
					if (is_sitting == 1)
					{
						newplayer->SetStance(Map_Player::PlayerStance::ChairSitting);
					}
					else if (is_sitting == 2)
					{
						newplayer->SetStance(Map_Player::PlayerStance::GroundSitting);
					}
					else
					{
						newplayer->SetStance(Map_Player::PlayerStance::Standing);
					}
					unsigned char is_hideinvisible = reader.GetChar();
					newplayer->hidden = is_hideinvisible;
					reader.Getbyte();

					bool playerfound = false;
					{
						std::lock_guard<std::mutex> lock(game->map->ThreadLock);
						auto player = game->map->m_Players.find(newplayer->ID);
						if (player != game->map->m_Players.end() && player->second != nullptr)
						{
							playerfound = true;
							Map_Player* currentPlayer = player->second;
							const bool confirmsCurrentWalk = newplayer->Stance == Map_Player::PlayerStance::Standing &&
								currentPlayer->IsWalkingTo(newplayer->x, newplayer->y);
							currentPlayer->guildtag = newplayer->guildtag;
							currentPlayer->mapid = newplayer->mapid;
							if (!confirmsCurrentWalk)
							{
								if (currentPlayer->destination_x >= 0 && currentPlayer->destination_y >= 0)
									World::DebugPrint("Server corrected player walk position.");
								currentPlayer->SetStance(newplayer->Stance);
								currentPlayer->x = newplayer->x;
								currentPlayer->y = newplayer->y;
								currentPlayer->direction = newplayer->direction;
							}
							currentPlayer->level = newplayer->level;
							currentPlayer->Gender = newplayer->Gender;
							currentPlayer->HairStyle = newplayer->HairStyle;
							currentPlayer->HairCol = newplayer->HairCol;
							currentPlayer->SkinCol = newplayer->SkinCol;
							currentPlayer->maxhp = newplayer->maxhp;
							currentPlayer->hp = newplayer->hp;
							currentPlayer->maxtp = newplayer->maxtp;
							currentPlayer->tp = newplayer->tp;
							currentPlayer->ShoeID = newplayer->ShoeID;
							currentPlayer->ArmorID = newplayer->ArmorID;
							currentPlayer->HatID = newplayer->HatID;
							currentPlayer->ShieldID = newplayer->ShieldID;
							currentPlayer->WeaponID = newplayer->WeaponID;
							currentPlayer->hidden = newplayer->hidden;
							currentPlayer->UpdateAppearence();
						}
					}
					if (playerfound)
					{
						delete newplayer;
					}
					if (!playerfound)
					{
						newplayer->Initialize(game);
						game->map->AddPlayer(newplayer);
					}
				}

				for (auto p : game->map->m_Players)
				{
					bool keepplayer = false;
					for (int i : preserveIDList)
					{
						if (p.first == i)
						{
							keepplayer = true;
						}
					}
					if (!keepplayer)
					{
						game->map->RemovePlayer(p.first);
					}
				}

				while(true)
				{
					int index = reader.GetChar();
					if (index == 254) { break; }
					Map_NPC* new_npc = new Map_NPC();
					new_npc->Index = index;
					new_npc->ID = reader.GetShort();
					new_npc->x = reader.GetChar();
					new_npc->y = reader.GetChar();
					new_npc->direction = reader.GetChar();
					bool npcFound = false;
					{
						std::lock_guard<std::mutex> lock(game->map->ThreadLock);
						auto npc = game->map->m_NPCs.find(index);
						if (npc != game->map->m_NPCs.end() && npc->second != nullptr)
						{
							npcFound = true;
							if (!npc->second->IsWalkingTo(new_npc->x, new_npc->y))
							{
								if (npc->second->destination_x >= 0 && npc->second->destination_y >= 0)
									World::DebugPrint("Server corrected NPC walk position.");
								npc->second->SetStance(Map_NPC::NPC_Stance::Standing);
								npc->second->x = new_npc->x;
								npc->second->y = new_npc->y;
								npc->second->direction = new_npc->direction;
							}
							npc->second->ID = new_npc->ID;
						}
					}
					if (npcFound)
						delete new_npc;
					else
					{
						new_npc->Initialize((LPVOID*)game);
						game->map->AddNPC(new_npc);
					}
				}

				while (true)
				{
					int remaining = reader.Remaining();
					if (remaining <= 1)
					{
						break;
					}
					Map::Map_Item m_item;

					int ItemIndex = reader.GetShort();
					m_item.ItemID = reader.GetShort();
					m_item.x = reader.GetChar();
					m_item.y = reader.GetChar();
					m_item.amount = reader.GetThree();
					game->map->AddItem(ItemIndex, m_item);
				}
			
				break;
			}
			case PACKET_REMOVE: 
				{
					int PlayerID = reader.GetShort();
					int Animation = reader.GetChar();

					game->map->RemovePlayer(PlayerID);
					break;
				}
			default:
				return false;
		}
		return true;
}
