#include "EntityManager.h"
#include "Audio.h"
#include "Townhall.h"
#include "Scene.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Transitions.h"
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
		
		if (level == 0 && App->scenechange->IsChanging() == false)
		{
			SDL_Rect upgrade;
			upgrade.x = 0;
			upgrade.y = 34;
			upgrade.w = 32;
			upgrade.h = 20;
			App->render->Blit(App->scene->upgrade_lvl, position.first - 20, position.second - 70, &upgrade);
		}

		if (level == 1 && App->scenechange->IsChanging() == false)
		{
			SDL_Rect upgrade;
			upgrade.x = 36;
			upgrade.y = 17;
			upgrade.w = 32;
			upgrade.h = 37;
			App->render->Blit(App->scene->upgrade_lvl, position.first - 20, position.second - 70, &upgrade);
		}

		if (level == 2 && App->scenechange->IsChanging() == false)
		{
			SDL_Rect upgrade;
			upgrade.x = 72;
			upgrade.y = 0;
			upgrade.w = 32;
			upgrade.h = 54;
			App->render->Blit(App->scene->upgrade_lvl, position.first - 20, position.second - 70, &upgrade);
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

	}
	else
	{
		App->player2->health = health; //update health bar ui
		App->player2->max_health = health_lv[level];

		if (level == 0 && App->scenechange->IsChanging() == false)
		{
			SDL_Rect upgrade;
			upgrade.x = 0;
			upgrade.y = 34;
			upgrade.w = 32;
			upgrade.h = 20;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 180, position.second - 10, &upgrade);
		}

		if (level == 1 && App->scenechange->IsChanging() == false)
		{
			SDL_Rect upgrade;
			upgrade.x = 36;
			upgrade.y = 17;
			upgrade.w = 32;
			upgrade.h = 37;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 180, position.second - 10, &upgrade);
		}

		if (level == 2 && App->scenechange->IsChanging() == false)
		{
			SDL_Rect upgrade;
			upgrade.x = 72;
			upgrade.y = 0;
			upgrade.w = 32;
			upgrade.h = 54;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 180, position.second - 10, &upgrade);
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
