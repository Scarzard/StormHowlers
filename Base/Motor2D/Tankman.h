#ifndef __Tankman_H__
#define __Tankman_H__

#include "Troop.h"


class Tankman : public Troop
{
public:
	Tankman();
	Tankman(bool isPlayer1, pair<int, int> pos, Collider collider);
	~Tankman();

	bool Update(float dt);
	void SetDestination();
	bool Is_inRange(pair<int, int> pos, int & distance, pair<int, int> position, int range);
	void ActOnDestroyed();
	void CleanUp();
	void MovementPathfind(Entity* target, pair <int, int> map_pos);
	void SimpleMovement();
	void ChangeAnimation(TroopDir facing, bool fromplayer1);

	Entity* FindEntityInAttackRange(pair<int, int> pos, bool fromplayer1, int attackrange, entityType type, int zone = 0);
	Entity* FindNearestEntity(pair<int, int> pos, bool fromplayer1, entityType type, int zone = 0);

	bool IsInAllyZone(pair <int, int > map_pos);
	bool IsInEnemyZone(pair <int, int > map_pos);
	bool IsInWarZone(pair <int, int > map_pos);

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