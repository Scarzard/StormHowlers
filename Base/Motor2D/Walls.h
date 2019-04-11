#ifndef __Walls_H__
#define __Walls_H__

#include "Building.h"

class Walls : public Building
{
public:
	Walls();
	Walls(bool isPlayer1, pair<int, int> position);
	~Walls();

	bool Update(float dt);
	void CleanUp();

};

#endif