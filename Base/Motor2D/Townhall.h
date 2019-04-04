#ifndef __Townhall_H__
#define __Townhall_H__
#include "Entity.h"

class Townhall : public Entity
{
public:
	Townhall();
	Townhall(bool isPlayer1, pair<int, int> position);
	~Townhall();

	bool Update(float dt);
	void CleanUp();

	void LoadAnimations();
	void ChangeAnimation();

public:
	vector<uint> production_lv, health_lv, upgrade_cost;

	Animation	building;
	Animation	level1;
	Animation	level2;
	Animation	level3;
	Animation	damaged;
	Animation	destroyed;
};

#endif