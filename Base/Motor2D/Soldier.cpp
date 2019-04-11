#include "Soldier.h"
#include "Render.h"


Soldier::Soldier()
{
	
}

Soldier::Soldier(bool isPlayer1, pair<int, int> pos):Entity(entityType::SOLDIER)
{
	LOG("Loading Soldier");

	pugi::xml_document	config_file;
	pugi::xml_node		config;
	config = App->LoadConfig(config_file);
	config = config.child("entitymanager").child("troops").child("soldier");


	health_lv.push_back(config.child("health").attribute("lvl1").as_uint());
	health_lv.push_back(config.child("health").attribute("lvl2").as_uint());
	health_lv.push_back(config.child("health").attribute("lvl3").as_uint());
	

	upgrade_cost.push_back(config.child("upgrade_cost").attribute("ToLvl2").as_int());
	upgrade_cost.push_back(config.child("upgrade_cost").attribute("ToLvl3").as_int());

	size.first = config.child("size").attribute("width").as_int();
	size.first = config.child("size").attribute("height").as_int();

	damage_missiles.push_back(config.child("damage").attribute("lvl1").as_uint());
	damage_missiles.push_back(config.child("damage").attribute("lvl2").as_uint());
	damage_missiles.push_back(config.child("damage").attribute("lvl3").as_uint());

	range = config.child("range").attribute("value").as_int();

	name = config.child("name").attribute("string").as_string();

	level = 1;
	fromPlayer1 = isPlayer1;
	position = pos;
	health = health_lv[level];
	upgrade = repair = false;
	health = health_lv[0];
	damage = damage_missiles[0];

	LoadAnimations();
	ChangeAnimation();

	tex = App->tex->Load("maps/meta.png");
	collider = { 64,0,64,32 };
}


Soldier::~Soldier()
{
}

bool Soldier::Update(float dt)
{
	int x = 0;
	int y = 0;
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) && position.first != 350) {

		App->input->GetMousePosition(x, y);
		position.first = x;
		position.second = y;
	}

	App->render->Blit(tex, position.first, position.second, &collider);

	return true;
}

void Soldier::TakeDamage(int damage) {
	
	if (alive) {

	health -= damage;

	if (health <= 0) {
		alive = false;
	}

	}
	LOG("Soldier Health: %d", health);
}
