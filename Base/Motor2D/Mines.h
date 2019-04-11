#ifndef __Mines_H__
#define __Mines_H__
#include "Entity.h"

class Mines : public Entity
{
public:
	Mines();
	Mines(bool isPlayer1, pair<int, int> position);
	~Mines();

	bool PreUpdate();
	bool Update(float dt);
	void CleanUp();

};

#endif