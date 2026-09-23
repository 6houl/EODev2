#include "..\stdafx.h"
#include "Handler.h"
#include "..\Send\SCharacter.h"
#include "..\Send\SConnection.h"
CLIENT_F_FUNC(Character)
{	
	switch(action)
		{		
			case PACKET_PLAYER: 
				{
					int ID = reader.GetShort();//Delete ID - Used for confirmations.
					Menu::SrvrCharID = reader.GetInt();//CharacterID
					Menu::SrvrDeleteID = ID;
					break;
				}
			case PACKET_REPLY: 
				{
						int ID = reader.GetShort();
						switch(ID)
						{
						case (CHARACTER_EXISTS):
							{
								World::ThrowMessage("Already exists", "The character name you provided \nalready exists in our database, use \nanother.");
								return false;
							}
						case (CHARACTER_FULL):
							{
								World::ThrowMessage("Request Denied", "You can only have 3 characters. Please \ndelete a character and try again.");
								return false;
							}
						case (CHARACTER_NOT_APPROVED):
							{
								World::ThrowMessage("Not approved", "The character name you provided is not \napproved,try another name.");
								return false;
							}
						case (CHARACTER_OK):
							{	
								World::ThrowMessage("Character created", "Your new character has been created and \nis ready to explore a new world.");
								game->SubStage = 0;
								break;
							}
						case (CHARACTER_DELETED):
							{
								break;
							}
						default:
							{
									Menu::SrvrCreateID = ID;
									std::string str = reader.GetEndString();
									if(str != "OK")
									{
										World::ThrowMessage("Request denied","You can only have 3 characters. Please \ndelete a character and try again");
										return false;
									}
									game->SubStage = 1;
									return true;
							}
						}

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
							game->menu->CSModels[i].AlignCharacter(game->menu->CSModels[i].Stance, game->menu->CSModels[i].frame_ID, game->menu->CSModels[i].direction);
						}
						game->AccountCharacterSize = std::min<int>(CharacterSize, 3);
						
					break;
				}
			default:
				{
					return false;
				}
		}
	return true;
}
