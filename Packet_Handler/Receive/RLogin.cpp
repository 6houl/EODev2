#include "..\stdafx.h"
#include "Handler.h"

CLIENT_F_FUNC(Login)
{
		switch(action)
		{
			case PACKET_REPLY: 
				{
					game->world->connection->CompleteLogin();
					int ID = reader.GetShort();
         
					switch(ID)
					{
					case(1):
						{
						World::ThrowMessage("Login refused", "The account name you provided does not\nexist in our database.");
						break;
						}
					case(2):
						{
							World::ThrowMessage( "Login refused", "The account or passsword you provided\ncould not be found in our database.");
							break;
						}
					case(3):
						{
							unsigned char CharacterSize = reader.GetChar();
							reader.Getbyte();
							reader.Getbyte();
							for(int i = 0; i < 3;i++)
							{
								game->menu->CSModels[i].InitializeModel(game);
							}
	
							for (int i = 0; i < CharacterSize; i++)
							{
								std::string name = reader.GetBreakString();
								int gameId = reader.GetInt();
								int level = reader.GetChar();
								int gender = reader.GetChar();
								int hairStyle = reader.GetChar() - 1;
								int hairColor = reader.GetChar();
								int skinColor = reader.GetChar();
								int admin = reader.GetChar();
								int shoeId = reader.GetShort() - 1;
								int armorId = reader.GetShort() - 1;
								int hatId = reader.GetShort() - 1;
								int shieldId = reader.GetShort() - 1;
								int weaponId = reader.GetShort() - 1;
								reader.Getbyte();

								if (i >= 3)
									continue;

								if (!name.empty() && name[0] >= 'a' && name[0] <= 'z')
									name[0] += 'A' - 'a';
								game->menu->CSModels[i].name = name;
								game->menu->CSModels[i].ID = i;
								game->menu->CSModels[i].Game_ID = gameId;
								if (i == 0)
									game->PlayerID = gameId;
								game->menu->CSModels[i].level = level;
								game->menu->CSModels[i].Gender = gender;
								game->menu->CSModels[i].HairStyle = hairStyle;
								game->menu->CSModels[i].HairCol = hairColor;
								game->menu->CSModels[i].SkinCol = skinColor;
								game->menu->CSModels[i].admin = admin;
								game->menu->CSModels[i].ShoeID = shoeId;
								game->menu->CSModels[i].ArmorID = armorId;
								game->menu->CSModels[i].HatID = hatId;
								game->menu->CSModels[i].ShieldID = shieldId;
								game->menu->CSModels[i].WeaponID = weaponId;
								game->menu->CSModels[i].UpdateAppearence();
								
							}
							game->AccountCharacterSize = std::min<int>(CharacterSize, 3);
							game->Stage = game->PCharacterChoose;
							break;
						}
					case(4):
						{
							World::ThrowMessage("Login refused", "This account is banned from the server.");
							break;
						}
					case(5):
						{
							World::ThrowMessage( "Login refused", "This account is already logged on. Please\ntry again in a few minutes.");
							break;
						}
					case(6):
						{
							World::ThrowMessage( "Login refused", "The server is currently full.\nPlease try again in a few minutes.");
							break;
						}
					}
					break;
				}
			default:
					return false;
		}	
		return true;
}
