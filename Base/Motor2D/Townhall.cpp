#include "EntityManager.h"
#include "Townhall.h"
#include "Scene.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Gui.h"
#include "Brofiler\Brofiler.h"


Townhall::Townhall()
{
}

Townhall::~Townhall()
{
}

Townhall::Townhall(bool isPlayer1, pair<int,int> pos): Building(Entity::entityType::TOWNHALL,isPlayer1,pos)
{
}

bool Townhall::Update(float dt)
{
	BROFILER_CATEGORY("Townhall Update", Profiler::Color::SandyBrown);
	health = App->player1->live;

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
			//put dt to 0
			//App->render->Blit(App->gui->allied_win_tex, 0, 0, NULL, SDL_FLIP_NONE, 0.0f);
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
