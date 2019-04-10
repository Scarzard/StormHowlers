#ifndef __MainDefense_H__
#define __MainDefense_H__
#include "Entity.h"

class MainDefense : public Entity
{
public:
	MainDefense();
	MainDefense(bool isPlayer1, pair<int, int> position);
	~MainDefense();

	bool Update(float dt);
	void CleanUp();

	void LoadAnimations();
	void ChangeAnimation();

public:
	vector<uint> health_lv, upgrade_cost, damage_lv;

	Animation	level1;
	Animation	level2;
	Animation	level3;
	Animation	damaged;
	Animation	destroyed;
};

#endif