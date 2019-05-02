#include "EntityManager.h"
#include "MainDefense.h"
#include "Scene.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
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
			closest->TakeDamage(damage_lv[level]);
			timer.Start();
			//LOG("Distance: %d", d);
		}
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

	if (Current_Animation->Finished() == true)
		Current_Animation = level1;

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
	building = building->LoadAnimation(path.data(), (isPlayer1) ? "red_constructing" : "blue_constructing");
	level1 = level1->LoadAnimation(path.data(), (isPlayer1) ? "red_SW" : "blue_NE");

	level1->speed = 10;
	building->speed = 10;

	building->loop = false;
	level1->loop = false;
	Current_Animation = building;
}

bool MainDefense::Is_inRange(pair<int, int> pos, int &distance) {

	pair <int, int> vector_distance = { position.first - pos.first, position.second - pos.second };
	distance = (int)(sqrt(pow(vector_distance.first, 2) + pow(vector_distance.second, 2)));

	return distance <= range;
}