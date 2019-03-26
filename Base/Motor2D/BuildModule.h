#ifndef __BUILD_MODULE_H__
#define __BUILD_MODULE_H__

#include "Log.h"
#include "Animation.h"
#include "Map.h"
#include "App.h"
#include "Entity.h"

class Entity;
class ActionBuild : public Module
{
public:
	ActionBuild();
	virtual ~ActionBuild();
	bool Awake(pugi::xml_node&);
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void CheckBuildingPos();
	void PlaceBuilding(Entity::entityType type, pair<int,int> position);
	void UpdateWalkabilityMap();

public:
	string name;

	pair<int, int> position;
	pair<int, int> size;

};

#endif