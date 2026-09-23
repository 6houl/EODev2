#include "..\stdafx.h"
#include "SConnection.h"
#include "..\game.h"
void SConnection::SendPlayer(pt::ipstream* ClientStream, int response, LPVOID game)
{
	Game* gme = (Game*)game;
	PacketBuilder builder = PacketBuilder(PACKET_CONNECTION, PACKET_ACCEPT);
	builder.AddShort(gme->SendMulti); // encryption multiplier
	builder.AddShort(gme->RecvMulti); // decryption multiplier
    builder.AddShort(gme->GameID); // player id
	World::Send(gme,ClientStream,builder);
}

void SConnection::Ping(pt::ipstream* ClientStream, LPVOID game)
{
	Game* gme = (Game*)game;
	PacketBuilder builder = PacketBuilder(PACKET_CONNECTION, PACKET_PING);
	builder.Addbyte(107);
	World::Send(gme,ClientStream,builder);
}
