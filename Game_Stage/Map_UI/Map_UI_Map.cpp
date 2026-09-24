#include "..\..\stdafx.h"
#include "Map_UI.h"
#include "..\game.h"

namespace
{
	constexpr int MiniMapTileWidth = 28;
	constexpr int MiniMapTileHeight = 14;
	constexpr int MiniMapCenterX = 320;
	constexpr int MiniMapCenterY = 144;
	constexpr float MiniMapDepth = 0.0085f;

	enum MiniMapGraphic
	{
		UpLine = 0,
		LeftLine = 1,
		Corner = 2,
		Solid = 3,
		OtherPlayer = 4,
		AttackableNPC = 5,
		MainPlayer = 6,
		Interactive = 7,
		OtherNPC = 8
	};

	bool IsInteractiveTile(EMF_Tile_Spec spec)
	{
		const int value = static_cast<int>(spec);
		return spec == EMF_Tile_Spec::BankVault ||
			spec == EMF_Tile_Spec::ChairAll ||
			spec == EMF_Tile_Spec::ChairDown ||
			spec == EMF_Tile_Spec::ChairLeft ||
			spec == EMF_Tile_Spec::ChairRight ||
			spec == EMF_Tile_Spec::ChairUp ||
			spec == EMF_Tile_Spec::ChairDownRight ||
			spec == EMF_Tile_Spec::ChairUpLeft ||
			spec == EMF_Tile_Spec::Chest ||
			value == 8 || (value >= 10 && value <= 15);
	}
}

Map_UI_Map::Map_UI_Map(Map_UI* m_UIElement, Game* m_Game)
{
	this->m_MapUI = m_UIElement;
	this->m_game = m_Game;

	this->UI_Element_MapButton = new Button(this->m_game, 62, 350, 0, 19, 36, 19, false, this->m_game->ResourceManager->GetResource(2, 25, false));
}

void Map_UI_Map::Update()
{
	this->UI_Element_MapButton->Update(this->m_MapUI->MouseX, this->m_MapUI->MouseY, this->m_MapUI->MousePressed);
	if (this->UI_Element_MapButton->MouseClickedOnElement())
	{
		this->m_MapUI->SetStage(Map_UI::UI_ElementStage::UI_Element_Map, 0);
	}
	switch (this->m_MapUI->UI_Stage)
	{
	case(Map_UI::UI_ElementStage::UI_Element_Map):
	{
		break;
	}
	default:
		return;
	}
}

void Map_UI_Map::Render(float depth)
{
	this->UI_Element_MapButton->Draw();
	switch (this->m_MapUI->UI_Stage)
	{
	case(Map_UI::UI_ElementStage::UI_Element_Map):
	{
		auto mainPlayerEntry = this->m_game->map->m_Players.find(World::WorldCharacterID);
		if (mainPlayerEntry == this->m_game->map->m_Players.end() || mainPlayerEntry->second == nullptr)
			break;

		Map_Player* mainPlayer = mainPlayerEntry->second;
		Resource_Manager::TextureData* miniMapTexture = this->m_game->ResourceManager->GetResource(2, 45, true);
		const int sourceWidth = miniMapTexture->_width / 9;
		const int sourceHeight = miniMapTexture->_height;
		const sf::Color mapColor(255, 255, 255, 160);

		auto drawGraphic = [&](int graphic, int mapX, int mapY)
		{
			const int drawX = (mapX - mainPlayer->x - mapY + mainPlayer->y) * (MiniMapTileWidth / 2) + MiniMapCenterX;
			const int drawY = (mapX - mainPlayer->x + mapY - mainPlayer->y) * (MiniMapTileHeight / 2) + MiniMapCenterY;
			if (drawX < 11 || drawY < 9 || drawX + MiniMapTileWidth > 629 || drawY + MiniMapTileHeight > 307)
				return;
			const int sourceX = graphic * sourceWidth;
			this->m_game->Draw(miniMapTexture, static_cast<float>(drawX), static_cast<float>(drawY), mapColor,
				sourceX, 0, sourceX + sourceWidth, sourceHeight, sf::Vector2f(1, 1), MiniMapDepth);
		};

		const int mapWidth = this->m_game->map->m_emf.header.width;
		const int mapHeight = this->m_game->map->m_emf.header.height;
		const int firstX = (std::max)(0, mainPlayer->x - 24);
		const int lastX = (std::min)(mapWidth, mainPlayer->x + 25);
		const int firstY = (std::max)(0, mainPlayer->y - 24);
		const int lastY = (std::min)(mapHeight, mainPlayer->y + 25);
		for (int y = firstY; y < lastY; ++y)
		{
			for (int x = firstX; x < lastX; ++x)
			{
				const Full_EMF::TileMeta& tile = this->m_game->map->m_emf.meta(x, y);
				if (tile.spec == EMF_Tile_Spec::MapEdge)
					continue;

				int edgeGraphic = Corner;
				const bool leftEdge = x == 0 || this->m_game->map->m_emf.meta(x - 1, y).spec == EMF_Tile_Spec::MapEdge;
				const bool upperEdge = y == 0 || this->m_game->map->m_emf.meta(x, y - 1).spec == EMF_Tile_Spec::MapEdge;
				if (leftEdge && upperEdge)
					continue;
				if (leftEdge)
					edgeGraphic = UpLine;
				else if (upperEdge)
					edgeGraphic = LeftLine;
				drawGraphic(edgeGraphic, x, y);

				if (tile.spec == EMF_Tile_Spec::Wall || tile.spec == EMF_Tile_Spec::FakeWall)
					drawGraphic(Solid, x, y);
				else if (IsInteractiveTile(tile.spec) || tile.warp.warp_map > 0)
					drawGraphic(Interactive, x, y);
			}
		}

		for (const auto& player : this->m_game->map->m_Players)
		{
			if (player.second != nullptr)
				drawGraphic(player.first == World::WorldCharacterID ? MainPlayer : OtherPlayer, player.second->x, player.second->y);
		}
		for (const auto& npc : this->m_game->map->m_NPCs)
		{
			if (npc.second == nullptr)
				continue;
			const ENF::Type npcType = World::ENF_File->Get(npc.second->ID).type;
			drawGraphic(npcType == ENF::Passive || npcType == ENF::Aggressive ? AttackableNPC : OtherNPC, npc.second->x, npc.second->y);
		}
		break;
	}
	default:
		return;
	}
}

Map_UI_Map::~Map_UI_Map()
{

}
