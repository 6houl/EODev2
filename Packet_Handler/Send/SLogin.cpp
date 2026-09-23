#include "..\stdafx.h"
#include "SLogin.h"
#include "..\World.h"
#include "..\game.h"
#include "..\ClientPackets.h"
void SLogin::SendLoginRequest(Game* _Game, pt::ipstream* ClientStream, std::string AccountName, std::string Password)
{
			PacketBuilder builder = ClientPackets::LoginRequest(AccountName, Password);
			World::Send(_Game,ClientStream,builder);
}
