#include "..\..\stdafx.h"
#include "Map_UI.h"
#include "..\game.h"
#include "..\..\Packet_Handler\Send\SPlayers.h"

namespace
{
	constexpr int PanelX = 101;
	constexpr int PanelY = 331;
	constexpr int VisibleRows = 7;

	int ChatIcon(unsigned char icon)
	{
		switch (icon)
		{
			case 4: return 12;
			case 5: return 14;
			case 6: return 10;
			case 9: return 13;
			case 10: return 15;
			case 20: return 11;
			default: return 9;
		}
	}

	std::string DisplayValue(const std::string& value, bool capitalize = false)
	{
		const std::size_t first = value.find_first_not_of(' ');
		if (first == std::string::npos)
			return "-";
		const std::size_t last = value.find_last_not_of(' ');
		std::string result = value.substr(first, last - first + 1);
		if (capitalize)
			result[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(result[0])));
		return result;
	}
}

Map_UI_WhoIsOnline::Map_UI_WhoIsOnline(Map_UI* m_UIElement, Game* m_Game)
{
	this->m_MapUI = m_UIElement;
	this->m_game = m_Game;

	this->UI_Element_WhoIsOnlineButton = new Button(this->m_game, 590, 350, 0, 114, 36, 19, false, this->m_game->ResourceManager->GetResource(2, 25, false));

	this->UI_WhoisOnlineScrollbar = new UI_Scrollbar(568, 351, 0, 0, 82, this->m_game->ResourceManager->GetResource(2, 29, false), this->m_game, this->m_game->ResourceManager->GetResource(2, 32, true));
	this->UI_WhoisOnlineScrollbar->SetNumberOfLines(VisibleRows);
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
		const std::vector<World::OnlinePlayerContainer> players = World::GetOnlinePlayers();
		this->UI_WhoisOnlineScrollbar->SetMaxIndex(static_cast<int>(players.size()));
		this->UI_WhoisOnlineScrollbar->Update(this->m_MapUI->MouseX, this->m_MapUI->MouseY, this->m_game->MouseWheelVal, this->m_MapUI->MousePressed, this->m_MapUI->MouseHeld, this->m_game->FPS);
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
		this->m_game->Draw(this->m_game->ResourceManager->GetResource(2, 36, false), PanelX, PanelY, sf::Color(255, 255, 255, 255), 0, 0, -1, -1, sf::Vector2f(1, 1), depth);
		const std::vector<World::OnlinePlayerContainer> players = World::GetOnlinePlayers();
		const int firstRow = (std::max)(0, this->UI_WhoisOnlineScrollbar->GetIndex());
		const int lastRow = (std::min)(static_cast<int>(players.size()), firstRow + VisibleRows);
		const sf::Color textColor = sf::Color::Black;
		for (int i = firstRow; i < lastRow; ++i)
		{
			const int y = PanelY + 23 + (i - firstRow) * 13;
			const int icon = ChatIcon(players[i]._Icon);
			this->m_game->Draw(this->m_game->ResourceManager->GetResource(2, 32, true), PanelX + 4, y, sf::Color::White, 0, icon * 13, 13, icon * 13 + 13, sf::Vector2f(1, 1), depth - 0.001f);
			this->m_game->DrawText(DisplayValue(players[i]._Name, true), PanelX + 18, y, textColor, 9, false, depth - 0.001f);
			this->m_game->DrawText(DisplayValue(players[i]._Title, true), PanelX + 133, y, textColor, 9, false, depth - 0.001f);
			this->m_game->DrawText(DisplayValue(players[i]._GuildTag), PanelX + 245, y, textColor, 9, false, depth - 0.001f);
			const std::string className = World::ECF_File->Get(players[i]._ClassID).name;
			this->m_game->DrawText(DisplayValue(className), PanelX + 359, y, textColor, 9, false, depth - 0.001f);
		}
		this->m_game->DrawText(std::to_string(players.size()), PanelX + 467, PanelY + 1, sf::Color(220, 220, 220), 9, true, depth - 0.001f);
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
