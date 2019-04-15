#ifndef __Walls_H__
#define __Walls_H__

#include "Building.h"
struct wall_parts
{

	pair <int, int> coordinates;
	int health;
	Animation* wall_anim;

};

class Walls : public Building
{
public:
	Walls();
	Walls(bool isPlayer1, pair<int, int> position);
	~Walls();

	bool LoadWalls();
	bool Update(float dt);
	void CleanUp();
	void LoadAnimations(bool isPlayer1, string path);

	
	
	Animation* tower;
	Animation* front;
	Animation* side;
	Animation* corner_down_left;
	Animation* corner_down_right;
	Animation* corner_up_left;
	Animation* corner_up_right;

};

#endif