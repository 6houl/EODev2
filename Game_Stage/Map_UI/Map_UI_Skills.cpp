#include "..\..\stdafx.h"
#include "Map_UI.h"
#include "..\..\game.h"


Map_UI_Skills::Map_UI_Skills(Map_UI* m_UIElement, Game* m_Game)
{
	this->m_MapUI = (Map_UI*)m_UIElement;
	this->m_game = m_Game;

	this->UI_Element_SkillsButton = new Button(this->m_game, 62, 370, 0, 38, 36, 19, false, this->m_game->ResourceManager->GetResource(2, 25, false));
}

void Map_UI_Skills::Update()
{
	this->UI_Element_SkillsButton->Update(this->m_MapUI->MouseX, this->m_MapUI->MouseY, this->m_MapUI->MousePressed);
	if (this->UI_Element_SkillsButton->MouseClickedOnElement())
	{
		this->m_MapUI->SetStage(Map_UI::UI_ElementStage::UI_Element_Skills, 0);
	}
	switch (this->m_MapUI->UI_Stage)
	{
	case(Map_UI::UI_ElementStage::UI_Element_Skills):
	{
		auto player = this->m_game->map->m_Players.find(World::WorldCharacterID);
		if (player == this->m_game->map->m_Players.end() || player->second == nullptr)
			break;
		const int totalRows = static_cast<int>((player->second->spells.size() + 7) / 8);
		const int maximumRow = (std::max)(0, totalRows - 2);
		if (this->m_game->MouseWheelVal < 0)
			this->FirstVisibleRow--;
		else if (this->m_game->MouseWheelVal > 0)
			this->FirstVisibleRow++;
		this->FirstVisibleRow = (std::max)(0, (std::min)(maximumRow, this->FirstVisibleRow));

		const int firstSpell = this->FirstVisibleRow * 8;
		int spellIndex = 0;
		for (const Map_Player::Character_Spell& spell : player->second->spells)
		{
			if (spellIndex < firstSpell)
			{
				++spellIndex;
				continue;
			}
			const int slot = spellIndex - firstSpell;
			if (slot >= 16)
				break;
			const int x = 200 + (slot % 8) * 42;
			const int y = 337 + (slot / 8) * 36;
			if (this->m_MapUI->MouseX >= x && this->m_MapUI->MouseX < x + 42 &&
				this->m_MapUI->MouseY >= y && this->m_MapUI->MouseY < y + 36)
			{
				const ESF_Data spellData = World::ESF_File->Get(spell.id);
				this->m_MapUI->DrawHelpMessage("Spell", spellData.name + " (Level " + std::to_string(spell.level) + ")");
			}
			++spellIndex;
		}
		break;
	}
	default:
		return;
	}
}

void Map_UI_Skills::Render(float depth)
{
	this->UI_Element_SkillsButton->Draw();
	switch (this->m_MapUI->UI_Stage)
	{
	case(Map_UI::UI_ElementStage::UI_Element_Skills):
	{
		this->m_game->Draw(this->m_game->ResourceManager->GetResource(2, 62, false), 101, 331, sf::Color(255, 255, 255, 255), 0, 0, -1, -1, sf::Vector2f(1, 1), depth);
		auto player = this->m_game->map->m_Players.find(World::WorldCharacterID);
		if (player == this->m_game->map->m_Players.end() || player->second == nullptr)
			break;

		this->m_game->DrawText(std::to_string(player->second->skillpoints), 133, 427, sf::Color(222, 255, 255, 255), 9, false, depth - 0.001f);
		const int firstSpell = this->FirstVisibleRow * 8;
		int spellIndex = 0;
		for (const Map_Player::Character_Spell& spell : player->second->spells)
		{
			if (spellIndex < firstSpell)
			{
				++spellIndex;
				continue;
			}
			const int slot = spellIndex - firstSpell;
			if (slot >= 16)
				break;
			const ESF_Data spellData = World::ESF_File->Get(spell.id);
			Resource_Manager::TextureData* icon = this->m_game->ResourceManager->GetResource(25, spellData.icon, true);
			const int iconWidth = icon->_width / 2;
			const float x = static_cast<float>(200 + (slot % 8) * 42 + (42 - iconWidth) / 2);
			const float y = static_cast<float>(337 + (slot / 8) * 36);
			this->m_game->Draw(icon, x, y, sf::Color::White, 0, 0, iconWidth, icon->_height, sf::Vector2f(1, 1), depth - 0.001f);
			this->m_game->DrawText(std::to_string(spell.level), x + 2, y + 23, sf::Color(222, 255, 255, 255), 8, false, depth - 0.002f, 1);
			++spellIndex;
		}
		break;
	}
	default:
		return;
	}
}

Map_UI_Skills::~Map_UI_Skills()
{

}
