#include "..\..\stdafx.h"
#include "Map_NPC.h"
#include "..\..\game.h"
#include "..\..\World.h"
#include <algorithm>
#include <cmath>

namespace
{
	constexpr double NPCWalkSeconds = 0.40;
	constexpr std::size_t MaximumQueuedWalks = 8;
	constexpr double NPCActionFrameSeconds = 0.08;
	constexpr double NPCActionSeconds = 0.24;
	constexpr double DamageDisplaySeconds = 0.40;
}

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#undef max
#undef min
Map_NPC::Map_NPC()
{
}


Map_NPC::~Map_NPC()
{
}

void Map_NPC::Initialize(LPVOID* M_Game)
{
	m_Game = (Game*)M_Game;
	this->Deathcounter = 0;
}
void Map_NPC::SetStance(NPC_Stance m_Stance)
{
	this->Stance = m_Stance;
	this->FrameID = 0;
	FrameCounter = 0;
	this->AnimationElapsedSeconds = 0.0;
	if (m_Stance != NPC_Stance::Walking)
	{
		this->destination_x = -1;
		this->destination_y = -1;
		this->WalkElapsedSeconds = 0.0;
		this->xoffset = 0;
		this->yoffset = 0;
		this->QueuedWalks.clear();
	}
}

void Map_NPC::QueueWalk(int dest_x, int dest_y)
{
	if (this->destination_x == dest_x && this->destination_y == dest_y)
		return;
	if (!this->QueuedWalks.empty() && this->QueuedWalks.back().first == dest_x && this->QueuedWalks.back().second == dest_y)
		return;
	if (this->QueuedWalks.size() >= MaximumQueuedWalks)
		this->QueuedWalks.pop_front();
	this->QueuedWalks.emplace_back(dest_x, dest_y);
}

bool Map_NPC::IsWalkingTo(int dest_x, int dest_y) const
{
	if (this->destination_x == dest_x && this->destination_y == dest_y)
		return true;
	for (const auto& queuedWalk : this->QueuedWalks)
	{
		if (queuedWalk.first == dest_x && queuedWalk.second == dest_y)
			return true;
	}
	return false;
}

int  Map_NPC::FindWalkDirection(int dest_x, int dest_y)
{
	this->destination_x = dest_x;
	this->destination_y = dest_y;

	int desx = this->x - dest_x;
	int desy = this->y - dest_y;
	if (desx < 0)
	{
		return 2;
	}
	else if (desx > 0)
	{
		return 3;
	}
	else if (desy < 0)
	{
		return 0;
	}
	else if (desy > 0)
	{
		return 1;
	}
	return 0;
}
void Map_NPC::MoveNPC(double deltaSeconds, int DestX, int DestY)
{
	const bool startingWalk = this->destination_x != DestX || this->destination_y != DestY || this->Stance != NPC_Stance::Walking;
	int move_direction = this->FindWalkDirection(DestX, DestY);
	this->direction = move_direction;
	if (startingWalk)
	{
		this->WalkElapsedSeconds = 0.0;
		this->xoffset = 0;
		this->yoffset = 0;
		this->SetStance(NPC_Stance::Walking);
	}

	this->WalkElapsedSeconds += (std::max)(0.0, deltaSeconds);
	const double progress = (std::min)(1.0, this->WalkElapsedSeconds / NPCWalkSeconds);
	const float horizontal = static_cast<float>(32.0 * progress);
	const float vertical = horizontal * 0.5f;
	this->FrameID = (std::min)(3, static_cast<int>(progress * 4.0));
	switch (move_direction)
	{
		case 0: this->xoffset = -horizontal; this->yoffset = vertical; break;
		case 1: this->xoffset = horizontal; this->yoffset = -vertical; break;
		case 2: this->xoffset = horizontal; this->yoffset = vertical; break;
		case 3: this->xoffset = -horizontal; this->yoffset = -vertical; break;
	}
	if (progress >= 1.0)
	{
		const double remainingSeconds = (std::max)(0.0, this->WalkElapsedSeconds - NPCWalkSeconds);
		auto queuedWalks = std::move(this->QueuedWalks);
		this->x = DestX;
		this->y = DestY;
		this->destination_x = -1;
		this->destination_y = -1;
		this->xoffset = 0;
		this->yoffset = 0;
		this->WalkElapsedSeconds = 0.0;
		this->SetStance(Map_NPC::NPC_Stance::Standing);
		if (!queuedWalks.empty())
		{
			const auto queuedWalk = queuedWalks.front();
			queuedWalks.pop_front();
			this->QueuedWalks = std::move(queuedWalks);
			this->MoveNPC(remainingSeconds, queuedWalk.first, queuedWalk.second);
		}
	}
}
void Map_NPC::Update(double deltaSeconds)
{
	if (this->Deathcounter > 0)
	{
		this->DeathElapsedSeconds += deltaSeconds;
	}
	if (this->destination_x >= 0 || this->destination_y >= 0)
	{
		MoveNPC(deltaSeconds, destination_x, destination_y);
	}
	if (this->Damage.size() > 0)
	{
		this->DamageElapsedSeconds += deltaSeconds;
	}
	if (this->DamageElapsedSeconds >= DamageDisplaySeconds)
	{
		this->Damage.clear();
		this->time = 0;
		this->DamageElapsedSeconds = 0.0;
		this->isattacked = false;
	}
	this->AnimationElapsedSeconds += deltaSeconds;
	switch (this->Stance)
	{
	case(NPC_Stance::Standing):
	{
		this->FrameID = 0;
		this->FrameCounter = 0;
		break;
	}
	case(NPC_Stance::Walking):
	{
		break;
	}
	case(NPC_Stance::Attacking):
	{
		this->FrameID = this->AnimationElapsedSeconds >= NPCActionFrameSeconds ? 1 : 0;
		if (this->AnimationElapsedSeconds >= NPCActionSeconds)
		{
			this->SetStance(NPC_Stance::Standing);
		}
		break;
	}
	case(NPC_Stance::Dead):
	{
		this->FrameID = 0;
		break;
	}
	}
}

void Map_NPC::DealDamage(short HpLeft, int _damage)
{
	const ENF_Data& npcData = World::ENF_File->Get(this->ID);
	if (!npcData)
		return;
	this->MaxHP = (std::max)(1, npcData.hp);
	this->HP = (float)(HpLeft / 100.0f)*(float)this->MaxHP;
	std::string p_Damage = to_string(_damage);
	this->Damage.clear();
	this->time = 0;
	this->DamageElapsedSeconds = 0.0;
	for (int i = 0; i < p_Damage.size(); i++)
	{
		unsigned char damage = p_Damage[i];
		this->Damage.push_back(damage);
	}
	std::reverse(this->Damage.begin(), this->Damage.end());
	this->isattacked = true;
}
void Map_NPC::NPCKill()
{
	this->SetStance(Map_NPC::NPC_Stance::Standing);
	this->DeathElapsedSeconds = 0.0;
	this->Deathcounter += 1;
}
void Map_NPC::Render(sf::Sprite* _Sprite, float x, float y, float depth, sf::Color m_color)
{
	const ENF_Data& npcData = World::ENF_File->Get(this->ID);
	if (!npcData)
		return;
	int IndexOffSet = 0;
	int DirectionOffset = 0;
	if (this->Stance == Map_NPC::Standing) { IndexOffSet = 0; DirectionOffset = 2; }
	if (this->Stance == Map_NPC::Walking) { IndexOffSet = 4; DirectionOffset = 4;}
	if (this->Stance == Map_NPC::Attacking) { IndexOffSet = 4 + 8; DirectionOffset = 2;}
	int Scaleflip = 0;
	if (this->direction == 1 || this->direction == 2)
	{
		Scaleflip = 1;
	}

	int TextureIndex = 1 + IndexOffSet + FrameID + (npcData.graphic - 1) * 40;
	if (this->direction == 1 || this->direction == 3)
	{
 		TextureIndex += DirectionOffset;
	}
	int height = this->m_Game->ResourceManager->GetResource(21, TextureIndex, true)->_height;
	int width = this->m_Game->ResourceManager->GetResource(21, TextureIndex, true)->_width;
	
	sf::Vector2f Scale =  sf::Vector2f(1 - (Scaleflip * 2), 1);
	int offsety = max(0, (std::min(41, width - 23)) / 4);
	sf::Vector3f Pos(x + 32 - (width / 2) + this->xoffset, offsety + y +  this->yoffset + 22 - height, depth);
	const float deathProgress = (std::min)(1.0f, static_cast<float>(this->DeathElapsedSeconds / 0.40));
	const sf::Uint8 deathAlpha = static_cast<sf::Uint8>(255.0f * (1.0f - deathProgress));
	m_color = sf::Color(255, 255, 255, deathAlpha);

	//_Sprite->SetTransform(&mat);
	this->m_Game->Draw(this->m_Game->ResourceManager->GetResource(21, TextureIndex, true), Pos.x, Pos.y, m_color, 0, 0, -1, -1, Scale, depth);
	//_Sprite->Draw(m_n_Game->ResourceManager->CreateTexture(21, TextureIndex, true)._Texture.get(), NULL, Center, Pos, m_color);
	//_Sprite->SetTransform(originalTransform);

	//delete originalTransform;
	if (this->isattacked)
	{
		int _x = this->x;
		int _y = this->y;
		RECT IconSrcRect;
		IconSrcRect.left = 0;
		IconSrcRect.top = 28;
		IconSrcRect.bottom = IconSrcRect.top + 7;
		IconSrcRect.right = 40;
		int scalex = 1;

		if (this->direction == 1 || this->direction == 2)
		{
			scalex = -1;
		}
		//Pos = new sf::Vector3f(x + 32 - (width / 2) + this->xoffset, offsety + y + this->yoffset + 22 - height, depth);
		sf::Vector3f IconPos(x + 32 - 35/2 +this->xoffset * scalex, y + this->yoffset +22 - height, 0);
	//	m_n_Game->map->Sprite->Draw(m_n_Game->Map_UserInterface->HudStatsTexture.get(), &IconSrcRect, IconCentre, IconPos, sf::Color::Color(255, 255, 255, 255));
		this->m_Game->Draw(this->m_Game->ResourceManager->GetResource(2, 58, true), IconPos.x, IconPos.y, sf::Color(255, 255, 255, 255), IconSrcRect.left, IconSrcRect.top, IconSrcRect.right, IconSrcRect.bottom, sf::Vector2f(1, 1), 0.02f);

		float HPPercent = this->MaxHP > 0 ? (float)this->HP / (float)this->MaxHP : 0.0f;
		HPPercent = (std::max)(0.0f, (std::min)(1.0f, HPPercent));
		int hpcolormultiplier = 0;
		if (HPPercent < 0.7f)
		{
			hpcolormultiplier++;
		}
		if (HPPercent < 0.4f)
		{
			hpcolormultiplier++;
		}
		IconSrcRect.left = 0;
		IconSrcRect.top = 35 + (hpcolormultiplier*7);
		IconSrcRect.bottom = IconSrcRect.top + 7;
		IconSrcRect.right = HPPercent*40;
		//m_n_Game->map->Sprite->Draw(m_n_Game->Map_UserInterface->HudStatsTexture.get(), &IconSrcRect, IconCentre, IconPos, sf::Color::Color(255, 255, 255, 255));
		this->m_Game->Draw(this->m_Game->ResourceManager->GetResource(2, 58, true), IconPos.x, IconPos.y, sf::Color(255, 255, 255, 255), IconSrcRect.left, IconSrcRect.top, IconSrcRect.right, IconSrcRect.bottom, sf::Vector2f(1, 1), 0.02f);

		const float damageProgress = (std::min)(1.0f, static_cast<float>(this->DamageElapsedSeconds / DamageDisplaySeconds));
		IconPos.y -= 14;
		IconPos.x += 20;
		IconPos.x += (this->Damage.size() * 9) / 2;
		IconPos.y -= 7.0f * damageProgress;
		for (int i = 0; i < this->Damage.size(); i++)
		{
			unsigned char damage = this->Damage[i] - 48;

			IconSrcRect.left = 40 + (damage * 9);
			IconSrcRect.top = 28;
			IconSrcRect.bottom = IconSrcRect.top + 12;
			IconSrcRect.right = IconSrcRect.left + 9;
			if (damage == 0 && this->Damage.size() == 1)
			{
				IconSrcRect.left = 132;
				IconSrcRect.top = 28;
				IconSrcRect.bottom = IconSrcRect.top + 12;
				IconSrcRect.right = IconSrcRect.left + 30;
				IconPos.x -= 18;
			}
			else
			{
				IconPos.x -= 9;
			}
			
			const sf::Uint8 alpha = static_cast<sf::Uint8>(255.0f - 160.0f * damageProgress);
			//m_n_Game->map->Sprite->Draw(m_n_Game->Map_UserInterface->HudStatsTexture.get(), &IconSrcRect, IconCentre, IconPos, sf::Color::Color(int)alpha, 255, 255, 255));
			this->m_Game->Draw(this->m_Game->ResourceManager->GetResource(2, 58, true), IconPos.x, IconPos.y, sf::Color(255, 255, 255, alpha), IconSrcRect.left, IconSrcRect.top, IconSrcRect.right, IconSrcRect.bottom, sf::Vector2f(1, 1), 0.02f);

		}
	}
}
