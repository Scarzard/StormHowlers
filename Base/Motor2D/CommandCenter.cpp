#include "EntityManager.h"
#include "CommandCenter.h"
#include "Scene.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Brofiler\Brofiler.h"

CmdCenter::CmdCenter()
{
}

CmdCenter::~CmdCenter()
{
}

CmdCenter::CmdCenter(bool isPlayer1, pair<int, int> pos) : Building(Entity::entityType::COMMAND_CENTER, isPlayer1, pos)
{
	pugi::xml_document	config_file;
	pugi::xml_node		config;
	config = App->LoadConfig(config_file);
	config = config.child("entitymanager").child("buildings").child("command_center");

	damage_cast2.push_back(0);
	damage_cast2.push_back(config.child("damage_cast2").attribute("lvl1").as_uint(0));
	damage_cast2.push_back(config.child("damage_cast2").attribute("lvl2").as_uint(0));
	damage_cast2.push_back(config.child("damage_cast2").attribute("lvl3").as_uint(0));

	damage_cast3.push_back(0);
	damage_cast3.push_back(config.child("damage_cast3").attribute("lvl1").as_uint(0));
	damage_cast3.push_back(config.child("damage_cast3").attribute("lvl2").as_uint(0));
	damage_cast3.push_back(config.child("damage_cast3").attribute("lvl3").as_uint(0));


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
				damage = damage_lv[level];

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
				damage = damage_lv[level];

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

	Building::Update(dt);

	return true;
}

void CmdCenter::CleanUp()
{
	LOG("---CmdCenter Deleted");

	damage_cast2.clear();
	damage_cast2.resize(0);

	damage_cast3.clear();
	damage_cast3.resize(0);

}

