#include "Soldier.h"
#include "Pathfinding.h"
#include "Player.h"
#include "Audio.h"


Soldier::Soldier()
{
	
}

Soldier::Soldier(bool isPlayer1, pair<int, int> pos, Collider collider):Troop(Entity::entityType::SOLDIER,isPlayer1,pos, collider)
{
	string path = "animation/" + name + ".tmx";
	LoadAnimations(isPlayer1, path.data());
	rate_of_fire = 1;
	fromPlayer1 = isPlayer1;
	type = Entity::entityType::SOLDIER;

}


Soldier::~Soldier()
{
}

bool Soldier::Update(float dt)
{
	if (!alive) return true;

	pair<int, int> pos;
	pair<int, int> map_pos = App->map->WorldToMap(position.first, position.second);
	Entity* closest = nullptr;
	
	if (isMoving == false)
	{
		closest = App->entitymanager->findEntity(map_pos, fromPlayer1, range);
		if (closest != nullptr && closest->health >0)
		{
			// Shoots the closest one if in range
			if (timer.ReadSec() >= rate_of_fire )
			{
				isShooting = true;
				closest->TakeDamage(damage_lv[level]);
				timer.Start(); 
				App->audio->PlayFx(SOLDIER_ATTACK);
				//LOG("Damage to wall: %i     Wall life:%i", 1, closest.;
			}
		}
		if (closest != nullptr && closest->health <= 0)
		{
			isShooting = false;
		}

	}
	if (timer.ReadSec() >= 3)
	{
		isShooting = false;
	}

	if (fromPlayer1)  // --- Player 1 --------------------------------
	{
		if (health <= 0) //destroyed
		{
			App->player1->DeleteEntity(this);
		}
	}
	else if (!fromPlayer1) // --- Player 2 ---------------------------
	{
		if (health <= 0) //destroyed
		{
			App->player2->DeleteEntity(this);
		}
	}

	// OLD PATHFINDING WITHOUT GROUP MOVEMENT
	if (false){

		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) {

			LOG("Soldier pathfinding player %d", (fromPlayer1) ? 1 : 2);

			//Getting positions
			App->input->GetMousePosition(pos.first, pos.second);
			pos = App->render->ScreenToWorld(pos.first, pos.second);
			pos = App->map->WorldToMap(pos.first, pos.second);
			//pos.first--;
			LOG("mouseInMap: [%d,%d]", pos.first, pos.second);
			LOG("soldierInMap: [%d,%d]", map_pos.first, map_pos.second);


			//Creating path, reset path index counter
			if (App->pathfinding->CreatePath(map_pos, pos, false) == -1) {

				LOG("Pathfinding soldier: origin or destination not walkable");
			}
			else {
				path.clear();
				path_count = 1;
				path = *App->pathfinding->GetLastPath();

			}

		}


		if (path.size() > 1) {
			isMoving = true;
			Speed = { path.at(path_count).first - map_pos.first, path.at(path_count).second - map_pos.second };
			Speed.first = Speed.first * speed;
			Speed.second = Speed.second * speed;
			//LOG("Speed: [%d,%d]", Speed.first, Speed.second);

			// If it's still, change to next path tile
			if (Speed.first == 0 && Speed.second == 0) {
				if (path.size() > path_count + 1)
					path_count++;
				else
					isMoving = false;
			}

			//Visual path debug
			pair<int, int> tmppair;
			for (int i = 0; i < path.size(); i++) {
				tmppair = App->map->MapToWorld(path.at(i).first, path.at(i).second);
				App->render->Blit(tex, tmppair.first, tmppair.second, &rect);
			}
		}



		position.first += (int)(Speed.first * dt);
		position.second += (int)(Speed.second * dt);
		//App->render->Blit(tex, position.first, position.second, &rect);

	}
	
	/*if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN) {
		Current_Animation =  moving[(curr++)%SOUTHWEST];
		
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN) {
		Current_Animation = shooting[(curr++) % SOUTHWEST];

	}
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		Current_Animation = idle;
		idle->SetCurrentFrame((curr++) % SOUTHWEST);

	}*/
	
	ChangeAnimation(Speed,closest);

	Troop::Update(dt);
	return true;
}

void Soldier::CleanUp() {
	Troop::CleanUp();

}

void Soldier::ChangeAnimation(pair<int, int> Speed, Entity* closest) {
	Current_Animation = idle;
	if (isMoving) 
	{
		isShooting = false;
		if (Speed.first == 0 && Speed.second < 0)
		{
			//north
			Current_Animation = moving[NORTH];
		}
		else if (Speed.first == 0 && Speed.second > 0)
		{
			//south
			Current_Animation = moving[SOUTH];
		}
		else if (Speed.first < 0 && Speed.second == 0)
		{
			//west
			Current_Animation = moving[WEST];
		}
		else if (Speed.first > 0 && Speed.second == 0)
		{
			//east
			Current_Animation = moving[EAST];
		}
		else if (Speed.first > 0 && Speed.second < 0)
		{
			//north east
			Current_Animation = moving[NORTHEAST];
		}
		else if (Speed.first > 0 && Speed.second > 0)
		{
			//south east
			Current_Animation = moving[SOUTHEAST];
		}
		else if (Speed.first < 0 && Speed.second < 0)
		{
			//north west
			Current_Animation = moving[NORTHWEST];
		}
		else if (Speed.first < 0 && Speed.second > 0)
		{
			//south wst
			Current_Animation = moving[SOUTHWEST];
		}
	}
	else if (!isMoving)
	{
		
			Current_Animation = idle;
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
						else if (closest->position.first==position.first)
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
	
}

void Soldier::ChangeAnimation() {
	Current_Animation = idle;
	if (isMoving)
	{
		Current_Animation = moving[curr];
		
	}
	else if (isShooting) {
		Current_Animation = shooting[curr];
	}
}

void Soldier::LoadAnimations(bool isPlayer1, string path)
{
	moving = vector<Animation*>(entityDir::MAX, nullptr);
	shooting = vector<Animation*>(entityDir::MAX, nullptr);

	idle = idle->LoadAnimation(path.data(), (isPlayer1) ? "red_idle" : "blue_idle");

	moving[NORTH] = moving[NORTH]->LoadAnimation(path.data(), (isPlayer1) ? "red_north" : "blue_north");
	moving[SOUTH] = moving[SOUTH]->LoadAnimation(path.data(), (isPlayer1) ? "red_south" : "blue_south");
	moving[EAST] = moving[EAST]->LoadAnimation(path.data(), (isPlayer1) ? "red_east" : "blue_east");
	moving[WEST] = moving[WEST]->LoadAnimation(path.data(), (isPlayer1) ? "red_west" : "blue_west");
	moving[NORTHEAST] = moving[NORTHEAST]->LoadAnimation(path.data(), (isPlayer1) ? "red_northeast" : "blue_northeast");
	moving[NORTHWEST] = moving[NORTHWEST]->LoadAnimation(path.data(), (isPlayer1) ? "red_northwest" : "blue_northwest");
	moving[SOUTHEAST] = moving[SOUTHEAST]->LoadAnimation(path.data(), (isPlayer1) ? "red_southeast" : "blue_southeast");
	moving[SOUTHWEST] = moving[SOUTHWEST]->LoadAnimation(path.data(), (isPlayer1) ? "red_southwest" : "blue_southwest");

	shooting[NORTH] = shooting[NORTH]->LoadAnimation(path.data(),		  (isPlayer1) ? "red_shoot_N" : "blue_shoot_N");
	shooting[SOUTH] = shooting[SOUTH]->LoadAnimation(path.data(),		  (isPlayer1) ? "red_shoot_S" : "blue_shoot_S");
	shooting[EAST] = shooting[EAST]->LoadAnimation(path.data(),			  (isPlayer1) ? "red_shoot_E" : "blue_shoot_E");
	shooting[WEST] = shooting[WEST]->LoadAnimation(path.data(),			  (isPlayer1) ? "red_shoot_W" : "blue_shoot_W");
	shooting[NORTHEAST] = shooting[NORTHEAST]->LoadAnimation(path.data(), (isPlayer1) ? "red_shoot_NE" : "blue_shoot_NE");
	shooting[NORTHWEST] = shooting[NORTHWEST]->LoadAnimation(path.data(), (isPlayer1) ? "red_shoot_NW" : "blue_shoot_NW");
	shooting[SOUTHEAST] = shooting[SOUTHEAST]->LoadAnimation(path.data(), (isPlayer1) ? "red_shoot_SE" : "blue_shoot_SE");
	shooting[SOUTHWEST] = shooting[SOUTHWEST]->LoadAnimation(path.data(), (isPlayer1) ? "red_shoot_SW" : "blue_shoot_SW");

	for (int i = NORTH; i <= SOUTHWEST; i++) {
		moving[i]->speed = 3;
		shooting[i]->speed = 3;
	}

	idle->speed = 0;
	if (fromPlayer1)
	{
		idle->SetCurrentFrame(2);
	}
	else
	{
		idle->SetCurrentFrame(6);
	}
	Current_Animation = moving[NORTH];
}
