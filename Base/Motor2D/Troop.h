#ifndef  __Troop_H__
#define  __Troop_H__

#include "Entity.h"

class Troop : public Entity
{
public:

	Troop() {};
	Troop(entityType type, bool isPlayer1, pair<int, int> pos) : Entity(type, isPlayer1, pos) {

		pugi::xml_document	config_file;
		pugi::xml_node config;
		config = App->LoadConfig(config_file);
		config = config.child("entitymanager").child("troops").child(&name[0]);

		speed = config.child("speed").attribute("value").as_int();

	}
	~Troop() {};

	void CleanUp() {

	}

public:

	int speed;
};

#endif