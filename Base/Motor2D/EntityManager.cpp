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
	debug_tex = App->tex->Load("maps/pathfinding.png");

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

	if (App->scene->change == false && App->scene->pause == false)
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
	App->tex->UnLoad(debug_tex);

	return true;
}

bool EntityManager::Save(pugi::xml_node& file) const
{
	bool ret = true;
	list<Entity*>::const_iterator tmp = Entities.begin();
	while (tmp != Entities.end())
	{
		(*tmp)->Save(file);
		tmp++;
	}
	return ret;
}

bool EntityManager::Load(pugi::xml_node& file)
{
	bool ret = true;

	DeleteEntities();
	App->scene->SpawnEntities();

	list<Entity*>::iterator tmp = Entities.begin();
	pugi::xml_node box = file.child("box");

	while (tmp != Entities.end())
	{
		if ((*tmp)->type == Entity::entityType::PLAYER)
		{
			(*tmp)->Load(file.child("player"));
		}
		tmp++;
	}
	return ret;
}

bool EntityManager::Restart()
{
	bool ret = true;

	DeleteEnemies();
	App->scene->SpawnEnemies();
	list<Entity*>::reverse_iterator tmp = Entities.rbegin();
	while (tmp != Entities.rend())
	{
		if ((*tmp)->type == Entity::entityType::PLAYER)
		{
			(*tmp)->Restart();
			break;
		}
		tmp++;
	}
	return ret;
}

void EntityManager::DeleteEnemies()
{
	list<Entity*>::reverse_iterator tmp = Entities.rbegin();
	while (tmp != Entities.rend())
	{
		if ((*tmp)->type != Entity::entityType::PLAYER)
		{
			Entities.remove(*tmp);
			RELEASE(*tmp);
		}
		tmp++;
	}
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
		(*tmp)->Draw(dt);
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

Entity* EntityManager::AddEntity(Entity::entityType type, pair<int,int> position, pair<int,int> Size, string Type, string side)
{
	Entity* tmp = nullptr;

	switch (type)
	{
	case Entity::entityType::PLAYER:
		tmp = new Player();
		break;
	}

	if (tmp)
		Entities.push_back(tmp);

	return tmp;
}

bool EntityManager::DeleteEntity(Entity * entity)
{
	entity->CleanUp();
	//Entities.del(Entities.At(Entities.find(entity)));
	return true;
}