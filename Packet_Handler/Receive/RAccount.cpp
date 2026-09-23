#include "..\stdafx.h"
#include "Handler.h"
#include "..\Send\SCharacter.h"
#include "..\Send\SAccount.h"

CLIENT_F_FUNC(Account)
{	
	switch(action)
	{	
	case(PACKET_REPLY): //reply to change pass request.
		{
			game->world->connection->CompleteAccountRequest();
			int ID = reader.GetShort(); 
			int val;
			if(reader.Length() > 6)
			{
				val = reader.GetChar();
				game->world->PacketCount = val;
			}
			std::string ret =  reader.GetEndString();
			if(ret == "OK")
			{
				if(ID == ACCOUNT_CHANGED)
				{
					game->SubStage = 0;
					World::ThrowMessage("Password changed", "Your password has changed, please use \nyour new password next time you login.");
				}
				else if(ID == ACCOUNT_CREATED)
				{
					World::ThrowMessage("Welcome", "Use your new account name and \npassword to login to the game.");
					game->SubStage=0;
					game->Stage = game->PMenu;
				}
				else
				{
					game->menu->SrvrCreateID = ID;
					game->world->connection->ScheduleAccountCreate((game->menu->TB_CAAccNme->text),(game->menu->TB_CAPassWrdOne->text),(game->menu->TB_CAName->text),(game->menu->TB_CACountry->text),(game->menu->TB_CAEmail->text));
				}
			}
			else if(ret == "NO")
			{
				if(ID == ACCOUNT_CHANGE_FAILED)
				{
				World::ThrowMessage("Request denied", "The account name or old password you \nprovided do not match with our database.");
				}
				if(ID == ACCOUNT_EXISTS)
				{
				World::ThrowMessage("Already exists", "The account name you provided already \nexists in our database, use another.");
				}
				if(ID == ACCOUNT_NOT_APPROVED)
				{
				World::ThrowMessage("Request denied", "Sorry");
				}

			}
			else if(ret == "GO")
			{
				if(ID == ACCOUNT_CONTINUE || ID == ACCOUNT_CREATED)
				{
					World::ThrowMessage("Welcome", "Use your new account name and \npassword to login to the game.");
					game->SubStage=0;
					game->Stage = game->PMenu;

				}
			}
			break;
		}
		default :
			return false;
	}
	return true;
}
