#include "EntityManager.h"
#include "Walls.h"
#include "Scene.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Brofiler\Brofiler.h"


Walls::Walls() : Entity(entityType::WALLS)
{
}

Walls::~Walls()
{
}

Walls::Walls(bool isPlayer1, pair<int, int> pos) : Entity(entityType::WALLS)
{
	LOG("Loading Walls");

	pugi::xml_document	config_file;
	pugi::xml_node		config;
	config = App->LoadConfig(config_file);
	config = config.child("entitymanager").child("buildings").child("walls");

	size.first = config.child("size").attribute("width").as_int();
	size.first = config.child("size").attribute("height").as_int();
	name = config.child("name").attribute("string").as_string();
	health = config.child("health").attribute("value").as_int();

	fromPlayer1 = isPlayer1;
	position = pos;

	upgrade = repair = false;
	damage = capacity = range = level = 0;

	LoadAnimations();
	ChangeAnimation();
}

bool Walls::Update(float dt)
{
	BROFILER_CATEGORY("Walls Update", Profiler::Color::SandyBrown);

	ChangeAnimation();

	return true;
}

void Walls::CleanUp()
{
	LOG("---Wall Deleted");
}

void Walls::LoadAnimations()
{
	if (fromPlayer1) //load player 1 sprites
	{

	}
	else if (!fromPlayer1) //load player 2 sprites
	{

	}

}

void Walls::ChangeAnimation()
{
	if (health <= 0)
		Current_Animation = &destroyed;

	else if (health < (health / 2))
		Current_Animation = &damaged;

	else
		Current_Animation = &idle;
}
