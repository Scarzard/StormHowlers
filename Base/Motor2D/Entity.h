#ifndef __Entity_H__
#define __Entity_H__

#include "Log.h"
#include "Animation.h"
#include "Map.h"
#include "App.h"
#include "Animation.h"
#include "Render.h"
#include "Group.h"
#include "MovementManager.h"
#include "Brofiler/Brofiler.h"

struct Collider
{
	pair<int, int> dimensions;
	vector<pair<int, int>> tiles;
};
enum TroopState {
	NOT_DEPLOYED,
	TROOP_IDLE,
	MOVING,
	SHOOTING,
	REST,
	SEARCH,

	MAX_STATE
};


class Entity
{
public:
	enum entityType
	{
		TOWNHALL,
		MAIN_DEFENSE,
		COMMAND_CENTER,
		WALLS,
		DEFENSE_AOE,
		DEFENSE_TARGET,
		MINES,
		BARRACKS,
		SOLDIER,
		TANKMAN,
		INFILTRATOR,
		ENGINEER,
		WAR_HOUND
	};

public:
	Entity() {};
	Entity(Entity::entityType type, bool isPlayer1, pair<int, int> pos, Collider Collider) {
		BROFILER_CATEGORY("Entity constructor", Profiler::Color::Blue);
		this->type = type;
	
		char *s_type = (type > BARRACKS) ? "troops" : "buildings";
		name = "ERROR";
		name = GetName(type);
		collider = Collider;

		LOG("Player %d: Loading %s",(isPlayer1)?1:2,name.data());

		pugi::xml_document	config_file;
		pugi::xml_node config;
		config = App->LoadConfig(config_file);
		config = config.child("entitymanager").child(s_type).child(name.data());

		// Parsing data

		health_lv.push_back(config.child("health").attribute("lvl1").as_uint(0));
		health_lv.push_back(config.child("health").attribute("lvl2").as_uint(0));
		health_lv.push_back(config.child("health").attribute("lvl3").as_uint(0));

		/*upgrade_cost.push_back(0);
		upgrade_cost.push_back(config.child("upgrade_cost").attribute("ToLvl2").as_int(0));
		upgrade_cost.push_back(config.child("upgrade_cost").attribute("ToLvl3").as_int(0));*/

		damage_lv.push_back(config.child("damage").attribute("lvl1").as_uint(0));
		damage_lv.push_back(config.child("damage").attribute("lvl2").as_uint(0));
		damage_lv.push_back(config.child("damage").attribute("lvl3").as_uint(0));

		size.first = config.child("size").attribute("width").as_int(6);
		size.second = config.child("size").attribute("height").as_int(6);
		
		rate_of_fire = config.child("attack").attribute("rate").as_float(0);
		max_targets  = config.child("attack").attribute("targets").as_int(0);
		range		 = config.child("attack").attribute("range").as_int(0);

		position	= pos;

		fromPlayer1 = isPlayer1;
		upgrade		= false;
		repair		= false;

		level		= 0;
		
		health		= health_lv[level];
		damage		= damage_lv[level];


		// DEBUG PURPOSES
		//tex = App->tex->Load("maps/meta.png");

	};

	~Entity() {};
	virtual bool Awake(pugi::xml_node & config) { return true; };
	virtual bool Start() { return true; };
	virtual bool PreUpdate() { 	return true; };
	virtual bool Update(float dt) { 
		
		return true; };
	virtual bool PostUpdate(
	
	) { return true; };

	virtual void CleanUp() {
		health_lv.clear();
		health_lv.resize(0);

		//upgrade_cost.clear();
		//upgrade_cost.resize(0);

		damage_lv.clear();
		damage_lv.resize(0);

	};
	virtual void Save(pugi::xml_node& file) const {};
	virtual void Load(pugi::xml_node& file) {};
	virtual void Restart() {};
	virtual void LoadAnimations(bool isPlayer1, string path) {};

	// Damage animation does not care about levels(?)
	virtual void ChangeAnimation() {

		/*if (health <= 0)
			Current_Animation = destroyed;

		else if (health < (health_lv[level] / 2))
			Current_Animation = damaged;

		else if (level == 0)
			Current_Animation = level1;

		else if (level == 1)
			Current_Animation = level2;

		else if (level == 2)
			Current_Animation = level3;*/
	};

	virtual void TakeDamage(int damage) {
		if (alive) {

			health -= damage;

			if (health <= 0) {
				//alive = false;
				//LOG("P[%d] %s DEAD", (fromPlayer1) ? 1 : 2, &name[0]);
			}

			//LOG("P[%d] %s health: %d", (fromPlayer1)?1:2, &name[0], health);
		}
		
	};

	pair<int, int> GetSize() { return size; };
	pair<int, int> GetPos() { return position; };

public:
	Animation*	Current_Animation = nullptr;
	Animation*	building = nullptr;
	Animation*	level1 = nullptr;
	Animation*	level2 = nullptr;
	Animation*	level3 = nullptr;
	Animation*	damaged = nullptr;
	Animation*	destroyed = nullptr;
	Animation*  glow = nullptr; 

	entityType	type;
	string		name;

	vector<uint> health_lv;
	vector<uint> damage_lv;
	//vector<uint> upgrade_cost;
	int Upgrade_Cost = 0;

	pair<int, int> position;
	pair<int,int> size;
	
	Timer timer;

	bool fromPlayer1 = false;
	bool upgrade = false;
	bool repair = false;
	bool flip	= false;
	bool alive	= true;
	bool built = false;
	
	int health = 0;
	int damage = 0;
	int level = 0;
	int range = 0;
	int repair_cost = 0;
	int frame = 0;
	int max_targets = 0;

	float rate_of_fire = 0.0f;

	// DEBUG PURPOSES
	SDL_Rect	debug_collider;
	SDL_Rect	rect = { 180,0,60,30 };
	SDL_Texture* tex = nullptr;

	int offset = 0;
	Collider collider;

	// Group Movement
	int speed = 0;
	Group_Unit info;
	bool isSelected = true;
	TroopState state;
	pair<int, int> Speed = { 1,1 };

public:

		char* GetName(const Entity::entityType& type) {
			switch (type)
			{
			case Entity::entityType::TOWNHALL:
				return"Townhall";
				break;
			case Entity::entityType::MAIN_DEFENSE:
				return"sentrygun";
				break;
			case Entity::entityType::COMMAND_CENTER:
				return"CommandCenter";
				break;
			case Entity::entityType::WALLS:
				return"Walls";
				break;
			case Entity::entityType::DEFENSE_AOE:
				return"aoe";
				break;
			case Entity::entityType::DEFENSE_TARGET:
				return"Tesla";
				break;
			case Entity::entityType::MINES:
				return"GoldMine";
				break;
			case Entity::entityType::BARRACKS:
				return"Barracks";
				break;
			case Entity::entityType::SOLDIER:
				return"BasicSoldier";
				break;
			case Entity::entityType::TANKMAN:
				return"tankman";
				break;
			case Entity::entityType::INFILTRATOR:
				return"infiltrator";
				break;
			case Entity::entityType::ENGINEER:
				return"engineer";
				break;
			case Entity::entityType::WAR_HOUND:
				return"war_hound";
				break;
			default:
				break;
			}
		}
};

#endif