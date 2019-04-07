#ifndef __CmdCenter_H__
#define __CmdCenter_H__
#include "Entity.h"

class CmdCenter : public Entity
{
public:
	CmdCenter();
	CmdCenter(bool isPlayer1, pair<int, int> position);
	~CmdCenter();

	bool Update(float dt);
	void CleanUp();

	void LoadAnimations();
	void ChangeAnimation();

public:
	vector<uint> health_lv, upgrade_cost;
	vector<uint> damage_missiles, damage_cast2, damage_cast3;

	Animation	level1;
	Animation	level2;
	Animation	level3;
	Animation	damaged;
	Animation	destroyed;
};

#endif