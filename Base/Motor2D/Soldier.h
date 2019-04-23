#ifndef __Soldier_H__
#define __Soldier_H__
#include "Troop.h"
#include "EntityManager.h"

class Soldier :	public Troop
{
public:
	Soldier();
	Soldier(bool isPlayer1, pair<int, int> pos, Collider collider);
	~Soldier();

	bool Update(float dt);
	bool Is_inRange(pair<int, int> pos, int & distance, pair<int, int> position, int range);
	void PrintState();
	void ForceAnimations();
	void ActOnDestroyed();
	void CleanUp();

	void ChangeAnimation(pair<int, int> Speed, Entity * closest);


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
