#include "EntityManager.h"
#include "MainDefense.h"
#include "Scene.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Brofiler\Brofiler.h"


MainDefense::MainDefense()
{
}

MainDefense::~MainDefense()
{
}

MainDefense::MainDefense(bool isPlayer1, pair<int, int> pos) : Building(Entity::entityType::MAIN_DEFENSE, isPlayer1, pos)
{
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


	Building::Update(dt);


	return true;
}

void MainDefense::CleanUp()
{
	LOG("---Main Defense Deleted");

}


