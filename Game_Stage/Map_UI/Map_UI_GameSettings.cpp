#include "..\..\stdafx.h"
#include "Map_UI.h"
#include "..\game.h"
#include "..\..\Packet_Handler\ClientPackets.h"


Map_UI_GameSettings::Map_UI_GameSettings(Map_UI* m_UIElement, Game* p_Game)
{
	this->m_MapUI = m_UIElement;
	this->m_game = p_Game;
	this->UI_Element_GameSettingsButton = new Button(this->m_game, 590, 410, 0, 171, 36, 19, false, this->m_game->ResourceManager->GetResource(2, 25, false));
	this->SettingsButtons[HearWhispers] = new Button(this->m_game, 316, 426, 0, 0, 19, 15, false, this->m_game->ResourceManager->GetResource(2, 27, true));
	this->SettingsButtons[ShowBalloons] = new Button(this->m_game, 555, 354, 0, 0, 19, 15, false, this->m_game->ResourceManager->GetResource(2, 27, true));
	this->SettingsButtons[ShowShadows] = new Button(this->m_game, 555, 372, 0, 0, 19, 15, false, this->m_game->ResourceManager->GetResource(2, 27, true));
}

void Map_UI_GameSettings::ToggleSetting(Setting setting)
{
	switch (setting)
	{
	case HearWhispers:
		IniConfiguration::HearWhispers = !IniConfiguration::HearWhispers;
		World::Send(this->m_game, this->m_game->world->connection->ClientStream,
			ClientPackets::HearWhispers(IniConfiguration::HearWhispers));
		break;
	case ShowBalloons:
		IniConfiguration::ChatBalloons = !IniConfiguration::ChatBalloons;
		break;
	case ShowShadows:
		IniConfiguration::Shadows = !IniConfiguration::Shadows;
		break;
	}

	this->m_game->Config->SaveSettings();
}

void Map_UI_GameSettings::Update()
{
	this->UI_Element_GameSettingsButton->Update(this->m_MapUI->MouseX, this->m_MapUI->MouseY, this->m_MapUI->MousePressed);
	if (this->UI_Element_GameSettingsButton->MouseClickedOnElement())
	{
		this->m_MapUI->SetStage(Map_UI::UI_ElementStage::UI_Element_GameSettings, 0);
	}
	switch (this->m_MapUI->UI_Stage)
	{
	case(Map_UI::UI_ElementStage::UI_Element_GameSettings):
	{
		for (int i = 0; i < SettingCount; ++i)
		{
			this->SettingsButtons[i]->Update(this->m_MapUI->MouseX, this->m_MapUI->MouseY, this->m_MapUI->MousePressed);
			if (this->SettingsButtons[i]->MouseClickedOnElement())
			{
				this->ToggleSetting(static_cast<Setting>(i));
				this->SettingsButtons[i]->MouseClickProccessed();
			}
		}
		break;
	}
	default:
		return;
	}
}

void Map_UI_GameSettings::Render(float depth)
{
	this->UI_Element_GameSettingsButton->Draw();
	switch (this->m_MapUI->UI_Stage)
	{
	case(Map_UI::UI_ElementStage::UI_Element_GameSettings):
	{
		this->m_game->Draw(this->m_game->ResourceManager->GetResource(2, 47, false), 101, 331, sf::Color(255, 255, 255, 255), 0, 0, -1, -1, sf::Vector2f(1, 1), depth);
		for (int i = 0; i < SettingCount; ++i)
			this->SettingsButtons[i]->Draw(depth - 0.001f);

		const sf::Color textColor(240, 240, 199, 255);
		this->m_game->DrawText(IniConfiguration::HearWhispers ? "Enabled" : "Disabled", 219, 427, textColor, 9, false, depth - 0.002f);
		this->m_game->DrawText(IniConfiguration::ChatBalloons ? "Enabled" : "Disabled", 458, 355, textColor, 9, false, depth - 0.002f);
		this->m_game->DrawText(IniConfiguration::Shadows ? "Enabled" : "Disabled", 458, 373, textColor, 9, false, depth - 0.002f);
		break;
	}
	default:
		return;
	}
}

Map_UI_GameSettings::~Map_UI_GameSettings()
{
	delete this->UI_Element_GameSettingsButton;
	for (int i = 0; i < SettingCount; ++i)
		delete this->SettingsButtons[i];
}
