#include "..\stdafx.h"
#include "Handler.h"
#include "..\Send\SConnection.h"
CLIENT_F_FUNC(Init)
{
	int id = reader.Getbyte();
	short PlayerID;

	switch (id)
    {
        case (1):
        {
			break;
		}
		case (2):
        {
			World::DebugPrint("Connection established!\n");
            int EIFVAL1 = reader.Getbyte();
            int EIFVAL2 = reader.Getbyte();

			int PCVal = EIFVAL1 * 7 - 13 + EIFVAL2;
			bool FindFlag = false;

			game->world->PacketCount = PCVal;
			game->world->RawPacketCount = 0;
			unsigned char serverEncryptionMultiple = reader.Getbyte();
			unsigned char clientEncryptionMultiple = reader.Getbyte();
			PlayerID = reader.GetShort();
			game->SendMulti = clientEncryptionMultiple;
			game->RecvMulti = serverEncryptionMultiple;
			game->world->PProcessor.SetEMulti(clientEncryptionMultiple, serverEncryptionMultiple);
			game->GameID = PlayerID;
			reader.GetThree();
			if (!SConnection::SendPlayer(clientstream, game))
				return false;
			World::Connecting = false;
			game->world->connection->ConnectionAccepted = true;
			World::Connected = true;
			if(game->menu->SelectID == 1)
			{
				game->Stage = Game::GameStage::PLogin;
			}
			else if(game->menu->SelectID == 2)
			{
				game->Stage = Game::GameStage::PCreateAccount;
			}
			game->menu->SelectID = 0;
			break;
		}
		case (3):
        {
            int bid = reader.Getbyte();
            int banlength = reader.Getbyte();
            if (bid == 0)
            {
			 std::string message = "";
			 message += "The server dropped the connection,\nreason: temporary ip ban. ";
			 message += std::to_string(banlength);
			 message +=  " minutes.";
             World::ThrowMessage("Connection is blocked", message);
            }
            else if (bid == 2)
            {
             World::ThrowMessage("Connection is blocked", "The server dropped the connection,\nreason: permanent ip ban.");
            }
            break;
         }
		 default:
         {
	     }
	}

	return true;
}
