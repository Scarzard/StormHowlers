#ifndef __EntityManager_H__
#define __EntityManager_H__

#include "Module.h"
#include "Entity.h"

class Entity;

class EntityManager : public Module
{
public:
	EntityManager();
	~EntityManager();

	bool Awake(pugi::xml_node & config);
	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	bool Save(pugi::xml_node&) const;
	bool Load(pugi::xml_node&);
	bool Restart();
	void DeleteEnemies();
	bool Draw(float dt);
	bool DebugDraw();

	Entity* AddEntity(Entity::entityType type, pair<int,int> position);
	bool DeleteEntity(Entity* entity);
	void DeleteEntities();

public:
	string			folder;
	string			texture_path;
	SDL_Texture*	texture;
	list<Entity*>	Entities;

	SDL_Texture*	debug_tex;

	bool draw_path = false;
	bool godmode = false;
};

#endif //