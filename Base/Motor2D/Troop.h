#ifndef  __Troop_H__
#define  __Troop_H__

#include "Entity.h"
#include "Group.h"

enum TroopDir {
	NORTH,
	SOUTH,
	EAST,
	WEST,
	NORTHEAST,
	NORTHWEST,
	SOUTHEAST,
	SOUTHWEST,

	MAX_DIR
};


class Troop : public Entity
{
public:

	Troop() {};
	Troop(entityType type, bool isPlayer1, pair<int, int> pos, Collider Collider) : Entity(type, isPlayer1, pos, Collider) {

		pugi::xml_document	config_file;
		pugi::xml_node config;
		config = App->LoadConfig(config_file);
		config = config.child("entitymanager").child("troops").child(name.data());

		speed = config.child("speed").attribute("value").as_int();
		collider = Collider;
		state = TROOP_IDLE;
		init_position = pos;
		

	}
	~Troop() {};

	void CleanUp() {
		Entity::CleanUp();
	}
	bool Update(float dt) {

		
		return true;
	}

public:

	
	

	Animation* idle = nullptr;
	vector<Animation*> moving;
	vector<Animation*> shooting;

	int curr = 0;

	vector<pair<int, int>> path;
	int path_count = 1;


	
	bool isInvulnerable = false;
	bool offensive_mode = false;
	float time_to_awake = 2.0f;


	
	
	pair<int, int> init_position;




	// CHANGED TO TROOP STATE
	/*bool isMoving = false;
	bool isShooting = false;*/
};

#endif