#pragma once
#include "..\..\Game_Stage\Map_Element\CharacterModel.h"
class Map_Player : public CharacterModel
{
	int FindWalkDirection(int dest_x, int dest_y);
public:
	std::string PlayerName = "";
	int x = 0;
	int y = 0;
	int destination_x = -1;
	int destination_y = -1;
	void Update(double deltaSeconds);
	/**
 * One type of item in a Characters inventory
 */
	struct Character_Item
	{
		short id = 0;
		int amount = 0;

		Character_Item() = default;
		Character_Item(short id, int amount) : id(id), amount(amount) { }
	};

	/**
	 * One spell that a Character knows
	 */
	struct Character_Spell
	{
		short id = 0;
		unsigned char level = 0;

		Character_Spell() = default;
		Character_Spell(short id, unsigned char level) : id(id), level(level) { }
	};
	void SetStance(PlayerStance m_Stance);
	void MovePlayer(double deltaSeconds, int dest_x, int dest_y);

	void Initialize(Game* M_Game);
	int login_time = 0;
	bool online = false;
	bool nowhere = false;
	unsigned int CharacterID = 0;
	
	double WalkElapsedSeconds = 0.0;
	double AnimationElapsedSeconds = 0.0;
	double DamageElapsedSeconds = 0.0;
	double DeathElapsedSeconds = 0.0;
	//AdminLevel admin;
	std::string guildname;
	std::string guildtag;
	std::string guildrank;
	std::string real_name;
	std::string title;
	std::string home;
	std::string fiance;
	std::string partner;
	unsigned char clas = 0;
	short mapid = 0;
	int fpscounter = 0;
	int moveFPS = 0;
	int WalkCounter = 0;
	//Direction direction;
	unsigned char level = 0;
	int exp = 0;
	short hp = 0, tp = 0;
	short str = 0, intl = 0, wis = 0, agi = 0, con = 0, cha = 0;
	short adj_str = 0, adj_intl = 0, adj_wis = 0, adj_agi = 0, adj_con = 0, adj_cha = 0;
	short statpoints = 0, skillpoints = 0;
	short weight = 0, maxweight = 0;
	short karma = 0;
	//SitState sitting;
	int hidden = 0;
	int nointeract = 0;
	bool whispers = true;
	int bankmax = 0;
	int goldbank = 0;
	int usage = 0;
	int muted_until = 0;
	short maxsp = 0;
	short maxhp = 1, maxtp = 1;
	short accuracy = 0, evade = 0, armor = 0;
	short mindam = 0, maxdam = 0;

	std::list<Character_Item> bank;

	std::list<Character_Spell> spells;
	int time = 0;
	int Deathcounter = 0;
	std::vector<unsigned char> Damage;
	void DealDamage(int Damage);
	void PlayerKill();
	bool isattacked = false;
	void Map_PlayerRender(sf::Sprite* _Sprite, int x, int y, float depth, sf::Color _Color = sf::Color::White );
	Map_Player();
	~Map_Player();
};

