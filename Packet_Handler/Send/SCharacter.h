#pragma once 
class Game;

class SCharacter
{
	public:
		static void DeletePlayer(pt::ipstream* ClientStream, Game* game);
		static void RequestDeletePlayer(pt::ipstream* ClientStream, int deleteId, Game* game);
		static void RequestCreatePlayer(pt::ipstream* ClientStream, LPVOID game);
		static void CreatePlayer(pt::ipstream* ClientStream, LPVOID game);
};
