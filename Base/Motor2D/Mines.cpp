#include "EntityManager.h"
#include "Mines.h"
#include "Scene.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Brofiler\Brofiler.h"


Mines::Mines()
{
}

Mines::~Mines()
{
}

Mines::Mines(bool isPlayer1, pair<int, int> pos) : Building(Entity::entityType::MINES, isPlayer1, pos)
{

}

bool Mines::PreUpdate()
{
	BROFILER_CATEGORY("Mines PreUpdate", Profiler::Color::SandyBrown);

	repair_cost = (health_lv[level] - health ) / 2;

	return true;
}

bool Mines::Update(float dt)
{
	BROFILER_CATEGORY("Mines Update", Profiler::Color::SandyBrown);

		if (fromPlayer1)  // --- Player 1 --------------------------------
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
			if (repair == true) //repair
			{
				App->player1->gold -= repair_cost;
				health = health_lv[level];
				repair = false;
				//play fx (repair);
			}
		}
		else if (!fromPlayer1) // --- Player 2 ---------------------------
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
			if (repair == true) //repair
			{
				App->player2->gold -= repair_cost;
				health = health_lv[level];
				repair = false;
				//play fx (repair);
			}
		}

	ChangeAnimation();

	return true;
}

void Mines::CleanUp()
{
	LOG("---Mine Deleted");

}

