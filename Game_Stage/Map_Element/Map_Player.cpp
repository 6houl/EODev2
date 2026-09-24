#include "..\..\stdafx.h"
#include "Map_Player.h"
#include "..\..\World.h"
#include "..\..\game.h"
#include <algorithm>
#include <cmath>

namespace
{
	constexpr double PlayerWalkSeconds = 0.45;
	constexpr std::size_t MaximumQueuedWalks = 8;
	constexpr double PlayerActionFrameSeconds = 0.12;
	constexpr double PlayerActionSeconds = 0.60;
	constexpr double DamageDisplaySeconds = 0.40;
}

Map_Player::Map_Player()
{
}


Map_Player::~Map_Player()
{
}
void Map_Player::Initialize(Game* M_Game)
{
	this->m_game = M_Game;
	this->Deathcounter = 0;
	this->InitializeModel(M_Game);
}
void Map_Player::SetStance(PlayerStance m_Stance)
{
	this->Stance = m_Stance;
	fpscounter = 0;
	this->frame_ID = 0;
	this->AnimationElapsedSeconds = 0.0;
	if (m_Stance != PlayerStance::Walking)
	{
		this->destination_x = -1;
		this->destination_y = -1;
		this->WalkElapsedSeconds = 0.0;
		this->xoffset = 0;
		this->yoffset = 0;
		this->QueuedWalks.clear();
	}
}

void Map_Player::QueueWalk(int dest_x, int dest_y)
{
	if (this->destination_x == dest_x && this->destination_y == dest_y)
		return;
	if (!this->QueuedWalks.empty() && this->QueuedWalks.back().first == dest_x && this->QueuedWalks.back().second == dest_y)
		return;
	if (this->QueuedWalks.size() >= MaximumQueuedWalks)
		this->QueuedWalks.pop_front();
	this->QueuedWalks.emplace_back(dest_x, dest_y);
}

bool Map_Player::IsWalkingTo(int dest_x, int dest_y) const
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

int  Map_Player::FindWalkDirection(int dest_x, int dest_y)
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

void Map_Player::MovePlayer(double deltaSeconds, int dest_x, int dest_y)
{
	const bool startingWalk = this->destination_x != dest_x || this->destination_y != dest_y || this->Stance != PlayerStance::Walking;
	int move_direction = this->FindWalkDirection(dest_x, dest_y);
	this->direction = move_direction;
	if (startingWalk)
	{
		this->WalkElapsedSeconds = 0.0;
		this->xoffset = 0;
		this->yoffset = 0;
		this->SetStance(PlayerStance::Walking);
	}

	this->WalkElapsedSeconds += (std::max)(0.0, deltaSeconds);
	const double progress = (std::min)(1.0, this->WalkElapsedSeconds / PlayerWalkSeconds);
	const int horizontal = static_cast<int>(std::lround(32.0 * progress));
	const int vertical = static_cast<int>(std::lround(16.0 * progress));
	this->frame_ID = (std::min)(3, static_cast<int>(progress * 4.0));

	switch (move_direction)
	{
		case 0: this->xoffset = -horizontal; this->yoffset = vertical; break;
		case 1: this->xoffset = horizontal; this->yoffset = -vertical; break;
		case 2: this->xoffset = horizontal; this->yoffset = vertical; break;
		case 3: this->xoffset = -horizontal; this->yoffset = -vertical; break;
	}

	if (progress >= 1.0)
	{
		const double remainingSeconds = (std::max)(0.0, this->WalkElapsedSeconds - PlayerWalkSeconds);
		auto queuedWalks = std::move(this->QueuedWalks);
		this->x = dest_x;
		this->y = dest_y;
		this->destination_x = -1;
		this->destination_y = -1;
		this->xoffset = 0;
		this->yoffset = 0;
		this->WalkElapsedSeconds = 0.0;
		this->frame_ID = 0;
		this->SetStance(CharacterModel::PlayerStance::Standing);
		if (!queuedWalks.empty())
		{
			const auto queuedWalk = queuedWalks.front();
			queuedWalks.pop_front();
			this->QueuedWalks = std::move(queuedWalks);
			this->MovePlayer(remainingSeconds, queuedWalk.first, queuedWalk.second);
		}
	}
}

void Map_Player::DealDamage(int Damage)
{
	this->hp -= Damage;
	std::string p_Damage = to_string(Damage);
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

void Map_Player::PlayerKill()
{
	this->SetStance(Map_Player::PlayerStance::Standing);
	this->DeathElapsedSeconds = 0.0;
	this->Deathcounter += 1;
}
void Map_Player::Update(double deltaSeconds)
{
	if (this->destination_x >= 0 || this->destination_y >= 0)
	{
		MovePlayer(deltaSeconds, destination_x, destination_y);
	}
	if (this->Damage.size() > 0)
	{
		this->DamageElapsedSeconds += deltaSeconds;
	}
	if (this->Deathcounter > 0)
	{
		this->DeathElapsedSeconds += deltaSeconds;
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
		case(PlayerStance::Standing):
		{
			this->frame_ID = 0;
			fpscounter = 0;
			break;
		}
		case(PlayerStance::BluntAttacking):
		{
			this->frame_ID = this->AnimationElapsedSeconds >= PlayerActionFrameSeconds ? 1 : 0;
			if (this->AnimationElapsedSeconds >= PlayerActionSeconds)
			{
				this->SetStance(CharacterModel::Standing);
			}
			break;
		}
		case(PlayerStance::BowAttacking):
		{
			this->frame_ID = 0;
			if (this->AnimationElapsedSeconds >= PlayerActionSeconds)
			{
				this->SetStance(CharacterModel::Standing);
			}
			break;
		}
		case(PlayerStance::Spelling):
		{
			if (this->AnimationElapsedSeconds >= PlayerActionSeconds)
			{
				this->frame_ID = 0;
				fpscounter = 0;
				this->SetStance(CharacterModel::Standing);
			}
			break;
		}
		case(PlayerStance::GroundSitting):
		{
			this->frame_ID = 0;
			fpscounter = 0;
			break;
		}
		case(PlayerStance::ChairSitting):
		{
			this->frame_ID = 0;
			fpscounter = 0;
			break;
		}
	}
}

void Map_Player::Map_PlayerRender(sf::Sprite* _Sprite, int x, int y, float depth, sf::Color m_Color)
{
	this->Render(x, y, depth, m_Color);

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
		sf::Vector3f IconPos(x + 6 - 35 / 2 + this->xoffset * scalex, y + this->yoffset + 50 - 70, 0);
		this->m_game->Draw(this->m_game->ResourceManager->GetResource(2, 58, true), IconPos.x, IconPos.y, sf::Color::White, IconSrcRect.left, IconSrcRect.top, IconSrcRect.right, IconSrcRect.bottom, sf::Vector2f(1, 1), depth);
		//_Sprite->Draw(m_this->m_game->Map_UserInterface->HudStatsTexture.get(), &IconSrcRect, IconCentre, IconPos, sf::Color::Color(255, 255, 255, 255));

		float HPPercent = this->maxhp > 0 ? (float)this->hp / (float)this->maxhp : 0.0f;
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
		IconSrcRect.top = 35 + (hpcolormultiplier * 7);
		IconSrcRect.bottom = IconSrcRect.top + 7;
		IconSrcRect.right = HPPercent * 40;

		this->m_game->Draw(this->m_game->ResourceManager->GetResource(2, 58, true), IconPos.x, IconPos.y, sf::Color::White, IconSrcRect.left, IconSrcRect.top, IconSrcRect.right, IconSrcRect.bottom, sf::Vector2f(1, 1), depth);
		//m_this->m_game->map->Sprite->Draw(m_this->m_game->Map_UserInterface->HudStatsTexture.get(), &IconSrcRect, IconCentre, IconPos, sf::Color::Color(255, 255, 255, 255));
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
			//m_this->m_game->map->Sprite->Draw(m_this->m_game->Map_UserInterface->HudStatsTexture.get(), &IconSrcRect, IconCentre, IconPos, sf::Color::Color(int)alpha, 255, 255, 255));
			this->m_game->Draw(this->m_game->ResourceManager->GetResource(2, 58, true), IconPos.x, IconPos.y, sf::Color(255,255,255,alpha), IconSrcRect.left, IconSrcRect.top, IconSrcRect.right, IconSrcRect.bottom, sf::Vector2f(1, 1), depth);
		}
	}
}
