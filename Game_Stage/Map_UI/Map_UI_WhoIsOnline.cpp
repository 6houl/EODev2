#include "..\..\stdafx.h"
#include "Map_UI.h"
#include "..\game.h"
#include "..\..\Packet_Handler\Send\SPlayers.h"

Map_UI_WhoIsOnline::Map_UI_WhoIsOnline(Map_UI* m_UIElement, Game* m_Game)
{
	this->m_MapUI = m_UIElement;
	this->m_game = m_Game;

	this->UI_Element_WhoIsOnlineButton = new Button(this->m_game, 590, 350, 0, 114, 36, 19, false, this->m_game->ResourceManager->GetResource(2, 25, false));

	this->UI_WhoisOnlineScrollbar = new UI_Scrollbar(568, 351, 600, 117, -445, 0, 82, TextTools::ChatGroups[TextTools::ChatIndex::WhoIsOnline], this->m_game->ResourceManager->GetResource(2, 29, false), this->m_game, this->m_game->MessageFont, this->m_game->ResourceManager->GetResource(2, 32, true));
	LastListRefresh = 0;
}

void Map_UI_WhoIsOnline::Update()
{
	this->UI_Element_WhoIsOnlineButton->Update(this->m_MapUI->MouseX, this->m_MapUI->MouseY, this->m_MapUI->MousePressed);
	if (this->UI_Element_WhoIsOnlineButton->MouseClickedOnElement())
	{
		this->m_MapUI->SetStage(Map_UI::UI_ElementStage::UI_Element_WhoIsOnline);
		SPlayers::SendPlayerListRequest(this->m_game->world->connection->ClientStream, this->m_game);
	}
	switch (this->m_MapUI->UI_Stage)
	{
	case(Map_UI::UI_ElementStage::UI_Element_WhoIsOnline):
	{
		this->UI_WhoisOnlineScrollbar->Update(this->m_MapUI->MouseX, this->m_MapUI->MouseY, this->m_game->MouseWheelVal, this->m_MapUI->MousePressed, this->m_MapUI->MouseHeld, this->m_game->FPS);
		const DWORD now = GetTickCount();
		if (now - LastListRefresh >= 250)
		{
			LastListRefresh = now;
			const std::vector<World::OnlinePlayerContainer> players = World::GetOnlinePlayers();
			TextTools::ChatGroups[TextTools::ChatIndex::WhoIsOnline]->clear();
			for (std::size_t i = 0; i < players.size(); i++)
			{
				TextTools::ChatContainer newcontainer;
				newcontainer.Chat_Icon = 9 + (players[i]._Icon / 5);
				newcontainer.CharacterName = players[i]._Name;
				std::string details = players[i]._Title;
				if (!players[i]._GuildTag.empty())
					details += " [" + players[i]._GuildTag + "]";
				const std::string className = World::ECF_File->Get(static_cast<unsigned char>(players[i]._ClassID)).name;
				if (!className.empty())
					details += " " + className;
				newcontainer.Message.push_back(details);
				TextTools::ChatGroups[TextTools::ChatIndex::WhoIsOnline]->push_back(newcontainer);
			}
		}
		break;
	}
	default:
		return;
	}
}

void Map_UI_WhoIsOnline::Render(float depth)
{
	this->UI_Element_WhoIsOnlineButton->Draw();
	switch (this->m_MapUI->UI_Stage)
	{
	case(Map_UI::UI_ElementStage::UI_Element_WhoIsOnline):
	{
		this->m_game->Draw(this->m_game->ResourceManager->GetResource(2, 36, false), 101, 331, sf::Color(255, 255, 255, 255), 0, 0, -1, -1, sf::Vector2f(1, 1), depth);
		this->UI_WhoisOnlineScrollbar->Draw(depth);
		break;
	}
	default:
		return;
	}
}

Map_UI_WhoIsOnline::~Map_UI_WhoIsOnline()
{

}
