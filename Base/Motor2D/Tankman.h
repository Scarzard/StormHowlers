#ifndef __Tankman_H__
#define __Tankman_H__

#include "Troop.h"
#include "EntityManager.h"


class Tankman : public Troop
{
public:
	Tankman();
	Tankman(bool isPlayer1, pair<int, int> pos, Collider collider);
	~Tankman();

	bool Update(float dt);
	void SetDestination();
	bool Is_inRange(pair<int, int> pos, int & distance, pair<int, int> position, int range);
	void PrintState();
	void ForceAnimations();
	void ActOnDestroyed();
	void CleanUp();

	void ChangeAnimation();

	Building * FindBuilding(pair<int, int> pos, bool fromplayer1, int attackrange);

	void LoadAnimations(bool isPlayer1, string path);

	bool Is_inRange(pair<int, int> pos, int & distance);
	bool defensive = true;
	bool offensive = true;
	pair<int, int> destination;
	Building* closest = nullptr;

	float resting_time = 2.0f;
	Timer rest;

	int original_range;
};

#endif
