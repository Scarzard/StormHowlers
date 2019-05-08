#ifndef __Soldier_H__
#define __Soldier_H__

#include "Troop.h"
#include "EntityManager.h"

class Engineer : public Troop
{
public:
	Engineer();
	Engineer(bool isPlayer1, pair<int, int> pos, Collider collider);
	~Engineer();

	bool Update(float dt);
	void SetDestination();
	bool Is_inRange(pair<int, int> pos, int & distance, pair<int, int> position, int range);
	void PrintState();
	void ForceAnimations();
	void ActOnDestroyed();
	void CleanUp();

	void ChangeAnimation();



	void LoadAnimations(bool isPlayer1, string path);

	bool Is_inRange(pair<int, int> pos, int & distance);
	bool defensive = true;
	bool offensive = true;
	pair<int, int> destination;
	Entity* closest = nullptr;

	float resting_time = 2.0f;
	Timer rest;

	int original_range;
};

#endif
