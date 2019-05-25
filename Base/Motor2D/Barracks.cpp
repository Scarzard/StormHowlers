#include "EntityManager.h"
#include "Barracks.h"
#include "Audio.h"
#include "Scene.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Transitions.h"
#include "Brofiler\Brofiler.h"


Barracks::Barracks(){}
Barracks::~Barracks(){}

Barracks::Barracks(bool isPlayer1, pair<int, int> pos, Collider collider) : Building(Entity::entityType::BARRACKS, isPlayer1, pos, collider)
{
	string path = "animation/" + name + ".tmx";
	LoadAnimations(isPlayer1, path.data());

	myPlayer = (isPlayer1)?App->player1:App->player2;
	
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

		if (level == 1 && App->scenechange->IsChanging() == false) //Poner abajo a la derecha
		{
			SDL_Rect upgrade;
			upgrade.x = 0;
			upgrade.y = 34;
			upgrade.w = 32;
			upgrade.h = 20;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 25, position.second + 50, &upgrade);
		}

		if (level == 2 && App->scenechange->IsChanging() == false)
		{
			SDL_Rect upgrade;
			upgrade.x = 36;
			upgrade.y = 17;
			upgrade.w = 32;
			upgrade.h = 37;
			App->render->Blit(App->scene->upgrade_lvl, position.first + 25 , position.second + 50, &upgrade);
		}
		
		if (building->Finished() && built == false)
		{
			App->audio->PlayFx(ALLIED_BARRACKS_B);
			built = true;
		}
		

		if (health > 0) //if not destroyed
		{
			if (upgrade == true && level <= 1) //upgrade
			{
				App->player1->gold -= Upgrade_Cost; //pay costs
				level++;
				capacity = capacity_lv[level]; //update capacity
				Upgrade_Cost = cost_upgrade_lv[level];
				health = health_lv[level];
				upgrade = false;
				//play fx (upgrade);
			}

			if (TroopsCreated.empty() == false && is_deploying == false)
			{
				is_deploying = true;
				deploy_state = DeployState::START;

			}
		}
		else //destroyed
		{
			App->player1->DeleteEntity(this);
			App->audio->PlayFx(BUILDING_EXPLOSION);
			App->render->Blit(App->scene->explosion_tex, position.first + 25, position.second + 25, &App->map->explosion_anim->GetCurrentFrame(dt));
			App->player1->BarracksCreated -= 1;
			App->audio->PlayFx(ALLIED_BARRACKS_D);
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

			if (level == 1 && App->scenechange->IsChanging() == false)
			{
				SDL_Rect upgrade;
				upgrade.x = 0;
				upgrade.y = 34;
				upgrade.w = 32;
				upgrade.h = 20;
				App->render->Blit(App->scene->upgrade_lvl, position.first + 25, position.second + 50, &upgrade);
			}

			if (level == 2 && App->scenechange->IsChanging() == false)
			{
				SDL_Rect upgrade;
				upgrade.x = 36;
				upgrade.y = 17;
				upgrade.w = 32;
				upgrade.h = 37;
				App->render->Blit(App->scene->upgrade_lvl, position.first + 25, position.second + 50, &upgrade);
			}


			if (upgrade == true && level <= 1) //upgrade
			{
				App->player2->gold -= Upgrade_Cost; //pay costs
				level++;
				capacity = capacity_lv[level]; //update capacity
				Upgrade_Cost = cost_upgrade_lv[level];
				health = health_lv[level];
				upgrade = false;
				//play fx (upgrade);
			}


			if (building->Finished() && built == false)
			{
				App->audio->PlayFx(SOVIET_BARRACKS_B);
				built = true;
			}

			if (TroopsCreated.empty() == false && is_deploying == false)
			{
				is_deploying = true;
				deploy_state = DeployState::START;
			}
		}
		else //destroyed
		{
			App->player2->DeleteEntity(this);
			App->audio->PlayFx(BUILDING_EXPLOSION);
			App->map->explosion_anim->speed = 0.5f;

			App->render->Blit(App->scene->explosion_tex, position.first, position.second, &App->map->explosion_anim->GetCurrentFrame(dt));
			App->player2->BarracksCreated -= 1; 
			App->audio->PlayFx(SOVIET_BARRACKS_D);
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

	if (fromPlayer1)
	{
		if (App->player1->currentUI == Player::CURRENT_UI::CURR_CREATE_TROOPS && App->player1->GetSelectedBuilding() == this)
		{
			if (building->Finished())
				Current_Animation = glow;
		}
		else
		{
			if (building->Finished())
				Current_Animation = level1; 
		}
	}
	else
	{
		if (App->player2->currentUI == Player::CURRENT_UI::CURR_CREATE_TROOPS && App->player2->GetSelectedBuilding() == this)
		{
			if (building->Finished())
				Current_Animation = glow;
		}
		else
		{
			if (building->Finished())
				Current_Animation = level1;
		}
	}

	if (is_deploying) {

		is_deploying = DeployTroops();
	}

	Building::Update(dt);

	return true;
}

bool Barracks::DeployTroops(int amount_per_frame)
{
	bool ret = true;
	if (amount_per_frame <= 0) {
		amount_per_frame = MAX_DEPLOY_SIZE;
	}

	switch (deploy_state)
	{
		case DeployState::START:

			//deploy_counter = 0;
			//GROUP MANAGEMENT
			tmp_entity = myPlayer->troops.begin();
			while (tmp_entity != myPlayer->troops.end())
			{
				(*tmp_entity)->isSelected = false;
				tmp_entity++;
			}
			
			deploy_pos = position;
			deploy_state = DeployState::DEPLOYING;

			break;

		case DeployState::DEPLOYING:
			if (TroopsCreated.empty()) {
				deploy_state = DeployState::END;
			}
			else {
				//collider.dimensions = { 1,1 };
				
				for (int i = 0; i < amount_per_frame && !TroopsCreated.empty(); i++) {

					Troop* e;
					deploy_pos.first += 20;
					deploy_pos.second += 10;
					e = (Troop*)App->entitymanager->AddEntity(fromPlayer1, *TroopsCreated.begin(), deploy_pos, collider);
					if (*TroopsCreated.begin() == e->SOLDIER && fromPlayer1)
					{
						App->audio->PlayFx(ALLIED_SOLDIER_SPAWN);
					}
					else if (*TroopsCreated.begin() == e->SOLDIER && !fromPlayer1)
					{
						App->audio->PlayFx(SOVIET_SOLDIER_SPAWN);
					}
					if (*TroopsCreated.begin() == e->ENGINEER)
					{
						App->audio->PlayFx(ENG_SPAWN);
					}
					if (*TroopsCreated.begin() == e->WAR_HOUND)
					{
						App->audio->PlayFx(WARHOUND_ATTACK);
					}
					TroopsCreated.pop_front();
					e->state = TROOP_IDLE;
					e->isSelected = true;
					//deploying_counter++;
				}
				deploy_pos.first = position.first;
				//deploy_pos.first -= 10;
				//deploy_pos.second += 20;
			}
			break;
		case DeployState::END:
			deploy_state = DeployState::NONE;
			ret = false;
			//deploy_counter = 0;
			//isDeploying = false;
			myPlayer->groups.push_back(App->move_manager->CreateGroup(myPlayer));
			myPlayer->group++;
			break;

		case DeployState::NONE:
			break;
		default:
			LOG("REACHED WRONG DEPLOY STATE");
			break;
	}
	return ret;
	
}

void Barracks::LoadAnimations(bool isPlayer1, string path) {
	level1 = level1->LoadAnimation(&path[0], (isPlayer1) ? "blue_idle" : "red_idle");
	building = building->LoadAnimation(&path[0], (isPlayer1) ? "blue_constructing" : "red_constructing");
	glow = glow->LoadAnimation(&path[0], (isPlayer1) ? "blue_glow" : "red_glow");

	level1->speed = 5;
	building->speed = 8;
	glow->speed = 0; 

	building->loop = false;
	level1->loop = true;
	glow->loop = true;

	Current_Animation = building;
};


void Barracks::CleanUp()
{
	LOG("---Barracks Deleted");

	Building::CleanUp();

}
