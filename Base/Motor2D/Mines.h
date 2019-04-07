#ifndef __Mines_H__
#define __Mines_H__
#include "Entity.h"

class Mines : public Entity
{
public:
	Mines();
	Mines(bool isPlayer1, pair<int, int> position);
	~Mines();

	bool PreUpdate();
	bool Update(float dt);
	void CleanUp();

	void LoadAnimations();
	void ChangeAnimation();

public:
	vector<uint> production_lv, health_lv, upgrade_cost;
	uint repair_cost;

	Animation	building;
	Animation	level1;
	Animation	level2;
	Animation	level3;
	Animation	damaged;
	Animation	destroyed;
};

#endif