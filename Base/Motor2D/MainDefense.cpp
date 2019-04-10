#include "EntityManager.h"
#include "MainDefense.h"
#include "Scene.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Brofiler\Brofiler.h"


MainDefense::MainDefense() : Entity(entityType::MAIN_DEFENSE)
{
}

MainDefense::~MainDefense()
{
}

MainDefense::MainDefense(bool isPlayer1, pair<int, int> pos) : Entity(entityType::MAIN_DEFENSE)
{
	LOG("Loading Main Defense");

	pugi::xml_document	config_file;
	pugi::xml_node		config;
	config = App->LoadConfig(config_file);
	config = config.child("entitymanager").child("buildings").child("main_defense");


	health_lv.push_back(0);
	health_lv.push_back(config.child("health").attribute("lvl1").as_uint());
	health_lv.push_back(config.child("health").attribute("lvl2").as_uint());
	health_lv.push_back(config.child("health").attribute("lvl3").as_uint());

	damage_lv.push_back(0);
	damage_lv.push_back(config.child("damage").attribute("lvl1").as_uint());
	damage_lv.push_back(config.child("damage").attribute("lvl2").as_uint());
	damage_lv.push_back(config.child("damage").attribute("lvl3").as_uint());

	upgrade_cost.push_back(0);
	upgrade_cost.push_back(config.child("upgrade_cost").attribute("ToLvl2").as_int());
	upgrade_cost.push_back(config.child("upgrade_cost").attribute("ToLvl3").as_int());

	size.first = config.child("size").attribute("width").as_int();
	size.first = config.child("size").attribute("height").as_int();

	name = config.child("name").attribute("string").as_string();
	range = config.child("range").attribute("value").as_uint();

	level = 1;
	health = health_lv[level];
	damage = damage_lv[level];

	fromPlayer1 = isPlayer1;
	position = pos;

	upgrade = repair = false;
	production = capacity = level = 0;

	LoadAnimations();
	ChangeAnimation();
}

bool MainDefense::Update(float dt)
{
	BROFILER_CATEGORY("Main Defense Update", Profiler::Color::SandyBrown);

	if (fromPlayer1)
	{
		if (health > 0) //if not destroyed
		{
			if (upgrade == true) //upgrade
			{
				App->player2->gold -= upgrade_cost[level]; //pay costs
				level++;
				damage = damage_lv[level]; //update production
				upgrade = false;
				//play fx (upgrade);
			}

			//if troops in range -> attack
		}
		else
		{
			//destroyed
		}
	}
	else if (!fromPlayer1)
	{
		if (health > 0) //if not destroyed
		{
			if (upgrade == true) //upgrade
			{
				App->player2->gold -= upgrade_cost[level]; //pay costs
				level++;
				damage = damage_lv[level]; //update production
				upgrade = false;
				//play fx (upgrade);
			}

			//if troops in range -> attack
		}
		else
		{
			//destroyed
		}
	}
	ChangeAnimation();

	return true;
}

void MainDefense::CleanUp()
{
	LOG("---Main Defense Deleted");

	damage_lv.clear();
	damage_lv.resize(0);

	health_lv.clear();
	health_lv.resize(0);

	upgrade_cost.clear();
	upgrade_cost.resize(0);
}

void MainDefense::LoadAnimations()
{
	if (fromPlayer1) //load player 1 sprites
	{

	}
	else if (!fromPlayer1) //load player 2 sprites
	{

	}

}

void MainDefense::ChangeAnimation()
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
