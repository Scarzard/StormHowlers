#ifndef __DefenseAOE_H__
#define __DefenseAOE_H__

#include "Building.h"
#include "Timer.h"

class DefenseAoe : public Building
{
public:
	DefenseAoe();
	DefenseAoe(bool isPlayer1, pair<int, int> pos, Collider collider);
	~DefenseAoe();

	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	void CleanUp();

	bool Is_inRange(pair<int, int> pos, int & distance);
	void LoadAnimations(bool isPlayer1, string path);

public:

	Collider	colider;
	SDL_Texture* tex = nullptr;
};

#endif