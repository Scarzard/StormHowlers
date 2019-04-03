#include "EntityManager.h"
#include "App.h"
#include "Audio.h"
#include "Render.h"
#include "Pathfinding.h"
#include "Scene.h"
#include "Transitions.h"
#include "Entity.h"
#include "Player.h"
#include "Textures.h"

#include "Brofiler\Brofiler.h"
#include "PugiXml/src/pugixml.hpp"

EntityManager::EntityManager()
{
	name.append("EntityManager");
}

EntityManager::~EntityManager()
{
}

bool EntityManager::Awake(pugi::xml_node &config)
{
	bool ret = true;
	folder.append(config.child("folder").child_value());
	texture_path = config.child("sprite_sheet").attribute("source").as_string();

	return ret;
}

bool EntityManager::Start()
{
	bool ret = true;
	texture = App->tex->Load(PATH(folder.data(), texture_path.data()));

	return ret;
}

bool EntityManager::Update(float dt)
{
	BROFILER_CATEGORY("EntityManager Update", Profiler::Color::Blue);

	bool ret = true;

	if (App->map->debug)
	{
		DebugDraw();
	}

	if (App->scene->pause == false)
	{
		list<Entity*>::const_iterator tmp = Entities.begin();
		while (tmp != Entities.end())
		{
			ret = (*tmp)->Update(dt);
			tmp++;
		}
	}

	return ret;
}

bool EntityManager::PostUpdate()
{
	BROFILER_CATEGORY("EntityManager PostUpdate", Profiler::Color::Blue);

	bool ret = true;

	if (App->scene->pause == false)
	{
		list<Entity*>::const_iterator tmp = Entities.begin();
		while (tmp != Entities.end())
		{
			(*tmp)->PostUpdate();
			tmp++;
		}
	}
	
	return ret;
}

bool EntityManager::CleanUp()
{
	DeleteEntities();
	App->tex->UnLoad(texture);

	return true;
}

void EntityManager::DeleteEntities()
{
	list<Entity*>::iterator tmp = Entities.begin();
	while (tmp != Entities.end())
	{
		RELEASE(*tmp);
		tmp++;
	}
	Entities.clear();
}

bool EntityManager::Draw(float dt) //modificar segun posicion en el mapa
{
	bool ret = true;
	list<Entity*>::iterator tmp = Entities.begin();

	while (tmp != Entities.end())
	{
		if ((*tmp)->flip)
		{
			App->render->Blit(texture, (*tmp)->position.first, (*tmp)->position.second, &((*tmp)->Current_Animation->GetCurrentFrame(dt)), SDL_FLIP_HORIZONTAL);
		}
		else
		{
			App->render->Blit(texture, (*tmp)->position.first, (*tmp)->position.second, &((*tmp)->Current_Animation->GetCurrentFrame(dt)), SDL_FLIP_NONE);
		}
		tmp++;
	}
	return ret;
}

bool EntityManager::DebugDraw()
{
	list<Entity*>::iterator tmp = Entities.begin();
	SDL_Rect col;
	while (tmp != Entities.end())
	{
		col.x = (*tmp)->Collider.x;
		col.y = (*tmp)->Collider.y;
		col.h = (*tmp)->Collider.h;
		col.w = (*tmp)->Collider.w;
		App->render->DrawQuad(col, 0, 0, 255, 50); //blue

		tmp++;
	}
	return true;
}

Entity* EntityManager::AddEntity(bool isPlayer1, Entity::entityType type, pair<int, int> position)
{
	Entity* tmp = nullptr;

	switch (type)
	{
	case Entity::entityType::TOWNHALL:
		//tmp = new TownHall(isPlayer1, position);
		break;

	case Entity::entityType::MAIN_DEFENSE:
		//tmp = new Main_Defense(isPlayer1, position);
		break;

	case Entity::entityType::COMMAND_CENTER:
		//tmp = new Command_Center(isPlayer1, position);
		break;

	case Entity::entityType::WALLS:
		//tmp = new Wall(isPlayer1, position);
		break;

	case Entity::entityType::DEFENSE_AOE:
		//tmp = new Defense_AOE(isPlayer1, position);
		break;

	case Entity::entityType::DEFENSE_TARGET:
		//tmp = new Defense_Target(isPlayer1, position);
		break;

	case Entity::entityType::MINES:
		//tmp = new Mines(isPlayer1, position);
		break;

	case Entity::entityType::BARRACKS:
		//tmp = new Barracks(isPlayer1, position);
		break;
	}

	if (tmp)
		Entities.push_back(tmp);

	return tmp;
}

bool EntityManager::DeleteEntity(Entity * entity)
{
	entity->CleanUp();

	list<Entity*> ::iterator item = Entities.begin(); //main entities list
	while (item != Entities.end())
	{
		if ((*item) == entity)
			Entities.erase(item);
	}
	return true;
}