#include "Hound.h"
#include "Pathfinding.h"
#include "Player.h"
#include "Audio.h"


Hound::Hound()
{

}

Hound::Hound(bool isPlayer1, pair<int, int> pos, Collider collider) :Troop(Entity::entityType::WAR_HOUND, isPlayer1, pos, collider)
{
	BROFILER_CATEGORY("Hound constructor", Profiler::Color::Red);
	string path = "animation/" + name + ".tmx";
	LoadAnimations(isPlayer1, path.data());
	
	destination = pos;
	original_range = range;
	//Managed in entity.h constructor
	//rate_of_fire = 1;
	//fromPlayer1 = isPlayer1;
	//type = Entity::entityType::SOLDIER;

}


Hound::~Hound()
{
}

bool Hound::Update(float dt)
{

	if (alive) {

		pair<int, int> map_pos = App->map->WorldToMap(position.first, position.second);
		pair<int, int> map_init_pos = App->map->WorldToMap(init_position.first, init_position.second);
		int time_to_act = 2;

		// Checks for group defined closest entity
		if (info.closest != nullptr)
		{
			//Entity to attack is found
			if (info.closest->health > 0 ) {
				//LOG("Closest FOUND");
				int d = 0;

				// Modify the goal tile to make it walkable
				SetDestination();

				if (!Is_inRange(map_pos, d, destination, original_range)) {
					state = MOVING;
					destination = App->map->MapToWorld(destination.first, destination.second);
					if (info.current_group->IsGroupLead(this)) {

						info.current_group->CheckForMovementRequest(dt, destination);

					}
					else if (info.current_group->GetLead()->state != MOVING) {
						//Lead has reached the destination but the target is still out of range, need to move individually

						info.current_group->CheckForMovementIndividual(this, dt, destination);
					}
					else {
					}
					//LOG("Closest FOUND - NOT in range => MOVING");
				}
				else {
					info.UnitMovementState = MovementState::MovementState_NoState;
					state = SHOOTING;

					// Shoots the closest one if in range
					if (timer.ReadSec() >= rate_of_fire)
					{
						info.closest->TakeDamage(damage_lv[level]);
						timer.Start();
						App->audio->PlayFx(SOLDIER_ATTACK);
					}
					//LOG("Closest FOUND - IN range => SHOOTING");

				}
			}
			else {
				state = TROOP_IDLE;
				info.closest = nullptr;
			}
		}
		//ENTITY TO ATTACK IS NOT FOUND OR JUST DIED
		if (state != SHOOTING) {

			if (state == TROOP_IDLE) {
				//LOG("Closest NOT FOUND - SEARCHING");

				info.closest = FindBuilding(map_pos, fromPlayer1, range);
				range = (info.closest == nullptr) ? range + 20 : original_range;

			}
		}
		else if (info.closest == nullptr) {
			state = TROOP_IDLE;
		}

	}
	else {
		//Current_Animation = Die;

	}

	ActOnDestroyed();
	ChangeAnimation();

	//DOES NOT CHANGE ANYTHING BY ITSELF - ONLY INPUT INSIDE -�
	ForceAnimations();
	//            _|

	Troop::Update(dt);
	return true;
}
void Hound::SetDestination()
{
	destination = App->map->WorldToMap(info.closest->position.first, info.closest->position.second);

	if (!App->pathfinding->IsWalkable(destination)) {
		destination.second -= 1;
		if (!App->pathfinding->IsWalkable(destination)) {
			destination.second += 2;
			if (!App->pathfinding->IsWalkable(destination)) {

				destination.first -= 1;
				destination.second -= 1;
				if (!App->pathfinding->IsWalkable(destination)) {
					destination.first += 2;

				}
			}
		}
	}
}
bool Hound::Is_inRange(pair<int, int> pos, int &distance, pair <int, int> position, int range) {

	//posicion entre dos entidades cualquiera
	//determina si esta en el rango

	pair <int, int> vector_distance = { position.first - pos.first, position.second - pos.second };
	distance = (int)(sqrt(pow(vector_distance.first, 2) + pow(vector_distance.second / 2, 2)));

	return distance <= range;
}
void Hound::PrintState() {
	switch (state)
	{
	case NOT_DEPLOYED:
		LOG("STATE = NOT_DEPLOYED");
		break;
	case TROOP_IDLE:
		LOG("STATE = IDLE");
		break;
	case MOVING:
		LOG("STATE = MOVING");
		break;
	case SHOOTING:
		LOG("STATE = SHOOTING");
		break;
	case REST:
		LOG("STATE = REST");
		break;
	case MAX_STATE:
		LOG("MOVING");
		break;
	default:
		break;
	}
}

void Hound::ForceAnimations() {

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
void Hound::ActOnDestroyed() {

	if (fromPlayer1)  // --- Player 1 --------------------------------
	{
		if (health <= 0) //destroyed
		{
			/*std::list <Entity*>::const_iterator unit = info.current_group->Units.begin();
			while (unit != info.current_group->Units.end()) {
			(*unit)->isSelected = true;
			unit++;
			}
			isSelected = false;
			App->move_manager->CreateGroup(App->player1);*/
			info.current_group->removeUnit(this);
			App->player1->DeleteEntity(this);
		}
	}
	else if (!fromPlayer1) // --- Player 2 ---------------------------
	{
		if (health <= 0) //destroyed
		{
			info.current_group->removeUnit(this);
			App->player2->DeleteEntity(this);
		}
	}
}

void Hound::CleanUp() {
	Troop::CleanUp();

}

void Hound::ChangeAnimation() {
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
	else if (state == SHOOTING)
	{

		if (fromPlayer1)
		{
			Current_Animation = shooting[SOUTH];
		}
		else
		{
			Current_Animation = shooting[NORTH];
		}

		if (info.closest->position == position)
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
		else if (info.closest->position.second <= position.second && info.closest->position.first >= position.first)
		{
			//noth
			Current_Animation = shooting[NORTH];
			if (info.closest->position.second == position.second)
			{
				//northwest
				Current_Animation = shooting[NORTHWEST];
			}
			//else if (info.closest->position.second > position.second)
			//{
			//	//north
			//	Current_Animation = shooting[NORTH];
			//}
			else if (info.closest->position.first == position.first)
			{
				//northeast
				Current_Animation = shooting[NORTHEAST];
			}
		}
		else if (info.closest->position.first >= position.first && info.closest->position.second >= position.second)
		{
			//south
			Current_Animation = shooting[SOUTH];
			if (info.closest->position.second == position.second)
			{
				//southwest
				Current_Animation = shooting[SOUTHWEST];
			}
			//else if (info.closest->position.second > position.second)
			//{
			//	//north
			//	Current_Animation = shooting[NORTH];
			//}
			else if (info.closest->position.first == position.first)
			{
				//southeast
				Current_Animation = shooting[SOUTHEAST];
			}
		}
		else if (info.closest->position.second > position.second && info.closest->position.first > position.first)
		{
			//east
			Current_Animation = shooting[EAST];
		}
		else if (info.closest->position.second < position.second && info.closest->position.first < position.first)
		{
			//west
			Current_Animation = shooting[WEST];

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


void Hound::LoadAnimations(bool isPlayer1, string path)
{
	BROFILER_CATEGORY("Hound Load Animations", Profiler::Color::Blue);
	moving = vector<Animation*>(TroopDir::MAX_DIR, nullptr);
	shooting = vector<Animation*>(TroopDir::MAX_DIR, nullptr);

	idle = idle->LoadAnimation(path.data(), (isPlayer1) ? "idle" : "idle_blue");

	moving[NORTH] = moving[NORTH]->LoadAnimation(path.data(), (isPlayer1) ? "run_N" : "blue_run_N");
	moving[SOUTH] = moving[SOUTH]->LoadAnimation(path.data(), (isPlayer1) ? "run_S" : "blue_run_S");
	moving[EAST] = moving[EAST]->LoadAnimation(path.data(), (isPlayer1) ? "run_E" : "blue_run_E");
	moving[WEST] = moving[WEST]->LoadAnimation(path.data(), (isPlayer1) ? "run_W" : "blue_run_W");
	moving[NORTHEAST] = moving[NORTHEAST]->LoadAnimation(path.data(), (isPlayer1) ? "run_NE" : "blue_run_NE");
	moving[NORTHWEST] = moving[NORTHWEST]->LoadAnimation(path.data(), (isPlayer1) ? "run_NW" : "blue_run_NW");
	moving[SOUTHEAST] = moving[SOUTHEAST]->LoadAnimation(path.data(), (isPlayer1) ? "run_SE" : "blue_run_SE");
	moving[SOUTHWEST] = moving[SOUTHWEST]->LoadAnimation(path.data(), (isPlayer1) ? "run_SW" : "blue_run_SW");

	shooting[NORTH] = shooting[NORTH]->LoadAnimation(path.data(), (isPlayer1) ? "attack_N" : "blue_attack_N");
	shooting[SOUTH] = shooting[SOUTH]->LoadAnimation(path.data(), (isPlayer1) ? "attack_S" : "blue_attack_S");
	shooting[EAST] = shooting[EAST]->LoadAnimation(path.data(), (isPlayer1) ? "attack_E" : "blue_attack_E");
	shooting[WEST] = shooting[WEST]->LoadAnimation(path.data(), (isPlayer1) ? "attack_W" : "blue_attack_W");
	shooting[NORTHEAST] = shooting[NORTHEAST]->LoadAnimation(path.data(), (isPlayer1) ? "attack_NE" : "blue_attack_NE");
	shooting[NORTHWEST] = shooting[NORTHWEST]->LoadAnimation(path.data(), (isPlayer1) ? "attack_NW" : "blue_attack_NW");
	shooting[SOUTHEAST] = shooting[SOUTHEAST]->LoadAnimation(path.data(), (isPlayer1) ? "attack_SE" : "blue_attack_SE");
	shooting[SOUTHWEST] = shooting[SOUTHWEST]->LoadAnimation(path.data(), (isPlayer1) ? "attack_SW" : "blue_attack_SW");

	for (int i = NORTH; i <= SOUTHWEST; i++) {
		moving[i]->speed = 10;
		shooting[i]->speed = 6;
	}
 
	idle->speed = 0;
	if (fromPlayer1)
	{
		idle->SetCurrentFrame(7);
	}
	else
	{
		idle->SetCurrentFrame(3);
	}
	Current_Animation = moving[NORTH];
}

Troop* Hound::FindBuilding(pair <int, int> pos, bool fromplayer1, int attackrange)
{
	Player* enemy = (!fromplayer1) ? App->player1 : App->player2;

	Troop* found = *enemy->troops.begin();
	int distance = 0;

	if (found != nullptr)
	{

		pair<int, int> map_pos = App->map->WorldToMap(found->position.first, found->position.second);
		Is_inRange(pos, distance, map_pos, attackrange);
		int min_dist = distance;

		for (list<Troop*>::iterator tmp = enemy->troops.begin(); tmp != enemy->troops.end(); tmp++) // traverse entity list (unordered)
		{
			map_pos = App->map->WorldToMap((*tmp)->position.first, (*tmp)->position.second);

			if (Is_inRange(pos, distance, map_pos, attackrange))
			{

				if (min_dist >= distance)
				{
					found = (*tmp);
					min_dist = distance;
				}
			}

		}
		if (min_dist <= attackrange)
		{
			return found;
		}

	}
	return nullptr;

}