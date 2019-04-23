#ifndef  __Troop_H__
#define  __Troop_H__

#include "Entity.h"
#include "Group.h"

enum TroopState{
	NOT_DEPLOYED,
	TROOP_IDLE,
	MOVING,
	SHOOTING,
	ALERT,

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

	
	pair<int, int> Speed = { 1,1 };

	Animation* idle = nullptr;
	vector<Animation*> moving;
	vector<Animation*> shooting;

	int curr = 0;

	vector<pair<int, int>> path;
	int path_count = 1;


	TroopState state;
	bool isInvulnerable = false;
	bool offensive_mode = false;
	float time_to_awake = 2.0f;


	// Group Movement
	int speed = 0;
	Group_Unit info;
	bool isSelected = true;
	
	pair<int, int> init_position;




	// CHANGED TO TROOP STATE
	/*bool isMoving = false;
	bool isShooting = false;*/
};

#endif