#include "EntityManager.h"
#include "Barracks.h"
#include "Audio.h"
#include "Scene.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Brofiler\Brofiler.h"


Barracks::Barracks(){}
Barracks::~Barracks(){}

Barracks::Barracks(bool isPlayer1, pair<int, int> pos, Collider collider) : Building(Entity::entityType::BARRACKS, isPlayer1, pos, collider)
{
	string path = "animation/" + name + ".tmx";
	LoadAnimations(isPlayer1, path.data());

	
}

bool Barracks::PreUpdate()
{
	BROFILER_CATEGORY("Barracks PreUpdate", Profiler::Color::SandyBrown);

	SDL_Rect r = { position.first, position.second,size.first,size.second*0.5 };
	SDL_RenderCopy(App->render->renderer, tex, &debug_collider, &r);

	repair_cost = (health_lv[level] - health) / 2;

	return true;
}

bool Barracks::Update(float dt)
{
	BROFILER_CATEGORY("Barracks Update", Profiler::Color::SandyBrown);

	if (fromPlayer1)  // --- Player 1 --------------------------------
	{
		if (health > 0) //if not destroyed
		{
			if (upgrade == true) //upgrade
			{
				App->player1->gold -= upgrade_cost[level]; //pay costs
				level++;
				capacity = capacity_lv[level]; //update capacity
				upgrade = false;
				//play fx (upgrade);
			}

			if (TroopsCreated.empty() == false)
			{
				if (timer.ReadSec() >= 1)
				{
					pair<int, int> pos;
					pos.first = position.first - 100;
					pos.second = position.second + 60;

					list<Entity::entityType>::iterator first_troop = TroopsCreated.begin();
					App->entitymanager->AddEntity(true, (*first_troop), pos, collider);
					TroopsCreated.pop_front();
					timer.Start();
					
				}
			}
		}
		else //destroyed
		{
			App->player1->DeleteEntity(this);
			App->audio->PlayFx(BUILDING_EXPLOSION);
			App->render->Blit(App->scene->explosion_tex, position.first + 25, position.second + 25, &App->map->explosion_anim->GetCurrentFrame(dt));
			App->player1->BarracksCreated -= 1;
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
		if (health > 0) //if not destroyed
		{
			if (upgrade == true) //upgrade
			{
				App->player2->gold -= upgrade_cost[level]; //pay costs
				level++;
				capacity = capacity_lv[level]; //update capacity
				upgrade = false;
				//play fx (upgrade);
			}

			if (TroopsCreated.empty() == false)
			{
				if (timer.ReadSec() >= 1)
				{
					pair<int, int> pos;
					pos.first = position.first - 100;
					pos.second = position.second + 60;

					list<Entity::entityType>::iterator first_troop = TroopsCreated.begin();
					App->entitymanager->AddEntity(false, (*first_troop), pos, collider);
					TroopsCreated.pop_front();
					timer.Start();

				}
			}
		}
		else //destroyed
		{
			App->player2->DeleteEntity(this);
			App->audio->PlayFx(BUILDING_EXPLOSION);
			App->map->explosion_anim->speed = 0.5f;
			App->render->Blit(App->scene->explosion_tex, position.first, position.second, &App->map->explosion_anim->GetCurrentFrame(dt));
			App->player2->BarracksCreated -= 1; 
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

	if (Current_Animation->Finished() == true)
		Current_Animation = level1;

	Building::Update(dt);

	return true;
}

void Barracks::LoadAnimations(bool isPlayer1, string path) {
	level1 = level1->LoadAnimation(&path[0], (isPlayer1) ? "blue_idle" : "red_idle");
	building = building->LoadAnimation(&path[0], (isPlayer1) ? "blue_constructing" : "red_constructing");

	level1->speed = 5;
	building->speed = 8;

	building->loop = false;
	Current_Animation = building;
};


void Barracks::CleanUp()
{
	LOG("---Barracks Deleted");

	Building::CleanUp();

}
