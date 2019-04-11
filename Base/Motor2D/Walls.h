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

};

#endif