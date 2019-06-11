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
	Animation* tower = nullptr;
	Animation* front = nullptr;
	Animation* side = nullptr;
	Animation* corner_down_left = nullptr;
	Animation* corner_down_right = nullptr;
	Animation* corner_up_left = nullptr;
	Animation* corner_up_right = nullptr;
};

#endif