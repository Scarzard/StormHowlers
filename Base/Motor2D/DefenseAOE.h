#ifndef __DefenseAOE_H__
#define __DefenseAOE_H__
#include "Entity.h"
#include "Timer.h"

class DefenseAoe : public Entity
{
public:
	DefenseAoe();
	DefenseAoe(bool isPlayer1, pair<int, int> pos);
	~DefenseAoe();

	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	void CleanUp();

	bool Is_inRange(pair<int, int> pos, int & distance);


public:
	SDL_Rect	collider;
	SDL_Texture* tex;
};

#endif