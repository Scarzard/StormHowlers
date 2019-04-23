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

	//Managed in entity.h constructor
	//rate_of_fire = 1;
	//fromPlayer1 = isPlayer1;
	//type = Entity::entityType::SOLDIER;

}


Soldier::~Soldier()
{
}

bool Soldier::Update(float dt)
{
	
	

	if (alive) {
		
		pair<int, int> map_pos = App->map->WorldToMap(position.first, position.second);
		pair<int, int> map_init_pos = App->map->WorldToMap(init_position.first, init_position.second);
		int time_to_act = 2;
		

		// Defensive
		if (defensive) {

			if (closest != nullptr && closest->health > 0)
			{
				state = SHOOTING;
				// Shoots the closest one if in range
				if (timer.ReadSec() >= rate_of_fire)
				{
					closest->TakeDamage(damage_lv[level]);
					timer.Start();
					App->audio->PlayFx(SOLDIER_ATTACK);
					//LOG("Damage to wall: %i     Wall life:%i", 1, closest.;
				}
			}
			else {

				int time = (timer.ReadSec());
				if (time % time_to_act == 0){
					//LOG("DGKLGSK");
					closest = App->entitymanager->findEntity(map_pos, fromPlayer1, range);
				}

				if (state == MOVING) {
					App->move_manager->Move(info.current_group, dt, init_position);

				}
				else if (closest == nullptr && state == TROOP_IDLE) {

					if (map_init_pos.first != map_pos.first && map_init_pos.second != map_pos.second) {
						// If no enemies in range and not at home, go home
						if (info.current_group->IsGroupLead(this)) {
							//MOVING home
							state = MOVING;
						}
					}
					
				}
				
				

				pair<int, int> pos;
				App->input->GetMousePosition(pos.first, pos.second);
				pos = App->render->ScreenToWorld(pos.first, pos.second);

				if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN) {
					position = pos;
				}

			}

			//if (state == MOVING || state == SHOOTING)
			//{
			//	
			//	// Unsuccessful search
			//	// Continues IDLE
			//	if (state = SHOOTING) {
			//		if (init_position.first != position.first && init_position.second != position.second) {
			//			state = ALERT;
			//		}
			//	}

			//}
		}
	}
	else {
		//Current_Animation = Die;

	}
	
	
	
	//
	//if (Speed.first == 0 && Speed.second == 0) {
	//	state == TROOP_IDLE;
	//}
	ActOnDestroyed();
	ChangeAnimation(Speed,closest);

	// ONLY INPUT INSIDE -¬
	ForceAnimations();
	//            _|

	Troop::Update(dt);
	return true;
}

void Soldier::ForceAnimations() {

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN) {
		Current_Animation = moving[(curr++) % SOUTHWEST];

	}
	else if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN) {
		Current_Animation = shooting[(curr++) % SOUTHWEST];

	}
	else if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		Current_Animation = idle;
		idle->SetCurrentFrame((curr++) % SOUTHWEST);
	}
}
void Soldier::ActOnDestroyed() {

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
}

void Soldier::CleanUp() {
	Troop::CleanUp();

}

void Soldier::ChangeAnimation(pair<int, int> Speed, Entity* closest) {
	Current_Animation = idle;
	if (state == MOVING)
	{
		//isShooting = false;
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
	else if (state == SHOOTING && closest != nullptr)
	{
		
		if (fromPlayer1)
		{
			Current_Animation = shooting[SOUTH];
		}
		else
		{
			Current_Animation = shooting[NORTH];
		}

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
		
	

	//}
	}
	
}


void Soldier::LoadAnimations(bool isPlayer1, string path)
{
	moving = vector<Animation*>(TroopDir::MAX_DIR, nullptr);
	shooting = vector<Animation*>(TroopDir::MAX_DIR, nullptr);

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
		moving[i]->speed = 6;
		shooting[i]->speed = 6;
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
