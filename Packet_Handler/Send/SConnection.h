#pragma once 
class SConnection
{
	public:
		static bool SendPlayer(pt::ipstream* ClientStream, LPVOID game);
		static void Ping(pt::ipstream* ClientStream, LPVOID game);
};
