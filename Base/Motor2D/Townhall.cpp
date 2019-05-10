#include "EntityManager.h"
#include "Audio.h"
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
			
	
	if (fromPlayer1)
	{
  

		App->player1->health = health; //update health bar ui
		App->player1->max_health = health_lv[level];
		
		if (level == 0)
		{
			SDL_Rect upgrade;
			upgrade.x = 0;
			upgrade.y = 9;
			upgrade.w = 8;
			upgrade.h = 5;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 10, position.second - 50, &upgrade);
		}

		if (level == 1)
		{
			SDL_Rect upgrade;
			upgrade.x = 12;
			upgrade.y = 6;
			upgrade.w = 8;
			upgrade.h = 9;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 10, position.second - 50, &upgrade);
		}

		if (level == 2)
		{
			SDL_Rect upgrade;
			upgrade.x = 22;
			upgrade.y = 2;
			upgrade.w = 8;
			upgrade.h = 13;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 10, position.second - 50, &upgrade);
		}

		if (health == 0) //if destroyed
		{

			App->player1->UpdateWalkabilityMap(true, collider);
			App->player1->DeleteEntity(this);
			App->audio->PlayFx(BUILDING_EXPLOSION);
			App->map->explosion_anim->speed = 0.5f;
			App->render->Blit(App->scene->explosion_tex, position.first + 25, position.second + 25, &App->map->explosion_anim->GetCurrentFrame(dt));
			App->scene->Victorious(App->player1, dt);
		}
		else
		{
			if (upgrade == true && level <= 1) //upgrade
			{
				App->player1->gold -= upgrade_cost[level]; //pay costs
				level++;
				production = production_lv[level]; //update production
				health = health_lv[level];
				upgrade = false;
				//play fx (upgrade);
			}
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

		if (level == 0)
		{
			SDL_Rect upgrade;
			upgrade.x = 0;
			upgrade.y = 9;
			upgrade.w = 8;
			upgrade.h = 5;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 10, position.second - 50, &upgrade);
		}

		if (level == 1)
		{
			SDL_Rect upgrade;
			upgrade.x = 12;
			upgrade.y = 6;
			upgrade.w = 8;
			upgrade.h = 9;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 10, position.second - 50, &upgrade);
		}

		if (level == 2)
		{
			SDL_Rect upgrade;
			upgrade.x = 22;
			upgrade.y = 2;
			upgrade.w = 8;
			upgrade.h = 13;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 10, position.second - 50, &upgrade);
		}

		if (health == 0) //if destroyed
		{

			App->player2->UpdateWalkabilityMap(true, collider);
			App->player2->DeleteEntity(this);
			App->audio->PlayFx(BUILDING_EXPLOSION);
			App->map->explosion_anim->speed = 0.5f;
			App->render->Blit(App->scene->explosion_tex, position.first + 25, position.second + 25, &App->map->explosion_anim->GetCurrentFrame(dt));
			App->scene->Victorious(App->player2, dt);

		}
		else
		{
			if (upgrade == true && level <= 1) //upgrade
			{
				App->player1->gold -= upgrade_cost[level]; //pay costs
				level++;
				production = production_lv[level]; //update production
				health = health_lv[level];
				upgrade = false;
				//play fx (upgrade);
			}
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
