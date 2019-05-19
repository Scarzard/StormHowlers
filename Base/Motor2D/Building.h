#ifndef  __BUILDING_H__
#define  __BUILDING_H__

#include "Entity.h"

class Building   : public Entity
{
public:

	Building() {};
	Building(entityType type, bool isPlayer1, pair<int, int> pos, Collider collider) : Entity(type, isPlayer1, pos, collider) {
		pugi::xml_document	config_file;
		pugi::xml_node config;
		config = App->LoadConfig(config_file);
		config = config.child("entitymanager").child("buildings").child(&name[0]);
		
		capacity_lv.push_back(config.child("capacity").attribute("lvl1").as_uint(0));
		capacity_lv.push_back(config.child("capacity").attribute("lvl2").as_uint(0));
		capacity_lv.push_back(config.child("capacity").attribute("lvl3").as_uint(0));

		production_lv.push_back(config.child("production").attribute("lvl1").as_uint(0));
		production_lv.push_back(config.child("production").attribute("lvl2").as_uint(0));
		production_lv.push_back(config.child("production").attribute("lvl3").as_uint(0));

		cost_upgrade_lv.push_back(config.child("upgrade_cost").attribute("ToLvl2").as_uint(0));
		cost_upgrade_lv.push_back(config.child("upgrade_cost").attribute("ToLvl3").as_uint(0));
		cost_upgrade_lv.push_back(config.child("upgrade_cost").attribute("ToLvl4").as_uint(0));

		capacity = capacity_lv[level];
		production = production_lv[level];
		Upgrade_Cost = cost_upgrade_lv[level];

		////DEBUG PURPOSES
		//collider = { 0,0,59,28 };
		//if (!isPlayer1)
		//	collider = { 60,0,59,28 };


	}
	~Building() {};

	void LoadAnimations(bool isPlayer1, string path) {};

	void CleanUp() {
		production_lv.clear();
		production_lv.resize(0);

		capacity_lv.clear();
		capacity_lv.resize(0);
		
		Entity::CleanUp();
	}

	bool Update(float dt) {
		//SDL_Rect r = { position.first, position.second,size.first,size.second*0.5 };
		//SDL_RenderCopy(App->render->renderer, tex, &collider, &r);
		return true;
	}

public:

	vector<uint>	production_lv;
	vector<uint>	capacity_lv;
	vector<uint>	cost_upgrade_lv;

	list<Entity::entityType> TroopsCreated; //for barracks

	int production = 0;
	int capacity = 0;
	
};

#endif