#ifndef __Walls_H__
#define __Walls_H__
#include "Entity.h"

class Walls : public Entity
{
public:
	Walls();
	Walls(bool isPlayer1, pair<int, int> position);
	~Walls();

	bool Update(float dt);
	void CleanUp();

	void LoadAnimations();
	void ChangeAnimation();

public:
	Animation	idle;
	Animation	damaged;
	Animation	destroyed;
};

#endif