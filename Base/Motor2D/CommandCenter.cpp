#include "EntityManager.h"
#include "CommandCenter.h"
#include "Scene.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Brofiler\Brofiler.h"


CmdCenter::CmdCenter() : Entity(entityType::COMMAND_CENTER)
{
}

CmdCenter::~CmdCenter()
{
}

CmdCenter::CmdCenter(bool isPlayer1, pair<int, int> pos) : Entity(entityType::COMMAND_CENTER)
{
	LOG("Loading CmdCenter");

	pugi::xml_document	config_file;
	pugi::xml_node		config;
	config = App->LoadConfig(config_file);
	config = config.child("entitymanager").child("buildings").child("command_center");


	health_lv.push_back(0);
	health_lv.push_back(config.child("health").attribute("lvl1").as_uint());
	health_lv.push_back(config.child("health").attribute("lvl2").as_uint());
	health_lv.push_back(config.child("health").attribute("lvl3").as_uint());

	upgrade_cost.push_back(0);
	upgrade_cost.push_back(config.child("upgrade_cost").attribute("ToLvl2").as_int());
	upgrade_cost.push_back(config.child("upgrade_cost").attribute("ToLvl3").as_int());

	damage_missiles.push_back(0);
	damage_missiles.push_back(config.child("damage_missiles").attribute("lvl1").as_uint());
	damage_missiles.push_back(config.child("damage_missiles").attribute("lvl2").as_uint());
	damage_missiles.push_back(config.child("damage_missiles").attribute("lvl3").as_uint());

	damage_cast2.push_back(0);
	damage_cast2.push_back(config.child("damage_cast2").attribute("lvl1").as_uint());
	damage_cast2.push_back(config.child("damage_cast2").attribute("lvl2").as_uint());
	damage_cast2.push_back(config.child("damage_cast2").attribute("lvl3").as_uint());

	damage_cast3.push_back(0);
	damage_cast3.push_back(config.child("damage_cast3").attribute("lvl1").as_uint());
	damage_cast3.push_back(config.child("damage_cast3").attribute("lvl2").as_uint());
	damage_cast3.push_back(config.child("damage_cast3").attribute("lvl3").as_uint());

	size.first = config.child("size").attribute("width").as_int();
	size.first = config.child("size").attribute("height").as_int();

	name = config.child("name").attribute("string").as_string();

	level = 1;
	health = health_lv[level];

	fromPlayer1 = isPlayer1;
	position = pos;

	upgrade = repair = false;
	damage = capacity = range = production = 0;

	LoadAnimations();
	ChangeAnimation();
}

bool CmdCenter::Update(float dt)
{
	BROFILER_CATEGORY("CmdCenter Update", Profiler::Color::SandyBrown);

	if (fromPlayer1)
	{
		if (health > 0) //if not destroyed
		{
			if (upgrade == true) //upgrade
			{
				App->player1->gold -= upgrade_cost[level]; //pay costs
				level++;
				upgrade = false;
			}
			if (App->player1->isCasting == true) //player casting
			{
				//if (missiles)
				damage = damage_missiles[level];

				//else if (cast2)
				damage = damage_cast2[level];

				//else if (cast 3)
				damage = damage_cast3[level];
			}
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
				App->player1->gold -= upgrade_cost[level]; //pay costs
				level++;
				upgrade = false;
			}
			if (App->player1->isCasting == true) //player casting
			{
				//if (missiles)
				damage = damage_missiles[level];

				//else if (cast2)
				damage = damage_cast2[level];

				//else if (cast 3)
				damage = damage_cast3[level];
			}
		}
		else
		{
			//destroyed
		}
	}
	
	ChangeAnimation();

	return true;
}

void CmdCenter::CleanUp()
{
	LOG("---CmdCenter Deleted");

	health_lv.clear();
	health_lv.resize(0);

	upgrade_cost.clear();
	upgrade_cost.resize(0);

	damage_missiles.clear();
	damage_missiles.resize(0);

	damage_cast2.clear();
	damage_cast2.resize(0);

	damage_cast3.clear();
	damage_cast3.resize(0);

}

void CmdCenter::LoadAnimations()
{
	if (fromPlayer1) //load player 1 sprites
	{

	}
	else if (!fromPlayer1) //load player 2 sprites
	{

	}

}

void CmdCenter::ChangeAnimation()
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
