#include "EntityManager.h"
#include "Townhall.h"
#include "Scene.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Brofiler\Brofiler.h"


Townhall::Townhall() : Entity(entityType::TOWNHALL)
{
}

Townhall::~Townhall()
{
}

Townhall::Townhall(bool isPlayer1, pair<int,int> pos) : Entity(entityType::TOWNHALL)
{
	LOG("Loading Townhall");

	pugi::xml_document	config_file;
	pugi::xml_node		config;
	config = App->LoadConfig(config_file);
	config = config.child("entitymanager").child("buildings").child("townhall");


	health_lv.push_back(0);
	health_lv.push_back(config.child("health").attribute("lvl1").as_uint());
	health_lv.push_back(config.child("health").attribute("lvl2").as_uint());
	health_lv.push_back(config.child("health").attribute("lvl3").as_uint());

	production_lv.push_back(0);
	production_lv.push_back(config.child("production").attribute("lvl1").as_uint());
	production_lv.push_back(config.child("production").attribute("lvl2").as_uint());
	production_lv.push_back(config.child("production").attribute("lvl3").as_uint());

	upgrade_cost.push_back(0);
	upgrade_cost.push_back(config.child("upgrade_cost").attribute("ToLvl2").as_int());
	upgrade_cost.push_back(config.child("upgrade_cost").attribute("ToLvl3").as_int());

	size.first = config.child("size").attribute("width").as_int();
	size.first = config.child("size").attribute("height").as_int();

	name = config.child("name").attribute("string").as_string();

	level = 1;
	health = health_lv[level];
	production = production_lv[level];

	fromPlayer1 = isPlayer1;
	position = pos;

	upgrade = repair = false;
	damage = capacity = range = 0;

	LoadAnimations();
	ChangeAnimation();
}

bool Townhall::Update(float dt)
{
	BROFILER_CATEGORY("Townhall Update", Profiler::Color::SandyBrown);

	if (fromPlayer1)
	{
		if (health > 0) //if not destroyed
		{
			if (upgrade == true) //upgrade
			{
				App->player1->gold -= upgrade_cost[level]; //pay costs
				level++;
				production = production_lv[level]; //update production
				upgrade = false;
				//play fx (upgrade);
			}
		}
		else
		{
			//player 1 lose
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
				production = production_lv[level]; //update production
				upgrade = false;
				//play fx (upgrade);
			}
		}
		else
		{
			//player 2 lose
		}
	}
	ChangeAnimation();

	return true;
}

void Townhall::CleanUp()
{
	LOG("---Townhall Deleted");

	production_lv.clear();
	production_lv.resize(0);

	health_lv.clear();
	health_lv.resize(0);

	upgrade_cost.clear();
	upgrade_cost.resize(0);

}

void Townhall::LoadAnimations()
{
	if (fromPlayer1) //load player 1 sprites
	{

	}
	else if (!fromPlayer1) //load player 2 sprites
	{

	}

}

void Townhall::ChangeAnimation()
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
