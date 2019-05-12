#include "EntityManager.h"
#include "Mines.h"
#include "Audio.h"
#include "Scene.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Transitions.h"
#include "Brofiler\Brofiler.h"


Mines::Mines()
{
}

Mines::~Mines()
{
}

Mines::Mines(bool isPlayer1, pair<int, int> pos, Collider collider) : Building(Entity::entityType::MINES, isPlayer1, pos, collider)
{
	string path = "animation/" + name + ".tmx";
	LoadAnimations(isPlayer1, path.data());
	production = production_lv[level];
	
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

			if (level == 0 && App->scenechange->IsChanging() == false)
			{
				SDL_Rect upgrade;
				upgrade.x = 0;
				upgrade.y = 34;
				upgrade.w = 32;
				upgrade.h = 20;
				App->render->Blit(App->scene->upgrade_lvl, position.first, position.second - 140, &upgrade);
			}

			if (level == 1 && App->scenechange->IsChanging() == false)
			{
				SDL_Rect upgrade;
				upgrade.x = 36;
				upgrade.y = 17;
				upgrade.w = 32;
				upgrade.h = 37;
				App->render->Blit(App->scene->upgrade_lvl, position.first + 10, position.second - 140, &upgrade);
			}

			if (level == 2 && App->scenechange->IsChanging() == false)
			{
				SDL_Rect upgrade;
				upgrade.x = 72;
				upgrade.y = 0;
				upgrade.w = 32;
				upgrade.h = 54;
				App->render->Blit(App->scene->upgrade_lvl, position.first + 10, position.second - 140, &upgrade);
			}

			if (health > 0) //if not destroyed
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
			else //destroyed
			{
				App->player1->UpdateWalkabilityMap(true, collider);
				App->player1->DeleteEntity(this);
				App->audio->PlayFx(BUILDING_EXPLOSION);
				App->render->Blit(App->scene->explosion_tex, position.first + 25, position.second + 25, &App->map->explosion_anim->GetCurrentFrame(dt));
			}

			if (repair == true) //repair
			{
				App->player1->gold -= repair_cost;
				health = health_lv[level];
				repair = false;
				App->audio->PlayFx(BUILDING_REPAIR);
				//play fx (repair);
			}
		}
		else if (!fromPlayer1) // --- Player 2 ---------------------------
		{

			if (level == 0 && App->scenechange->IsChanging() == false)
			{
				SDL_Rect upgrade;
				upgrade.x = 0;
				upgrade.y = 34;
				upgrade.w = 32;
				upgrade.h = 20;
				App->render->Blit(App->scene->upgrade_lvl, position.first, position.second - 140, &upgrade);
			}

			if (level == 1 && App->scenechange->IsChanging() == false)
			{
				SDL_Rect upgrade;
				upgrade.x = 36;
				upgrade.y = 17;
				upgrade.w = 32;
				upgrade.h = 37;
				App->render->Blit(App->scene->upgrade_lvl, position.first + 10, position.second - 140, &upgrade);
			}

			if (level == 2 && App->scenechange->IsChanging() == false)
			{
				SDL_Rect upgrade;
				upgrade.x = 72;
				upgrade.y = 0;
				upgrade.w = 32;
				upgrade.h = 54;
				App->render->Blit(App->scene->upgrade_lvl, position.first + 10, position.second - 140, &upgrade);
			}

			if (health > 0) //if not destroyed
			{
				if (upgrade == true && level<=1) //upgrade
				{
					App->player2->gold -= upgrade_cost[level]; //pay costs
					level++;
					production = production_lv[level]; //update production
					health = health_lv[level];
					upgrade = false;
					//play fx (upgrade);
				}
			}
			else //destroyed
			{
				App->player2->UpdateWalkabilityMap(true, collider);
				App->player2->DeleteEntity(this);
				App->audio->PlayFx(BUILDING_EXPLOSION);
				App->render->Blit(App->scene->explosion_tex, position.first + 25, position.second + 25, &App->map->explosion_anim->GetCurrentFrame(dt));
			}

			if (repair == true) //repair
			{
				App->player2->gold -= repair_cost;
				health = health_lv[level];
				repair = false;
				App->audio->PlayFx(BUILDING_REPAIR);
				//play fx (repair);
			}
		}

	ChangeAnimation();

	if(Current_Animation->Finished() == true)
		Current_Animation = level1;

	Building::Update(dt);

	return true;
}

void Mines::CleanUp()
{
	LOG("---Mine Deleted");
	Building::CleanUp();

}

void Mines::LoadAnimations(bool isPlayer1, string path)
{
	building = building->LoadAnimation(path.data(), (isPlayer1) ? "red_constructing" : "blue_constructing");
	level1 = level1->LoadAnimation(path.data(), (isPlayer1) ? "red_idle" : "blue_idle");
	//level1->PushBack(building->GetLastAnimationFrame());// level1->LoadAnimation(&path[0], (!isPlayer1) ? "red" : "blue");
	level1->speed = 3;
	building->speed = 10;
	building->loop = false;
	level1->loop = true;
	Current_Animation = building;
}

