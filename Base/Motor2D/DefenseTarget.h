#ifndef __DefenseTarget_H__
#define __DefenseTarget_H__
#include "Entity.h"
#include "Timer.h"

class DefenseTarget : public Entity
{
public:
	DefenseTarget();
	DefenseTarget(bool isPlayer1, pair<int, int> position);
	~DefenseTarget();

	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	void CleanUp();

	void LoadAnimations();
	void ChangeAnimation();

	bool Is_inRange(pair<int, int> pos, int & distance);


public:
	vector<uint>  health_lv, upgrade_cost;
	vector<uint>  damage_missiles;
	int repair_cost;

	Animation	building;
	Animation	level1;
	Animation	level2;
	Animation	level3;
	Animation	damaged;
	Animation	destroyed;

	SDL_Rect	collider;
	SDL_Texture* tex;

	Timer		timer;
	float			rate_of_fire;
};

#endif