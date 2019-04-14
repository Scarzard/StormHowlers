#ifndef __MainDefense_H__
#define __MainDefense_H__

#include "Building.h"

class MainDefense : public Building
{
public:
	MainDefense();
	MainDefense(bool isPlayer1, pair<int, int> position);
	~MainDefense();

	bool Update(float dt);
	void CleanUp();
	void LoadAnimations(bool isPlayer1, string path);
};

#endif