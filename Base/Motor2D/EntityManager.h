#ifndef __EntityManager_H__
#define __EntityManager_H__

#include "Module.h"
#include "Entity.h"
#include "Walls.h"

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

	Entity* AddEntity(bool player1, Entity::entityType type, pair<int,int> position, Collider collider);
	char * GetName(Entity::entityType type);
	void DeleteAllEntities();

	list<Entity*> EntityManager::OrderEntities(list<Entity*> List);
	int EntityManager::GetDepth(Entity* entity);

public:
	string			folder;
	string			texture_path;
	SDL_Texture*	texture;

	vector<SDL_Texture*> entitiesTextures;

	list<Entity*>	entity_list;

	bool draw_path = false;
	bool godmode = false;

	list<wall_parts*> wall_parts_list;
	SDL_Texture* wall_text;
};

#endif //