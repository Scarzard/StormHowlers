#include "EntityManager.h"
#include "Barracks.h"
#include "Scene.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Brofiler\Brofiler.h"


Barracks::Barracks() : Entity(entityType::BARRACKS)
{
}

Barracks::~Barracks()
{
}

Barracks::Barracks(bool isPlayer1, pair<int, int> pos) : Entity(entityType::BARRACKS)
{
	LOG("Loading Barracks");

	pugi::xml_document	config_file;
	pugi::xml_node		config;
	config = App->LoadConfig(config_file);
	config = config.child("entitymanager").child("buildings").child("barracks");


	health_lv.push_back(0);
	health_lv.push_back(config.child("health").attribute("lvl1").as_uint());
	health_lv.push_back(config.child("health").attribute("lvl2").as_uint());
	health_lv.push_back(config.child("health").attribute("lvl3").as_uint());

	capacity_lv.push_back(0);
	capacity_lv.push_back(config.child("capacity").attribute("lvl1").as_uint());
	capacity_lv.push_back(config.child("capacity").attribute("lvl2").as_uint());
	capacity_lv.push_back(config.child("capacity").attribute("lvl3").as_uint());

	upgrade_cost.push_back(0);
	upgrade_cost.push_back(config.child("upgrade_cost").attribute("ToLvl2").as_int());
	upgrade_cost.push_back(config.child("upgrade_cost").attribute("ToLvl3").as_int());

	size.first = config.child("size").attribute("width").as_int();
	size.first = config.child("size").attribute("height").as_int();

	name = config.child("name").attribute("string").as_string();

	level = 1;
	fromPlayer1 = isPlayer1;
	position = pos;
	capacity = capacity_lv[level];

	upgrade = repair = false;
	damage = production = range = 0;

	LoadAnimations();
	ChangeAnimation();
}

bool Barracks::PreUpdate()
{
	BROFILER_CATEGORY("Barracks PreUpdate", Profiler::Color::SandyBrown);

	//update repair cost

	return true;
}

bool Barracks::Update(float dt)
{
	BROFILER_CATEGORY("Barracks Update", Profiler::Color::SandyBrown);

	if (fromPlayer1)  // --- Player 1 --------------------------------
	{
		if (health > 0) //if not destroyed
		{
			if (upgrade == true) //upgrade
			{
				App->player1->gold -= upgrade_cost[level]; //pay costs
				capacity = capacity_lv[level]; //update capacity
				level++;
				upgrade = false;
			}
		}
		if (repair == true) //repair
		{
			App->player1->gold -= repair_cost;
			health = health_lv[level];
			repair = false;
		}
	}
	else if (!fromPlayer1) // --- Player 2 ---------------------------
	{
		if (health > 0) //if not destroyed
		{
			if (upgrade == true) //upgrade
			{
				App->player2->gold -= upgrade_cost[level]; //pay costs
				capacity = capacity_lv[level]; //update capacity
				level++;
				upgrade = false;
			}
		}
		if (repair == true) //repair
		{
			App->player2->gold -= repair_cost;
			health = health_lv[level];
			repair = false;
		}
	}

	ChangeAnimation();

	return true;
}

void Barracks::CleanUp()
{
	LOG("---Barracks Deleted");

	capacity_lv.clear();
	capacity_lv.resize(0);

	health_lv.clear();
	health_lv.resize(0);

	upgrade_cost.clear();
	upgrade_cost.resize(0);

}

void Barracks::LoadAnimations()
{
	if (fromPlayer1) //load player 1 sprites
	{

	}
	else if (!fromPlayer1) //load player 2 sprites
	{

	}

}

void Barracks::ChangeAnimation()
{
	if (health <= 0)
		Current_Animation = &destroyed;

	else if (health < (health_lv[level] / 2))
		Current_Animation = &damaged;

	else if (level == 1)
		Current_Animation = &level1;

	else if (level == 2)
		Current_Animation = &level2;

	else if (level == 3)
		Current_Animation = &level3;
}
