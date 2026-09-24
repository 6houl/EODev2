#pragma once
#include <deque>

class Map_NPC
{
	int FindWalkDirection(int dest_x, int dest_y);
	Game* m_Game = nullptr;
public:
	Map_NPC();
	int Index = 0;
	int ID = 0;
	int x = 0, y = 0, direction = 0;
	int FrameID = 0;
	int FrameCounter = 0;
	int HP = 0;
	int MaxHP = 1;
	int destination_x = -1;
	int destination_y = -1;
	int moveFPS = 0;
	int yoffset = 0;
	int xoffset = 0;
	int WalkCounter = 0;
	double WalkElapsedSeconds = 0.0;
	double AnimationElapsedSeconds = 0.0;
	double DamageElapsedSeconds = 0.0;
	double DeathElapsedSeconds = 0.0;
	std::deque<std::pair<int, int>> QueuedWalks;

	bool isattacked = false;
	int time = 0;

	int Deathcounter = 0;
	std::vector<unsigned char> Damage;

	void NPCKill();
	enum NPC_Stance
	{
		Standing,
		Walking,
		Attacking,
		Dead
	};
	//sf::Color::White Color;
	NPC_Stance Stance = NPC_Stance::Standing;
	void Initialize(LPVOID* m_game);
	void SetStance(NPC_Stance m_Stance);
	void MoveNPC(double deltaSeconds, int DestX,int DestY);
	void QueueWalk(int dest_x, int dest_y);
	bool IsWalkingTo(int dest_x, int dest_y) const;
	void DealDamage(short HpLeft, int damage);
	void Update(double deltaSeconds);
	void Render(sf::Sprite* _Sprite, int x, int y, float depth, sf::Color m_color = sf::Color::White );
	~Map_NPC();
};

