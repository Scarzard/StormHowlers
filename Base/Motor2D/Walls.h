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

	bool Start();
	bool Update(float dt);
	void CleanUp();
	void LoadAnimations(bool isPlayer1, string path);

	list<wall_parts*> wall_parts_list;
};

#endif