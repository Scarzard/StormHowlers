#ifndef  __BUILDING_H__
#define  __BUILDING_H__

#include "Entity.h"

class Building   : public Entity
{
public:

	Building() {};
	Building(entityType type, bool isPlayer1, pair<int, int> pos) : Entity(type, isPlayer1, pos) {

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

		capacity = capacity_lv[level];
		production = production_lv[level];

	}
	~Building() {};

	void CleanUp() {
		production_lv.clear();
		production_lv.resize(0);

		capacity_lv.clear();
		capacity_lv.resize(0);
	}

public:

	vector<uint>	production_lv;
	vector<uint>	capacity_lv;

	int production;
	int capacity;
};

#endif