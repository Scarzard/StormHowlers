#ifndef __Barracks_H__
#define __Barracks_H__
#include "Entity.h"

class Barracks : public Entity
{
public:
	Barracks();
	Barracks(bool isPlayer1, pair<int, int> position);
	~Barracks();

	bool PreUpdate();
	bool Update(float dt);
	void CleanUp();

	void LoadAnimations();
	void ChangeAnimation();

public:
	vector<uint> capacity_lv, health_lv, upgrade_cost;
	int repair_cost;

	Animation	building;
	Animation	level1;
	Animation	level2;
	Animation	level3;
	Animation	damaged;
	Animation	destroyed;
};

#endif