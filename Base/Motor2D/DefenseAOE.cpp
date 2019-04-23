#include "DefenseAoe.h"
#include "Brofiler\Brofiler.h"
#include "Audio.h"
#include "Player.h"
#include "Render.h"
#include "Log.h"
#include <map>


DefenseAoe::DefenseAoe()
{
	// NOW IS A COPY OF DEFENSE TARGET WITH DIFERENT BEHAVIOUR (UPDATE)
	// NEED TO CHANGE THE NAME INSIDE GetName() TO "defense_aoe" ONCE ANIMATIONS PARSED
}

DefenseAoe::~DefenseAoe()
{
}

DefenseAoe::DefenseAoe(bool isPlayer1, pair<int, int> pos, Collider collider) : Building(Entity::entityType::DEFENSE_AOE, isPlayer1, pos, collider)
{
	string path = "animation/" + name + ".tmx";
	LoadAnimations(isPlayer1, path.data());
	colider = collider;
}

bool DefenseAoe::Start()
{


	return true;
}

bool DefenseAoe::PreUpdate()
{
	BROFILER_CATEGORY("DefenseAoe PreUpdate", Profiler::Color::SandyBrown);


	return true;
}

bool DefenseAoe::Update(float dt)
{
	BROFILER_CATEGORY("DefenseAoe Update", Profiler::Color::SandyBrown);

	//Checks where to look for enemies
	Player* tmpMod = (fromPlayer1) ? App->player2 : App->player1;
	list<Troop*>::iterator tmp = tmpMod->troops.begin();

	// Finds the closest one
	vector<pair<int,Troop*>> enemies;

	if (tmp != tmpMod->troops.end()) {
		int d = 0;

		// Gets first distance
		Is_inRange((*tmp)->position, d);

		while (tmp != tmpMod->troops.end())
		{
			if ((*tmp)->alive && Is_inRange((*tmp)->position, d)) {

				if (enemies.size() < max_targets) {
					enemies.push_back({ d,*tmp });
				}
				else {
					int i = max_targets-1;
					while (i > 0 && enemies.at(i).first > d) {
						
						i--;
					}
					if (i != max_targets - 1) {
						enemies.at(i).first = d;
						enemies.at(i).second = *tmp;
					}
				}
			}
			tmp++;
		}

		// Shoots
		if (timer.ReadSec() >= rate_of_fire)
		{
			for (int i = 0; i < enemies.size(); i++) {

				enemies.at(i).second->TakeDamage(damage_lv[level]);
			}
			timer.Start();
			//LOG("Distance: %d", d);
		}
	}

	if (fromPlayer1)  // --- Player 1 --------------------------------
	{
		if (health <= 0) //destroyed
		{
			App->player1->UpdateWalkabilityMap(true, colider);
			App->player1->DeleteEntity(this);
			App->audio->PlayFx(BUILDING_EXPLOSION);
			App->render->Blit(App->scene->explosion_tex, position.first + 25, position.second + 25, &App->map->explosion_anim->GetCurrentFrame(dt));
		}
	}
	else if (!fromPlayer1) // --- Player 2 ---------------------------
	{
		if (health <= 0) //destroyed
		{
			App->player2->UpdateWalkabilityMap(true, colider);
			App->player2->DeleteEntity(this);
			App->audio->PlayFx(BUILDING_EXPLOSION);
			App->render->Blit(App->scene->explosion_tex, position.first + 25, position.second + 25, &App->map->explosion_anim->GetCurrentFrame(dt));
		}
	}

	if (Current_Animation->Finished() == true)
		Current_Animation = level1;

	Building::Update(dt);
	
	return true;
}

void DefenseAoe::CleanUp()
{
	// Its needed or the super class is always called?
	Entity::CleanUp();
}



bool DefenseAoe::Is_inRange(pair<int, int> pos, int &distance) {

	pair <int, int> vector_distance = { position.first - pos.first, position.second - pos.second };
	distance = (int)(sqrt(pow(vector_distance.first, 2) + pow(vector_distance.second, 2)));

	return distance <= range;
}

void DefenseAoe::LoadAnimations(bool isPlayer1, string path)
{
	building = building->LoadAnimation(path.data(), (isPlayer1) ? "soviet_aoe_build" : "allied_aoe_build");
	level1 = level1->LoadAnimation(path.data(), (isPlayer1) ? "soviet_aoe_idle" : "allied_aoe_idle");
	level1->speed = 1;
	building->speed = 9;
	building->loop = false;
	level1->loop = true;
	Current_Animation = building;
}

