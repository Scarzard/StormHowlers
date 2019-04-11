#ifndef __Townhall_H__
#define __Townhall_H__

#include "Building.h"

class Townhall : public Building
{
public:
	Townhall();
	Townhall(bool isPlayer1, pair<int, int> position);
	~Townhall();

	bool Update(float dt);
	void CleanUp();

};

#endif