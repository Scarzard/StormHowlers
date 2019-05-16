#ifndef __Hound_H__
#define __Hound_H__

#include "Troop.h"
#include "EntityManager.h"


class Hound : public Troop
{
public:
	Hound();
	Hound(bool isPlayer1, pair<int, int> pos, Collider collider);
	~Hound();

	bool Update(float dt);
	void SetDestination();
	bool Is_inRange(pair<int, int> pos, int & distance, pair<int, int> position, int range);
	void ActOnDestroyed();
	void CleanUp();
	void MovementPathfind(Entity* target, pair <int, int> map_pos);
	void SimpleMovement();
	void ChangeAnimation(TroopDir facing, bool fromplayer1);

	Entity * FindEntityInAttackRange(pair<int, int> pos, bool fromplayer1, int attackrange, entityType type);
	Entity * FindNearestEntity(pair<int, int> pos, bool fromplayer1, entityType type);


	void LoadAnimations(bool isPlayer1, string path);

	//bool Is_inRange(pair<int, int> pos, int & distance);
	//bool defensive = false;
	bool offensive = true;
	pair<int, int> destination;
	Entity* closest = nullptr;

	float resting_time = 2.0f;
	Timer rest;

	int original_range;
};

#endif