#include "EntityManager.h"
#include "CommandCenter.h"
#include "Audio.h"
#include "Scene.h"
#include "Render.h"
#include "Map.h"
#include "Player.h"
#include "Brofiler\Brofiler.h"

CmdCenter::CmdCenter()
{
}

CmdCenter::~CmdCenter()
{
}

CmdCenter::CmdCenter(bool isPlayer1, pair<int, int> pos, Collider collider) : Building(Entity::entityType::COMMAND_CENTER, isPlayer1, pos, collider)
{
	pugi::xml_document	config_file;
	pugi::xml_node		config;
	config = App->LoadConfig(config_file);
	config = config.child("entitymanager").child("buildings").child("command_center");

	damage_cast2.push_back(0);
	damage_cast2.push_back(config.child("damage_cast2").attribute("lvl1").as_uint(0));
	damage_cast2.push_back(config.child("damage_cast2").attribute("lvl2").as_uint(0));
	damage_cast2.push_back(config.child("damage_cast2").attribute("lvl3").as_uint(0));

	damage_cast3.push_back(0);
	damage_cast3.push_back(config.child("damage_cast3").attribute("lvl1").as_uint(0));
	damage_cast3.push_back(config.child("damage_cast3").attribute("lvl2").as_uint(0));
	damage_cast3.push_back(config.child("damage_cast3").attribute("lvl3").as_uint(0));

	string path = "animation/" + name + ".tmx";
	LoadAnimations(isPlayer1, path.data());
}

bool CmdCenter::Update(float dt)
{
	BROFILER_CATEGORY("CmdCenter Update", Profiler::Color::SandyBrown);

	if (fromPlayer1 == true)
	{
		position = App->map->data.special_skill;
	}
	else
	{
		position = App->map->data.special_skill2;
	}

	if (fromPlayer1)
	{
		if (health > 0) //if not destroyed
		{
			if (upgrade == true) //upgrade
			{
				App->player1->gold -= upgrade_cost[level]; //pay costs
				level++;
				upgrade = false;
			}
			if (App->player1->isCasting == true) //player casting
			{
				//if (missiles)
				damage = damage_lv[level];

				//else if (cast2)
				damage = damage_cast2[level];

				//else if (cast 3)
				damage = damage_cast3[level];
			}
		}
		else
		{
			App->player1->DeleteEntity(this);
			App->audio->PlayFx(BUILDING_EXPLOSION);
			App->render->Blit(App->scene->explosion_tex, position.first + 25, position.second + 25, &App->map->explosion_anim->GetCurrentFrame(dt));

		}
	}
	else if (!fromPlayer1)
	{
		if (health > 0) //if not destroyed
		{
			if (upgrade == true) //upgrade
			{
				App->player1->gold -= upgrade_cost[level]; //pay costs
				level++;
				upgrade = false;
			}
			if (App->player1->isCasting == true) //player casting
			{
				//if (missiles)
				damage = damage_lv[level];

				//else if (cast2)
				damage = damage_cast2[level];

				//else if (cast 3)
				damage = damage_cast3[level];
			}
		}
		else
		{
			App->player2->DeleteEntity(this);
			App->audio->PlayFx(BUILDING_EXPLOSION);
			App->render->Blit(App->scene->explosion_tex, position.first, position.second, &App->map->explosion_anim->GetCurrentFrame(dt));
		}
	}
	
	ChangeAnimation();

	Building::Update(dt);

	return true;
}

void CmdCenter::CleanUp()
{
	LOG("---CmdCenter Deleted");

	damage_cast2.clear();
	damage_cast2.resize(0);

	damage_cast3.clear();
	damage_cast3.resize(0);

	Building::CleanUp();

}

void CmdCenter::LoadAnimations(bool isPlayer1, string path)
{
	building = building->LoadAnimation(path.data(), (isPlayer1) ? "red" : "blue");
	level1 = level1->LoadAnimation(path.data(), (isPlayer1) ? "red" : "blue");
	//level1->PushBack(building->GetLastAnimationFrame());// level1->LoadAnimation(&path[0], (!isPlayer1) ? "red" : "blue");
	level1->speed = 10;
	building->speed = 10;
	building->loop = false;
	level1->loop = false;
	Current_Animation = building;
	offset = 5 * App->map->data.tile_height;
}

