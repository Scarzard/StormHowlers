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

		collider = { 180,0,59,28 };
		if (!isPlayer1)
			collider = { 120,0,59,28 };

	}
	~Troop() {};

	void CleanUp() {
		Entity::CleanUp();
	}
	bool Update(float dt) {

		SDL_Rect r = { position.first, position.second,size.first,size.second*0.5 };
		SDL_RenderCopy(App->render->renderer, tex, &collider, &r);
		return true;
	}

public:

	int speed;
};

#endif