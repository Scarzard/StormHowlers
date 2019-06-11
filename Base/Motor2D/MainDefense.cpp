#include "EntityManager.h"
#include "MainDefense.h"
#include "Scene.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Audio.h"
#include "Transitions.h"
#include "Brofiler\Brofiler.h"


MainDefense::MainDefense()
{
}

MainDefense::~MainDefense()
{
}

MainDefense::MainDefense(bool isPlayer1, pair<int, int> pos, Collider collider) : Building(Entity::entityType::MAIN_DEFENSE, isPlayer1, pos, collider)
{
	string path = "animation/" + name + ".tmx";
	LoadAnimations(isPlayer1, path.data());
}

bool MainDefense::Update(float dt)
{
	BROFILER_CATEGORY("Main Defense Update", Profiler::Color::SandyBrown);
	
	/*if (fromPlayer1 == true)
	{
		position = App->map->data.tower;
	}
	else
	{
		position = App->map->data.tower2;
	}*/

	//Checks where to look for enemies
	Player* tmpMod = (fromPlayer1) ? App->player2 : App->player1;
	list<Troop*>::iterator tmp = tmpMod->troops.begin();

	// Finds the closest one
	Troop* closest = *tmpMod->troops.begin();
	if (tmp != tmpMod->troops.end()) {
		int min_distance;
		int d = 0;

		// Gets first distance
		Is_inRange(closest->position, min_distance);

		while (tmp != tmpMod->troops.end())
		{
			if ((*tmp)->alive && Is_inRange((*tmp)->position, d) && min_distance >= d) {
				closest = *tmp;
				min_distance = d;
			}
			tmp++;
		}

		// Shoots the closest one if in range
		if (timer.ReadSec() >= rate_of_fire && Is_inRange(closest->position, d))
		{
			 isShooting = true;
			 if (fromPlayer1)
			 {
				 if (!App->player2->inmune)
				 {
					 closest->TakeDamage(damage_lv[level]);

				 }
			 }
			 else if (!fromPlayer1)
			 {

				 if (!App->player1->inmune)
				 {
					 closest->TakeDamage(damage_lv[level]);

				 }
			 }
			timer.Start();
			App->audio->PlayFx(SENTRYGUN_ATTACK);
			//LOG("Distance: %d", d);
		}
	}
	if (timer.ReadSec() >= 3)
	{
		isShooting = false;
		Current_Animation = idle;
	}



	if (fromPlayer1)  // --- Player 1 --------------------------------
	{

		if (level == 1 && App->scenechange->IsChanging() == false)
		{
			SDL_Rect upgrade;
			upgrade.x = 0;
			upgrade.y = 34;
			upgrade.w = 32;
			upgrade.h = 20;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 20, position.second + 20, &upgrade);
		}

		if (level == 2 && App->scenechange->IsChanging() == false)
		{
			SDL_Rect upgrade;
			upgrade.x = 36;
			upgrade.y = 17;
			upgrade.w = 32;
			upgrade.h = 37;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 20, position.second + 20, &upgrade);
		}

		if (upgrade == true && level <= 1) //upgrade
		{
			App->player1->gold -= Upgrade_Cost; //pay costs
			level++;
			damage = damage_lv[level];
			Upgrade_Cost = cost_upgrade_lv[level];
			health = health_lv[level];
			upgrade = false;
			//play fx (upgrade);
		}

		if (building->Finished() && built == false)
		{
			App->audio->PlayFx(ALLIED_SENTRY_B);
			built = true;
		}

		if (health <= 0) //destroyed
		{
			App->player1->DeleteEntity(this);
			App->audio->PlayFx(BUILDING_EXPLOSION);
			App->render->Blit(App->scene->explosion_tex, position.first + 25, position.second + 25, &App->map->explosion_anim->GetCurrentFrame(dt));
			App->audio->PlayFx(ALLIED_SENTRY_D);
			App->player1->SentruGunsCreated--;
		}
	}
	else if (!fromPlayer1) // --- Player 2 ---------------------------
	{

		if (level == 1 && App->scenechange->IsChanging() == false)
		{
			SDL_Rect upgrade;
			upgrade.x = 0;
			upgrade.y = 34;
			upgrade.w = 32;
			upgrade.h = 20;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 20, position.second + 20, &upgrade);
		}

		if (level == 2 && App->scenechange->IsChanging() == false)
		{
			SDL_Rect upgrade;
			upgrade.x = 36;
			upgrade.y = 17;
			upgrade.w = 32;
			upgrade.h = 37;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 20, position.second + 20, &upgrade);
		}

		if (upgrade == true && level <= 1) //upgrade
		{
			App->player2->gold -= Upgrade_Cost; //pay costs
			level++;
			damage = damage_lv[level];
			Upgrade_Cost = cost_upgrade_lv[level];
			health = health_lv[level];
			upgrade = false;
			//play fx (upgrade);
		}

		if (building->Finished() && built == false)
		{
			App->audio->PlayFx(SOVIET_SENTRY_B);
			built = true;
		}

		if (health <= 0) //destroyed
		{
			App->player2->DeleteEntity(this);
			App->audio->PlayFx(BUILDING_EXPLOSION);
			App->render->Blit(App->scene->explosion_tex, position.first + 25, position.second + 25, &App->map->explosion_anim->GetCurrentFrame(dt));
			App->audio->PlayFx(SOVIET_SENTRY_D);
			App->player2->SentruGunsCreated--;
		}
	}

	ChangeAnimation(closest);

	if (fromPlayer1)
	{
		if (App->player1->currentUI == Player::CURRENT_UI::CURR_SELECTING_BUILDING && App->player1->GetSelectedBuilding() == this)
		{
			if (building->Finished())
				Current_Animation = glow;
		}
		else
		{
			if (building->Finished())
				Current_Animation = idle;
		}
	}
	else
	{
		if (App->player2->currentUI == Player::CURRENT_UI::CURR_SELECTING_BUILDING && App->player2->GetSelectedBuilding() == this)
		{
			if (building->Finished())
				Current_Animation = glow;
		}
		else
		{
			if (building->Finished())
				Current_Animation = idle;
		}
	}


	Building::Update(dt);


	return true;
}

void MainDefense::CleanUp()
{
	LOG("---Main Defense Deleted");
	Building::CleanUp();

}

void MainDefense::LoadAnimations(bool isPlayer1, string path)
{
	shooting = vector<Animation*>(TroopDir::MAX_DIR, nullptr);

	shooting[NORTH] = shooting[NORTH]->LoadAnimation(path.data(), (isPlayer1) ? "red_N" : "blue_N");
	shooting[SOUTH] = shooting[SOUTH]->LoadAnimation(path.data(), (isPlayer1) ? "red_S" : "blue_S");
	shooting[EAST] = shooting[EAST]->LoadAnimation(path.data(), (isPlayer1) ? "red_E" : "blue_E");
	shooting[WEST] = shooting[WEST]->LoadAnimation(path.data(), (isPlayer1) ? "red_W" : "blue_W");
	shooting[NORTHEAST] = shooting[NORTHEAST]->LoadAnimation(path.data(), (isPlayer1) ? "red_NE" : "blue_NE");
	shooting[NORTHWEST] = shooting[NORTHWEST]->LoadAnimation(path.data(), (isPlayer1) ? "red_NW" : "blue_NW");
	shooting[SOUTHEAST] = shooting[SOUTHEAST]->LoadAnimation(path.data(), (isPlayer1) ? "red_SE" : "bluet_SE");
	shooting[SOUTHWEST] = shooting[SOUTHWEST]->LoadAnimation(path.data(), (isPlayer1) ? "red_SW" : "blue_SW");

	for (int i = NORTH; i <= SOUTHWEST; i++) {
		//moving[i]->speed = 3;
		shooting[i]->speed = 3;
	}

	
	if (fromPlayer1)
	{
		idle = shooting[SOUTHWEST];
	}
	else
	{
		idle = shooting[NORTHEAST];
	}

	building = building->LoadAnimation(path.data(), (isPlayer1) ? "red_constructing" : "blue_constructing");
	level1 = level1->LoadAnimation(path.data(), (isPlayer1) ? "red_NE" : "blue_SW");
	glow = glow->LoadAnimation(path.data(), (isPlayer1) ? "red_glow" : "blue_glow");


	idle->speed = 0;
	level1->speed = 10;
	building->speed = 10;
	glow->speed = 0; 

	idle->loop = false;
	building->loop = false;
	level1->loop = false;
	glow->loop = false;
	Current_Animation = building;
}

bool MainDefense::Is_inRange(pair<int, int> pos, int &distance) {

	pair <int, int> vector_distance = { position.first - pos.first, position.second - pos.second };
	distance = (int)(sqrt(pow(vector_distance.first, 2) + pow(vector_distance.second, 2)));

	return distance <= range;
}

void MainDefense::ChangeAnimation( Entity* closest) {

		
		if (isShooting)
		{
			if (closest != nullptr)
			{

				if (closest->position == position)
				{
					if (fromPlayer1)
					{
						Current_Animation = shooting[SOUTH];
					}
					else
					{
						Current_Animation = shooting[NORTH];
					}
				}
				else if (closest->position.second <= position.second && closest->position.first >= position.first)
				{
					//noth
					Current_Animation = shooting[NORTH];
					if (closest->position.second == position.second)
					{
						//northwest
						Current_Animation = shooting[NORTHWEST];
					}
					//else if (closest->position.second > position.second)
					//{
					//	//north
					//	Current_Animation = shooting[NORTH];
					//}
					else if (closest->position.first == position.first)
					{
						//northeast
						Current_Animation = shooting[NORTHEAST];
					}
				}
				else if (closest->position.first >= position.first && closest->position.second >= position.second)
				{
					//south
					Current_Animation = shooting[SOUTH];
					if (closest->position.second == position.second)
					{
						//southwest
						Current_Animation = shooting[SOUTHWEST];
					}
					//else if (closest->position.second > position.second)
					//{
					//	//north
					//	Current_Animation = shooting[NORTH];
					//}
					else if (closest->position.first == position.first)
					{
						//southeast
						Current_Animation = shooting[SOUTHEAST];
					}
				}
				else if (closest->position.second > position.second && closest->position.first > position.first)
				{
					//east
					Current_Animation = shooting[EAST];

				}
				else if (closest->position.second < position.second && closest->position.first < position.first)
				{
					//west
					Current_Animation = shooting[WEST];

				}
			}

			else
			{
				if (fromPlayer1)
				{
					Current_Animation = shooting[SOUTH];
				}
				else
				{
					Current_Animation = shooting[NORTH];
				}
			}


		}
	}

