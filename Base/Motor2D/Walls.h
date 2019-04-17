#ifndef __Walls_H__
#define __Walls_H__

#include "Building.h"

class Walls : public Building
{
public:
	Walls();
	Walls(bool isPlayer1, pair<int, int> position, Collider collider, Animation* current_anim);
	~Walls();

	bool Update(float dt);
	void CleanUp();

public:
	Animation* tower;
	Animation* front;
	Animation* side;
	Animation* corner_down_left;
	Animation* corner_down_right;
	Animation* corner_up_left;
	Animation* corner_up_right;
};

#endif