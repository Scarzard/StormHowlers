#include "Infiltrator.h"
#include "Pathfinding.h"
#include "Player.h"
#include "Audio.h"


Infiltrator::Infiltrator()
{

}

Infiltrator::Infiltrator(bool isPlayer1, pair<int, int> pos, Collider collider) :Troop(Entity::entityType::INFILTRATOR, isPlayer1, pos, collider)
{
	BROFILER_CATEGORY("Infiltrator constructor", Profiler::Color::Red);
	string path = "animation/" + name + ".tmx";
	//LoadAnimations(isPlayer1, path.data());
	offset = range + 10;
	destination = pos;
	original_range = range;

	state = MOVING;
	timer.Start();
	lead = true;
}


Infiltrator::~Infiltrator()
{
}

bool Infiltrator::Update(float dt)
{
	// change bool from offensive and defensive
	if (fromPlayer1)
	{
		offensive = App->player1->Infiltrator_Offensive;
	}
	else
	{
		offensive = App->player2->Infiltrator_Offensive;
	}

	if (alive) {

		if (lead)
		{

			pair <int, int > map_pos = App->map->WorldToMap(position.first, position.second);

			switch (state)
			{
			case TROOP_IDLE:
				if (timer.ReadSec() >= 1 && info.closest == nullptr)
				{
					state = SEARCH;
					timer.Start();
					//facing direction for 
				}
				if (offensive)
				{
					state = MOVING;
					info.closest = nullptr;
					pathfind = false;
					timer.Start();
				}
				break;

			case MOVING:
				if (pathfind)
				{
					if (info.closest != nullptr)
					{
						MovementPathfind(info.closest, map_pos);
					}
					else
					{
						pathfind = false;
						state = MOVING;
					}
				}
				else if (timer.ReadSec() > 1 && info.closest == nullptr && offensive)
				{
					state = SEARCH;
					timer.Start();
				}
				else
				{

					SimpleMovement();

					if (!offensive)
					{
						if (fromPlayer1 && IsInAllyZone(map_pos))
						{
							state = TROOP_IDLE;
							timer.Start();
						}
						else if (!fromPlayer1 && IsInEnemyZone(map_pos))
						{
							state = TROOP_IDLE;
							timer.Start();
						}
					}

				}
				break;

			case SHOOTING:

				// Shoots the closest one if in range
				if (info.closest == nullptr)
				{
					state = MOVING;
					timer.Start();

				}
				if (timer.ReadSec() >= rate_of_fire)
				{
					info.closest->TakeDamage(damage_lv[level]);
					timer.Start();
					App->audio->PlayFx(SOLDIER_ATTACK);
				}
				if (info.closest->health <= 0)
				{
					info.closest->alive = false;
					state = MOVING;
					timer.Start();
					info.closest = nullptr;
				}

				break;

			case SEARCH:

				state = MOVING;
				timer.Start();

				//ally zone
				if (IsInAllyZone(map_pos))
				{
					if (fromPlayer1)
					{
						info.closest = FindEntityInAttackRange(map_pos, fromPlayer1, original_range, entityType::BARRACKS);

						if (info.closest != nullptr)
						{
							state = SHOOTING;
							timer.Start();

						}
						else if (info.closest == nullptr && offensive == false)
						{
							// capar a entidades en la misma zona
							info.closest = FindNearestSoldierforDefense(map_pos, fromPlayer1, entityType::SOLDIER, 1);
							if (info.closest->type < entityType::SOLDIER)
							{
								info.closest = nullptr;
								state = TROOP_IDLE;
								timer.Start();

							}
							else
							{

								state = MOVING;
								pathfind = true;
							}
						}
						else
						{
							state = MOVING;
							//pathfind = true;
						}

					}
					else
					{
						info.closest = FindEntityInAttackRange(map_pos, fromPlayer1, original_range, entityType::BARRACKS);

						if (info.closest != nullptr)
						{
							state = SHOOTING;
							timer.Start();
						}
						else if (info.closest == nullptr && offensive == true)
						{
							info.closest = FindNearestEntity(map_pos, fromPlayer1, entityType::BARRACKS, 1);

							state = MOVING;
							pathfind = true;
						}
						else
						{
							state = MOVING;
							//pathfind = true;
						}
					}


				}
				// enemy zone
				else if (IsInEnemyZone(map_pos))
				{
					if (fromPlayer1)
					{
						info.closest = FindEntityInAttackRange(map_pos, fromPlayer1, original_range, entityType::BARRACKS);

						if (info.closest != nullptr)
						{
							state = SHOOTING;
							timer.Start();
						}
						else if (info.closest == nullptr && offensive == true)
						{
							info.closest = FindNearestEntity(map_pos, fromPlayer1, entityType::BARRACKS, 2);
							state = MOVING;
							pathfind = true;
						}
						else
						{
							state = MOVING;
							//pathfind = true;
						}
						//find entity in range
						//if exist, state= shoot;
						//else, state= move


					}
					else
					{
						info.closest = FindEntityInAttackRange(map_pos, fromPlayer1, original_range, entityType::BARRACKS);

						if (info.closest != nullptr)
						{
							state = SHOOTING;
							timer.Start();
						}
						else if (info.closest == nullptr && offensive == false)
						{
							info.closest = FindNearestSoldierforDefense(map_pos, fromPlayer1, entityType::SOLDIER, 2);
							if (info.closest->type < entityType::SOLDIER)
							{
								info.closest = nullptr;
								state = TROOP_IDLE;
								timer.Start();

							}
							else
							{

								state = MOVING;
								pathfind = true;
							}

						}
						else
						{
							state = MOVING;
							//pathfind = true;
						}
						//find entity in range
						//if exist, state= shoot;
						//else, state= move
					}
				}
				// war zone
				else if (IsInWarZone(map_pos) && offensive)
				{

					info.closest = FindEntityInAttackRange(map_pos, fromPlayer1, original_range, entityType::SOLDIER);

					if (info.closest != nullptr)
					{
						state = SHOOTING;
						timer.Start();
					}
					else
					{
						state == MOVING;
					}


					//find entity en shoot range,
					//if closes exits, state=shoot
					//else state move
				}




				break;

			default:

				state = MOVING;
				pathfind = false;

				break;
			}

		}

		else
		{
			// get info from lead
			// lead gives out an information of state and enemy, and speed of moving, the troops haves 2 stages. move and shoot, when leader shoot, they shoot, when leader does other the move

		}

	}
	else {
		//Current_Animation = Die;

	}

	ActOnDestroyed();
	ChangeAnimation(facing, pathfind);

	//DOES NOT CHANGE ANYTHING BY ITSELF - ONLY INPUT INSIDE -¬
	//ForceAnimations();
	//            _|

	Troop::Update(dt);

	return true;
}

void Infiltrator::SetDestination()
{
	destination = App->map->WorldToMap(info.closest->position.first, info.closest->position.second);

	/*if (!App->pathfinding->IsWalkable(destination)) {
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

	}*/
}
bool Infiltrator::Is_inRange(pair<int, int> pos, int &distance, pair <int, int> position, int range) {

	//posicion entre dos entidades cualquiera
	//determina si esta en el rango

	pair <int, int> vector_distance = { position.first - pos.first, position.second - pos.second };
	distance = (int)(sqrt(pow(vector_distance.first, 2) + pow(vector_distance.second / 2, 2)));

	return distance <= range;
}


void Infiltrator::ActOnDestroyed() {

	if (fromPlayer1)  // --- Player 1 --------------------------------
	{
		if (health <= 0) //destroyed
		{

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

void Infiltrator::CleanUp() {
	Troop::CleanUp();

}

void Infiltrator::MovementPathfind(Entity* target, pair <int, int> map_pos)
{
	bool north = true;
	bool south = true;
	bool west = true;
	bool east = true;
	bool move = false;

	// search tiles around
	pair <int, int> cell = { map_pos.first, map_pos.second + 1 };

	if (!App->pathfinding->IsWalkable(cell))
	{
		east = false;
	}

	else if (!App->pathfinding->IsWalkable({ map_pos.first, map_pos.second - 1 }))
	{
		west = false;
	}

	if (!App->pathfinding->IsWalkable({ map_pos.first + 1, map_pos.second }))
	{
		north = false;
	}


	else if (!App->pathfinding->IsWalkable({ map_pos.first - 1, map_pos.second }))
	{
		south = false;
	}

	if (position.first <= target->position.first - offset && east == true)
	{
		position.first += 2 * speed;
		move = true;
	}
	else if (position.first >= target->position.first + offset && west == true)
	{
		position.first -= 2 * speed;
		move = true;
	}
	if (position.second <= target->position.second - offset * 2 && south == true)
	{
		position.second += 1 * speed;
		move = true;
	}
	else if (position.second >= target->position.second + offset * 2 && north == true)
	{
		position.second -= 1 * speed;
		move = true;
	}


	/*if (move==false )
	{
		pathfind = false;
	}*/

	/*if (move==false && west==false)
	{
		position.first += 2;
	}
	if (move==false && east==false)
	{
		position.first -= 2;
	}
	if (move==false && south==false)
	{
		position.second += 1;
	}
	if (move==false && north==false)
	{
		position.second -= 1;
	}*/

	if (position.first >= target->position.first - offset * 2 &&
		position.first <= target->position.first + offset * 2 &&
		position.second >= target->position.second - offset * 4 &&
		position.second <= target->position.second + offset * 4
		)
	{
		state = SHOOTING;

	}
	/*else
	{
		state = SEARCH;
		info.closest = nullptr;
	}*/

	if (north == false || south == false && east == false || west == false)
	{
		state = SEARCH;
		info.closest = nullptr;
	}
}

void Infiltrator::SimpleMovement()
{
	//NOTE: pathfind is wakable is giving prolem, returning the info moved one to the left

	bool north = true;
	bool south = true;
	bool west = true;
	bool east = true;

	bool fromPlayer = fromPlayer1;

	if (offensive == false)
	{
		fromPlayer = !fromPlayer;
	}

	pair <int, int> aux = position;

	if (fromPlayer)
	{
		//north
		pair <int, int > map_pos_aux = App->map->WorldToMap(aux.first - 2, aux.second + 1);
		south = App->pathfinding->IsWalkable(map_pos_aux);

		// if can go north, go north
		if (south)
		{
			//move front
			position.first += -2 * speed;
			position.second += 1 * speed;
			facing = SOUTH;
		}
		else
		{
			//move rigth
			position.first += 2 * speed;
			position.second += 1 * speed;
			facing = EAST;
		}

	}
	else if (!fromPlayer)
	{

		//north
		pair <int, int > map_pos_aux = App->map->WorldToMap(aux.first + 2, aux.second - 1);
		north = App->pathfinding->IsWalkable(map_pos_aux);

		// if can go north, go north
		if (north)
		{
			//move front
			position.first += 2 * speed;
			position.second += -1 * speed;
			facing = NORTH;
		}
		else
		{
			//move rigth
			position.first += 2 * speed;
			position.second += 1 * speed;
			facing = EAST;
		}
	}


}

void Infiltrator::ChangeAnimation(TroopDir facing, bool pathfind) {
	Current_Animation = idle;
	if (pathfind)
	{
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
	else
	{
		if (state == MOVING)
		{
			//isShooting = false;
			if (facing == NORTH)
			{
				//north
				Current_Animation = moving[NORTHEAST];
			}
			else if (facing == SOUTH)
			{
				//south
				Current_Animation = moving[SOUTHWEST];
			}
			else if (facing == WEST)
			{
				//west
				Current_Animation = moving[NORTHWEST];
			}
			else if (facing == EAST)
			{
				//east
				Current_Animation = moving[SOUTHEAST];
			}
			//else if (Speed.first > 0 && Speed.second < 0)
			//{
			//	//north east
			//	Current_Animation = moving[NORTHEAST];
			//}
			//else if (Speed.first > 0 && Speed.second > 0)
			//{
			//	//south east
			//	Current_Animation = moving[SOUTHEAST];
			//}
			//else if (Speed.first < 0 && Speed.second < 0)
			//{
			//	//north west
			//	Current_Animation = moving[NORTHWEST];
			//}
			//else if (Speed.first < 0 && Speed.second > 0)
			//{
			//	//south wst
			//	Current_Animation = moving[SOUTHWEST];
			//}
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

}


void Infiltrator::LoadAnimations(bool isPlayer1, string path)
{
	BROFILER_CATEGORY("Infiltrator Load Animations", Profiler::Color::Blue);
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

	shooting[NORTH] = shooting[NORTH]->LoadAnimation(path.data(), (isPlayer1) ? "red_shoot_N" : "blue_shoot_N");
	shooting[SOUTH] = shooting[SOUTH]->LoadAnimation(path.data(), (isPlayer1) ? "red_shoot_S" : "blue_shoot_S");
	shooting[EAST] = shooting[EAST]->LoadAnimation(path.data(), (isPlayer1) ? "red_shoot_E" : "blue_shoot_E");
	shooting[WEST] = shooting[WEST]->LoadAnimation(path.data(), (isPlayer1) ? "red_shoot_W" : "blue_shoot_W");
	shooting[NORTHEAST] = shooting[NORTHEAST]->LoadAnimation(path.data(), (isPlayer1) ? "red_shoot_NE" : "blue_shoot_NE");
	shooting[NORTHWEST] = shooting[NORTHWEST]->LoadAnimation(path.data(), (isPlayer1) ? "red_shoot_NW" : "blue_shoot_NW");
	shooting[SOUTHEAST] = shooting[SOUTHEAST]->LoadAnimation(path.data(), (isPlayer1) ? "red_shoot_SE" : "blue_shoot_SE");
	shooting[SOUTHWEST] = shooting[SOUTHWEST]->LoadAnimation(path.data(), (isPlayer1) ? "red_shoot_SW" : "blue_shoot_SW");

	for (int i = NORTH; i <= SOUTHWEST; i++) {
		moving[i]->speed = 10;
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

Entity* Infiltrator::FindEntityInAttackRange(pair <int, int> pos, bool fromplayer1, int attackrange, entityType desiredtype, int zone)
{
	// zone 0 is no zone
	// zone 1 ally zone
	// zone 2 enemy zone
	// zone 3 war zone


	Player* enemy = (!fromplayer1) ? App->player1 : App->player2;

	Entity* found = *enemy->entities.begin();
	int distance = 0;
	pair<int, int> map_pos;
	int min_dist;
	//bool once = false;

	if (found != nullptr)
	{
		//take the first of de desired group

		map_pos = App->map->WorldToMap(found->position.first, found->position.second);
		Is_inRange(pos, distance, map_pos, attackrange);
		min_dist = distance;



		for (list<Entity*>::iterator tmp = enemy->entities.begin(); tmp != enemy->entities.end(); tmp++) // traverse entity list (unordered)
		{
			if ((*tmp)->type <= desiredtype)
			{
				map_pos = App->map->WorldToMap((*tmp)->position.first, (*tmp)->position.second);

				if (zone == 0)
				{

					if (Is_inRange(pos, distance, map_pos, attackrange))
					{

						if (min_dist >= distance)
						{
							found = (*tmp);
							min_dist = distance;
						}
					}

				}
				if (zone == 1)
				{
					if (Is_inRange(pos, distance, map_pos, attackrange) && IsInAllyZone(map_pos))
					{

						if (min_dist >= distance)
						{
							found = (*tmp);
							min_dist = distance;
						}
					}

				}
				if (zone == 2)
				{
					if (Is_inRange(pos, distance, map_pos, attackrange) && IsInEnemyZone(map_pos))
					{

						if (min_dist >= distance)
						{
							found = (*tmp);
							min_dist = distance;
						}
					}

				}
				if (zone == 3)
				{
					if (Is_inRange(pos, distance, map_pos, attackrange) && IsInWarZone(map_pos))
					{

						if (min_dist >= distance)
						{
							found = (*tmp);
							min_dist = distance;
						}
					}

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

Entity* Infiltrator::FindNearestEntity(pair <int, int> pos, bool fromplayer1, entityType desiredtype, int zones)
{

	// zone 0 is no zone
	// zone 1 ally zone
	// zone 2 enemy zone
	// zone 3 war zone

	Player* enemy = (!fromplayer1) ? App->player1 : App->player2;

	Entity* found = *enemy->entities.begin();
	int distance = 0;
	pair<int, int> map_pos;
	int min_dist;
	bool once = false;

	if (found != nullptr)
	{
		//take the first of de desired group
		map_pos = App->map->WorldToMap(found->position.first, found->position.second);
		Is_inRange(pos, distance, map_pos, 0);
		min_dist = distance;

		for (list<Entity*>::iterator tmp = enemy->entities.begin(); tmp != enemy->entities.end(); tmp++) // traverse entity list (unordered)
		{
			if ((*tmp)->type <= desiredtype)
			{
				if (zones == 0)
				{
					map_pos = App->map->WorldToMap((*tmp)->position.first, (*tmp)->position.second);


					Is_inRange(pos, distance, map_pos, 0);


					if (min_dist > distance)
					{
						found = (*tmp);
						min_dist = distance;
					}

				}
				else if (zones == 1)
				{
					map_pos = App->map->WorldToMap((*tmp)->position.first, (*tmp)->position.second);

					Is_inRange(pos, distance, map_pos, 0);

					if (IsInAllyZone(map_pos))
					{
						if (once == false)
						{
							found = (*tmp);
							once = true;
						}

						if (min_dist > distance)
						{
							found = (*tmp);
							min_dist = distance;
						}

					}
				}
				else if (zones == 2)
				{
					map_pos = App->map->WorldToMap((*tmp)->position.first, (*tmp)->position.second);

					Is_inRange(pos, distance, map_pos, 0);

					if (IsInEnemyZone(map_pos))
					{
						if (once == false)
						{
							found = (*tmp);
							once = true;
						}

						if (min_dist > distance)
						{
							found = (*tmp);
							min_dist = distance;
						}

					}

				}
				else if (zones == 3)
				{
					map_pos = App->map->WorldToMap((*tmp)->position.first, (*tmp)->position.second);

					Is_inRange(pos, distance, map_pos, 0);

					if (IsInWarZone(map_pos))
					{
						if (once == false)
						{
							found = (*tmp);
							once = true;
						}

						if (min_dist > distance)
						{
							found = (*tmp);
							min_dist = distance;
						}

					}

				}
			}
		}

	}

	return found;

}


bool  Infiltrator::IsInAllyZone(pair <int, int > map_pos)
{
	//must be in map coordinates
	return (map_pos.second > App->map->allyzone.up_limit.second && map_pos.second < App->map->allyzone.down_limit.second);
}

bool  Infiltrator::IsInEnemyZone(pair <int, int > map_pos)
{
	//must be in map coordinates
	return (map_pos.second > App->map->sovietzone.up_limit.second && map_pos.second < App->map->sovietzone.down_limit.second);
}

bool  Infiltrator::IsInWarZone(pair <int, int > map_pos)
{
	//must be in map coordinates
	return (map_pos.first > App->map->warzone.up_limit.first && map_pos.first < App->map->warzone.down_limit.first);
}

Entity* Infiltrator::FindNearestSoldierforDefense(pair <int, int> pos, bool fromplayer1, entityType desiredtype, int zones)
{

	// zone 0 is no zone
	// zone 1 ally zone
	// zone 2 enemy zone
	// zone 3 war zone

	Player* enemy = (!fromplayer1) ? App->player1 : App->player2;

	Entity* found = *enemy->entities.begin();
	int distance = 0;
	pair<int, int> map_pos;
	int min_dist;
	bool once = false;

	if (found != nullptr)
	{
		//take the first of de desired group
		map_pos = App->map->WorldToMap(found->position.first, found->position.second);
		Is_inRange(pos, distance, map_pos, 0);
		min_dist = distance;

		for (list<Entity*>::iterator tmp = enemy->entities.begin(); tmp != enemy->entities.end(); tmp++) // traverse entity list (unordered)
		{
			if ((*tmp)->type >= desiredtype)
			{
				if (zones == 0)
				{
					map_pos = App->map->WorldToMap((*tmp)->position.first, (*tmp)->position.second);


					Is_inRange(pos, distance, map_pos, 0);


					if (min_dist > distance)
					{
						found = (*tmp);
						min_dist = distance;
					}

				}
				else if (zones == 1)
				{
					map_pos = App->map->WorldToMap((*tmp)->position.first, (*tmp)->position.second);

					Is_inRange(pos, distance, map_pos, 0);

					if (IsInAllyZone(map_pos))
					{
						if (once == false)
						{
							found = (*tmp);
							once = true;
						}

						if (min_dist > distance)
						{
							found = (*tmp);
							min_dist = distance;
						}

					}
				}
				else if (zones == 2)
				{
					map_pos = App->map->WorldToMap((*tmp)->position.first, (*tmp)->position.second);

					Is_inRange(pos, distance, map_pos, 0);

					if (IsInEnemyZone(map_pos))
					{
						if (once == false)
						{
							found = (*tmp);
							once = true;
						}

						if (min_dist > distance)
						{
							found = (*tmp);
							min_dist = distance;
						}

					}

				}
				else if (zones == 3)
				{
					map_pos = App->map->WorldToMap((*tmp)->position.first, (*tmp)->position.second);

					Is_inRange(pos, distance, map_pos, 0);

					if (IsInWarZone(map_pos))
					{
						if (once == false)
						{
							found = (*tmp);
							once = true;
						}

						if (min_dist > distance)
						{
							found = (*tmp);
							min_dist = distance;
						}

					}

				}
			}
		}

	}

	return found;

}