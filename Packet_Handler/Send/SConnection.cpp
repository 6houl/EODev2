#include "..\stdafx.h"
#include "SConnection.h"
#include "..\game.h"
#include "..\ClientPackets.h"

bool SConnection::SendPlayer(pt::ipstream* ClientStream, LPVOID game)
{
	Game* gme = (Game*)game;
	PacketBuilder builder = ClientPackets::ConnectionAccept(gme->SendMulti, gme->RecvMulti, gme->GameID);
	return World::Send(gme, ClientStream, builder);
}

void SConnection::Ping(pt::ipstream* ClientStream, LPVOID game)
{
	Game* gme = (Game*)game;
	PacketBuilder builder = PacketBuilder(PACKET_CONNECTION, PACKET_PING);
	builder.Addbyte(107);
	World::Send(gme,ClientStream,builder);
}
