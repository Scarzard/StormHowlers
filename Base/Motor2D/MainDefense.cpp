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

MainDefense::MainDefense(bool isPlayer1, pair<int, int> pos, Collider collider) : Building(Entity::entityType::MAIN_DEFENSE, isPlayer1, pos, collider)
{
	string path = "animation/" + name + ".tmx";
	LoadAnimations(isPlayer1, path.data());
}

bool MainDefense::Update(float dt)
{
	BROFILER_CATEGORY("Main Defense Update", Profiler::Color::SandyBrown);
	
	if (fromPlayer1 == true)
	{
		position = App->map->data.main_tower;
	}
	else
	{
		position = App->map->data.main_tower2;
	}

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
			App->player1->DeleteEntity(this);
			App->player1->UpdateWalkabilityMap(true, collider); //destroyed
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
			App->player2->DeleteEntity(this);
			App->player2->UpdateWalkabilityMap(true, collider); //destroyed
		}
	}
	ChangeAnimation();


	Building::Update(dt);


	return true;
}

void MainDefense::CleanUp()
{
	LOG("---Main Defense Deleted");
	Building::CleanUp();

}

void MainDefense::LoadAnimations(bool isPlayer1, string path)
{
	building = building->LoadAnimation(path.data(), (!isPlayer1) ? "red_constructing" : "blue_constructing");
	level1 = level1->LoadAnimation(path.data(), (!isPlayer1) ? "red_idle" : "blue_idle");
	level1->speed = 3;
	building->speed = 3;
	building->loop = false;
	level1->loop = false;
	Current_Animation = building;
}


