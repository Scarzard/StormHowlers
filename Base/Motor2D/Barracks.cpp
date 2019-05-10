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

	if (fromPlayer1)
		App->player1->total_capacity += 10;
	else
		App->player2->total_capacity += 10;
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
		App->render->Blit(App->scene->upgrade_lvl_1, 100, 120);
		if (level == 1)
		{



		}

		if (health > 0) //if not destroyed
		{
			if (upgrade == true && level <= 3) //upgrade
			{
				App->player1->gold -= upgrade_cost[level]; //pay costs
				level++;
				capacity = capacity_lv[level]; //update capacity
				upgrade = false;
				//play fx (upgrade);
			}
		}
		else //destroyed
		{
			App->player1->total_capacity -= 10;
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
		if (health > 0) //if not destroyed
		{
			if (upgrade == true && level <= 3) //upgrade
			{
				App->player2->gold -= upgrade_cost[level]; //pay costs
				level++;
				capacity = capacity_lv[level]; //update capacity
				upgrade = false;
				//play fx (upgrade);
			}
		}
		else //destroyed
		{

			App->player2->UpdateWalkabilityMap(true, collider);
			App->player2->DeleteEntity(this);
			App->audio->PlayFx(BUILDING_EXPLOSION);
			App->map->explosion_anim->speed = 0.5f;
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

	if (Current_Animation->Finished() == true)
		Current_Animation = level1;

	Building::Update(dt);

	return true;
}

void Barracks::LoadAnimations(bool isPlayer1, string path) {
	level1 = level1->LoadAnimation(&path[0], (isPlayer1) ? "red_idle" : "blue_idle");
	building = building->LoadAnimation(&path[0], (isPlayer1) ? "red_constructing" : "blue_constructing");

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
