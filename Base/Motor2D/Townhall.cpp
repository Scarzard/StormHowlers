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
	if (isPlayer1 == true)
	{
		position = App->map->data.main_building2;
	}
	else if (isPlayer1 == false)
	{
		position = App->map->data.main_building;
	}
	else
	{
		position = pos;
	}

	
	Current_Animation = building;

	health = health_lv[level];
}

bool Townhall::Update(float dt)
{
	BROFILER_CATEGORY("Townhall Update", Profiler::Color::SandyBrown);
	

	// ------ This is awful ------ but works ;D

	//if (fromPlayer1 == true)
	//{
	//	position = App->map->data.main_building2;
	//}
	//else 
	//{
	//	position = App->map->data.main_building;
	//}
	//////---------------------------------------

			
	
	if (fromPlayer1)
	{
  // MERGE DEVELOPMENT
		//if (App->player1->live > 0) //if not destroyed

		App->player1->health = health; //update health bar ui
		App->player1->max_health = health_lv[level];
		
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
/*
			App->player1->UpdateWalkabilityMap(true, collider);
			App->player1->DeleteEntity(this);*/
			App->scene->Victorious(App->player1, dt);
		}

		// Just to test the LiveBar
		if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		{
			health -= 100;

			if (health < 0)
				health = 0;
		}

	}
	else
	{
		App->player2->health = health; //update health bar ui
		App->player2->max_health = health_lv[level];

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

			/*App->player2->UpdateWalkabilityMap(true, collider);
			App->player2->DeleteEntity(this);*/
			App->scene->Victorious(App->player2, dt);

		}

		// Just to test the LiveBar
		if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
		{
			health -= 100;

			if (health < 0)
				health = 0;
		}

	}

	if (App->scene->worldminutes == 10 && App->player2->health < App->player1->health)
	{
		App->scene->Victorious(App->player1, dt);
	}
	else if (App->scene->worldminutes == 10 && App->player2->health > App->player1->health)
	{
		App->scene->Victorious(App->player2, dt);
	}
	else if (App->scene->worldminutes == 10 && App->player2->health == App->player1->health)
	{
		//Draw (empate)
		App->scene->MatchDraw();
	}

	Building::Update(dt);

	return true;
}

void Townhall::CleanUp()
{
	LOG("---Townhall Deleted");

	//Building::CleanUp();


}

void Townhall::LoadAnimations(bool isPlayer1, string path)
{
	if (isPlayer1)
	{
		building = building->LoadAnimation(path.data(), "soviet");
	}
	else
	{
		building = building->LoadAnimation(path.data(), "allied");
	}

	building->speed = 10;
	Current_Animation = building;
}
