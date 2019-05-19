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
		
		if (level == 1 && App->scenechange->IsChanging() == false)
		{
			SDL_Rect upgrade;
			upgrade.x = 0;
			upgrade.y = 34;
			upgrade.w = 32;
			upgrade.h = 20;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 200, position.second + 100, &upgrade);
		}

		if (level == 2 && App->scenechange->IsChanging() == false)
		{
			SDL_Rect upgrade;
			upgrade.x = 36;
			upgrade.y = 17;
			upgrade.w = 32;
			upgrade.h = 37;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 200, position.second + 100, &upgrade);
		}

		if (health <= 3000 && App->audio->song1played == false)
		{
			App->audio->song1played = true;
			App->audio->fading_out = true;
			App->audio->track = App->audio->folder_music + "/MiddleSong.ogg";
			App->audio->volume_before = Mix_VolumeMusic(-1);
		}

		else if (health <= 2000 && App->audio->song2played == false)
		{
			App->audio->song2played = true;
			App->audio->fading_out = true;
			App->audio->track = App->audio->folder_music + "/FinalSong.ogg";
			App->audio->volume_before = Mix_VolumeMusic(-1);
		}
		if (App->audio->fading_out)
			App->audio->FadeOut(App->audio->track.c_str());
		if (App->audio->fading_in)
			App->audio->FadeIn();

		if (health <= 0) //if destroyed
		{

			//App->player1->UpdateWalkabilityMap(true, collider);
			//App->player1->DeleteEntity(this);
			App->scene->Victorious(App->player1, dt);
		}
		else
		{
			if (upgrade == true && level <= 1) //upgrade
			{
				App->player1->gold -= Upgrade_Cost; //pay costs
				level++;
				production = production_lv[level]; //update production
				Upgrade_Cost = cost_upgrade_lv[level];
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

		if (level == 1 && App->scenechange->IsChanging() == false)
		{
			SDL_Rect upgrade;
			upgrade.x = 0;
			upgrade.y = 34;
			upgrade.w = 32;
			upgrade.h = 20;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 50, position.second +60, &upgrade);
		}

		if (level == 2 && App->scenechange->IsChanging() == false)
		{
			SDL_Rect upgrade;
			upgrade.x = 36;
			upgrade.y = 17;
			upgrade.w = 32;
			upgrade.h = 37;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 50, position.second + 60, &upgrade);
		}
		if (health < 2000 && App->audio->song1played == false)
		{
			App->audio->song1played = true;
			App->audio->fading_out = true;
			App->audio->track = App->audio->folder_music + "/MiddleSong.ogg";
			App->audio->volume_before = Mix_VolumeMusic(-1);
		}

		else if (health < 1000 && App->audio->song2played == false)
		{
			App->audio->song2played = true;
			App->audio->fading_out = true;
			App->audio->track = App->audio->folder_music + "/FinalSong.ogg";
			App->audio->volume_before = Mix_VolumeMusic(-1);
		}
		if (App->audio->fading_out)
			App->audio->FadeOut(App->audio->track.c_str());
		if (App->audio->fading_in)
			App->audio->FadeIn();
		
		if (health <= 0) //if destroyed
		{

			//App->player2->UpdateWalkabilityMap(true, collider);
			//App->player2->DeleteEntity(this);
			App->scene->Victorious(App->player2, dt);

		}
		else
		{
			if (upgrade == true && level <= 1) //upgrade
			{
				App->player1->gold -= Upgrade_Cost; //pay costs
				level++;
				production = production_lv[level]; //update production
				Upgrade_Cost = cost_upgrade_lv[level];
				health = health_lv[level];
				upgrade = false;
				//play fx (upgrade);
			}
		}

	}

	if (App->scene->worldminutes == 10 && App->player2->health < App->player1->health)
	{
		App->scene->Victorious(App->player2, dt);
	}
	else if (App->scene->worldminutes == 10 && App->player2->health > App->player1->health)
	{
		App->scene->Victorious(App->player1, dt);
	}
	else if (App->scene->worldminutes == 10 && App->player2->health == App->player1->health)
	{
		//Draw (empate)
		App->scene->MatchDraw();
	}

	if (fromPlayer1)
	{
		if (App->player1->currentUI == Player::CURRENT_UI::CURR_SELECTING_BUILDING && App->player1->GetSelectedBuilding() == this)
		{
			Current_Animation = glow;
		}
		else
		{
			Current_Animation = building;
		}
	}
	else
	{
		if (App->player2->currentUI == Player::CURRENT_UI::CURR_SELECTING_BUILDING && App->player2->GetSelectedBuilding() == this)
		{
			Current_Animation = glow;
		}
		else
		{
			Current_Animation = building;
		}
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
		glow = glow->LoadAnimation(path.data(), "allied_glow"); 
	}
	else
	{
		building = building->LoadAnimation(path.data(), "allied");
		glow = glow->LoadAnimation(path.data(), "soviet_glow");
	}

	building->speed = 10;
	Current_Animation = building;
}
