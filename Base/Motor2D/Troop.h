#ifndef  __Troop_H__
#define  __Troop_H__

#include "Entity.h"
#include "Group.h"

enum TroopState{
	IDLE,
	MOVING,
	SHOOTING,

	MAX_STATE
};

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
		config = config.child("entitymanager").child("troops").child(&name[0]);

		speed = config.child("speed").attribute("value").as_int();

		collider = Collider;

		//collider = { 180,0,59,28 };
		//if (!isPlayer1)
		//	collider = { 120,0,59,28 };

	}
	~Troop() {};

	void CleanUp() {
		Entity::CleanUp();
	}
	bool Update(float dt) {

		
		return true;
	}

public:

	
	pair<int, int> Speed = { 1,1 };

	Animation* idle;
	vector<Animation*> moving;
	vector<Animation*> shooting;

	int curr = 0;

	vector<pair<int, int>> path;
	int path_count = 1;


	TroopState state;
	bool isInvulnerable = false;
	// CHANGED TO TROOP STATE
	/*bool isMoving = false;
	bool isShooting = false;*/

	// Group Movement
	int speed;
	Group_Unit info;
	bool isSelected = true;
	
};

#endif