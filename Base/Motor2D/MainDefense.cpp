#include "EntityManager.h"
#include "MainDefense.h"
#include "Scene.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Brofiler\Brofiler.h"


MainDefense::MainDefense() : Entity(entityType::MAIN_DEFENSE)
{
}

MainDefense::~MainDefense()
{
}

MainDefense::MainDefense(bool isPlayer1, pair<int, int> pos) : Entity(entityType::MAIN_DEFENSE)
{
	LOG("Loading Main Defense");

	pugi::xml_document	config_file;
	pugi::xml_node		config;
	config = App->LoadConfig(config_file);
	config = config.child("entitymanager").child("buildings").child("main_defense");


	health_lv.push_back(0);
	health_lv.push_back(config.child("health").attribute("lvl1").as_uint());
	health_lv.push_back(config.child("health").attribute("lvl2").as_uint());
	health_lv.push_back(config.child("health").attribute("lvl3").as_uint());

	size.first = config.child("size").attribute("width").as_int();
	size.first = config.child("size").attribute("height").as_int();

	name = config.child("name").attribute("string").as_string();
	range = config.child("range").attribute("value").as_uint();
	damage = config.child("damage").attribute("value").as_uint();

	level = 1;
	fromPlayer1 = isPlayer1;
	position = pos;

	upgrade = repair = false;
	production = capacity = level = 0;

	LoadAnimations();
	ChangeAnimation();
}

bool MainDefense::Update(float dt)
{
	BROFILER_CATEGORY("Main Defense Update", Profiler::Color::SandyBrown);

	if (fromPlayer1)
	{
		if (health > 0) //if not destroyed
		{
			//if troops in range -> attack
		}
		else
		{
			//destroyed
		}
	}
	else if (!fromPlayer1)
	{
		if (health > 0) //if not destroyed
		{
			//if troops in range -> attack
		}
		else
		{
			//destroyed
		}
	}
	ChangeAnimation();

	return true;
}

void MainDefense::CleanUp()
{
	LOG("---Main Defense Deleted");

	health_lv.clear();
	health_lv.resize(0);
}

void MainDefense::LoadAnimations()
{
	if (fromPlayer1) //load player 1 sprites
	{

	}
	else if (!fromPlayer1) //load player 2 sprites
	{

	}

}

void MainDefense::ChangeAnimation()
{
	if (health <= 0)
		Current_Animation = &destroyed;

	else if (health < (health_lv[level] / 2))
		Current_Animation = &damaged;

	else
		Current_Animation = &idle;
}
