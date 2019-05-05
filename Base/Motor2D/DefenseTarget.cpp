#include "DefenseTarget.h"
#include "Brofiler\Brofiler.h"
#include "Player.h"
#include "Audio.h"
#include "Log.h"

DefenseTarget::DefenseTarget()
{
}

DefenseTarget::~DefenseTarget()
{
}

DefenseTarget::DefenseTarget(bool isPlayer1, pair<int, int> pos, Collider collider) : Building(entityType::DEFENSE_TARGET, isPlayer1, pos, collider)
{
	string path = "animation/" + name + ".tmx";
	LoadAnimations(isPlayer1, path.data());

}

bool DefenseTarget::Start()
{
	

	return true;
}

bool DefenseTarget::PreUpdate()
{
	BROFILER_CATEGORY("DefenseTarget PreUpdate", Profiler::Color::SandyBrown);


	return true;
}

bool DefenseTarget::Update(float dt)
{
	BROFILER_CATEGORY("DefenseTarget Update", Profiler::Color::SandyBrown);
	
	if (fromPlayer1 == true)
	{
		position = App->map->data.main_tower;
	}
	else
	{
		position = App->map->data.main_tower2;
	}

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
			App->audio->PlayFx(TESLA_ATTACK);
			//LOG("Distance: %d", d);
		}
	}
	if (fromPlayer1)  // --- Player 1 --------------------------------
	{
		if (health <= 0) //destroyed
		{
			App->player1->DeleteEntity(this);
			App->audio->PlayFx(BUILDING_EXPLOSION);
			App->render->Blit(App->scene->explosion_tex, position.first + 25, position.second + 25, &App->map->explosion_anim->GetCurrentFrame(dt));
		}
	}
	else if (!fromPlayer1) // --- Player 2 ---------------------------
	{
		if (health <= 0) //destroyed
		{
			App->player2->DeleteEntity(this);
			App->audio->PlayFx(BUILDING_EXPLOSION);
			App->render->Blit(App->scene->explosion_tex, position.first + 25, position.second + 25, &App->map->explosion_anim->GetCurrentFrame(dt));
		}
	}
	Building::Update(dt);
	return true;
}

void DefenseTarget::CleanUp()
{
	Entity::CleanUp();
}



bool DefenseTarget::Is_inRange(pair<int, int> pos, int &distance) {

	pair <int, int> vector_distance = { position.first - pos.first, position.second - pos.second };
	distance = (int)(sqrt(pow(vector_distance.first, 2) + pow(vector_distance.second, 2)));
	
	return distance <= range;
}

void DefenseTarget::LoadAnimations(bool isPlayer1, string path)
{
	building = building->LoadAnimation(path.data(), (isPlayer1) ? "red" : "blue");
	level1 = level1->LoadAnimation(path.data(), (isPlayer1) ? "red" : "blue");
	level1->speed = 10;
	building->speed = 10;
	building->loop = false;
	level1->loop = false;
	Current_Animation = building;
}

