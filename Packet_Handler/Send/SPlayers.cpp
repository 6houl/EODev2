#include "..\stdafx.h"
#include "SPlayers.h"
#include "..\game.h"
#include "..\ClientPackets.h"

void SPlayers::SendPlayerListRequest(pt::ipstream* ClientStream,LPVOID game)
{
	Game* gme = (Game*)game;
	PacketBuilder builder = ClientPackets::PlayerListRequest();
	World::Send(gme,ClientStream,builder);
}
