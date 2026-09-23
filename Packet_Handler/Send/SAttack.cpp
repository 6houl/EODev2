#include "..\stdafx.h"
#include "SAttack.h"
#include "..\game.h"


void SAttack::SendAttack(pt::ipstream* ClientStream, int Direction,LPVOID game)
{
	Game* gme = (Game*)game;
	PacketBuilder builder = PacketBuilder(PACKET_ATTACK, PACKET_USE);
	int procDirection = Direction;
	if (Direction == 1)
	{
		procDirection = 2;
	}
	if (Direction == 2)
	{
		procDirection = 3;
	}
	if (Direction == 3)
	{
		procDirection = 1;
	}
	
	builder.AddChar(procDirection);
	builder.AddThree(World::ProtocolTickCount());
	World::Send(gme,ClientStream,builder);
}
