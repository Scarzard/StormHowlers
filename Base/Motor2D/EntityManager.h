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

	bool Draw(float dt);

	Entity* AddEntity(bool player1, Entity::entityType type, pair<int,int> position);
	bool DeleteEntity(Entity* entity);
	void DeleteAllEntities();

public:
	string			folder;
	string			texture_path;
	SDL_Texture*	texture;
	list<Entity*>	Entities;

	bool draw_path = false;
	bool godmode = false;
};

#endif //