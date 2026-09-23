//#include "game.h"

#pragma once
#include "Utilities\UI_InformationBox.h"
#include <list>
#include "include\Ptypes\ptypes.h"
#include "include\Ptypes\pport.h"
#include "include\EOServ\packet.hpp"
#include "Utilities\Button.h"
#include "Utilities\Textbox.h"
#include "Utilities\Resource_Manager.h"
#include "Connection.h"
#include "Utilities\ConfigFile.h"
#include "include\EOServ\eodata.hpp"
#include "Utilities\UI_InformationBox.h"
#include <mutex>

class World
{
protected:
	Game* m_game;
public:
	World(Game* _Game);
	typedef std::list<Textbox> TextType;
	UI_InformationBox* W_UI_Infobox;
	static int WorldCharacterID;
	TextType TextBoxLst;
	Connection* connection;
	static std::string MBMessage;
	static std::string MBTitle;
	static PacketProcessor PProcessor;
	static bool MBHidden;
	static bool UIBox_Hidden;
	static unsigned int World::PacketCount;
	static unsigned char World::RawPacketCount;
	static ENF* ENF_File;
	static EIF* EIF_File;
	static ESF* ESF_File;
	static ECF* ECF_File;
	static int ChestX;
	static int ChestY;
	struct OnlinePlayerContainer
	{
		std::string _Name;
		std::string _Title;
		char _Icon;
		char _ClassID;
		std::string _GuildTag;
	};
	static std::vector<OnlinePlayerContainer> OnlinePlayers;
	static void SetOnlinePlayers(const std::vector<OnlinePlayerContainer>& players);
	static std::vector<OnlinePlayerContainer> GetOnlinePlayers();
	void HandleTextInput(WPARAM Parameter, int  phase, int  subphase);
	void HandleKeyInput(WPARAM Parameter, int  phase,int subphase);
    Textbox* RegisterTextBox(Textbox TxtBox);
    void UnregisterTextBox(Textbox* TxtBox);
	void MassTextBoxReset();
	void RenderTextBoxes(int  phase, int subphase);
	void SetFocusedTextbox(Textbox* TxtBox);

	
	static void DebugPrint(pt::string Message);
	static unsigned int ProtocolTickCount();
	static void ThrowMessage(std::string Title, std::string Message);
	Textbox* GetFocusedTextbox();

	void CreateConnection();
	void DropConnection();
	static bool Connected;
	static bool Connecting;
	static bool Send(Game* t_game, pt::ipstream* Stream, PacketBuilder Builder);
	static std::string Receive(Game* t_game, std::string Builder);

private:
	static std::mutex OnlinePlayersMutex;
};
