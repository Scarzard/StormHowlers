#ifndef __Barracks_H__
#define __Barracks_H__
#include "Building.h"

class Barracks : public Building {
public:
	Barracks();
	Barracks(bool isPlayer1, pair<int, int> position, Collider collider);
	~Barracks();

	bool PreUpdate();
	bool Update(float dt);
	void DeployTroops();
	void CleanUp();

	void LoadAnimations(bool isPlayer1, string path);

	
};

#endif