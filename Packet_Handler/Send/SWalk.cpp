#include "..\stdafx.h"
#include "SWalk.h"
#include "..\game.h"

void SWalk::SendWalk(pt::ipstream* ClientStream, int Direction, int x, int y, LPVOID game)
{
	Game* gme = (Game*)game;
	PacketBuilder builder = PacketBuilder(PACKET_WALK, PACKET_PLAYER);
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
	builder.AddChar(x);
	builder.AddChar(y);
	World::Send(gme,ClientStream,builder);
}
