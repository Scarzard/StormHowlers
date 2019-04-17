#include "EntityManager.h"
#include "Townhall.h"
#include "Scene.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Brofiler\Brofiler.h"


Townhall::Townhall()
{
}

Townhall::~Townhall()
{
}

Townhall::Townhall(bool isPlayer1, pair<int,int> pos, Collider collider): Building(Entity::entityType::TOWNHALL,isPlayer1,pos, collider)
{

	string path = "animation/" + name + ".tmx";
	LoadAnimations(isPlayer1, path.data());
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

	Building::Update(dt);

	return true;
}

void Townhall::CleanUp()
{
	LOG("---Townhall Deleted");

	Building::CleanUp();


}

void Townhall::LoadAnimations(bool isPlayer1, string path)
{
}
