#ifndef __Soldier_H__
#define __Soldier_H__
#include "Troop.h"

class Soldier :	public Troop
{
public:
	Soldier();
	Soldier(bool isPlayer1, pair<int, int> pos, Collider collider);
	~Soldier();

	bool Update(float dt);
	void CleanUp();

	void ChangeAnimation();

	void LoadAnimations(bool isPlayer1, string path);
	vector<pair<int, int>> path;
	int path_count = 1;
	SDL_Rect rect = { 180,0,60,30 };

	bool isMoving = false;
	bool isShooting = false;
};

#endif
