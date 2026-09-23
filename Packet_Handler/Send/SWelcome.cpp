#include "..\stdafx.h"
#include "SWelcome.h"
#include "..\game.h"
#include "..\World.h"
#include "..\ClientPackets.h"

void SWelcome::LoginCharacter(pt::ipstream* ClientStream, int ID, LPVOID game)
{
	Game* gme = (Game*)game;
	PacketBuilder builder = ClientPackets::WelcomeRequest(ID);
	World::Send(gme, ClientStream, builder);
}
void SWelcome::LoginWelcome(pt::ipstream* ClientStream, int Char_ID, LPVOID game)
{
	Game* gme = (Game*)game;
	PacketBuilder builder = ClientPackets::WelcomeMessage(World::WorldCharacterID, Char_ID);
	World::Send(gme, ClientStream, builder);
}

void SWelcome::RequestFile(pt::ipstream* ClientStream, char FileType, LPVOID game, unsigned short fileId)
{
	Game* gme = (Game*)game;
	PacketBuilder builder = ClientPackets::FileRequest(FileType, World::WorldCharacterID, fileId);
	World::Send(gme, ClientStream, builder);
}
