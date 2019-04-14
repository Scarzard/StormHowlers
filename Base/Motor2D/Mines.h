#ifndef __Mines_H__
#define __Mines_H__

#include "Building.h"

class Mines : public Building
{
public:
	Mines();
	Mines(bool isPlayer1, pair<int, int> position);
	~Mines();

	bool PreUpdate();
	bool Update(float dt);
	void CleanUp();
	void LoadAnimations(bool isPlayer1, string path);
};

#endif