#include "DefenseTarget.h"
#include "Brofiler\Brofiler.h"
#include "Player.h"
#include "Render.h"
#include "Log.h"

DefenseTarget::DefenseTarget()
{
}

DefenseTarget::~DefenseTarget()
{
}

DefenseTarget::DefenseTarget(bool isPlayer1, pair<int, int> pos) : Entity(entityType::DEFENSE_TARGET)
{
	LOG("Loading DefenseTarget");

	pugi::xml_document	config_file;
	pugi::xml_node		config;
	config = App->LoadConfig(config_file);
	config = config.child("entitymanager").child("buildings").child("defense_target");


	health_lv.push_back(config.child("health").attribute("lvl1").as_uint());
	health_lv.push_back(config.child("health").attribute("lvl2").as_uint());
	health_lv.push_back(config.child("health").attribute("lvl3").as_uint());
	

	upgrade_cost.push_back(0);
	upgrade_cost.push_back(config.child("upgrade_cost").attribute("ToLvl2").as_int());
	upgrade_cost.push_back(config.child("upgrade_cost").attribute("ToLvl3").as_int());

	size.first = config.child("size").attribute("width").as_int();
	size.first = config.child("size").attribute("height").as_int();

	damage_missiles.push_back(config.child("damage").attribute("lvl1").as_uint());
	damage_missiles.push_back(config.child("damage").attribute("lvl2").as_uint());
	damage_missiles.push_back(config.child("damage").attribute("lvl3").as_uint());

	range = config.child("range").attribute("value").as_int();
	rate_of_fire = config.child("rate_of_fire").attribute("value").as_float();

	name = config.child("name").attribute("string").as_string();

	level = 0;
	fromPlayer1 = isPlayer1;
	position = pos;
	health = health_lv[0];
	damage = damage_missiles[0];

	upgrade = repair = false;

	collider = { 0,0,64,32 };
	tex = App->tex->Load("maps/meta.png");
	LoadAnimations();
	ChangeAnimation();
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

	int x = 0;
	int y = 0;
	if (App->input->GetMouseButtonDown(SDL_BUTTON_MIDDLE)) {

		App->input->GetMousePosition(x, y);
		position.first = x;
		position.second = y;
	}

	App->render->Blit(tex, position.first, position.second, &collider);

	Player* tmpMod = App->player2;
	if (!fromPlayer1) {
		tmpMod = App->player1;
	}
	list<Entity*>::iterator tmp = tmpMod->troops.begin();
	
	Entity* closest = *tmpMod->troops.begin();
	int min_distance;
	int d = 0;
	Is_inRange(closest->position, min_distance);

	while (tmp != tmpMod->troops.end())
	{
		
		if (Is_inRange((*tmp)->position, d) && min_distance >= d) {
			closest = *tmp;
			min_distance = d;
		}
		tmp++;
	}

	if (timer.ReadSec() >= rate_of_fire && Is_inRange(closest->position,d))
	{
		closest->TakeDamage(damage_missiles[level]);
		timer.Start();
		LOG("Distance: %d", d);
	}

	return true;
}

void DefenseTarget::CleanUp()
{
}

void DefenseTarget::LoadAnimations()
{
	//not working, the Entity is represented by a green square
	building.PushBack({1212,3672,50,50});
	Current_Animation = &building;
}

void DefenseTarget::ChangeAnimation()
{
}

bool DefenseTarget::Is_inRange(pair<int, int> pos, int &distance) {

	pair <int, int> vector_distance = { position.first - pos.first, position.second - pos.second };
	distance = (int)(sqrt(pow(vector_distance.first, 2) + pow(vector_distance.second, 2)));
	
	return distance <= range;
}

